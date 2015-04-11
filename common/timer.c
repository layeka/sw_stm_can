
#include "timer.h"
#include "stm32f0xx.h"

long long timerTick1ms = 0x10000;

long long timerTick05ms = 0;

void timerTick1msInit(void) { //timer for 0.5 ms interrupt
    SysTick_Config(SystemCoreClock / 2000);
}

unsigned long long getTimerTick(){
    return  timerTick1ms;
}
