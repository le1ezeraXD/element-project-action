#ifndef __DRV_LED_H
#define __DRV_LED_H
#include "sys.h"

// LED引脚定义
#define LED1_PIN     5
#define LED1_PORT    GPIOB


//LED状态枚举
typedef enum{
	LED_OFF = 0,
	LED_ON = 1
}led_state;

void drv_led_init(void);

void led_control(GPIO_TypeDef* GPIOx, uint8_t gpio_pin_x, led_state state);

#endif

