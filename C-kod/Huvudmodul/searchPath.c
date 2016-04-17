/*
* Avsökningsalgoritm.c
*
* Created: 4/15/2016 8:17:32 AM
*  Author: eletr654
*/


#include <avr/io.h>

int direction = 0;// 0: norrut, 1: österut, 2: söderut, 3: västerut
int map[28][28] = {{0}};
int position[2] = {14,14};
int walls[3];

void readSensors()
{
	walls[3] = {0, 0, 0};
	map[position[0]][position[1]] = 0;
	
	//Väderstreck: norrut, österut, söderut, västerut
	if(direction == 0){
		//Rakt fram, höger, !!öppet söderut!!, vänster		
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
		//Vänster, rakt fram, höger, !!öppet västerut!!
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
		//!!öppet norrut!!, vänster, rakt fram, höger
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
		//Höger, !!öppet österut!!, vänster, rakt fram
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
		position[1] = position[1] + 1;
	} else if (direction == 1){
		position[0] = position[0] + 1;
	} else if (direction == 2){
		position[1] = position[1] - 1;
	} else {
		position[0] = position[0] - 1;
	}
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
		direction = 1; //österut
	} else if (((direction == 0) & (rotation == 0x04)) | ((direction == 2) & (rotation == 0x03))){
		direction = 3; //västerut
	} else if(((direction == 1) & (rotation == 0x03)) | ((direction == 3) & (rotation == 0x04))){
		direction = 2; //söderut
	} else {
		direction = 0; //norrut
	}
}

int main(void)
{
	readSensors(); //Måste fixas
	chooseDirection();
}