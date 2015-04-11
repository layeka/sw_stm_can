/************************************************
//���ò��������鵥����������ʱ�ĵ��� �õ���ǰ���������ĵ���
//���ݵ����жϵ�ǰ��������״̬������Χ������ʾ
**************************************************/
#include "Check_Qifa_Res_ADC.h"	
#include "cancmd.h"
#include "anniuled.h"
#include "SWQF_OUTPUT.h"
uint32_t  Voltage_f;	//
uint32_t  Voltage;
S_ADC_Value  ADC_Value ;

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
  * @brief  ADC configuration
  * @note   
  * @retval
  */
void ADC_Cfg(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);  
	ADC_Cmd(ADC1, DISABLE);
	ADC_VrefintCmd(ENABLE);
	ADC_GetCalibrationFactor(ADC1);//У׼

	ADC1->CHSELR |= ADC_CHSELR_CHSEL0 | ADC_CHSELR_CHSEL17  ;
	ADC1->SMPR |= ADC_SMPR_SMP_0 | ADC_SMPR_SMP_1 | ADC_SMPR_SMP_2; 
	ADC_Cmd(ADC1, ENABLE);

	/* GPIOA Clocks enable */
	RCC_AHBPeriphClockCmd( RCC_AHBPeriph_GPIOA, ENABLE);
	
	/* GPIOA Configuration: Channel 1, 2, 3 and 4 as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
  * @brief  Calculate the actual Voltage
  * @note   
  * @retval The value of the VoltageCal data.
  //STM32F042Cx  Ϊ12 bit ����ADC
  */
float VoltageCal(void)
{	
	//uint32_t Voltage;
	#define Vref_CAL_ADDR ((uint16_t*) ((uint32_t) 0x1FFFF7ba))
	float Voltage;
	float Voltage2;
	float Voltage3;
	//����ת��
	ADC_StartOfConversion(ADC1);
	//wait for conversion complete
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC)){;}
	//read ADC value
	Voltage = (float)ADC_GetConversionValue(ADC1);    
	#if  1
	//wait for conversion complete
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC)){;}
	//read ADC value
	Voltage2 = (float)ADC_GetConversionValue(ADC1);    
	Voltage3 =   ((*Vref_CAL_ADDR )* 3.3) /Voltage2;
	Voltage = ( Voltage * Voltage3) /0xFFF;
	#else
	Voltage = ( Voltage * 3.3) /0xFFF;
	#endif	
    return Voltage;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
  * @brief  Read and Calculate the actual 
  * @note   
  * @retval The value of the VoltageCal data.
  */
void ADC_ReadAndCal(void)
{	
	
	//����ת��
	ADC_StartOfConversion(ADC1);
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC)){;}
	//read ADC_02 value
	Voltage = ADC_GetConversionValue(ADC1);	
		
	Voltage = (Voltage * Max_Voltage ) /0xFFF;	
}


///////////////////////////////////////////////////////
void Calculate_Qifa_Return(void)
{
	uint8_t i ;
	//������������
	ADC_Value.Value_data = 0 ;
	ADC_Value.end = 0;
	//����10�� �ӵ�5�� ��ʼȡ������5������ƽ��
	for( i = Start_ADC_Num ; i <  (Start_ADC_Num + Average_ADC_Value_Num) ; i ++ )
	{
		ADC_Value.Value_data = ADC_Value.Value_data + ADC_Value.Vlaue_buf[ i ];
	}
	ADC_Value.Value_data = ADC_Value.Value_data / Average_ADC_Value_Num ;
	
	if(ADC_Value.Value_data )
	{
		//ADC_Value.Res_Value == 	U/I  	=  24V/((����ֵ��ѹ/�Ŵ���)/��������ֵ)
		//  == 24V /( (����ֵ��ѹ/�Ŵ��� )/0.1��)
		//  ==  24 * �Ŵ��� * 0.1 / ����ֵ��ѹ
		//�Ŵ�50�� ��������ֵ0.1ŷķ  Beilv_ADC_Value =  5 ��
		//�Ŵ�50�� ��������ֵ0.2ŷķ  Beilv_ADC_Value =  10 ��
		ADC_Value.Res_Value =(uint16_t) ((24* Beilv_ADC_Value)/(ADC_Value.Value_data));
	}
	else 
	{ 
		ADC_Value.Res_Value = 0xfff;
	}
	CanCmd_ReturnQIfa_Res_value(ADC_Value.Num ,ADC_Value.Res_Value);		
	//�رյ�ǰ����
	Qifa_Io_Out_Single(0, ADC_Value.Num);
}

//����ʱ�� ����һ����75us ��������10����720us
void Scan_qifa_ADC(void)
{
	//uint16_t i = 0 ;
	if((ADC_Value.Start_enable == 1 ) && ADC_Value.delaytime )
	{
		ADC_Value.delaytime -- ;		
		if(ADC_Value.delaytime   == 0 )
		{
			ADC_Value.Start_enable = 2 ;
			ADC_Value.Count = Max_ADC_Value ;
		}
	}
	if((ADC_Value.Start_enable == 2) && ( ADC_Value.Count ) )		
	{
		// LedCtr_RUN_ERR(LED_ON);
		 #if 0
		 //������������ 
		for(i = 0 ; i < Max_ADC_Value ; i ++ )
		{
			ADC_Value.Vlaue_buf[ i ] =  VoltageCal();
		}
		ADC_Value.end = 1 ;
		ADC_Value.Start_enable  = 0 ;
		#else
		// ����1ms ����һ��  
		ADC_Value.Vlaue_buf[Max_ADC_Value -ADC_Value.Count ] =  VoltageCal();
		ADC_Value.Count --  ;
		if(ADC_Value.Count == 0)
		{
			ADC_Value.end = 1 ;
			ADC_Value.Start_enable  = 0 ;
		}
		#endif		
		 //LedCtr_RUN_ERR(LED_OFF);
	}
}


