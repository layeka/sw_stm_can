


#include "stm32f0xx_it.h"
#include "can.h"
#include "anniuled.h"
#include "main.h"
#include "pwm.h"
#include "Check_Qifa_Res_ADC.h"				//adcºÏ≤‚QIFA res
#include "swqf_output.h"
#include "algorithm.h"
#include "cancmd.h"






/** @addtogroup Template_Project
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M0 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */




void NMI_Handler(void) {
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void) {
    /* Go to infinite loop when Hard Fault exception occurs */
    while (1) {
    }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void) {
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void) {
}


extern long long timerTick1ms;
extern long long timerTick05ms;
/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void) {
    timerTick05ms++;
    if (timerTick05ms % 2 == 0) {
        timerTick1ms++;
        static unsigned char a= 0 ;
        a = !a;
        LedCtr_RUN_ERR(a,a );

        //qifa power save pwm open
        if (Qifa_state.PWM_Close_Time_Count && (Qifa_state.Out_PWM_Enable == ENABLE)) {
            Qifa_state.PWM_Close_Time_Count--;
            if (Qifa_state.PWM_Close_Time_Count == 0) {
                PWM_Plus_On();
            }
        }
        Scan_qifa_ADC();
    }
}



/******************************************************************************/
/*                 STM32F0xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f0xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/





