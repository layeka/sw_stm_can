#include "stm32f0xx.h"

#define PWM_OUT_PORT   GPIOB
#define PWM_OUT_PIN    GPIO_Pin_8
#define PWM_OUT_PINSOC    GPIO_PinSource8


#define XIANGJINCLKDIV  32     //橡筋 时钟分频 
#define  XIANGJINMOTOSTOPHZ       25


#define PWMHZMIN        200

#define PWMHZ          1000
#define PWMHIGHLEN     25    //高电平宽度25us;


#define  XiangjinMoto_REF_PORT   GPIOB
#define  XiangjinMoto_REF_PIN    GPIO_Pin_6
#define  XiangjinMoto_FREE_PORT   GPIOB
#define  XiangjinMoto_FREE_PIN    GPIO_Pin_9
#define  XiangjinMoto_DIR_PORT   GPIOB
#define  XiangjinMoto_DIR_PIN    GPIO_Pin_7

#define  Moto4_ZERO_PORT   GPIOA
#define  Moto4_ZERO_PIN    GPIO_Pin_15



typedef struct __pwmpra{
  uint8_t    XiangjinMotoState;    //0表示不工作，1表示工作中
  uint32_t   TimerPeriod;
  uint32_t   Channel1Pulse;
  
}PWMPRA;



extern void TIM16_PWM_Config(void);
extern void TIMPeriodReload(TIM_TypeDef* TIMx,uint32_t  PwmHz);
extern void TIMSetPwmValue(TIM_TypeDef* TIMx,uint8_t PwmValue);
extern void XiangjinMotoini(void);


extern void TIMSetPwmValueUs(TIM_TypeDef* TIMx,uint16_t PwmValueUs);


extern void XiangjinMotoOff(void);
extern void XiangjinMotoOn(uint16_t  PwmHz);
extern void SetXiangjinMotoHz(uint16_t  PwmHz);
extern PWMPRA PwmPra;






