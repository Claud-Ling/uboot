 /*
 * uart.c
 *
 *  Created on: 06.10.2011
 *      Author: IBezhenar
 */

#include <avr/io.h>              // Most basic include files
#include <avr/interrupt.h>       // Add the necessary ones here
#include "uart.h"
#include <string.h>
#include <util/delay.h>
#include <stdio.h>

#define ACK		0x06
#define NACK	0x15
#define EOT		0x04
#define SOH		0x01
#define CR		0x0D
#define LF		0x0A

#define HEAD		3
#define CSUM		2
#define PKT_LEN 	128
#define CRCXMODEM	0x0000

#define MAX_ATTEMPTS 5

volatile unsigned int buffer_index = 0;
volatile int attempts = 0;
volatile char command_indicator = 0x00;
volatile char CR_indicator = 0x00; // Shows that we received <CR> already.
//<CR> dublicates by the ARM console, probably because of the linux requirements.
//We need to eliminate that duplication


//volatile int packet = 1;
volatile   char receive_buffer[HEAD+PKT_LEN+CSUM];
	volatile union  {
			unsigned volatile  int checksum;
			unsigned volatile  char val[4];
		} csum;

volatile  int i;
volatile  char receive[500];

int xmodem_calculate_crc(unsigned int crc, unsigned char data)
{
	crc  = (unsigned char)(crc >> 8) | (crc << 8);
	crc ^= data;
	crc ^= (unsigned char)(crc & 0xff) >> 4;
	crc ^= (crc << 8) << 4;
	crc ^= ((crc & 0xff) << 4) << 1;
	return crc;
}

ISR(TIMER1_COMPA_vect)
{
	//This enterrupt allows to skip
	//all the odd data on com port, so we can ignore it in the beginning
	//and start to receive only required data
	command_indicator = 'g';
	puts("C\r");
	//USART_transmit('C');
	//USART_transmit(0x0A);

   TIMSK &= ~(1 << OCIE1A); // Disable CTC interrupt
}


int main(void) {


	//DDRB = 0xFF; //PORTB for output
	//UART Port speed 115200  for the crystal freq. 7.3MHz
	USART_init ();


   TCCR1B |= (1 << WGM12); // Configure timer 1 for CTC mode

   TIMSK |= (1 << OCIE1A); // Enable CTC interrupt
   TCCR1B |= ((1 << CS10) | (1 << CS11)); // Set up timer at Fcpu/64

   OCR1A   = 7000; // Set CTC compare value
	TCCR1B |= ((1 << CS10) | (1 << CS11)); // Start timer at Fcpu/64

	puts("cd /home\r");
	puts("./xmodem -p /dev/console -i avr_robot.hex\r");
/*
	USART_transmit('c');// Change the directory on
	USART_transmit('d');//ARM to home. Cannot run
	USART_transmit(' ');///home/status STATUS__
	USART_transmit('/');//need to cd to /home
	USART_transmit('h');
	USART_transmit('o');
	USART_transmit('m');
	USART_transmit('e');
	USART_transmit(0x0A);

	USART_transmit('.');// Change the directory on
	USART_transmit('/');
	USART_transmit('x');//ARM to home. Cannot run
	USART_transmit('m');///home/status STATUS__
	USART_transmit('o');//need to cd to /home
	USART_transmit('d');
	USART_transmit('e');
	USART_transmit('m');
	USART_transmit(' ');
	USART_transmit('-');// Change the directory on
	USART_transmit('p');//ARM to home. Cannot run
	USART_transmit(' ');///home/status STATUS__
	USART_transmit('/');//need to cd to /home
	USART_transmit('d');
	USART_transmit('e');
	USART_transmit('v');
	USART_transmit('/');
	USART_transmit('c');
	USART_transmit('o');
	USART_transmit('n');
	USART_transmit('s');
	USART_transmit('o');
	USART_transmit('l');
	USART_transmit('e');
	USART_transmit(' ');
	USART_transmit('-');// Change the directory on
	USART_transmit('i');// Change the directory on
	USART_transmit(' ');// Change the directory on
	USART_transmit('a');// Change the directory on
	USART_transmit('v');
	USART_transmit('r');
	USART_transmit('_');
	USART_transmit('r');
	USART_transmit('o');
	USART_transmit('b');
	USART_transmit('o');
	USART_transmit('t');
	USART_transmit('.');
	USART_transmit('h');
	USART_transmit('e');
	USART_transmit('x');
	USART_transmit(0x0A);

*/
	//PORTA = xmodem_receive();
	//NO SUCCES - NOTIFY

   while(1) {
   }

	//close xmodem on ARM side
	//USART_transmit(0x03)//^C;
	//USART_transmit('^');
	//USART_transmit('C');
	//USART_transmit(0x0A);
}

// USART Receiver interrupt service routine

ISR(USART_RXC_vect) {
//SIGNAL (SIG_UART_RECV) {
	char data;
	data=UDR;
	receive[i]= data;
	i++;




	if (command_indicator != 0x00 ){
		if (attempts < MAX_ATTEMPTS) {
			switch (data ) {
				case EOT:
					if ( buffer_index != 0 ) {
						receive_buffer[buffer_index] = data;
						buffer_index++;
						
					}
					break;

				case SOH://Start of header
					if ( buffer_index == 0 || buffer_index == 1) {
						receive_buffer[buffer_index] = data;
						buffer_index++;
				   }
				break;
				

			
				default:
				
					
					//Here we receive the general byte of the 128 bytes length packet
					if ( buffer_index > 2 && buffer_index < HEAD + PKT_LEN ) {
						//general byte

						//Eliminate of <CR> duplication character
						if ( data != CR && receive_buffer[buffer_index - 1]  != CR) {
							//No Duplication
							csum.checksum = xmodem_calculate_crc(csum.checksum, receive_buffer[buffer_index]);
							//write data to receive buffer
							receive_buffer[buffer_index] = data;
							buffer_index++;
						}


					}//if ( buffer_index > 2 && buffer_index < HEAD + PKT_LEN ) {

					if (  buffer_index == 0x02 ){
						//header
						receive_buffer[buffer_index] = data;
						buffer_index++;
					}
					//CRC
					if ( buffer_index == HEAD+PKT_LEN + 1 || buffer_index == HEAD+PKT_LEN ){
						//header
						receive_buffer[buffer_index] = data;
						buffer_index++;
					}
					
					
					
					//Check CRC
					if ( buffer_index == HEAD+PKT_LEN+CSUM ) {

						if (receive_buffer[HEAD+PKT_LEN] == csum.val[1] && receive_buffer[HEAD+PKT_LEN - 1] == csum.val[0]) {
							//CRC is correct
							buffer_index = 0;
							puts("\x06\r");
							//USART_transmit(ACK);
						}
						else {
							//CRC is not correct

							buffer_index = 0;
							puts("\x15\r");
							//USART_transmit(NACK);
							attempts++;
						}
					}

				
			}//switch (data )

		
		}//if (attempts < MAX_ATTEMPTS)
		else {	//Couldn't receive packets. MAX_ATTEMPTS were performed
      	//close xmodem on ARM side
			USART_transmit(0x03);//^C
			//USART_transmit('^');
			//USART_transmit('C');
			//USART_transmit(0x0A);
      }
	
  }//if (command_indicator != 0x00 )
	//Ignore all the data from ARM till the <SOH> will be received
	
}//ISR(USART_RXC_vect)






































