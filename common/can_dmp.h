#ifndef __CAN__DMP__H
#define __CAN__DMP__H

#include "string.h"
#include "algorithm.h"


#define CAN_WP_DEV_TYPE_MAIN    0x01UL
#define CAN_WP_DEV_TYPE_MOTOR   0x02UL
#define CAN_WP_DEV_TYPE_INPUT   0x03UL
#define CAN_WP_DEV_TYPE_QIFA    0x04UL
#define CAN_WP_DEV_TYPE_SIFU    0x05UL
#define CAN_WP_DEV_TYPE_CANHUB  0X06UL


/**************config by user */
#if  defined(DMP_TYPE_INPUT)
#define DMP_DEV_TYPE   CAN_WP_DEV_TYPE_INPUT
#elif defined(DMP_TYPE_OUTPUT)
#define DMP_DEV_TYPE   CAN_WP_DEV_TYPE_QIFA
#elif defined(DMP_TYPE_MOTOR)
#define DMP_DEV_TYPE   CAN_WP_DEV_TYPE_MOTOR
#elif defined(DMP_TYPE_CANHUB)
#define DMP_DEV_TYPE   CAN_WP_DEV_TYPE_CANHUB
#else
#error "must define DMP_DEV_XXX"
#endif
/***************end************/

#define CAN_DMP_BROADCAST_ID                    0X7FFFFFF

#define CANCMD_READSOFTVER       		0x01
#define CANCMD_CHECKID				0X02
#define CANCMD_READUID                          0X03
#define CANCMD_BURNAPP                          0XF7
#define CANCMD_ERASEFLASH                       0XF8
#define CANCMD_SETADRR       			0XFD
#define CANCMD_GOTOAPP       			0xFC
#define CANCMD_GOTOIAP      	 		0xFB
#define CANCMD_WAITSETADD			0XFA


typedef struct {
    unsigned int uid:27;
    unsigned int dir:1;
    unsigned int flag:1;
    unsigned int candir:1;
    unsigned int xtd:1;
    unsigned int dlc;
    unsigned char data[8];
}CAN_DMP;


extern uint32_t Flash_Device_ID_STM ;
static inline void __candmpinit(CAN_DMP *frame){
    frame->uid = Flash_Device_ID_STM;
}
#define DEFINE_CAN_DMP_FRAME(frame) CAN_DMP frame =  \
              {.flag = 0,.candir = 0,.xtd=1,};\
              __candmpinit(&frame)




static inline void CANRxMSG_TO_CANDMP(CAN_DMP *cmd,CanRxMsg *msg){
    cmd->uid = msg->ExtId & 0x7ffffff;
    cmd->flag = 0;
    cmd->dir = 0;
    cmd->xtd = 1;
    cmd->dlc = msg->DLC;
    memcpy(cmd->data,msg->Data,lenthof(msg->Data));
}


static inline bool IS_CANMSG_CMD(CanRxMsg *msg){
    if (msg->IDE == CAN_Id_Extended && msg->RTR == CAN_RTR_DATA) {
        return true;
    }
    return false;
}


static inline bool IS_CANMSG_DMPCMD_RCV(CanRxMsg *msg){
    return !(msg->ExtId & 0x03<<28) && (msg->DLC!=0);
}


static inline bool IS_CANMSG_WPCMD(CanRxMsg *msg){
    return !!(msg->ExtId & 1<<28);
}






//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
extern const char Ver[];

extern void candmpInit(void);
extern void canRcvHandler(CanRxMsg *msg);
extern void ScanCmd(void);
extern void dmpGoToIap(CAN_DMP *dmp);
extern uint8_t dmpSetAddr(CAN_DMP *dmp);
extern void dmpWaitSetAdd(CAN_DMP *dmp);
extern void dmpReadWpId(CAN_DMP *dmp);
extern void dmpReadSoftVer(CAN_DMP *dmp);
extern void doCmdDmp(CAN_DMP *dmp);
extern uint8_t dmpSend(CAN_DMP *dmp);
extern void dmpAppToIapReturn();
extern void dmpIapToAppReturn();

#endif