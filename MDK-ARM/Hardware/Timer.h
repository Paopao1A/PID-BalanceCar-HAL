#ifndef __TIMER_H
#define __TIMER_H

#include "PID.h"

void Timer_Init(void);
extern void Timer_Update_Handler(void);
uint8_t Get_MPU6050_Data_Flag(void);

extern float SpeedL,SpeedR;
extern int16_t AX,AY,AZ,GX,GY,GZ;
extern float AngleAcc,AngleGyro;
extern float Angle;
extern uint16_t Timecount;
extern uint8_t RunFlag;
extern PID_t AnglePID,SpeedPID,TurnPID;
extern int16_t DifPWM;
extern float AveSpeed,DifSpeed;

#endif
