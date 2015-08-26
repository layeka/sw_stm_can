#include "stm32f0xx.h"
#include "main.h"
#include "can_dmp.h"

#define CAN_TX_PORT   GPIOA
#define CAN_TX_PIN    GPIO_Pin_12
#define CAN_TX_SOUCE   GPIO_PinSource12

#define CAN_RX_PORT   GPIOA
#define CAN_RX_PIN    GPIO_Pin_11
#define CAN_RX_SOUCE   GPIO_PinSource11


#define Management_global_broadcast           0x07ffffff//管理协议下全局广播management protocol  broadcast

extern  uint8  canBOFs;

extern uint8_t CanSend(CanTxMsg *msg);

extern void Canini(void);
extern void canAddfilter(unsigned int filter,unsigned int mask,unsigned int filternum);
extern void canClearfilter(unsigned int filternumbitmap);
extern void canRegistRcvHandler(void (*handler)(CanRxMsg *msg));
