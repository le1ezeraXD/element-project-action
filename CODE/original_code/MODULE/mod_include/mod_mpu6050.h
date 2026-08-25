#ifndef __MOD_MPU6050_H
#define __MOD_MPU6050_H
#include "sys.h"

/* mpu_6050 寄存器 */
#define MPU6050_ID        0x75    /*mpu6050_id*/
#define POWER_MANAGEMENT1 0x6B    /*电源管理寄存器1*/
#define POWER_MANAGEMENT2 0x6C    /*电源管理寄存器2*/
#define SIGNAL_PATH_RESET 0x68    /*信号路径复位*/ 

#define X_SELT_TEST       0x0D    /* x轴加速度&陀螺仪自检寄存器 */ 
#define Y_SELT_TEST       0x0E    /* Y轴加速度&陀螺仪自检寄存器 */ 
#define Z_SELT_TEST       0x0F    /* Z轴加速度&陀螺仪自检寄存器 */ 
#define A_SELT_TEST				0x10		/* X_Y_Z轴加速度自检寄存器低位 */ 

#define SAMPLE_RATE       0x19    /* 采样频率配置寄存器 */ 
#define CONFIGURE         0x1A    /* 低通滤波配置寄存器 */ 

#define GYRO_CONFIGURE    0x1B    /* 陀螺仪配置寄存器 */ 
#define GYRO_XOUT_H       0x43    /* X轴角度值高位 */ 
#define GYRO_XOUT_L       0x44    /* X轴角度值低位 */ 
#define GYRO_YOUT_H       0x45    /* Y轴角度值高位 */ 
#define GYRO_YOUT_L       0x46    /* Y轴角度值低位 */ 
#define GYRO_ZOUT_H       0x47    /* Z轴角度值高位 */ 
#define GYRO_ZOUT_L       0x48    /* Z轴角度值低位 */ 

#define ACCEL_CONFIGURE   0x1C    /* 加速度计配置寄存器 */ 
#define ACCEL_XOUT_H      0x3B    /* X轴角加速度值高位 */ 
#define ACCEL_XOUT_L      0x3C    /* X轴角加速度值低位 */ 
#define ACCEL_YOUT_H      0x3D    /* Y轴角加速度值高位 */ 
#define ACCEL_YOUT_L      0x3E    /* Y轴角加速度值低位 */ 
#define ACCEL_ZOUT_H      0x3F    /* Z轴角加速度值高位 */ 
#define ACCEL_ZOUT_L      0x40    /* Z轴角加速度值低位 */ 

#define TEMPERATURE_H     0x41    /* 温度值高位 */ 
#define TEMPERATURE_L     0x42    /* 温度值低位 */ 

#define INT_STATUS        0x3A    /* 中断状态寄存器 */ 
#define INT_ENABLE        0x38    /* 中断使能寄存器 */ 
#define INT_BYPASS_CON    0x37    /* 中断引脚配置寄存器 */ 

#define MOTION_DETECTIONG_THRESHOLD    0x1F    /* 运动检测阈值寄存器 */ 

void mod_mpu6050_init(I2C_TypeDef* I2Cx);

uint8_t mpu_6050_get_id(I2C_TypeDef* I2Cx);

void mpu_6050_get_gyro_accel(I2C_TypeDef* I2Cx, mpu6050_data_t* mpu6050);


uint8_t mpu6050_get_int_status(I2C_TypeDef* I2Cx);

#endif


