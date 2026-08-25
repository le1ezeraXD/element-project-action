#include "drv_systick.h"

#define SYSTICK_E  9
#define COUNTFLAG  0x10000

/**
  ******************************************************************************
  * @file    drv_systick.c
  * @author  zyj
  * @version V0.0.1
  * @date    14-10-2025
  * @brief   systick-delay
  ******************************************************************************
  *
  * SysTick timer (STK)
  * 24-bit system timer  24位系统定时器-递减定时器
  * 初始值始终从load寄存器加载
  *
  ******************************************************************************
  */


/**
  * @brief  SysTick滴答定时器的初始化
  * @param  None
  * @retval None
  */
void drv_systick_init(void){
	
	//设置systick时钟频率 系统时钟HCLK是72MHz 这里systick时钟为(HCLK/8)= 9MHz
	//Systick定时器产生1ms的时钟基准  可配置中断
	SysTick->CTRL &= ~(1<<2);
	//打开Systick的中断
	//SysTick->CTRL |= (1<<1);
}
 

/** 最大计数次数2^24次  系统提供时钟9MHz  每计数一次需要 1/9M 秒  1/9us
  * @brief  us级别延时函数  
  * @param  count:计数值(STK_LOAD - 最大支持24位装载值大小) 
  * @retval None
  */
void delay_us(uint32_t count){
	
	//设置LOAD寄存器  存储的是最终要记的次数 - load
	SysTick->LOAD = SYSTICK_E * count;
	//清0计数器
	SysTick->VAL = 0;
	//使能(开启系统嘀嗒定时器) 
	//当SysTick定时器从禁用状态被使能时，它会在下一个时钟周期，将重载寄存器(LOAD)的值自动装载到当前值寄存器(VAL)中，并开始递减计数
	SysTick->CTRL |= (1<<0); 
	//Returns 1 if timer counted to 0 since last time this was read
	while(!(SysTick->CTRL & COUNTFLAG));
	//计数结束关闭Systick
	SysTick->CTRL &= ~(1<<0);
}


/** 
  * @brief  ms级别延时函数  
  * @param  count:计数值
  * @retval None
  */
void delay_ms(uint32_t count){
	
	uint32_t n = count / 1000;
	uint32_t last = count % 1000;
	
	if(n > 1){
		//循环n次1000ms
		uint32_t i = 0;
		for(i = 0; i < n; i++){
			delay_us(1000 * 1000);
		}
	}
	
	if(last != 0){
		//执行剩余时间
		delay_us(1000 * last);
	}
}







