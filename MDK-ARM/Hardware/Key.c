#include "main.h"                 // Device header
#include "gpio.h"

uint8_t Key_Num;


uint8_t Key_GetNum(void)
{
	uint8_t Temp;
	if (Key_Num)
	{
		Temp = Key_Num;
		Key_Num = 0;
		return Temp;
	}
	return 0;
}

uint8_t Key_GetState(void)
{
	if (HAL_GPIO_ReadPin(GPIOB, K1_Pin) == 0)
	{
		return 1;
	}
	else if (HAL_GPIO_ReadPin(GPIOB, K2_Pin) == 0)
	{
		return 2;
	}
	else if (HAL_GPIO_ReadPin(GPIOA, K3_Pin) == 0)
	{
		return 3;
	}
	else if (HAL_GPIO_ReadPin(GPIOA, K4_Pin) == 0)
	{
		return 4;
	}
	return 0;
}

void Key_Tick(void)
{
	static uint8_t Count;
	static uint8_t CurrState, PrevState;
	
	Count ++;
	if (Count >= 20)
	{
		Count = 0;
		
		PrevState = CurrState;
		CurrState = Key_GetState();
		
		if (CurrState == 0 && PrevState != 0)
		{
			Key_Num = PrevState;
		}
	}
}
