/*************************************************************
io_input.h
***************************************************************/
#include "stm32f0xx.h"


//////////////datie
#define  Input0_PORT   GPIOB
#define  Input0_PIN    GPIO_Pin_15

#define  Input1_PORT   GPIOB
#define  Input1_PIN    GPIO_Pin_14

#define  Input2_PORT   GPIOB
#define  Input2_PIN    GPIO_Pin_13

#define  Input3_PORT   GPIOB
#define  Input3_PIN    GPIO_Pin_12

//////////sig
#define  Input4_PORT   GPIOA
#define  Input4_PIN    GPIO_Pin_3

#define  Input5_PORT   GPIOA
#define  Input5_PIN    GPIO_Pin_4

#define  Input6_PORT   GPIOA
#define  Input6_PIN    GPIO_Pin_5

#define  Input7_PORT   GPIOA
#define  Input7_PIN    GPIO_Pin_6



#define Max_Input_Single 	8
#define INPUT_MASK          ((1UL << Max_Input_Single)-1)

typedef struct __gpiogroup_IoInput{
  GPIO_TypeDef* port;
  unsigned int bit;
}GPIOGROUP_IoInput;



typedef struct _Input_pra_S{
uint32_t IO_Alarmget_Databuf;				////当前状态备份
uint32_t IO_Input_Real;			//实际端口状态

uint32_t REC_Scan_Input_Enable;	//主板设定输入检测使能信号  =1 允许扫描输入 =0 不允许扫描
uint8_t Scan_Input_State;		//当前扫描状态
uint8_t Can_sendup_enable;		//符合状态向上发送 使能标志
}Input_pra_S;



#define IO_SCAN_STAT_NO   0
#define IO_SCAN_STAT_DEBOUNCE  1
#define IO_SCAN_STAT_HOLD      2
#define IO_SCAN_STAT_ALARM     3
#define IO_SCAN_STAT_OK        5
#define IO_SCAN_STAT_ERR       6
#define IO_SCAN_STAT_TEST_BEGIN 4
#define IO_SCAN_STAT_TEST_FINISH 5

#define IO_SCAN_STAT_EN_ALRAM  1
#define IO_SCAN_STAT_EN_TEST   2

#define IO_SCAN_DEBOUNCE_TIMES 30


typedef struct __io_scan{
    unsigned char en;
    unsigned char val;
    unsigned long long timertick;
    unsigned long long begintime;
    unsigned int timelast;
    int debouceCnt;
    int sensitivity;
    int delay;
    unsigned int stat;
}IO_SCAN;


typedef struct __io_scan_scan{
    unsigned char state;
    unsigned char val;
    unsigned char valhold;
    unsigned int count;
}IO_SCAN_SCAN;

extern unsigned int    io_alarm_enable_bitmap ;
extern  IO_SCAN        io_scan[Max_Input_Single];

extern unsigned int    io_bitmap;
//extern  IO_SCAN_SCAN   io_scan_scan[Max_Input_Single];


extern void io_scan_init();
extern Input_pra_S Input_pra;
extern void Io_input_Init(void);
uint8_t Get_Io_Input(uint8_t num);
extern void ScanIOstate(void);
uint32_t Scan_Input_state(void);
extern void ioTesttimeProcess(void);
extern void ioAlarmProcess(void);
extern void ioScan(void);


