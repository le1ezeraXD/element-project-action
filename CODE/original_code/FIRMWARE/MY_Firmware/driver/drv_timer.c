#include "drv_timer.h"


/**
  * @brief  TIM4的初始化
  * @param  arr:自动装载值 psc 预分频系数
  * @retval None
  */
void drv_tim4_init(uint16_t arr, uint16_t psc) {
	
#if MY_DRIVER
	//先使能APB1  TIM4的时钟
	RCC->APB1ENR |=(1<<2);
	
	//设置预分频系数  设置“精度”  分度值
	TIM4->PSC = psc-1;   //默认加一   （不分频）
	
	//72000  72M/7200=10000hz  ===  0.1ms   精度0.1ms
	
	//设置ARPE位默认为0，当arr寄存器被写入值之后，会立即更新到计数器
	TIM4->ARR = arr-1;    //量程减一
	
	//使能更新中断
	TIM4->DIER |= (1<<0);
	
	//使能预装载寄存器
	TIM4->CR1 |= (1<<7);
	
	//使能计数器
	TIM4->CR1 |=(1<<0);
	
	//使用NVIC去配置溢出中断
	//1、设置中断优先级分组   设置为101  ----   5  主4  子4
	//SCB->AIRCR  |=(0x05<<8);
	NVIC_SetPriorityGrouping(5);
	
	//2、设置使能TIM2全局中断
	NVIC_EnableIRQ(TIM4_IRQn);   //中断向量
	
	//3、设置中断优先级  1010  主优先级为2，子优先级2
	NVIC_SetPriority (TIM4_IRQn, 10);
	
	
#else
	NVIC_InitTypeDef  NVIC_InitStruct;
	TIM_TimeBaseInitTypeDef TIM_Struct;
	
	//1、先使能APB1  TIM6的时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);
	
	//2、设置中断优先级分组 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	//3、配置TIM2的参数
	TIM_Struct.TIM_Prescaler = psc-1;   //设置预分频系数
	TIM_Struct.TIM_Period = arr-1;      //设置重装载值
	TIM_Struct.TIM_CounterMode = TIM_CounterMode_Up;  //向上计数
	TIM_Struct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInit(TIM4,&TIM_Struct);
	
	//使能定时器的外设   使能计数器
	TIM_Cmd(TIM4,ENABLE);
	
	//4、使能更新中断
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE);
	
	//使能预装载寄存器
	TIM_ARRPreloadConfig(TIM4,ENABLE);
	
	//NVIC的相关参数的配置
	NVIC_InitStruct.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x02;  //主优先级
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x02;
	
	//5、NVIC的初始化
	NVIC_Init(&NVIC_InitStruct);
	
#endif
	
}




