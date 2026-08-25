#ifndef __DRV_USART1_H
#define __DRV_USART1_H
#include "sys.h"


#define USART1_REC_MAX  200

//初始化USART1
void drv_usart1_Init(uint32_t bound);
//串口数据发送函数
void usart1_send_data(const char *data, uint8_t len);



extern u8 USART1_REC[USART1_REC_MAX];
extern u16 USART1_STA;

#endif


