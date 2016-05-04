﻿/*
 * searchPath.h
 *
 * Created: 4/28/2016 1:18:29 PM
 *  Author: ollul666
 */ 

#include <stdlib.h>

//TWI
int SLA_styr_W = 0xCC;

//////////////////////////////////////////////////////////////////////////
uint8_t keepExploring = 1;
//////////////////////////////////////////////////////////////////////////

/************************************************************************/
/*                       POSITION AND ORIENTATION                       */
/************************************************************************/
uint8_t direction = 0;// 0: north, 1: east, 2: south, 3: west
uint8_t position[2] = {14,14}; //Where the robot is right now (is updated after command is sent)
	
/************************************************************************/
/*                           INTERNAL MAP                               */
/************************************************************************/
uint8_t walls[3]; //[0] = Right side, [1]: forward, [2]: left side (0 if open, 1 if wall)	
uint8_t map[28][28]; //F5: initial value, F4: wall, F3: target, F2: start, [0,225] steps from start
uint8_t path[28][28]; //FF initial value, F1 blocked way, >0 steps from target

uint16_t straightAhead = 0; //Value straight ahead (Lidar)
uint16_t oldStraighAhead = 0;
uint16_t oneModuleAhead = 24; //When can we say that the next module is a wall?

uint8_t	targetPathValue = 0;
uint8_t blockedWayValue = 0xF1;
uint8_t startPositionValue[2] = {0xF2,0xF0};
uint8_t	targetValue = 0xF3;
uint8_t wallValue = 0xF4;
uint8_t	initialValue = 0xF5;

uint8_t hasFoundWayBack = 0; //Keeps track of whether or not to block a way at an intersection

/************************************************************************/
/*                              TARGET                                  */
/************************************************************************/
uint8_t target[3] = {0xFF,0xFF,0xFF}; //x-coordinate, y-coordinate, value in map
uint8_t z[2] = {0xFF,0xFF}; //Estimations of shortest path (optimistic, pessimistic)

/************************************************************************/
/*                            CONTROLLER                                */
/************************************************************************/
uint8_t lastCommand[3]; //Last sent controller command
uint8_t oneForward[3] = {0xFF, 0x01, 0x01};
uint8_t rotateRight[3] = {0xFF, 0x03, 0x5A};
uint8_t rotateLeft[3] = {0xFF, 0x04, 0x5A};
uint8_t rotate180[3] = {0xFF, 0x02, 0xB4};
uint8_t openClaw[3] = {0xFF, 0x09, 0x01};
uint8_t falseCommand[3] = {0,0,0};

uint8_t forward = 0x01;
uint8_t oneEighty = 0x02;
uint8_t right = 0x03;
uint8_t left = 0x04;
uint8_t claw = 0x09;
	
uint8_t forward = 0x01;
uint8_t oneEighty = 0x02;
uint8_t right = 0x03;
uint8_t left = 0x04;

/************************************************************************/
/*                            SHORTEST PATH                             */
/************************************************************************/
uint8_t currentCommand[3];
uint8_t doneShortestPathInit = 0;
uint8_t shortestPath[150][2];

uint8_t s;
uint8_t t;
uint8_t u;
uint8_t v;
uint8_t w;
uint8_t r;

uint8_t dist;

int nrOfCoordinates = 0;
int returnStart = 0xFF;
int goTheShortestPath = 0;

/************************************************************************/
/*                              HEADER                                  */
/************************************************************************/
void btSend(unsigned char data);
uint8_t * chooseDirection();
uint8_t deadEndTarget();
uint8_t distanceToStart(uint8_t x, uint8_t y);
uint8_t distanceToTarget(uint8_t x, uint8_t y);
void explore(void);
uint8_t * exploreTargetFound();
uint8_t * findStart();
uint8_t * findTarget();
uint8_t * findWayBack();
uint8_t * findWayBackTarget();
uint8_t hasFoundTarget(void);
void newCoordinates();
void newDirection(uint8_t rotation, uint8_t degrees)
void readSensors();
void returnToStart();
void ruleOutPath();
void searchPathInit();
void sendMapCoordinate(uint8_t x, uint8_t y);
void shortestPathInit();
void shortestPathToTarget();
uint8_t unexploredPaths();
void updateCoordinates();
void updateTargetFound();


void btSend(unsigned char data)
{
	// Wait for empty transmit buffer
	while ( !( UCSR0A & (1<<UDRE0)) );
	// Put data into buffer, sends the data
	UDR0 = data;
}

/************************************************************************/
/*	findTarget - Sends a controller command according to search 
		algorithm

	walls[i] = 	0	open
				1	wall
																		*/
