#include "can.h"
#include "main.h"
#include "can_wp.h"

//#include "cancmd.h"
#include "anniuled.h"
#include "timer.h"
#include "algorithm.h"
#include "can_dmp.h"
#include "boot.h"
#include <string.h>



uint32_t Flash_Device_ID_STM;
uint32 DeviceCanAddr = 0;



static RINGBUF canCmdRcvBuf;

static void cancmd_buf_init() {
    static CanRxMsg __msg[20];
    ringBufInit(&canCmdRcvBuf, __msg, sizeof(CanRxMsg), lenthof (__msg));
}

void candmpInit(void){
    cancmd_buf_init();
    canRegistRcvHandler(canRcvHandler);
}


void canRcvHandler(CanRxMsg *msg){
    if(IS_CANMSG_CMD(msg)){
	ringBufPush(&canCmdRcvBuf,msg);
    }
}


extern  uint8  canBOFs;

static void heartbeat(CAN_WP *wp){
  if(DeviceCanAddr==0){
     return;
  }
    uint32 esr = CAN->ESR;
    DEFINE_CAN_WP_FRAME(twp);
    twp.funcode = wp->funcode;
    twp.desid = wp->srcid;
    twp.dlc = 5;
    twp.data[0]= (uint8)esr;
    twp.data[1]= (uint8)(esr>>8);
    twp.data[2]= (uint8)(esr>>16);
    twp.data[3]= (uint8)(esr>>24);
    twp.data[4] = canBOFs;
    wpSend(&twp);
}


extern __weak void doCmdWp(CAN_WP *wp);

void ScanCmd(void) {
    CanRxMsg msg;
    if (ringBufPop(&canCmdRcvBuf, &msg)) {
        if (IS_CANMSG_WPCMD(&msg)) {
            CAN_WP wp;
            CANRxMSG_TO_CANWP(&wp, &msg);
            if(wp.funcode==CAN_WP_FUNCODE_HEARDBEAT){
                heartbeat(&wp);
            }else{
                doCmdWp(&wp);
            }
        } else if (IS_CANMSG_DMPCMD_RCV(&msg)) {
            CAN_DMP dmp;
            CANRxMSG_TO_CANDMP(&dmp, &msg);
            doCmdDmp(&dmp);
        }
    }
}


void dmpAppToIapReturn() {
    if (BOOT_FLAG_APPTOIAP == POWERUP_APPTOIAP) {
        DEFINE_CAN_DMP_FRAME(dmpt);
        dmpt.dlc = 2;
        dmpt.data[0] = CANCMD_GOTOIAP;
        dmpt.data[1] = 0;
        dmpSend(&dmpt);
    }
}



void dmpIapToAppReturn() {
    if (BOOT_FLAG_APPTOIAP == POWERUP_IAPTOAPP) {
        DEFINE_CAN_DMP_FRAME(dmpt);
        dmpt.dlc = 2;
        dmpt.data[0] = CANCMD_GOTOAPP;
        dmpt.data[1] = 0;
        dmpSend(&dmpt);
    }
}



void dmpGoToApp(void) {
    DEFINE_CAN_DMP_FRAME(dmpt);
    dmpt.dlc = 2;
    dmpt.data[0] = CANCMD_GOTOAPP;
    if (isAppAvailable()) {
        BOOT_FLAG_APPTOIAP = POWERUP_IAPTOAPP;
        bootApp();
    }
    return;
}



static uint8 eraseFlash(void) {
    uint32_t NbrOfPage, proaddr, re = 1;
    FLASH_Unlock();
    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR);
    unsigned long long *pAppAvailableFlag;
    pAppAvailableFlag = (unsigned long long *)APP_AVAILABLE_FLAG_ADDR;
    if (FLASH_ErasePage(APP_AVAILABLE_FLAG_ADDR) != FLASH_COMPLETE || *pAppAvailableFlag != -1) {
        return 0;
    }
    NbrOfPage = (APP_ADDREND - APP_ADDR) / FLASH_PAGE_SIZE;
    for (uint32_t i = 0; i < NbrOfPage; i++) {
        proaddr = FLASH_PAGE_SIZE;
        proaddr = proaddr * i;
        proaddr = proaddr + APP_ADDR;
        if (FLASH_ErasePage(proaddr) != FLASH_COMPLETE) {
            re = 0;
            break;
        }
    }
    return re;
}




