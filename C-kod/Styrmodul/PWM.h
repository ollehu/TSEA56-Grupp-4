/*
 * PWM.h
 *
 * Created: 4/4/2016 11:33:21 AM
 *  Author: ollul666
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define speedPicker		OCR3A

#define speedLeft		OCR1A
#define speedRight		OCR1B 

#define F_CPU 16000000UL

/**
	Initiates the PWM for all three
	PWM-outputs (left wheel pair, 
	right wheel pair and picker).
*/
void initPWM(void)
{
		//Set output for PWM
		DDRD |= (1<<DDD5)|(1<<DDD4);
		DDRB |= (1<<DDB6);
		
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
		
		//Top value for picker => 20ms duty cycle
		ICR3 = 5000;
}

/**
	Sets speed and direction for 
	left wheel pair. 
	
	speed = integer 0 -> 100
	direction = 0 for reverse
				1 for forward
*/
void leftWheelPair(uint8_t speed, uint8_t direction)
{
	if (direction == 1){
		PORTD |= (1<<PORTD1);
		} else {
		PORTD &= ~(1<<PORTD1);
	}
	speedLeft = speed*(1024/100);
}

/**
	Sets speed and direction for 
	right wheel pair. 
	
	speed = integer 0 -> 100
	direction = 0 for reverse
				1 for forward
*/
void rightWheelPair(uint8_t speed, uint8_t direction)
{
	if (direction == 1){
		PORTD |= (1<<PORTD0);
		} else {
		PORTD &= ~(1<<PORTD0);
	}
	speedRight = speed*(1024/100);
}

/**
	Stop both wheel pairs.
*/
void stopWheels(void)
{
	speedLeft = 0;
	speedRight = 0;
}

/**
	Open picker.
*/
void openPicker(void)
{
	speedPicker = 400; //ändrat från 125. Fungerar lite bättre

}

/**
	Close picker
*/
void closePicker(void)
{
	
	speedPicker = 625;
}