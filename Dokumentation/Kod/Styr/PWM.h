/*
 * PWM.h
 *
 * Created: 4/4/2016 11:33:21 AM
 *  Author: ollul666
 */ 

#define F_CPU 16000000UL
#define speedPicker		OCR3A
#define speedLidar		OCR3B 
#define speedLeft		OCR1A
#define speedRight		OCR1B

uint16_t pickerMin = 250;
uint16_t pickerMax = 550;

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

/************************************************************************/
/*                              HEADER                                  */
/************************************************************************/
void initPWM(void);
void leftWheelPair(uint8_t speed, uint8_t direction);
void rightWheelPair(uint8_t speed, uint8_t direction);
void stopWheels(void);
void openPicker(void);
void closePicker(void);
void scanLidar(void);

/************************************************************************/
/*	initPWM - Initiate PWM.

	Initiate timers in Phase Correct PWM-mode.
																		*/
/************************************************************************/
void initPWM(void)
{
		//Set output for PWM
		DDRD |= (1<<DDD5)|(1<<DDD4);
		DDRB |= (1<<DDB6)|(1<<DDB7);
		
		
		//Set output for direction
		DDRD |= (1<<DDD1)|(1<<DDD0);
		
		//PWM Phase Correct TOP = ICR3
		TCCR3A |= (1<<WGM31)|(0<<WGM30);
		TCCR3B |= (1<<WGM33)|(1<<WGM32);
		
		TCCR1A |= (1<<WGM11)|(1<<WGM10);
		TCCR1B |= (0<<WGM13)|(1<<WGM12);
		
		//Non-inverted
		TCCR3A |= (1<<COM3A1)|(0<<COM3A0);
		TCCR3A |= (1<<COM3B1)|(0<<COM3B0);
		
		TCCR1A |= (1<<COM1A1)|(0<<COM1A0);
		TCCR1A |= (1<<COM1B1)|(0<<COM1B0);
		
		//Prescale settings
		TCCR3B |= (0<<CS02)|(1<<CS01)|(1<<CS00);
		TCCR1B |= (0<<CS12)|(1<<CS11)|(1<<CS10);
		
		//Top value for picker => 20ms duty cycle-
		ICR3 = 5000;
		
		//Set values for lidar servo and claw
		speedPicker = pickerMax;
}

/************************************************************************/
/*	leftWheelPair - Control left wheel pair.

	speed     : (0-100)	 - Speed
	direction : 1        - Forward
				0        - Reverse
																		*/
/************************************************************************/
void leftWheelPair(uint8_t speed, uint8_t direction)
{
	if (speed >= 0){
		if (direction == 1){
			PORTD |= (1<<PORTD1);
		} else {
			PORTD &= ~(1<<PORTD1);
		}
		if (speed >= 100){
			speedLeft = 1000;	
		} else {
			speedLeft = speed*(1000/100);
		}
	} else {
		speedLeft = 0;
	}
}

/************************************************************************/
/*	rightWheelPair - Control right wheel pair.

	speed     : (0-100)	 - Speed
	direction : 1        - Forward
				0        - Reverse
																		*/
/************************************************************************/
void rightWheelPair(uint8_t speed, uint8_t direction)
{
	if (speed >= 0){
		if (direction == 1){
			PORTD |= (1<<PORTD0);
		} else {
			PORTD &= ~(1<<PORTD0);
		}
		if (speed >= 100){
			speedRight = 1000;
		} else {
			speedRight = speed*(1000/100);
		}
	} else {
		speedRight = 0;
	}
}

/************************************************************************/
/*	stopWheels - Stop both wheel pairs.  								*/
/************************************************************************/
void stopWheels(void)
{
	speedLeft = 0;
	speedRight = 0;
}

/************************************************************************/
/*	openPicker - Open picker.			  								*/
/************************************************************************/
void openPicker(void)
{
	speedPicker = pickerMin;
}

/************************************************************************/
/*	closePicker - Close picker.			  								*/
/************************************************************************/
void closePicker(void)
{
	speedPicker = pickerMax;
}