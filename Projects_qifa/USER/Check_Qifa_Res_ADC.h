
#ifndef __Check_Qifa_Res_ADC_H
#define __Check_Qifa_Res_ADC_H

#ifdef __cplusplus
 extern "C" {
#endif
/*************************************************************************************************
**************************************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx.h"

/* Exported types ------------------------------------------------------------*/
//+++++++++++++++++++++++++++++++++++++++++++
//ADC采样参数
#define Max_ADC_Value 		10////4			//8		//12 
#define Beilv_ADC_Value                5//10		//	5			//  50 / 10 
#define Average_ADC_Value_Num     5
#define Start_ADC_Num			5
#define Max_ADC_DelayTIME	80
typedef struct __S_ADC_Value{
	uint8_t Start_enable;
	uint8_t delaytime;
	uint8_t Count;
	uint8_t end;
	float Vlaue_buf[Max_ADC_Value];
	//float Value_max_buf[5];
	float Value_data;
	uint16_t Res_Value;
	uint8_t Num;
	
}S_ADC_Value;

extern S_ADC_Value  ADC_Value ;


/* Exported constants --------------------------------------------------------*/

/* Exported Variables --------------------------------------------------------*/
extern uint32_t Voltage_f;	//

/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------- */ 
#define Max_Voltage 33000    // // 3.3v  放大10000倍  计算基准按照MV来计算



void ADC_Cfg(void);
float VoltageCal(void);
void ADC_ReadAndCal(void);


extern void Scan_qifa_ADC(void);
extern void Calculate_Qifa_Return(void);

/*-------------------------------------------------------------------------*/
#ifdef __cplusplus
}
#endif

#endif /* __Check_Qifa_Res_ADC_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT  *****END OF FILE****/

