#include "drv_spi.h"



/** PB12 - CS  PB13 - SCK  PB14 MISO  PB15 MOSI
  * @brief  SPI初始化
  * @param  None
  * @retval None
  */
void drv_spi2_init(void) {
#if MY_DRIVER
	//1、使能时钟 
	RCC->APB2ENR |= (1<<3);    //GPIOB
	RCC->APB2ENR |= (1<<0);    //AFIO
	RCC->APB1ENR |= (1<<14);   //SPI2   APB1 外设时钟频率36MHz
	
	//2、初始化GPIO
	GPIOB->CRH &= ~(0xFFFFU<<16);

	GPIOB->CRH |= (0x03<<16);   //PB12 - NSS   推挽输出
	GPIOB->CRH |= (0x0B<<20);   //PB13 - SCK   复用推挽输出
	GPIOB->CRH |= (0x08<<24);   //PB14 - MISO  上拉/下拉输入
	GPIOB->CRH |= (0x0BU<<28);  //PB15 - MOSI  复用推挽输出
	
	//3、初始化SPI
	//3.1配置spi时钟 设置波特率
	SPI2->CR1 &=~(0x07<<3);
    //3.2配置spi极性CPOL
	SPI2->CR1 &=~(1<<1);
	//3.3配置spi相位CPHA
	SPI2->CR1 &=~(1<<0); 
	//3.4配置传输数据位数  
	SPI2->CR1 &=~(1<<11);
	//3.5定义帧格式
	SPI2->CR1 &=~(1<<7);
	//3.6NSS引脚通过软件控制GPIO 来模拟片选信号
	SPI2->CR1 |= (1<<9);
	SPI2->CR1 |= (1<<8);
	//3.7配置双线双向方向
	SPI2->CR1 &=~(1<<15);
	//3.8配置为主设备
	SPI2->CR1 |= (1<<2);
	//3.9使能SPI
	SPI2->CR1 |= (1<<6);	
#else

	GPIO_InitTypeDef  GPIO_InitStruct;
	SPI_InitTypeDef   SPI_InitStruct;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
	
	//PB12 - NSS   推挽输出
	GPIO_InitStruct.GPIO_Pin = SPI2_NSS_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SPI2_PORT, &GPIO_InitStruct);
	
	//PB13 - SCK   PB15 - MOSI  复用推挽输出
	GPIO_InitStruct.GPIO_Pin = SPI2_SCK_PIN | SPI2_MOSI_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SPI2_PORT, &GPIO_InitStruct);
	
	//PB14 - MISO  上拉/下拉输入
	GPIO_InitStruct.GPIO_Pin = SPI2_MISO_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(SPI2_PORT, &GPIO_InitStruct);
	
	//初始化SPI
	SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;   //方向，选择2线全双工
	SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;   										  //设置相位为数据采样从第一个时钟边沿开始
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStruct.SPI_CRCPolynomial = 7;															//CRC多项式，暂时用不到，给默认值7
	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_Init(SPI2, &SPI_InitStruct);
	
	//SPI使能
	SPI_Cmd(SPI2, ENABLE);
	
#endif
}



/**
  * @brief  spi等待标志位
  * @param  SPIx: spi外设
  * @param  flag: 要等待的标志位
  * @retval 0:成功 1:超时
  */
uint8_t drv_spi_wait_flag(SPI_TypeDef* SPIx, uint32_t flag){
#if MY_DRIVER
	uint32_t timeout = 10000;
	while(!(SPIx->SR & flag)){
		timeout--;
		if(timeout == 0){
			return 1;
		}
	}
	return 0;
#else
	uint32_t timeout = 10000;
	while(SPI_I2S_GetFlagStatus(SPIx, flag) != SET){
		timeout--;
		if(timeout == 0){
			return 1;
		}
	}
	return 0;
#endif
}

/**
  * @brief  SPI批量数据传输
  * @param  SPIx: SPI1 & SPI2
  * @param  tx_data: 发送数据缓冲区
  * @param  rx_data: 接收数据缓冲区
  * @param  size: 数据长度
  * @retval 传输状态
  */
uint8_t drv_spi_tran_bulk(SPI_TypeDef* SPIx, uint8_t *tx_data, uint8_t *rx_data, uint16_t size) {
#if MY_DRIVER
	if(size == 0) { return 0; }
	for(uint16_t i = 0; i < size; i++){
		//wait send_buf is empty
		drv_spi_wait_flag(SPIx, SPI_TDR_EMPTY);
		//send data
		SPIx->DR = tx_data ? tx_data[i] : 0xFF;  //if only recvive, send 0xFF
		
		//wait recv_buf is empty
		drv_spi_wait_flag(SPIx, SPI_RDR_EMPTY);
		//recv data
		uint8_t receive_data = SPIx->DR;
		if(rx_data){
			rx_data[i] = receive_data;
		}
	}
	return 0;
#else
	if(size == 0) { return 0; }
	for(uint16_t i = 0; i < size; i++){
		//wait send_buf is empty
		drv_spi_wait_flag(SPIx, SPI_I2S_FLAG_TXE);
		//send data
		SPI_I2S_SendData(SPIx, (tx_data ? tx_data[i] : 0xFF));   //if only recvive, send 0xFF
		
		//wait recv_buf is empty
		drv_spi_wait_flag(SPIx, SPI_I2S_FLAG_RXNE);
		//recv data
		uint8_t receive_data = SPI_I2S_ReceiveData(SPIx);		
		if(rx_data){
			rx_data[i] = receive_data;
		}
	}
	return 0;
	
#endif
}



/**
	* @brief  SPI单字节数据发送
  * @param  SPIx: SPI1 & SPI2
  * @param  data: 发送数据缓冲区
  * @param  size: 数据长度
  * @retval 传输状态
  */
uint8_t drv_spi_send_byte(SPI_TypeDef* SPIx, uint8_t* data) {
		drv_spi_wait_flag(SPIx, SPI_TDR_EMPTY);
		SPIx->DR = *data;
    return 0;
}

/**
	* @brief  SPI单字节数据接收
  * @param  SPIx: SPI1 & SPI2
  * @param  buffer: 接收数据缓冲区
  * @param  size: 数据长度
  * @retval 传输状态
  */
uint8_t drv_spi_receive_byte(SPI_TypeDef* SPIx, uint8_t* data) {
		drv_spi_wait_flag(SPIx, SPI_TDR_EMPTY);
		SPIx->DR = 0xFF; // 发送虚拟字节产生时钟
		
		drv_spi_wait_flag(SPIx, SPI_RDR_EMPTY);
		*data = SPIx->DR;
    return 0;
}



