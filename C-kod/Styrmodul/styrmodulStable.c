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
#include <stdlib.h>

uint8_t commandType;
int commandSubType;
int commandValue;

int forwardSensor;
uint8_t sideSensors[4][5];

/////////////////////////////REGLERING/////////////////////////////////////////////
uint8_t wallLeft = 1;
uint8_t wallRight = 1;

uint8_t frontIndex = 1;
uint8_t backIndex = 3;

//PD-controller
float P = 1;
float D = 1;
float K = 0.1;

int distance;
int oldDistance;


volatile uint8_t preferredSpeed = 40;
volatile uint8_t preferredDistance = 100;


//Control mode (0 = manual control, 1 = autonomous mode)
int autonomousMode = 0;

volatile int madeChange = 1;
volatile uint8_t dataOrder;

//////////////////////////////////////////////////////////////////////////
int tempCount = 0;
uint8_t tempArray[14];
//////////////////////////////////////////////////////////////////////////

int y_out;
float p_out;
float d_out;

//////////////////////////////////////////////////////////////////////////
////////////////////////////////HEADER////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void updateSensorData(int sensor, int data);
void updateControl(int direction, int controlSetting);
void autonomousForward(void);
void autonomousRotate(int direction);
void updateSetting(int setting, int newValue);
//////////////////////////////////////////////////////////////////////////

char mes[16] = "";
char mes2[16] = "";

uint16_t tempForward;
int8_t angVelocity;
int64_t ackAngVelocity;

int currentVelocity;

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

						

				if (dataOrder == 1){
					commandSubType = TWDR;
					dataOrder = 2;
				} else if (dataOrder == 2){
					commandValue = TWDR;
					updateControl(commandSubType, commandValue);
				}
				
			} else if (commandType == sensorCommand){
				
				//////////////////////////////////////////////////////////////////////////
				
				
				//////////////////////////////////////////////////////////////////////////
				
				/*tempArray[tempCount] = TWDR;
				tempCount = tempCount + 1;
				
				if (tempCount == 14){
					tempCount = 0;
				}*/
				
				 //Should be working:
				if (dataOrder % 2 == 1){
					commandSubType = TWDR;
					dataOrder =  dataOrder + 1;
				} else {
					commandValue = TWDR;
					updateSensorData(commandSubType, commandValue);
					dataOrder = dataOrder + 1;	
					if (dataOrder == 15){
						dataOrder = 0;
						//updateControl(1,50);
					
					}
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
		
		//Shift down...
		for(i = 3; i >= 0; --i){
			sideSensors[sensor - 1][i + 1] = sideSensors[sensor - 1][i];
		}
		
		sideSensors[sensor - 1][0] = data;
		
	} else if (sensor == 5){
		tempForward = data*256;
	} else if (sensor == 6){
		forwardSensor = tempForward + data;
		madeChange = 1; 
	} else if (sensor == 7){
		
		if(ackAngVelocity > (-3.4*currentVelocity + 841))
		{
			stopWheels();
		}
		
		if (abs(data) > 1){
			angVelocity = data;
			ackAngVelocity = ackAngVelocity + angVelocity;
		} else {
			angVelocity = 0;
		}
	}
	
	//TODO: Other sensors
	
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
				//leftWheelPair(controlSetting, 0);
				//rightWheelPair(controlSetting, 1);
				currentVelocity = controlSetting;
				autonomousRotate(controlSetting);
				break;
			case 4:
				leftWheelPair(controlSetting, 1);
				rightWheelPair(controlSetting, 0);
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
				break;
					//stopWheels();
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
	if (sideSensors[1][0] == 245){
		wallLeft = 0;
	} else {
		wallLeft = 1;
		frontIndex = 1;
		backIndex = 3;
	}
	
	if (sideSensors[2][0] == 245){
		wallRight = 0;
	} else {
		wallRight = 1;
		
		if (wallLeft == 0){
			frontIndex = 0;
			backIndex = 2;
		}
	}
	
	if (wallRight == 0 && wallLeft == 0){
		
		rightWheelPair(preferredSpeed, 1);
		leftWheelPair(preferredSpeed, 1);
		
	} else {
		if (wallRight == 1 && wallLeft == 1){
			preferredDistance = (sideSensors[frontIndex][0] + sideSensors[backIndex][0] + sideSensors[frontIndex - 1][0] + sideSensors[backIndex - 1][0])/4;
		}
	
		distance = (sideSensors[frontIndex][0] + sideSensors[backIndex][0])/2;
		oldDistance = (sideSensors[frontIndex][4] + sideSensors[backIndex][4])/2;
	

		p_out = P * (distance - 100);
		d_out = D * (sideSensors[frontIndex][0] - sideSensors[backIndex][0]);

		y_out = K * (p_out + d_out);

		if (frontIndex == 0){
			y_out = -y_out;
		}
	
		if(y_out < 0) {
		
			if (preferredSpeed - y_out > 100){
				rightWheelPair(100 + y_out, 1);
				leftWheelPair(100, 1);
			} else {
				rightWheelPair(preferredSpeed + y_out/2, 1);
				leftWheelPair(preferredSpeed - y_out/2, 1);
			}
		

		} else {
		
			if (preferredSpeed + y_out > 100){
				rightWheelPair(100, 1);
				leftWheelPair(100 - y_out, 1);
			} else {
				rightWheelPair(preferredSpeed + y_out/2, 1);
				leftWheelPair(preferredSpeed - y_out/2, 1);
			}
		}
	
	
	}
	madeChange = 1;
}

/**
	Autonomously rotate
	direction = 0 : 90 degrees left
			  = 1 : 90 degrees right
			  = 2 : 180 degrees right
*/
void autonomousRotate(int direction)
{
	ackAngVelocity = 0;
	
	leftWheelPair(direction, 0);
	rightWheelPair(direction, 1);
	
}



void updateSetting(int setting, int newValue)
{
	switch(setting){
		case 1:
			autonomousMode = newValue;
			break;
		case 2:
			P = ((double) newValue)/100.0;
			break;
		case 3:
			D = ((double) newValue)/100.0;
			break;
		case 4:
			K = ((double) newValue)/10.0;
			break;
		case 5:
			preferredSpeed = newValue;
			break;
		default:
			break;
	}
}

int main(void)
{
	
	//Styrmodul = 0xCC
	TWISetup(0xCC);
	initPWM();
	initLCD();
	//writeTempMessage("Hej hej :)","//Styrmodul");
	sei();
	sprintf(mes, "%d %d %d", 10, 20, 200);
	lcdWriteBottomRow(mes);
	
	autonomousMode = 0;
	ackAngVelocity = 0;
	//updateControl(1,50);
	while(1)
	{
		if (madeChange == 1){
			sprintf(mes, "Dist:%d", forwardSensor);
			lcdWriteTopRow(mes);
			sprintf(mes2, "R:%d A:%d", angVelocity, ackAngVelocity);
			lcdWriteBottomRow(mes2);
			madeChange = 0;
		} 
	}
}