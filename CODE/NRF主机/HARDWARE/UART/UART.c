#include "uart.h"
#include "oled.h"
#include "control.h"
#include "stdio.h"

void Openmv_Data(void);
void Receive_Openmv_Data(u16 data);//接收Openmv传过来的数据
int openmv[5];//stm32接收数据数组
/*
  p4 -> PC11
	p5 -> PC10
*/

void UART4_Init(void)
{
		GPIO_InitTypeDef GPIO_InitStructure; //串口端口配置结构体变量
		USART_InitTypeDef USART_InitStructure;//串口参数配置结构体变量
		NVIC_InitTypeDef NVIC_InitStructure;//串口中断配置结构体变量
		//使能 UART4 时钟
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);	//打开串口复用时钟
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);   //打开PC端口时钟
  
	//UART4_TX   GPIOC.10
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PC.10
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//设定IO口的输出速度为50MHz
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
		GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化GPIOC.10
   
  	//UART4_RX	  GPIOC.11
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//PC.11
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
  	GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化GPIOC.11
	
	 	//Usart4 NVIC 配置
  	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;//抢占优先级0
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		  //子优先级2
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			    //IRQ通道使能
		NVIC_Init(&NVIC_InitStructure);	                        //根据指定的参数初始化VIC寄存器
  
  	//USART 初始化设置
		USART_InitStructure.USART_BaudRate = 115200;//串口波特率为115200
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
		USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
		USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式使能
		USART_Init(UART4, &USART_InitStructure); //初始化串口4
	
		USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);//开启串口接收中断
		
		USART_Cmd(UART4, ENABLE);                    //使能串口4 
  
 	 //如下语句解决第1个字节无法正确发送出去的问题
   	USART_ClearFlag(UART4, USART_FLAG_TC);       //清串口4发送标志
		
}
	

void UART4_IRQHandler(void)
{
  //接收中断
	if( USART_GetITStatus(UART4,USART_IT_RXNE) )
	{
		USART_ClearITPendingBit(UART4,USART_IT_RXNE);//清除中断标志
//		OPENMV_Meadian = USART_ReceiveData(UART4);
		OPENMV_Meadian = UART4->DR;
		Receive_Openmv_Data(OPENMV_Meadian);//openmv数据处理函数
		Openmv_Data();
	}
	
	
}
	

int i=0;
//处理openmv传过来的数据
void Receive_Openmv_Data(u16 data)//处理Openmv传过来的数据
{
	static u8 state = 0;
	if(state==0 && data==0xb3)
	{
		state=1;
		openmv[0]=data;
	}
	else if(state==1 && data==0xb3)
	{
		state=2;
		openmv[1]=data;
	}
	else if(state==2)
	{
		state=3;
		openmv[2]=data;  // 停止和启动标志
	}
	else if(state==3)
	{
		state = 4;
		openmv[3]=data;  // 偏差量
	}

	else if(state==4)		//检测是否接受到结束标志
	{
        if(data == 0x5b)
        {
            state = 0;
            openmv[4]=data;
            Openmv_Data();
        }
        else if(data != 0x5b)
        {
            state = 0;
            for(i=0;i<5;i++)
            {
                openmv[i]=0x00;
            }           
        }
	}    
	else
		{
			state = 0;
			for(i=0;i<5;i++)
			{
					openmv[i]=0x00;
			}
		}
}


void Openmv_Data(void)
{
	  Stop = openmv[2];
    OPENMV_Meadian = openmv[3];
}
