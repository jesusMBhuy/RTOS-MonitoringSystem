#ifndef UART_H_
#define UART_H_

#include <stdio.h>


//#define USARTDIV         0x1A04			// Baud rate divisor for desired baud rate
#define USARTDIV       0x22B

// Transmitter bits



int _write(int file, char *ptr, int len);
void USER_USART1_Init( void );
//void USER_USART1_Transmit( uint8_t *pData, uint16_t size );
uint8_t USER_USART1_Receive(void );

#endif /* UART_H_ */
