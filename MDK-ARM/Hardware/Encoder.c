#include "main.h"
#include "gpio.h"
#include "tim.h"

typedef enum Num_Encoder
{
	Encoder_left=0,
	Encoder_right
}Num_Encoder_t;

void Encoder_Init(void)
{
	MX_TIM3_Init();
	MX_TIM4_Init();
	HAL_TIM_Base_Start(&htim3);//使能定时器
	HAL_TIM_Base_Start(&htim4);
	HAL_TIM_Encoder_Start(&htim3,TIM_CHANNEL_1);//开启编码器模式
	HAL_TIM_Encoder_Start(&htim3,TIM_CHANNEL_2);
	HAL_TIM_Encoder_Start(&htim4,TIM_CHANNEL_1);
	HAL_TIM_Encoder_Start(&htim4,TIM_CHANNEL_2);
}

int16_t Encoder_Get(Num_Encoder_t Num_Encoder)
{
	int16_t Temp;
	if(Num_Encoder==Encoder_left)
	{
		Temp = __HAL_TIM_GetCounter(&htim3);
		__HAL_TIM_SetCounter(&htim3, 0);
		return Temp;
	}
	else if(Num_Encoder==Encoder_right)
	{
		Temp = __HAL_TIM_GetCounter(&htim4);
		__HAL_TIM_SetCounter(&htim4, 0);
		return Temp;
	}

	return 0;
}
