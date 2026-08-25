#include "drv_led.h"



/**
  * @brief  LED的初始化
  * @param  None
  * @retval None
  */
void drv_led_init(void) {
	
#if MY_DRIVER
	
	//使能GPIOB的时钟  使能APB2下的GPIOB
	RCC->APB2ENR |= (1<<3);
	
	//设置为推挽输出
	LED1_PORT->CRL &= ~(0x0F << (LED1_PIN * 4));
	LED1_PORT->CRL |= (0x03 << (LED1_PIN * 4));
	
	LED1_PORT->ODR |= (1 << LED1_PIN);

#else
	
	GPIO_InitTypeDef GPIO_InitStruct;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(LED1_PORT,&GPIO_InitStruct);
	
  GPIO_SetBits(GPIOB, GPIO_Pin_5);
//	GPIO_ResetBits(GPIOB, GPIO_Pin_5);
	
#endif
	
}

/**
  * @brief  LED状态控制
  * @param  LED状态
  * @retval None
  */
void led_control(GPIO_TypeDef* GPIOx, uint8_t gpio_pin_x, led_state state) {
    if(state == LED_ON) {
        GPIOx->ODR &= ~(1 << gpio_pin_x);
    } 
		else {
        GPIOx->ODR |= (1 << gpio_pin_x);
    }
}


