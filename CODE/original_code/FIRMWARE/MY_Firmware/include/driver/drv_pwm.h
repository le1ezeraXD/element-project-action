#ifndef __DRV_PWM_H
#define __DRV_PWM_H
#include "sys.h"


void drv_tim8_pwm_init(uint16_t arr, uint16_t psc);

void drv_tim8_set_duty(uint8_t channel, uint16_t duty);

void pwm_init(uint8_t init_angle);
void pwm_set_compare1(uint16_t compare);
void pwm_set_compare2(uint16_t compare);
void pwm_set_compare3(uint16_t compare);
void pwm_set_compare4(uint16_t compare);
#endif


