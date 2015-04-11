
#include "boot.h"
#include "type.h"
#include "stm32f0xx.h"


#if defined(APP)
const unsigned long long appAvailableFlag @ APP_AVAILABLE_FLAG_ADDR  =  APP_OK ;

void  appAvailableFlagNotOptimiz(){
    volatile unsigned int val;
    val =  (unsigned int)appAvailableFlag;
}
#endif





unsigned char isAppAvailable(){
    unsigned long long *pAppAvailableFlag;
    pAppAvailableFlag = (unsigned long long *)APP_AVAILABLE_FLAG_ADDR;
    return (*(uint32_t *)APP_ADDR !=0xffffffff && *pAppAvailableFlag==APP_OK);
}

void bootApp(){
    __disable_irq();
    __set_MSP(*(__IO uint32_t *)APPLICATION_ADDRESS);
    ((void (*)())(*(unsigned int *)(APPLICATION_ADDRESS + 4)))();
}


void bootAppToIap(){
    __disable_irq();
    BOOT_FLAG_APPTOIAP = POWERUP_APPTOIAP;
    __set_MSP(*(__IO uint32_t *)IAPLICATION_ADDRESS);
    ((void (*)())(*(unsigned int *)(IAPLICATION_ADDRESS + 4)))();
}
