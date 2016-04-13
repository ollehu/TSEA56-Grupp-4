/*
 * LCD.c
 *
 * Created: 4/4/2016 11:33:21 AM
 *  Author: ollul666
 */ 
#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <string.h>

#define F_CPU 16000000UL
#define lcdControl PORTB
#define lcdData PORTA
#define RS PORTB2
#define RW PORTB1
#define EN PORTB0

uint8_t counter = 0;
char *messageTopRow;
char *messageBottomRow;

//Boolean dummy, 
// activeTempMessage = 0 => free to write
// activeTempMessage = 1 => ignore requests to write
uint8_t activeTempMessage = 0;

void lcdClearTopRow(void);
void lcdClearBottomRow(void);

/**
	Flip the Enable bit
*/
void lcdFlipEN(void)
{
	//_delay_us(100);
	lcdControl |= (1<<EN);
	_delay_us(100);
	lcdControl &= ~(1<<EN);
}

/**
	Send a command to LCD
*/
void lcdCommand(unsigned char command)
{
	//RS = 0, R/W = 0
	lcdControl &= ~(1<<RS);
	lcdControl &= ~(1<<RW);
	_delay_us(1);
	
	lcdData = command;
	lcdFlipEN();
}

/**
	Clear display
*/
void lcdClear(void)
{
	//Clear display
	lcdCommand(0x1);
	_delay_ms(2);
}



/**
	Write a single character
*/
void lcdWriteChar(char data)
{
	lcdControl |= (1<<RS);
	lcdControl &= ~(1<<RW);
	lcdData = data;
	lcdFlipEN();
}

/**
	Write a string to top row
*/
void lcdWriteTopRow(char *topRow)
{
	messageTopRow = topRow;
	if (activeTempMessage == 0){
		lcdClearTopRow();
		lcdCommand(0x80 + (16 - strlen(topRow))/2);
		
		int i;
		for(i=0;topRow[i]!='\0';i++){
			lcdWriteChar(topRow[i]);
		}
	}
}

/**
	Write a string to bottom row
*/
void lcdWriteBottomRow(char *bottomRow)
{
	messageBottomRow = bottomRow;
	if (activeTempMessage == 0){
		lcdClearBottomRow();
		lcdCommand(0xC0 + (16 - strlen(bottomRow))/2);
		
		int i;
		for(i=0;bottomRow[i]!='\0';i++){
			lcdWriteChar(bottomRow[i]);
		}
	}
}

void lcdClearTopRow(void)
{
	lcdCommand(0x80);
	int i;
	for(i = 0; i < 16; i++){
		lcdWriteChar(0x20);
	}
}

void lcdClearBottomRow(void)
{
	lcdCommand(0xC0);
	int i;
	for(i = 0; i < 16; i++){
		lcdWriteChar(0x20);
	}
}

/**
	Write temporary message (for about ~1s)
*/
void writeTempMessage(char *topRow, char *bottomRow)
{
	
	counter = 0;
	activeTempMessage = 1;
	
	//Start counter
	TCCR0B |= (1<<CS02)|(1<<CS00);
	TCCR0B &= ~(1<<CS01);
	
	lcdClear();
	
	int i;
	lcdCommand(0x80 + (16 - strlen(topRow))/2);
	for(i=0;topRow[i]!='\0';i++){
		lcdWriteChar(topRow[i]);
	}
	
	lcdCommand(0xC0 + (16 - strlen(bottomRow))/2);
	for(i=0;bottomRow[i]!='\0';i++){
		lcdWriteChar(bottomRow[i]);
	}
}

/**
	Interrupt for ~1s counter
*/
ISR (TIMER0_COMPA_vect)
{
	if (counter < 100){
		counter = counter + 1;
	} else {
		counter = 0;
		activeTempMessage = 0;
		lcdClear();
		lcdCommand(0xC1);
		lcdWriteTopRow(messageTopRow);
		lcdWriteBottomRow(messageBottomRow);
		
		//Stop Counter
		TCCR0B &= ~(1<<CS00);
		TCCR0B &= ~(1<<CS01);
		TCCR0B &= ~(1<<CS02);
	}
}

/**
	Initiate counter
*/
void lcdInitCounter(void)
{
	//Set output
	DDRB |= (1<<DDB3);
	//CTC and toggle on compare
	TCCR0A |= (1<<COM0A0)|(1<<WGM01);
	TCCR0A &= ~(1<<COM0A1);
	TCCR0A &= ~(1<<WGM00);
	TCCR0B &= ~(1<<WGM02);
	
	//Stop Counter
	TCCR0B &= ~(1<<CS00);
	TCCR0B &= ~(1<<CS01);
	TCCR0B &= ~(1<<CS02);
	OCR0A = 0xFF;
	
	//Enable TIMER0 interrupt
	TIMSK0 |= (1<<OCIE0A);
	
	sei();
}

/**
	Initiate LCD
*/
void initLCD(void)
{
	//Set output
	DDRA = 0xFF;
	DDRB |= (1<<DDB2)|(1<<DDB1)|(1<<DDB0);
	
	//RS = 0, R/W = 0
	lcdControl &= ~(1<<RS);
	lcdControl &= ~(1<<RW);
	_delay_ms(1);
	
	//8-bit mode, 2-line mode, 5x7 dots	
	lcdCommand(0x38);
	_delay_us(50);
	
	//Clear display
	lcdCommand(0x1);
	_delay_ms(2);
	
	//Display on, cursor off, blink off
	lcdCommand(0x0C);
	_delay_us(50);
	
	//Increment mode = ON
	//Entire shift = OFF
	lcdCommand(0x6);
	_delay_us(50);
	
	//First position
	lcdCommand(0x80);
	
	//Initiate counter
	lcdInitCounter();
}