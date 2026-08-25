#ifndef __SYS_SPI_H
#define __SYS_SPI_H
#include "sys.h"

/*Ð´ÃüÁî*/
uint8_t sys_spi_write_cmd(SPI_TypeDef* SPIx, uint8_t cmd);
/*¶Á×´Ì¬¼Ä´æÆ÷*/
uint8_t sys_spi_read_busy(SPI_TypeDef* SPIx, uint8_t* status);
/*Ð´Êý¾Ý*/
uint8_t sys_spi_write_data(SPI_TypeDef* SPIx, spi_device* dev);
/*¶ÁÊý¾Ý*/
uint8_t sys_spi_read_data(SPI_TypeDef* SPIx, spi_device* dev);


#endif



