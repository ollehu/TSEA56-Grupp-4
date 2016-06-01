/*
* styrmodulStable.c
*
* Created: 4/18/2016 11:07:55 AM
*  Author: ollul666
*/

#define F_CPU 16000000UL
#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "constants.h"
#include "I2C_slave.h"
#include "PWM.h"
#include "LCD.h"

/*******************************************************/
/*                            CONTROLLER                                */
/*******************************************************/
uint8_t autonomousMode = 1;
volatile uint8_t currControlCommand = stop;
volatile uint8_t adjustRotationMode = 0; // 0 = no adjust, 1 = adjust after, 2 = adjust before, 3 = compensate
uint16_t numberOfAdjustSamples = 0;
volatile uint8_t forwardsSinceLeftWallLost = 0;
volatile uint8_t forwardsSinceRightWallLost = 0;

float P = 0.45;
float D = 1.2;
float K = 0.8;//0.7
float L = 1;
float K_Adjust = 1;

volatile uint8_t preferredSpeed = 40;
uint8_t preferredHalfModuleSpeed = 40;
uint8_t preferredRotationSpeed = 30;
uint8_t preferredAdjustSpeed = 25;
uint8_t preferredDistance = 120;
int8_t lastDistanceDifference;
int8_t preferredDistanceDifference = 1;
volatile uint8_t openLeft = 0;
volatile uint8_t openRight = 0;
uint8_t preHalfModuleDistance;

uint8_t direction = 1;
uint8_t controlOn = 1;

uint8_t maximumDistanceDifference = 0;
/*******************************************************/
/*                              SENSOR                                  */
/*******************************************************/
uint8_t sideSensors[4];
uint16_t forwardSensor[5];
int16_t oldForwardSensor;

int8_t angularVelocity;

int16_t convertAngle90 = 1000;
int16_t convertAngle180 = 1980;//1980

int16_t accumulatedAngle;
int16_t preferredAccumulatedAngle;

uint8_t lastReceivedData[9];

uint16_t lidarMid = 390;
uint16_t lidarMax;
uint16_t lidarMin;
uint8_t lidarIncrement = 3;
uint16_t currentLidarAngle;
uint16_t lidarMaxDistance = 0;
uint16_t lidarMaxAngle;
uint8_t activeScan = 0;

volatile uint8_t accumulatedDistance = 0;

uint8_t targetDetected = 0; 

/*******************************************************/
/*                               LCD                                    */
/*******************************************************/
volatile uint8_t madeChange = 0;
uint8_t debugCount = 0;

/*******************************************************/
/*                               DEBUG                                  */
/*******************************************************/
volatile uint8_t called;
volatile uint8_t received;
volatile uint8_t debugMode;
volatile uint8_t difference = 0;

float p_out;
float d_out;
uint8_t speed;
int16_t y_out;
uint8_t leftSpeed;
uint8_t rightSpeed;

int8_t ymax = 0;
int8_t y_out_old = 0;
int8_t y_diff = 0;
int8_t y_diff_max = 0;

/*******************************************************/
/*                              HEADER                                  */
/*******************************************************/
char *commandToString(uint8_t command);
void updateSensorData(uint8_t sensorIndex, uint8_t data);
void respondToSensorData();
void respondToControlData(uint8_t command, uint8_t value);
int16_t convertAngle(int8_t value);
void respondToSettingsData(uint8_t identifier, uint8_t value);
void autonomousForward();
void autonomousHalfModule(void);
void autonomousRotate();
void autonomousAdjust();
void adjustRotation(void);
void autonomousScan(void);
void initInterrupt();
void callMainInterrupt(void);
uint8_t edgesToCentimeter(uint8_t edgeCount);
uint8_t centimeterToEdges(uint8_t centimeter);
uint16_t getForwardAverage(void);

/*******************************************************/
/*	Interrupt: TWI vector - receive data from main module.

	Get sensor data or control command from main module.
																		*/
