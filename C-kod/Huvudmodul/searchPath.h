/*
 * searchPath.h
 *
 * Created: 4/28/2016 1:18:29 PM
 *  Author: ollul666
 */ 

#include <stdlib.h>

//TWI
int SLA_styr_W = 0xCC;

//Search path
uint8_t direction = 0;// 0: north, 1: east, 2: south, 3: west
uint8_t map[28][28]; //F5: initial value, F4: wall, F3: target, F2: start, [0,225] steps from start
uint8_t path[28][28]; //FF initial value, F1 blocked way, >0 steps from target
uint8_t position[2] = {14,14};
uint8_t walls[3]; //[0] = Right side, [1]: forward, [2]: left side
uint8_t target[3] = {0xFF,0xFF,0xFF}; //Coordinates for target plus value in path
uint8_t z[2] = {0xFF,0xFF}; //Estimations of shortest path
uint8_t currentCommand[3];
uint8_t doneShortestPathInit = 0;

uint8_t lastCommand[3]; //Last send controller command

//Controller commands
uint8_t oneForward[3] = {0xFF, 0x01, 0x01};
uint8_t rotateRight[3] = {0xFF, 0x03, 0x5A};
uint8_t rotateLeft[3] = {0xFF, 0x04, 0x5A};
uint8_t rotate180[3] = {0xFF, 0x02, 0xB4};
uint8_t falseCommand[3] = {0,0,0};

uint8_t hasFoundWayBack = 0;
//uint8_t testWalls[][3] = {{1, 0, 1},{0, 1, 0},{1, 1, 0},{1, 0, 1},{1, 0, 1},{1, 0, 0},{0, 1, 1},{1, 1, 1},{0,0,1},{1,0,1},{0,0,0},{1,1,1},{0,0,0},{1,0,1},{0,1,1},{1,0,1},{1,0,0},{1,0,1},{0,0,0},{1,0,1},{0,0,1},{1,1,0}};
//uint8_t testWalls[][3] = {{1,0,1},{1,0,1},{0,1,0},{1,0,0},{1,1,1},{0,0,1},{1,0,1},{1,0,1},{1,1,0},{1,0,1},{1,0,1},{1,0,1},{0,0,0},{1,0,1},{0,0,0},{1,0,1},{1,0,1},{0,0,0},{1,0,1},{0,1,1},{0,0,0},{1,0,1},{1,0,1}};
//uint8_t testWalls[][3] = {{0,0,0},{1,0,1},{1,1,0},{1,0,1},{1,0,1},{1,0,1},{1,0,1},{1,0,0},{1,0,1},{1,1,1},{0,0,1},{1,0,1},{1,0,0},{1,0,1},{1,0,0},{0,0,1},{1,0,1},{0,1,0},{1,0,1},{0,0,1},{1,0,1},{1,0,1},{0,1,1},{1,0,0},{1,0,1},{0,1,0},{1,0,1},{1,0,1},{0,0,1},{1,0,1},{1,0,1},{1,0,1},{1,0,1}};
uint8_t testWalls[][3] = {{1,0,0},{1,0,1},{1,1,0},{1,0,1},{1,1,0},{1,0,1},{1,1,0},{1,0,1}};
uint16_t straightAhead = 0;
uint16_t oldStraighAhead = 0;
uint16_t oneModuleAhead = 24;

uint8_t s;
uint8_t t;
uint8_t u;
uint8_t v;
uint8_t w;
uint8_t r;

uint8_t dist;
uint8_t wallValue = 0xF4;
uint8_t	targetValue = 0xF3;
uint8_t	initialValue = 0xF5;
uint8_t blockedWayValue = 0xF1;
uint8_t startPositionValue = 0xF2;
uint8_t	targetPathValue = 0;

int i = 0;
int nrOfCoordinates = 0;

//////////////////////////////////////////////////////////////////////////
uint8_t keepExploring = 1;
//////////////////////////////////////////////////////////////////////////

void sendMapCoordinate(uint8_t x, uint8_t y);