/************************************************************************/
uint8_t * findTarget()
{
	if(walls[0] == 0){ //Rotate 90 degrees to the right
		return rotateRight;
		} else if(walls[1] == 0){ //One forward
		return oneForward;
		} else if (walls[2] == 0){ //Rotate 90 degrees to the left
		return rotateLeft;
		} else { //Rotate 180 degrees (clockwise)
		return rotate180;
	}
}

/************************************************************************/
/*	ruleOutPath - rules out path for easier calculation of shortest 
		path
		
	Sets appropriate coordinate in path to 0xF4
																		*/
/************************************************************************/
void ruleOutPath()
{
	switch(direction){
		case 0:
		path[position[0]][position[1]-1] = blockedWayValue;
		break;
		case 1:
		path[position[0]-1][position[1]] = blockedWayValue;
		break;
		case 2:
		path[position[0]][position[1]+1] = blockedWayValue;
		break;
		case 3:
		path[position[0]+1][position[1]] = blockedWayValue;
		break;
	}
}

/************************************************************************/
/*	hasFoundTarget - checks if target has been found

	Returns: 	0 	not found
				1	target in the module ahead
				2	target found 
																		*/
/************************************************************************/
uint8_t hasFoundTarget(void) //NEEDS IMPLEMENTATION!!!
{
	//Göra skillnad på tre tillstånd: Målet ej hittat, ser målet, vet precis vilken kartmodul målet befinner sig i???
	/*if((target[0] != 0xFF) && (target[1] != 0xFF)){
		//
	} else {
		//Check sensors to see if target is found
	}*/
	
	return 0;
	
	/*if((position[0] == 11) && (position[1] == 20) && (direction == 0)){
		return 1;
	} else if((target[0] != 0xFF) && (target[1] != 0xFF)){
		return 2;
	} else {
		return 0;
	}*/
}

/************************************************************************/
/*	unexploredPaths - checks if there are any unexplored paths

	Returns:	0	No unexplored path
				1	Unexplored path(s)
																		*/
/************************************************************************/
uint8_t unexploredPaths()
{
	if((map[position[0]+1][position[1]] == initialValue) && (path[position[0]+1][position[1]] != blockedWayValue)){
		return 1;
		} else if((map[position[0]-1][position[1]] == initialValue) && (path[position[0]-1][position[1]]!= blockedWayValue)){
		return 1;
		} else if ((map[position[0]][position[1]+1] == initialValue) && (path[position[0]][position[1]+1] != blockedWayValue)){
		return 1;
		} else if ((map[position[0]][position[1]-1] == initialValue) && (path[position[0]][position[1]-1] != blockedWayValue)){
		return 1;
		} else {
		return 0;
	}
}

/************************************************************************/
/*	readSensors - checks in which directions there are walls

	Updates map and walls accordingly
																		*/
/************************************************************************/
void readSensors()
{
	walls[0] = 0;
	walls[1] = 0;
	walls[2] = 0;
	
	straightAhead = sensorData[10]*128 + sensorData[12];
	

	//Cardinal direction: north, east, south, west
	if(direction == 0){
		//Straight ahead, right, !!south open!!, left
		if (straightAhead < oneModuleAhead){
			map[position[0]][position[1]+1] = wallValue;
			walls[1] = 1;
		}

		if ((sensorData[2] < 245) && (sensorData[6] < 245)){
			map[position[0]+1][position[1]] = wallValue;
			walls[0] = 1;
		}

		if ((sensorData[4] < 245) && (sensorData[8] < 245)){
			map[position[0]-1][position[1]] = wallValue;
			walls[2] = 1;
		}

	} else if(direction == 1) {
		//Left, straight ahead, right, !!west open!!
		if ((sensorData[4] < 245) && (sensorData[8] < 245)){
			map[position[0]][position[1]+1] = wallValue;
			walls[2] = 1;
		}

		if (straightAhead < oneModuleAhead){
			map[position[0]+1][position[1]] = wallValue;
			walls[1] = 1;
		}

		if ((sensorData[2] < 245) && (sensorData[6] < 245)){
			map[position[0]][position[1]-1] = wallValue;
			walls[0] = 1;
		}
	} else if(direction == 2) {
		//!!north open!!, left, straight ahead, right
		if ((sensorData[4] < 245) && (sensorData[8] < 245)){
			map[position[0]+1][position[1]] = wallValue;
			walls[2] = 1;
		}

		if (straightAhead < oneModuleAhead){
			map[position[0]][position[1]-1] = wallValue;
			walls[1] = 1;
		}

		if ((sensorData[2] < 245) && (sensorData[6] < 245)){
			map[position[0]-1][position[1]] = wallValue;
			walls[0] = 1;
		}


	} else if (direction == 3){
		//Right, !!east open!!, left, straight ahead
		if ((sensorData[2] < 245) && (sensorData[6] < 245)){
			map[position[0]][position[1]+1] = wallValue;
			walls[0] = 1;
		}

		if ((sensorData[4] < 245) && (sensorData[8] < 245)){
			map[position[0]][position[1]-1] = wallValue;
			walls[2] = 1;
		}

		if (straightAhead < oneModuleAhead){
			map[position[0]-1][position[1]] = wallValue;
			walls[1] = 1;
		}
	}
	
	//Send map
	
	sendMapCoordinate(position[0], position[1]);
	if (position[0] + 1 < 28){
		sendMapCoordinate(position[0] + 1, position[1]);
	}
	if (position[0] - 1 >= 0){
		sendMapCoordinate(position[0] - 1, position[1]);
	}
	if (position[1] + 1 < 28){
		sendMapCoordinate(position[0], position[1] + 1);
	}
	if (position[0] - 1 >= 0){
		sendMapCoordinate(position[0], position[1] - 1);
	}
	
	
}

