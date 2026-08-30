#include "motor.h"

/* 电机IO初始化  */

void MOTOR_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;      /*GPIO初始化*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,ENABLE);
	
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_Out_PP;  
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;   /*电机一  PD4  PD5   电机二 PD6  PD7*/
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOD,&GPIO_InitStruct);
//	/* 防止电机乱转 */ 
	GPIO_ResetBits(GPIOD,GPIO_Pin_4);
	GPIO_ResetBits(GPIOD,GPIO_Pin_5);
	GPIO_ResetBits(GPIOD,GPIO_Pin_6);
	GPIO_ResetBits(GPIOD,GPIO_Pin_7);
}



/*
绝对值函数
*/

int ZYJ_abs(int a)
{
	int temp;
	if(a<0)
	{
		temp = -a;
	}else temp = a;
	return temp;
}



void LOAP_PWM(int motor_right,int motor_left)  
{
	if(motor_right<0)   Ain1 = 1, Ain2 = 0;  //前进
	else                Ain1 = 0, Ain2 = 1;  //后退
	TIM_SetCompare1(TIM1,ZYJ_abs(motor_right));
	if(motor_left>0)    Bin1 = 1, Bin2 = 0;  //前进
	else                Bin1 = 0, Bin2 = 1;  //后退
  TIM_SetCompare4(TIM1,ZYJ_abs(motor_left));
}










