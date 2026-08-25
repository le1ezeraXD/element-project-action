#ifndef __IRQ_HANDLER_H
#define __IRQ_HANDLER_H
#include "sys.h"

#define TRAIL_DISPLAY_TIME 3  // πÏº£œ‘ æ3√Î

#define USART1_REC_MAX  200

void trail_timer_start(void);
void trail_timer_stop(void);


extern u8 USART1_REC[USART1_REC_MAX];
extern u16 USART1_STA;

#endif
