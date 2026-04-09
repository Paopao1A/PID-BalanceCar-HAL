#include "main.h"
#include "PID.h"



void PID_Init(PID_t* PID,float kp,float ki,float kd,float outmax,float outmin,float outoffset,float accumlation_max,float accumlation_min)
{
	PID->kp=kp;
	PID->ki=ki;
	PID->kd=kd;
	PID->actual=0;
	PID->actual_last=0;
	PID->out=0;

	PID->error_accumlation=0;
	PID->error_last=0;
	PID->error_now=0;

	PID->target=0;//每次初始化都得清除目标值，防止因为上次数据出错而导致重启的时候电机突然动作
	PID->outmax=outmax;
	PID->outmin=outmin;
	PID->outoffset=outoffset;

	PID->accumlation_max=accumlation_max;
	PID->accumlation_min=accumlation_min;
}

void PID_Culculate(PID_t* PID)
{
	PID->error_now=PID->target-PID->actual;
	PID->error_accumlation+=PID->error_now;
	
	if(PID->error_accumlation>PID->accumlation_max)
		PID->error_accumlation=PID->accumlation_max;
	else if(PID->error_accumlation<PID->accumlation_min)
		PID->error_accumlation=PID->accumlation_min;
	//PID->out=PID->kp*PID->error_now+PID->ki*PID->error_accumlation+PID->kd*(PID->error_now-PID->error_last);

	//采用微分先行，利用实际值的变化率来预测未来的误差，这样可以让系统响应更快更稳定，特别是对于有较大滞后或者惯性的系统
	PID->out=PID->kp*PID->error_now+PID->ki*PID->error_accumlation-PID->kd*(PID->actual-PID->actual_last);
	
	if(PID->out>PID->outmax)
		PID->out=PID->outmax;
	else if(PID->out<PID->outmin)
		PID->out=PID->outmin;
	
	PID->out+=PID->outoffset;//这个作用于角度环，用于克服摩擦力

	PID->error_last=PID->error_now;
	PID->actual_last=PID->actual;
}
