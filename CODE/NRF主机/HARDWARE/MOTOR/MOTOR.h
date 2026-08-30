#ifndef _MOTOR_H
#define _MOTOR_H
#include "sys.h"

/* 舵机&电机 */ 
void MOTOR_Init(void);

 
 /* 取绝对值  */ 
int ZYJ_abs(int a);

/* 赋值函数 */ 
void LOAP_PWM(int motor_left,int motor_right);

/* 电机IO */ 
#define Ain1  PDout(4)   
#define Ain2  PDout(5)

/*反接*/
#define Bin1  PDout(6)   
#define Bin2  PDout(7)

//#define PWMA TIM1->CCR1        //PA8
//#define PWMB TIM1->CCR4        //PA11



#endif

