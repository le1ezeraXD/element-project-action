#ifndef __SYS_ENCODER_H
#define __SYS_ENCODER_H
#include "sys.h"

typedef enum {
    encoder_none,
    encoder_cw,     //顺时针
    encoder_ccw     //逆时针
}encoder_dir;


int32_t sys_get_encoder_count(TIM_TypeDef* TIMx, tim_encoder_t* encoder_dev);

void sys_reset_encoder_count(TIM_TypeDef* TIMx, tim_encoder_t* encoder_dev);

void sys_encoder_init(tim_encoder_t* encoder_dev, uint32_t accuracy, uint16_t timx_irq, uint8_t multi);

int32_t sys_encoder_speed(TIM_TypeDef* TIMx, tim_encoder_t* encoder_dev);

int32_t sys_get_encoder_diff(TIM_TypeDef* TIMx, tim_encoder_t* encoder_dev);


#endif
