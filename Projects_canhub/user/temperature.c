#include "type.h"
#include "stm32f0xx.h"
#include "algorithm.h"
#include <string.h>
#include "timer.h"






#define ADC1_DR_Address                0x40012440

#define ADC_SAMPLE_NUM   20

static int32 adcbuf[ADC_SAMPLE_NUM],adcbufbak[ADC_SAMPLE_NUM-2];


void temperaInit(void){
  //clock
  /* Enable  GPIOA clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  /* ADC1 Periph clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
  /* DMA1 clock enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1 , ENABLE);

  NVIC_InitTypeDef NVIC_InitStructure;

   /* NVIC configuration */
  NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  //io
  /* Configure PA.02  as analog input */
  GPIO_InitTypeDef    GPIO_InitStruct;
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* DMA1 Channel1 Config */
  DMA_InitTypeDef  DMA_InitStruct;
  DMA_DeInit(DMA1_Channel1);
  DMA_InitStruct.DMA_PeripheralBaseAddr = ADC1_DR_Address;
  DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)&adcbuf;
  DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralSRC;
  DMA_InitStruct.DMA_BufferSize = lenthof(adcbuf);
  DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
  DMA_InitStruct.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStruct.DMA_Priority = DMA_Priority_High;
  DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(DMA1_Channel1, &DMA_InitStruct);
  DMA_ITConfig(DMA1_Channel1, DMA_IT_TC,ENABLE);
  DMA_Cmd(DMA1_Channel1, ENABLE);

    //ADC
  ADC_DeInit(ADC1);


  ADC_InitTypeDef   ADC_InitStruct;

  ADC_StructInit(&ADC_InitStruct);
  ADC_InitStruct.ADC_Resolution = ADC_Resolution_10b;
  ADC_InitStruct.ADC_ContinuousConvMode = ENABLE;
  ADC_InitStruct.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
  ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStruct.ADC_ScanDirection = ADC_ScanDirection_Backward;
  ADC_Init(ADC1, &ADC_InitStruct);
  ADC_ChannelConfig(ADC1, ADC_Channel_2 , ADC_SampleTime_239_5Cycles);
    /* ADC DMA request in circular mode */

  ADC_DMARequestModeConfig(ADC1, ADC_DMAMode_Circular);
    /* Enable ADC_DMA */
  ADC_DMACmd(ADC1, ENABLE);

  ADC_GetCalibrationFactor(ADC1);
  ADC_ClockModeConfig(ADC1, ADC_ClockMode_SynClkDiv4);
    //start adc
  ADC_Cmd(ADC1, ENABLE);
  while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_ADEN));
}

void temperaAdcStart(void){
    ADC_StartOfConversion(ADC1);
};



int tempraAdcData = 0;
bool adcfinish = false;


void DMA1_Channel1_IRQHandler(void){
   if (DMA_GetFlagStatus(DMA1_FLAG_TC1) == SET) {
        DMA_ClearFlag(DMA1_FLAG_TC1);
        if(adcfinish==false){
          //every 0.5s
          everydiffdo(long long,__adctime,timerTick05ms/1000){
                memcpy(adcbufbak,adcbuf+2,sizeof(adcbufbak));
                adcfinish = true;
          }
       }
   }
}

int32 temperature = 0;
static int ad2tempera(int adc){
  int temp;
  int32 res_table[111] = { 228600,217800,207600,198000,189100,180700,
  172800,165300,158400,151700,145500,138900,132700,126800,121200,115800,
  110700,105900,101400,97000,92860,88930,85180,81620,78230,75000,71930,
  69000,66210,63560,61020,58610,56300,54100,52000,50000,48090,46260,44510,
  42840,41250,39720,38260,36870,35530,34250,33030,31850,30730,29650,28620,
  27630,26680,25760,24890,24050,23240,22470,21720,21010,20320,19660,19020,
  18410,17820,17260,16710,16190,15680,15190,14730,14270,13840,13420,13010,
  12620,12240,11880,11530,11190,10860,10540,10240,9940,9654,9378,9111,8852,
  8602,8361,8127,7900,7681,7469,7264,7065,6872,6685,6504,6329,6159,5994,5834,
  5679,5529,5383,5242,5105,4971,4842,4717};

  int32 ohm;
  if(adc<20){
    return 1000;
  }else if(adc>1000){
    return 1001;
  }else{
    ohm = (1000*adc)*10/(1024-adc);
  }
  if(ohm>=228600){
    temp = -10;
  }
  else if(ohm<=4717) {
    temp = 100;
  } else {
    for(int i=0;i<111;i++){
        if(ohm>=res_table[i]){
            temp = i-10;
            break;
        }
    }
  }
  return temp;
}


void getTempera(void){
  int adc;
  if(adcfinish){
    bubbleSortAscend_I32(adcbufbak, lenthof(adcbufbak));
    temperaAdcStart();
    adc = sum(adcbufbak+2,lenthof(adcbufbak)-4)/(lenthof(adcbufbak)-4);
    temperature = ad2tempera(adc);
    adcfinish = false;
  }
}

