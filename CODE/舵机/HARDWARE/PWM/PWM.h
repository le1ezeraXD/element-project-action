#ifndef __PWM_H
#define __PWM_H
#include "sys.h"

#define In1 PAout(4)
#define In2 PAout(5)
#define In3 PAout(6)
#define In4 PAout(7)


void TIM4_PWM_Init(u16 arr,u16 psc);
void qianjin(void);
void houtui(void);
void zuozhuan(void);
void youzhuan(void);
void stop(void);
void diaotou(void);

#endif
