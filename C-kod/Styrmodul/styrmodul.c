/*
 * styrmodul.c
 *
 * Created: 4/6/2016 3:35:29 PM
 *  Author: ollul666
 */ 

#define F_CPU 16000000UL
#include <avr/io.h>
#include "PWM.h"
#include "LCD.h"
#include <util/delay.h>
#include <stdio.h>

int main(void)
{
	
	//initPWM();
	initLCD();
	//leftWheelPair(0,0);
	//rightWheelPair(0,0);
	lcdWriteTopRow("Hejhejhej");
	int i = 0;
	char *mes = "before";
	//char *ide = "Speed:";
	//lcdWriteTopRow(mes);
	for(i = 0; i<200; i++){
			_delay_ms(500);
			//leftWheelPair(i,1);
			//rightWheelPair(i,1);
			sprintf(mes, "Speed: %d", i);
			lcdWriteTopRow(mes);

	}
}