/*
* Master.c
*
* Created: 4/8/2016 9:28:26 AM
*  Author: eletr654
*/

#include "I2C_master.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU 14745600UL
#include <util/delay.h>

int styrKommando[] = {0x00, 0x02};
	
int SLA_sensor_R = 0xCB;
int SLA_sensor_W = 0xCA;
int SLA_styr_R = 0xCD;
int SLA_styr_W = 0xCC;



ISR(INT1_vect){ //Avbrott styr
	//1. Välj ny modul att utforska
	
	//2. Skicka styrkommando till styrmodulen enligt beslut i 1.
	Master(2,SLA_styr_W,styrKommando);
	
	//SENSORDATA KOMMER I AVBROTTET FRÅN SENSORMODULEN!
	
	
}

ISR(INT2_vect){ //Avbrott sensor
	PORTD = (0<<PORTD6);
	
	
	//1. Hämta sensordata från sensormodulen
	Master(8,SLA_sensor_R,sensorData);
	
	//2. Skicka sensordata till styrmodulen
	//Master(8,SLA_styr_W,sensorData);
	
	//(3. Ev skicka sensordata till datormodulen (var tionde gång?))
}

int main(void)
{
	_delay_ms(30);
	DDRD = (1<<PORTD6);
	for (int i = 0; i < 8; i++ ) {
		sensorData[ i ] = i+2;
	}
	
	TWISetup();
	
	while(1){
		PORTD = (1<<PORTD6);
	}
}