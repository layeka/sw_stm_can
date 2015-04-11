
#ifndef __CANCMD__H__
#define __CANCMD__H__

#include "can_wp.h"

#define SW_Needle_Moto           0
#define SW_fengMen_Moto	    1
#define SW_SinkMoto1_3	    2
#define SW_SinkMoto2_4	    3

#define TONGYONG                 		4
#define DOCMDMAXTIME             		1000



#define CANCMD_DUMUMOTOGO_LOW        			        0x01
#define CANCMD_GETDUMUMOTOSTATE  				0x02   //读取电机状态
#define CANCMD_SETDUMUMOTOHZ     				0x41	//设定电机平率
#define CANCMD_RESETDUMU         	    			0x42    //复位度目电机
#define CANCMD_WORKCURRENT_SET				        0x43 	//工作电流设定
#define CANCMD_LOCKCURRENT_SET					0x44 	//半流电流值设定





extern uint8_t ScanXiangJinMotoCmd(void);


extern void CanCmd_DumuMotoGo(CAN_WP *wp);
extern void DoXiangJinMotoCmd(void);
//extern void CanCmd_SetDumuMotoHz(void);
extern void CanCmd_GetDumuMotoState(CAN_WP *wp);
//extern uint8_t  CanCmd_ResetDumu(void);
extern void CanCmd_ResetDumu(CAN_WP *wp);
extern void CanCmd_SetDumuMotoHz(CAN_WP *wp);


#endif


