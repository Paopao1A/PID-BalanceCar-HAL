#ifndef __MOTOR_H
#define __MOTOR_H

typedef enum Num_Motor
{
	Motor_left=0,
	Motor_right
}Num_Motor_t;


void Motor_Init(void);
void Motor_SetSpeed(Num_Motor_t Num_Motor,int8_t Speed);



#endif
