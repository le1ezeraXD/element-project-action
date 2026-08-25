#ifndef __SYS_I2C_H
#define __SYS_I2C_H
#include "sys.h"
#include "drv_systick.h"

/** 
  * @brief  I2C主设备写数据（支持重试机制）
  * @param  I2Cx: I2C控制器 (I2C1, I2C2)
  *         dev:  I2C设备信息结构体指针
  *               - dev_addr_w: 设备写地址
  *               - dev_addr_r: 设备读地址  
  *               - reg_addr:   寄存器地址
  *               - pdata:      数据缓冲区指针
  *               - len:        数据长度
  * @retval 成功返回0，失败返回1
  * @note   针对不同I2C控制器使用不同的底层驱动函数
  *         - I2C1:  使用i2c_master_transmit
  *         - I2C2:  使用i2c_write_multibyte
  */
uint8_t sys_i2c_write(I2C_TypeDef* I2Cx, i2c_device* dev);

/** 
  * @brief  I2C主设备读数据（支持重试机制）
  * @param  I2Cx: I2C控制器实例 (I2C1, I2C2)
  *         dev:  I2C设备信息结构体指针
  *               - dev_addr_w: 设备写地址
  *               - dev_addr_r: 设备读地址  
	*								-	*cmd:				命令缓冲区指针
	*								-	*pdata:     数据缓冲区指针
	*								-	cmd_len:		命令数据长度
	*								-	data_len:   数据长度
  * @retval 成功返回0，失败返回1
  * @note   针对不同I2C控制器使用不同的底层驱动函数
  *         - I2C1:  使用i2c_read_master_receive
  *         - I2C2:  使用i2c_read_multibyte
  */
uint8_t sys_i2c_write_read(I2C_TypeDef* I2Cx, i2c_device* dev);



#endif



