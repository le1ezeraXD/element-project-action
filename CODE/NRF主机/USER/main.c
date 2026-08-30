#include "LED.h"
#include "key.h"
#include "oled.h"
#include "spi.h"
#include "24l01.h"
#include "delay.h"
#include "sys.h"
#include "string.h"
#include "stdio.h"
#include "usart.h"

typedef struct{
	
	int action;
	
	
}NRF;

NRF *nrf_send_receive;
NRF nrf_pay;

uint8_t FLAG = 0;

void NRF_send_data(uint8_t *data,uint8_t length);

int main(void)
{
  u8 key = 0;
	u8 string[10] = {0};
	u8 tmp_buf[33];	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	delay_init(); 
	LED_Init();	
	KEY_Init();
	uart_init(115200);
	SPI2_Init();
	NRF24L01_Init();
	OLED_Init();
	OLED_Clear();


	while(NRF24L01_Check() != RESET)
	{
	  LED0=0;
		delay_ms(500);
		LED0=1;
		delay_ms(500);
	}
//	printf("NRF24L01_Check:%d \r\n",NRF24L01_Check());
   NRF24L01_TX_Mode();
	
	while(1)
	{
		
				sprintf((char *)string,"action=%d",	FLAG);
		    OLED_ShowString(0,0,string,16);
		    printf("action:%d \r\n",nrf_pay.action);
				memset((char *)&nrf_pay,0x00,sizeof(nrf_pay));
		    nrf_pay.action = FLAG;
				NRF_send_data((uint8_t *)&nrf_pay,sizeof(nrf_pay));
		    nrf_send_receive = (NRF*)tmp_buf;
		
    key = KEY_Read(0);
		if(key)
		{
			switch(key)
			{
				case KEY1_PRES: 
						 FLAG = 1;
				     LED0 = 0;
						 break;
				case KEY2_PRES: 

				     break;
				case KEY3_PRES:  

				     break;
				case KEY4_PRES: 

				     break;
			}
		}else delay_ms(10);
	}
}


/* 
通过NRF24L01传输数据 
入口参数：数据指针 数据长度

memset：复制字符 c（一个无符号字符）到参数 str 所指向的字符串的前 n 个字符
void *memset(void *str, int c, size_t n)
参数
str -- 指向要填充的内存块。
c -- 要被设置的值。该值以 int 形式传递，但是函数在填充内存块时是使用该值的无符号字符形式。
n -- 要被设置为该值的字符数。
返回值：该值返回一个指向存储区 str 的指针。
*/
void NRF_send_data(uint8_t *data,uint8_t length)
{
	static uint8_t zyj[32];
	int i = 0;
	memset(zyj,0x00,32);  //初始化zyj这个数组的值
	for(i=0;i<length;i++)
	{
		zyj[i] = *data;
		data++;
	}
	NRF24L01_TxPacket(zyj);
}

