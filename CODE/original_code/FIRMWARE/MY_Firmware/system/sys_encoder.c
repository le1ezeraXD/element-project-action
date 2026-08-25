#include "sys_encoder.h"
#include "drv_encoder.h"
#include "param.h"


/** 
  * @brief  复位 编码器统计数值
  * @param  TIMx: 选择定时器	
  * @retval None  
  */
void sys_reset_encoder_count(TIM_TypeDef* TIMx, tim_encoder_t* encoder_dev) {
		TIM_SetCounter(TIMx, 0);
		encoder_dev->timx_encoder_cnt = 0;
		TIM_Cmd(TIMx, DISABLE);
}


/** 
  * @brief  获取timx 编码器统计数值
  * @param  TIMx: 选择定时器
  * @retval 返回计数总值
  */
int32_t sys_get_encoder_count(TIM_TypeDef* TIMx, tim_encoder_t* encoder_dev) {
	/* 计算当前总计数值, 当前总计数值 = 计数器当前值 + 溢出次数 * 65536 */
	return (int32_t)(TIM_GetCounter(TIMx) + encoder_dev->timx_encoder_cnt * 65536);
}

/** 
  * @brief  获取timx 编码器旋转插值
  * @param  TIMx: 选择定时器
  * @retval 返回计数总值
  */
int32_t sys_get_encoder_diff(TIM_TypeDef* TIMx, tim_encoder_t* encoder_dev) {
	encoder_dev->current = sys_get_encoder_count(TIMx, encoder_dev);
	encoder_dev->change = encoder_dev->current - encoder_dev->last;
	encoder_dev->last = encoder_dev->current;

	return encoder_dev->change;
}


/** 
  * @brief  编码器初始化
	* @param  TIMx: 选择定时器
  * @retval 初始化编码器设备
  */
void sys_encoder_init(tim_encoder_t* encoder_dev, uint32_t accuracy, uint16_t timx_irq, uint8_t multi) {
	encoder_dev->current = 0;
	encoder_dev->last = 0;
	encoder_dev->change = 0;
	encoder_dev->speed = 0;
	encoder_dev->accuracy	= accuracy;
	encoder_dev->timx_irq = timx_irq;
	encoder_dev->multi = multi;
	encoder_dev->timx_encoder_cnt = 0;
}

/** 在5ms的中断中计算
  * @brief  编码器旋转速度计算
  * @param  TIMx: 选择定时器
  * @retval 返回旋转速度
  */
int32_t sys_encoder_speed(TIM_TypeDef* TIMx, tim_encoder_t* encoder_dev) {
	encoder_dev->current = sys_get_encoder_count(TIMx, encoder_dev);
	encoder_dev->change = encoder_dev->current - encoder_dev->last;
	encoder_dev->last = encoder_dev->current;
	encoder_dev->speed = encoder_dev->change * (1000 / encoder_dev->timx_irq) * 60 / encoder_dev->multi / encoder_dev->accuracy;
	return (int32_t)encoder_dev->speed;
}