uint32_t apppacknumber = 0;
bool static flashErased = 0;
bool static burnpackfinished = 0;


void dmpEraseFlash(CAN_DMP *dmp) {
    uint8 re;
    DEFINE_CAN_DMP_FRAME(dmpt);
    apppacknumber = dmp->data[1] | dmp->data[2] << 8 | dmp->data[3] << 16;
    FLASH_Unlock();
    re = eraseFlash();
    dmpt.dlc = 2;
    dmpt.data[0] = CANCMD_ERASEFLASH;
    dmpt.data[1] = re;
    dmpSend(&dmpt);
    flashErased = re;
    burnpackfinished = 0;
}


#define BOOT_BURN_APP_PACKERROR     -1
#define BOOT_BURN_APP_WRITE_TIMEOUT -2
#define BOOT_BURN_APP_CHECK_ERROR   -3
#define BOOT_BURN_APP_NOT_ERASE     -4


void dmpBurnFlash(CAN_DMP *dmp) {
    int8_t re;
    uint32_t addr, data;
    uint32_t ipack;
    static uint32_t nextpack = 0;
    DEFINE_CAN_DMP_FRAME(dmpt);
    ipack = dmp->data[1] | dmp->data[2] << 8 | dmp->data[3] << 16;
    if (dmp->dlc != 8) {
        re = BOOT_BURN_APP_PACKERROR;
        goto ERROR;
    }
    if (flashErased == 0) {
        re = BOOT_BURN_APP_NOT_ERASE;
        goto ERROR;

    }
    if (ipack != nextpack) {
        re = BOOT_BURN_APP_PACKERROR;
        goto ERROR;
    } else {
        nextpack++;
    }
    addr = ipack * 4 + APP_ADDR;
    data = dmp->data[4] | dmp->data[5] << 8 | dmp->data[6] << 16 | dmp->data[7] << 24;
    if (addr == APP_AVAILABLE_FLAG_ADDR) {
        if (data != (uint32_t)APP_OK) {
            re = BOOT_BURN_APP_PACKERROR;
            goto ERROR;
        }
    } else if (addr == APP_AVAILABLE_FLAG_ADDR + 4) {
        if (data != (uint32_t)(APP_OK >> 32)) {
            re = BOOT_BURN_APP_PACKERROR;
            goto ERROR;
        }
    } else {
        if (FLASH_ProgramWord(addr, data) != FLASH_COMPLETE) {
            re = BOOT_BURN_APP_WRITE_TIMEOUT;
            goto ERROR;
        }
    }
    dmpt.dlc = 5;
    dmpt.data[0] = CANCMD_BURNAPP;
    dmpt.data[1] = (uint8_t)(ipack);
    dmpt.data[2] = (uint8_t)(ipack >> 8);
    dmpt.data[3] = (uint8_t)(ipack >> 16);
    dmpt.data[4] = (uint8_t)(ipack >> 24);
    dmpSend(&dmpt);

    if (ipack == apppacknumber - 1) {
        //todo check
        //todo write check;
        burnpackfinished = 1;
        if (FLASH_ProgramWord(APP_AVAILABLE_FLAG_ADDR, (uint32_t)APP_OK) != FLASH_COMPLETE ||
            FLASH_ProgramWord(APP_AVAILABLE_FLAG_ADDR+4, (uint32_t)(APP_OK>>32)) != FLASH_COMPLETE) {
            re = BOOT_BURN_APP_PACKERROR;
            goto ERROR;
        }
        FLASH_Lock();
    }
    return;
    ERROR:
    dmpt.dlc = 5;
    dmpt.data[0] = CANCMD_BURNAPP;
    dmpt.data[1] = (uint8_t)(re);
    dmpt.data[2] = (uint8_t)(re >> 8);
    dmpt.data[3] = (uint8_t)(re >> 16);
    dmpt.data[4] = (uint8_t)(re >> 24);
    dmpSend(&dmpt);
    FLASH_Lock();
}



