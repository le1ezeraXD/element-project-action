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
#include "sys_i2c.h"
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

/*passthrough*/
#define PASSTHROUGH_RX_MAX 200

extern uint8_t USART1_REC[USART1_REC_MAX];
extern uint16_t USART1_STA;

void usart1_send_data_hex(uint8_t arr[], uint16_t len) {
	for (uint16_t i = 0; i < len; i++) {
		LOG_DEBUG("arr[%d]: 0x%x", i, arr[i]);
	}
}

// frame format [W][cmd*W][R] W=写命令字节数(≥1)；R=期望读回字节数(0=只写)
void passthrough(void) {
	//PROJ_FUNC_ENTER;
	int8_t ret = 0;
	uint8_t w_len = 0;
	uint8_t r_len = 0;
	uint16_t len = USART1_STA & 0x3FFF;
	static uint8_t cmd_buf[PASSTHROUGH_RX_MAX] = {0};   //cmd copy
	static uint8_t rx_buf[PASSTHROUGH_RX_MAX] = {0};	//recv data
	uint8_t err[] = {0xFF, 0xFF};
	uint8_t ack[] = {0x00};

	if (!(USART1_STA & 0X8000)) {  //not full frame
		//LOG_DEBUG("not full frame");
		return;
	}

	//parsing frame : [W][cmd*W][R]
	if (len < 3) { //empty frame
		USART1_STA = 0;
		return;
	}

	w_len = USART1_REC[0];
	r_len = USART1_REC[len - 1];
	if ((w_len == 0) || ((w_len + 2) != len) || (r_len > PASSTHROUGH_RX_MAX)) {
		//usart1_send_data(err, 2);
		LOG_ERROR("cmd error!!!");
		usart1_send_data_hex(err, 2);
		USART1_STA = 0;
		return;
	}
	
	memcpy(cmd_buf, &USART1_REC[1], w_len);  //copy,prevent receiving interruptions from being overwritten
	USART1_STA = 0;  //allow recv
	
	for (uint8_t i = 0; i < w_len; i++) {
		LOG_DEBUG("w_len:%d, cmd[%d]:%x", w_len, i, cmd_buf[i]);
	}

	// block gpio irq
	NVIC_DisableIRQ(EXTI4_IRQn);

	i2c_device hyn_dev = {
		.dev_addr_w = 0x70,				//test addr,actual usage substitution
		.dev_addr_r = 0x70 | 0x01,		//test addr,actual usage substitution
		.cmd = cmd_buf,
		.cmd_len = w_len,
		.pdata = rx_buf,
		.data_len = r_len,
	};

	if (r_len == 0) { //only write
		ret = sys_i2c_write(I2C1, &hyn_dev);
	}
	else {
		ret = sys_i2c_write_read(I2C1, &hyn_dev);
	}

	NVIC_EnableIRQ(EXTI4_IRQn);

	if (ret) {
		usart1_send_data_hex(err, 2);
	}
	else if (r_len == 0) {
		usart1_send_data_hex(ack, 1);
	}
	else { // callback read data
		usart1_send_data_hex(rx_buf, r_len);
	}
	
	//PROJ_FUNC_EXIT;
}


int main(void) {
	
	uint16_t key_sta1;
//	uint16_t key_sta2;
//	uint16_t w25qxx_chipid = 0;
//	uint8_t mpu_6050_id = 0;

	init_system();
	
	module_init();

	oled_clear();
	menu_init();
	oled_update();

	while(1) {
		key_sta1 = sys_key_mes_disp(&g_key1);
//		key_sta2 = sys_key_mes_disp(&g_key2);

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

