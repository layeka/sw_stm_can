

/* Includes ------------------------------------------------------------------*/
#include "type.h"
#include "main.h"
#include "io_input.h"
//#include "step.h"
#include "can.h"
#include "cancmd.h"
#include "anniuled.h"
#include "flash id.h"
#include "timer.h"
#include "boot.h"
#include "can_dmp.h"
#include "output.h"
#include "temperature.h"
#include "lcdbl.h"



int main(void) {
    SystemCoreClockUpdate();
    appAvailableFlagNotOptimiz();
    memcpy((void *)0x20000000,(void *)(APPLICATION_ADDRESS),48*4);//copy vectortable
    /* Enable the SYSCFG peripheral clock*/
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_SYSCFG, ENABLE);
    /* Remap SRAM at 0x00000000 */
    SYSCFG_MemoryRemapConfig(SYSCFG_MemoryRemap_SRAM);
    Flash_Device_ID_STM = Get_Flash_ID();

    __enable_irq();
    timerTick1msInit();
    LedInit();
    //AnniuInit();
    Io_input_Init();
    ledRunSetState(LED_STAT_NOTREGEST);
    candmpInit();
    Canini();
    dmpIapToAppReturn();
    io_scan_init();
    outputInit();
    LcdBlInit();
    temperaInit();
    temperaAdcStart();

    while (1) {
       ScanCmd();
       ioAlarmProcess();
       ioScan();
       ledProcess();
       getTempera();
    }
}




#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line) {
    /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

    /* Infinite loop */
    while (1) {
    }
}
#endif

/**
  * @}
  */
//////////////////////////////////
/*void Timer14ini(void) {
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14, ENABLE);


    TIM_TimeBaseStructure.TIM_Period = T1msCNT;
    //TIM_TimeBaseStructure.TIM_Period = T10msCNT;
    TIM_TimeBaseStructure.TIM_Prescaler = 47; //48MHz / (47+1) = 1MHz about 1us
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM14, &TIM_TimeBaseStructure);

    TIM_ITConfig(TIM14, TIM_EventSource_Update, ENABLE);
    TIM_Cmd(TIM14, ENABLE);
    NVIC_InitTypeDef NVIC_InitStructure;

    NVIC_InitStructure.NVIC_IRQChannel = TIM14_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}*/

void IWDG_Config(void) {
    /* IWDG timeout equal to 250 ms (the timeout may varies due to LSI frequency
    dispersion) */
    /* Enable write access to IWDG_PR and IWDG_RLR registers */
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);

    /* IWDG counter clock: LSI/32 */
    IWDG_SetPrescaler(IWDG_Prescaler_32);

    /* Set counter reload value to obtain 250ms IWDG TimeOut.
    Counter Reload Value = 250ms/IWDG counter clock period
    = 250ms / (LSI/32)
    = 0.25s / (LsiFreq/32)
    = LsiFreq/(32 * 4)
    = LsiFreq/128
    LsiFreq=40k;
    */
    IWDG_SetReload(312);

    /* Reload IWDG counter */
    IWDG_ReloadCounter();

    /* Enable IWDG (the LSI oscillator will be enabled by hardware) */
    IWDG_Enable();
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
