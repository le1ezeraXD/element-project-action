#include "mod_w25qxx.h"
#include "sys_spi.h"
 
//状态使能
#define W25QXX_BUSY  							0x05
#define W25QXX_W_ENABLE  					0X06
#define W25QXX_W_DISABLE  				0X04

//读取芯片ID
#define W25QXX_R_CHIPID						0x90

//读数据
#define W25QXX_R_DATA							0x03  

//芯片擦除
#define W25QXX_ERASE_CHIP					0xC7
#define W25QXX_ERASE_SECTOR				0x20
#define W25QXX_ERASE_BLOCK_32K		0x52
#define W25QXX_ERASE_BLOCK_64K    0xD8

//写数据(最小操作单元page)
#define W25QXX_W_PAGE							0x02


/**
	******************************************************************************
	* @file    mod_w25qxx.c
	* @author  zyj
	* @version V0.0.1
	* @date    22-11-2025
	* @brief   w25qxx
	******************************************************************************
	*	驱动代码: w25qxx
	* ic: w25qxx
	* note: 分为块区&扇区&页(block & sector & page) 
	*       一个块是64KB 每个块由16个扇区组成 每个扇区4KB 每个扇区由16个页组成  每个页256个字节
	*				最大可操作单元是256个字节  也就是一个page
	*				内存地址从0x000000开始 一个页就是到0x0000FF  一个扇区就是到0x000FFF  一个块区就是到0x00FFFF
	******************************************************************************
	*/

/** @brief  w25qxx芯片初始化
  * @param  None
  * @retval None
  */
void mod_w25qxx_init(void) {
	//CS片选默认拉高
	w25qxx_cs = 1;
	
}

//Flash正在执行编程(写),擦除或其他内部操作时,BUSY位会被置1,此时不能进行新的操作
/**
  * @brief  检查W25QXX是否处于忙状态
  * @param  None
  * @retval 1:忙, 0:就绪
  */
uint8_t w25qxx_busy(SPI_TypeDef* SPIx) {
	uint8_t ret;
	uint8_t status = 0;
	w25qxx_cs = 0;
	
	sys_spi_write_cmd(SPIx, W25QXX_BUSY);
	ret = sys_spi_read_busy(SPIx, &status);
	if(ret){
		LOG_ERROR("spi_read_busy fail");
	}

	w25qxx_cs = 1;
	if(status & 0x01){
		return 1;
	}
	return 0;
}

/******使能&失能函数******/

/** 
  * @brief  写使能
  * @param  None
  * @retval 0: success  1 fail
  */
uint8_t w25qxx_write_enable(SPI_TypeDef* SPIx) {
	uint8_t ret;
	w25qxx_cs = 0;
	ret = sys_spi_write_cmd(SPIx, W25QXX_W_ENABLE);  //Write_enable
	if(ret){
		LOG_ERROR("spi_write_enable fail");
		return 1;
	}
	w25qxx_cs = 1;
	return 0;
}

/** 
  * @brief  失能
  * @param  None
  * @retval 0: success  1 fail
  */
uint8_t w25qxx_write_disable(SPI_TypeDef* SPIx) {
	uint8_t ret;
	w25qxx_cs = 0;
	ret = sys_spi_write_cmd(SPIx, W25QXX_W_DISABLE);  //Write_disenable
	if(ret){
		LOG_ERROR("spi_read_disable fail");
		return 1;
	}
	w25qxx_cs = 1;
	return 0;
}
/********************************
*			芯片读取函数							*																				
********************************/

/**
  * @brief  读取芯片ID
  * @param  None
  * @retval 1:忙, 0:就绪
  */