/************************************************************************/
/*	sendMapCoordinate - sends map information to the computer			*/															*/
/************************************************************************/
void sendMapCoordinate(uint8_t x, uint8_t y)
{
	btSend(0xFE);
	btSend(x);
	btSend(y);
	btSend(map[x][y]);
}

/************************************************************************/
/*	searchPathInit - initiate map and path

	Fills map and path with initialValue and sets startPositionValue
																		*/
/************************************************************************/
void searchPathInit() //Fills map and path, sets initial values 
{
	//Fill path with 255, map with 0
	for(int i = 0;i<28;i++){
		for(int j = 0;j<28;j++){
			path[i][j] = initialValue;
			map[i][j] = initialValue;
		}
	}
	
	//Set start value for map
	map[14][14] = startPositionValue[0];
	map[14][13] = startPositionValue[1];
}

/************************************************************************/
/*	newDirection - updates direction in which the robot is positioned.

	rotation:	0x02	180 degrees
				0x03	right turn
				0x04	left turn
																		*/
/************************************************************************/
void newDirection(uint8_t rotation, uint8_t degrees)
{
	if(degrees == 0x5A){
		if(((direction == 0) && (rotation == 0x03)) || ((direction == 2) && (rotation == 0x04))){
			direction = 1; //east
			} else if (((direction == 0) && (rotation == 0x04)) || ((direction == 2) && (rotation == 0x03))){
			direction = 3; //west
			} else if(((direction == 1) && (rotation == 0x03)) || ((direction == 3) && (rotation == 0x04))){
			direction = 2; //south
			} else {
			direction = 0; //north
		}
	} else {
		switch(direction){
			case 0:
			direction = 2;
			break;
			case 1:
			direction = 3;
			break;
			case 2:
			direction = 0;
			break;
			case 3:
			direction = 1;
			break;
		}
	}
}

/************************************************************************/
/*	deadEndTarget - keeps track of whether or not the target was found 
		in the current dead end
																		*/
/************************************************************************/
uint8_t deadEndTarget() //Was the target found in the current dead end?
{
	switch(direction){
		case 0:
		if((target[0] == position[0]) && (target[1] == position[1] - 1)){
			return 1;
			} else {
			return 0;
		}
		break;
		case 1:
		if((target[0] == position[0] - 1) && (target[1] == position[1])){
			return 1;
			} else {
			return 0;
		}
		break;
		case 2:
		if((target[0] == position[0]) && (target[1] == position[1] + 1)){
			return 1;
			} else {
			return 0;
		}
		break;
		case 3:
		if((target[0] == position[0] + 1) && (target[1] == position[1])){
			return 1;
			} else {
			return 0;
		}
		break;
	}
	
	return 0;
}

/************************************************************************/
/*	updateCoordinates - updates map, path and position when a move 
		forward has been made
																		*/