void dmpGoToIap(CAN_DMP *dmp) {
    if (dmp->dlc != 1) {
        return;
    }
    BOOT_FLAG_APPTOIAP = POWERUP_APPTOIAP;
    //DEFINE_CAN_DMP_FRAME(dmpt);
    //dmpt.dlc = 1;
    ///dmpt.data[0] = CANCMD_GOTOIAP;
    //dmpSend(&dmpt);
    bootAppToIap();
}


#define Ver_No_Send  8  //  "SoftVer-" should not send

void dmpReadSoftVer(CAN_DMP *dmp) {
    uint16_t i;
    uint8_t j;
    uint8_t length;
    uint8_t num;
    DEFINE_CAN_DMP_FRAME(dmpt);
    dmpt.data[0] = CANCMD_READSOFTVER;

    length = strlen(Ver) + 1;
    length = length + 1 - Ver_No_Send + 1;      //实际要发送的数据长度  每包实际数据为6个
    num = DIVUP(length, 6);
    for (j = 0; j < num; j++) {
        dmpt.data[1] = ((num & 0xf) << 4) + j;
        if (j == (num - 1)) {
            //最高一包数据
            dmpt.dlc = (length % 6) + 2;
            dmpt.data[dmpt.dlc - 1] = DeviceCanAddr;
            for (i = 2; i < (dmpt.dlc - 1); i++) {
                dmpt.data[i] = Ver[8 + j * 6 + i - 2];
            }
        } else {
            dmpt.dlc = 8;
            for (i = 2; i < 8; i++) {
                dmpt.data[i] = Ver[8 + (i - 2) + j * 6];
            }
        }
        dmpSend(&dmpt);
    }
}


#define CAN_FILTER_IDX_DATA    0X04
#define CAN_MASK_IDX_DATA      0X06

#define Work_global_broadcast 			      (0x3ff<<18 | 1<<28)
#define Work_Local_broadcast 			      ((DMP_DEV_TYPE<<6 | 0x3f)<<18 | 1<<28)


uint8_t dmpSetAddr(CAN_DMP *dmp) {
    if ((dmp->uid != Flash_Device_ID_STM) || (dmp->dlc != 3)) {
        return 0;
    }

    uint32 addr;
    addr = dmp->data[2] << 8 | dmp->data[1];
    if ((addr != 0) && (IS_CAN_WP_ID_GROUP_BROADCAST(addr) ||
                        (CAN_WP_GET_TYPE(addr) != DMP_DEV_TYPE))) {
        return 0;
    }
    if (addr != 0) {
        DeviceCanAddr = addr;

        unsigned int mask, filter;

        //work protocal globle broadcast
        mask = ((0x3ff << 18 | 1 << 28) << 3) | CAN_MASK_IDX_DATA;
        filter = (Work_global_broadcast << 3) | CAN_FILTER_IDX_DATA;
        canAddfilter(filter, mask, 2);

        //work protocal local broadcast
        mask = (0x3ff << 18 | 1 << 28) << 3 | CAN_MASK_IDX_DATA;
        filter = (Work_Local_broadcast << 3) | CAN_FILTER_IDX_DATA;
        canAddfilter(filter, mask, 3);

        mask = (0x3ff << 18 | 1 << 28) << 3 | CAN_MASK_IDX_DATA;
        filter = (CAN_WP_ID(DMP_DEV_TYPE, addr) << 18 | 1 << 28) << 3
                 | CAN_FILTER_IDX_DATA;
        canAddfilter(filter, mask, 4);

        ledRunSetState(LED_STAT_RUN);
    } else {
        DeviceCanAddr = 0;
        ledRunSetState(LED_STAT_NOTREGEST);
        canClearfilter(1 << 2 | 1 << 3 | 1 << 4);
    }
    DEFINE_CAN_DMP_FRAME(dmpt);
    dmpt.dlc = 3;
    dmpt.data[0] = CANCMD_SETADRR;
    dmpt.data[1] = (uint8)DeviceCanAddr;
    dmpt.data[2] = (uint8)(DeviceCanAddr >> 8);
    dmpSend(&dmpt);
    return 1;
}


