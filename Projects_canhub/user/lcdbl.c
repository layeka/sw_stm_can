
#include "stm32f0xx.h"
#include  "lcdbl.h"




static unsigned short timerPeriod ;

static void TIM_PWM_Config(void)
{
	/* TIM1 的配置 ---------------------------------------------------
   TIM1 输入时钟(TIM1CLK) 设置为 APB2 时钟 (PCLK2)
    => TIM1CLK = PCLK2 = SystemCoreClock
   TIM1CLK = SystemCoreClock, Prescaler = 0, TIM1 counter clock = SystemCoreClock
   SystemCoreClock 为48 MHz */

  unsigned int TimerPeriod = SystemCoreClock/50000-1;
  timerPeriod = TimerPeriod;
  /* TIM1 时钟使能 */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM16 , ENABLE);

  /* Time 定时基础设置*/
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  TIM_TimeBaseStructure.TIM_Prescaler = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  /* Time 定时设置为上升沿计算模式*/
  TIM_TimeBaseStructure.TIM_Period = TimerPeriod;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

  TIM_TimeBaseInit(TIM16, &TIM_TimeBaseStructure);
  TIM_OCInitTypeDef TIM_OCInitStructure;

  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;
  TIM_OCInitStructure.TIM_Pulse = 0;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
  TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
  TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
  TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;


  TIM_OC1Init(TIM16, &TIM_OCInitStructure);
  /* TIM1 主输出使能 */
  TIM_CtrlPWMOutputs(TIM16, ENABLE);
  /* TIM1 计算器使能*/
  TIM_Cmd(TIM16, ENABLE);
}

void LcdBlInit(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
    GPIO_InitStructure.GPIO_Pin = LCDBL_GPIOPIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(LCDBL_GPIO, &GPIO_InitStructure);
    GPIO_PinAFConfig(LCDBL_GPIO, LCDBL_GPIOBIT_PinSource, GPIO_AF_2); //TIM16_CH1
    TIM_PWM_Config();
    LcdBlCtr(0);
    GPIO_InitStructure.GPIO_Pin = LCDBL_POWER_GPIOPIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_Init(LCDBL_POWER_GPIO, &GPIO_InitStructure);
    GPIO_ResetBits(LCDBL_POWER_GPIO, LCDBL_POWER_GPIOPIN);
}



void LcdBlCtr(uint8_t  val) {
    unsigned int valtemp;
    valtemp = (val!=255)?timerPeriod * val / 255 :timerPeriod * val / 255 + 1;
    GPIO_WriteBit(LCDBL_POWER_GPIO, LCDBL_POWER_GPIOPIN, val); //control power en pin
    TIM_SetCompare1(TIM16, valtemp);
}






