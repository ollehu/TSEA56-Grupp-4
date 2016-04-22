/*
* SearchPath.c
*
* Created: 4/20/2016 9:01:32 AM
*  Author: eletr654
*/

#include <avr/io.h>
#include <stdlib.h> //For use of abs()

int direction = 0;// 0: north, 1: east, 2: south, 3: west
int map[28][28]; //0: initial value, 1: wall, 2: visited, 3: target, 4: the starting point
int path[28][28]; //FF initial value, AA blocked way, >0 steps from start
int position[2] = {14,14};
int walls[3]; //[0] = Right side, [1]: forward, [2]: right
int target[2] = {0xFF,0xFF}; //Coordinates for target
int z[2] = {0xFF,0xFF}; //Estimations of shortest path

int lastCommand[3]; //Last send controller command

//Controller commands
int oneForward[3] = {0xFF, 0x01, 0x01}; 
int rotateRight[3] = {0xFF, 0x03, 0x5A};
int rotateLeft[3] = {0xFF, 0x04, 0x5A};
int rotate180[3] = {0xFF, 0x03, 0xB4};
int falseCommand[3] = {0,0,0};

uint8_t hasFoundWayBack = 0;
int testWalls[][3] = {{1, 0, 1},{0, 1, 0},{1, 1, 0},{1, 0, 1},{1, 0, 1},{1, 0, 0},{0, 1, 1},{1, 1, 1},{0,0,1},{1,0,1},{0,0,0},{1,1,1},{0,0,0},{1,0,1},{0,1,1},{1,0,1},{1,0,0},{1,1,1},{0,0,1}};



/*void readSensors() //In which directions are there walls? 
{
	walls[0] = 0;
	walls[1] = 0;
	walls[2] = 0;
	map[position[0]][position[1]] = 0;

	//Cardinal direction: north, east, south, west
	if(direction == 0){
		//Straight ahead, right, !!south open!!, left
		if (sensorData[5] < 245){
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

		if (sensorData[5] < 245){
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

		if (sensorData[5] < 245){
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

		if (sensorData[5] < 245){
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

int distanceToStart(int x, int y) //Returns the Manhattan distance from (x,y) to start
{
	return abs(x-14)+abs(y-14);
}

int distanceToTarget(int x, int y) //Returns the walking distance from (x,y) to target
{
	return path[x][y]-path[target[0]][target[1]];
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

void newDirection(int rotation, int degrees)//Updates direction
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

int * findTarget() //Search for target
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

int unexploredPaths() //Are there any unexplored paths from current position?
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

int * findWayBack() //Where to go to get closer to the start 
{
	if(path[position[0]+1][position[1]] == path[position[0]][position[1]] - 1){
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
	} else if(path[position[0]][position[1]+1] == path[position[0]][position[1]] - 1){
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
	} else if(path[position[0]-1][position[1]] == path[position[0]][position[1]] - 1){
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
	} else if(path[position[0]][position[1]-1] == path[position[0]][position[1]] - 1){
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

int hasFoundTarget() // Checks if target is found NEEDS IMPLEMENTATION!!!
{
	//Göra skillnad på tre tillstånd: Målet ej hittat, ser målet, vet precis vilken kartmodul målet befinner sig i???
	/*if((target[0] != 0xFF) && (target[1] != 0xFF)){
		//
	} else {
		//Check sensors to see if target is found
	}*/
	return 0;
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
	
	z[0] = distanceToStart(target[0],target[1]); //Optimistic value
	z[1] = path[target[0]][target[1]]; //Pessimistic value
}

int * exploreTargetFound() //Decides where to go when target is found
{
	switch(direction){
		case 0:
			if((map[position[0]+1][position[1]] == 0) && (distanceToStart(position[0]+1,position[1]) + distanceToTarget(position[0]+1,position[1]) < z[1])) {
				return rotateRight;
			} else if((map[position[0]-1][position[1]] == 0) && (distanceToStart(position[0]+1,position[1]) + distanceToTarget(position[0]+1,position[1])< z[1])){
				return rotateLeft;
			} else {
				return rotate180;
			}
		case 1:
			if((map[position[0]][position[1]-1] == 0) && (distanceToStart(position[0],position[1]-1) + distanceToTarget(position[0],position[1]-1)< z[1])){
				return rotateRight;
			} else if((map[position[0]][position[1]+1] == 0) && (distanceToStart(position[0],position[1]+1) + distanceToTarget(position[0],position[1]+1) < z[1])){
				return rotateLeft;
			} else {
				return rotate180;
			}
		case 2:
			if((map[position[0]-1][position[1]] == 0) && (distanceToStart(position[0]-1,position[1]) + distanceToTarget(position[0]-1,position[1]) < z[1])){
				return rotateRight;
			} else if((map[position[0]+1][position[1]] == 0) && (distanceToStart(position[0]+1,position[1]) + distanceToTarget(position[0]+1,position[1])< z[1])){
				return rotateLeft;
			} else {
				return rotate180;
			}
		case 3:
			if((map[position[0]][position[1]+1] == 0) && (distanceToStart(position[0],position[1]+1) + distanceToTarget(position[0],position[1]+1) < z[1])){
				return rotateRight;
			} else if((map[position[0]][position[1]-1] == 0) && (distanceToStart(position[0],position[1]-1) + distanceToTarget(position[0],position[1]-1) < z[1])){ 
				return rotateLeft;
			} else {
				return rotate180;
			}
	}
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

int main(void)
{
	searchPathInit();
	
	int i = 0;
	
	while(1){
		if(hasFoundTarget()){
			//Update map with the right number and target with the right coordinates
			updateTargetFound();

			//Continue to explore
			///////////////////////////////////////////////////////////
			//lastCommand = exploreTargetFound();		
			//////////////////////////////////////////////////////
			//Master(3,...,lastCommand);
		} else if((lastCommand[1] == 0x03) || (lastCommand[1] == 0x04)) {
			lastCommand[1] = 0x01;
			//Master(3,...,lastCommand);
		} else if (unexploredPaths()) {
			
			if(hasFoundWayBack || ((lastCommand[1] == 0x01) && (lastCommand[2] == 0xB4))){
				hasFoundWayBack = 0;
				ruleOutPath();
			}			
			
			/////////////////////////////////////////////
			//readSensors();
			walls[0] = testWalls[i][0];
			walls[1] = testWalls[i][1];
			walls[2] = testWalls[i][2];
			i++;
			tempUpdateWalls();
			/////////////////////////////////////////////
			int * temp = findTarget();
			lastCommand[0] = temp[0];
			lastCommand[1] = temp[1];
			lastCommand[2] = temp[2];
			if(lastCommand[1] != 0){
				//Master(3,...,lastCommand);
			}
		} else {
			///////////////////////////////////////////////
			hasFoundWayBack = 1;
			int * temp = findWayBack();
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
		} else {
			updateCoordinates();
		}
	}
}