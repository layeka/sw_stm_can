#include "stm32f0xx.h"
#include "algorithm.h"


#define  MiDuMotoNum         4

#define  Moto1_REF_PORT    GPIOA
#define  Moto1_REF_PIN     GPIO_Pin_0
#define  Moto1_FREE_PORT   GPIOA
#define  Moto1_FREE_PIN    GPIO_Pin_9
#define  Moto1_DIR_PORT    GPIOA
#define  Moto1_DIR_PIN     GPIO_Pin_10
#define  Moto1_STEP_PORT   GPIOA
#define  Moto1_STEP_PIN    GPIO_Pin_8
#define  Moto1_ZERO_PORT   GPIOB
#define  Moto1_ZERO_PIN    GPIO_Pin_5
#define  Moto1_ADVALUE_PORT	GPIOA
#define  Moto1_ADVALUE_PIN     GPIO_Pin_4

#define  Moto2_REF_PORT    GPIOA
#define  Moto2_REF_PIN     GPIO_Pin_1
#define  Moto2_FREE_PORT   GPIOB
#define  Moto2_FREE_PIN    GPIO_Pin_10
#define  Moto2_DIR_PORT    GPIOB
#define  Moto2_DIR_PIN     GPIO_Pin_1
#define  Moto2_STEP_PORT   GPIOB
#define  Moto2_STEP_PIN    GPIO_Pin_2
#define  Moto2_ZERO_PORT   GPIOB
#define  Moto2_ZERO_PIN    GPIO_Pin_4
#define  Moto2_ADVALUE_PORT	GPIOA
#define  Moto2_ADVALUE_PIN     GPIO_Pin_5

#define  Moto3_REF_PORT   GPIOA
#define  Moto3_REF_PIN    GPIO_Pin_2
#define  Moto3_FREE_PORT   GPIOB
#define  Moto3_FREE_PIN    GPIO_Pin_15
#define  Moto3_DIR_PORT   GPIOB
#define  Moto3_DIR_PIN    GPIO_Pin_13
#define  Moto3_STEP_PORT   GPIOB
#define  Moto3_STEP_PIN    GPIO_Pin_14
#define  Moto3_ZERO_PORT   GPIOB
#define  Moto3_ZERO_PIN    GPIO_Pin_3
#define  Moto3_ADVALUE_PORT	GPIOA
#define  Moto3_ADVALUE_PIN     GPIO_Pin_6

#define  Moto4_REF_PORT   GPIOA
#define  Moto4_REF_PIN    GPIO_Pin_3
#define  Moto4_FREE_PORT   GPIOB
#define  Moto4_FREE_PIN    GPIO_Pin_9
#define  Moto4_DIR_PORT   GPIOB
#define  Moto4_DIR_PIN    GPIO_Pin_7
#define  Moto4_STEP_PORT   GPIOB
#define  Moto4_STEP_PIN    GPIO_Pin_8
#define  Moto4_ZERO_PORT   GPIOA
#define  Moto4_ZERO_PIN    GPIO_Pin_15
#define  Moto4_ADVALUE_PORT	GPIOA
#define  Moto4_ADVALUE_PIN     GPIO_Pin_7


#define  MIDUMOTO1          0
#define  MIDUMOTO2          1
#define  MIDUMOTO3          2
#define  MIDUMOTO4          3

#define  OUT_UP              1
#define  OUT_DOWN            0

#define  OUT_DIR_ZHENG        1
#define  OUT_DIR_FAN          0


#define  MotoSpeedAddTabNUM    21    //�ܶȵ��15������
#define  MIDUMOTOHZMAX         2200
#define  MIDUMOTOBEGINHZ        500

#define  MOTOSTOPFERR              0
#define  MOTOWORKING               1
#define  MOTOSTOPHALF              2
#define  MOTOSTOPPAUSE             3


#define  DUMUMOTOSTOPHZ            25

#define  DUMUCLKDIV                16

typedef struct __midustepctrpra{

  uint8_t    MotoRunState;          //  0��ʾ�������ͷ�״̬
                                    //  1   ��ʾȫ������״̬
                                    //  2��ʾ��������״̬
                                    //  3��ʾ��ͣ״̬
  uint8_t    fg_clear;			    //��ﵽ����λʱ �Ƿ���Ҫ����PLUS
  uint8_t    MotoRunDir;            //��﷽��
  uint8_t    shiji_zero_zhuangtai;	//ʵ����λ״̬
  uint8_t    MotoZeroState[3];      // ����3�ε������λ��0���ϴ���λ״̬��1�:�ϴΣ�2��ǰ��λ״̬
  uint8_t    MotoStopData[3];       //ֹͣ���� ��MotoZeroState��Ӧ
                                    //  MotoStopData��MotoZeroState ���ʱֹͣ������PULE��DANGQIANDUMU������
                                    //��MotoStopData�е����ݲ���0Ҳ����1ʱ����ʾ������λ״����Ь
  uint8_t    MotoPwmPinState ;      // PWM�����״̬
  uint16_t   MotoRunSetHz;          //����������Ƶ�ʣ���λHZ
  uint16_t   MotoRunBeginHz;        //����������Ƶ�ʣ�����Ƶ�ʣ���λHZ
  uint16_t   MotoRunHz;             //������е�ǰƵ�ʣ� ��λHZ
  uint32_t   MotoRunCmpValue;       //�жϱȽ� ֵ
  int16_t    MotoDumuPuls;          //��Ҫ���е������� * 2
  int16_t    MotoDumuDangqian;      //��ﵱǰ����
  uint16_t   MotoDumuAddTab[MotoSpeedAddTabNUM];  //�������Ƶ�ʱ�

  uint32_t   stepTimerChanel;

}MIDUSTEPCTRPRA;


#define MOTOR_CMD_STAT_WAITE   0
#define MOTOR_CMD_STAT_BUSY    1
#define MOTOR_CMD_STAT_FINISH  2

typedef struct {
    uint8_t  stat;
    uint8_t  dir;
    uint8_t  stopflag;
    uint8_t __dummy;
    uint16_t puls;
    uint16_t speedHz;
    uint64_t timertick;
}MOTOR_CMD;




typedef struct __gpiogroup{
  GPIO_TypeDef* port;
  unsigned int bit;
}GPIOGROUP;


extern RINGBUF motorcmdbuf[MiDuMotoNum];


extern uint32_t timerChanel2Motor(uint32_t);
extern void ResetDumu(uint8_t moto,uint8_t cmd);
extern void Timer1ini(void);
extern uint32_t GetMotoCmpValue(uint16_t MotorunHz);
void PinOutini(void);
extern uint8_t GetMiduMotoZero(uint8_t Moto);
extern void MiduMotoStepCtr(uint8_t Moto,uint8_t NewState);
extern void MiduMotoDirCtr(uint8_t Moto,uint8_t NewState);
extern void MiduMotoFreeCtr(uint8_t Moto,uint8_t NewState);
extern void MiduMotoRefCtr(uint8_t Moto,uint8_t NewState);
extern void GetMotoAddValue(uint8_t  Moto);
extern void MotoDuMuInit(void);
extern void GetMotoDumuStopData(uint8_t Moto,uint8_t  Stop);
extern void MotoDumuRunScan(uint8_t Moto);
extern void MotoDumuRun(uint8_t Moto,uint8_t Dir,uint16_t Plus,uint8_t Stop ,uint16_t HzMax);
extern void GetMotoAddTab(uint8_t  Moto,uint16_t MaxHz);
extern MIDUSTEPCTRPRA   MiduMotoCtrPra[MiDuMotoNum];

