/*************************************************************
io_output.h
***************************************************************/
#include "stm32f0xx.h"
#include "type.h"

#define  Output0_PORT   GPIOB
#define  Output0_PIN    GPIO_Pin_1
#define  Output0_defval  0

#define  Output1_PORT   GPIOB
#define  Output1_PIN    GPIO_Pin_2
#define  Output1_defval  0

#define  Output2_PORT   GPIOB
#define  Output2_PIN    GPIO_Pin_10
#define  Output2_defval  0

#define  Output3_PORT   GPIOB
#define  Output3_PIN    GPIO_Pin_11
#define  Output3_defval  0


//LAMP
#define  Output4_PORT   GPIOB
#define  Output4_PIN    GPIO_Pin_8
#define  Output4_defval  0


#define Max_Output_Single 	5

typedef struct {
  GPIO_TypeDef* port;
  unsigned int bit;
  bool defval;
  bool xor;
}OUTPUTGPIOGROUP;


extern uint32 outputval;

extern const OUTPUTGPIOGROUP  Output_Single[Max_Output_Single];

extern void outputInit(void);
extern void outputDrive(uint32 ioutput, bool val);


