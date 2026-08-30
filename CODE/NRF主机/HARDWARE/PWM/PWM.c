#include "pwm.h"


/*  电机    TIM1_CH1 PA8 */  //右轮
/*  TIM1_CH4  PA11 */   //左轮
 
void PWM_TIM1_Init(u16 arr,u16 psc)
{
		GPIO_InitTypeDef GPIO_InitStruct;
		TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
		TIM_OCInitTypeDef TIM_OCInitStruct;

		/*开启时钟*/
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1|RCC_APB2Periph_AFIO,ENABLE);

		GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF_PP;  /*复用推挽输出*/
		GPIO_InitStruct.GPIO_Pin=GPIO_Pin_8|GPIO_Pin_11;   /*PA8 PA11*/
		GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
		GPIO_Init(GPIOA,&GPIO_InitStruct);

		TIM_TimeBaseStructInit(&TIM_TimeBaseInitStruct);
		TIM_TimeBaseInitStruct.TIM_ClockDivision=TIM_CKD_DIV1;      /*不分频*/
		TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up;  /*向上计数*/
		TIM_TimeBaseInitStruct.TIM_Period=arr;             //TOUT=(arr+1)(psc+1)/TCLK
		TIM_TimeBaseInitStruct.TIM_Prescaler=psc;   
		TIM_TimeBaseInit(TIM1,&TIM_TimeBaseInitStruct);

		/*输出比较函数初始化*/
		TIM_OCInitStruct.TIM_OCMode=TIM_OCMode_PWM1;  /*pwm模式*/
		TIM_OCInitStruct.TIM_OutputState=TIM_OutputState_Enable;   /*输出使能*/
		TIM_OCInitStruct.TIM_OCPolarity=TIM_OCPolarity_High;   /*输出极性设为高*/
		TIM_OCInitStruct.TIM_Pulse=0;     /*比较值  可在外部利用函数更改*/
		TIM_OC1Init(TIM1,&TIM_OCInitStruct);
		TIM_OC4Init(TIM1,&TIM_OCInitStruct);

		/*主输出使能函数  高级定时器特有  TIM1  TIM8*/  /*MOE*/
		TIM_CtrlPWMOutputs(TIM1,ENABLE);

		/*输出比较寄存器的预装载寄存器*/
		TIM_OC1PreloadConfig(TIM1,TIM_OCPreload_Enable);
		TIM_OC4PreloadConfig(TIM1,TIM_OCPreload_Enable);

		/*预装载寄存器的使能 arr*/
		TIM_ARRPreloadConfig(TIM1,ENABLE);

		TIM_Cmd(TIM1,ENABLE);
}





