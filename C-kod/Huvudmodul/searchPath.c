/*
* SearchPath.c
*
* Created: 4/20/2016 9:01:32 AM
*  Author: eletr654
*/

#include <avr/io.h>
#include <stdlib.h> //For use of abs()

uint8_t direction = 0;// 0: north, 1: east, 2: south, 3: west
uint8_t map[28][28]; //0: initial value, 1: wall, 2: visited, 3: target, 4: the starting point
uint8_t path[28][28]; //FF initial value, AA blocked way, >0 steps from start
uint8_t position[2] = {14,14};
uint8_t walls[3]; //[0] = Right side, [1]: forward, [2]: left side
uint8_t target[2] = {0xFF,0xFF}; //Coordinates for target
uint8_t z[2] = {0xFF,0xFF}; //Estimations of shortest path

uint8_t lastCommand[3]; //Last send controller command

//Controller commands
uint8_t oneForward[3] = {0xFF, 0x01, 0x01}; 
uint8_t rotateRight[3] = {0xFF, 0x03, 0x5A};
uint8_t rotateLeft[3] = {0xFF, 0x04, 0x5A};
uint8_t rotate180[3] = {0xFF, 0x03, 0xB4};
uint8_t falseCommand[3] = {0,0,0};

uint8_t hasFoundWayBack = 0;
uint8_t testWalls[][3] = {{1, 0, 1},{0, 1, 0},{1, 1, 0},{1, 0, 1},{1, 0, 1},{1, 0, 0},{0, 1, 1},{1, 1, 1},{0,0,1},{1,0,1},{0,0,0},{1,1,1},{0,0,0},{1,0,1},{0,1,1},{1,0,1},{1,0,0},{1,0,1},{0,0,0},{1,0,1}};

uint16_t straightAhead = 0;
uint16_t oneModuleAhead = 50;

uint8_t s;
uint8_t t;
uint8_t u;
uint8_t v;
uint8_t w;
uint8_t r;

uint8_t p = 0;
uint8_t dist = 0;
uint8_t val;

/*void readSensors() //In which directions are there walls? 
{
	walls[0] = 0;
	walls[1] = 0;
	walls[2] = 0;
	map[position[0]][position[1]] = 0;
	
	straightAhead = sensorData[10]*256 + sensorData[12];
	

	//Cardinal direction: north, east, south, west
	if(direction == 0){
		//Straight ahead, right, !!south open!!, left
		if (straightAhead < oneModuleAhead){
			map[position[0]][position[1]+1] = 1;
			walls[1] = 1;
		}

		if ((sensorData[2] < 245) && (sensorData[6] < 245)){
			map[position[0]+1][position[1]] = 1;
			walls[0] = 1;
		}

		if ((sensorData[4] < 245) && (sensorData[8] < 245)){
			map[position[0]-1][position[1]] = 1;
			walls[2] = 1;
		}

	} else if(direction == 1) {
		//Left, straight ahead, right, !!west open!!
		if ((sensorData[4] < 245) && (sensorData[8] < 245)){
			map[position[0]][position[1]+1] = 1;
			walls[2] = 1;
		}

		if (straightAhead < oneModuleAhead){
			map[position[0]+1][position[1]] = 1;
			walls[1] = 1;
		}

		if ((sensorData[2] < 245) && (sensorData[6] < 245)){
			map[position[0]][position[1]-1] = 1;
			walls[0] = 1;
		}
	} else if(direction == 2) {
		//!!north open!!, left, straight ahead, right
		if ((sensorData[4] < 245) && (sensorData[8] < 245)){
			map[position[0]+1][position[1]] = 1;
			walls[2] = 1;
		}

		if (straightAhead < oneModuleAhead){
			map[position[0]][position[1]-1] = 1;
			walls[1] = 1;
		}

		if ((sensorData[2] < 245) && (sensorData[6] < 245)){
			map[position[0]-1][position[1]] = 1;
			walls[0] = 1;
		}


	} else if (direction == 3){
		//Right, !!east open!!, left, straight ahead
		if ((sensorData[2] < 245) && (sensorData[6] < 245)){
			map[position[0]][position[1]+1] = 1;
			walls[0] = 1;
		}

		if ((sensorData[4] < 245) && (sensorData[8] < 245)){
			map[position[0]][position[1]-1] = 1;
			walls[2] = 1;
		}

		if (straightAhead < oneModuleAhead){
			map[position[0]-1][position[1]] = 1;
			walls[1] = 1;
		}
	}
}
*/
void redoPath()//Do we need this one...??? 
{
	switch(direction){
		case 0:
			if (path[position[0]][position[1]] - path[position[0]][position[1]+1] > 1){
				int i = 0;
				while((path[position[0]][position[1]-i] - path[position[0]][position[1]-i+1] > 1) && 
						(map[position[0]][position[1]-i] == 2)){
					path[position[0]][position[1]-i] = path[position[0]][position[1]-i+1];
					i = i + 1;
				}
			}
		case 1:
			if (path[position[0]][position[1]] - path[position[0]+1][position[1]] > 1){
				
			}
		case 2:
			if (path[position[0]][position[1]] - path[position[0]][position[1]-1] > 1){
				
			}
		case 3:
			if (path[position[0]][position[1]] - path[position[0]-1][position[1]] > 1){
				
			}
	}
}

