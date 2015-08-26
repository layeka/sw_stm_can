
#include "can.h"
#include "main.h"
#include "anniuled.h"



#define CAN_FILTER_IDX_DATA    0X04
#define CAN_MASK_IDX_DATA      0X06



void Canini(void) {
    unsigned int mask;
    unsigned int filter;
    CAN_InitTypeDef CAN_InitStructure;
    CAN_FilterInitTypeDef CAN_FilterInitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
    /* GPIOA Configuration:  */
    GPIO_InitStructure.GPIO_Pin = CAN_TX_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(CAN_TX_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = CAN_RX_PIN;
    GPIO_Init(CAN_RX_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(CAN_TX_PORT, CAN_TX_SOUCE, GPIO_AF_4);
    GPIO_PinAFConfig(CAN_RX_PORT, CAN_RX_SOUCE, GPIO_AF_4);
    ////////////////////////////////////////////////////
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN, ENABLE);
    CAN_DeInit(CAN);                   //所有寄存器恢复到默认
                                       ///////////////////////////////////////////////////
                                       //CAN_StructInit(&CAN_InitStructure);
    /* CAN cell init */
    CAN_InitStructure.CAN_TTCM = DISABLE;
    CAN_InitStructure.CAN_ABOM = ENABLE;
    CAN_InitStructure.CAN_AWUM = DISABLE;
    CAN_InitStructure.CAN_NART = DISABLE;
    CAN_InitStructure.CAN_RFLM = DISABLE;
    CAN_InitStructure.CAN_TXFP = DISABLE;
    CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;

    //CAN_InitStructure.CAN_Mode = CAN_Mode_LoopBack;
    CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;

    // CAN Baudrate =     1Mbps (CAN clocked at 48 MHz) //
    //bittim=CAN_BS1+1+CAN_BS2+1+1=CAN_BS1+CAN_BS2+3
    //Baudrate=CLK/((CAN_BS1+CAN_BS2+3)*(BRP+1))
    CAN_InitStructure.CAN_BS1 = CAN_BS1_8tq;    //
    CAN_InitStructure.CAN_BS2 = CAN_BS2_7tq;
    CAN_InitStructure.CAN_Prescaler = 6;
    CAN_Init(CAN, &CAN_InitStructure);

    //////////////////////////////////////////////////////////////////
    //dmp protocal globle broadcast
    CAN_FilterInitStructure.CAN_FilterNumber = 0;
    CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
    CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
    mask = (0x1fffffff << 3) | CAN_MASK_IDX_DATA;
    filter = (Management_global_broadcast << 3) | CAN_FILTER_IDX_DATA;
    CAN_FilterInitStructure.CAN_FilterIdHigh = (uint16_t)(filter >> 16);
    CAN_FilterInitStructure.CAN_FilterIdLow = (uint16_t)filter;
    CAN_FilterInitStructure.CAN_FilterMaskIdHigh = (uint16_t)(mask >> 16); //0x3fff;
    CAN_FilterInitStructure.CAN_FilterMaskIdLow = (uint16_t)mask;

    CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FIFO0;
    CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
    CAN_FilterInit(&CAN_FilterInitStructure);
    //dmp FLASHID filter
    CAN_FilterInitStructure.CAN_FilterNumber = 1;
    CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
    CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
    mask = (0x1fffffff << 3) | CAN_MASK_IDX_DATA;
    filter = (Flash_Device_ID_STM << 3) | CAN_FILTER_IDX_DATA;
    CAN_FilterInitStructure.CAN_FilterIdHigh = (uint16_t)(filter >> 16);
    CAN_FilterInitStructure.CAN_FilterIdLow = (uint16_t)filter;
    CAN_FilterInitStructure.CAN_FilterMaskIdHigh = (uint16_t)(mask >> 16); //0x3fff;
    CAN_FilterInitStructure.CAN_FilterMaskIdLow = (uint16_t)mask;
    CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FIFO0;
    CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
    CAN_FilterInit(&CAN_FilterInitStructure);

    CAN_ITConfig(CAN, CAN_IT_FMP0 | CAN_IT_EPV | CAN_IT_EWG | CAN_IT_ERR | CAN_IT_BOF, ENABLE);
    /* Enable CAN RX0 interrupt IRQ channel */
    NVIC_InitStructure.NVIC_IRQChannel = CEC_CAN_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

}


void canAddfilter(unsigned int filter, unsigned int mask, unsigned int filternum) {
    uint32_t filter_number_bit_pos;
    filter_number_bit_pos = ((uint32_t)1) << filternum;

    /* Initialisation mode for the filter */
    CAN->FMR |= 0x00000001;
    /* Filter Deactivation */
    CAN->FA1R &= ~(uint32_t)filter_number_bit_pos;

    /* 32-bit scale for the filter */
    CAN->FS1R |= filter_number_bit_pos;
    /* 32-bit identifier or First 32-bit identifier */
    CAN->sFilterRegister[filternum].FR1 = filter;

    /* 32-bit mask or Second 32-bit identifier */
    CAN->sFilterRegister[filternum].FR2 = mask;

    /*Id/Mask mode for the filter*/
    CAN->FM1R &= ~(uint32_t)filter_number_bit_pos; //set 0 ,mask mode

    /* FIFO 0 assignation for the filter */
    CAN->FFA1R &= ~(uint32_t)filter_number_bit_pos;

    /* Filter activation */
    CAN->FA1R |= filter_number_bit_pos;

    /* Leave the initialisation mode for the filter */
    CAN->FMR &= ~0x00000001;
}

void canClearfilter(unsigned int filternumbitmap) {
    /* Initialisation mode for the filter */
    CAN->FMR |= 0x00000001;
    /* Filter Deactivation */
    CAN->FA1R &= ~filternumbitmap;
    /* Leave the initialisation mode for the filter */
    CAN->FMR &= ~0x00000001;
}






uint8_t CanSend(CanTxMsg *msg) {
    uint16_t i;
    uint8_t TransmitMailbox = 0;
    TransmitMailbox = CAN_Transmit(CAN, msg);
    i = 0;
    while ((CAN_TransmitStatus(CAN, TransmitMailbox) != CANTXOK) && (i != 0xFFFF)) {
        i++;
    }
    if (i > 0xfff0) {
        return 1;
    }
    ledCanTxRxON();
    return 0;
}

static void (*canrcvhandler)(CanRxMsg *msg) = NULL;

uint8  canBOFs = 0 ;

void CEC_CAN_IRQHandler(void) {
    if (CAN_GetFlagStatus(CAN, CAN_FLAG_FMP0)) {
        LedCtr_CAN_WORK(LED_ON);
        CanRxMsg msg;
        ledCanTxRxON();
        CAN_Receive(CAN, CAN_FIFO0, &msg);
        if (canrcvhandler != NULL) {
            canrcvhandler(&msg);
        }
    }
    //错误处理 +++++++++++++++++++++++++++++++++++++++++
    //++++++++++++++++++++++++++++++++++++++++++++++++++++
    if (CAN_GetITStatus(CAN, CAN_IT_ERR)) {
        if (CAN_GetFlagStatus(CAN, CAN_FLAG_BOF)) {               //掉线
            //CAN_ClearITPendingBit(CAN, CAN_FLAG_BOF);
            canBOFs++;

        }
        CAN_ClearITPendingBit(CAN, CAN_IT_ERR);

        /*if (CAN_GetFlagStatus(CAN, CAN_FLAG_EWG)) {               //各种错
            CAN_ClearITPendingBit(CAN, CAN_FLAG_EWG);
        }

        if (CAN_GetFlagStatus(CAN, CAN_FLAG_LEC)) {               //掉线
            CAN_ClearITPendingBit(CAN, CAN_FLAG_LEC);
        }
        if (CAN_GetFlagStatus(CAN, CAN_FLAG_EPV)) {               //错误次数超出
            CAN_ClearITPendingBit(CAN, CAN_FLAG_EPV);
        }*/
    }
}


void canRegistRcvHandler(void (*handler)(CanRxMsg *msg)) {
    canrcvhandler = handler;
}






