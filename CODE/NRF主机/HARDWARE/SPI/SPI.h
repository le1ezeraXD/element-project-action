#ifndef _SPI_H
#define _SPI_H
#include "sys.h"

void SPI2_SetSpeed(u8 SPI_BaudRatePrescaler);
u8 SPI2_Send_receive_Byte(u8 Byte);
void SPI2_Init(void);


#endif


