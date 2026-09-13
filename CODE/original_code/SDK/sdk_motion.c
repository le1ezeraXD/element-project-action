#include "sdk_motion.h"
#include "sys_servo.h"

//站立角度
#define STAND_ANGLE       90.0f
//蹲下姿态：前腿/后腿角度
#define CROUCH_ANGLE_F    50.0f
#define CROUCH_ANGLE_R    130.0f
// 趴下姿态：前腿/后腿角度
#define LIE_ANGLE_F       15.0f
#define LIE_ANGLE_R       165.0f

//行走摆幅（度），越大步幅越大
#define WALK_AMP          20.0f

//每 5ms 允许的最大角度变化（度），3°/5ms -> 300°/500ms
#define EASE_STEP         3.0f

//舵机角度范围
#define MIN_ANGLE         0.0f
#define MAX_ANGLE         180.0f

#define LEG_NUM           4

typedef enum {
    MOTION_ST_STAND = 0,   //站立
    MOTION_ST_WALK,        //行走
    MOTION_ST_CROUCH,      //蹲下
    MOTION_ST_LIE,         //趴下
} motion_state_t;

typedef struct {
    //整体属性
    uint8_t  ch[LEG_NUM];       //每个舵机对应的通道
    int8_t   dir;               //行走方向
    int8_t   leg_dir[LEG_NUM];  //行走时每个腿的摆动方向
    motion_state_t state;       //当前状态

    //各个状态的角度
    float stand_angle;            //站立角度
    float crouch_angle[LEG_NUM];  //蹲下角度
    float lie_angle[LEG_NUM];     //趴下角度

    //动态属性
    float cur_angle[LEG_NUM];         //当前角度
    float tgt_angle[LEG_NUM];         //目标角度
} MOTION_t;

MOTION_t motion_ = {
    .ch = {1,2,3,4},
    .dir = KEEP_STATIC,
    .leg_dir = {1,-1,1,-1},
    .state = MOTION_ST_STAND,
    .stand_angle = STAND_ANGLE,
    .crouch_angle = {CROUCH_ANGLE_F, CROUCH_ANGLE_R, CROUCH_ANGLE_F, CROUCH_ANGLE_R},
    .lie_angle = {LIE_ANGLE_F, LIE_ANGLE_R,LIE_ANGLE_F, LIE_ANGLE_R}
};

/**
  * @brief  根据当前状态计算四腿目标角度
  */
static void motion_update_targets(MOTION_t* motion) {
    for(uint8_t i = 0; i < LEG_NUM; i++) {
        switch(motion->state) {
            case MOTION_ST_STAND:       //站立
                motion->tgt_angle[i] = motion->stand_angle;
                break;
            case MOTION_ST_CROUCH:      //蹲下
                motion->tgt_angle[i] = motion->crouch_angle[i];
                break;
            case MOTION_ST_LIE:         //趴下
                motion->tgt_angle[i] = motion->lie_angle[i];
                break;
            case MOTION_ST_WALK:        //行走
                motion->tgt_angle[i] = motion->stand_angle + EASE_STEP;
                if(motion->tgt_angle[i] > motion->stand_angle + motion->leg_dir[i] * WALK_AMP) {
                    motion->tgt_angle[i] = motion->tgt_angle[i] + (-motion->leg_dir[i]) * EASE_STEP;
                }
                else if (motion->tgt_angle[i]  < motion->stand_angle - WALK_AMP) {
                    motion->tgt_angle[i] = motion->tgt_angle[i] + motion->leg_dir[i] * EASE_STEP;
                }
                break;
            default:
                break;
        }
    }
}

/**
  * @brief  动作模块初始化
  */
void sdk_motion_init(void) {
    servo_init();
}

/**
  * @brief  站立
  */
void set_stand(void) {
    motion_.state = MOTION_ST_STAND;
}

/**
  * @brief  行走
  */
void set_walk(motion_dir_t dir) {
    motion_.dir = dir;
    motion_.state = MOTION_ST_WALK;
}

/**
  * @brief  蹲下
  */
void set_crouch(void) {
    motion_.state = MOTION_ST_CROUCH;
}

/**
  * @brief  趴下
  */
void set_lie_down(void) {
    motion_.state = MOTION_ST_LIE;
}

/**
  * @brief  运动节拍（5ms 周期调用）
  */
void motion_tick(void) {
    uint8_t i;

    motion_update_targets(&motion_);

    for (i = 0; i < LEG_NUM; i++) {
        float diff = motion_.tgt_angle[i] - motion_.cur_angle[i];
        if (diff > EASE_STEP) {
            diff = EASE_STEP;
        } else if (diff < -EASE_STEP) {
            diff = -EASE_STEP;
        }
        motion_.cur_angle[i] += diff;

        if (motion_.cur_angle[i] < MIN_ANGLE) motion_.cur_angle[i] = MIN_ANGLE;
        if (motion_.cur_angle[i] > MAX_ANGLE) motion_.cur_angle[i] = MAX_ANGLE;

        servo_set_angle(motion_.ch[i], motion_.cur_angle[i]);
    }
}
