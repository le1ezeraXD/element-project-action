#include "param.h"

/* module include */
#include "mod_init.h"
#include "mod_oled.h"
#include "mod_oled_font.h"
#include "mod_at24cxx.h"
#include "mod_w25qxx.h"
#include "mod_mpu6050.h"

/* system include */
#include "sys.h"
#include "sys_key.h"
#include "sys_encoder.h"

/* driver include */
#include "drv_led.h"
#include "drv_systick.h"
#include "drv_usart1.h"
#include "drv_encoder.h"

/* sdk include */
#include "sdk_menu.h"

/* irq_handler */
#include "irq_handler.h"

//全局设备变量
uint8_t g_mpu6050_data_ready;
key_state_t g_key1, g_key2, g_key3;
tim_encoder_t g_encoder_dev;
mpu6050_data_t mpu6050_data; 

int main(void) {
	
	uint16_t key_sta1;
	uint16_t key_sta2;
	uint16_t w25qxx_chipid = 0;
	uint8_t mpu_6050_id = 0;

	init_system();
	
	module_init();

	oled_clear();
	menu_init();
	oled_update();

	while(1) {
		key_sta1 = sys_key_mes_disp(&g_key1);
		key_sta2 = sys_key_mes_disp(&g_key2);

		switch(key_sta1){
			case KEY_DOWN:
				led_control(GPIOB, gpio_pin_5, LED_ON);
				menu_enter();
				oled_update();
				// g_encoder_dev.encoder_reset = 1;
				break;
			case KEY_UP:
				led_control(GPIOB, gpio_pin_5, LED_OFF);
				break;
			case KEY_LONG_PRESS:
				menu_back();
				oled_update();
				// led_control(GPIOB, gpio_pin_5, LED_ON);
				// g_encoder_dev.encoder_reset = 0;
				// TIM_Cmd(TIM3,ENABLE);
				break;
			default:
				// menu_encoder_process();
				oled_update();
				break;
		}
	}
}

