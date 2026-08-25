#include "drv_exti.h"

/**
	******************************************************************************
	* @file    drv_exti.c
	* @author  zyj
	* @version V0.0.1
	* @date    21-10-2025
	* @brief   exti
	******************************************************************************
	*
	*
	******************************************************************************
	*/
	
/**
  * @brief  EXTI的初始化
  * @param  None
  * @retval None
  */
void drv_exti_init(void) {
#if MY_DRIVER
	
	//设置输入
	//使能GPIOB的时钟 
	RCC->APB2ENR |= (1<<3);
	//使能复用时钟AFIO
	RCC->APB2ENR |= (1<<0);
	
	//仅SWD模式（释放JTAG引脚，保留SWD调试）
	AFIO->MAPR &= ~(0x7 << 24);     // 清除SWJ_CFG[2:0]位
	AFIO->MAPR |= (0x2 << 24);      // 设置为010 - 仅SWD模式
	
	//设置上拉输入
	GPIOB->CRL &= ~(0x0F<<16);
	GPIOB->CRL |= (0x08<<16);
	
	//配置上拉 
	GPIOB->ODR |= (1<<4);
	
	//选择EXTIx外部中断的输入源
	AFIO->EXTICR[1] &= 0xFFF0;   //PB4
	AFIO->EXTICR[1] |= 0x01;     //PB4设置值为0x0001
	
	//配置NVIC
	NVIC_SetPriorityGrouping(5);  				//设置中断优先级分组
	NVIC_EnableIRQ(EXTI4_IRQn);   				//使能中断
	NVIC_SetPriority(EXTI4_IRQn,10);      //设置中断优先级
	
	//配置EXTI4相关触发
	EXTI->IMR |= (1<<4);   //使能EXTI4
	
	//配置所选中断线的触发选择位(EXTI_RTSR和EXTI_FTSR)
	EXTI->FTSR |= (1<<4);  //配置EXTI4下降沿触发
	
#else

	GPIO_InitTypeDef GPIO_InitStruct;
	EXTI_InitTypeDef EXTI_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO,ENABLE);
	
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);       

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;   //上拉输入
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource4);
	
	EXTI_InitStruct.EXTI_Line = EXTI_Line4;
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_Init(&EXTI_InitStruct);
	
	NVIC_InitStruct.NVIC_IRQChannel = EXTI4_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x00;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x02;
	NVIC_Init(&NVIC_InitStruct);
	
#endif

}




	
	
	