/*******************************************************/
ISR(TWI_vect)
{
	
	uint8_t commandType = 0;
	uint8_t commandSubType = 0;
	uint8_t commandValue = 0;
	
	uint8_t dataOrder = 0;
	
	madeChange = 1;
	
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
					
					if (dataOrder == sensorArraySize){
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
					received += 1;
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

/*******************************************************/
/*	commandToString - Convert a command to a string.

	command: Command to translate.
																		*/
/*******************************************************/
char *commandToString(uint8_t command)
{
		switch(command){
			case forward:
				return "Forward";
				break;
			case rotation:
				return "Rotation";
				break;
			default:
				break;
		}
		
		return "Error";
}

/*******************************************************/
/*	updateSensorData - update vector containing last received data
		with argument data.

	sensorIndex = 1 : Front right IR-sensor
				  2 : Front left IR-sensor
				  3 : Rear right IR-sensor
				  4 : Rear left IR-sensor
				  5 : Front laser sensor (high byte)
				  6 : Front laser sensor (low byte)
				  7 : Angular velocity
																		*/
/*******************************************************/
void updateSensorData(uint8_t sensorIndex, uint8_t data)
{
	lastReceivedData[sensorIndex - 1] = data;
}

/*******************************************************/
/*	respondToSensorData - act on sensor data.

	Fill sideSensors, forwardSensor and angularVelocity.
																		*/
/*******************************************************/
void respondToSensorData()
{
	//IR-sensors
	for(uint8_t sensorIndex = 0; sensorIndex < 4; sensorIndex += 1)
	{
		sideSensors[sensorIndex] = lastReceivedData[sensorIndex];
	}
		
	for(uint8_t i = 1; i <= 4; ++i){
		forwardSensor[i] = forwardSensor[i-1];	
	}
	//Lidar
	forwardSensor[0] = lastReceivedData[4]*128 + lastReceivedData[5];

	//Gyro
	if (lastReceivedData[6] >= 124){
		angularVelocity = lastReceivedData[6] - zeroAngVel;
		} else {
		angularVelocity = -(zeroAngVel - lastReceivedData[6]);
	}
	
	targetDetected = lastReceivedData[7];
	
	//Reflex sensor
	accumulatedDistance += lastReceivedData[8];
	
	switch(currControlCommand){
		case stop:
			stopWheels();
			break;
		case forward:
			autonomousForward();
			break;
		case halfModule:
			autonomousHalfModule();
			break;
		case rotation:
			autonomousRotate();
			break;
		case commandScan:
			autonomousScan();
			break;
		case commandAdjust:
			autonomousAdjust();
			break;
		default:
			break;
	}
	
	//Update LCD
	madeChange = 1;
}

/*******************************************************/
/*	respondToControlData - act on control data.
	
	autonomousMode = 0:
	  	command = 1 : Forward							value : speed
			      2 : Reverse							value : speed
				  3 : Rotate clockwise					value : speed
				  4 : Rotate c-clockwise				value : speed
				  5 : Forward and right					value : speed
				  6 : Forward and left					value : speed
				  7 : Reverse and right					value : speed
				  8 : Reverse and left					value : speed
				  9 : Control Claw						value : 1 (O)
																0 (C)
				  0 : Stop								value : n/a
				  
	autonomouseMode = 1:
		command = 1 : Autonomous forward				value : n/a
				  2 : Autonomous rotate 180*			value : n/a
				  3 : Autonomous rotate clockwise		value : n/a
				  4 : Autonomous rotate c-clockwise		value : n/a
																		*/
/*******************************************************/
void respondToControlData(uint8_t command, uint8_t value)
{
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
			default:
				stopWheels();
				break;
		} 
	} else if (autonomousMode == 1){
		switch(command){
			case commandForward:
				openLeft = 1;
				openRight = 1;
				currControlCommand = forward;
				direction = 1;
				controlOn = 1;
				
				if (sideSensors[frontRightIndex] != maxDistance &&
				sideSensors[frontLeftIndex] != maxDistance  &&
				sideSensors[rearRightIndex] != maxDistance &&
				sideSensors[rearLeftIndex] != maxDistance ){
					
					preferredDistance = (sideSensors[frontRightIndex] +
										 sideSensors[frontLeftIndex]  +
										 sideSensors[rearRightIndex]  +
										 sideSensors[rearLeftIndex])/4;
				}
				
				break;
			case commandReverse:
				currControlCommand = rotation;
				if (sideSensors[frontRightIndex] > 
					sideSensors[frontLeftIndex]){
					preferredAccumulatedAngle = convertAngle(2);
				} else {
					preferredAccumulatedAngle = convertAngle(-2);
				}
				accumulatedAngle = 0;
				break;
			case commandRight:
				currControlCommand = rotation;
				preferredAccumulatedAngle = convertAngle(1);
				accumulatedAngle = 0;
				break;
			case commandLeft:
				currControlCommand = rotation;
				preferredAccumulatedAngle = convertAngle(-1);
				accumulatedAngle = 0;
				break;
			case commandHalfForward:
				currControlCommand = halfModule;
				direction = 1;
				preHalfModuleDistance = getForwardAverage();
				break;
			case commandHalfBackward:
				currControlCommand = halfModule;
				direction = 0;
				break;
			case commandStop:
				currControlCommand = stop;
				break;
			default:
				break;
		}
		
		// determine adjusted rotation
		if(currControlCommand == rotation) {
			if(((sideSensors[0] != maxDistance) &&
			(sideSensors[2] != maxDistance)) ||
			((sideSensors[1] != maxDistance) &&
			(sideSensors[3] != maxDistance))) {
				adjustRotationMode = 2;
				currControlCommand = commandAdjust;
			} else if(getForwardAverage() <= moduleDepth) {
				adjustRotationMode = 2;
				currControlCommand = commandAdjust;
			} else {
				adjustRotationMode = 2;
				currControlCommand = commandAdjust;
				//preferredAccumulatedAngle = preferredAccumulatedAngle + K_Adjust * lastDistanceDifference;
			}
		}
	}
}

