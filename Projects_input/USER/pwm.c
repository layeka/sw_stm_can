#include "pwm.h"  


PWMPRA PwmPra;
const uint8_t    PwmLev[9]={60,65,70,75,80,85,90,95,120};   //占空比档位


void TIM16_PWM_Config(void)
{
  
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_OCInitTypeDef  TIM_OCInitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
   
  
  /* GPIOB a Clocks enable */
  RCC_AHBPeriphClockCmd(  RCC_AHBPeriph_GPIOB, ENABLE);
  
  
  /* GPIOA Configuration: Channel 1, 2, 3, 4 and Channel 1N as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = PWM_OUT_PIN; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
  GPIO_Init(PWM_OUT_PORT, &GPIO_InitStructure);
  
  GPIO_PinAFConfig(PWM_OUT_PORT, PWM_OUT_PINSOC, GPIO_AF_2);
  
    
  
  
  /* TIM1 Configuration ---------------------------------------------------
   Generate 7 PWM signals with 4 different duty cycles:
   TIM1 input clock (TIM1CLK) is set to APB2 clock (PCLK2)    
    => TIM1CLK = PCLK2 = SystemCoreClock
   TIM1CLK = SystemCoreClock, Prescaler = 0, TIM1 counter clock = SystemCoreClock
   SystemCoreClock is set to 48 MHz for STM32F0xx devices
   
   The objective is to generate 7 PWM signal at 17.57 KHz:
     - TIM1_Period = (SystemCoreClock / 17570) - 1
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
  /* Compute the value to be set in ARR regiter to generate signal frequency at PWMHZ Khz */
  TIMPeriodReload(TIM16,PWMHZ);
  
  /* Compute CCR1 value to generate a duty cycle at 50% for channel 1 and 1N */
  //PwmPra.Channel1Pulse = (uint16_t) (((uint32_t) 5 * (PwmPra.TimerPeriod - 1)) / 10);
  TIMSetPwmValue( TIM16,PwmLev[0]);

  /* TIM16 clock enable */
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM16 , ENABLE);
 
  
  /* Time Base configuration */
  TIM_TimeBaseStructure.TIM_Prescaler =XIANGJINCLKDIV-1;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseStructure.TIM_Period = PwmPra.TimerPeriod;
  TIM_TimeBaseStructure.TIM_ClockDivision =0;
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0; // 重复寄存器，用于自动更新pwm占空比
 

  TIM_TimeBaseInit(TIM16, &TIM_TimeBaseStructure);
  
  /* Channel 1, 2,3 and 4 Configuration in PWM mode */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;
  TIM_OCInitStructure.TIM_Pulse = PwmPra.Channel1Pulse;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
  TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
  TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
  TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;

  TIM_OC1Init(TIM16, &TIM_OCInitStructure);
  
  TIM_OC1PreloadConfig(TIM16, TIM_OCPreload_Enable);
  

  
 TIM_ARRPreloadConfig(TIM16, ENABLE);

 /* TIM1 Main Output Enable */
 // TIM_CtrlPWMOutputs(TIM16, ENABLE);
  /* TIM16 counter enable */
 // TIM_Cmd(TIM16, ENABLE);

  
}
//调整PWM频率
void TIMPeriodReload(TIM_TypeDef* TIMx,uint32_t  PwmHz)
{
  /*
    if(PwmHz<PWMHZMIN) 
    	{
    	  PwmHz=PWMHZMIN; 
    	}
  */
    PwmPra.TimerPeriod = (SystemCoreClock / (XIANGJINCLKDIV*PwmHz) ) - 1;
    TIMx->ARR=PwmPra.TimerPeriod;
    
}

//调整PWM占空比
void TIMSetPwmValue(TIM_TypeDef* TIMx,uint8_t PwmValue)
{
   PwmPra.Channel1Pulse= (PwmValue *(PwmPra.TimerPeriod - 1)) / 100;
   TIM_SetCompare1(TIMx, PwmPra.Channel1Pulse);
}

//调整PWM高电平时间
void TIMSetPwmValueUs(TIM_TypeDef* TIMx,uint16_t PwmValueUs)
{
   uint32_t  buf;
   buf=XIANGJINCLKDIV*1000000;
   buf=SystemCoreClock/buf;
   PwmPra.Channel1Pulse= buf*PwmValueUs;
   TIM_SetCompare1(TIMx, PwmPra.Channel1Pulse);
}
void XiangjinMotoini(void)
{
      GPIO_InitTypeDef GPIO_InitStructure;
      RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB , ENABLE);
	  
      GPIO_InitStructure.GPIO_Pin = XiangjinMoto_REF_PIN;
      GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
      GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;  
      GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
      GPIO_Init(XiangjinMoto_REF_PORT, &GPIO_InitStructure);
      GPIO_InitStructure.GPIO_Pin = XiangjinMoto_DIR_PIN;
      GPIO_Init(XiangjinMoto_DIR_PORT, &GPIO_InitStructure);
	  GPIO_InitStructure.GPIO_Pin = XiangjinMoto_FREE_PIN;
      GPIO_Init(XiangjinMoto_FREE_PORT, &GPIO_InitStructure);

	  GPIO_SetBits(XiangjinMoto_REF_PORT,XiangjinMoto_REF_PIN);
	  GPIO_SetBits(XiangjinMoto_DIR_PORT,XiangjinMoto_DIR_PIN);
	  GPIO_SetBits(XiangjinMoto_FREE_PORT,XiangjinMoto_FREE_PIN);
	  
      TIM16_PWM_Config();
	  PwmPra.XiangjinMotoState=0;
         
}

void XiangjinMotoOff(void)
{
  TIM_CtrlPWMOutputs(TIM16, DISABLE);
  TIM_Cmd(TIM16, DISABLE);
  GPIO_SetBits(XiangjinMoto_FREE_PORT,XiangjinMoto_FREE_PIN);
  PwmPra.XiangjinMotoState=0;
}
void XiangjinMotoOn(uint16_t  PwmHz)
{
 
  GPIO_ResetBits(XiangjinMoto_FREE_PORT,XiangjinMoto_FREE_PIN);
  
  TIM_SetCounter(TIM16,0);
  TIMPeriodReload(TIM16,PwmHz);
 //TIMSetPwmValueUs(TIM16,PWMHIGHLEN);
 TIMSetPwmValue(TIM16,50);
 TIM_ARRPreloadConfig(TIM16, ENABLE);
  TIM_CtrlPWMOutputs(TIM16, ENABLE);
  TIM_Cmd(TIM16, ENABLE);
  PwmPra.XiangjinMotoState=1;
}
void SetXiangjinMotoHz(uint16_t  PwmHz)
{
  
   if(PwmPra.XiangjinMotoState)
   	{
   	  TIMPeriodReload(TIM16,PwmHz);
          TIMSetPwmValue(TIM16,50);
          
   	}
   else
   	{
   	   XiangjinMotoOn(PwmHz);
   	}
}


