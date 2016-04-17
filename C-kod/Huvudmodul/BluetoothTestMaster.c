/*
 * BluetoothTestMaster.c
 *
 * Created: 4/14/2016 11:12:24 AM
 *  Author: ollul666
 */ 


#include <avr/io.h>
#include <avr/interrupt.h>
#include "I2C_master.h"

int controlCommand[3];
int data = 0;
int SLA_styr_W = 0xCC;
int recieved;

ISR(USART0_RX_vect){
	recieved = UDR0;
	controlCommand[data] = recieved;
	
	if (data < 2){
		data = data + 1;
	} else {
		Master(3,SLA_styr_W,controlCommand);
		data = 0;
	}
}

void btInit(void){
	//16Mhz, Baud rate = 115.2kbps
	unsigned char baud = 7;

	UBRR0H = (unsigned char)(baud >> 8);
	UBRR0L = (unsigned char)(baud);
	
	// Enable reciever, transmitter and interrupt
	UCSR0B = (1<<RXEN0)|(1<<TXEN0)|(1<<RXCIE0);
	
	//8 bit data, no parity and 1 stop bit
	UCSR0C = (1<<UCSZ01)|(1<<UCSZ00)|(0<<USBS0);
	sei();	
}


int main(void)
{
	
	btInit();
	TWISetup();
	sei();
    while(1)
    {
        //TODO:: Please write your application code 
    }
}