/************************************************************************/
void updateCoordinates() //Updates map, path and position variables
{
	//Cardinal direction: north, east, south, west
	if (direction == 0){
		//increase length from start by one
		if (map[position[0]][position[1]+1] == initialValue){
			if (map[position[0]][position[1]] == startPositionValue[0]){
				map[position[0]][position[1]+1] = 1;
			} else{
				map[position[0]][position[1]+1] = map[position[0]][position[1]] + 1;
			}
		}
		if (((path[position[0]][position[1]+1] == initialValue) || (path[position[0]][position[1]+1] == blockedWayValue)) && (hasFoundTarget())){
			path[position[0]][position[1]+1] = path[position[0]][position[1]] + 1;
		}		
		position[1] = position[1] + 1;
	} else if (direction == 1){
		//increase length from start by one
		if (map[position[0]+1][position[1]] == initialValue){
			if (map[position[0]][position[1]] == startPositionValue[0]){
				map[position[0]+1][position[1]] = 1;
			} else{
				map[position[0]+1][position[1]] = map[position[0]][position[1]] + 1;
			}
		}
		if (((path[position[0]+1][position[1]] == initialValue) || (path[position[0]+1][position[1]] == blockedWayValue)) && (hasFoundTarget())){
			path[position[0]+1][position[1]] = path[position[0]][position[1]] + 1;
		}
		position[0] = position[0] + 1;
	} else if (direction == 2){
		//increase length from start by one
		if (map[position[0]][position[1]-1] == initialValue){
			if (map[position[0]][position[1]] == startPositionValue[0]){
				map[position[0]][position[1]-1] = 1;
			} else{
				map[position[0]][position[1]-1] = map[position[0]][position[1]] + 1;
			}
		}
		if (((path[position[0]][position[1]-1] == initialValue) || (path[position[0]][position[1]-1] == blockedWayValue)) && (hasFoundTarget())){
			path[position[0]][position[1]-1] = path[position[0]][position[1]] + 1;
		}
		position[1] = position[1] - 1;
	} else {
		//increase length from start by one
		if (map[position[0]-1][position[1]] == initialValue){
			if (map[position[0]][position[1]] == startPositionValue[0]){
				map[position[0]-1][position[1]] = 1;
			} else{
				map[position[0]-1][position[1]] = map[position[0]][position[1]] + 1;
			}
		}
		if (((path[position[0]-1][position[1]] == initialValue) || (path[position[0]-1][position[1]] == blockedWayValue)) && (hasFoundTarget())){
			path[position[0]-1][position[1]] = path[position[0]][position[1]] + 1;
		}
		position[0] = position[0] - 1;
	}
}

/************************************************************************/
/*	findWayBack - decides where to go to get closer to the start

	Follows the numbers in map in decending order
																		*/
/************************************************************************/
uint8_t * findWayBack() //Where to go to get closer to the start 
{	
	if((map[position[0]+1][position[1]] == map[position[0]][position[1]] - 1) || (map[position[0]+1][position[1]] == startPositionValue[0])){
		switch (direction){// 0: north, 1: east, 2: south, 3: west
			case 0: 
				//Rotate right
				return rotateRight;
				break;
			case 1:	
				//One forward, update position with new coordinates
				return oneForward;
				break;
			case 2:
				//Rotate left
				return rotateLeft;
				break;
			case 3:
				//Should never happen??
				//Rotate 180 degrees
				return rotate180;
				break;
		}
	} else if((map[position[0]][position[1]+1] == map[position[0]][position[1]] - 1) || (map[position[0]][position[1]+1] == startPositionValue[0])){
		switch (direction){// 0: north, 1: east, 2: south, 3: west
			case 0: 
				//One forward
				return oneForward;
				break;
			case 1:
				//Rotate left
				return rotateLeft;
				break;
			case 2:
				return rotate180;
				break;
			case 3: 
				//Rotate right
				return rotateRight;
				break;
		}
	} else if((map[position[0]-1][position[1]] == map[position[0]][position[1]] - 1) || (map[position[0]-1][position[1]] == startPositionValue[0])){
		switch (direction){// 0: north, 1: east, 2: south, 3: west
			case 0:
				//Rotate left
				return rotateLeft;
				break;
			case 1:
				//Should never happen??
				//Rotate 180 degrees
				return rotate180;
				break;
			case 2:
				//Rotate right
				return rotateRight;
				break;
			case 3:
				//One forward
				return oneForward;
				break;
			}
	} else if((map[position[0]][position[1]-1] == map[position[0]][position[1]] - 1) || (map[position[0]][position[1]-1] == startPositionValue[0])){
		switch (direction){// 0: north, 1: east, 2: south, 3: west
			case 0:
				//Should never happen??
				//Rotate 180 degrees
				return rotate180;
				break;
			case 1:	
				//Rotate right
				return rotateRight;
				break;
			case 2:
				//One forward
				return oneForward;
				break;
			case 3:
				//Rotate left
				return rotateLeft;
				break;
		}
	}
	
	return falseCommand;
	
}

/************************************************************************/
/*	explore - Decides where to go depending on whether or not the 
		target has been found and updates variables accordingly
																		*/
