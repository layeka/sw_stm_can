
#ifndef __CANCMD_H__
#define __CANCMD_H__
#include "stm32f0xx_can.h"
#include "algorithm.h"
#include <string.h>
#include "main.h"




#define CANCMD_REC_DATA_IN  	    0X10
#define CANCMD_SEND_IO_STATE_UP 	0X11
#define CANCMD_SETALARM 			0x10
#define CANCMD_TEST_TIME 	        0x11
#define CANCMD_READ_VALUE           0x12
#define CANCMD_SET_ALARM_ENABLE     0x13
#define CANCMD_IO_UPDATA            0x14






typedef struct __can_cmd_set_alarm{
    unsigned char bit:7;
    unsigned char en:1;
    unsigned char val;
    unsigned short time;
}CAN_CMD_SET_ALARM;


typedef struct __can_cmd_alarm{
    unsigned char bit;
    unsigned char val;
}CAN_CMD_ALARM;


typedef struct __can_cmd_test_time{
    unsigned char bit;
    unsigned char val;
    unsigned short timeout;
}CAN_CMD_TEST_TIME;



extern uint8_t CanCmd_SendAlarm(void);

extern void  ScanCmd(void);

extern void DoTongYongCmd(void);




#endif

