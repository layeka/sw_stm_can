#include "stm32f0xx.h"

unsigned int CRC_32(unsigned int *buf,unsigned int len_32){
    CRC_DeInit();
    return CRC_CalcBlockCRC(buf, len_32);
}