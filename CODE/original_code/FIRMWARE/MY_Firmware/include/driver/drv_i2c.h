#ifndef __DRV_I2C_H
#define __DRV_I2C_H
#include "sys.h"
#include "drv_systick.h"


// i2c1引脚定义
#define I2C1_PORT    		GPIOB
#define I2C1_SCL_PIN    GPIO_Pin_6
#define I2C1_SDA_PIN    GPIO_Pin_7

// i2c2引脚定义
#define I2C2_PORT    		GPIOB
#define I2C2_SCL_PIN    GPIO_Pin_10
#define I2C2_SDA_PIN    GPIO_Pin_11

//测试函数
#define test  0

//i2c1初始化
void drv_i2c1_init(void);

//i2c2初始化
void drv_i2c2_init(void);


//i2c硬件写多个数据1  不指定地址或者寄存器
uint8_t i2c_master_transmit(I2C_TypeDef* I2Cx, uint8_t device_addr, uint8_t *pdata, uint16_t len);

//i2c硬件读多个数据1  不指定地址或者寄存器
uint8_t i2c_read_master_receive(I2C_TypeDef* I2Cx, uint8_t device_addr_r, uint8_t *pdata, uint16_t len);

//i2c硬件写单个字节2
uint8_t i2c_write_Byte(I2C_TypeDef* I2Cx, uint8_t device_addr, uint8_t addr, uint8_t data);

//i2c硬件读单个字节2
uint8_t i2c_read_Byte(I2C_TypeDef* I2Cx, uint8_t device_addr_w, uint8_t device_addr_r, uint8_t addr, uint8_t *data);

//i2c硬件写多个数据2
uint8_t i2c_write_multibyte(I2C_TypeDef* I2Cx, uint8_t device_addr, uint16_t addr, uint8_t *pdata, uint16_t len);

//i2c硬件读多个数据2
uint8_t i2c_read_multibyte(I2C_TypeDef* I2Cx, uint8_t device_addr_w, uint8_t device_addr_r, uint16_t addr, uint8_t *pdata, uint16_t len);

#endif








