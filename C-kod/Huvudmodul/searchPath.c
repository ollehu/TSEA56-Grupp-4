/*
* Avsökningsalgoritm.c
*
* Created: 4/15/2016 8:17:32 AM
*  Author: eletr654
*/


#include <avr/io.h>

int direction = 0;// 0: north, 1: east, 2: south, 3: west
int map[28][28] = {{0}};
int path[28][28] = {{0xFF}}};
int position[2] = {14,14};
int walls[3];

void readSensors() //In which directions are there walls?
{
	walls[3] = {0, 0, 0};
	map[position[0]][position[1]] = 0;
	
	//Cardinal direction: north, east, south, west
	if(direction == 0){
		//Straight ahead, right, !!south!!, left		
		if (sensorData[5] ger vägg){
			map[position[0]][position[1]+1] = 1;
			walls[1] = 1;
		}
		
		if (Vägg åt höger){
			map[position[0]+1][position[1]] = 1;
			walls[0] = 1;
		}
		
		if (Vägg åt vänster){
			map[position[0]-1][position[1]] = 1;
			walls[2] = 1;
		}
		
	} else if(direction == 1) {
		//Left, straight ahead, right, !!west!!
		if (Vägg åt vänster){
			map[position[0]][position[1]+1] = 1;
			walls[2] = 1;
		}
		
		if (sensorData[5] ger vägg){
			map[position[0]+1][position[1]] = 1;
			walls[1] = 1;
		}
		
		if (Vägg åt höger){
			map[position[0]][position[1]-1] = 1;
			walls[0] = 1;
		}		
	} else if(direction == 2) {
		//!!north!!, left, straight ahead, right
		if (Vägg åt vänster){
			map[position[0]+1][position[1]] = 1;
			walls[2] = 1;
		}
		
		if (sensorData[5] ger vägg){
			map[position[0]][position[1]-1] = 1;
			walls[1] = 1;
		}
		
		if (Vägg åt höger){
			map[position[0]-1][position[1]] = 1;
			walls[0];
		}
		
		
	} else if (direction == 3){
		//Right, !!east!!, left, straight ahead
		if (Vägg åt höger){
			map[position[0]][position[1]+1] = 1;
			walls[0] = 1;
		}
		
		if (Vägg åt vänster){
			map[position[0]][position[1]-1] = 1;
			walls[2] = 1; 
		}
		
		if (sensorData[5] ger vägg){
			map[position[0]-1][position[1]] = 1;
			walls[1] = 1;
		}
	}
}

void updateCoordinates()
{
	if (direction == 0){
		if (path[position[0]][position[1]+1] == 0xFF){
			path[position[0]][position[1]+1] = path[position[0]][position[1]] + 1;
		}
		position[1] = position[1] + 1;
	} else if (direction == 1){
		if (path[position[0]+1][position[1]] == 0xFF){
			path[position[0]+1][position[1]] = path[position[0]][position[1]] + 1;
		}
		position[0] = position[0] + 1;
	} else if (direction == 2){
		if (path[position[0]][position[1]-1] == 0xFF){
			path[position[0]][position[1]-1] = path[position[0]][position[1]] + 1;
		}
		position[1] = position[1] - 1;
	} else {
		if (path[position[0]-1][position[1]] == 0xFF){
			path[position[0]-1][position[1]] = path[position[0]][position[1]] + 1;
		}
		position[0] = position[0] - 1;
	}
	
	map[position[0]][position[1]] = 2;
}

void chooseDirection()
{
	if(walls[0] = 0){
		//Rotera höger och uppdatera vilken riktning roboten nu kör i
		Master(2,....,{0x00,0x03});
		newDirection(direction, 0x03);
		
		//Kör framåt, uppdatera relevant koordinat
		Master(2,....,{0x00,0x01});
		updateCoordinates();
		
	} else if(walls[1] = 0){
		//Kör framåt, uppdatera relevant koordinat
		Master(2,....,{0x00,0x01});
		updateCoordinates();
	} else if (walls[2] = 0){
		//Rotera vänster och uppdatera vilken riktning roboten nu kör i
		Master(2,....,{0x00,0x04});
		newDirection(direction,0x04);
		
		//Kör framåt, uppdatera relevant koordinat
		Master(2,....,{0x00,0x01});
		updateCoordinates();
	} else {
		//Rotera 180 grader och uppdatera vilken riktning roboten nu kör i
		Master(2,....,{0x00,0x04});
		newDirection(direction,0x04);
		Master(2,....,{0x00,0x04});
		newDirection(direction,0x04);
		//Kör framåt, uppdatera relevant koordinat
		Master(2,....,{0x00,0x01});
		updateCoordinates();
	}
}

void newDirection(int direction, int rotation){
	if(((direction == 0) & (rotation == 0x03)) | ((direction == 2) & (rotation == 0x04))){
		direction = 1; //east
	} else if (((direction == 0) & (rotation == 0x04)) | ((direction == 2) & (rotation == 0x03))){
		direction = 3; //west
	} else if(((direction == 1) & (rotation == 0x03)) | ((direction == 3) & (rotation == 0x04))){
		direction = 2; //south
	} else {
		direction = 0; //north
	}
}

int main(void)
{
	readSensors(); //Sensor values to compare with?
	chooseDirection();
}