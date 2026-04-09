/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "OLED.h"
#include "Timer.h"
#include "Key.h"
#include "MPU6050.h"
#include "Motor.h"
#include "Encoder.h"
#include <stdio.h>
#include "Uart.h"
#include "PID.h"
#include "string.h"
#include "stdlib.h"
#include "NRF24L01.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
	
uint8_t KeyNum;


int main(void)
{
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
	
	OLED_Init();
	Timer_Init();
	MPU6050_Init();
	Motor_Init();
	Encoder_Init();
	UATR_Init();
	NRF24L01_Init();
	
//	Num_Motor_t Motor_Left=Motor_left;
//	Num_Motor_t Motor_Right=Motor_right;
	
//	printf("Hello World");
	//OLED_Printf(0,0,OLED_8X16,"Hello World");
	//OLED_Update();
	
//	NRF24L01_WriteReg(NRF24L01_SETUP_RETR,0xA5);
//  uint8_t test=NRF24L01_ReadReg(NRF24L01_SETUP_RETR);
//  OLED_Printf(0, 0, OLED_8X16,"TEST:%02X",test);
//	OLED_Update();
  while (1)
  {

//		if(NRF24L01_Receive()==1)
//		{
//				OLED_Printf(56, 56, OLED_6X8, "%02X", NRF24L01_RxPacket[0]);
//			  OLED_Update();
//		}

		KeyNum=Key_GetNum();
		
		if(KeyNum==1)
		{
			if (RunFlag == 0)
			{
				PID_Init(&AnglePID,4.5,0.2,3,100,-100,3,250,-250); //按键1按下，停止运行并且清零PID参数，参数是测试好的
        PID_Init(&SpeedPID,2,0.01,0,20,-20,0,70,-70);
        PID_Init(&TurnPID,4,3,0,50,-50,0,20,-20);
				RunFlag = 1;
			}
			else
			{
				RunFlag = 0;
			}//控制PID调控
		}

    if(RunFlag==1)
    {
      HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);//点亮LED灯表示正在运行
    }
    else
    {
      HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);//熄灭LED灯表示停止运行
    }

//		if(Get_MPU6050_Data_Flag()==1)
//		{
			//MPU6050_GetData(&AX,&AY,&AZ,&GX,&GY,&GZ);//更新数据,在中断处理太慢了
			//GY+=30;//可以手动解决一下零点的偏移，这样可以让误差更小，根据实际情况来
			//这个GY+=30必须放在这里，也就是跟在数据更新的后面，如果单独放在中断里面
			//会因为程序多次进中断导致GY多次修正，反而不正确，应该更新一次数据进行一次修正
			//但是主循环数据刷新太慢，反而导致控制不好，没办法只能放在中断执行

      /*这里我要测试一s下获取一次数据所花费的时间，以看清对控制的影响*/
      /*
      static uint32_t current_time,last_time;
      current_time=HAL_GetTick();
      OLED_Printf(48, 0, OLED_6X8, "during:%d", current_time-last_time);
      last_time=current_time;
			OLED_Update();
      */
      /*大概一次650毫秒，非常明显的数据延迟，所以放在主循环完全行不通，刷新数据太慢*/
