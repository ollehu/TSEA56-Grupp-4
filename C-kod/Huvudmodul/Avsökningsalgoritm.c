/*
* Avsökningsalgoritm.c
*
* Created: 4/15/2016 8:17:32 AM
*  Author: eletr654
*/


#include <avr/io.h>

int direction = 0;// 0: norrut, 1: österut, 2: söderut, 3: västerut


void readSensors()
{
	//Väderstreck: norrut, österut, söderut, västerut
	if(direction norrut){
		//Rakt fram, höger, !!öppet söderut!!, vänster
	} else if(direction österut) {
		//Vänster, rakt fram, höger, !!öppet västerut!!
	} else if(direction söderut) {
		//!!öppet norrut!!, vänster, rakt fram, höger
	} else (direction västerut){
		//Höger, !!öppet österut!!, vänster, rakt fram
	}
}

void updateCoordinates()
{
	assert(!"The method or operation is not implemented.");
}

void chooseDirection()
{
	if((0 & ((walls & 0x30) != 0x30)) | (1 & ((walls & 0x0C) != 0x0C)) |
			(2 & ((walls & 0x03) != 0x03)) | (3 & ((walls & 0xC0) != 0xC0))){
		//Rotera höger och uppdatera vilken riktning roboten nu kör i
		Master(2,....,{0x00,0x03})
		updateDirection(direction, 0x03);
		
		//Kör framåt, uppdatera relevant koordinat
		Master(2,....,{0x00,0x01});
		updateCoordinates(); //Måste fixas!!!
		
	} else if((0 & ((walls & 0xC0) != 0xC0)) | (1 & ((walls & 0x30) != 0x30)) |
			(2 & ((walls & 0x0C) != 0x0C)) | (3 & ((walls & 0x03) != 0x03))){
		//Kör framåt, uppdatera relevant koordinat
		Master(2,....,{0x00,0x01});
		updateCoordinates();
	} else if ((0 & ((walls & 0x03) != 0x03)) | (1 & ((walls & 0xC0) != 0xC0)) |
			(2 & ((walls & 0x30) != 0x30)) | (3 & ((walls & 0x0C) != 0x0C))){
		//Rotera vänster och uppdatera vilken riktning roboten nu kör i
		Master(2,....,{0x00,0x04});
		updateDirection(direction,0x04);
		
		//Kör framåt, uppdatera relevant koordinat
		Master(2,....,{0x00,0x01});
		updateCoordinates(); //Måste fixas!!!
	} else {
		//Rotera 180 grader och uppdatera vilken riktning roboten nu kör i
		Master(2,....,{0x00,0x04});
		updateDirection(direction,0x04);
		Master(2,....,{0x00,0x04});
		updateDirection(direction,0x04);
		//Kör framåt, uppdatera relevant koordinat
		Master(2,....,{0x00,0x01});
		updateCoordinates(); //Måste fixas!!!
	}
}

void updateDirection(int direction, int rotation){
	if(((direction == 0) & (rotation == 0x03)) | ((direction == 2) & (rotation == 0x04))){
		direction = 1; //österut
	} else if ((direction == 0) & (rotation == 0x04) | ((direction == 2) & (rotation == 0x03))){
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