uint8_t distanceToStart(uint8_t x, uint8_t y) //Returns the Manhattan distance from (x,y) to start
{
	return abs(x-14)+abs(y-14);
}

uint8_t distanceToTarget(uint8_t x, uint8_t y) //Returns the walking distance from (x,y) to target
{
	val = abs(path[x][y]-path[target[0]][target[1]]);
	return val;
}

void updateCoordinates() //Updates map, path and position variables
{
	//Cardinal direction: north, east, south, west
	if (direction == 0){
		//increase length from start by one
		if (path[position[0]][position[1]+1] == 0xFF){
			path[position[0]][position[1]+1] = path[position[0]][position[1]] + 1;
		}
		position[1] = position[1] + 1;
	} else if (direction == 1){
		//increase length from start by one
		if (path[position[0]+1][position[1]] == 0xFF){
			path[position[0]+1][position[1]] = path[position[0]][position[1]] + 1;
		}
		position[0] = position[0] + 1;
	} else if (direction == 2){
		//increase length from start by one
		if (path[position[0]][position[1]-1] == 0xFF){
			path[position[0]][position[1]-1] = path[position[0]][position[1]] + 1;
		}
		position[1] = position[1] - 1;
	} else {
		//increase length from start by one
		if (path[position[0]-1][position[1]] == 0xFF){
			path[position[0]-1][position[1]] = path[position[0]][position[1]] + 1;
		}
		position[0] = position[0] - 1;
	}
	//update current module to visited
	map[position[0]][position[1]] = 2;
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

uint8_t unexploredPaths() //Are there any unexplored paths from current position?
{
	if(map[position[0]+1][position[1]] == 0){
		return 1;
	} else if(map[position[0]-1][position[1]] == 0) {
		return 1;
	} else if (map[position[0]][position[1]+1] == 0){
		return 1;
	} else if (map[position[0]][position[1]-1] == 0){
		return 1;
	} else {
		return 0;
	}
}

uint8_t * findWayBack(uint8_t distance) //Where to go to get closer to the start 
{
	dist = distance;
	
	if(path[position[0]+1][position[1]] == path[position[0]][position[1]] - distance){
		switch (direction){// 0: north, 1: east, 2: south, 3: west
			case 0: 
				//Rotate right, one forward
				return rotateRight;
				break;
			case 1:	
				//One forward, update position with new coordinates
				return oneForward;
				break;
			case 2:
				//Rotate left, one forward
				return rotateLeft;
				break;
			case 3:
				//Should never happen??
				//Rotate 180 degrees, one forward
				return rotate180;
				break;
		}
	} else if(path[position[0]][position[1]+1] == path[position[0]][position[1]] - distance){
		switch (direction){// 0: north, 1: east, 2: south, 3: west
			case 0: 
				//One forward
				return oneForward;
				break;
			case 1:
				//Rotate left, one forward
				return rotateLeft;
				break;
			case 2:
				//Should never happen??
				//Rotate 180 degrees, one forward
				return rotate180;
				break;
			case 3: 
				//Rotate right, one forward
				return rotateRight;
				break;
		}
	} else if(path[position[0]-1][position[1]] == path[position[0]][position[1]] - distance){
		switch (direction){// 0: north, 1: east, 2: south, 3: west
			case 0:
				//Rotate left, one forward
				return rotateLeft;
				break;
			case 1:
				//Should never happen??
				//Rotate 180 degrees, one forward
				return rotate180;
				break;
			case 2:
				//Rotate right, one forward
				return rotateRight;
				break;
			case 3:
				//One forward
				return oneForward;
				break;
			}
	} else if(path[position[0]][position[1]-1] == path[position[0]][position[1]] - distance){
		switch (direction){// 0: north, 1: east, 2: south, 3: west
			case 0:
				//Should never happen??
				//Rotate 180 degrees, one forward
				return rotate180;
				break;
			case 1:	
				//Rotate right, one forward
				return rotateRight;
				break;
			case 2:
				//One forward
				return oneForward;
				break;
			case 3:
				//Rotate left, one forward
				return rotateLeft;
				break;
		}
	}
	
	return falseCommand;
	
}

void searchPathInit() //Fills map and path, sets initial values 
{
	//Fill path with 255, map with 0
	for(int i = 0;i<28;i++){
		for(int j = 0;j<28;j++){
			path[i][j] = 0xFF;
			map[i][j] = 0;
		}
	}
	
	//Set start value for path and map
	path[14][14] = 0;
	map[14][14] = 3;
}

uint8_t hasFoundTarget() // Checks if target is found NEEDS IMPLEMENTATION!!!
{
	//Göra skillnad på tre tillstånd: Målet ej hittat, ser målet, vet precis vilken kartmodul målet befinner sig i???
	/*if((target[0] != 0xFF) && (target[1] != 0xFF)){
		//
	} else {
		//Check sensors to see if target is found
	}*/
	
	if((position[0] == 11) && (position[1] == 20) && (direction == 0)){
		return 1;
	} else if((target[0] != 0xFF) && (target[1] != 0xFF)){
		return 2;
	} else {
		return 0;
	}
}

void updateTargetFound()//Updates map, path and position variables when target is found
{
	switch(direction){
		case 0:
			if (path[position[0]][position[1]+1] > path[position[0]][position[1]] + 1){
				path[position[0]][position[1]+1] = path[position[0]][position[1]] + 1;
			}
			map[position[0]][position[1]+1] = 3;
			target[0] = position[0];
			target[1] = position[1]+1;
			break;
		case 1:
			if (path[position[0]+1][position[1]] > path[position[0]][position[1]] + 1){
				path[position[0]+1][position[1]] = path[position[0]][position[1]] + 1;
			}
			map[position[0]+1][position[1]] = 3;
			target[0] = position[0]+1;
			target[1] = position[1];
			break;
		case 2:
			if (path[position[0]][position[1]-1] > path[position[0]][position[1]] + 1){
				path[position[0]][position[1]-1] = path[position[0]][position[1]] + 1;
			}
			map[position[0]][position[1]-1] = 3;
			target[0] = position[0];
			target[1] = position[1]-1;
			break;
		case 3:
			if (path[position[0]-1][position[1]] > path[position[0]][position[1]] + 1){
				path[position[0]-1][position[1]] = path[position[0]][position[1]] + 1;
			}
			map[position[0]-1][position[1]] = 3;
			target[0] = position[0]-1;
			target[1] = position[1];
			break;
		}
	
	path[position[0]][position[1]] = path[position[0]][position[1]] + 2;
	z[0] = distanceToStart(target[0],target[1]); //Optimistic value
	z[1] = path[target[0]][target[1]]; //Pessimistic value
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
		
			if((map[position[0]+1][position[1]] == 0) && ( w + r < z[1])) { //(map[position[0]+1][position[1]] == 0)
				return rotateRight;
			} else if((map[position[0]][position[1]+1] == 0) && ( u + v < z[1])){ //(map[position[0]-1][position[1]] == 0)
				return oneForward;
			} else if((map[position[0]-1][position[1]] == 0) && ( s + t < z[1])){ //(map[position[0]-1][position[1]] == 0)
				return rotateLeft;
			} else {
				return rotate180;
			}
		case 1:
			w = distanceToStart(position[0],position[1]-1);
			r = distanceToTarget(position[0],position[1])+1;
			
			u = distanceToStart(position[0]+1,position[1]);
			v = distanceToTarget(position[0],position[1])+1;
			
			s = distanceToStart(position[0],position[1]+1);
			t = distanceToTarget(position[0],position[1])+1;
			
			if((map[position[0]][position[1]-1] == 0) && (w + r < z[1])){ //(map[position[0]][position[1]-1] == 0)
				return rotateRight;
			} else if((map[position[0]+1][position[1]] == 0) && (u + v < z[1])){ //(map[position[0]][position[1]+1] == 0)
				return oneForward;
			} else if((map[position[0]][position[1]+1] == 0) && (s + t < z[1])){ //(map[position[0]][position[1]+1] == 0)
				return rotateLeft;
			} else {
				return rotate180;
			}
		case 2:
			w = distanceToStart(position[0]-1,position[1]);
			r = distanceToTarget(position[0],position[1])+1;
			
			u = distanceToStart(position[0],position[1]-1);
			v = distanceToTarget(position[0],position[1])+1;
			
			s = distanceToStart(position[0]+1,position[1]);
			t = distanceToTarget(position[0],position[1])+1;
			
			if((map[position[0]-1][position[1]] == 0) && (w + r < z[1])){ //(map[position[0]-1][position[1]] == 0)
				return rotateRight;
			} else if((map[position[0]][position[1]-1] == 0) && (u + v < z[1])){ //(map[position[0]+1][position[1]] == 0)
				return oneForward;
			} else if((map[position[0]+1][position[1]] == 0) && (s + t < z[1])){ //(map[position[0]+1][position[1]] == 0)
				return rotateLeft;
			} else {
				return rotate180;
			}
		case 3:
			w = distanceToStart(position[0],position[1]+1);
			r = distanceToTarget(position[0],position[1])+1;
			
			u = distanceToStart(position[0]-1,position[1]);
			v = distanceToTarget(position[0],position[1])+1;
			
			s = distanceToStart(position[0],position[1]-1);
			t = distanceToTarget(position[0],position[1])+1;
			
			if((map[position[0]][position[1]+1] == 0) && (w + r < z[1])){ //(map[position[0]][position[1]+1] == 0)
				return rotateRight;
			} else if((map[position[0]-1][position[1]] == 0) && (u + v < z[1])){ //(map[position[0]][position[1]-1] == 0)
				return oneForward;
			} else if((map[position[0]][position[1]-1] == 0) && (s + t < z[1])){ //(map[position[0]][position[1]-1] == 0)
				return rotateLeft;
			} else {
				return rotate180;
			}
	}
	return falseCommand;
	
}

