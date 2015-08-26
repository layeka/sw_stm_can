

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_it.h"
//#include "step.h"
#include "io_input.h"
#include "can.h"
#include "anniuled.h"
#include "cancmd.h"
#include "timer.h"
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




void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
	/* Go to infinite loop when Hard Fault exception occurs */
	while (1)
	{
	}
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
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

/**
  * @}
  */

#if 0
void TIM1_CC_IRQHandler(void)
{
	//uint32_t  capture;
	// TIM1_CH1
	if (TIM_GetITStatus(TIM1, TIM_IT_CC1) != RESET)
	{
	TIM_ClearITPendingBit(TIM1, TIM_IT_CC1 );
	MotoDumuRunScan(MIDUMOTO1);
	}
	// TIM1_CH2
	if (TIM_GetITStatus(TIM1, TIM_IT_CC2) != RESET)
	{
	TIM_ClearITPendingBit(TIM1, TIM_IT_CC2);
	MotoDumuRunScan(MIDUMOTO2);
	}
	// TIM1_CH3
	if (TIM_GetITStatus(TIM1, TIM_IT_CC3) != RESET)
	{
	TIM_ClearITPendingBit(TIM1, TIM_IT_CC3);
	MotoDumuRunScan(MIDUMOTO3);
	}
	// TIM1_CH4
	if (TIM_GetITStatus(TIM1, TIM_IT_CC4) != RESET)
	{
	TIM_ClearITPendingBit(TIM1, TIM_IT_CC4);
	MotoDumuRunScan(MIDUMOTO4);
	//capture = TIM_GetCapture4(TIM1);
	//TIM_SetCompare4(TIM1, capture + 2000);
	}
}
#endif
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/




