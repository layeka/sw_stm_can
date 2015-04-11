/**
  ******************************************************************************
  * @file    Project/STM32F0xx_StdPeriph_Templates/main.h
  * @author  MCD Application Team
  * @version V1.3.1
  * @date    17-January-2014
  * @brief   Header for main.c module
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx.h"
#include "boot.h"


#define   TRUE      1
#define   FALSE     0

//#define STM32F042C4
//#if  !defined (STM32F042C4)
//#define   APP_ADDREND             ((uint32_t)0x08008000)       //STM32F042C6      APP羲宎窒煦華硊
//#else
//#define   APP_ADDREND             ((uint32_t)0x08004000)       //STM32F042C4      APP羲宎窒煦華硊
//#endif

#define   PROENDMARK              ((uint32_t)(APP_ADDREND-4))     //程序结束标志

#define   POWERUP                 *((uint32_t*)0x200017f0)     //切换标志位地址


void Timer14ini(void);
void Timer1ini(void);
void IWDG_Config(void);
//void delay1xms(uint16_t dly);

void ScanAlarm(void);
//void Qifa_state_Prainit(void);
//extern const uint8_t Ver[40];
extern __IO uint32_t Powerup;
extern uint32_t Flash_Device_ID_STM;                   //STM32 唯一ID


/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