void ruleOutPath() //Rules out map modules for easier calculation of shortest path
{
	switch(direction){
		case 0:
		path[position[0]][position[1]-1] = 0xAA;
		break;
		case 1:
		path[position[0]-1][position[1]] = 0xAA;
		break;
		case 2:
		path[position[0]][position[1]+1] = 0xAA;
		break;
		case 3:
		path[position[0]+1][position[1]] = 0xAA;
		break;
	}
}

void tempUpdateWalls()
{
	switch(direction){
		case 0:
			if(walls[0] == 1){
				map[position[0]+1][position[1]] = 1;
			} 
			
			if(walls[1] == 1){
				map[position[0]][position[1]+1] = 1;
			} 
			
			if (walls[2] == 1){
				map[position[0]-1][position[1]] = 1;
			}
			break;
		case 1:
			if(walls[0] == 1){
				map[position[0]][position[1]-1] = 1;
			} 
			
			if(walls[1] == 1){
				map[position[0]+1][position[1]] = 1;
			} 
			
			if (walls[2] == 1){
				map[position[0]][position[1]+1] = 1;
			}
			break;
		case 2:
			if(walls[0] == 1){
				map[position[0]-1][position[1]] = 1;
			} 
			
			if(walls[1] == 1){
				map[position[0]][position[1]-1] = 1;
			} 
			
			if (walls[2] == 1){
				map[position[0]+1][position[1]] = 1;
			}
			break;
		case 3:
			if(walls[0] == 1){
				map[position[0]][position[1]+1] = 1;
			} 
			
			if(walls[1] == 1){
				map[position[0]-1][position[1]] = 1;
			} 
			
			if (walls[2] == 1){
				map[position[0]][position[1]-1] = 1;
			}
			
			break;
	}
}

