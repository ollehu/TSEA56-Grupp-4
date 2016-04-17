/*
 * BluetoothTest.c
 *
 * Created: 4/14/2016 10:41:01 AM
 *  Author: ollul666
 */ 

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "I2C_slave.h"
#include "PWM.h"
#include "LCD.h"
#include <stdio.h>

int controlCommand[3];
volatile int madeChange = 0;
int data = 0;
ISR(TWI_vect){
	TWCR = (1<<TWEA)|(1<<TWEN)|(0<<TWIE);
	PORTA = (0<<PORTA0);
	data = 0;
	
	//int komID;
	
	while(1){
		//SLAVE RECEIVER
		if((TWSR & 0xF8) == 0x60){
			//SLA_W received, ACK returned
			TWCR |= (1<<TWINT)|(1<<TWEA)|(1<<TWEN);
			TWCR &= ~(1<<TWSTA);
			TWCR &= ~(1<<TWSTO);
			} else if ((TWSR & 0xF8) == 0x80){
			//SLA_W, ACK returned, wait for data
			
			madeChange = 1;
				controlCommand[data] = TWDR;
				
				if (data < 2){
					data = data + 1;
				} else {
							if (controlCommand[1] == 1){
								leftWheelPair(controlCommand[2],1);
								rightWheelPair(controlCommand[2],1);
								} else if (controlCommand[1] == 2){
								leftWheelPair(controlCommand[2],0);
								rightWheelPair(controlCommand[2],0);
								} else if (controlCommand[1] == 3){
								rightWheelPair(controlCommand[2], 1);
								leftWheelPair(controlCommand[2], 0);
								} else if (controlCommand[1] == 4){
								rightWheelPair(controlCommand[2], 0);
								leftWheelPair(controlCommand[2], 1);
								} else if (controlCommand[1] == 5){
								rightWheelPair(controlCommand[2]*0.6,1);
								leftWheelPair(controlCommand[2],1);
								} else if (controlCommand[1] == 6){
								rightWheelPair(controlCommand[2],1);
								leftWheelPair(controlCommand[2]*0.6,1);
								} else if (controlCommand[1] == 7){
								rightWheelPair(controlCommand[2]*0.6,0);
								leftWheelPair(controlCommand[2],0);
								} else if (controlCommand[1] == 8){
								rightWheelPair(controlCommand[2],0);
								leftWheelPair(controlCommand[2]*0.6,0);
								} else if (controlCommand[1] == 9){
								if (controlCommand[2] == 1){
									openPicker();
									} else {
									closePicker();
								}
								} else if (controlCommand[1] == 10){
								stopWheels();
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

int main(void)
{	
	char *mes = "";

	//Styrmodul = 0xCC
	TWISetup(0xCC);
	initPWM();
	initLCD();
	writeTempMessage("hello...","--- --- ---");
	sei();
	sprintf(mes, "%d %d %d", controlCommand[0], controlCommand[1], controlCommand[2]);
	lcdWriteBottomRow(mes);
    while(1)
    {
		if (madeChange == 1){
       sprintf(mes, "%d %d %d", controlCommand[0], controlCommand[1], controlCommand[2]);
		lcdWriteBottomRow(mes); 
		madeChange = 0;
		}
    }
}