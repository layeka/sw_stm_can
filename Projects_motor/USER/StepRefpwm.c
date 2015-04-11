//T2PWM  通过T2PWM来设定REF工作电压  
#include "stm32f0xx.h"
#include "step.h"  
#include "StepRefpwm.h"  

TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
TIM_OCInitTypeDef  TIM_OCInitStructure;
uint16_t TimerPeriod = 0;
uint16_t ChannelNum[4]={Moto1_Base,Moto2_Base,Moto3_Base,Moto4_Base};
uint16_t HalfPulseNum[4]={Moto1_Half_Current,Moto2_Half_Current,Moto3_Half_Current,Moto4_Half_Current};
uint16_t ChannelPulse[4] = {0};
/************************************************************************
电流对应的PWM值
电流分别为0.1 - 2.5A;对应的PWM值为数组里的值 
*************************************************************************/
							// 0.1    0.2   0.3    0.4    0.5    0.6   0.7    0.8     0.9    1.0
const uint16_t CurrentToPWM[25]={ 49,   93,  128, 157, 184, 209, 233, 256, 279, 302,
						    326, 350, 375, 400, 426, 452, 479, 506, 533, 561,
						    588, 616, 644, 672, 699};
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Configure the TIM2 Pins.
  * @param  None
  * @retval None
  */
void TIM2_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* GPIOA Clocks enable */
  RCC_AHBPeriphClockCmd( RCC_AHBPeriph_GPIOA, ENABLE);
  
  /* GPIOA Configuration: Channel 1, 2, 3 and 4 as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 |GPIO_Pin_2| GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource0, GPIO_AF_2);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_2);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_2);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_2);

}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void TIM2_PWM_OUTPUT(void)
{
  /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       file (startup_stm32f0xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f0xx.c file
     */

  
  /* TIM1 Configuration ---------------------------------------------------
   Generate PWM signals with 4 different duty cycles:
   TIM1 input clock (TIM1CLK) is set to APB2 clock (PCLK2)    
    => TIM1CLK = PCLK2 = SystemCoreClock
   TIM1CLK = SystemCoreClock, Prescaler = 0, TIM1 counter clock = SystemCoreClock
   SystemCoreClock is set to 48 MHz for STM32F0xx devices
   
   The objective is to generate 4 PWM signal at 40.00 KHz:
     - TIM1_Period = (SystemCoreClock / 40000) - 1
   The channel 1 and channel 1N duty cycle is set to 50%
   The channel 2 and channel 2N duty cycle is set to 37.5%
   The channel 3 and channel 3N duty cycle is set to 25%
   The channel 4 duty cycle is set to 12.5%
   The Timer pulse is calculated as follows:
     - ChannelxPulse = DutyCycle * (TIM1_Period - 1) / 100
   
   Note: 
    SystemCoreClock variable holds HCLK frequency and is defined in system_stm32f0xx.c file.
    Each time the core clock (HCLK) changes, user had to call SystemCoreClockUpdate()
    function to update SystemCoreClock variable value. Otherwise, any configuration
    based on this variable will be incorrect. 
  ----------------------------------------------------------------------- */
  /* Compute the value to be set in ARR regiter to generate signal frequency at 17.57 Khz */
  TimerPeriod = (SystemCoreClock / 20000 ) - 1;
  /* Compute CCR1 value to generate a duty cycle at 50% for channel 1 */
  ChannelPulse[0] = (uint16_t) (((uint32_t) (146) * (TimerPeriod - 1)) / 1000);
  /* Compute CCR2 value to generate a duty cycle at 37.5%  for channel 2 */
  ChannelPulse[1] = (uint16_t) (((uint32_t) (146) * (TimerPeriod - 1)) / 1000);
  /* Compute CCR3 value to generate a duty cycle at 25%  for channel 3 */
  //ChannelPulse[2] = (uint16_t) (((uint32_t) 25 * (TimerPeriod - 1)) / 100);
  ChannelPulse[2] = (uint16_t) (((uint32_t) (146) * (TimerPeriod - 1)) / 1000);
  /* Compute CCR4 value to generate a duty cycle at 12.5%  for channel 4 */
  //ChannelPulse[3] = (uint16_t) (((uint32_t) (440) * (TimerPeriod- 1)) / 1000);
  ChannelPulse[3] = (uint16_t) (((uint32_t) (146) * (TimerPeriod - 1)) / 1000);

  /* TIM2 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
  
  /* Time Base configuration */
  TIM_TimeBaseStructure.TIM_Prescaler = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseStructure.TIM_Period = TimerPeriod;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

  /* Channel 1, 2, 3 and 4 Configuration in PWM mode */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low;
  TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
  TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;

  TIM_OCInitStructure.TIM_Pulse = CurrentToPWM[ ChannelNum[0]];		//ChannelPulse[0];
  TIM_OC1Init(TIM2, &TIM_OCInitStructure);

  TIM_OCInitStructure.TIM_Pulse = CurrentToPWM[ ChannelNum[1]];		//ChannelPulse[1];
  TIM_OC2Init(TIM2, &TIM_OCInitStructure);

   TIM_OCInitStructure.TIM_Pulse = CurrentToPWM[ ChannelNum[2]];		//ChannelPulse[2];
  TIM_OC3Init(TIM2, &TIM_OCInitStructure);

  TIM_OCInitStructure.TIM_Pulse = CurrentToPWM[ ChannelNum[1]];		//ChannelPulse[3];
  TIM_OC4Init(TIM2, &TIM_OCInitStructure);

  /* TIM2 counter enable */
  TIM_Cmd(TIM2, ENABLE);

  /* TIM2 Output Enable */
  TIM_CtrlPWMOutputs(TIM2, ENABLE);
  
}

