#ifndef __STEPREFPWM_H
#define __STEPREFPWM_H

#ifdef __cplusplus
 extern "C" {
#endif
#define Moto1_Base	3			//[ 3 ]    0.4A
#define Moto2_Base	3			//[ 3 ]    0.4A
#define Moto3_Base	3			//[ 3 ]    0.4A
#define Moto4_Base	3			//[ 3 ]    0.4A

#define Moto1_Half_Current		1	//默认锁定电流0.2A
#define Moto2_Half_Current		1	//默认锁定电流0.2A
#define Moto3_Half_Current		1	//默认锁定电流0.2A
#define Moto4_Half_Current		1	//默认锁定电流0.2A
//#define Half_Lock_Value	30             //锁定电流  百分比






extern uint16_t ChannelPulse[4];
extern uint16_t ChannelNum[4];
extern uint16_t HalfPulseNum[4];
extern const uint16_t CurrentToPWM[25];

void TIM2_Config(void);
void TIM2_PWM_OUTPUT(void);
void SetRefPwm(uint8_t Moto, uint32_t PwmTim);
//extern uint8_t Get_HalfLock_PwmPluse(uint8_t Moto,uint8_t Value);


#ifdef __cplusplus
}
#endif




#endif


//##############################################################
//NO MORE 
//##############################################################
