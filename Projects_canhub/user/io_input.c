/*******************************************************************************
IO_INPUT.C
********************************************************************************/
#include "io_input.h"
#include "cancmd.h"
#include "main.h"
#include "can.h"
#include "timer.h"
#include "anniuled.h"
#include "can_wp.h"


IO_SCAN io_scan[Max_Input_Single];


const GPIOGROUP_IoInput Input_Single[Max_Input_Single] = {
    { Input0_PORT, Input0_PIN },
    { Input1_PORT, Input1_PIN},
    { Input2_PORT, Input2_PIN},
    { Input3_PORT, Input3_PIN},
    { Input4_PORT, Input4_PIN},
    { Input5_PORT, Input5_PIN},
    { Input6_PORT, Input6_PIN},
    { Input7_PORT, Input7_PIN},
};
//IO inint
void Io_input_Init(void) {
    uint8_t i;
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB, ENABLE);
    for (i = 0; i < lenthof(Input_Single); i++) {
        GPIO_InitStructure.GPIO_Pin = Input_Single[i].bit;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
        GPIO_Init(Input_Single[i].port, &GPIO_InitStructure);
    }
    for (int i = 0; i < lenthof(io_scan); i++) {
        io_scan[i].en = 0;
    }
    io_bitmap = Scan_Input_state();
}






//»ñµÃ¹Ü½ÅIO ×´Ì¬
uint8_t Get_Io_Input(uint8_t num) {
    return(GPIO_ReadInputDataBit(Input_Single[num].port, Input_Single[num].bit));
}




unsigned int io_alarm_enable_bitmap = 0;


void ioAlarmProcess(void) {
    unsigned int re = 0;
    if (DeviceCanAddr==0) {
        return;
    }
    unsigned long long time = getTimerTick();
    for (int i = 0; i < lenthof(io_scan); i++) {
        if ((io_scan[i].en != IO_SCAN_STAT_EN_ALRAM) || !(io_alarm_enable_bitmap & 1 << i)) {
            continue;
        }
        if (io_scan[i].timelast != 0xffff) {
            switch (io_scan[i].stat) {
            case IO_SCAN_STAT_NO:
                if (time >= io_scan[i].begintime && time <= io_scan[i].begintime + io_scan[i].timelast) {
                    if (Get_Io_Input(i) == io_scan[i].val) {
                        io_scan[i].debouceCnt = 0;
                        io_scan[i].stat = IO_SCAN_STAT_DEBOUNCE;
                    } else {
                        io_scan[i].stat = IO_SCAN_STAT_NO;
                    }
                } else if (time > io_scan[i].begintime + io_scan[i].timelast) {
                    io_scan[i].stat = IO_SCAN_STAT_ALARM;
                } else {
                    io_scan[i].stat = IO_SCAN_STAT_NO;
                }
                break;
            case IO_SCAN_STAT_DEBOUNCE:
                if (time >= io_scan[i].begintime && time <= io_scan[i].begintime + io_scan[i].timelast) {
                    if (Get_Io_Input(i) == io_scan[i].val) {
                        if (io_scan[i].debouceCnt++ >= io_scan[i].sensitivity) {
                            io_scan[i].stat = IO_SCAN_STAT_HOLD;
                        }
                    } else {
                        io_scan[i].stat = IO_SCAN_STAT_NO;
                        io_scan[i].debouceCnt = 0;
                    }
                } else if (time > io_scan[i].begintime + io_scan[i].timelast) {
                    io_scan[i].stat = IO_SCAN_STAT_ALARM;
                } else {
                    io_scan[i].stat = IO_SCAN_STAT_NO;
                }
                break;
            case IO_SCAN_STAT_HOLD:
                io_scan[i].en = 0;
                io_scan[i].stat = IO_SCAN_STAT_NO;
                io_scan[i].debouceCnt = 0;
                break;
            case IO_SCAN_STAT_ALARM:
                io_scan[i].en = 0;
                io_scan[i].stat = IO_SCAN_STAT_NO;
                io_scan[i].debouceCnt = 0;
                re |= 1 << i;
                break;
            default:
                break;
            }
        } else {
            switch (io_scan[i].stat) {
            case IO_SCAN_STAT_OK:
                if (Get_Io_Input(i) != io_scan[i].val) {
                    io_scan[i].debouceCnt = 0;
                    io_scan[i].stat = IO_SCAN_STAT_DEBOUNCE;
                }
                break;
            case IO_SCAN_STAT_DEBOUNCE:
                if (Get_Io_Input(i) != io_scan[i].val) {
                    if (io_scan[i].debouceCnt++ >= io_scan[i].sensitivity) {
                        io_scan[i].stat = IO_SCAN_STAT_ERR;
                        io_scan[i].timertick = timerTick1ms;
                        io_scan[i].debouceCnt = 0;
                        re |= 1 << i;
                    }
                } else {
                    io_scan[i].stat = IO_SCAN_STAT_OK;
                }
                break;
            case IO_SCAN_STAT_ERR:
                if (Get_Io_Input(i) == io_scan[i].val &&
                    (timerTick1ms - io_scan[i].timertick) > io_scan[i].delay ) {
                    io_scan[i].stat = IO_SCAN_STAT_OK;
                }
                break;
            default:
                break;
            }
        }
    }
    DEFINE_CAN_WP_FRAME(wpt);
    //wpt.srcid = DeviceCanAddr;
    wpt.desid = CAN_WP_ID_GROUP_BROADCAST(CAN_WP_DEV_TYPE_MAIN);
    wpt.dlc =0;
    unsigned int icandat = 0;
    for (int i = 0; i < Max_Input_Single; i++) {
        if (re & 1 << i) {
            wpt.data[icandat] = i;
            wpt.data[icandat + 1] = !io_scan[i].val;
            wpt.dlc += 2;
            icandat += 2;
            if (icandat == 8) {
                wpSend(&wpt);
                wpt.dlc = 0;
                icandat = 0;
            }
        }
    }
    if (icandat != 0) {
        wpSend(&wpt);
    }
}



