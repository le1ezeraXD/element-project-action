#include "mod_mpu6050.h"
#include "sys_i2c.h"
#include "param.h"

/**
	******************************************************************************
	* @file    mod_mpu6050.c
	* @author  zyj
	* @version V0.0.1
	* @date    07-12-2025
	* @brief   mpu6050
	******************************************************************************
	*	驱动代码: mpu6050
	* ic: mpu6050  
	*    
	******************************************************************************
	*/
/* 1101000x 默认AD0接地*/
/* 从机地址 7位地址0x68*/
#define MPU_6050_ADDR 0xD0

i2c_device mpu6050_dev = {
	.dev_addr_w = MPU_6050_ADDR,    			// 设备写地址
	.dev_addr_r = MPU_6050_ADDR | 0x01  		// 设备读地址
};

/**
 * @brief  mpu6050陀螺仪驱动配置
 * @param  
 * @param  addr:  
 * @return 
 */
void mod_mpu6050_init(I2C_TypeDef* I2Cx) {
	/* initialization */
	uint8_t write_data = 0;
	
	/*器件复位*/
	mpu6050_dev.reg_addr = POWER_MANAGEMENT1;
	write_data = 0x80;
	mpu6050_dev.pdata = &write_data;
	mpu6050_dev.data_len = 1;
	sys_i2c_write(I2Cx, &mpu6050_dev);
	
	delay_ms(100);

	/*唤醒并配置电源模式和时钟源*/
	mpu6050_dev.reg_addr = POWER_MANAGEMENT1;
	write_data = 0x01;
	mpu6050_dev.pdata = &write_data;
	mpu6050_dev.data_len = 1;
	sys_i2c_write(I2Cx, &mpu6050_dev);
	
	/*所有轴均不待机*/
	mpu6050_dev.reg_addr = POWER_MANAGEMENT2;
	write_data = 0x00;
	mpu6050_dev.pdata = &write_data;
	mpu6050_dev.data_len = 1;
	sys_i2c_write(I2Cx, &mpu6050_dev);
	
	/*陀螺仪配置*/
	mpu6050_dev.reg_addr = GYRO_CONFIGURE;
	write_data = 0x18;	
	mpu6050_dev.pdata = &write_data;   /*full scale range = ±2000°/s & not allow self test*/
	mpu6050_dev.data_len = 1;
	sys_i2c_write(I2Cx, &mpu6050_dev);

	/*加速度计配置*/
	mpu6050_dev.reg_addr = ACCEL_CONFIGURE;
	write_data = 0x18;	
	mpu6050_dev.pdata = &write_data;   /*full scale range = ±16g & not allow self test*/
	mpu6050_dev.data_len = 1;
	sys_i2c_write(I2Cx, &mpu6050_dev);

	/*配置采样速率*/
	mpu6050_dev.reg_addr = SAMPLE_RATE;
	write_data = 0x09;
	mpu6050_dev.pdata = &write_data;
	mpu6050_dev.data_len = 1;
	sys_i2c_write(I2Cx, &mpu6050_dev);

	/*配置mpu6050自带低通滤波*/
	/*when DLPF enable, the gyro output rate is set 1KHz*/
	mpu6050_dev.reg_addr = CONFIGURE;
	write_data = 0x06;
	mpu6050_dev.pdata = &write_data;
	mpu6050_dev.data_len = 1;
	sys_i2c_write(I2Cx, &mpu6050_dev);

	/*中断引脚配置*/
	mpu6050_dev.reg_addr = INT_BYPASS_CON;
	write_data = 0xB0;
	mpu6050_dev.pdata = &write_data;  //中断脚被配置为低电平有效,每次触发直到中断被清除,再次读取中断状态寄存器可以清除
	mpu6050_dev.data_len = 1;
	sys_i2c_write(I2Cx, &mpu6050_dev);

	/*使能数据就绪中断*/
	mpu6050_dev.reg_addr = INT_ENABLE;
	write_data = 0x01;
	mpu6050_dev.pdata = &write_data;
	mpu6050_dev.data_len = 1;
	sys_i2c_write(I2Cx, &mpu6050_dev);

}


/**
 * @brief  mpu6050设备id
 * @param  
 * @return 
 */
uint8_t mpu_6050_get_id(I2C_TypeDef* I2Cx) {
	uint8_t dev_id = 0;
	mpu6050_dev.reg_addr = MPU6050_ID;
	mpu6050_dev.pdata = &dev_id;
	mpu6050_dev.data_len = 1;
	sys_i2c_write_read(I2Cx, &mpu6050_dev);
	return dev_id;
}


/**
 * @brief  mpu6050获取陀螺仪&加速度计数据
 * @param  
 * @return 
 */
void mpu_6050_get_gyro_accel(I2C_TypeDef* I2Cx, mpu6050_data_t* mpu6050) {
	
	if(g_mpu6050_data_ready) {
		//mpu6050数据准备就绪
		uint8_t buffer[14] = {0};

		/* x y z axis gyro & axis accel & temperature*/
		mpu6050_dev.reg_addr = ACCEL_XOUT_H;
		mpu6050_dev.pdata = buffer;
		mpu6050_dev.data_len = sizeof(buffer);
		sys_i2c_write_read(I2Cx, &mpu6050_dev);

		if (sys_i2c_write_read(I2Cx, &mpu6050_dev) == 0) {  // 假设返回0表示成功
			/*加速度计数据*/
			mpu6050->ax = ((uint16_t)buffer[0]<<8) |  buffer[1];
			mpu6050->ay = ((uint16_t)buffer[2]<<8) |  buffer[3];
			mpu6050->az = ((uint16_t)buffer[4]<<8) |  buffer[5];

			/*温度数据*/
			mpu6050->temperature = ((uint16_t)buffer[6]<<8) |  buffer[7];

			/*陀螺仪数据*/
			mpu6050->gx = ((uint16_t)buffer[8]<<8) 	|  buffer[9];
			mpu6050->gy = ((uint16_t)buffer[10]<<8) |  buffer[11];
			mpu6050->gz = ((uint16_t)buffer[12]<<8) |  buffer[13];
		}
		else{
			//读取失败处理
			mpu6050->ax = mpu6050->ay = mpu6050->az = 0;
			mpu6050->gx = mpu6050->gy = mpu6050->gz = 0;
		}
		
		g_mpu6050_data_ready = 0;
	}
}

/**
 * @brief  读取中断状态寄存器
 * @param  
 * @return 
 */
uint8_t mpu6050_get_int_status(I2C_TypeDef* I2Cx) {
	uint8_t int_status = 0;
	mpu6050_dev.reg_addr = INT_STATUS;
	mpu6050_dev.pdata = &int_status;
	mpu6050_dev.data_len = 1;
	/*读取中断状态寄存器（会清除MPU6050中断标志）*/
	sys_i2c_write_read(I2Cx, &mpu6050_dev);
	return int_status;
}