/*******************************************************/
/*	ConvertAngle - convert integer input to match preferred 
		accumulated angular velocity.

	value = -1 : Rotate 90 left
			 1 : Rotate 90 right
			-2 : Rotate 180 left
			 2 : Rotate 180 right
																		*/
/*******************************************************/
int16_t convertAngle(int8_t value)
{
	if (value == -1) {
		return convertAngle90;
	} else if (value == 1) {
		return -convertAngle90;
	} else if (value == 2) {
		return -convertAngle180;
	} else if (value == -2) {
		return convertAngle180;
	}
	
	return 0;
}

/*******************************************************/
/*	respondToSettingsData - act on settings data.

	identifier = 1 : Set autonomous mode: value = 1 <-> ON
												= 0 <-> OFF
			     2 : Set P value		    : P = value / 100
				 3 : Set D value		    : D = value / 100
				 4 : Set K value		    : K = value / 10
				 5 : Set preferred speed
				 6 : Manually call interrupt
																		*/
/*******************************************************/
void respondToSettingsData(uint8_t identifier, uint8_t value)
{
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
		case 6:
			callMainInterrupt();
			break;
		case 7:
			debugMode = value;
			break;
		case 9:
			convertAngle90 = 10 * value;
			break;
		case 10:
			convertAngle180 = 10 * value;
			break;
		case 11:
			if (value == 1){
				openPicker();
			} else {
				closePicker();
			}
			break;
		case 14:
			preferredRotationSpeed = value;
			break;
		default:
			break;
	}
}

