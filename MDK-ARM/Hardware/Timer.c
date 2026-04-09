#include "main.h"
#include "tim.h"
#include "Key.h"
#include "Encoder.h"
#include "math.h"
#include "MPU6050.h"
#include "OLED.h"
#include "PID.h"
#include "Motor.h"

float SpeedL,SpeedR;
int16_t AX,AY,AZ,GX,GY,GZ;//MPU6050得到的加速度和角速度原始数据
float AngleAcc,AngleGyro;//加速度计和陀螺仪分别计算出来的角度
float Angle;//互补滤波之后得到的最终角度

Num_Encoder_t Encoder_Left=Encoder_left;
Num_Encoder_t Encoder_Right=Encoder_right;
Num_Motor_t Motor_Left=Motor_left;
Num_Motor_t Motor_Right=Motor_right;

uint16_t Timecount;
uint8_t MPU6050_Data_Flag=0;//MPU6050数据更新标志位，在定时器中断里面置位，在主循环里面轮询检查，如果为1则更新数据并且清零标志位
uint8_t RunFlag;//这个标志位用来控制是否允许PID运算，在定时器中断里面根据角度大小来设置，在主循环里面轮询检查，如果为1则进行PID运算，否则停止电机
PID_t AnglePID,SpeedPID,TurnPID;//定义一个PID结构体变量来存储平衡小车的PID参数和状态

int16_t LeftPWM,RightPWM;
int16_t AvePWM,DifPWM;//利用平均PWM和差分PWM来控制两轮的速度
float AveSpeed, DifSpeed;//平均速度和差分速度，差分速度用来调整转向

void Timer_Init(void)
{
	MX_TIM1_Init();//初始化Timer1的基准时钟
	HAL_TIM_Base_MspInit(&htim1);//使能定时器并且使能更新中断
	HAL_TIM_Base_Start_IT(&htim1);//启动定时器以及中断
	
	PID_Init(&AnglePID,4.5,0.2,3,100,-100,3,250,-25); //初始化PID参数，参数是测试好的
	PID_Init(&SpeedPID,2,0.01,0,20,-20,0,70,-70);
	PID_Init(&TurnPID,4,3,0,50,-50,0,20,-20);
}

uint8_t Get_MPU6050_Data_Flag(void)
{
	if(MPU6050_Data_Flag==1)
	{
		MPU6050_Data_Flag=0;
		return 1;
	}
	return 0;
}


void Timer_Update_Handler(void)
{
	static uint16_t CountTick0;
	static uint16_t CountTick1;
	if (__HAL_TIM_GET_FLAG(&htim1, TIM_FLAG_UPDATE) != RESET)
	{
		
		Key_Tick();//非阻塞式按键检查,20ms进行一次 	

		CountTick0++;
		CountTick1++;
		
		if(CountTick0>=10)//周期10ms，内环角度环
		{
			CountTick0=0;
			MPU6050_GetData(&AX,&AY,&AZ,&GX,&GY,&GZ);
				
			AngleAcc=-atan2(AX,AZ) / 3.14159 * 180;//加速度计得出的俯仰角
			AngleAcc+=1.5;//可以手动解决一下零点的偏移，这样可以让误差更小，根据实际情况来
			//加速度计的问题是会受到加速和减速的影响，导致出现偏差，比如猛然推小车，计算的角度会出现尖峰然后回落
				
			//AngleGyro=AngleGyro + GY / 32768.0 * 2000 * 0.001;//其实就是角速度积分得到角度，GY / 32768.0 * 2000是根据MPU6050手册计算出的真实角速度值，0.001是周期
			//陀螺仪的问题是零飘，GY值不是0，因为偏差而导致其一直累加，导致角度零飘
				
			GY+=30;//可以手动解决一下零点的偏移，这样可以让误差更小，根据实际情况来
			AngleGyro=Angle + GY / 32768.0 * 2000 * 0.01;
			float alpha=0.01;
			Angle=alpha * AngleAcc + (1-alpha) * AngleGyro;//互补滤波得到最终角度，注意陀螺仪角度之前的累加要改成Angle
				
			Timecount=__HAL_TIM_GET_COUNTER(&htim1);

			MPU6050_Data_Flag=1;
				
			if(Angle>=50 || Angle<=-50)
			{
				RunFlag=0;//如果俯仰角过大直接停止PID运算
			}
			
								
			if(RunFlag==1)//允许PID运算，这里进行PID运算
			{
				AnglePID.actual=Angle;
				PID_Culculate(&AnglePID);
				AvePWM=AnglePID.out;
				LeftPWM=AvePWM-DifPWM/2;
				RightPWM=AvePWM+DifPWM/2;

				if (LeftPWM > 100) {LeftPWM = 100;} else if (LeftPWM < -100) {LeftPWM = -100;}
				if (RightPWM > 100) {RightPWM = 100;} else if (RightPWM < -100) {RightPWM = -100;}
				
				Motor_SetSpeed(Motor_left,-LeftPWM);
				Motor_SetSpeed(Motor_right,-RightPWM);
			}
			else
			{
				Motor_SetSpeed(Motor_left,0);
				Motor_SetSpeed(Motor_right,0);
			}
			
		}
		
		if(CountTick1>=50)//外环速度环，以及并联的转向环
		{
			CountTick1=0;
			SpeedL=Encoder_Get(Encoder_left) / 44.0 / 0.05 / 9.27666;//电机输出轴的转速，编码器值/一圈的边沿数/周期/减速比
			SpeedR=Encoder_Get(Encoder_right) / 44.0 / 0.05 / 9.27666;

			AveSpeed=(SpeedL+SpeedR)/2;
			DifSpeed=SpeedR-SpeedL;
			
			if(RunFlag==1)
			{
				SpeedPID.actual=AveSpeed;
				PID_Culculate(&SpeedPID);
				AnglePID.target=SpeedPID.out;//把速度环的输出作为角度环的目标值，这样就形成了一个两层的级联PID控制
				
				TurnPID.actual=DifSpeed;
				PID_Culculate(&TurnPID);
				DifPWM=-TurnPID.out;
			}
			

		}
		
	}		 
}