/*void ioTesttimeProcess(void) {
    unsigned long long time = getTimerTick();
    for (int i = 0; i < lenthof(io_scan); i++) {
        if (io_scan[i].en != IO_SCAN_STAT_EN_TEST) {
            continue;
        }
        switch (io_scan[i].stat) {
        case IO_SCAN_STAT_NO:
            if (time <= io_scan[i].begintime + io_scan[i].timelast) {
                if (Get_Io_Input(i) == io_scan[i].val) {
                    io_scan[i].stat = IO_SCAN_STAT_TEST_BEGIN;
                }
            } else if (time > io_scan[i].begintime + io_scan[i].timelast) { //timeout
                io_scan[i].en = 0;
                io_scan[i].debouceCnt = 0;
            }
            break;
        case IO_SCAN_STAT_TEST_BEGIN:
            if (time > io_scan[i].begintime + io_scan[i].timelast) { //timeout,end test
                io_scan[i].debouceCnt = 0;
                io_scan[i].stat = IO_SCAN_STAT_NO;
                io_scan[i].en = 0;
            } else {
                if (Get_Io_Input(i) == io_scan[i].val) {
                    io_scan[i].debouceCnt++;
                    if (io_scan[i].debouceCnt == IO_SCAN_DEBOUNCE_TIMES) {
                        io_scan[i].timelast = time - io_scan[i].begintime;
                        io_scan[i].stat = IO_SCAN_STAT_TEST_FINISH;
                    }
                } else {
                    io_scan[i].debouceCnt = 0;
                }
            }
            break;
        case IO_SCAN_STAT_TEST_FINISH:
            {
                DEFINE_CAN_WP_FRAME (wpt);
                wpt.desid = CAN_WP_ID_GROUP_BROADCAST(CAN_WP_DEV_TYPE_MAIN);
                wpt.funcode = CANCMD_TEST_TIME;
                wpt.dlc = 4;
                wpt.data[0] = i;
                wpt.data[1] = io_scan[i].val;
                wpt.data[2] = (unsigned char)io_scan[i].timelast;
                wpt.data[3] = (unsigned char)(io_scan[i].timelast >> 8);
                wpSend(&wpt);
                io_scan[i].stat = IO_SCAN_STAT_NO;
                io_scan[i].en = 0;
                break;
            }
        default:
            break;
        }
    }
} */



unsigned int Scan_Input_state(void) {
    uint8_t i;
    uint32_t databuf = 0;
    for (i = 0; i < lenthof(Input_Single); i++) {
        databuf |= (Get_Io_Input(i) << i);
    }
    return databuf;
}


#define everydiffdo(TYPE,MARK,val)\
            static TYPE __old ## MARK = 0;\
            for (TYPE MARK = val; MARK!=__old ## MARK;\
              __old ## MARK=MARK)



#define IO_SCAN_TIMES     6

unsigned int io_bitmap = 0;

void ioScan() {
    static IO_SCAN_SCAN io_scan_scan[Max_Input_Single];
    uint8_t send = 0;
    uint8_t data = 0;
    everydiffdo(uint32_t, __io_scan_timetick, timerTick05ms) {
        for (int i = 0; i < lenthof(Input_Single); i++) {
            data = Get_Io_Input(i);
            switch (io_scan_scan[i].state) {
            case IO_SCAN_STAT_NO:
                if (data != io_scan_scan[i].val) {
                    io_scan_scan[i].val = data;
                    io_scan_scan[i].state = IO_SCAN_STAT_DEBOUNCE;
                    io_scan_scan[i].count = 0;
                }
                break;
            case IO_SCAN_STAT_DEBOUNCE:
                if (data != io_scan_scan[i].val) {
                    io_scan_scan[i].state = IO_SCAN_STAT_NO;
                    io_scan_scan[i].val = data;
                } else {
                    io_scan_scan[i].count++;
                    if (io_scan_scan[i].count >= IO_SCAN_TIMES) {
                        io_scan_scan[i].state = IO_SCAN_STAT_HOLD;
                        io_scan_scan[i].valhold = io_scan_scan[i].val;
                        send = 1;
                    }
                }
                break;
            case IO_SCAN_STAT_HOLD:
                if (data != io_scan_scan[i].val) {
                    io_scan_scan[i].state = IO_SCAN_STAT_NO;
                }
                break;
            }
        }
        if (send == 1) {
            unsigned int io_bitmap_temp = 0;
            for (int i = 0; i < Max_Input_Single; i++) {
                io_bitmap_temp |= !!io_scan_scan[i].valhold << i;
            }
            io_bitmap = io_bitmap_temp & ((1 << Max_Input_Single) - 1);

            DEFINE_CAN_WP_FRAME(twp);
            twp.funcode = CANCMD_IO_UPDATA;
            twp.desid = CAN_WP_ID_GROUP_BROADCAST(CAN_WP_DEV_TYPE_MAIN);
            twp.dlc = 4;
            *(unsigned int *)&twp.data = io_bitmap;
            wpSend(&twp);
        }
    }
}


void io_scan_init() {
    for (int i = 0; i < lenthof(io_scan); i++) {
        io_scan[i].en = 0;
    }
}


