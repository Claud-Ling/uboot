/*
 * uart.c
 *
 *  Created on: 06.10.2011
 *      Author: IBezhenar
 */
#include <avr/io.h>              // Most basic include files
#include <avr/interrupt.h>       // Add the necessary ones here


#ifndef F_CPU         //define cpu clock speed if not defined
#define F_CPU 7372800UL   //
#endif

/* UART baud rate */
#define USART_BAUDRATE  115200

#define BAUD_PRESCALE ((F_CPU/(USART_BAUDRATE * 16UL)) - 1)
//BAUD_PRESCALE -> the value to be inserted in UBRRL and UBRRH to set the required baud rate
//F_CPU         -> the crystal frequency, using 16MHz in this program




void USART_init() {
	/* Set baud rate */
	UBRRL = BAUD_PRESCALE;      //load lower 8-bits for baud rate, calculated/stored in BAUD_PRESCALE
	UBRRH = BAUD_PRESCALE >> 8;   //load higher 8-bits for baud rate, calculated/stored in BAUD_PRESCALE

	//UBRRH = (unsigned char)(baud>>8);
	//UBRRL = (unsigned char)baud;

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

void USART_transmit( unsigned char data ) {
	/* Wait for empty transmit buffer */
	while ( !( UCSRA & (1<<UDRE)) ); /* Put data into buffer, sends the data */
	UDR = data;
}
