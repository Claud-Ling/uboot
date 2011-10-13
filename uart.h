/*
 * uart.h
 *
 *  Created on: 06.10.2011
 *      Author: moldov
 */

#ifndef UART_H_
#define UART_H_


//UART functions
void USART_transmit( unsigned char data );
void USART_init();
unsigned char USART_receive( void );


#endif /* UART_H_ */
