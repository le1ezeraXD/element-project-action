#include "sys_key.h"
#include "drv_key.h"



/**
  * @brief  按键引脚读取函数
  * @param  None
  * @retval None
  */
uint8_t read_key1(void) { return KEY1_PIN; }
uint8_t read_key2(void) { return KEY2_PIN; }
uint8_t read_key3(void) { return KEY3_PIN; }

/**
  * @brief  初始化按键状态
  * @param  key: 按键状态结构体指针
  * @param  active_type: 按键有效类型
  * @param  read_pin: 读取引脚状态
  * @retval None
  */
void sys_key_init(key_state_t *key, KEY_ACTIVE_TYPE active_type, uint8_t (*read_pin)(void)) {
    key->key_buf = 0;
    key->key_current = 0;
    key->key_before = 0;
    key->key_down_time = 0;
    key->key_event = 0;
    key->active_type = active_type;
    key->read_pin = read_pin;
}

/**
  * @brief  读取按键物理状态并转换为逻辑状态
  * @param  key: 按键状态结构体指针
  * @retval 逻辑状态（1-按下，0-释放）
  */
static uint8_t read_key_logicstate(key_state_t *key) {
    uint8_t physical_state = key->read_pin();
    
    if (key->active_type == KEY_ACTIVE_HIGH) {
			return physical_state;  // 高电平有效: 1表示按下
    } 
		else {
			return !physical_state; // 低电平有效: 0转换为1表示按下
    }
}

/***如果 key 参数为 NULL，会直接导致 HardFault：***/
/**	key_msg_buf 高两位存储up down long_press
  * @brief  获取按键key状态
  * @param  key: 按键状态结构体指针
  * @retval None
  */
void sys_get_key_state(key_state_t *key) {
	
	//空指针检查
	if (key == NULL || key->read_pin == NULL) {
			return;
	}
	
	//读取逻辑状态
	uint8_t logic_state = read_key_logicstate(key);
	
	key->key_buf = (key->key_buf << 1) | logic_state;
	
	if(key->key_buf == 0x00) { 			//up
		key->key_current = 0;
	}
	else if(key->key_buf == 0xFF) { 	//down
		key->key_current = 1;
	}
	//检测状态变化
	if(key->key_current != key->key_before){
		key->key_event &=~(0x3<<6);   //clear high 2bit
		
		if(key->key_current){
			//down
			key->key_event |= (1<<6);
			key->key_down_time = 0;    	 //clear key_time
		}
		else{
			//up
			key->key_event |= (1<<7);
		}
		key->key_before = key->key_current;
	}
	//处理长按检测
	if(key->key_current){
		key->key_down_time++;
		if(key->key_down_time >= LONG_PRESS_TIME){
			key->key_event &=~0xc0;
			key->key_event |= (0x3<<6);  //long_press
			key->key_down_time = 0;			 //重置计时
		}
	}
}

/**
  * @brief  获取按键事件
  * @param  key: 按键状态结构体指针
  * @retval 按键事件
  */
uint16_t sys_key_mes_disp(key_state_t *key) {
	
    uint8_t event = key->key_event >> 6;
    
    // 读取后清除事件
    key->key_event = 0;
    
    switch(event) {
        case 0x01: return KEY_DOWN;
        case 0x02: return KEY_UP;
        case 0x03: return KEY_LONG_PRESS;
        default: return 0;
    }
}


