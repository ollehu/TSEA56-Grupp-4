/*
 * Styrmodul.c
 *
 * Created: 4/11/2016 2:24:48 PM
 *  Author: eletr654
 */ 


/*
 * Styrmodul.cpp
 *
 * Created: 4/11/2016 2:17:07 PM
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

int sensorData[24];
int styrKommando;

ISR(TWI_vect){
	TWCR = (1<<TWEA)|(1<<TWEN)|(0<<TWIE);
	PORTA = (0<<PORTA0);
	
	int counter = 0;
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
			TWCR |= (1<<TWINT)|(1<<TWEA)|(1<<TWEN);
			
			if (counter == 0){
				komID = TWDR;
				PORTB = komID;
				counter = counter + 1;
			} else {
				if (komID == 0){
					styrKommando = TWDR;
					PORTB = styrKommando;
				} else {
					sensorData[counter-1] = TWDR;
					PORTB = sensorData[counter-1];
					counter = counter + 1;
				}
			}
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
	TWAR = (1<<TWA6)|(1<<TWA5)|(0<<TWA4)|(0<<TWA3)|(1<<TWA2)|(0<<TWA1)|(0<<TWA0)|(0<<TWGCE);
	TWCR |= (1<<TWEA)|(1<<TWEN)|(1<<TWIE);
}

int main(void)
{
	DDRB = 0xFF;
	TWISetup();
	sei();
	while(1);
}