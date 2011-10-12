/*
 * boot_demo.c
 *
 *  Created on: 06.10.2011
 *      Author: IBezhenar
 */

#include <avr/io.h>              // Most basic include files
#include <avr/interrupt.h>       // Add the necessary ones here
#include "uart.h"
#include <string.h>

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

void xmodem_receive(){
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
		break;
		}
	}//while(attempts < MAX_ATTEMPTS){



}



int main(void) {

	xmodem_receive();
	//close xmodem on ARM side
	//USART_transmit(0x03)//^C;
	USART_transmit('^');
	USART_transmit('C');
	USART_transmit(0x0A);
}





