#include "drv_usart1.h"


#pragma import(__use_no_semihosting)

struct __FILE { 
    int handle; 
}; 

FILE __stdout;

// 必需的最小实现
void _sys_exit(int x) { 
    while(1); // 死循环
}

void _ttywrch(int ch) {
    while((USART1->SR & 0x40) == 0);
    USART1->DR = (uint8_t)ch;
}

int fputc(int ch, FILE *f) {
    while((USART1->SR & 0x40) == 0);
    USART1->DR = (uint8_t)ch;
    return ch;
}


/**
  * @brief  USART1的初始化
  * @param  None
  * @retval None
  */
void drv_usart1_Init(uint32_t bound) {
	
#if MY_DRIVER
	uint32_t apb2_clk = 72000000; 
  uint32_t usart_div = (apb2_clk + bound / 2) / bound;  // 四舍五入
	//1、使能时钟
	//使能GPIOA时钟
	RCC->APB2ENR |=(1<<2);
	//使能AFIO时钟
	RCC->APB2ENR |=(1<<0);
	//使能USART1时钟
	RCC->APB2ENR |=(1<<14);
	
	//2、初始化GPIO
	//MODE  11   CNFY  10   [7:4]   1011  0X0B;
	//PA9为复用推挽输出模式
	GPIOA->CRH &=~(0x0F<<4);  //先清零
	GPIOA->CRH |= (0x0B<<4);  //复用推挽输出模式
	
	//PA10为浮空输入
	//MODE 00  CNFY  01   [11:8]  0100  0x04
	GPIOA->CRH &=~(0x0F<<8);  //先清零
	GPIOA->CRH |= (0x04<<8);  //浮空输入
	
	//3、初始化USART1
	//3.1设置波特率
  USART1->BRR = (usart_div / 16) << 4 | (usart_div % 16);
	//3.2设置校验位
	USART1->CR1 &= ~(1<<10);     	  //不使用校验位
	//3.3数据位
	USART1->CR1 &= ~(1<<12);     	  //设置8位长度
	//3.4停止位
	USART1->CR2 &= ~(0x02<<12);  	  // 一个停止位
	
	//4、使能
	//使能发送  TE位
	USART1->CR1 |=(1<<3);
	//使能接收  RE位
	USART1->CR1 |=(1<<2);
	//串口使能  使能USART模块  UE位
	USART1->CR1 |=(1<<13);

	//配置数据接收需要用到中断
	//5、NVIC 使用NVIC去配置溢出中断
	//1、设置中断优先级分组   设置为101  ----   5  主4  子4
	//SCB->AIRCR  |=(0x05<<8);
	NVIC_SetPriorityGrouping(5);
	
	//2、设置使能USART1全局中断
	NVIC_EnableIRQ(USART1_IRQn);   //中断向量
	
	//3、设置中断优先级  1010  主优先级为2，子优先级2
	NVIC_SetPriority (USART1_IRQn, 10);
	
	
	//6、使能接收数据中断寄存器
	USART1->CR1 |= (1<<5);  //RXNEIE  数据接收中断
	USART1->CR1 |= (1<<4);  //IDLEIE  总线空闲中断

#else
	GPIO_InitTypeDef  GPIO_InitStruct;
	USART_InitTypeDef  USART_InitStruct;
	NVIC_InitTypeDef  NVIC_InitStruct;
	
	//1、使能时钟
	//使能GPIOA时钟  使能AFIO时钟  使能USART1时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO|RCC_APB2Periph_USART1,ENABLE);

	//2、初始化GPIO
	//MODE  11   CNFY  10   [7:4]   1011  0X0B;
	//PA9为复用推挽输出模式
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	//PA10为浮空输入
	//MODE 00  CNFY  01   [11:8]  0100  0x04
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	
	//3、初始化USART1
	//3.1  设置波特率
	//9600   468.75   整数部分468-->0x1D4  75--->0x4B
	USART_InitStruct.USART_BaudRate = bound;
	//3.2  设置停止位  [13:12]  00
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	//3.3  设置数据位 设置数据长度  8位or9位  设置为一位起始位
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;
	//3.4  设置校验位
	//不使用校验位
	USART_InitStruct.USART_Parity = USART_Parity_No;
	//3.5  是否使用硬件流控制
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;   
	//3.6  使能了数据的发送和接收
	USART_InitStruct.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;
	
	//初始化串口1
	USART_Init(USART1,&USART_InitStruct);

	//4、使能
	//串口1使能  使能USART模块  UE位
	USART_Cmd(USART1,ENABLE);
	
	//5、配置接收中断使能
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
	//使能串口总线空闲中断
	USART_ITConfig(USART1,USART_IT_IDLE,ENABLE);
	
	//6、配置NVIC
	//使用NVIC去配置溢出中断 	NVIC的相关参数的配置
	//设置中断优先级分组 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x02;  //主优先级
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x02;
	//NVIC的初始化
	NVIC_Init(&NVIC_InitStruct);
	
#endif
}


/**
  * @brief  usart1 串口发送len长度的char数据
  * @param  const char *data(字符串)  uint8_t len(数据长度)
  * @retval None
  */
void usart1_send_data(const char *data, uint8_t len) {
	int i = 0 ;
	for (i = 0; i < len; i++) {
		if(*(data+i) == '\0')  //空白符不需要发送
			break;

		//判断是否允许发送数据
		while((USART1->SR & USART_SR_TXE) == 0);
		//USART1->DR = *(data+i); 
		USART1->DR = (uint8_t)data[i];
	}
}






