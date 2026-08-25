#include "sys_i2c.h"
#include "drv_i2c.h"

/** 
  * @note   针对不同I2C控制器使用不同的底层驱动函数
	*					使用I2C1的模块   发送函数使用cmd传输数据
	*					使用I2C2的模块   发送函数使用data传输数据
  */


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
uint8_t sys_i2c_write(I2C_TypeDef* I2Cx, i2c_device* dev){

	int8_t ret = 0;
	int8_t retry_count = 0;

	if(I2Cx == I2C1) {
			for (retry_count = 0; retry_count < 3; retry_count++) {
					ret = i2c_master_transmit(I2Cx, dev->dev_addr_w, dev->cmd, dev->cmd_len);
					if (ret) {
						LOG_ERROR("sys_i2c_write_retry: %d;", retry_count);
						delay_ms(1);
						continue;
					}
					break;
			}
			return (retry_count >= 3) ? 1 : 0;
	}
	else{
			for (retry_count = 0; retry_count < 3; retry_count++) {
					ret = i2c_write_multibyte(I2Cx, dev->dev_addr_w, dev->reg_addr, dev->pdata, dev->data_len);
					if (ret) {
						LOG_ERROR("sys_i2c_write_retry: %d;", retry_count);
						delay_ms(1);
						continue;
					}
					break;
			}
			return (retry_count >= 3) ? 1 : 0;
	}
}

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
  *         - I2C1:  使用i2c_master_transmit  i2c_read_master_receive
  *         - I2C2:  使用i2c_read_multibyte
  */
uint8_t sys_i2c_write_read(I2C_TypeDef* I2Cx, i2c_device* dev){

	int8_t ret = 0;
	int8_t retry_count = 0;

	if(I2C1 == I2Cx) {
			for (retry_count = 0; retry_count < 5; retry_count++) {
					ret = i2c_master_transmit(I2Cx, dev->dev_addr_w, dev->cmd, dev->cmd_len);
					if(ret){
						LOG_ERROR("sys_i2c_write_read_retry: ret:%d,retry:%d;", ret, retry_count);
						delay_ms(1);
						continue;
					}
					
					ret |= i2c_read_master_receive(I2Cx, dev->dev_addr_r, dev->pdata, dev->data_len);
					if (ret) {
						LOG_ERROR("sys_i2c_read_retry: ret:%d,retry:%d;", ret, retry_count);
						delay_ms(1);
						continue;
					}
					break;
			}
			return (retry_count >= 5) ? 1 : 0;
	}
	else{
			for (retry_count = 0; retry_count < 5; retry_count++) {
					ret = i2c_read_multibyte(I2Cx, dev->dev_addr_w, dev->dev_addr_r, dev->reg_addr, dev->pdata, dev->data_len);
					if (ret) {
						LOG_ERROR("sys_i2c_read_retry: ret:%d,retry:%d;", ret, retry_count);
						delay_ms(1);
						continue;
					}
					break;
			}
			return (retry_count >= 5) ? 1 : 0;
	}
}











