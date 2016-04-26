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

#define zeroAngVel 124

uint8_t commandType;
uint8_t commandSubType;
uint8_t commandValue;

uint16_t forwardSensor;
uint8_t sideSensors[4][5];

/////////////////////////////REGLERING/////////////////////////////////////////////
uint8_t wallLeft = 1;
uint8_t wallRight = 1;

uint8_t frontIndex = 1;
uint8_t backIndex = 3;

//PD-controller
float P = 0.45;
float D = 1.2;
float K = 0.7;

uint8_t distance;
uint8_t oldDistance;


volatile uint8_t preferredSpeed = 30;
volatile uint8_t preferredDistance = 100;


//Control mode (0 = manual control, 1 = autonomous mode)
uint8_t autonomousMode = 0;

volatile uint8_t madeChange = 1;
volatile uint8_t dataOrder;

//////////////////////////////////////////////////////////////////////////
int tempCount = 0;
uint8_t tempArray[14];
//////////////////////////////////////////////////////////////////////////

int y_out;
float p_out;
float d_out;

volatile uint8_t activeTurn = 0;

//////////////////////////////////////////////////////////////////////////
////////////////////////////////HEADER////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void updateSensorData(uint8_t sensor, uint8_t data);
void updateControl(uint8_t direction, uint8_t controlSetting);
void autonomousForward(void);
void autonomousRotate(uint8_t direction);
void updateSetting(uint8_t setting, uint8_t newValue);
//////////////////////////////////////////////////////////////////////////

char mes[16] = "";
char mes2[16] = "";

uint16_t tempForward;
int16_t angVelocity;
int16_t ackAngVelocity;

int8_t currentVelocity;

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

void updateSensorData(uint8_t sensor, uint8_t data)
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
		/*if (forwardSensor == 0){
			madeChange = 0;
		}*/
		
		madeChange += 1; 
		
		//////////////////////////////////////////////////////////////////////////
		if ((forwardSensor < 25) && (activeTurn == 0)){
			currentVelocity = 70;
			autonomousRotate(70);
		} else {
			if (activeTurn == 0){
				updateControl(1,50);
			}
		}
		//////////////////////////////////////////////////////////////////////////
		
	} else if (sensor == 7){
		
		if((ackAngVelocity > (-3.4*currentVelocity + 841)) && (activeTurn >= 1))
		{
			if (activeTurn == 1){
				stopWheels();
				activeTurn = 0;

			} else {
				activeTurn = 0;
				ackAngVelocity = 0;
			}
		}
		
		if (abs(data - zeroAngVel) > 3){
			angVelocity = ((int16_t) data) - zeroAngVel;
			if (activeTurn >= 1){
				ackAngVelocity = ackAngVelocity + angVelocity;
			}
		} else {
			angVelocity = 0;
		}
	}
	
	//TODO: Other sensors
	
}

void updateControl(uint8_t direction, uint8_t controlSetting)
{
	//currentVelocity = controlSetting;
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
				leftWheelPair(controlSetting, 0);
				rightWheelPair(controlSetting, 1);
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
	madeChange += 1;
}

/**
	Autonomously rotate
	direction = 0 : 90 degrees left
			  = 1 : 90 degrees right
			  = 2 : 180 degrees right
*/
void autonomousRotate(uint8_t direction)
{
	ackAngVelocity = 0;
	
	
	if (activeTurn == 0){
		activeTurn = 1;
		leftWheelPair(direction, 0);
		rightWheelPair(direction, 1);
	}
	
}



void updateSetting(uint8_t setting, uint8_t newValue)
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
	DDRD |= (1<<DDD7);
	
	//Styrmodul = 0xCC
	TWISetup(0xCC);
	initPWM();
	initLCD();
	//writeTempMessage("Hej hej :)","//Styrmodul");
	sei();
	sprintf(mes, "%d %d %d", 10, 20, 200);
	lcdWriteBottomRow(mes);
	
	autonomousMode = 1;
	ackAngVelocity = 0;
	updateControl(1,50);
	while(1)
	{
		//////////////////////////////////////////////////////////////////////////
		PORTD |= (1<<PORTD7);
		//////////////////////////////////////////////////////////////////////////
	
		if (madeChange >= 1){
			sprintf(mes, "D:%u", forwardSensor);
			lcdWriteTopRow(mes);
			sprintf(mes2, "R:%d A:%d T:%u", angVelocity, ackAngVelocity, activeTurn);
			lcdWriteBottomRow(mes2);
			madeChange = 0;
		} 
		
		//////////////////////////////////////////////////////////////////////////
		PORTD &= ~(1<<PORTD7);
		//////////////////////////////////////////////////////////////////////////
	}
}