#include "drv_encoder.h"


/** PA6 TIM3_CH1   PA7 TIM3_CH2
  * @brief  tim3_encoder初始化
  * @param  arr:自动装载值 psc 预分频系数
  * @retval None
  */
void drv_tim3_encoder_init(uint16_t arr, uint16_t psc) {
#if MY_DRIVER
	//1.使能时钟
	RCC->APB1ENR |=(1<<1);	//TIM3
	RCC->APB2ENR |=(1<<2);  //GPIOA
	
	//2.配置GPIOA6 GPIOA7(CH1 & CH2)
	GPIOA->CRL &=~(0xFFUL<<24);
	GPIOA->CRL |= (0x44<<24);  //浮空输入
	
	//3.配置TIM
	//设置预分频系数  设置“精度”  分度值
	TIM3->PSC = psc-1;   //默认加一  
	//设置ARPE位默认为0，当arr寄存器被写入值之后，会立即更新到计数器
	TIM3->ARR = arr-1;    //量程减一
	
	//4.配置编码器模式
	//将chx设置输入,映射到TI1 TI2
	TIM1->CCMR1 |= (0x01<<0);				//ch1
	TIM1->CCMR1 |= (0x01<<8);				//ch2
	//配置输入预分频器
	TIM1->CCMR1 &=~(0x03<<2); 			//ch1无预分频器，在检测到的每一个边沿都触发一次捕获
	TIM1->CCMR1 &=~(0x03<<10); 			//ch2无预分频器，在检测到的每一个边沿都触发一次捕获
	//选择编码器模式三
	TIM3->SMCR |= (0x03<<0);  			//双边沿计数 - 4倍频
	//设置输入极性
	TIM3->CCER &=~(1<<0);						//CH1不反相
	TIM3->CCER &=~(1<<5);						//CH2不反相
	//输入滤波
	TIM3->CCMR1 |= (0x03<<4); 			//采样8次
	TIM3->CCMR1 |= (0x03<<12); 			//采样8次
	
	//5.使能更新中断
	TIM3->DIER |= (1<<0);

	//使用NVIC去配置溢出中断
	//1.设置中断优先级分组   设置为101  ----   5  主4  子4
	//SCB->AIRCR  |=(0x05<<8);
	NVIC_SetPriorityGrouping(5);
	
	//2.设置使能TIM2全局中断
	NVIC_EnableIRQ(TIM3_IRQn);   //中断向量
	
	//3.设置中断优先级  1010  主优先级为2，子优先级2
	NVIC_SetPriority (TIM3_IRQn, 10);
	
	//6.清零计数器的值
	TIM3->CNT = 0;
	
	//使能计数器
	TIM3->CR1 |=(1<<0);
	
#else
	TIM_ICInitTypeDef  TIM_ICInitStruct;
	NVIC_InitTypeDef  NVIC_InitStruct;
	GPIO_InitTypeDef  GPIO_InitStruct;
	TIM_TimeBaseInitTypeDef TIM_Struct;
	
	//1、先使能APB1  TIM3的时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO,ENABLE);
	
	//2、设置中断优先级分组 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	//PA6  PA7
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;  	//选择配置为浮空输入
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
  GPIO_Init(GPIOA,&GPIO_InitStruct);	
	
	//3、配置TIM3的参数
	TIM_TimeBaseStructInit(&TIM_Struct);  //初始化定时器
	TIM_Struct.TIM_Prescaler = psc-1;                 //设置预分频系数
	TIM_Struct.TIM_Period = arr-1;                    //设置重装载值
	TIM_Struct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInit(TIM3,&TIM_Struct);

	//配置编码器参数
	TIM_EncoderInterfaceConfig(TIM3,TIM_EncoderMode_TI12,TIM_ICPolarity_Rising,TIM_ICPolarity_Rising);
	
	TIM_ICStructInit(&TIM_ICInitStruct);  //输入捕获初始化
	TIM_ICInitStruct.TIM_ICFilter = 10;   //设置输入滤波器
	TIM_ICInit(TIM3,&TIM_ICInitStruct);
	
	//4、使能更新中断
	TIM_ClearFlag(TIM3,TIM_FLAG_Update);
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);
	
	//NVIC的相关参数的配置
	NVIC_InitStruct.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x02;  //主优先级
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x02;
	
	//5、NVIC的初始化
	NVIC_Init(&NVIC_InitStruct);
	
	//清零定时器的计数器值
	TIM_SetCounter(TIM3,0);
		
	//使能定时器的外设   使能计数器
	TIM_Cmd(TIM3,ENABLE);

#endif
	
}
