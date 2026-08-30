#include "SEARCH.h"
#include "delay.h"
#include "PWM.h"
#include "stm32f10x.h"   
 


void SearchInit(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOA,ENABLE);  //使用PA端口时钟
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode= GPIO_Mode_IPU;  //配置GPIO模式,输入上拉
	GPIO_InitStructure.GPIO_Speed= GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

}

void searchrun(void)
{
	int a=0;
	if(M==black && L==white && R==white)
	{
		qianjin();
	}
	else if(M==black && L==black && R==black)
	{
		stop();
		a++;
		delay_ms(500);
	}
	else if(a==1)
	{
		zuozhuan();
	}
	else if(a==1 && M==black && L==white && R==white)
	{
		qianjin();
	}
	else if( M==white && L==white && R==white)
	{
		diaotou();
	}
		
}
	