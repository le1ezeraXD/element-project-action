#include "sys_spi.h"
#include "drv_spi.h"


/**
  * @brief  SPI写入操作(写指令&写寄存器)
  * @param  SPIx: SPI外设
  * @param  data: 要写入的指令
  * @param  data_size: 数据长度
  * @retval 0: success  1 fail
  */
uint8_t sys_spi_write_cmd(SPI_TypeDef* SPIx, uint8_t cmd) {
	uint8_t ret = 0;
	
	//发送指令
	ret = drv_spi_tran_bulk(SPIx, &cmd, NULL, 1);
	if(ret){
		LOG_ERROR("sys_spi_write_cmd failed");
		return 1;
	}
	return 0;
}


/**
	* @brief  SPI单字节数据接收(用来读取状态寄存器)
  * @param  SPIx: SPI1 & SPI2
  * @param  data: 发送数据缓冲区
  * @param  size: 数据长度
  * @retval 传输状态
  */
uint8_t sys_spi_read_busy(SPI_TypeDef* SPIx, uint8_t* status) {
	uint8_t ret;
	ret = drv_spi_receive_byte(SPIx, status);
	if(ret){
		LOG_ERROR("spi_read_busy fail!");
		return 1;
	}
	return 0;
}

/**
  * @brief  带地址的SPI写入操作
  * @param  SPIx: SPI外设
  * @param  addr: 地址
  * @param  data: 要写入的数据缓冲区
  * @param  data_size: 数据长度
  * @param  addr_size: 地址字节数(1-4)
  * @retval 0: success  1 fail
  */
uint8_t sys_spi_write_data(SPI_TypeDef* SPIx, spi_device* dev) {
	uint8_t ret = 0;
	uint8_t addr_buf[4];
	
	//将地址转换为大端字节序
	for(uint8_t i = 0; i < dev->addr_size; i++){
		addr_buf[i] = (dev->addr >> ((dev->addr_size - 1 - i) * 8)) & 0xFF;
	}
	
	//先发送地址
	ret = drv_spi_tran_bulk(SPIx, addr_buf, NULL, dev->addr_size);
	if(ret){
		LOG_ERROR("sys_spi_write_addr failed");
		return 1;
	}
	
	//发送数据
	ret = drv_spi_tran_bulk(SPIx, dev->tx_data, NULL, dev->data_size);
	if(ret){
		LOG_ERROR("sys_spi_write_data failed");
		return 1;
	}
	return 0;
	
}

/**
  * @brief  带地址的SPI读操作
  * @param  SPIx: SPI外设  
  * @param  addr: 地址
  * @param  data: 接收数据缓冲区
  * @param  data_size: 数据长度
  * @param  addr_size: 地址字节数(1-4)
  * @retval 0: success  1 fail
  */
uint8_t sys_spi_read_data(SPI_TypeDef* SPIx, spi_device* dev) {
	uint8_t ret = 0;
	uint8_t addr_buf[4];
	
	//将地址转换为大端字节序
	for(uint8_t i = 0; i < dev->addr_size; i++){
		addr_buf[i] = (dev->addr >> ((dev->addr_size - 1 - i) * 8)) & 0xFF;
	}
	
	//发送地址
	ret = drv_spi_tran_bulk(SPIx, addr_buf, NULL, dev->addr_size);
	if(ret){
		LOG_ERROR("sys_spi_write_addr failed");
		return 1;
	}
		
	//接收数据
	ret = drv_spi_tran_bulk(SPIx, NULL, dev->rx_data, dev->data_size);
	if(ret){
		LOG_ERROR("sys_spi_read_data failed");
		return 1;
	}
	return 0;
	
}

