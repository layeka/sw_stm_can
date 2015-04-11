//#include "step.h"
#include "can.h"
#include "main.h"

#include "cancmd.h"
#include "anniuled.h"
#include "timer.h"
#include "algorithm.h"
#include "can_dmp.h"
#include "can_wp.h"




void ScanCmd(void) {
    CanRxMsg msg;
    if (ringBufPop(&canCmdRcvBuf, &msg)) {
        if(IS_CANMSG_DMPCMD_RCV(&msg)){
            CAN_DMP dmp;
            CANRxMSG_TO_CANDMP(&dmp,&msg);
            doCmdDmp(&dmp);
        }
    }
}


