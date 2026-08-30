#ifndef _ENCODER_H
#define _ENCODER_H
#include "sys.h"

void Encoder_TIM2_Init(void);
void Encoder_TIM4_Init(void);

int Read_Speed(u8 TIMx);

#endif


