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

#define ACK		0x06
#define NACK	0x15
#define EOT		0x04
#define SOH		0x01

#define HEAD		3
#define CSUM		2
#define PKT_LEN 	128
#define CRCXMODEM	0x0000

#define MAX_ATTEMPTS 5

int xmodem_calculate_crc(unsigned int crc, unsigned char data)
{
	crc  = (unsigned char)(crc >> 8) | (crc << 8);
	crc ^= data;
	crc ^= (unsigned char)(crc & 0xff) >> 4;
	crc ^= (crc << 8) << 4;
	crc ^= ((crc & 0xff) << 4) << 1;
	return crc;
}

int xmodem_receive(){
	int attempts = 0;
	int packet = 1;
	unsigned int checksum;
	unsigned char receive_buffer[HEAD+PKT_LEN+CSUM];

	USART_transmit('C');

	while(attempts < MAX_ATTEMPTS){

		USART_receive();
		if ( UDR != EOT ) {
			if (UDR == SOH ) {
				USART_receive();
				if (UDR == packet ) {
					USART_receive();
					if (UDR == 255 - packet ) {
					//all headers checks passed so we can receive
						int i;
						for (i=HEAD ; i < HEAD + PKT_LEN ; i++) {
							USART_receive();
							receive_buffer[i] = UDR;
							//Calculate CRC
							checksum = xmodem_calculate_crc(checksum, receive_buffer[i]);
						}//for (i=0 ; i < HEAD+PKT_LEN+CSUM ; i++) {

						USART_receive();//CRC1
						receive_buffer[HEAD+PKT_LEN+1] = UDR;
						USART_receive();//CRC2
						receive_buffer[HEAD+PKT_LEN+2] = UDR;

						//Check CRC
						if (checksum == 0x00 ) {
							USART_transmit(ACK);
							packet++;
							attempts = 0;
						}
						else {
							USART_transmit(NACK);
							attempts++;
						}

					}//if (UDR == 255 - packets ) {
				}//if (UDR == packets )
			}//if (UDR == SOH ) {
		}//if ( UDR != EOT ) {
		else {
		//ALL RECEIVED SUCCESFULLY
		//NOTIFY
		return 0;
		}
	}//while(attempts < MAX_ATTEMPTS){
	return packet;


}



int main(void) {

	//UART Port speed 115200  for the crystal freq. 7.3MHz
	USART_init ();

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



	_delay_us(100);
	USART_transmit('C');
	//PORTA = xmodem_receive();
	//NO SUCCES - NOTIFY



	//close xmodem on ARM side
	//USART_transmit(0x03)//^C;
	USART_transmit('^');
	USART_transmit('C');
	USART_transmit(0x0A);
}







