#ifndef __PARAM_H
#define __PARAM_H
#include <stdint.h>

enum my_gpio_pin_x{
	gpio_pin_0,
	gpio_pin_1,
	gpio_pin_2,
	gpio_pin_3,
	gpio_pin_4,
	gpio_pin_5,
	gpio_pin_6,
	gpio_pin_7,
};

/* 中断优先级 */
// enum irq_level{


// };

typedef struct {
	int16_t gx;
	int16_t gy;
	int16_t gz;
	int16_t ax;
	int16_t ay;
	int16_t az;
	int16_t temperature;
}mpu6050_data_t;

typedef struct {
	int32_t current;    				//当前计数值
  	int32_t last;   	    			//上一次计数值
	int32_t speed;          			//编码器旋转速度
	int32_t change; 				    //编码器变化量
	uint16_t accuracy;		    		//编码器分辨率
	uint8_t multi;			    		//编码器倍频
	int32_t timx_encoder_cnt;		    //计数溢出次数
	int16_t timx_irq;					//定时器扫描时间间隔
	uint8_t encoder_reset;				//编码器复位标志
}tim_encoder_t;


/* 按键(KEY) */
typedef enum {
    KEY_ACTIVE_HIGH = 0,    // 高电平有效(按下为1)
    KEY_ACTIVE_LOW = 1      // 低电平有效(按下为0)
} KEY_ACTIVE_TYPE;

typedef struct {
    uint8_t key_buf;           		//按键缓冲区，用于消抖   key_buf[7:0] 	 0xFF down  0x00 up  0xFF~0x00  未知
    uint8_t key_current;       		//当前按键状态
    uint8_t key_before;        		//前一次按键状态
    uint16_t key_down_time;    		//按键按下时间
    uint8_t key_event;         		//按键事件								key_event[7:6]   01  dowm   10  up   11  long_press
    KEY_ACTIVE_TYPE active_type; 	//按键有效类型
    uint8_t (*read_pin)(void);   	//读取引脚状态的函数指针
}key_state_t;


/* i2c */
typedef struct i2c_params {
	uint8_t dev_addr_w;    		// 设备写地址
	uint8_t dev_addr_r;    		// 设备读地址
	uint8_t reg_addr;         // 寄存器地址 & 从机内存地址
	uint8_t *cmd;							// 命令缓冲区指针
	uint8_t *pdata;           // 数据缓冲区指针
	uint16_t cmd_len;					// 命令数据长度
	uint16_t data_len;        // 数据长度
}i2c_device;

/* spi */
typedef struct spi_params{
	uint32_t addr;
	uint8_t  *tx_data;
	uint8_t  *rx_data;
	uint32_t addr_size;
	uint32_t data_size;
}spi_device;


//typedef struct {


//} GLOBAL_CONFIG_CONST;


/* 全局变量 */
extern key_state_t g_key1, g_key2, g_key3;
extern tim_encoder_t g_encoder_dev;
extern mpu6050_data_t mpu6050_data; 
extern uint8_t g_mpu6050_data_ready;





#endif



