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

//////////////////////////////Variables////////////////////////////////////////////
volatile uint8_t lastControlCommand = forward;

//Control mode (0 = manual control, 1 = autonomous mode)
uint8_t autonomousMode = 1;

uint8_t lastReceivedData[7];
volatile uint8_t madeChange = 0;

uint8_t debugCount = 0;

/////////////////////////////Controller/////////////////////////////////////////////
uint8_t sideSensors[4];
uint16_t forwardSensor;

int8_t angularVelocity;
uint8_t zeroAngVel = 124;

//PD-controller
float P = 0.45;
float D = 1.2;
float K = 0.7;

int16_t accumulatedAngle;
int16_t preferredAccumulatedAngle;

volatile uint8_t preferredSpeed = 50;
uint8_t preferredRotationSpeed = 70;
uint8_t preferredDistance = 100;

//////////////////////////////////////////////////////////////////////////
////////////////////////////////HEADER////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void initInterrupt();
void updateSensorData(uint8_t sensorIndex, uint8_t data);
void respondToSensorData();
void respondToControlData(uint8_t command, uint8_t value);
void respondToSettingsData(uint8_t identifier, uint8_t value);
void autonomousForward();
void autonomousRotate();
int16_t convertAngle(int8_t value);
void callMainInterrupt(void);
/////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
uint8_t commandType = 0;
uint8_t commandSubType = 0;
uint8_t commandValue = 0;
	
uint8_t dataOrder = 0;
//////////////////////////////////////////////////////////////////////////

