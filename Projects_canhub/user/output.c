/*******************************************************************************
IO_INPUT.C
********************************************************************************/
#include "output.h"


//////////////////////////////////////////////////////////////////////////
const OUTPUTGPIOGROUP Output_Single[Max_Output_Single] = {
    { Output0_PORT, Output0_PIN,Output0_defval,0},
    { Output1_PORT, Output1_PIN,Output1_defval,0},
    { Output2_PORT, Output2_PIN,Output2_defval,0},
    { Output3_PORT, Output3_PIN,Output3_defval,0},
    { Output4_PORT, Output4_PIN,Output4_defval,1},
};



uint32 outputval = 0;

void outputInit(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB, ENABLE);
    for (int i = 0; i < Max_Output_Single; i++) {
        GPIO_InitStructure.GPIO_Pin = Output_Single[i].bit;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
        GPIO_Init(Output_Single[i].port, &GPIO_InitStructure);
        bool outval =  Output_Single[i].defval ^ Output_Single[i].xor;
        GPIO_WriteBit(Output_Single[i].port, Output_Single[i].bit,
                      outval?Bit_SET:Bit_RESET);
        outputval |= (uint32)Output_Single[i].defval << i;
    }
}



void outputDrive(uint32 ioutput, bool val) {
     if(val){
        outputval |= 1 << ioutput;
     }else{
        outputval &= ~(1UL << ioutput);
     }
     val ^= Output_Single[ioutput].xor;
     if(val){
        GPIO_SetBits( Output_Single[ioutput].port, Output_Single[ioutput].bit);
     }else{
        GPIO_ResetBits( Output_Single[ioutput].port, Output_Single[ioutput].bit);
     }
}