uint16_t w25qxx_read_chip_id(SPI_TypeDef* SPIx) {
	uint8_t ret;
	uint8_t temp_id[2] = {0};
	uint16_t chip_id;
	
	while(w25qxx_busy(SPIx));
	
	//CS拉低表示开始
	w25qxx_cs = 0;
	
	//发送读芯片id的指令
	ret = sys_spi_write_cmd(SPIx, W25QXX_R_CHIPID);
	
	spi_device spi_dev = {
		.addr = 0x000000,    //这里不是内存地址  是读取寄存器固定要发送的指令
		.rx_data = temp_id,
		.addr_size = 3,
		.data_size = 2
	};
	
	ret |= sys_spi_read_data(SPIx, &spi_dev);
	if(ret){
		LOG_ERROR("spi_read_chip_id fail");
	}
	
	//拉高片选
	w25qxx_cs = 1;
	
	//制造商id  manufacturer | device_ID
	chip_id = (uint16_t)((uint16_t)temp_id[0] << 8 | temp_id[1]);
	
	return chip_id;
}



/**
  * @brief  读取w25qxx数据
	* @param  addr:读取的地址 pdata:读取数据缓存区 len:要读取的字节数
	* @retval 成功:0 失败:1
  */
uint8_t w25qxx_read_data(SPI_TypeDef* SPIx, uint32_t addr, uint8_t* pdata, uint32_t len) {
	PROJ_FUNC_ENTER;
	uint8_t ret = 0;
	
	// 等待设备就绪
	while(w25qxx_busy(SPIx));
	
	//cs拉低
	w25qxx_cs = 0;
	
	//发送读指令
	sys_spi_write_cmd(SPIx, W25QXX_R_DATA);
	
	spi_device spi_dev = {
		.addr = addr,
		.rx_data = pdata,
		.addr_size = 3,
		.data_size = len
	};
	
	ret = sys_spi_read_data(SPIx,&spi_dev);
	if(ret){
		LOG_ERROR("w25qxx_read fail!!!");
		return 1;
	}
	
	w25qxx_cs = 1;
	PROJ_FUNC_EXIT;
	
	return 0;
}

/********************************
*			芯片内存擦除函数					*																				
********************************/

/**
  * @brief  芯片整片擦除(state of all 1(FFh))
  * @param  None
  * @retval None
  */
void w25qxx_chip_erase(SPI_TypeDef* SPIx) {
	PROJ_FUNC_ENTER;
	uint8_t ret;
	//擦除类似写操作  需要先写使能
	w25qxx_write_enable(SPIx);
	//cs拉低
	w25qxx_cs = 0;
	
	//发送指令
	ret = sys_spi_write_cmd(SPIx, W25QXX_ERASE_CHIP);
	if(ret){
		LOG_DEBUG("w25qxx_chip_erase_cmd_send fail!!!");
	}
	
	w25qxx_cs = 1;
	
	//等待忙状态结束
	while(w25qxx_busy(SPIx));
	
	if(w25qxx_busy(SPIx) == 0){
		LOG_DEBUG("w25qxx chip erase success!!!");
	}
	else{
		LOG_DEBUG("w25qxx chip erase fail!!!");
	}
	
	PROJ_FUNC_EXIT;
}

/**
  * @brief  芯片扇区擦除(state of all 1(FFh))
  * @param  None
  * @retval None 
  */
void w25qxx_sector_erase(SPI_TypeDef* SPIx, uint32_t sector_addr) {
	PROJ_FUNC_ENTER;
	uint8_t ret;
	//擦除类似写操作  需要先写使能
	w25qxx_write_enable(SPIx);
	//cs拉低
	w25qxx_cs = 0;
	
	//发送指令
	ret = sys_spi_write_cmd(SPIx, W25QXX_ERASE_SECTOR);
	if(ret){
		LOG_DEBUG("w25qxx_sector_erase_cmd_send fail!!!");
	}
	
	spi_device sector_erase = {
		.addr = sector_addr,
		.addr_size = 3
	};
	
	//发送地址
	sys_spi_write_data(SPIx, &sector_erase);
	
	w25qxx_cs = 1;
	
	//等待忙状态结束
	while(w25qxx_busy(SPIx));
	
	if(w25qxx_busy(SPIx) == 0){
		LOG_DEBUG("w25qxx sector erase success!!!");
	}
	else{
		LOG_DEBUG("w25qxx sector erase fail!!!");
	}
	
	PROJ_FUNC_EXIT;
}

