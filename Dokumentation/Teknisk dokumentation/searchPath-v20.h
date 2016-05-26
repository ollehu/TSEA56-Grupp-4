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
/*	hasFoundTarget - checks if target has been found

	Returns: 	0 	not found
				1	target in the module ahead
				2	target found 
																		*/
/************************************************************************/
uint8_t hasFoundTarget(void)
{	
	straightAhead = sensorData[10]*128 + sensorData[12];
	
	if((sensorData[16] == 1) && (straightAhead < oneModuleAhead)){
		return 1;
	} else if((target[0] != 0xFF) && (target[1] != 0xFF)){
		return 2;
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

		if (sensorData[2] < 245){
			map[position[0]+1][position[1]] = wallValue;
			walls[0] = 1;
		} 

		if (sensorData[4] < 245){
			map[position[0]-1][position[1]] = wallValue;
			walls[2] = 1;
		} 

	} else if(direction == 1) {
		//Left, straight ahead, right, !!west open!!
		if (sensorData[4] < 245){
			map[position[0]][position[1]+1] = wallValue;
			walls[2] = 1;
		

		if (straightAhead < oneModuleAhead){
			map[position[0]+1][position[1]] = wallValue;
			walls[1] = 1;
		} 

		if (sensorData[2] < 245){
			map[position[0]][position[1]-1] = wallValue;
			walls[0] = 1;
		} 

	} else if(direction == 2) {
		//!!north open!!, left, straight ahead, right
		if (sensorData[4] < 245){
			map[position[0]+1][position[1]] = wallValue;
			walls[2] = 1;
		} 

		if (straightAhead < oneModuleAhead){
			map[position[0]][position[1]-1] = wallValue;
			walls[1] = 1;
		} 

		if (sensorData[2] < 245){
			map[position[0]-1][position[1]] = wallValue;
			walls[0] = 1;
		} else if (countNrOfExploredPaths() <= 1){
			nrOfUnexploredPaths = nrOfUnexploredPaths + 1;
		}

	} else if (direction == 3){
		//Right, !!east open!!, left, straight ahead
		if (sensorData[2] < 245){
			map[position[0]][position[1]+1] = wallValue;
			walls[0] = 1;
		} 

		if (sensorData[4] < 245){
			map[position[0]][position[1]-1] = wallValue;
			walls[2] = 1;
		} 

		if (straightAhead < oneModuleAhead){
			map[position[0]-1][position[1]] = wallValue;
			walls[1] = 1;
		} 
	}

	//Send map
	if (position[0] + 1 < dimx){
		sendMapCoordinate(position[0] + 1, position[1]);
	}
	if (position[0] - 1 >= 0){
		sendMapCoordinate(position[0] - 1, position[1]);
	}
	if (position[1] + 1 < dimy){
		sendMapCoordinate(position[0], position[1] + 1);
	}
	if (position[0] - 1 >= 0){
		sendMapCoordinate(position[0], position[1] - 1);
	}
	
	
}