//		}

		
    OLED_Clear();
		OLED_Printf(0, 0, OLED_6X8, "  Angle");
		OLED_Printf(0, 8, OLED_6X8, "P:%05.2f", AnglePID.kp);
		OLED_Printf(0, 16, OLED_6X8, "I:%05.2f", AnglePID.ki);
		OLED_Printf(0, 24, OLED_6X8, "D:%05.2f", AnglePID.kd);
		OLED_Printf(0, 32, OLED_6X8, "T:%+05.1f", AnglePID.target);
		OLED_Printf(0, 40, OLED_6X8, "A:%+05.1f", Angle);
		OLED_Printf(0, 48, OLED_6X8, "O:%+05.0f", AnglePID.out);
		OLED_Printf(0, 56, OLED_6X8, "GY:%+05d", GY);
    OLED_Printf(56, 56, OLED_6X8, "Offset:%+02f", AnglePID.outoffset);
    OLED_Printf(50, 0, OLED_6X8, "Speed");
		OLED_Printf(50, 8, OLED_6X8, "%05.2f", SpeedPID.kp);
		OLED_Printf(50, 16, OLED_6X8, "%05.2f", SpeedPID.ki);
		OLED_Printf(50, 24, OLED_6X8, "%05.2f", SpeedPID.kd);
		OLED_Printf(50, 32, OLED_6X8, "%+05.1f", SpeedPID.target);
		OLED_Printf(50, 40, OLED_6X8, "%+05.1f", AveSpeed);
		OLED_Printf(50, 48, OLED_6X8, "%+05.0f", SpeedPID.out);
		OLED_Printf(88, 0, OLED_6X8, "Turn");
		OLED_Printf(88, 8, OLED_6X8, "%05.2f", TurnPID.kp);
		OLED_Printf(88, 16, OLED_6X8, "%05.2f", TurnPID.ki);
		OLED_Printf(88, 24, OLED_6X8, "%05.2f", TurnPID.kd);
		OLED_Printf(88, 32, OLED_6X8, "%+05.1f", TurnPID.target);
		OLED_Printf(88, 40, OLED_6X8, "%+05.1f", DifSpeed);
		OLED_Printf(88, 48, OLED_6X8, "%+05.0f", TurnPID.out);
    OLED_Update();

		if(GET_BlueSerial_RX_Flag()==1)//采用蓝牙来调节PID参数
		{
			char* Tag=strtok(BlueSerial_Rx_Data,",");//解析数据包，逗号分隔
      
      if(strcmp(Tag,"key")==0)//如果是按键数据包
      {
        char* Name=strtok(NULL,",");
        char* Value=strtok(NULL,",");
      }

      else if(strcmp(Tag,"slider")==0)//如果是滑杆数据包
      {
        char* Name=strtok(NULL,",");
        char* Value=strtok(NULL,",");

        if (strcmp(Name, "AngleKp") == 0)
				{
					AnglePID.kp = atof(Value);//把字符串转化为浮点数
				}
				else if (strcmp(Name, "AngleKi") == 0)
				{
					AnglePID.ki = atof(Value);
				}
				else if (strcmp(Name, "AngleKd") == 0)
				{
					AnglePID.kd = atof(Value);
				}

        else if (strcmp(Name, "SpeedKp") == 0)
				{
					SpeedPID.kp = atof(Value);
				}
				else if (strcmp(Name, "SpeedKi") == 0)
				{
					SpeedPID.ki = atof(Value);
				}
				else if (strcmp(Name, "SpeedKd") == 0)
				{
					SpeedPID.kd = atof(Value);
				}
				
				else if (strcmp(Name, "TurnKp") == 0)
				{
					TurnPID.kp = atof(Value);
				}
				else if (strcmp(Name, "TurnKi") == 0)
				{
					TurnPID.ki = atof(Value);
				}
				else if (strcmp(Name, "TurnKd") == 0)
				{
					TurnPID.kd = atof(Value);
				}

        else if(strcmp(Name,"Offset")==0)
        {
          AnglePID.outoffset=atof(Value);
        }
      }  

      else if(strcmp(Tag,"joystick")==0)//如果是摇杆数据包
      {
        int8_t LH = atoi(strtok(NULL, ","));
				int8_t LV = atoi(strtok(NULL, ","));
				int8_t RH = atoi(strtok(NULL, ","));
				int8_t RV = atoi(strtok(NULL, ","));
				
				SpeedPID.target = LV / 25.0;
				TurnPID.target = -RH / 25.0;
		  }
    }

    printf("[plot,%f,%f,%f]", AnglePID.error_accumlation,SpeedPID.error_accumlation,TurnPID.error_accumlation);//把数据发到上位机，逗号分隔，方便解析
  }

}



int fputc(int ch,FILE *f)
{
//采用轮询方式发送1字节数据，超时时间设置为无限等待
	HAL_UART_Transmit(&huart2,(uint8_t *)&ch,1,HAL_MAX_DELAY);
	return ch;
}

int fgetc(FILE *f)
{
	uint8_t ch;
	// 采用轮询方式接收 1字节数据，超时时间设置为无限等待
	HAL_UART_Receive( &huart2,(uint8_t*)&ch,1, HAL_MAX_DELAY );
	return ch;
}
/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
