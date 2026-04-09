#include "main.h"
#include "tim.h"

void PWM_Init(void)
{
  MX_TIM2_Init();
	HAL_TIM_Base_MspInit(&htim2);
	HAL_TIM_Base_Start(&htim2);//使能定时器2
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);//开启PWMA输出
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);//开启PWMB输出
}

void PWM_SetCompare1(uint16_t Compare)
{
	__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1,Compare);
}

void PWM_SetCompare2(uint16_t Compare)
{
	__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_2,Compare);
}
