/*
 * I2C_master.h
 *
 * Created: 4/19/2016 8:46:29 AM
 *  Author: lovgu777
 */ 

#include <avr/io.h>

volatile uint8_t sensorData[15];

void TWIStart(void){
	TWCR = (1<<TWINT)|(1<<TWEA)|(1<<TWSTA)|(1<<TWEN);
}

void TWIChooseSlave(void){
	TWCR = (1<<TWINT)|(1<<TWEA)|(0<<TWSTA)|(1<<TWEN);
}

void TWISendData(void){
	TWCR &= ~(1<<TWSTA);
	TWCR &= ~(1<<TWSTO);
	TWCR = (1<<TWINT)|(1<<TWEA)|(1<<TWEN);
}

void TWIStop(void){
	TWCR = (0<<TWSTA)|(1<<TWINT)|(1<<TWEA)|(1<<TWSTO)|(1<<TWEN);
}

void TWISetup(){
	//Enable TWI
	TWCR = (1<<TWEA)|(1<<TWEN);
	
	//Set clock frequency for SCL
	TWBR = (1<<TWBR6)|(1<<TWBR1);
}

void interruptSetup(){
	//Rising edge INT1 & INT2
	EICRA = (1<<ISC21)|(1<<ISC20)|(1<<ISC11)|(1<<ISC10)|(1<<ISC01)|(1<<ISC00);
	
	//Enable interrupt INT1 & INT2
	EIMSK = (1<<INT2)|(1<<INT1)|(1<<INT0);
}

void Master(int times, uint8_t SLA, uint8_t *data){
	
	int counter = 0;
	
	
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
				//times = times - 1;
				TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWEA);
			}
		} else if((TWSR & 0xF8) == 0x48){
			//SLA_R transmitted with NOT ACK
			
			//What happens here?
			
			
		} else if((TWSR & 0xF8) == 0x50){
			//Data byte received, ACK returned, wait for data
			if(times == 1){
				sensorData[counter] = TWDR;
				counter = counter + 1;
				TWCR = (1<<TWINT)|(1<<TWEN);
			} else {
				times = times - 1;
				//////////////////////////////////////////////////////////////////////////
				sensorData[counter] = TWDR;
				counter = counter + 1;
				//////////////////////////////////////////////////////////////////////////
				TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWEA);
			}
		} else if((TWSR & 0xF8) == 0x58){
			//Data byte received, NOT ACK returned, stop transmission
			TWIStop();
			break;
		}
	}
}