/**
  * @brief  芯片块区32K擦除(state of all 1(FFh))
  * @param  None
  * @retval None
  */
void w25qxx_block_32k_erase(SPI_TypeDef* SPIx, uint32_t block_addr) {
	PROJ_FUNC_ENTER;
	uint8_t ret;
	//擦除类似写操作  需要先写使能
	w25qxx_write_enable(SPIx);
	//cs拉低
	w25qxx_cs = 0;
	
	//发送指令
	ret = sys_spi_write_cmd(SPIx, W25QXX_ERASE_BLOCK_32K);
	if(ret){
		LOG_DEBUG("w25qxx_block_erase_cmd_send fail!!!");
	}
	
	spi_device block_erase = {
		.addr = block_addr,
		.addr_size = 3
	};
	
	//发送地址
	sys_spi_write_data(SPIx, &block_erase);
	
	w25qxx_cs = 1;
	
	//等待忙状态结束
	while(w25qxx_busy(SPIx));
	
	if(w25qxx_busy(SPIx) == 0){
		LOG_DEBUG("w25qxx block erase success!!!");
	}
	else{
		LOG_DEBUG("w25qxx block erase fail!!!");
	}
	
	PROJ_FUNC_EXIT;
}


/**
  * @brief  芯片块区64K擦除(state of all 1(FFh))
  * @param  None
  * @retval None
  */
void w25qxx_block_64k_erase(SPI_TypeDef* SPIx, uint32_t block_addr) {
	PROJ_FUNC_ENTER;
	uint8_t ret;
	//擦除类似写操作  需要先写使能
	w25qxx_write_enable(SPIx);
	//cs拉低
	w25qxx_cs = 0;
	
	//发送指令
	ret = sys_spi_write_cmd(SPIx, W25QXX_ERASE_BLOCK_64K);
	if(ret){
		LOG_DEBUG("w25qxx_block_erase_cmd_send fail!!!");
	}
	
	spi_device block_erase = {
		.addr = block_addr,
		.addr_size = 3
	};
	
	//发送地址
	sys_spi_write_data(SPIx, &block_erase);
	
	w25qxx_cs = 1;
	
	//等待忙状态结束
	while(w25qxx_busy(SPIx));
	
	if(w25qxx_busy(SPIx) == 0){
		LOG_DEBUG("w25qxx block erase success!!!");
	}
	else{
		LOG_DEBUG("w25qxx block erase fail!!!");
	}
	
	PROJ_FUNC_EXIT;
}


/**
  * @brief  写page数据(最大写256个字节)
  * @param  SPIx SPI实例
  * @param  tx_addr 写入起始地址
  * @param  pdata 数据指针
  * @param  data_len 数据长度
  * @retval 操作状态
  */
uint8_t w25qxx_page_program(SPI_TypeDef* SPIx, uint32_t tx_addr, uint8_t* pdata, uint32_t data_len) {
	PROJ_FUNC_ENTER;
	uint8_t ret;
	//写使能
	w25qxx_write_enable(SPIx);
	//cs拉低
	w25qxx_cs = 0;
	
	//发送指令
	ret = sys_spi_write_cmd(SPIx, W25QXX_W_PAGE);
	if(ret){
		LOG_DEBUG("w25qxx_write_cmd_send fail!!!");
		return 1;
	}
	
	spi_device page_program = {
		.addr = tx_addr,
		.tx_data = pdata,
		.addr_size = 3,
		.data_size = data_len
	};
	
	//发送地址&数据
	sys_spi_write_data(SPIx, &page_program);
	
	w25qxx_cs = 1;
	
	//等待忙状态结束
	while(w25qxx_busy(SPIx));
	
	if(w25qxx_busy(SPIx) == 0){
		LOG_DEBUG("w25qxx page_program success!!!");
	}
	else{
		LOG_DEBUG("w25qxx page_program fail!!!");
	}
	
	PROJ_FUNC_EXIT;
	return 0;
}


