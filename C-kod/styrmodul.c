/*
 * styrmodul.c
 *
 * Created: 4/6/2016 3:35:29 PM
 *  Author: ollul666
 */ 


#include <avr/io.h>
#include "PWM.h"
#include <util/delay.h>

int main(void)
{
	
	initPWM();
	
	leftWheelPair(100,1);
	rightWheelPair(100,1);
	closePicker();
}