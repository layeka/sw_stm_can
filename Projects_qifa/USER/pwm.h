#include "stm32f0xx.h"

#define PWM_OUT_PORT   GPIOB
#define PWM_OUT_PIN    GPIO_Pin_8
#define PWM_OUT_PINSOC    GPIO_PinSource8

#define PWM_CLK_DIV		32


#define   QIFAPWMHZ   18900
#define   QIFAPWMVALUE   65

#define PWMHZMIN        	200
#define PWMHZ          		1000


typedef struct __pwmpra {
    uint8_t PWM_work_state;  //0表示不工作，1表示工作中
    uint32_t TimerPeriod;
    uint32_t Channel1Pulse;
}PWMPRA;



extern void TIM16_PWM_Config(void);
extern void TIMPeriodReload(TIM_TypeDef *TIMx, uint32_t PwmHz);
extern void TIMSetPwmValue(TIM_TypeDef *TIMx, uint8_t PwmValue);


extern void TIMSetPwmValueUs(TIM_TypeDef *TIMx, uint16_t PwmValueUs);


extern void PWM_Plus_OFF(void);
extern void PWM_Plus_On(void);
extern void Set_PWM_Plus_Hz(uint16_t PwmHz);

extern PWMPRA PwmPra;






