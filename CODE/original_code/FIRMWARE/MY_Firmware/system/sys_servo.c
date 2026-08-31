#include "sys_servo.h"
#include "drv_pwm.h"

void servo_init(void) {
    SERVO sv;
    sv.pulse_width_0 = PULSE_WIDTH_0;
    sv.pulse_width_180 = PULSE_WIDTH_180;
    sv.init_angle = (sv.pulse_width_0 + sv.pulse_width_180) / 2;
    pwm_init(sv.init_angle);
}

void servo_set_angle(float angle) {
    uint16_t pulse;

    // 角度限幅，防止超出机械范围损坏舵机
    if (angle < 0) {
        angle = 0;
    }

    if (angle > 180) {
        angle = 180;
    }

    pulse = PULSE_WIDTH_0 + angle * (PULSE_WIDTH_180 - PULSE_WIDTH_0) / 180;
    
    pwm_set_compare1(pulse);
}
