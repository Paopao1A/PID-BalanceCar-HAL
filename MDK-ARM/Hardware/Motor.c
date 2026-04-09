#include "main.h"
#include "PWM.h"
#include "gpio.h"

typedef enum Num_Motor
{
	Motor_left=0,
	Motor_right
}Num_Motor_t;


void Motor_Init(void)
{
	PWM_Init();
}

void Motor_SetSpeed(Num_Motor_t Num_Motor,int8_t Speed)
{
	
	if(Num_Motor==Motor_left)
	{
		if (Speed >= 0)
		{
			HAL_GPIO_WritePin(GPIOB, AIN1_Pin,GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOB, AIN2_Pin,GPIO_PIN_RESET);
			PWM_SetCompare1(Speed);
		}
		else
		{
			HAL_GPIO_WritePin(GPIOB, AIN1_Pin,GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOB, AIN2_Pin,GPIO_PIN_SET);
			PWM_SetCompare1(-Speed);
		}
	}
	else if(Num_Motor==Motor_right)	
	{
		if (Speed >= 0)
		{
			HAL_GPIO_WritePin(GPIOB, BIN1_Pin,GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOB, BIN2_Pin,GPIO_PIN_SET);
			PWM_SetCompare2(Speed);
		}
		else
		{
			HAL_GPIO_WritePin(GPIOB, BIN1_Pin,GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOB, BIN2_Pin,GPIO_PIN_RESET);
			PWM_SetCompare2(-Speed);
		}
	}

}
