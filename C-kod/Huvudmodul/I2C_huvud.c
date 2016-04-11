/*
* Master.c
*
* Created: 4/8/2016 9:28:26 AM
*  Author: eletr654
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU 14745600UL
#include <util/delay.h>

int sensorData[24];

	
int komID1[] = {0};
int komID2[] = {2};
int kommando[] = {0xF1};


void TWIStart(void){
	TWCR = (1<<TWINT)|(1<<TWEA)|(1<<TWSTA)|(1<<TWEN);
	//|(1<<TWIE);
}

void TWIChooseSlave(void){
	TWCR = (1<<TWINT)|(1<<TWEA)|(0<<TWSTA)|(1<<TWEN);
	//|(1<<TWIE);
}

void TWISendData(void){

	TWCR = (1<<TWINT)|(1<<TWEA)|(1<<TWEN);
	//|(1<<TWIE);
}

void TWIStop(void){
	TWCR = (0<<TWSTA)|(1<<TWINT)|(1<<TWEA)|(1<<TWSTO)|(1<<TWEN);
	//|(1<<TWIE);
}

void MasterWrite()
{
	uint8_t SLA_W = 0xC8;
	uint8_t SLA_R = 0xC9;
	//Send START condition
	TWIStart();
	
	while(1){
		//Wait for TWINT Flag set
		while(!(TWCR & (1<<TWINT)));
		
		if((TWSR & 0xF8) == 0x08){
			//START condition sent, send slave address and read/write
			TWDR = SLA_W;
			TWIChooseSlave();
		} else if((TWSR & 0xF8) == 0x18){
			//Send data over the bus
			TWDR = 0xF1;
			TWISendData();
		} else if((TWSR & 0xF8) == 0x28){
			TWIStop();
			break;
		} else if ((TWSR & 0xF8) == 0x20){
			//Not able to connect to slave
			PORTB = (1<<PORTB0);
			while(1);
		}
	}
}

void MasterRead()
{
	uint8_t SLA_W = 0xC8;
	uint8_t SLA_R = 0xC9;
	
	//Send START condition
	TWIStart();
	
	while(1){
		//Wait for TWINT Flag set
		while(!(TWCR & (1<<TWINT)));
		
		if((TWSR & 0xF8) == 0x08){
			//Send slave address and read/write
			TWDR = SLA_R;
			TWIChooseSlave();
		} else if((TWSR & 0xF8) == 0x40){
			//Reset TWINT flag
			TWCR = (1<<TWINT)|(1<<TWEN);
		} else if((TWSR & 0xF8) == 0x58){
			DDRB = 0xFF;
			PORTB = TWDR;
			TWIStop();
			break;
		}
	}
}

void Master(int times, uint8_t SLA, int data[]){
	DDRA = 0xFF;
	
	int counter = 0;
	
	//Set clock frequency for SCL
	TWBR = (1<<TWBR6)|(1<<TWBR1);
	
	//Send START condition
	TWIStart();
	
	while(1){
		//Wait for TWINT Flag set
		while(!(TWCR & (1<<TWINT)));
		
		if((TWSR & 0xF8) == 0x08){
			//START condition transmitted, send slave address and read/write
			TWDR = SLA;
			TWIChooseSlave();
			
			
		//MASTER TRANSMITTER
		} else if((TWSR & 0xF8) == 0x18){
			//SLA_W transmitted with ACK, load data
			times = times - 1;
			TWDR = data[counter];
			counter = counter + 1;
			TWISendData();
		} else if ((TWSR & 0xF8) == 0x20){
			//Not able to connect to slave
			
			//What happens here?
			
			
			
		} else if((TWSR & 0xF8) == 0x28){
			//Data transmitted with ACK, load data or stop
			if(times > 0){
				times = times - 1;
				TWDR = data[counter];
				counter = counter + 1;
				TWISendData();
			} else {
				TWIStop();
				break;
			}
		} else if ((TWSR & 0xF8) == 0x30){
			//Data transmitted with NOT ACK
			
			//What happens here?
			
		
		//MASTER RECEIVER
		} else if((TWSR & 0xF8) == 0x40){
			//SLA_R transmitted with ACK, wait for data
			if(times == 1){
				TWCR = (1<<TWINT)|(1<<TWEN);
			} else {
				times = times - 1;
				TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWEA);
			}
		} else if((TWSR & 0xF8) == 0x48){
			//SLA_R transmitted with NOT ACK
			
			//What happens here?
			
			
		} else if((TWSR & 0xF8) == 0x50){
			//Data byte received, ACK returned, wait for data
			if(times == 1){
				sensorData[counter] = TWDR;
				PORTA = sensorData[counter];
				counter = counter + 1;
				TWCR = (1<<TWINT)|(1<<TWEN);
			} else {
				PORTA = TWDR;
				times = times - 1;
				TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWEA);
			}
		} else if((TWSR & 0xF8) == 0x58){
			//Data byte received, NOT ACK returned, stop transmission
			TWIStop();
			break;
		} 
	}
}

ISR(INT1_vect){
	//Avbrott styr
	Master(3,0xC9,sensorData);
	PORTB = (1<<PORTB0);
}

ISR(INT2_vect){
	//Avbrott sensor
	Master(4,0xC8,sensorData);
}

void program()
{
	Master(1,0xC8,komID1);
	_delay_ms(30);
	Master(1,0xC8,kommando);
	Master(1,0xC8,komID2);
	Master(4,0xC8,sensorData);
}

int main(void)
{
	_delay_ms(30);
	
	for (int i = 0; i < 24; i++ ) {
		sensorData[ i ] = i;
	}
	
	//Falling edge INT1 & INT2 (button)
	EICRA = (1<<ISC21)|(0<<ISC20)|(1<<ISC11)|(0<<ISC10);
	//EICRA = (1<<ISC21)|(1<<ISC20)|(1<<ISC11)|(1<<ISC10); Rising edge INT1 & INT2
	
	//Enable interrupt INT1 & INT2
	EIMSK = (1<<INT2)|(1<<INT1);
	
	//Enable global interrupts
	sei();
	//Enable TWI
	TWCR = (1<<TWEA)|(1<<TWEN);
	
	//while(1);
	
	//MasterWrite();
	//MasterRead();
	//uint8_t SLA_W = 0xC8;
	//uint8_t SLA_R = 0xC9;
	//Master(2, SLA_R,0xF1);
	
	program();
	
	
	
}