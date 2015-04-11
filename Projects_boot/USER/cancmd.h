
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





extern void  ScanCmd(void);

extern void DoTongYongCmd(void);




#endif

