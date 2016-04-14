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

int sensorData[8];
int styrKommando[] = {0x00, 0x02};
	
int SLA_sensor_R = 0xCB;
int SLA_sensor_W = 0xCA;
int SLA_styr_R = 0xCD;
int SLA_styr_W = 0xCC;

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

ISR(INT1_vect){ //Avbrott styr
	//1. Välj ny modul att utforska
	
	
	//2. Skicka styrkommando till styrmodulen enligt beslut i 1.
	Master(4,SLA_sensor_R,sensorData);
	//Master(2,SLA_styr_W,styrKommando);
	
	//SENSORDATA KOMMER I AVBROTTET FRÅN SENSORMODULEN!
	
	
}

ISR(INT2_vect){ //Avbrott sensor
	PORTD = (0<<PORTD6);
	
	
	//1. Hämta sensordata från sensormodulen
	Master(4,SLA_sensor_R,sensorData);
	//Master(25,SLA_sensor_R,sensorData);
	
	//2. Skicka sensordata till styrmodulen
	//Master(25,SLA_styr_W,sensorData);
	
	//(3. Ev skicka sensordata till datormodulen (var tionde gång?))
}

int main(void)
{
	_delay_ms(30);
	DDRD = (1<<PORTD6);
	for (int i = 0; i < 8; i++ ) {
		sensorData[ i ] = i+2;
	}
	
	//EICRA = (1<<ISC21)|(0<<ISC20)|(1<<ISC11)|(0<<ISC10);//Falling edge INT1 & INT2 (button)
	EICRA = (1<<ISC21)|(1<<ISC20)|(1<<ISC11)|(1<<ISC10); //Rising edge INT1 & INT2
	
	//Enable interrupt INT1 & INT2
	EIMSK = (1<<INT2)|(1<<INT1);
	
	//Enable global interrupts
	sei();
	//Enable TWI
	TWCR = (1<<TWEA)|(1<<TWEN);
	
	while(1){
		PORTD = (1<<PORTD6);
	}
	
	//MasterWrite();
	//MasterRead();
	//uint8_t SLA_W = 0xC8;
	//uint8_t SLA_R = 0xC9;
	//Master(2, SLA_R,0xF1);
	
	//Master(2,0xCC,komID1);
	//_delay_ms(2000);
	//Master(4,SLA_sensor_W,sensorData);
	//_delay_ms(2000);
}