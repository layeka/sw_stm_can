
/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_it.h"
#include "step.h"
#include "can.h"
#include "anniuled.h"
#include "main.h"
#include "algorithm.h"
#include "cancmd.h"








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

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void) {
  timerTick05ms++;
    if (timerTick05ms % 2 ==0) {
		timerTick1ms++;
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



void TIM1_CC_IRQHandler(void) {
    for (int i = 0; i < 4; i++) {
        unsigned int ch = MiduMotoCtrPra[i].stepTimerChanel;
        if (TIM_GetITStatus(TIM1, ch) != RESET){
            TIM_ClearITPendingBit(TIM1, ch);
            MotoDumuProcess(i);
        }
    }
}



/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/




