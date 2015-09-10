//#include "step.h"
#include "can.h"
#include "main.h"
#include "io_input.h"
#include "output.h"
#include "temperature.h"

#include "cancmd.h"
#include "anniuled.h"
#include "timer.h"
#include "algorithm.h"
#include "can_dmp.h"
#include "can_wp.h"
#include "lcdbl.h"






uint8_t CanCmd_setAlarm(CAN_WP *wp) {
    if (wp->dlc != 8) {
        return 0;
    }
    CAN_CMD_SET_ALARM *cmd = (CAN_CMD_SET_ALARM *)wp->data;
    unsigned int bit = cmd->bit;
    if (bit >= Max_Input_Single) {
        return 0;
    }
    if (cmd->en == 0) {
        io_scan[bit].en = 0;
        io_scan[bit].stat = IO_SCAN_STAT_NO;
    } else {
        if (cmd->time != 0xffff) {
            io_scan[bit].en = cmd->en ? IO_SCAN_STAT_EN_ALRAM : 0;
            io_scan[bit].val = !!cmd->val;
            io_scan[bit].sensitivity = 3000 - MIN(cmd->sensitivity *30,3000);
            io_scan[bit].begintime = getTimerTick() + cmd->time / 2;
            io_scan[bit].timelast = cmd->time / 2;
            io_scan[bit].delay = cmd->delay;
            io_scan[bit].stat = IO_SCAN_STAT_NO;

        } else {
            io_scan[bit].en = cmd->en ? IO_SCAN_STAT_EN_ALRAM : 0;
            io_scan[bit].val = !!cmd->val;
            io_scan[bit].sensitivity  = 3000 - MIN(cmd->sensitivity * 30,3000);
            io_scan[bit].begintime = getTimerTick() - 1;
            io_scan[bit].timelast = cmd->time;
            io_scan[bit].delay = cmd->delay;
            io_scan[bit].stat = IO_SCAN_STAT_OK;
        }
    }
    return 0;
}



/*void CANCMD_test_time(CAN_WP *wp) {
    unsigned cmdnum = wp->dlc;
    if (cmdnum % sizeof(CAN_CMD_TEST_TIME) != 0) {
        return;
    }
    cmdnum = cmdnum / sizeof(CAN_CMD_TEST_TIME);
    for (int i = 0; i < cmdnum; i++) {
        CAN_CMD_TEST_TIME *cmd = (CAN_CMD_TEST_TIME *)(wp->data) + i;
        unsigned int bit = cmd->bit;
        if (bit > Max_Input_Single) {
            break;
        }
        if (io_scan[bit].en == 0) {
            io_scan[bit].en = IO_SCAN_STAT_EN_TEST;
            io_scan[bit].val = !!cmd->val;
            io_scan[bit].timelast = cmd->timeout;
            io_scan[bit].stat = IO_SCAN_STAT_NO;
            io_scan[bit].debouceCnt = 0;
            io_scan[bit].begintime = getTimerTick();
        }
    }
    return;
} */

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
uint8_t CANCMD_readstate(CAN_WP *wp) {
    DEFINE_CAN_WP_FRAME(twp);
    //twp.srcid = DeviceCanAddr;
    twp.funcode = CANCMD_READ_STATE;
    twp.desid = wp->srcid;
    twp.dlc = 2;
    twp.data[0] = (uint8)io_bitmap;
    twp.data[1] = (uint8)outputval;
    if (wpSend(&twp)) {
        return 1;
    }
    return 0;
}




void CANCMD_set_alarm_enable(CAN_WP *wp) {
    if (wp->dlc != 4) {
        return;
    }
    io_alarm_enable_bitmap = *(unsigned int *)&wp->data & INPUT_MASK;

    DEFINE_CAN_WP_FRAME(twp);
    twp.funcode = CANCMD_SET_ALARM_ENABLE;
    twp.desid = wp->srcid;
    twp.dlc = 4;
    *(unsigned int *)&twp.data = io_alarm_enable_bitmap;
    wpSend(&twp);
}


void   CANCMD_read_temp(CAN_WP *wp){
    if (wp->dlc != 0) {
        return;
    }
    DEFINE_CAN_WP_FRAME(twp);
    twp.funcode = CANCMD_READ_TEMP;
    twp.desid = wp->srcid;
    twp.dlc = 2;
    uint16 temp =  temperature*10;
    twp.data[0] = (uint8)temp;
    twp.data[1] = (uint8)(temp>>8);
    wpSend(&twp);
}


void  CANCMD_output(CAN_WP *wp){
    uint16_t value;
    int32 iout;
    if(IS_CAN_WP_ID_GROUP_BROADCAST(wp->desid)){
       return;
    }
    for (uint32 i = 0; i < wp->dlc; i++) {
        value = (wp->data[i] & 0x80) ? 1 : 0;
        iout = wp->data[i] & 0x7f;
        if (iout < Max_Output_Single-1) { //exclude lamp
            outputDrive(iout, value);
        }
    }
}


void  CANCMD_ledout(CAN_WP *wp){
   if (wp->dlc != 1) {
        return;
    }
   LcdBlCtr(wp->data[0]);
}


void doCmdWp(CAN_WP *wp) {
    if (CAN_WP_GET_ID(wp->desid) == 0) {
        return;
    }
    if (DeviceCanAddr == 0) {
        return;
    }
    switch (wp->funcode) {
    case CANCMD_SETALARM:
        CanCmd_setAlarm(wp);
        break;
    case CANCMD_READ_STATE:
        CANCMD_readstate(wp);
        break;
    case CANCMD_SET_ALARM_ENABLE:
        CANCMD_set_alarm_enable(wp);
        break;
    case CANCMD_READ_TEMP:
        CANCMD_read_temp(wp);
        break;
    case CANCMD_OUTPUT:
        CANCMD_output(wp);
        break;
    case CANCMD_LEDOUT:
        CANCMD_ledout(wp) ;
        break;
    default:
        break;
    }
}