/************************************************************************/
void explore(void)
{
	
	if(hasFoundTarget() == 1){ 
		//Update map with the right number and target with the right 
		//updateTargetFound(); //Only just when the target has been found			
	} 
	
	if((lastCommand[1] == right) || (lastCommand[1] == left) || (lastCommand[1] == oneEighty)) {
		//straightAhead = sensorData[10]*256 + sensorData[12];
		lastCommand[1] = forward;
		
		Master(3,SLA_styr_W,lastCommand);
		for (int k = 0; k < 3; k++)	{
			btSend(lastCommand[k]);
		}
	} else if (unexploredPaths()) {
	
		if((hasFoundWayBack)){
			hasFoundWayBack = 0;
			ruleOutPath();
		}			
	
		readSensors();
		uint8_t * temp;
	
		if(hasFoundTarget()){
			//temp = exploreTargetFound();
		} else {
			temp = findTarget();
		}
	
		if ((hasFoundTarget() == 0) && (position[0] == 14) && (position[1] == 14)){
			if (lastCommand[0] != 0){
				lastCommand[1] = 0x00;
				Master(3,SLA_styr_W,lastCommand);
				keepExploring = 0;
			}
		}
	
		lastCommand[0] = temp[0];
		lastCommand[1] = temp[1];
		lastCommand[2] = temp[2];
	
		if(lastCommand[1] != 0){
			Master(3,SLA_styr_W,lastCommand);
			for (int k = 0; k < 3; k++)	{
				btSend(lastCommand[k]);
			}
		}
	
	} else {
	
	
		////////////////////////////////////////////////////////
		if((hasFoundWayBack)){
			hasFoundWayBack = 0;
			ruleOutPath();
		}	
		////////////////////////////////////////////////////////
	
		uint8_t * temp;

		if (hasFoundTarget()){
			temp = findWayBack();
		} else {
			hasFoundWayBack = 1;
			temp = findWayBack(0x01);
		}
	
		if ((hasFoundTarget() == 0) && (position[0] == 14) && (position[1] == 14)){
			if (lastCommand[0] != 0){
				lastCommand[1] = 0x00;
				Master(3,SLA_styr_W,lastCommand);
				keepExploring = 0;
			}
		}
	
		lastCommand[0] = temp[0];
		lastCommand[1] = temp[1];
		lastCommand[2] = temp[2];
	
		if(lastCommand[1] != 0){
			Master(3,SLA_styr_W,lastCommand);
		}
		for (int k = 0; k < 3; k++)	{
				btSend(lastCommand[k]);
		}
	}
	
	
	if((lastCommand[1] == right) || (lastCommand[1] == left) || (lastCommand[1] == oneEighty)) {
		newDirection(lastCommand[1],lastCommand[2]);
		uint8_t deadEnd = deadEndTarget();
		if((lastCommand[2] == 0xB4) && deadEnd == 0 && !hasFoundTarget()){
			hasFoundWayBack = 1;
		}
	} else {
		updateCoordinates();

	}
}

/************************************************************************/
/*	ruleOutPath - rules out path for easier calculation of shortest 
		path
		
	Sets appropriate coordinate in path to 0xF4
																		*/
/************************************************************************/
uint8_t distanceToStart(uint8_t x, uint8_t y) //Returns the Manhattan distance from (x,y) to start
{
	return abs(x-14)+abs(y-14);
}

uint8_t distanceToTarget(uint8_t x, uint8_t y) //Returns the walking distance from (x,y) to target
{
	dist = path[x][y];
	return dist;
}

uint8_t * findWayBackTarget() //Where to go to get closer to the start
{
	if(path[position[0]+1][position[1]] == path[position[0]][position[1]] - 1){
		switch (direction){// 0: north, 1: east, 2: south, 3: west
			case 0:
				//Rotate right
				return rotateRight;
				break;
			case 1:
				//One forward, update position with new coordinates
				return oneForward;
				break;
			case 2:
				//Rotate left
				return rotateLeft;
				break;
			case 3:
				//Should never happen??
				//Rotate 180 degrees
				return rotate180;
				break;
		}
	} else if(path[position[0]][position[1]+1] == path[position[0]][position[1]] - 1){
		switch (direction){// 0: north, 1: east, 2: south, 3: west
			case 0:
				//One forward
				return oneForward;
				break;
			case 1:
				//Rotate left
				return rotateLeft;
				break;
			case 2:
				return rotate180;
				break;
			case 3:
				//Rotate right
				return rotateRight;
				break;
		}
	} else if(path[position[0]-1][position[1]] == path[position[0]][position[1]] - 1){
		switch (direction){// 0: north, 1: east, 2: south, 3: west
			case 0:
				//Rotate left
				return rotateLeft;
				break;
			case 1:
				//Should never happen??
				//Rotate 180 degrees
				return rotate180;
				break;
			case 2:
				//Rotate right
				return rotateRight;
				break;
			case 3:
				//One forward
				return oneForward;
				break;
		}
	} else if(path[position[0]][position[1]-1] == path[position[0]][position[1]] - 1){
		switch (direction){// 0: north, 1: east, 2: south, 3: west
			case 0:
				//Should never happen??
				//Rotate 180 degrees
				return rotate180;
				break;
			case 1:
				//Rotate right
				return rotateRight;
				break;
			case 2:
				//One forward
				return oneForward;
				break;
			case 3:
				//Rotate left
				return rotateLeft;
				break;
		}
	}
	
	return falseCommand;
	
}

