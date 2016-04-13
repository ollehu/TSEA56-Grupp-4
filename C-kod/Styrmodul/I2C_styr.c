/*
 * Styrmodul.c
 *
 * Created: 4/11/2016 2:24:48 PM
 *  Author: eletr654
 */ 

/*
* Slave.c
*
* Created: 4/8/2016 9:41:52 AM
*  Author: eletr654
*/


#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#include <avr/interrupt.h>
#include "LCD.h"

int sensorData[25];
int styrKommando;

ISR(TWI_vect){
	TWCR = (1<<TWEA)|(1<<TWEN)|(0<<TWIE);
	PORTA = (0<<PORTA0);
	
	int ind = 0;	
	int komID;
	
	while(1){
		//SLAVE RECEIVER
		if((TWSR & 0xF8) == 0x60){
			//SLA_W received, ACK returned
			TWCR |= (1<<TWINT)|(1<<TWEA)|(1<<TWEN);
			TWCR &= ~(1<<TWSTA);
			TWCR &= ~(1<<TWSTO);
		} else if ((TWSR & 0xF8) == 0x80){
			//SLA_W, ACK returned, wait for data
			
			if (ind == 0){
				komID = TWDR;
				ind = ind + 1;
			} else {
				if (komID == 0){
					styrKommando = TWDR;					
				} else if (komID ==2) {
					sensorData[ind] = TWDR;
					ind = ind + 1;
				} else {
					//Incorrect com.ID, what to do?
				}
			}
			TWCR |= (1<<TWINT)|(1<<TWEA)|(1<<TWEN);
		} else if ((TWSR & 0xF8) == 0x80){
			//SLA_W, NOT ACK returned
			
			//What happens here?
		} else if ((TWSR & 0xF8) == 0xA0){
			//STOP or repeated START
			TWCR |= (1<<TWINT)|(1<<TWEA)|(1<<TWIE);
			break;
			
			
		//SLAVE TRANSMITTER
		} else if ((TWSR & 0xF8) == 0xA8){
			//SLA_R received, ACK returned, transmit data
			TWDR = 0x55;
			TWCR = (1<<TWINT)|(1<<TWEA)|(1<<TWEN);			
		
		} else if ((TWSR & 0xF8) == 0xB8){
			//Data transmitted, ACK received, transmit data
			TWDR = 0x55;
			TWCR = (1<<TWINT)|(1<<TWEA)|(1<<TWEN);
		} else if ((TWSR & 0xF8) == 0xC0){
			//SLA_R, last byte transmitted
			TWCR |= (1<<TWINT)|(1<<TWEA)|(1<<TWIE);
			break;
		}
		
		while(!(TWCR & (1<<TWINT)));
	}
}

void TWISetup()
{
	//Set slave address and start TWI (including the TWI-interrupt)
	TWAR = (1<<TWA6)|(1<<TWA5)|(0<<TWA4)|(0<<TWA3)|(1<<TWA2)|(1<<TWA1)|(0<<TWA0)|(0<<TWGCE);
	TWCR |= (1<<TWEA)|(1<<TWEN)|(1<<TWIE);
}

int main(void)
{
	initLCD();
	lcdWriteTopRow("Hej hopp");
	TWISetup();
	sei();
	//while(1);
	DDRB = (1<<PORTB4);
	_delay_ms(3000);
	PORTB = (1<<PORTB4);
	_delay_ms(10);
	PORTB = (0<<PORTB4);
		
	switch (styrKommando)
	{
	case 0:
		//Reglering av/på
		lcdWriteTopRow("Reglering");
		break;
	case 1:
		//Kör framåt
		lcdWriteTopRow("Framat");
		break;
	case 2:
		//Kör bakåt
		lcdWriteTopRow("Bakat");
		break;
	case 3:
		//Rotera åt höger
		lcdWriteTopRow("Rotera");
		lcdWriteBottomRow("hoger");
		break;
	case 4:
		//Rotera åt vänster
		lcdWriteTopRow("Rotera");
		lcdWriteBottomRow("vanster");
		break;
	case 5:
		//Gripklo öppna/stänga
		lcdWriteTopRow("Gripklo");
		break;
	}
}