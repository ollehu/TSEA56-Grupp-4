/*
* styrmodulStable.c
*
* Created: 4/18/2016 11:07:55 AM
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
#include "constants.h"

int commandType;
int commandSubType;
int commandValue;

int forwardSensor;
int sideSensors[4][5];

//PD-contorller
float P;
float D;
float K;

//Control mode (0 = manual control, 1 = autonomous mode)
int autonomousMode = 0;

volatile int madeChange = 0;
int dataOrder = 0;


//////////////////////////////////////////////////////////////////////////
////////////////////////////////HEADER////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void updateSensorData(int sensor, int data);
void updateControl(int direction, int controlSetting);
void autonomousForward(void);
void autonomousRotate(int direction);
void updateSetting(int setting, int newValue);
//////////////////////////////////////////////////////////////////////////

ISR(TWI_vect){
	TWCR = (1<<TWEA)|(1<<TWEN)|(0<<TWIE);
	PORTA = (0<<PORTA0);

	while(1){
		//SLAVE RECEIVER
		if((TWSR & 0xF8) == 0x60){
			//SLA_W received, ACK returned
			TWCR |= (1<<TWINT)|(1<<TWEA)|(1<<TWEN);
			TWCR &= ~(1<<TWSTA);
			TWCR &= ~(1<<TWSTO);
		} else if ((TWSR & 0xF8) == 0x80){
			//SLA_W, ACK returned, wait for data
			
			if (TWDR > 245){

				commandType = TWDR;
				dataOrder = 1;
				
			} else if (commandType == controlCommand){

				//////////////////////////////////////////////////////////////////////////
				madeChange = 1;
				//////////////////////////////////////////////////////////////////////////				

				if (dataOrder == 1){
					commandSubType = TWDR;
					dataOrder = 2;
				} else if (dataOrder == 2){
					commandValue = TWDR;
					updateControl(commandSubType, commandValue);
					dataOrder = 1;
				}
				
			} else if (commandType == sensorCommand){
				

				
				if (dataOrder == 1){
					commandSubType = TWDR;
					dataOrder = 2;
				} else if (dataOrder == 2){
					commandValue = TWDR;
					updateSensorData(commandSubType, commandValue);
					dataOrder = 1;	
				}
				
			} else if (commandType == settingCommand){
				
				if (dataOrder == 1){
					commandSubType = TWDR;
					dataOrder = 2;
				} else if (dataOrder == 2){
					commandValue = TWDR;
					updateSetting(commandSubType, commandValue);
					dataOrder = 1;
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

void updateSensorData(int sensor, int data)
{
	int i;
	if (sensor <= 4){

		for(i = 0; i < 4; ++i){
			sideSensors[sensor][i + 1] = sideSensors[sensor][i];
		}
		sideSensors[sensor][0] = data;
		
	} else if (sensor == 5){
		forwardSensor = data;
	}
}

void updateControl(int direction, int controlSetting)
{
	if (autonomousMode == 0){
		switch(direction){
			case 1:
				leftWheelPair(controlSetting, 1);
				rightWheelPair(controlSetting, 1);
				break;
			case 2:
				leftWheelPair(controlSetting, 0);
				rightWheelPair(controlSetting, 0);
				break;
			case 3:
				leftWheelPair(controlSetting, 1);
				rightWheelPair(controlSetting, 0);
				break;
			case 4:
				leftWheelPair(controlSetting, 0);
				rightWheelPair(controlSetting, 1);
				break;
			case 5:
				rightWheelPair(controlSetting*0.6, 1);
				leftWheelPair(controlSetting, 1);
				break;
			case 6:
				rightWheelPair(controlSetting, 1);
				leftWheelPair(controlSetting*0.6, 1);
				break;
			case 7:
				rightWheelPair(controlSetting*0.6, 0);
				leftWheelPair(controlSetting, 0);
				break;
			case 8:
				rightWheelPair(controlSetting, 0);
				leftWheelPair(controlSetting*0.6, 0);
				break;
			case 9:
				if (controlSetting == 1){
					openPicker();
				} else {
					closePicker();
				}
				break;
			default:
				stopWheels();
		} 
			
	} else if (autonomousMode == 1){
		switch(direction){
			case 1:
				autonomousForward();
				break;
			case 3:
				autonomousRotate(1);
				break;
			case 4:
				autonomousRotate(0);
				break;
			case 10:
				//////////////////////////////////////////////////////////////////////////
				autonomousRotate(2);
				break;
			default:
				break;
		}
	}
	/*if (data < 2){
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
		}*/
}

/**
	Autonomously drive forward one module
*/
void autonomousForward(void)
{
	//TODO: code
}

/**
	Autonomously rotate
	direction = 0 : 90 degrees left
			  = 1 : 90 degrees right
			  = 2 : 180 degrees right
*/
void autonomousRotate(int direction)
{
	//TODO: code
}



void updateSetting(int setting, int newValue)
{
	switch(setting){
		case 1:
			autonomousMode = newValue;
			break;
		case 2:
			P = newValue/100.0;
			break;
		case 3:
			D = newValue/100.0;
			break;
		case 4:
			K = newValue/10.0;
			break;
		default:
			break;
	}
}

int main(void)
{
	char *mes = "";

	//Styrmodul = 0xCC
	TWISetup(0xCC);
	initPWM();
	initLCD();
	writeTempMessage("Hej hej :)","//Styrmodul");
	sei();
	sprintf(mes, "%d %d %d", commandType, commandSubType, commandValue);
	lcdWriteBottomRow(mes);
	while(1)
	{
		if (madeChange == 1){
			sprintf(mes, "%d %d %d", commandType, commandSubType, commandValue);
			lcdWriteBottomRow(mes);
			madeChange = 0;
		}
	}
}