void updateTargetFound()//Updates map, path and position variables when target is found
{
	switch(direction){
		case 0:
			map[position[0]][position[1]+1] = targetValue;
			target[0] = position[0];
			target[1] = position[1]+1;
			target[2] = map[position[0]][position[1]] + 1;
			break;
		case 1:
			map[position[0]+1][position[1]] = targetValue;
			target[0] = position[0]+1;
			target[1] = position[1];
			target[2] = map[position[0]][position[1]] + 1;
			break;
		case 2:
			map[position[0]][position[1]-1] = targetValue;
			target[0] = position[0];
			target[1] = position[1]-1;
			target[2] = map[position[0]][position[1]] + 1;
			break;
		case 3:
			map[position[0]-1][position[1]] = targetValue;
			target[0] = position[0]-1;
			target[1] = position[1];
			target[2] = map[position[0]][position[1]] + 1;
			break;
		}

	
	path[target[0]][target[1]] = targetPathValue;
	path[position[0]][position[1]] = path[target[0]][target[1]] + 1;
	
	z[0] = distanceToStart(target[0],target[1]); //Optimistic value
	z[1] = target[2]; //Pessimistic value
}

uint8_t * exploreTargetFound() //Decides where to go when target is found
{ 
	switch(direction){
		case 0:
			w = distanceToStart(position[0]+1,position[1]);
			r = distanceToTarget(position[0],position[1]) + 1;
			
			u = distanceToStart(position[0],position[1]+1);
			v = distanceToTarget(position[0],position[1]) + 1;
			
			s = distanceToStart(position[0]-1,position[1]);
			t = distanceToTarget(position[0],position[1]) + 1;
		
			if((map[position[0]+1][position[1]] == initialValue) && ( w + r < z[1])) {
				return rotateRight;
			} else if((map[position[0]][position[1]+1] == initialValue) && ( u + v < z[1])){ 
				path[position[0]+1][position[1]] = blockedWayValue;
				path[target[0]][target[1]] = targetPathValue;
				return oneForward;
			} else if((map[position[0]-1][position[1]] == initialValue) && ( s + t < z[1])){ 
				path[position[0]+1][position[1]] = blockedWayValue;
				path[position[0]][position[1]+1] = blockedWayValue;
				path[target[0]][target[1]] = targetPathValue;
				return rotateLeft;
			} else {
				path[position[0]+1][position[1]] = blockedWayValue;
				path[position[0]][position[1]+1] = blockedWayValue;
				path[position[0]-1][position[1]] = blockedWayValue;
				path[target[0]][target[1]] = targetPathValue;
				return rotate180;
			}
		case 1:
			w = distanceToStart(position[0],position[1]-1);
			r = distanceToTarget(position[0],position[1])+1;
			
			u = distanceToStart(position[0]+1,position[1]);
			v = distanceToTarget(position[0],position[1])+1;
			
			s = distanceToStart(position[0],position[1]+1);
			t = distanceToTarget(position[0],position[1])+1;
			
			if((map[position[0]][position[1]-1] == initialValue) && (w + r < z[1])){ 
				return rotateRight;
			} else if((map[position[0]+1][position[1]] == initialValue) && (u + v < z[1])){ 
				path[position[0]][position[1]-1] = blockedWayValue;
				path[target[0]][target[1]] = targetPathValue;
				return oneForward;
			} else if((map[position[0]][position[1]+1] == initialValue) && (s + t < z[1])){
				path[position[0]][position[1]-1] = blockedWayValue;
				path[position[0]+1][position[1]] = blockedWayValue;
				path[target[0]][target[1]] = targetPathValue;
				return rotateLeft;
			} else {
				path[position[0]][position[1]-1] = blockedWayValue;
				path[position[0]+1][position[1]] = blockedWayValue;
				path[position[0]][position[1]+1] = blockedWayValue;
				path[target[0]][target[1]] = targetPathValue;
				return rotate180;
			}
		case 2:
			w = distanceToStart(position[0]-1,position[1]);
			r = distanceToTarget(position[0],position[1])+1;
			
			u = distanceToStart(position[0],position[1]-1);
			v = distanceToTarget(position[0],position[1])+1;
			
			s = distanceToStart(position[0]+1,position[1]);
			t = distanceToTarget(position[0],position[1])+1;
			
			if((map[position[0]-1][position[1]] == initialValue) && (w + r < z[1])){ 
				return rotateRight;
			} else if((map[position[0]][position[1]-1] == initialValue) && (u + v < z[1])){ 
				path[position[0]-1][position[1]] = blockedWayValue;
				path[target[0]][target[1]] = targetPathValue;
				return oneForward;
			} else if((map[position[0]+1][position[1]] == initialValue) && (s + t < z[1])){
				path[position[0]-1][position[1]] = blockedWayValue;
				path[position[0]][position[1]-1] = blockedWayValue;
				path[target[0]][target[1]] = targetPathValue;
				return rotateLeft;
			} else {
				path[position[0]-1][position[1]] = blockedWayValue;
				path[position[0]][position[1]-1] = blockedWayValue;
				path[position[0]+1][position[1]] = blockedWayValue;
				path[target[0]][target[1]] = targetPathValue;
				return rotate180;
			}
		case 3:
			w = distanceToStart(position[0],position[1]+1);
			r = distanceToTarget(position[0],position[1])+1;
			
			u = distanceToStart(position[0]-1,position[1]);
			v = distanceToTarget(position[0],position[1])+1;
			
			s = distanceToStart(position[0],position[1]-1);
			t = distanceToTarget(position[0],position[1])+1;
			
			if((map[position[0]][position[1]+1] == initialValue) && (w + r < z[1])){ 
				return rotateRight;
			} else if((map[position[0]-1][position[1]] == initialValue) && (u + v < z[1])){ 
				path[position[0]][position[1]+1] = blockedWayValue;
				path[target[0]][target[1]] = targetPathValue;
				return oneForward;
			} else if((map[position[0]][position[1]-1] == initialValue) && (s + t < z[1])){ 
				path[position[0]][position[1]+1] = blockedWayValue;
				path[position[0]-1][position[1]] = blockedWayValue;
				path[target[0]][target[1]] = targetPathValue;
				return rotateLeft;
			} else {
				path[position[0]][position[1]+1] = blockedWayValue;
				path[position[0]-1][position[1]] = blockedWayValue;
				path[position[0]][position[1]-1] = blockedWayValue;
				path[target[0]][target[1]] = targetPathValue;
				return rotate180;
			}
	}
	return falseCommand;
	
}

