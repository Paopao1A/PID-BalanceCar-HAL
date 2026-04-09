#include "main.h"
#include "usart.h"

char BlueSerial_Rx_Data[100];//数据包接收数组
uint8_t BlueSerial_Rx_Flag;


void UATR_Init(void)
{
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
	
	__HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE);
	__HAL_UART_CLEAR_FLAG(&huart2,UART_IT_RXNE);//清除RXNE标志位 
	
}

uint8_t GET_BlueSerial_RX_Flag(void)
{
	if(BlueSerial_Rx_Flag==1)
	{
		BlueSerial_Rx_Flag=0;
		return 1;
	}
	return 0;
}

	
void UATR_RXNE_Handler(void)
{
	static uint8_t State_Flag=0;
	static uint16_t i;
	
	if(__HAL_UART_GET_FLAG(&huart2,UART_IT_RXNE)==RESET)//查询之后会自动清除标志位
	{
		uint8_t data;
		HAL_UART_Receive(&huart2,&data,1, HAL_MAX_DELAY);
		
		switch(State_Flag)//状态机接收数据包
		{
			case 0:
				if(data=='[')//接收到包头
				{
					i=0;
					State_Flag=1;
				}
				break;
				
			case 1:
				if(data==']')//接收到包尾
				{
					State_Flag=0;
					BlueSerial_Rx_Data[i]='\0';//手动添加字符串结尾
					BlueSerial_Rx_Flag=1;//接收工作完成
				}
				else//实际数据接收
				{
					BlueSerial_Rx_Data[i]=data;
					i++;
				}
				break;
		}
		
		__HAL_UART_CLEAR_FLAG(&huart2,UART_IT_RXNE);
	}
}


