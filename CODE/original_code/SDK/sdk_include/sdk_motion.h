#ifndef __SDK_MOTION_H
#define __SDK_MOTION_H
#include "sys.h"

/**
  * @brief 行走方向
  */
typedef enum {
    WALK_FORWARD  = 1,   //前进
    KEEP_STATIC = 0,     //静止
    WALK_BACKWARD = -1   //后退
} motion_dir_t;

/**
  * @brief  动作模块初始化
  * @retval None
  */
void sdk_motion_init(void);

/**
  * @brief  站立
  * @retval None
  */
void sdk_motion_stand(void);

/**
  * @brief  开始行走
  * @param  dir 行走方向：MOTION_WALK_FORWARD 前进 / MOTION_WALK_BACKWARD 后退
  * @retval None
  */
void sdk_motion_walk(motion_dir_t dir);

/**
  * @brief  蹲下
  * @retval None
  */
void sdk_motion_crouch(void);

/**
  * @brief  趴下
  * @retval None
  */
void sdk_motion_lie_down(void);

/**
  * @brief  运动节拍：5ms 周期调用一次（挂在 TIM4 中断里）
  *         角度更新 + 输出 PWM
  * @retval None
  */
void motion_tick(void);

#endif
