#include "drv_capture.h"



/** 
  * @brief  tim1_capture初始化
  * @param  arr:自动装载值 psc 预分频系数
  * @retval None
  */
void drv_tim1_capture_init(uint16_t arr, uint16_t psc) {
#if MY_DRIVER
	//TIM1
	RCC->APB2ENR |= (1<<11);	 
	//AFIO
	RCC->APB2ENR |= (1<<0);   
	//GPIOA
	RCC->APB2ENR |= (1<<2);
	
	//capture_chx通道初始化
	GPIOA->CRH &=~(0x0FU<<0); 
	GPIOA->CRH |= (0x08U<<0);  //PA8 ch1
	//设置为下拉输入
	GPIOA->ODR &=~(1<<8);
	
	//设置计数方向
	TIM1->CR1 &= ~(1<<4);
	//设置自动装载值
	TIM1->ARR = arr - 1; 
	//设置预分频系数
	TIM1->PSC = psc - 1;
	//设置ARPE位
	TIM1->CR1 &= ~(1<<7);
	
	//输入捕获模式设置
	//将chx设置输入,映射到TI1
	TIM1->CCMR1 |= (0x01<<0);				//ch1
	//配置输入滤波
	TIM1->CCMR1 |= (0x03<<4);       //连续采样8次
	//选择TI1通道有效转换边沿(上升沿)
	TIM1->CCER &=~(1<<1);
	//配置输入预分频器
	TIM1->CCMR1 &=~(0x03<<2); 			//无预分频器，在检测到的每一个边沿都触发一次捕获
	//捕获使能
	TIM1->CCER |= (1<<0);
	
	//打开ch1捕获中断
	TIM1->DIER |= (1<<1);
	//打开溢出更新中断
	TIM1->DIER |= (1<<0);
	
	//使能计数器
	TIM1->CR1 |= (1<<0);
	
	//配置中断
	//设置中断优先级分组   设置为101  ----   5  主4  子4
	//SCB->AIRCR  |=(0x05<<8);
	NVIC_SetPriorityGrouping(5);
	
	//设置使能TIM2全局中断
	NVIC_EnableIRQ(TIM1_CC_IRQn);   //中断向量
	
	//设置中断优先级  1010  主优先级为2，子优先级2
	NVIC_SetPriority(TIM1_CC_IRQn, 10);
	
#else
	
	GPIO_InitTypeDef  GPIO_InitStruct;
	TIM_ICInitTypeDef TIM_ICInitStruct;
	NVIC_InitTypeDef  NVIC_InitStruct;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOA | RCC_APB2Periph_TIM1, ENABLE);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period = arr - 1;
	TIM_TimeBaseInitStruct.TIM_Prescaler = psc - 1;
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStruct);	
	
	TIM_ICInitStruct.TIM_Channel = TIM_Channel_1;
	TIM_ICInitStruct.TIM_ICFilter = 0x03;
	TIM_ICInitStruct.TIM_ICPolarity = TIM_ICPolarity_Rising;
	TIM_ICInitStruct.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	TIM_ICInitStruct.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInit(TIM1, &TIM_ICInitStruct);
	
	TIM_ITConfig(TIM1, TIM_IT_CC1, ENABLE);
	TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);
	
	TIM_Cmd(TIM1,ENABLE);
	
	//NVIC的相关参数的配置
	NVIC_InitStruct.NVIC_IRQChannel = TIM1_CC_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x02;  //主优先级
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x02;

	NVIC_Init(&NVIC_InitStruct);
	
#endif
	
}