uint8_t * chooseDirection()
{
	uint8_t currentCoordinateValue = 0xF9;
	
	if(distanceToTarget(position[0],position[1]) == 1){
		return openClaw;
	} else {
		switch(direction){
			case 0:
				if(path[position[0]+1][position[1]] < currentCoordinateValue){
					currentCoordinateValue = path[position[0]+1][position[1]];
					currentCommand[0] = rotateRight[0];
					currentCommand[1] = rotateRight[1];
					currentCommand[2] = rotateRight[2];
				}
			
				if(path[position[0]][position[1]+1] < currentCoordinateValue){
					currentCoordinateValue = path[position[0]][position[1]+1];
					currentCommand[0] = oneForward[0];
					currentCommand[1] = oneForward[1];
					currentCommand[2] = oneForward[2];
				}
			
				if(path[position[0]-1][position[1]] < currentCoordinateValue){
					currentCoordinateValue = path[position[0]-1][position[1]];
					currentCommand[0] = rotateLeft[0];
					currentCommand[1] = rotateLeft[1];
					currentCommand[2] = rotateLeft[2];
				}
			
				return currentCommand;
		
			case 1:
				if(path[position[0]+1][position[1]] < currentCoordinateValue){
					currentCoordinateValue = path[position[0]+1][position[1]];
					currentCommand[0] = oneForward[0];
					currentCommand[1] = oneForward[1];
					currentCommand[2] = oneForward[2];
				}
		
				if(path[position[0]][position[1]+1] < currentCoordinateValue){
					currentCoordinateValue = path[position[0]][position[1]+1];
					currentCommand[0] = rotateLeft[0];
					currentCommand[1] = rotateLeft[1];
					currentCommand[2] = rotateLeft[2];
				}

				if(path[position[0]][position[1]-1] < currentCoordinateValue){
					currentCoordinateValue = path[position[0]][position[1]-1];
					currentCommand[0] = rotateRight[0];
					currentCommand[1] = rotateRight[1];
					currentCommand[2] = rotateRight[2];
				}
		
				return currentCommand;
		
			case 2:
				if(path[position[0]+1][position[1]] < currentCoordinateValue){
					currentCoordinateValue = path[position[0]+1][position[1]];
					currentCommand[0] = rotateLeft[0];
					currentCommand[1] = rotateLeft[1];
					currentCommand[2] = rotateLeft[2];
				}
			
				if(path[position[0]-1][position[1]] < currentCoordinateValue){
					currentCoordinateValue = path[position[0]-1][position[1]];
					currentCommand[0] = rotateRight[0];
					currentCommand[1] = rotateRight[1];
					currentCommand[2] = rotateRight[2];
				}
			
				if(path[position[0]][position[1]-1] < currentCoordinateValue){
					currentCoordinateValue = path[position[0]][position[1]-1];
					currentCommand[0] = oneForward[0];
					currentCommand[1] = oneForward[1];
					currentCommand[2] = oneForward[2];
				}
			
				return currentCommand;
			
			case 3:			
				if(path[position[0]][position[1]+1] < currentCoordinateValue){
					currentCoordinateValue = path[position[0]][position[1]+1];
					currentCommand[0] = rotateRight[0];
					currentCommand[1] = rotateRight[1];
					currentCommand[2] = rotateRight[2];
				}
			
				if(path[position[0]-1][position[1]] < currentCoordinateValue){
					currentCoordinateValue = path[position[0]-1][position[1]];
					currentCommand[0] = oneForward[0];
					currentCommand[1] = oneForward[1];
					currentCommand[2] = oneForward[2];
				}
			
				if(path[position[0]][position[1]-1] < currentCoordinateValue){
					currentCoordinateValue = path[position[0]][position[1]-1];
					currentCommand[0] = rotateLeft[0];
					currentCommand[1] = rotateLeft[1];
					currentCommand[2] = rotateLeft[2];
				}
			
				return currentCommand;
		}
	
	}
}

