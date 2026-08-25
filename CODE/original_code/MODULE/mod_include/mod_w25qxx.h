#ifndef __MOD_W25QXX_H
#define __MOD_W25QXX_H
#include "sys.h"


#define w25qxx_cs 	PBout(12)


void mod_w25qxx_init(void);

uint8_t w25qxx_busy(SPI_TypeDef* SPIx);

uint8_t w25qxx_write_enable(SPI_TypeDef* SPIx);

uint8_t w25qxx_write_disable(SPI_TypeDef* SPIx);

uint16_t w25qxx_read_chip_id(SPI_TypeDef* SPIx);

uint8_t w25qxx_read_data(SPI_TypeDef* SPIx, uint32_t addr, uint8_t* pdata, uint32_t len);

void w25qxx_chip_erase(SPI_TypeDef* SPIx);

void w25qxx_sector_erase(SPI_TypeDef* SPIx, uint32_t sector_addr);

void w25qxx_block_32k_erase(SPI_TypeDef* SPIx, uint32_t block_addr);

void w25qxx_block_64k_erase(SPI_TypeDef* SPIx, uint32_t block_addr);

uint8_t w25qxx_page_program(SPI_TypeDef* SPIx, uint32_t tx_addr, uint8_t* pdata, uint32_t data_len);

uint8_t w25qxx_page_program_multi(SPI_TypeDef* SPIx, uint32_t tx_addr, uint8_t* pdata, uint32_t data_len);

#endif
