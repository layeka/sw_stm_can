#ifndef __CANCMD_H__
#define __CANCMD_H__


#include "can_wp.h"
#include "algorithm.h"

#define CANCMD_READSOFTVER       		0x01
#define CANCMD_CHECKID				0X02		//// 查询当前工作ID

#define CANCMD_SETADRR        0XFD  		//	 0X01FD
#define CANCMD_GOTOAPP        0xFC			//	0x01FC
#define CANCMD_GOTOIAP         0xFB			//	0x01FB
#define CANCMD_WAITSETADD	0XFA		//	等待设定地址状态



#define CANCMD_SETPWMEN       		0x21
#define CANCMD_SETQIFAEN      		0x22
#define CANCMD_QIFARST        		0x23
#define CANCMD_SETPWMVALUE    		0x24
#define CANCMD_QIFADRV        		0x25
#define CANCMD_READALARM      		0x28
#define CANCMD_READQIFA1TO16  		0x29
//#define CANCMD_READQIFA65TO80 		0x002a
#define CANCMD_SENDALARM      		0x002b
//单个气阀测试 单个气阀动作时测量气阀的阻值并上报到主板
#define CANCMD_SINGAL_QIFA			0X002C



extern void CanCmd_SetPwmEn(CAN_WP *wp);
extern void CanCmd_SetQifaEn(void);
extern void CanCmd_QifaRst(void);
extern void CanCmd_QifaDrv(CAN_WP *wp);
extern void CanCmd_SetPwmValue(CAN_WP *wp);
extern void CanCmd_SetAlarmEn(void);
extern void CanCmd_SetAlarmTure(void);
extern void CanCmd_ReadQifa1to16(CAN_WP *wp);
extern void ScanAlarm(void);

extern void CanCmd_ReturnQIfa_Res_value(uint8_t num, uint16_t res_value);
extern uint8_t CanCmd_GUANGBO(void);

extern uint8_t CanReturn_Dc(uint16_t cancmd, uint8_t state);




#endif

