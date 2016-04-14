#include <avr/io.h>

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
			PORTA = (1<<PORTA0);
			
			
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
				PORTA = (1<<PORTA1);
			}
		} else if((TWSR & 0xF8) == 0x48){
			//SLA_R transmitted with NOT ACK
		
			//What happens here?
			
			
		} else if((TWSR & 0xF8) == 0x50){
			//Data byte received, ACK returned, wait for data
			if(times == 1){
				sensorData[counter] = TWDR;
				//PORTA = sensorData[counter];
				counter = counter + 1;
				TWCR = (1<<TWINT)|(1<<TWEN);
			} else {
				//PORTA = TWDR;
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