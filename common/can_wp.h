
#ifndef __CAN__WP__H__
#define __CAN__WP__H__

#include <stdbool.h>
#include <string.h>
#include "can.h"
#include "algorithm.h"



#define CAN_WP_FUNCODE_HEARDBEAT   0xfe



typedef struct {
    unsigned int funcode:8;
    unsigned int srcid:10;
    unsigned int desid:10;
    unsigned int flag:1;
    unsigned int candir:1;
    unsigned int xtd:1;
    unsigned int dlc;
    unsigned char data[8];
}CAN_WP;



#define CAN_WP_GET_TYPE(WPID) ((WPID)>>6)
#define CAN_WP_GET_ID(WPID) ((WPID)&0x3f)
#define CAN_WP_ID(TYPE,id) ((TYPE)<<6 | (id))
#define CAN_WP_ID_GROUP_BROADCAST(type) CAN_WP_ID(type,0x3f)
#define CAN_WP_ID_ALL_BROADCAST 0x3ff

#define IS_CAN_WP_ID_GROUP_BROADCAST(ID) (CAN_WP_GET_ID(ID)==0x3f)
#define IS_CAN_WP_ID_ALL_BROADCAST(ID) ((ID)==CAN_WP_ID_ALL_BROADCAST)



extern uint32 DeviceCanAddr;

static inline void __canwpinit(CAN_WP *frame){
    frame->srcid = DeviceCanAddr;
}

#define DEFINE_CAN_WP_FRAME(frame) CAN_WP frame =  \
              {.flag = 1,.candir = 0,.xtd=1,};\
              __canwpinit(&frame)


static inline void CANWP_TO_CANTxMSG(CanTxMsg *msg, CAN_WP *cmd){

    msg->DLC = cmd->dlc;
    msg->ExtId = *(unsigned int *)cmd &0x1fffffff;
    msg->RTR = CAN_RTR_DATA;
    msg->IDE = CAN_ID_EXT;
    msg->DLC = cmd->dlc;
    memcpy(msg->Data,cmd->data,lenthof(msg->Data));
}


static inline void CANRxMSG_TO_CANWP(CAN_WP *cmd,CanRxMsg *msg){
    cmd->funcode = msg->ExtId & 0xff;
    cmd->srcid = msg->ExtId>>8 & 0x3ff;
    cmd->desid = msg->ExtId>>18 & 0x3ff;
    cmd->dlc = msg->DLC;
    memcpy(cmd->data,msg->Data,lenthof(msg->Data));
}

static inline uint8_t wpSend(CAN_WP *wp){
    if (DeviceCanAddr==0) {
        return 0;
    }
    CanTxMsg msg;
    msg.DLC = wp->dlc;
    msg.ExtId = *(unsigned int *)wp & 0x1fffffff;
    msg.RTR = CAN_RTR_DATA;
    msg.IDE = CAN_ID_EXT;
    msg.DLC = wp->dlc;
    memcpy(msg.Data,wp->data,lenthof(msg.Data));
    return CanSend(&msg);
}



#endif /*__CAN__WP__H__*/