void newCoordinates()
{
	switch(direction){
		case 0:
			position[1] = position[1] + 1;
			break;
		case 1:
			position[0] = position[0] + 1;
			break;
		case 2:
			position[1] = position[1] - 1;
			break;
		case 3:
			position[0] = position[0] - 1;
			break;
	}
	
	nrOfCoordinates = nrOfCoordinates + 1;
	shortestPath[nrOfCoordinates][0] = position[0];
	shortestPath[nrOfCoordinates][1] = position[1];
}

void shortestPathToTarget()
{
	uint8_t *temp;
	temp = chooseDirection();	
	
	lastCommand[0] = temp[0];
	lastCommand[1] = temp[1];
	lastCommand[2] = temp[2];
	if(lastCommand[1] != 0){
		//Master(3,...,lastCommand);
		/*for (int k = 0; k < 3; k++)	{
			btSend(lastCommand[i]);
		}*/
	}
	
	if((hasFoundWayBack)){
		hasFoundWayBack = 0;
		ruleOutPath();
	}
	
	if((lastCommand[1] == right) || (lastCommand[1] == left) || (lastCommand[1] == oneEighty)) {
		newDirection(lastCommand[1],lastCommand[2]);
	} else if (lastCommand[1] != claw){
		newCoordinates();
	}
}

void shortestPathInit()
{
	shortestPath[0][0] = 14;
	shortestPath[0][1] = 14;
	direction = 0;
}

uint8_t * findStart()
{
	if ((position[0] == shortestPath[returnStart][0]) && (position[1]-1 == shortestPath[returnStart][1])){
		switch(direction){
			case 0:
				return rotate180;
				break;
			case 1:
				return rotateRight;
				break;
			case 2:
				returnStart = returnStart - 1;
				return oneForward;
				break;
			case 3:
				return rotateLeft;
				break;
		}
	} else if ((position[0] == shortestPath[returnStart][0]) && (position[1]+1 == shortestPath[returnStart][1])){
		switch(direction){
			case 0:
				returnStart = returnStart - 1;
				return oneForward;
				break;
			case 1:
				return rotateLeft;
				break;
			case 2:
				return rotate180;
				break;
			case 3:
				return rotateRight;
				break;
		}
	} else if ((position[0]-1 == shortestPath[returnStart][0]) && (position[1] == shortestPath[returnStart][1])){
		switch(direction){
			case 0:
				return rotateLeft;
				break;
			case 1:
				return rotate180;
				break;
			case 2:
				return rotateRight;
				break;
			case 3:
				returnStart = returnStart - 1;
				return oneForward;
				break;
		}
	} else if ((position[0]+1 == shortestPath[returnStart][0]) && (position[1] == shortestPath[returnStart][1])){
		switch(direction){
			case 0:
				return rotateRight;
				break;
			case 1:
				returnStart = returnStart - 1;
				return oneForward;
				break;
			case 2:
				return rotateLeft;
				break;
			case 3:
				return rotate180;
				break;
		}
	} else {
		return falseCommand;
	}
}

void returnToStart()
{
	if(returnStart == -1){
		uint8_t temp = 0;
		while(1);
	}
	
	uint8_t * temp = findStart();
	lastCommand[0] = temp[0];
	lastCommand[1] = temp[1];
	lastCommand[2] = temp[2];
	//Master(3,SLA_styr_W,lastCommand);
	/*for (int k = 0; k < 3; k++)	{
	btSend(lastCommand[i]);
	}*/
	
	if((lastCommand[1] == right) || (lastCommand[1] == left) || (lastCommand[1] == oneEighty)) {
		newDirection(lastCommand[1],lastCommand[2]);
	} else if (lastCommand[1] != claw){
		newCoordinates();
	}
}