/*******************************************************/
/*	autonomousForward - autonomously drive forward.

	Prefer to follow left wall, if not available follow right wall.
	If neither are found, continue forward with preferredSpeed. If 
	both walls are found update the preferred distance 
	(preferredDistance) to nearby walls.
	
	Call main module for new control command (callMainInterrupt) each 
	module (40 cm) or when forward sensor detects a wall ahead.
	
	Regulate by adding or subtracting speed from preferredSpeed
	with respect to 
		i) Difference between average distance to wall and 
		   preferredDistance
	   ii) Difference in distance between forward and rear sensor
																		*/
/*******************************************************/
void autonomousForward()
{
	uint8_t frontIndex = noWallsIndex;
	uint8_t backIndex;
	uint8_t speed = preferredSpeed;
	uint8_t distance = preferredDistance;
	float k = K;
	
	/*if (((oldForwardSensor - forwardSensor > moduleDepth) && 
		 (forwardSensor > (moduleDepth - 5))) || 
		 (forwardSensor < minDistanceForward)){
			
		difference = oldForwardSensor - forwardSensor;
		madeChange = 1;
		currControlCommand = stop;
		oldForwardSensor = 0;
		stopWheels();
			 
		if (debugMode == 0) {
			callMainInterrupt();
		}
		
	}*/
	
	if (getForwardAverage() < 40){
		accumulatedDistance = 0;
	}
	
	//&& (forwardSensor > (moduleDepth))) 
	if ((edgesToCentimeter(accumulatedDistance) >= (moduleDepth-5))
	|| (getForwardAverage() < minDistanceForward)){
		
		currControlCommand = stop;
		stopWheels();
		
		//accumulatedDistance = 0;
		
		if (debugMode == 0){
			callMainInterrupt();
		}		
	}
	
	if ((sideSensors[frontLeftIndex] < maxControllerDistance) && 
		(sideSensors[rearLeftIndex] < maxControllerDistance)){
			
		frontIndex = frontLeftIndex;
		backIndex = rearLeftIndex;
		
		distance -= 10;
	} else if ((sideSensors[frontRightIndex] < maxControllerDistance) && 
			   (sideSensors[rearRightIndex] < maxControllerDistance)){
				   
		frontIndex = frontRightIndex;
		backIndex = rearRightIndex;
		distance += 10;
	} else {
		/*if ((sideSensors[frontLeftIndex] < maxDistance) &&
		(sideSensors[rearLeftIndex] < maxDistance) &&
		(sideSensors[rearRightIndex] == maxDistance)){
			
			frontIndex = frontLeftIndex;
			backIndex = rearLeftIndex;
			
			distance -= 10;
		} else if ((sideSensors[frontRightIndex] <= maxDistance) &&
		(sideSensors[rearRightIndex] <= maxDistance) &&
		(sideSensors[rearLeftIndex] == maxDistance)){
			
			frontIndex = frontRightIndex;
			backIndex = rearRightIndex;
			distance += 10;
		}*/
	}
	
	if (getForwardAverage() < moduleDepth ||
		(sideSensors[frontRightIndex] == maxDistance &&
		 sideSensors[rearRightIndex] != maxDistance) ||
		(sideSensors[frontLeftIndex]  == maxDistance &&
		sideSensors[rearLeftIndex] != maxDistance)){
		speed *= 0.6;
		k*= 0.8;
			  
		// Resync accumulatedDistance
		//if (adjustedInCorridor == 0){
		//	accumulatedDistance = 16;
		//	adjustedInCorridor = 1;
		//}

	}
	
	//Adjust in corridor
	if (sideSensors[frontLeftIndex] == maxDistance){
		if (openLeft == 0 && openRight == 0){
			openLeft = 1;
			accumulatedDistance = 14;
		}
	} else {
		openLeft = 0;
	}
	
	if (sideSensors[frontRightIndex] == maxDistance){
		if (openLeft == 0 && openRight == 0){
			openRight = 1;
			accumulatedDistance = 14;
			
		}
	} else {
		openRight = 0;
	}
	
	if (frontIndex == noWallsIndex){
		rightWheelPair(speed, 1);
		leftWheelPair(speed, 1);
		speedLidar = lidarMid;
	} else {
	
		// if both walls available, adjust preferred distance
		/*if (sideSensors[frontRightIndex] != maxControllerDistance && 
			sideSensors[frontLeftIndex] != maxControllerDistance  &&
			sideSensors[rearRightIndex] != maxControllerDistance &&
			sideSensors[rearLeftIndex] != maxControllerDistance ){
			
			//distance = (sideSensors[frontRightIndex] + 
			//					 sideSensors[frontLeftIndex]  + 
			//					 sideSensors[rearRightIndex]  + 
			//					 sideSensors[rearLeftIndex])/4;
								 
			//if ((frontIndex == frontRightIndex) &&
			//	(backIndex == rearRightIndex)){
			//		distance += 20;
			//	}
			//adjustedInCorridor = 0;
		}*/
	
		uint8_t averageDistance = (sideSensors[frontIndex] + 
							sideSensors[backIndex])/2;
		int8_t distanceDifference = sideSensors[frontIndex] - 
							 sideSensors[backIndex];
		
		// save lastDistanceDifference
		lastDistanceDifference = distanceDifference;
		if(distanceDifference > maximumDistanceDifference) {
			maximumDistanceDifference = distanceDifference;
		}
		
		if (distanceDifference > 40){
			distanceDifference = 0;
			averageDistance = 0;
		}
							 
		p_out = P * ((float) (averageDistance - distance));
		
		d_out = D * ((float) distanceDifference);
		
		y_out = (int16_t) (k * (p_out + d_out));
		
		if (labs(y_out) > labs(y_out_old)){
			y_diff = labs(y_out) - labs(y_out_old);
		}
		
		y_out_old = y_out;
		
		if (abs(y_diff) > abs(y_diff_max)){
			y_diff_max = labs(y_diff);
		}
		
		if (((int8_t) y_out) > ymax) {
			ymax = (int8_t) y_out;
		}

		if (frontIndex == 0){
			y_out = -y_out;
		}
	
		if(y_out < 0) {
		
			if (speed - y_out > maxSpeed){
				rightWheelPair(maxSpeed + y_out, 1);
				rightSpeed = maxSpeed + y_out;
				leftWheelPair(maxSpeed, 1);
				leftSpeed = maxSpeed;
			} else {
				rightWheelPair(speed + y_out/2, 1);
				rightSpeed = speed + y_out/2;
				leftWheelPair(speed - y_out/2, 1);
				leftSpeed = speed - y_out/2;
			}
	
		} else {
		
			if (speed + y_out > maxSpeed){
				rightWheelPair(maxSpeed, 1);
				rightSpeed = maxSpeed;
				leftWheelPair(maxSpeed - y_out, 1);
				leftSpeed = maxSpeed-y_out;
			} else {
				rightWheelPair(speed + y_out/2, 1);
				rightSpeed = speed + y_out/2;
				leftWheelPair(speed - y_out/2, 1);
				leftSpeed = speed - y_out/2;
			}
		}
	}
}

