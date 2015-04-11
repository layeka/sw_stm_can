/*************************************************************
io_output.h
***************************************************************/
#include "stm32f0xx.h"
#include "type.h"

#define  Output0_PORT   GPIOA
#define  Output0_PIN    GPIO_Pin_7

#define  Output1_PORT   GPIOB
#define  Output1_PIN    GPIO_Pin_0

#define  Output2_PORT   GPIOB
#define  Output2_PIN    GPIO_Pin_1

#define  Output3_PORT   GPIOB
#define  Output3_PIN    GPIO_Pin_2

#define  Output4_PORT   GPIOA
#define  Output4_PIN    GPIO_Pin_5


#define  Output5_PORT   GPIOA
#define  Output5_PIN    GPIO_Pin_6

#define  Output6_PORT   GPIOB
#define  Output6_PIN    GPIO_Pin_10

#define  Output7_PORT   GPIOB
#define  Output7_PIN    GPIO_Pin_11

#define  Output8_PORT   GPIOB
#define  Output8_PIN    GPIO_Pin_9

#define  Output9_PORT   GPIOA
#define  Output9_PIN    GPIO_Pin_4

#define  Output10_PORT   GPIOB
#define  Output10_PIN    GPIO_Pin_13

#define  Output11_PORT   GPIOB
#define  Output11_PIN    GPIO_Pin_14

#define  Output12_PORT   GPIOB
#define  Output12_PIN    GPIO_Pin_6

#define  Output13_PORT   GPIOB
#define  Output13_PIN    GPIO_Pin_7

#define  Output14_PORT   GPIOB
#define  Output14_PIN    GPIO_Pin_15

#define  Output15_PORT   GPIOA
#define  Output15_PIN    GPIO_Pin_8
//������λ��  ΪH ʱ��λ ΪLʱ��������״̬
#define Output_QF_RST_PORT 	GPIOB
#define Output_QF_RST_PIN	GPIO_Pin_5
//����ʹ�ܽ�  ΪH ʱƬѡ��Ч ΪLʱƬѡ��Ч��������״̬
#define Output_QF_EN_PORT 	GPIOB
#define Output_QF_EN_PIN	GPIO_Pin_4




#define Max_Output_Single 	16

typedef struct __gpiogroup{
  GPIO_TypeDef* port;
  unsigned int bit;
}GPIOGROUP;
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//�������� �������󱨾��ź�


#define Max_Input_Single 	4

//���������źŽ�  INPUT1
#define Input_QFERR1_PORT 	GPIOB
#define Input_QF_ERR1_PIN	GPIO_Pin_3
//���������źŽ�  INPUT2
#define Input_QFERR2_PORT 	GPIOA
#define Input_QF_ERR2_PIN	GPIO_Pin_1
//���������źŽ�  INPUT3
#define Input_QFERR3_PORT 	GPIOA
#define Input_QF_ERR3_PIN	GPIO_Pin_2
//���������źŽ�  INPUT4
#define Input_QFERR4_PORT 	GPIOA
#define Input_QF_ERR4_PIN	GPIO_Pin_3

typedef struct __gpiogroup_IoInput{
  GPIO_TypeDef* port;
  unsigned int bit;
}GPIOGROUP_IoInput;

extern const GPIOGROUP  Output_Single[Max_Output_Single];
extern const GPIOGROUP_IoInput  Input_Single[Max_Input_Single];


#define   QIFAPWMHZ   		18900
#define   QIFAPWMVALUE   	65
#define   MAX_PWM_Close_Time	 1000     // 1000 * 1MS  �������ʱ ���ܹر�ʱ�� 1S ��   ������������Ч �����Զ���



typedef struct __S_qifa_state{

  //uint8_t  InputDataBuf[15];      //�����źŻ���
   uint16_t fg_qifa_out;			//������� ʱ��״̬  BIT Ϊ 1��ʾ������� Ϊ0  ��ʾ����û�����
  //uint8_t  DeviceErr;		    // ������������

  //uint8_t  alarmbitmap;		    // ����˲�

  uint8_t  Out_PWM_Enable ;		    //���������Ƿ���Ч    �����·�����
  uint16_t PWM_Close_Time_Count;    //������Ч������

}S_qifa_state;


extern S_qifa_state   Qifa_state;


void Swqf_Io_Init(void);
void swqf_io_out(uint8_t num,uint8_t value);


uint8_t Get_Io_Input(uint8_t num);

uint8_t Scan_Input_state(void);

extern void QifaEnCtr(uint8_t  value);
extern void QifaRstCtr(uint8_t  value);
extern void QifaRstProcess(void);
extern void QifaPwmCtr(uint8_t  value);
extern void Qifa_Io_Out_Single(uint8_t onoff,  uint8_t num);
extern void Qifa_state_Prainit(void);
extern void QifaDrive(uint8 onoff, uint8 iqifa);

