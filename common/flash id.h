/**************************************************************8
GET FLASH ID 
***************************************************************/

#include "stm32f0xx.h"




#define STM32F0x_FLASHID_READ    1


#define STM32F0X_FLASHID_ADDRESS ((uint32_t)0x1FFFF7AC)    	


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++



extern uint32_t Get_Flash_ID(void);
uint16_t Get_CRC16(uint8_t *dat , uint16_t  len);



