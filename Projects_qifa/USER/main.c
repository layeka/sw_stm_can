#include "main.h"

#include "can.h"
#include "cancmd.h"
#include "anniuled.h"
//#include "bujin.h"
#include "swqf_output.h"
#include "pwm.h"
#include "flash id.h"
#include "Check_Qifa_Res_ADC.h"				//adc检测QIFA res
#include "boot.h"
#include "can_dmp.h"




int main(void) {
    appAvailableFlagNotOptimiz();
    memcpy((void *)0x20000000, (void *)(APPLICATION_ADDRESS), 48 * 4); //copy vectortable
    /* Enable the SYSCFG peripheral clock*/
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_SYSCFG, ENABLE);
    /* Remap SRAM at 0x00000000 */
    //SYSCFG_MemoryRemapConfig(SYSCFG_MemoryRemap_Flash);
    SYSCFG_MemoryRemapConfig(SYSCFG_MemoryRemap_SRAM);
    //系统外部时钟8M,PLL6倍，48M
    SystemInit();
    __enable_irq();
    Flash_Device_ID_STM = Get_Flash_ID();

    LedInit();
    ledRunSetState(LED_STAT_NOTREGEST);
    //AnniuInit();
    Swqf_Io_Init();
    candmpInit();
    Canini();
    timerTick1msInit();

    ADC_Cfg();
    TIM16_PWM_Config();
    TIMPeriodReload(TIM16, QIFAPWMHZ);        //设定PWM频率
    TIMSetPwmValue(TIM16, QIFAPWMVALUE);   //设定PWM占空比
    PwmPra.PWM_work_state = 1;        //默认上电 气阀为PWM节能状态
    PWM_Plus_On();
    //气阀状态变量初始化
    Qifa_state_Prainit();
    //***************************************************************//
    // HARD  INIT
    QifaRstCtr(1);
    QifaEnCtr(1);
    QifaPwmCtr(1);
    dmpIapToAppReturn();
    while (1) {
        ScanCmd();
        ledProcess();
        ScanAlarm();
        QifaRstProcess();
        if ((ADC_Value.end == 1) && (ADC_Value.Start_enable == 0)) {
            Calculate_Qifa_Return();
        }
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
