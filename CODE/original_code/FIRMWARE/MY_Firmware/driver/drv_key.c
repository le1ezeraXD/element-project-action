#include "drv_key.h"



/**
  * @brief  按键key的初始化
  * @param  None
  * @retval None
  */
void drv_key_init(void) {
#if MY_DRIVER
	//使能GPIOA & GPIOE的时钟 
	RCC->APB2ENR |= (1<<2);  //GPIOA
	RCC->APB2ENR |= (1<<6);  //GPIOE
	
	//设置下拉输入
	GPIOA->CRL &=~(0x0F<<0);  //PA0
	GPIOA->CRL |= (0x08<<0);
	
	//设置上拉输入
	GPIOE->CRL &=~(0x0F<<12);  //PE3
	GPIOE->CRL |= (0x08<<12);
	GPIOE->CRL &=~(0x0F<<16);  //PE4
	GPIOE->CRL |= (0x08<<16);
	
	//配置下拉
	GPIOA->ODR &= ~(1<<0);
	
	//配置上拉
	GPIOE->ODR |= (1<<3);
	GPIOE->ODR |= (1<<4);
#else

	GPIO_InitTypeDef GPIO_InitStruct;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOE, ENABLE);
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOE,&GPIO_InitStruct);
	
#endif
}





