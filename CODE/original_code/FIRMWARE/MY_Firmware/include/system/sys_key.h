#ifndef __SYS_KEY_H
#define __SYS_KEY_H
#include "sys.h"

// 按键引脚定义
#define KEY1_PIN       PAin(0)   // PA0 按下为1(下拉)
#define KEY2_PIN       PEin(3)   // PE3 按下为0(上拉)
#define KEY3_PIN       PEin(4)   // PE4 按下为0(上拉)

#define KEY_UP    				101
#define KEY_DOWN  				102
#define KEY_LONG_PRESS  	103
#define LONG_PRESS_TIME 	600            //5ms的时间基准  计数600次

/*按键状态初始化*/
void sys_key_init(key_state_t *key, KEY_ACTIVE_TYPE active_type, uint8_t (*read_pin)(void));

/*读取按键物理状态并转换为逻辑状态*/
static uint8_t read_key_logicstate(key_state_t *key);

/*获取按键key状态*/
void sys_get_key_state(key_state_t *key);

/*按键信息传输*/
uint16_t sys_key_mes_disp(key_state_t *key);

/*按键引脚读取函数*/
uint8_t read_key1(void);
uint8_t read_key2(void);
uint8_t read_key3(void);

//	sys_key_init(&key1, KEY_ACTIVE_HIGH, read_key1);  // PA0: 下拉，按下为1
//	sys_key_init(&key2, KEY_ACTIVE_LOW, read_key2);   // PE3: 上拉，按下为0
//	sys_key_init(&key3, KEY_ACTIVE_LOW, read_key3);   // PE4: 上拉，按下为0

#endif


