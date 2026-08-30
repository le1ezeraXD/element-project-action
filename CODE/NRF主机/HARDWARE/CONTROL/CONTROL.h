#ifndef _CONTROL_H
#define _CONTROL_H
#include "sys.h"

/* 电机速度控制相关参数 */ 
 typedef struct
{
	float Encoder;
	float PWM;
	float Target_Speed;
}MOTOR;

// Encoder structure
//编码器结构体
typedef struct  
{
  int A;      
  int B; 
}Encoder;


//编码器数据读取频率
#define   CONTROL_FREQUENCY 100

//黑色轮胎直径
#define	  Black_WheelDiameter   0.046

//编码器精度  电机转一圈编码器数值
#define Encoder_precision  1040

//轮子周长，单位：m
#define Wheel_perimeter 0.14 

float Limit_Pwm_float(float PWM,float pwm_max,float pwm_min);
int Limit_Pwm_int(int PWM,int pwm_max,int pwm_min);
int Velocity_PID_A(float Encoder,float Targrt);
int Velocity_PID_B(float Encoder,float Targrt);
void Get_Velocity_from_Encoder(void);
void Smooth_control(float vx,float vy,float vz);
void Get_OPENMV_Position(void);
void Drive_Motor(float VX,float VZ);

extern MOTOR MOTOA,MOTOB;
extern  Encoder OriginalEncoder;
extern float OPENMV_Velocity;

#endif