/**
  * @brief  调节每个PWM输出的占空比
  * @param  Pwm1Tim:比较器1输出的高电平相对时间
                                1:表示为千分之一
                                10:表示为千分之十
                                500:表示为千分之五百
                  Pwm2Tim:比较器2输出的高电平相对时间
                  Pwm3Tim:比较器3输出的高电平相对时间
                  Pwm4Tim:比较器4输出的高电平相对时间
  * @retval None
  */
void SetRefPwmAll(uint32_t Pwm1Tim, uint32_t Pwm2Tim, uint32_t Pwm3Tim, uint32_t Pwm4Tim)
{
	/* Set the Capture Compare Register value */
	TIM2->CCR1 = (uint16_t) (((uint32_t) Pwm1Tim * (TimerPeriod - 1)) / 1000); 
	TIM2->CCR2 = (uint16_t) (((uint32_t) Pwm2Tim * (TimerPeriod - 1)) / 1000); 
	TIM2->CCR3 = (uint16_t) (((uint32_t) Pwm3Tim * (TimerPeriod - 1)) / 1000); 
	TIM2->CCR4 = (uint16_t) (((uint32_t) Pwm4Tim * (TimerPeriod - 1)) / 1000); 
}

/**
  * @brief  调节每个PWM输出的占空比
  * @param  Pwm1Tim:比较器1输出的高电平相对时间
                                1:表示为千分之一
                                10:表示为千分之十
                                500:表示为千分之五百
                  Pwm2Tim:比较器2输出的高电平相对时间
                  Pwm3Tim:比较器3输出的高电平相对时间
                  Pwm4Tim:比较器4输出的高电平相对时间
  * @retval None
  */
void SetRefPwm(uint8_t Moto, uint32_t PwmTim)
{
	/* Set the Capture Compare Register value */
	switch(Moto) {
		case MIDUMOTO1:{ TIM2->CCR1 = (uint16_t) (((uint32_t) PwmTim * (TimerPeriod - 1)) / 1000);break;}
		case MIDUMOTO2:{ TIM2->CCR2 = (uint16_t) (((uint32_t) PwmTim * (TimerPeriod - 1)) / 1000);break;}
		case MIDUMOTO3:{ TIM2->CCR3 = (uint16_t) (((uint32_t) PwmTim * (TimerPeriod - 1)) / 1000);break;}
		case MIDUMOTO4:{ TIM2->CCR4 = (uint16_t) (((uint32_t) PwmTim * (TimerPeriod - 1)) / 1000);break;}
		default:break;
	}	
}



/***********************************************************************
10:  0.024
20:  0.036
30:  0.049
40:  0.063
50:  0.078
60:  0.095
70:  0.112
80:  0.131
90:  0.151
100: 0.172

110: 0.194
120: 0.217
130: 0.242
140: 0.268
150: 0.295
160: 0.324
170: 0.353
180: 0.384
190: 0.415
200: 0.448

210: 0.480
220: 0.514
230: 0.548
240: 0.582
250: 0.616
260: 0.651
270: 0.685
280: 0.719
290: 0.755
300: 0.789

310: 0.822
320: 0.856
330: 0.890
340: 0.923
350: 0.955
360: 0.988
370: 1.021
380: 1.053
390: 1.085
400: 1.117

410: 1.149
420: 1.180
430: 1.211
440: 1.241
450: 1.272
460: 1.302
470: 1.332
480: 1.361
490: 1.391
495: 1.405
500: 1.419

510: 1.450
520: 1.479
530: 1.508
540: 1.538
550: 1.567
560: 1.596
570: 1.625
580: 1.654
590: 1.683
600: 1.710

610: 1.741
620: 1.770
630: 1.799
640: 1.828
650: 1.857
660: 1.886
670: 1.916
680: 1.944
690: 1.974
700: 2.002

710: 2.033
720: 2.062
730: 2.088
740: 2.122
750: 2.152
760: 2.182
770: 2.213
780: 2.243
790: 2.274
800: 2.303

810: 2.335
820: 2.363
830: 2.398
840: 2.430
850: 2.462
860: 2.493
870: 2.525 
880: 2.557
890: 2.589
900: 2.620

910: 2.654
920: 2.687
930: 2.719
940: 2.751
950: 2.780
960: 2.813
970: 2.843
980: 2.872
990: 2.902
1000:2.931

1010:2.963
1020:2.990
1030:3.016
1040:3.041
1050:3.065
1060:3.088
1070:3.110
1080:3.131
1090:3.151
1100:3.166

1110:3.188
1120:3.204
1130:3.220
1140:3.235
1150:3.249
1160:3.261
1170:3.273
1180:3.284
1190:3.290
1198:3.297





************************************************************************/


