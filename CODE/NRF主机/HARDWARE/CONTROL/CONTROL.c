#include "control.h"
#include "motor.h"
#include "key.h"
#include "uart.h"
#include "encoder.h"
#include "string.h"
#include "time.h"
#include "led.h"

 MOTOR MOTOA,MOTOB;
 Encoder OriginalEncoder;

float Track_width = 0.145f;  /* 主动轮轮距 */ 
float Move_X,Move_Z;  //openmv寻线两轴速度处理
u16 OPENMV_Meadian ;  //中间点偏差
u16 Stop;


/* openmv寻线 */
float OPENMV_Velocity = 300;  //寻线速度  mm/s   0.35m/s
float OPENMV_Velocity_KP = 100;
float OPENMV_Velocity_KD = 100 ;

/* 速度控制PID参数 */ 
	float Velocity_KP = 100;
  float Velocity_KI = 100;

void TIM5_IRQHandler(void)
{
	static int ZYJ = 0,ZYJ1 = 0;
	int CNT = 0;
	if(TIM_GetFlagStatus(TIM5,TIM_FLAG_Update) != RESET)
	{
		TIM_ClearFlag(TIM5,TIM_FLAG_Update);
		/*赛题一 */
		/*****************************************/
		if(FLAG == 1)
		{
			ZYJ = 1;
			if(FLAG == 1 && ZYJ1 == 1) CNT++;
		}else ZYJ = 0;
		if(Stop == 0 && CNT>10000 )
		{
			ZYJ = 0;
		}
		if(ZYJ == 1)
		{
			Get_OPENMV_Position();
			Get_Velocity_from_Encoder();
			MOTOA.PWM = Velocity_PID_A(MOTOA.Encoder,MOTOA.Target_Speed);
			MOTOB.PWM = Velocity_PID_B(MOTOB.Encoder,MOTOB.Target_Speed);
			LOAP_PWM(MOTOA.PWM,MOTOB.PWM);
			ZYJ1 = 1;
		}else LOAP_PWM(0,0);
	   /*****************************************/
		
		/*赛题二 */
		/*****************************************/
		if(FLAG == 2)
		{
			ZYJ = 2;
			if(FLAG == 2 && ZYJ1 == 2) CNT++;
		}else ZYJ = 0;
		if(Stop == 0 && CNT>10000 )
		{
			ZYJ = 0;
		}
		if(ZYJ == 2)
		{
			Get_OPENMV_Position();
			Get_Velocity_from_Encoder();
			MOTOA.PWM = Velocity_PID_A(MOTOA.Encoder,MOTOA.Target_Speed);
			MOTOB.PWM = Velocity_PID_B(MOTOB.Encoder,MOTOB.Target_Speed);
			LOAP_PWM(MOTOA.PWM,MOTOB.PWM);
			ZYJ1 = 2;
		}else LOAP_PWM(0,0);
	   /*****************************************/
		
	}
}

/*	编码器转化为车轮速度 */
void Get_Velocity_from_Encoder(void)
{
	  float ENCODER_A,ENCODER_B;
	
	/*右轮*/	OriginalEncoder.A =  -Read_Speed(2);
	/*左轮*/	OriginalEncoder.B =  Read_Speed(4);
	
	ENCODER_A =  OriginalEncoder.A;
	ENCODER_B =  OriginalEncoder.B;
	
	/*编码器原始数据转化为车轮速度 */
	 MOTOA.Encoder = ENCODER_A * CONTROL_FREQUENCY * Wheel_perimeter / Encoder_precision;
	 MOTOB.Encoder = ENCODER_B * CONTROL_FREQUENCY * Wheel_perimeter / Encoder_precision;
	
}


/*
根据三轴目标速度计算个车轮目标转速
入口参数  ： X Y Z轴目标运动速度
无返回值
*/
void Drive_Motor(float VX,float VZ)
{
	float wheel_Target_speed =0.3;  //车轮目标速度限幅
	
	MOTOA.Target_Speed = VX + VZ * Track_width / 2.0f;  //计算出右轮目标速度
	MOTOB.Target_Speed = VX - VZ * Track_width / 2.0f;  //计算出左轮目标速度	
	
	/* 车轮目标速度限幅 */
	MOTOA.Target_Speed=Limit_Pwm_float( MOTOA.Target_Speed,wheel_Target_speed,-wheel_Target_speed); 
	MOTOB.Target_Speed=Limit_Pwm_float( MOTOB.Target_Speed,wheel_Target_speed,-wheel_Target_speed); 
}




/* 限幅函数 */
float Limit_Pwm_float(float PWM,float pwm_max,float pwm_min)
{
	if(PWM>pwm_max) 
		return pwm_max;
	else if(PWM<pwm_min) 
		return pwm_min;
	else
		return PWM;
}

int Limit_Pwm_int(int PWM,int pwm_max,int pwm_min)
{
	if(PWM>pwm_max) 
		return pwm_max;
	else if(PWM<pwm_min) 
		return pwm_min;
	else
		return PWM;
}



/*
速度环 PI 控制
入口参数：编码器测量值（实际速度），目标速度
返回值：电机pwm
PWM+=KP[e(k) - e(k-1)] + KI * e(k) +KD[e(k) - 2e(k-1) + e(k-2)]
e(k) : 本次偏差
e(k-1) : 上一次偏差
pwm 输出增量
速度闭环中，只用PI控制
PWM+=KP[e(k) - e(k-1)] + KI * e(k)
*/

/*  左轮  */ 
int Velocity_PID_A(float Encoder,float Target)
{
	 static float Bias,Pwm,Last_bias;
	 Bias=Target-Encoder; //Calculate the deviation //计算偏差
	 Pwm+=Velocity_KP*(Bias-Last_bias)+Velocity_KI*Bias; 
	 if(Pwm>5000)Pwm=5000;
	 if(Pwm<-5000)Pwm=-5000;
	 Last_bias=Bias; //Save the last deviation //保存上一次偏差 
	 return Pwm; 
}

/*  右轮 */
int Velocity_PID_B(float Encoder,float Target)
{
	 static float Bias,Pwm,Last_bias;
	 Bias=Target-Encoder; //Calculate the deviation //计算偏差
	 Pwm+=Velocity_KP*(Bias-Last_bias)+Velocity_KI*Bias; 
	 if(Pwm>5000)Pwm=5000;
	 if(Pwm<-5000)Pwm=-5000;
	 Last_bias=Bias; //Save the last deviation //保存上一次偏差 
	 return Pwm; 
}

///* openmv寻线 */
//float OPENMV_Velocity = 350;
//float OPENMV_Velocity_KP = 0;
//float OPENMV_Velocity_KD = 0;
void Get_OPENMV_Position(void)
{
	static float Bias,Last_Bias;
	float move_z = 0;
	
	Move_X = OPENMV_Velocity;     /*  openmv寻线线速度 */
//	Bias = OPENMV_Meadian - 64;   /*  提取偏差，64为寻巡线中心点  */
	Bias = OPENMV_Meadian;                                                     
	move_z = OPENMV_Velocity_KP * Bias * 0.1f+ OPENMV_Velocity_KD * (Bias - Last_Bias)* 0.1f;  /* PD控制，让小车靠近中点 */
	
	if(Move_X<0) move_z = -move_z;
	Move_Z = move_z * OPENMV_Velocity/67000;    //差速控制
	
	/* 单位转换  mm/s->m/s */
	Move_X = Move_X/1000;
	Move_Z = Move_Z;
	
	/* 得到控制目标值，进行运动学分析 */
	Drive_Motor(Move_X,Move_Z);
}