/*******************************************************/
/*	autonomousHalfModule - autonomously moves half a module.
																		*/
/*******************************************************/
void autonomousHalfModule(void)
{
	
	if (direction == 1){
		if (edgesToCentimeter(accumulatedDistance) >= halfModuleDepth){
			currControlCommand = stop;
			stopWheels();
	
			if (debugMode == 0){
				callMainInterrupt();
			}
		} 
		
	} else {
		if (edgesToCentimeter(accumulatedDistance) >= halfModuleDepth){
			currControlCommand = stop;
			stopWheels();
			
			if (debugMode == 0){
				callMainInterrupt();
			}
		}
		
		/*if (forwardSensor >= preHalfModuleDistance - 3){
			currControlCommand = stop;
			stopWheels();
						
			if (debugMode == 0){
				callMainInterrupt();
			}
		}*/
	} 
	
	rightWheelPair(preferredHalfModuleSpeed, direction);
	leftWheelPair(preferredHalfModuleSpeed, direction);
	
}

/*******************************************************/
/*	autonomousRotate - autonomously rotate.

	Rotate autonomously with respect to preferred accumulated angle
	(preferredAccumulatedAngle) and call for new control command
	when rotation is considered done.
	
	Before autonomousRotate is called, preferredAccumulatedAngle should 
	be updated using the convertAngle function. convertAngle sets the
	number of degrees and direction to rotate.
	
	Note: labs = long abs 
																		*/
