#ifndef __DRV_SYSTICK_H
#define __DRV_SYSTICK_H
#include "sys.h"

//systick初始化
void drv_systick_init(void);

//us级别延时函数
void delay_us(uint32_t count);

//ms级别延时函数
void delay_ms(uint32_t count);


#endif

