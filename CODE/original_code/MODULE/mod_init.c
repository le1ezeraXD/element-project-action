#include "mod_init.h"
#include "mod_oled.h"
#include "mod_at24cxx.h"
#include "mod_w25qxx.h"
#include "mod_mpu6050.h"


/** 
  * @brief  硬件驱动初始化
  * @param  None
  * @retval None
  */
void module_init(void) {
	mod_oled_init();
}
	