/*******************************************************/
void autonomousRotate()
{
	if(abs(accumulatedAngle) > abs(preferredAccumulatedAngle)){
		if(adjustRotationMode == 1) {
			currControlCommand = commandAdjust;
		} else {
			currControlCommand = stop;
			stopWheels();
			if (debugMode == 0){
			callMainInterrupt();
			}
		}
	} else if (preferredAccumulatedAngle > 0){
		rightWheelPair(preferredRotationSpeed, 1);
		leftWheelPair(preferredRotationSpeed, 0);
	} else {
		rightWheelPair(preferredRotationSpeed, 0);
		leftWheelPair(preferredRotationSpeed, 1);
	}
	
	accumulatedAngle += angularVelocity;
}

/*******************************************************/
/*	autonomousAdjust - autonomously adjust angle to wall.

	Autonomously adjust the angle to the wall or compensate the
	preferred accumulated angle (preferredAccumulatedAngle) 

	Called either before or after rotation, depending on adjust 
	rotation mode (adjustRotationMode). 
	
	Before autonomousAdjust is called, autonomousAdjustMode should
	be updated.
																		*/
/*******************************************************/
void autonomousAdjust()
{
	lastDistanceDifference = 0;
	
	numberOfAdjustSamples += 1;
	
	if ((sideSensors[frontRightIndex] != maxDistance) &&
			   (sideSensors[rearRightIndex] != maxDistance)){
		lastDistanceDifference = sideSensors[frontRightIndex] -
									sideSensors[rearRightIndex];
	} else if ((sideSensors[frontLeftIndex] != maxDistance) &&
		(sideSensors[rearLeftIndex] != maxDistance)){
					
		lastDistanceDifference = sideSensors[rearLeftIndex] -
									sideSensors[frontLeftIndex];
	} 
	
	if(labs(lastDistanceDifference) <= preferredDistanceDifference) {
		if(adjustRotationMode == 2) {
			adjustRotationMode = 1;
			currControlCommand = rotation;
		} else {
			currControlCommand = stop;
			stopWheels();
			if (debugMode == 0){
				callMainInterrupt();
			}
		}
	} else {
		if (lastDistanceDifference < 0){
			rightWheelPair(preferredAdjustSpeed, 1);
			leftWheelPair(preferredAdjustSpeed, 0);
			} else {
			rightWheelPair(preferredAdjustSpeed, 0);
			leftWheelPair(preferredAdjustSpeed, 1);
		}
	}
}

/*******************************************************/
/*	autonomousScan - Scan the corridor ahead to seek the largest 
		distance.														*/
/*******************************************************/
void autonomousScan(void)
{
	if ((currentLidarAngle + lidarIncrement <= lidarMax) &&
		(activeScan == 1)){
		speedLidar = currentLidarAngle;
		currentLidarAngle += lidarIncrement;
		if (getForwardAverage() > lidarMaxDistance){
			lidarMaxDistance = getForwardAverage();
			lidarMaxAngle = speedLidar;
		}
	} else {
		activeScan = 0;
	}
	
	
	if (activeScan == 0){
		if (currentLidarAngle - lidarIncrement >= lidarMid){
			speedLidar = currentLidarAngle;
			currentLidarAngle -= lidarIncrement*2;
		} else {
			oldForwardSensor = lidarMaxDistance;
			//forwardSensor = lidarMaxDistance;
			speedLidar = lidarMaxAngle;
			if (debugMode == 0){
				callMainInterrupt();
			}
		}
	}
}

