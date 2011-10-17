/*
 * uart.c
 *
 *  Created on: 06.10.2011
 *      Author: moldov
 */
#include <avr/io.h>              // Most basic include files
#include <avr/interrupt.h>       // Add the necessary ones here

/* CPU frequency */
#ifndef F_CPU
	#define F_CPU 7372800UL
#endif //F_CPU

/* UART baud rate */
#ifndef UART_BAUDRATE
	#define UART_BAUDRATE 115200
#endif //UART_BAUD

#define UBRRVAL ((F_CPU/(UART_BAUDRATE*16UL))-1)

void USART_transmit( unsigned char data ) {
	/* Wait for empty transmit buffer */
	while ( !( UCSRA & (1<<UDRE)) ); /* Put data into buffer, sends the data */
	UDR = data;
}
void USART_init( void ) {
	/* Set baud rate */
	UBRRH = UBRRVAL>>8;
	UBRRL = UBRRVAL;

	/* Enable receiver and transmitter */
	UCSRB = (1<<RXEN)|(1<<TXEN);
	UCSRC = (1<<URSEL)|(0<<USBS)|(3<<UCSZ0);  // Set frame format: 8data, 1stop bit
	UCSRB |= (1 << RXCIE); // Enable the USART Recieve Complete interrupt (USART_RXC)
	sei();// Enable the Global Interrupt Enable flag so that interrupts can be processed
}
unsigned char USART_receive( void ) {
	/* Wait for data to be received */
	while ( !(UCSRA & (1<<RXC)) ); /* Get and return received data from buffer */
	return UDR;
}


