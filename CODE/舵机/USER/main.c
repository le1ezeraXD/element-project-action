#include "stm32f10x.h"
#include "delay.h"
#include "PWM.h"
#include "key.h"
#include "led.h"


void servo_pwm(u16 pwm);
 int main(void)
 {		
	 	u8 key=0;	
		TIM4_PWM_Init(19999,83);
		delay_init();
		LED_Init();
		KEY_Init();
	 	LED0=0;					//先点亮红灯
	 while(1)
	 { 
		 key=KEY_Scan(0);	//得到键值
	   	if(key)
		{						   
			switch(key)
			{				 
				case WKUP_PRES:	//控制蜂鸣器
				  servo_pwm(950);
				  delay_ms(100);
					break; 
				case KEY1_PRES:	//  2号舵机  4号
					servo_pwm(600);
				  delay_ms(100);
					break;
				case KEY0_PRES:	//  1号舵机  3号
					servo_pwm(1400);
				delay_ms(100);
					break;
			}
		}else delay_ms(10); 
	 }
 } 
void servo_pwm(u16 pwm)
{
	u16 cnt;
	for(cnt=0;cnt<pwm;cnt++)
	{
		TIM_SetCompare2(TIM4,cnt);
	}
}	


