#ifndef __DRV_SPI_H
#define __DRV_SPI_H
#include "sys.h"

#define SPI2_PORT   		GPIOB
#define SPI2_NSS_PIN   	GPIO_Pin_12
#define SPI2_SCK_PIN	 	GPIO_Pin_13
#define SPI2_MOSI_PIN		GPIO_Pin_15
#define SPI2_MISO_PIN		GPIO_Pin_14

#define SPI_TDR_EMPTY  0x02
#define SPI_RDR_EMPTY  0x01


// 快速发送一个字节(不关心接收)
#define SPI_SEND_BYTE(spi, data) do { \
																			while (!((spi)->SR & SPI_SR_TXE)); \
																			(spi)->DR = (data); \
																	} while(0)

// 快速接收一个字节(发送0xFF)
#define SPI_RECEIVE_BYTE(spi) ({ \
																	while (!((spi)->SR & SPI_SR_TXE)); \
																	(spi)->DR = 0xFF; \
																	while (!((spi)->SR & SPI_SR_RXNE)); \
																	(spi)->DR; \
															})

//初始化spi2
void drv_spi2_init(void);
															
uint8_t drv_spi_tran_bulk(SPI_TypeDef* SPIx, uint8_t *tx_data, uint8_t *rx_data, uint16_t size);
uint8_t drv_spi_send_byte(SPI_TypeDef* SPIx, uint8_t* data);
uint8_t drv_spi_receive_byte(SPI_TypeDef* SPIx, uint8_t* data);

#endif


