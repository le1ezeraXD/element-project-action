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



#if 0
tim_encoder_t encoder_dev;

/** 
 * @brief  获取timx 编码器统计数值
* @param  TIMx: 选择定时器
 * @retval 返回计数总值
 */
uint32_t drv_get_encoder_count(TIM_TypeDef* TIMx){
	/* 计算当前总计数值, 当前总计数值 = 计数器当前值 + 溢出次数 * 65536 */
	return (uint32_t)(TIM_GetCounter(TIMx) + timx_encoder_cnt * 65536);
}


/** 
 * @brief  编码器初始化
 * @param  TIMx: 选择定时器
 * @retval 初始化编码器设备
 */
void encoder_init(TIM_TypeDef* TIMx, tim_encoder_t* encoder_dev, uint8_t (*get_count)(void)){
	encoder_dev->encoder_accuracy	= 20;
}

/** 在5ms的中断中计算
 * @brief  编码器旋转速度计算
 * @param  TIMx: 选择定时器
 * @retval 返回旋转速度
 */
int32_t encoder_speed(TIM_TypeDef* TIMx, tim_encoder_t* encoder_dev){
	encoder_dev->encoder_now = drv_get_encoder_count(TIMx);
	encoder_dev->encoder_change = encoder_dev->encoder_now - encoder_dev->encoder_old;
	encoder_dev->encoder_old = encoder_dev->encoder_now;
	/* 编码器旋转速度 = 计数值变化量 / 4倍频 /转一圈输出的脉冲数  */
	/* 脉冲数(该段时间内) = 计数值变化量 / 4倍频   */
	/* 编码器旋转圈数(该段时间内) = 脉冲数 / 编码器频率(转一圈输出的脉冲数)   */
	/* 编码器速度rpm = r / min */
	/* 编码器速度 = 编码器旋转圈数 / 5 * 1000 * 60  (/5是计算1ms旋转多少圈  *1000是计算1s转多少圈  *60是计算1min转多少圈) */
	/* 编码器速度 = 编码器旋转圈数 * 200 * 60  */

	//encoder_dev->speed = encoder_dev->encoder_change * 200 * 60 / ENCODER_MULTI / encoder_dev->encoder_accuracy;
	encoder_dev->speed = encoder_dev->encoder_change * 200 * 60 / 4 / 20;
	
	return (int32_t)encoder_dev->speed;
}
#endif

#if 0
int32_t encoder_now,encoder_old,var;
/** 
 * @brief  电机速度计算
	* @param  TIMx: 选择定时器
 * @retval 返回电机速度
 */
int32_t motor_speed_cal(){
	encoder_now = drv_get_encoder_count(TIM3);  		/*获取当前计数值*/
	var = encoder_now - encoder_old;								/*计数值变化量*/
	encoder_old = encoder_now;											/*保存这次计数值*/
	return (int32_t)(var * 5 * 60 / 4 / 13 /30);		/*返回电机速度*/
}
#endif




#if 0
/**
  * @brief  TIM2的初始化
  * @param  None
  * @retval None
  */
void drv_tim2_init(uint16_t arr, uint16_t psc) {
	
#if MY_DRIVER
	//1、先使能APB1  TIM2的时钟
	RCC->APB1ENR |=(1<<0);
		
	//2、使能计数器
	TIM2->CR1 |=(1<<0);
	
	//3、设置预分频系数  设置“精度”  分度值
	TIM2->PSC = psc-1;   //默认加一   （不分频）
	
	//72000  72M/7200=10000hz  ===  0.1ms   精度0.1ms
	
	//4、设置ARPE位默认为0，当arr寄存器被写入值之后，会立即更新到计数器
	TIM2->ARR = arr-1;    //量程减一
	
	//arr寄存器的值什么时候写入到cnt寄存器当中，是由ARPE位决定
	
	//5、使能更新中断
	TIM2->DIER |= (1<<0);
	
	//使能预装载寄存器
	TIM2->CR1 |= (1<<7);
	
	//使用NVIC去配置溢出中断
	//1、设置中断优先级分组   设置为101  ----   5  主4  子4
	//SCB->AIRCR  |=(0x05<<8);
	NVIC_SetPriorityGrouping(5);
	
	//2、设置使能TIM2全局中断
	NVIC_EnableIRQ(TIM2_IRQn);   //中断向量
	
	//3、设置中断优先级  1010  主优先级为2，子优先级2
	NVIC_SetPriority (TIM2_IRQn, 10);
	
	
#else
	NVIC_InitTypeDef  NVIC_InitStruct;
	TIM_TimeBaseInitTypeDef TIM_Struct;
	
	//1、先使能APB1  TIM2的时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	
	//2、设置中断优先级分组 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	//3、配置TIM2的参数
	TIM_Struct.TIM_Prescaler = psc-1;   //设置预分频系数
	TIM_Struct.TIM_Period = arr-1;      //设置重装载值
	TIM_Struct.TIM_CounterMode = TIM_CounterMode_Up;  //向上计数
	TIM_Struct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInit(TIM2,&TIM_Struct);
	
	//使能定时器的外设   使能计数器
	TIM_Cmd(TIM2,ENABLE);
	
	//4、使能更新中断
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
	
	//使能预装载寄存器
	TIM_ARRPreloadConfig(TIM2,ENABLE);
	
	//NVIC的相关参数的配置
	NVIC_InitStruct.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x02;  //主优先级
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x02;
	
	//5、NVIC的初始化
	NVIC_Init(&NVIC_InitStruct);
	
#endif
	
}
#endif

