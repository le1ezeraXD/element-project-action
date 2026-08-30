#include "PWM.h"
#include "delay.h"
#include "stm32f10x.h"   

//TIM3 PWM部分初始化 
//PWM输出初始化
//arr：自动重装值
//psc：时钟预分频数     （arr+1）（psc+1)/72兆    (19999,71)      72000000

 
void TIM4_PWM_Init(u16 arr,u16 psc)
{  
		GPIO_InitTypeDef GPIO_InitStructure;
		TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
		TIM_OCInitTypeDef  TIM_OCInitStructure;


		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE); 

		//TIM4通道2
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; //
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //复用推挽输出
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOB, &GPIO_InitStructure);//TIM4通道3
		//TIM4通道3
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; //
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOB, &GPIO_InitStructure);//TIM4通道4

		TIM_TimeBaseStructure.TIM_Period = arr; 
		TIM_TimeBaseStructure.TIM_Prescaler =psc; 
		TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
		TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

		TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 
		TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 
		TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; 
		TIM_OC2Init(TIM4, &TIM_OCInitStructure);  
		TIM_OC3Init(TIM4, &TIM_OCInitStructure);  
		
		TIM_Cmd(TIM4, ENABLE);  
 
}


