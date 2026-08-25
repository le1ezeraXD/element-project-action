#include "sys.h"
#include "sys_key.h"
#include "sys_encoder.h"

/* DRIVER */
#include "drv_led.h"
#include "drv_key.h"
#include "drv_systick.h"
#include "drv_usart1.h"
#include "drv_i2c.h"
#include "drv_spi.h"
#include "drv_exti.h"
#include "drv_timer.h"
#include "drv_pwm.h"
#include "drv_capture.h"
#include "drv_encoder.h"

void init_system(void) {

//	drv_led_init();     					//led
	drv_key_init();							//key
	
//	drv_exti_init();          				//exti
	drv_systick_init(); 					//systick
	drv_usart1_Init(115200);  				//usart1
	
//	sys_key_init(&g_key1, KEY_ACTIVE_HIGH, read_key1);  // key PA0: 下拉，按下为1
	sys_key_init(&g_key1, KEY_ACTIVE_LOW, read_key1);  // key PA0: 下拉，按下为1
	sys_key_init(&g_key2, KEY_ACTIVE_LOW, read_key2);  	// key PE3: 上拉，按下为0 for encoder
	
	//tim
	drv_tim4_init(5000,72);							//timer4 5ms的时间基准 用作任务处理(编码器任务处理)
	
	//i2c & spi
	drv_i2c1_init();										//i2c1
	drv_i2c2_init();										//i2c2
	drv_spi2_init();										//spi2
	
	//pwm & capture
//	drv_tim8_pwm_init(1000,20);					//pwm
//	drv_tim1_capture_init(0xFFFF,72);			//capture
	
	//	drv_tim2_init(10000,7200);  				//timer2 1s的时间基准

	//encoder
	sys_encoder_init(&g_encoder_dev, 20, 5, 4);  	//rotary_encoder
	drv_tim3_encoder_init(0xFFFF,1);				//tim3_encoder
}