/**
  * @brief  写page数据(支持写任意字节)
  * @param  SPIx SPI实例
  * @param  tx_addr 写入起始地址
  * @param  pdata 数据指针
  * @param  data_len 数据长度
  * @retval 操作状态
  */
uint8_t w25qxx_page_program_multi(SPI_TypeDef* SPIx, uint32_t tx_addr, uint8_t* pdata, uint32_t data_len) {
	
	PROJ_FUNC_ENTER;
	uint8_t ret;
	
	uint16_t page_size = 256;  							//page空间大小
	uint32_t current_addr = tx_addr;				//当前地址
	uint32_t remaining_data = data_len;			//未写入的数据长度
	uint8_t *current_data = pdata;					//当前数据
	
	while(remaining_data > 0){
		//计算当前页剩余空间
		uint32_t page_remain_size = page_size - (current_addr & 0xFF);
		//本次写入的数据量(取剩余数据和页剩余空间的较小值)
		uint32_t write_size = (remaining_data < page_remain_size) ? remaining_data : page_remain_size;
		//写入
		ret = w25qxx_page_program(SPIx, current_addr, current_data, write_size);
		if(ret){
			LOG_ERROR("w25qxx_write fail!!!");
			return 1;
		}
		
		//更新地址、数据以及写入长度
		current_addr += write_size;
		current_data += write_size;
		remaining_data -= write_size;
	}
	
	//等待忙状态结束
	while(w25qxx_busy(SPIx));
	
	if(w25qxx_busy(SPIx) == 0){
		LOG_DEBUG("w25qxx page_multi_program success!!!");
	}
	else{
		LOG_DEBUG("w25qxx page_multi_program fail!!!");
	}
	PROJ_FUNC_EXIT;
	return 0;
}

#if 0
uint8_t w25qxx_page_program_multi(SPI_TypeDef* SPIx, uint32_t tx_addr, uint8_t* pdata, uint32_t data_len) {
	PROJ_FUNC_ENTER;
	uint8_t ret;

	//当前页起始地址
	uint32_t current_page = tx_addr & 0xFFFF00;
	//取写入地址在当前页的偏移量
	uint32_t page_offset = tx_addr & 0xFF;
	//剩余空间大小 
	uint32_t remaining_in_page = 256 - page_offset;
	
	//如果写入字节超过剩余字节  就要写向下一页
	if(data_len > remaining_in_page){
		
		//先写满当前page
		w25qxx_page_program(SPIx, tx_addr, pdata, remaining_in_page);
		
		//写向下一页 如果剩余字节仍然超过一页  需要再次处理
		uint32_t remain_data_size = data_len - remaining_in_page;  //512
		//计算还需要写多少个page
		uint32_t remain_page_num = remain_data_size / 256;  //2
		
		//写完多个page之后剩余字节
		uint32_t remain_page_data = remain_data_size % 256;  //0

		if(remain_page_num == 0){  //表明剩余字节不超过一个page
				w25qxx_page_program(SPIx, current_page + 0x100, pdata + remaining_in_page, remain_data_size);
		}
		else{ //如果剩余字节超过一个page  循环写入
			for(uint16_t i = 0; i < remain_page_num; i++){  //0 1
					w25qxx_page_program(SPIx, current_page + ((i+1) * 0x100), pdata + remaining_in_page + (i * 256), 256);
			}
			
			if(remain_data_size % 256){
					//如果有余数 最后的字节应该往下一页写
					//这个时候还剩下一部分肯定不超过256的字节数  单独写入
					w25qxx_page_program(SPIx, current_page + ((remain_page_num + 1) * 0x100), pdata + remaining_in_page + (remain_page_num * 256), remain_page_data);
			}
		}
	}
	else{
			//发送地址&数据
			w25qxx_page_program(SPIx, tx_addr, pdata, data_len);
	}
	
	PROJ_FUNC_EXIT;
	return 0;
}
#endif