ISR(TWI_vect){
	
	/*uint8_t commandType = 0;
	uint8_t commandSubType = 0;
	uint8_t commandValue = 0;
	
	uint8_t dataOrder = 0;*/
	
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
				
			} else if (commandType == sensorCommand){
				
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
						respondToSensorData();
					}
				}
				
			} else if (commandType == controlCommand){

				if (dataOrder == 1){
					commandSubType = TWDR;
					dataOrder =  2;
				} else {
					commandValue = TWDR;
					respondToControlData(commandSubType, commandValue);
				}
				
			} else if (commandType == settingCommand){
				
				if (dataOrder == 1){
					commandSubType = TWDR;
					dataOrder =  dataOrder + 1;
				} else {
					commandValue = TWDR;
					respondToSettingsData(commandSubType, commandValue);
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

void updateSensorData(uint8_t sensorIndex, uint8_t data){
	lastReceivedData[sensorIndex - 1] = data;
}

void respondToSensorData(){
	// add IR sensors
	for(uint8_t sensorIndex = 0; sensorIndex < 4; sensorIndex += 1)
	{
		sideSensors[sensorIndex] = lastReceivedData[sensorIndex];
	}
		
	// add Lidar
	forwardSensor = lastReceivedData[4]*128 + lastReceivedData[5];
		
	// add gyro
	if (lastReceivedData[6] >= 124){
		angularVelocity = lastReceivedData[6] - zeroAngVel;
		} else {
		angularVelocity = -(zeroAngVel - lastReceivedData[6]);
	}
	

	// TODO: handle stop conditions
	//////////////////////////////////////////////////////////////////////////

	if (forwardSensor < 25)
	{
		if(sideSensors[2] == 245 && lastControlCommand == forward){
			lastControlCommand = rotation;
			preferredAccumulatedAngle = convertAngle(1);
			accumulatedAngle = 0;
		} else if(sideSensors[3] == 245 && lastControlCommand == forward){
			lastControlCommand = rotation;
			preferredAccumulatedAngle = convertAngle(-1);
			accumulatedAngle = 0;
		} else if (lastControlCommand == forward){
			lastControlCommand = rotation;
			preferredAccumulatedAngle = convertAngle(2);
			accumulatedAngle = 0;
		}
	}
	
	callMainInterrupt();
	//////////////////////////////////////////////////////////////////////////
	
	// update controller according to last control command
	switch(lastControlCommand){
		
		case stop:
			
			break;
		
		case forward:
			autonomousForward();
			break;
			
		case rotation:
			autonomousRotate();
			break;
			
		default:
			break;
	}
	
	madeChange = 1;
}
	
void respondToControlData(uint8_t command, uint8_t value){
	if (autonomousMode == 0){
		switch(command){
			case commandForward:
				leftWheelPair(value, 1);
				rightWheelPair(value, 1);
				
				break;
			case commandReverse:
				leftWheelPair(value, 0);
				rightWheelPair(value, 0);
				break;
			case commandRight:
				leftWheelPair(value, 0);
				rightWheelPair(value, 1);
				break;
			case commandLeft:
				leftWheelPair(value, 1);
				rightWheelPair(value, 0);
				break;
			case commandForwardRight:
				rightWheelPair(value*0.6, 1);
				leftWheelPair(value, 1);
				break;
			case commandForwardLeft:
				rightWheelPair(value, 1);
				leftWheelPair(value*0.6, 1);
				break;
			case commandReverseRight:
				rightWheelPair(value*0.6, 0);
				leftWheelPair(value, 0);
				break;
			case commandReverseLeft:
				rightWheelPair(value, 0);
				leftWheelPair(value*0.6, 0);
				break;
			case commandClaw:
				if (value == 1){
					openPicker();
				} else {
					closePicker();
				}
				break;
			default:
				stopWheels();
				break;
					
		} 
			
	} else if (autonomousMode == 1){
		switch(command){
			case commandForward:
				lastControlCommand = forward;
			
				//////////////////////////////////////////////////////////////////////////
				debugCount += 1;
				//////////////////////////////////////////////////////////////////////////
				
				break;
			case rotation:
				lastControlCommand = rotation;
				preferredAccumulatedAngle = convertAngle(value);
				accumulatedAngle = 0;
				break;
			default:
				break;
		}
	}
}

int16_t convertAngle(int8_t value){
	if (value == -1)
	{
		return -3.4*preferredRotationSpeed + 841;
	} else if (value == 1) {
		return 3.4*preferredRotationSpeed - 841;
	} else if (value == 2)
	{
		return (3.4*preferredRotationSpeed - 841)*2.2;
	}
	
}

void respondToSettingsData(uint8_t identifier, uint8_t value){
	switch(identifier){
		case 1:
			autonomousMode = value;
			break;
		case 2:
			P = ((double) value)/100.0;
			break;
		case 3:
			D = ((double) value)/100.0;
			break;
		case 4:
			K = ((double) value)/10.0;
			break;
		case 5:
			preferredSpeed = value;
			break;
		default:
			break;
	}
}

/**
	Straight line controller
*/
void autonomousForward()
{
	uint8_t frontIndex = 10;
	uint8_t backIndex = 0;
	
	// adjust wall index
	if (sideSensors[1] != 245){
		frontIndex = 1;
		backIndex = 3;
	} else {
		if (sideSensors[2] != 245){
			frontIndex = 0;
			backIndex = 2;
		}
	}
	
	if (frontIndex == 10){
		// no walls available
		rightWheelPair(preferredSpeed, 1);
		leftWheelPair(preferredSpeed, 1);
		
	} else {
	
		// if both walls available, adjust preferred distance
		if (sideSensors[1] != 245 && sideSensors[2] != 245){
			preferredDistance = (sideSensors[0] + sideSensors[1] + sideSensors[2] + sideSensors[3])/4;
		}
	
		// controller
		uint8_t distance = (sideSensors[frontIndex] + sideSensors[backIndex])/2;
	
		float p_out = P * (distance - preferredDistance);
		float d_out = D * (sideSensors[frontIndex] - sideSensors[backIndex]);

		int16_t y_out = K * (p_out + d_out);

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
}

/**
	Autonomously rotate
	direction = 0 : 90 degrees left
			  = 1 : 90 degrees right
			  = 2 : 180 degrees right
*/
void autonomousRotate()
{
	if(labs(accumulatedAngle) > labs(preferredAccumulatedAngle)){
		callMainInterrupt();
		stopWheels();
	} else if (preferredAccumulatedAngle > 0){
		rightWheelPair(preferredRotationSpeed, 1);
		leftWheelPair(preferredRotationSpeed, 0);
	} else {
		rightWheelPair(preferredRotationSpeed, 0);
		leftWheelPair(preferredRotationSpeed, 1);
	}
	
	accumulatedAngle += angularVelocity;
	
	
	
}

void initInterrupt(){
	DDRB |= (1<<DDB4);
	PORTB &= ~(1<<PORTB4);
}

void callMainInterrupt(void){
	//lastControlCommand = forward;
	PORTB |= (1<<PORTB4);
	
	PORTB &= ~(1<<PORTB4);
}

int main(void)
{
	char bottomRowMessage[16] = "";
	char topRowMessage[16] = "";
	
	//Styrmodul = 0xCC
	TWISetup(0xCC);
	initPWM();
	initLCD();
	initInterrupt();
	sei();
	sprintf(bottomRowMessage, "%d %d %d", 10, 20, 200);
	lcdWriteBottomRow(bottomRowMessage);
	
	autonomousMode = 1;
	while(1)
	{
	
		if (madeChange >= 1){
			sprintf(topRowMessage, "CC:%d DC:%u", lastControlCommand, debugCount);
			lcdWriteTopRow(topRowMessage);
			sprintf(bottomRowMessage, "L:%u R:%u A:%d", sideSensors[3], sideSensors[2], accumulatedAngle);
			lcdWriteBottomRow(bottomRowMessage);
			madeChange = 0;
		} 
	}
}