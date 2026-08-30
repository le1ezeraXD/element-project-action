#include "key.h"
#include "delay.h"

/*
PE3 KEY1
PE4 KEY2
PE5 KEY3
PE6 KEY4
*/

void KEY_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOE,&GPIO_InitStructure);
}


//按键处理函数
//返回按键值
//mode:0,不支持连续按;1,支持连续按;
//0，没有任何按键按下
//注意此函数有响应优先级,KEY0>KEY1>KEY_UP!!

u8 KEY_Read(u8 mode)
{
  static u8 key_up=1;//按键按松开标志
	if(mode)key_up=1;  //支持连按		  
	if(key_up&&(KEY1==1 || KEY2==1 || KEY3==1 || KEY4==1))
	{
		delay_ms(10);//去抖动 
		key_up=0;
		
		if(KEY1==1)      return  KEY1_PRES;
		else if(KEY2==1) return  KEY2_PRES;
		else if(KEY3==1) return  KEY3_PRES;
		else if(KEY4==1) return  KEY4_PRES;
	}else if(KEY1==1 && KEY2==1 && KEY3==1 && KEY4==1) key_up=1; 	    
	
 	return 0;// 无按键按下
}