void updateCoordinatesTargetFound()
{
	//Cardinal direction: north, east, south, west
	if (direction == 0){
		path[position[0]][position[1]+1] = path[position[0]][position[1]] + 1;
		position[1] = position[1] + 1;
	} else if (direction == 1){
		//increase length from start by one
		path[position[0]+1][position[1]] = path[position[0]][position[1]] + 1;
		position[0] = position[0] + 1;
	} else if (direction == 2){
		//increase length from start by one
		path[position[0]][position[1]-1] = path[position[0]][position[1]] + 1;
		position[1] = position[1] - 1;
	} else {
		//increase length from start by one
		path[position[0]-1][position[1]] = path[position[0]][position[1]] + 1;
		position[0] = position[0] - 1;
	}
}

int main(void)
{
	searchPathInit();
	
	int i = 0;
	
	while(1){
		if(hasFoundTarget() == 1){ 
			//Update map with the right number and target with the right 
			updateTargetFound(); //Only just when the target has been found
			walls[0] = testWalls[i][0];
			walls[1] = testWalls[i][1];
			walls[2] = testWalls[i][2];
			i++;
			tempUpdateWalls();
			
			//Continue to explore
			///////////////////////////////////////////////////////////
			/*uint8_t * temp = exploreTargetFound();
			lastCommand[0] = temp[0];
			lastCommand[1] = temp[1];
			lastCommand[2] = temp[2];*/	
			//////////////////////////////////////////////////////
			//Master(3,...,lastCommand);
		} 
		
		if((lastCommand[1] == 0x03) || (lastCommand[1] == 0x04)) {
			//straightAhead = sensorData[10]*256 + sensorData[12];
			lastCommand[1] = 0x01;
			
			//Master(3,...,lastCommand);
		} else if (unexploredPaths()) {
			
			if((hasFoundWayBack) || ((lastCommand[1] == 0x01) && (lastCommand[2] == 0xB4))){
				hasFoundWayBack = 0;
				ruleOutPath();
			}			
		
			//readSensors();
			walls[0] = testWalls[i][0];
			walls[1] = testWalls[i][1];
			walls[2] = testWalls[i][2];
			i++;
			tempUpdateWalls();
		
			uint8_t * temp;
			if(hasFoundTarget()){
				temp = exploreTargetFound();
			} else {
				temp = findTarget();
			}
			lastCommand[0] = temp[0];
			lastCommand[1] = temp[1];
			lastCommand[2] = temp[2];
			if(lastCommand[1] != 0){
				//Master(3,...,lastCommand);
			}
		} else {
			uint8_t * temp;

			///////////////////////////////////////////////
			hasFoundWayBack = 1;
			if (hasFoundTarget()){
				p = distanceToTarget(position[0],position[1]);
				p = p*2;
				p = p+1;
				temp = findWayBack(p);
			} else {
				temp = findWayBack(0x01);
			}
			
			lastCommand[0] = temp[0];
			lastCommand[1] = temp[1];
			lastCommand[2] = temp[2];
			
			
			
			///////////////////////////////////////////////
			if(lastCommand[1] != 0){
				//Master(3,...,lastCommand);
			}
		}
	
		if((lastCommand[1] == 0x03) || (lastCommand[1] == 0x04)) {
			newDirection(lastCommand[1],lastCommand[2]);
		} else if (hasFoundTarget()) {
			updateCoordinatesTargetFound();
		} else {
			updateCoordinates();
		}
	}
}