/*******************************************************/
/*	initInterrupt - Initiate interrupt where main module is called.     */
/*******************************************************/
void initInterrupt()
{
	DDRB |= (1<<DDB4);
	PORTB &= ~(1<<PORTB4);
}

/*******************************************************/
/*	callMainInterrupt - Call main module for new control command.       */
/*******************************************************/
void callMainInterrupt(void)
{

	stopWheels();
	currentLidarAngle = lidarMin;
	currControlCommand = stop;
	called += 1;
	
	PORTB |= (1<<PORTB4);
	PORTB &= ~(1<<PORTB4);
	
	adjustRotationMode = 0;
	accumulatedDistance = 0;
}

/*******************************************************/
/*	setAutonomousLED - Set autonomous LED.
		
		mode = 1 : On
			   2 : Off											        */
/*******************************************************/
void setAutonomousLED(uint8_t mode)
{
	if (mode == 1){
		PORTD |= (1<<PORTD6);
	} else {
		PORTD &= ~(1<<PORTD6);
	}
}

/*******************************************************/
/*	setDebugLED - Set debug LED.
		
		mode = 1 : On
			   2 : Off											        */
/*******************************************************/
void setDebugLED(uint8_t mode)
{
	if (mode == 1){
		PORTD |= (1<<PORTD7);
	} else {
		PORTD &= ~(1<<PORTD7);
	}
}

/*******************************************************/
/*	initLED - Initiate and set LEDs.								    */
/*******************************************************/
void initLED(void)
{
	DDRD |= (1<<DDD6)|(1<<DDD7);
	
	setAutonomousLED(autonomousMode);
	setDebugLED(debugMode);
}

/*******************************************************/
/*	initCommandModule - Initiate Command module.						*/
/*******************************************************/
void initCommandModule(void)
{
	TWISetup(mySlaveAdress);
	initPWM();
	initLCD();
	
	initInterrupt();
	
	sei();

	autonomousMode = 1;
	debugMode = 0;
	initLED();
	
	speedLidar = lidarMid;
	lidarMax = lidarMid + 60;
	lidarMin = lidarMid - 60;
}

/*******************************************************/
/*	edgedToCentimeter- Convert number of edges to centimeter.
		edgeCount: Number of edges.										*/
/*******************************************************/
uint8_t edgesToCentimeter(uint8_t edgeCount)
{
	return (uint8_t) (wheelDiameter*M_PI*((float) edgeCount)/
					  wheelNumberOfSections);
}

/*******************************************************/
/*	edgedToCentimeter- Convert number of edges to centimeter.
		edgeCount: Number of edges.										*/
/*******************************************************/
uint8_t centimeterToEdges(uint8_t centimeter)
{
	return (uint8_t) ((wheelNumberOfSections)/
					  (wheelDiameter*M_PI*((float) centimeter)));
}

uint16_t getForwardAverage(void){
	return (uint16_t) 
		   (forwardSensor[0] + 
			forwardSensor[1] + 
			forwardSensor[2] + 
			forwardSensor[3] + 
			forwardSensor[4])/5;
}

/*******************************************************/
/*	main - Main function.

	Initiate PWM, LCD, interrupt and TWI. Print data to LCD when
	madeChange is set to 1.
																		*/
/*******************************************************/
int main(void)
{
	initCommandModule();
	
	char bottomRowMessage[16] = "";
	char topRowMessage[16] = "";
	
	openPicker();
	
	while(1)
	{
		setDebugLED(debugMode);
		setAutonomousLED(autonomousMode);
	
		if (madeChange >= 1){
			sprintf(topRowMessage, "Yo:%d", y_out);
			lcdWriteTopRow(topRowMessage);
			sprintf(bottomRowMessage, "NoAs:%d", 
					numberOfAdjustSamples);
			lcdWriteBottomRow(bottomRowMessage);
			madeChange = 0;
		} 
	}
}
