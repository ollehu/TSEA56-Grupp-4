/*
 * Bluetooth.c
 *
 * Created: 4/11/2016 1:13:56 PM
 *  Author: ollul666
 */ 

#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include "lcd.h"

char message[50] = "";
int len;

ISR(USART1_RX_vect){
	len = strlen(message);
	message[len] = (char) UDR1;
	message[len + 1] = '\0';
	//message = message + (char) UDR1;
	lcdWriteBottomRow(message);
}

void btInit(void){
	//16Mhz, Baud rate = 115.2kbps
	unsigned char baud = 8;

	UBRR1H = (unsigned char)(baud >> 8);
	UBRR1L = (unsigned char)(baud);
	
	// Enable reciever, transmitter and interrupt
	UCSR1B = (1<<RXEN1)|(1<<TXEN1)|(1<<RXCIE1);
	
	//8 bit data, no parity and 1 stop bit
	UCSR1C = (1<<UCSZ11)|(1<<UCSZ10)|(0<<USBS1);
}

void btSend(unsigned char data)
{
	// Wait for empty transmit buffer
	while ( !( UCSR1A & (1<<UDRE1)) );
	// Put data into buffer, sends the data
	UDR1 = data;
}

int main(void)
{
	char *startMessage = "Start sending data";
	btInit();
	
	int i;
	for(i=0; startMessage[i]!='\0'; i++){
		btSend(startMessage[i]);
	}
	
	initLCD();
	lcdWriteTopRow("Message:");
	while(1)
	{
	//nothing
	}
}