void btSend(unsigned char data)
{
	// Wait for empty transmit buffer
	while ( !( UCSR0A & (1<<UDRE0)) );
	// Put data into buffer, sends the data
	UDR0 = data;
}

uint8_t * findTarget() //Search for target
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


void ruleOutPath() //Rules out map modules for easier calculation of shortest path
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


uint8_t hasFoundTarget(void) // Checks if target is found NEEDS IMPLEMENTATION!!!
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

uint8_t unexploredPaths() //Are there any unexplored paths from current position?
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

void readSensors() //In which directions are there walls? 
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

void sendMapCoordinate(uint8_t x, uint8_t y){
	btSend(0xFE);
	btSend(x);
	btSend(y);
	btSend(map[x][y]);
}

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
	map[14][14] = startPositionValue;
}


void newDirection(uint8_t rotation, uint8_t degrees)//Updates direction
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

uint8_t deadEndTarget()
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

void tempUpdateWalls()
{
	switch(direction){
		case 0:
			if(walls[0] == 1){
				map[position[0]+1][position[1]] = wallValue;
			} 
			
			if(walls[1] == 1){
				map[position[0]][position[1]+1] = wallValue;
			} 
			
			if (walls[2] == 1){
				map[position[0]-1][position[1]] = wallValue;
			}
			break;
		case 1:
			if(walls[0] == 1){
				map[position[0]][position[1]-1] = wallValue;
			} 
			
			if(walls[1] == 1){
				map[position[0]+1][position[1]] = wallValue;
			} 
			
			if (walls[2] == 1){
				map[position[0]][position[1]+1] = wallValue;
			}
			break;
		case 2:
			if(walls[0] == 1){
				map[position[0]-1][position[1]] = wallValue;
			} 
			
			if(walls[1] == 1){
				map[position[0]][position[1]-1] = wallValue;
			} 
			
			if (walls[2] == 1){
				map[position[0]+1][position[1]] = wallValue;
			}
			break;
		case 3:
			if(walls[0] == 1){
				map[position[0]][position[1]+1] = wallValue;
			} 
			
			if(walls[1] == 1){
				map[position[0]-1][position[1]] = wallValue;
			} 
			
			if (walls[2] == 1){
				map[position[0]][position[1]-1] = wallValue;
			}
			
			break;
	}
}

void updateCoordinates() //Updates map, path and position variables
{
	//Cardinal direction: north, east, south, west
	if (direction == 0){
		//increase length from start by one
		if (map[position[0]][position[1]+1] == initialValue){
			if (map[position[0]][position[1]] == startPositionValue){
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
			if (map[position[0]][position[1]] == startPositionValue){
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
			if (map[position[0]][position[1]] == startPositionValue){
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
			if (map[position[0]][position[1]] == startPositionValue){
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


uint8_t * findWayBack() //Where to go to get closer to the start
{
	if(map[position[0]+1][position[1]] == map[position[0]][position[1]] - 1){
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
		} else if(map[position[0]][position[1]+1] == map[position[0]][position[1]] - 1){
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
		} else if(map[position[0]-1][position[1]] == map[position[0]][position[1]] - 1){
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
		} else if(map[position[0]][position[1]-1] == map[position[0]][position[1]] - 1){
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


void explore(void)
{
	if (keepExploring == 1){
	
		if(hasFoundTarget() == 1){ 
			//Update map with the right number and target with the right 
			//updateTargetFound(); //Only just when the target has been found			
		} 
		
		if((lastCommand[1] == 0x03) || (lastCommand[1] == 0x04) || (lastCommand[1] == 0x02)) {
				//straightAhead = sensorData[10]*256 + sensorData[12];

			lastCommand[1] = 0x01;
			
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
		
		
		if((lastCommand[1] == 0x03) || (lastCommand[1] == 0x04) || (lastCommand[1] == 0x02)) {
			newDirection(lastCommand[1],lastCommand[2]);
			uint8_t deadEnd = deadEndTarget();
			if((lastCommand[2] == 0xB4) && deadEnd == 0){
				hasFoundWayBack = 1;
			}
		} else {
			updateCoordinates();

		}
	}
}
