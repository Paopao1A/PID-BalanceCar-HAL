#ifndef __UART_H
#define __UART_H

void UATR_Init(void);
uint8_t GET_BlueSerial_RX_Flag(void);
void UATR_RXNE_Handler(void);

extern char BlueSerial_Rx_Data[100];

#endif
