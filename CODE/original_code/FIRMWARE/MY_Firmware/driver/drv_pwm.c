#include "drv_pwm.h"


/** PWM频率 = 定时器时钟源频率 / ((预分频器值 + 1) * (自动重装载值 + 1))
  * @brief  TIM8_PWM初始化
  * @param  arr:自动装载值 psc 预分频系数
  * @retval None
  */
void drv_tim8_pwm_init(uint16_t arr, uint16_t psc) {
#if MY_DRIVER
	//TIM8
	RCC->APB2ENR |= (1<<13);	 
	//AFIO
	RCC->APB2ENR |= (1<<0);   
	//GPIOC
	RCC->APB2ENR |= (1<<4);
	
	//pwm_chx通道初始化
	GPIOC->CRL &=~(0xFFU<<24); 
	GPIOC->CRL |= (0xBBU<<24);  //PC6&PC7  ch1  ch2
	GPIOC->CRH &=~(0xFFU<<0); 
	GPIOC->CRH |= (0xBBU<<0);		//PC8&PC9  ch3  ch4
	
	//设置计数方向
	TIM8->CR1 &= ~(1<<4);
	//设置自动装载值
	TIM8->ARR = arr - 1; 
	//设置预分频系数
	TIM8->PSC = psc - 1;
	//设置ARPE位
	TIM8->CR1 &= ~(1<<7);
	
	//PWM模式设置
	//设置PWM模式(PWM mode1)
	TIM8->CCMR1 |= (0x06<<4);				//ch1
	TIM8->CCMR1 |= (0x06<<12);			//ch2
	TIM8->CCMR2 |= (0x06<<4);				//ch3
	TIM8->CCMR2 |= (0x06<<12);			//ch4		
	
	//预装载使能(CCRx的预装载使能)
//	TIM8->CCMR1 |= (1<<3);
//	TIM8->CCMR1 |= (1<<11);
//	TIM8->CCMR2 |= (1<<3);
//	TIM8->CCMR2 |= (1<<11);
	
	//将TIM通道配置为输出
	TIM8->CCMR1 &=~(0x3<<0);
	TIM8->CCMR1 &=~(0x3<<8);
	TIM8->CCMR2 &=~(0x3<<0);
	TIM8->CCMR2 &=~(0x3<<8);
	
	//设置极性(设置为高电平有效)
	TIM8->CCER &=~(1<<0);
	TIM8->CCER &=~(1<<5);
	TIM8->CCER &=~(1<<9);
	TIM8->CCER &=~(1<<13);
	
	//输出使能
	TIM8->CCER |= (1<<0);
	TIM8->CCER |= (1<<4);
	TIM8->CCER |= (1<<8);
	TIM8->CCER |= (1<<12);
	
	//使能计数器
	TIM8->CR1 |= (1<<0);
	
	//高级定时器,需要使能主输出
	TIM8->BDTR |= (1<<15); // MOE主输出使能
	
#else
	
	GPIO_InitTypeDef  GPIO_InitStruct;
	TIM_OCInitTypeDef TIM_OCInitStruct;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOC | RCC_APB2Periph_TIM8, ENABLE);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period = arr - 1;
	TIM_TimeBaseInitStruct.TIM_Prescaler = psc - 1;
	TIM_TimeBaseInit(TIM8, &TIM_TimeBaseInitStruct);	
	
	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OC1Init(TIM8, &TIM_OCInitStruct);
	TIM_OC2Init(TIM8, &TIM_OCInitStruct);  
	TIM_OC3Init(TIM8, &TIM_OCInitStruct);
	TIM_OC4Init(TIM8, &TIM_OCInitStruct);
	
	TIM_ARRPreloadConfig(TIM8,ENABLE);
	
	// 高级定时器TIM8,需要使能主输出
	TIM_CtrlPWMOutputs(TIM8, ENABLE);
	
	TIM_Cmd(TIM8,ENABLE);
	
#endif
	
}


void drv_tim8_set_duty(uint8_t channel, uint16_t duty) {
    switch(channel) {
        case 1: TIM8->CCR1 = duty; break;
        case 2: TIM8->CCR2 = duty; break;
        case 3: TIM8->CCR3 = duty; break;
        case 4: TIM8->CCR4 = duty; break;
    }
}

void pwm_init(uint8_t init_angle) {
    //1. GPIO初始化
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, & GPIO_InitStructure);
    
    //2. 开启时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    
	// 3. 配置定时器基本参数
    // 72MHz / 72 = 1MHz计数频率（周期1us）
    // 20000个计数 = 20ms周期 → 50Hz
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_Period = 20000 - 1;
    TIM_TimeBaseStructure.TIM_Prescaler = 72 - 1;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    
    // 4. 配置PWM输出模式
    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = init_angle;      //90°
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

    TIM_OC1Init(TIM2, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);

    TIM_OC2Init(TIM2, &TIM_OCInitStructure);
	TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);

    TIM_OC3Init(TIM2, &TIM_OCInitStructure);
	TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);

    TIM_OC4Init(TIM2, &TIM_OCInitStructure);
	TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Enable);

	TIM_ARRPreloadConfig(TIM2, ENABLE);
	
    //5. 使能定时器
    TIM_Cmd(TIM2, ENABLE);
}

void pwm_set_compare(uint8_t ch, uint16_t compare) {
	switch(ch) {
		case 1: TIM_SetCompare1(TIM2, compare); break;
		case 2: TIM_SetCompare2(TIM2, compare); break;
		case 3: TIM_SetCompare3(TIM2, compare); break;
		case 4: TIM_SetCompare4(TIM2, compare); break;
		default: break;
	}
}
