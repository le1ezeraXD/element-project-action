#ifndef __SYS_SERVO_H__
#define __SYS_SERVO_H__

#include "sys.h"

#define PULSE_WIDTH_0       500
#define PULSE_WIDTH_180     2500

void servo_init(void);
void servo_set_angle(float angle);

#endif
