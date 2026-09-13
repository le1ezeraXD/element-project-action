#include "irq_handler.h"
/*module*/
#include "mod_oled.h"
#include "mod_mpu6050.h"
/*system*/
#include "sys_key.h"
#include "sys_encoder.h"
/*driver*/
#include "drv_led.h"
/*params*/
#include "param.h"

#include "sdk_menu.h"
#include "sdk_motion.h"


void EXTI4_IRQHandler(void) {
	if(EXTI_GetITStatus(EXTI_Line4) != RESET) {
		uint8_t int_status = 0;
		/*读取中断状态寄存器（会清除MPU6050中断标志）*/
		int_status = mpu6050_get_int_status(I2C2);

		/*检查具体中断源*/
		if(int_status & 0x01){
			//DATA_RDY中断
			g_mpu6050_data_ready = 1;
		}
		mpu_6050_get_gyro_accel(I2C2, &mpu6050_data);
		EXTI_ClearITPendingBit(EXTI_Line4); // 清除中断标志
	}
}

//5ms产生一次中断
void TIM4_IRQHandler(void) {
	//如果TIMx_SR的第0位置一，则说明是更新中断产生
	if(TIM4->SR & 0x01) {	
		//按键扫描
		sys_get_key_state(&g_key1);
		sys_get_key_state(&g_key2);
		// sys_get_key_state(&g_key3);

		// if(g_encoder_dev.encoder_reset) {
		// 	sys_reset_encoder_count(TIM3, &g_encoder_dev);
		// }
		menu_encoder_process();
		motion_tick();
		// oled_update();
		// sys_encoder_speed(TIM3, &g_encoder_dev);

		//清除中断标志位
		TIM4->SR &=~(1<<0);
	}
}

//编码器
void TIM3_IRQHandler(void){
	if(TIM3->SR & 0x01){ //计数器溢出中断
		uint8_t dir = (TIM3->CR1 & 0x10) ? 1 : 0;
		if(dir){  
			// DIR=1, 向下计数
			g_encoder_dev.timx_encoder_cnt--;
		}else{
			// DIR=0, 向上计数
			g_encoder_dev.timx_encoder_cnt++;
		}
		TIM3->SR &=~(1<<0);
	}
}


//用数组来接收数据
uint8_t  USART1_REC[USART1_REC_MAX];

//USART1_STA[15]    标识是否接收完成的状态位   1：表示接收完成  0：接收未完成
//USART1_STA[14]    标识是否为第一个数据      0：是第一个数据  1：不是第一个数据
//USART1_STA[13:0]  标识是实际数据
uint16_t USART1_STA = 0;

/**
  * @brief  usart1 接收数据中断
  * @param  None
  * @retval None
  */
void USART1_IRQHandler(void) {
	
	uint8_t data = 0;
	
	//接收中断产生 RXNE
	if (USART1->SR & 0x20) {
		data = USART1->DR; 
		//判断第一次接收数据
		if(!(USART1_STA & 0x4000)) {  //第十四位  0x4000   0：是第一个数据  1：不是第一个数据
			USART1_STA = 0x4000;      //既清零了数据位 同时清除了状态位 对第十四位赋值 表示下次接收的时候就不在是第一个数据
			memset(USART1_REC,0,USART1_REC_MAX);
		}
	}
	//数据位 [13:0]  清除14位  15位
	USART1_REC[USART1_STA & 0x3FFF] = data;
	USART1_STA++;     //不影响最高位
	
	//满载处理
	if ((USART1_STA & 0x3FFF) >= USART1_REC_MAX) {
		//将要达到满载状态
		USART1_STA = 0;  //覆盖原数据
	}
	
	//总线空闲中断 
	if (USART1->SR & 0x10) {
		USART1_STA |=0x8000;  //当要读取数据的时候，只需要看STA标志位是否为一，如果是一，就表明数据接收完成
		USART1_STA &=~0x4000; //清零第十四位，表示下次接收是第一次接受
		//为了标识整个字符串的有效长度
		USART1_REC[USART1_STA & 0x3FFF] = '\0';  //保证有效的字符串
		data = USART1->DR;  //对DR寄存器进行读操作 如果RXNE被置一，保证总线空闲的状态
	}
}


#if 0
//轨迹清除定时器
uint8_t trail_timer_active = 0;
uint8_t trail_timer_count = 0;


/**
  * @brief  轨迹清除定时器启动
  * @param  None
  * @retval None
  */
void trail_timer_start(void) {
    trail_timer_active = 1;
    trail_timer_count = 0;
}

/**
  * @brief  轨迹清除定时器停止
  * @param  None
  * @retval None
  */
void trail_timer_stop(void) {
    trail_timer_active = 0;
    trail_timer_count = 0;
}

/**
  * @brief  轨迹清除定时器处理
  * @param  None
  * @retval None
  */
void trail_timer_handler(void) {
    if (trail_timer_active) {
				trail_timer_count++;
        if (trail_timer_count >= TRAIL_DISPLAY_TIME) {
            //3s 清除轨迹
            oled_clear_trail();
            trail_timer_stop();
        }
    }
}
#endif

void TIM2_IRQHandler(void) {
#if MY_DRIVER
	//如果TIMx_SR的第0位置一，则说明是更新中断产生
	if(TIM2->SR & 0x01)
	{	
		//清除中断标志位
		TIM2->SR &=~(1<<0);
	}
#else
	if(TIM_GetITStatus(TIM2,TIM_FLAG_Update) != RESET)
	{
		//清除中断标志位
		TIM_ClearITPendingBit(TIM2,TIM_FLAG_Update);
	
	}
#endif
}

