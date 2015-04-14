
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


#define MOTOR_CMDGO_FLAG_PROBE_SHIFT  0
#define MOTOR_CMDGO_FLAG_PROBE        (1<<MOTOR_CMDGO_FLAG_PROBE_SHIFT)
#define MOTOR_CMDGO_FLAG_CLEAR_POS_SHIFT     2
#define MOTOR_CMDGO_FLAG_CLEAR_POS           (1<<MOTOR_CMDGO_FLAG_CLEAR_POS_SHIFT)
#define MOTOR_CMDGO_FLAG_DIR                 (1<<5)
#define MOTOR_CMDGO_FLAG_STATE_SHIFT          6
#define MOTOR_CMDGO_FLAG_STATE                (3<<6)



#define MOTOR_CMDGO_RETURN_FLAG_PROBE_STOP  (1<<0)
#define MOTOR_CMDGO_RETURN_FLAG_FINISH_PLUS  (1<<1)
#define MOTOR_CMDGO_RETURN_FLAG_CLEAR_POS  (1<<2)
#define MOTOR_CMDGO_RETURN_FLAG_PROBE_SHIFT  3
#define MOTOR_CMDGO_RETURN_FLAG_PROBE  (1<<MOTOR_CMDGO_RETURN_FLAG_PROBE_SHIFT)
#define MOTOR_CMDGO_RETURN_FLAG_RETURN_SHIFT  4
#define MOTOR_CMDGO_RETURN_FLAG_RETURN  (1<<MOTOR_CMDGO_RETURN_FLAG_RETURN_SHIFT)
#define MOTOR_CMDGO_RETURN_FLAG_DIR_SHIFT  5
#define MOTOR_CMDGO_RETURN_FLAG_DIR  (1<<MOTOR_CMDGO_RETURN_FLAG_DIR_SHIFT)
#define MOTOR_CMDGO_RETURN_FLAG_STAT_SHIFT 6
#define MOTOR_CMDGO_RETURN_FLAG_STAT  (3<<MOTOR_CMDGO_STAT_SHIFT)

 typedef __packed struct {
    uint8_t  motor;
    uint16_t puls;
    uint16_t speedHz;
    uint8_t  flag;
    uint8_t  stopData;
    uint8_t  cmdcode;
}CMD_MOTORGO;




extern uint8_t ScanXiangJinMotoCmd(void);


extern void CanCmd_DumuMotoGo(CAN_WP *wp);
extern void DoXiangJinMotoCmd(void);
//extern void CanCmd_SetDumuMotoHz(void);
extern void CanCmd_GetDumuMotoState(CAN_WP *wp);
//extern uint8_t  CanCmd_ResetDumu(void);
extern void CanCmd_ResetDumu(CAN_WP *wp);
extern void CanCmd_SetDumuMotoHz(CAN_WP *wp);
extern void scanMotorCmd(void);


#endif