void dmpWaitSetAdd(CAN_DMP *dmp) {
    if ((dmp->uid != Flash_Device_ID_STM) || dmp->dlc != 2) {
        return;
    }
    if (dmp->data[1] == 1) {
        ledRunSetState(LED_STAT_WAITADRR);            //让LED灯进入等待配对模式 LED闪加快
    } else {
        if ((DeviceCanAddr != 0) && (DeviceCanAddr != 0xff)) {
            ledRunSetState(LED_STAT_RUN);
        } else {
            ledRunSetState(LED_STAT_NOTREGEST);
        }
    }
    DEFINE_CAN_DMP_FRAME(dmpt);
    dmpt.dlc = 1;
    dmpt.data[0] = CANCMD_WAITSETADD;
    dmpSend(&dmpt);
}




void dmpReadWpId(CAN_DMP *dmp) {
    DEFINE_CAN_DMP_FRAME(dmpt);
    dmpt.dlc = 3;
    dmpt.data[0] = CANCMD_CHECKID;
    dmpt.data[1] = (uint8)DeviceCanAddr;
    dmpt.data[2] = (uint8)(DeviceCanAddr >> 8);
    dmpSend(&dmpt);
}


void dmpReadUid(CAN_DMP *dmp){
    DEFINE_CAN_DMP_FRAME(dmpt);
    dmpt.dlc = 5;
    dmpt.data[0] = CANCMD_READUID;
    dmpt.data[1] = (uint8)Flash_Device_ID_STM;
    dmpt.data[2] = (uint8)(Flash_Device_ID_STM >> 8);
    dmpt.data[3] = (uint8)(Flash_Device_ID_STM >> 16);
    dmpt.data[4] = (uint8)(Flash_Device_ID_STM >> 24);
    dmpSend(&dmpt);
}


void doCmdDmp(CAN_DMP *dmp) {
    switch (dmp->data[0]) {
    case CANCMD_READSOFTVER:
        dmpReadSoftVer(dmp);
        break;
#if defined(APP)
    case CANCMD_WAITSETADD:
        dmpWaitSetAdd(dmp);
        break;
    case CANCMD_CHECKID:
        dmpReadWpId(dmp);
        break;
    case CANCMD_READUID:
        dmpReadUid(dmp);
        break;
    case CANCMD_SETADRR:
        dmpSetAddr(dmp);
        break;
    case CANCMD_GOTOIAP:
        dmpGoToIap(dmp);
        break;
#elif  defined(BOOT)
    case CANCMD_ERASEFLASH :
        dmpEraseFlash(dmp);
        break;
    case CANCMD_BURNAPP:
        dmpBurnFlash(dmp);
        break;
    case CANCMD_GOTOAPP:
        dmpGoToApp();
        break;
#else
#error  "must define BOOT or APP"
#endif
    default:
        break;
    }
}


uint8_t dmpSend(CAN_DMP *dmp) {
    CanTxMsg msg;
    msg.ExtId = dmp->uid | 1 << 27 | 0 << 28;
    msg.RTR = CAN_RTR_DATA;
    msg.IDE = CAN_ID_EXT;
    msg.DLC = dmp->dlc;
    memcpy(msg.Data, dmp->data, lenthof(msg.Data));
    return CanSend(&msg);
}




