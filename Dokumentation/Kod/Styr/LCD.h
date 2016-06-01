/*
 * LCD.c
 *
 * Created: 4/4/2016 11:33:21 AM
 *  Author: ollul666
 */ 

#define F_CPU 16000000UL
#define lcdControl PORTB
#define lcdData PORTA
#define RS PORTB2
#define RW PORTB1
#define EN PORTB0

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <string.h>

/************************************************************************/
/*                               LCD                                    */
/************************************************************************/
uint8_t counter = 0;
char *messageTopRow;
char *messageBottomRow;
uint8_t activeTempMessage = 0;

/************************************************************************/
/*                              HEADER                                  */
/************************************************************************/
void lcdFlipEN(void);
void lcdCommand(unsigned char command);
void lcdClear(void);
void lcdWriteChar(char data);
void lcdWriteTopRow(char *topRow);
void lcdWriteBottomRow(char *bottomRow);
void lcdClearTopRow(void);
void lcdClearBottomRow(void);
void writeTempMessage(char *topRow, char *bottomRow);
void lcdInitCounter(void);
void initLCD(void);

/************************************************************************/
/*	lcdFlipEN - flip the enable bit.

	Flip the enable bit for LCD to receive data.
																		*/
/************************************************************************/
void lcdFlipEN(void)
{
	lcdControl |= (1<<EN);
	_delay_us(100);
	lcdControl &= ~(1<<EN);
}

/************************************************************************/
/*	lcdCommand - Send command to LCD.

	Enter command mode, send command and flip the enable bit.
																		*/
/************************************************************************/
void lcdCommand(unsigned char command)
{
	lcdControl &= ~(1<<RS);
	lcdControl &= ~(1<<RW);
	_delay_us(100);
	
	lcdData = command;
	lcdFlipEN();
}

/************************************************************************/
/*	lcdClear - Clear the LCD.

	Send clear command and delay (do not remove delay).
																		*/
/************************************************************************/
void lcdClear(void)
{
	lcdCommand(0x1);
	_delay_ms(2);
}

/************************************************************************/
/*	lcdWriteChar - Write a single character to LCD.

	data : Character to put at cursor position.
	
	Enter write mode, puts character at cursor position and flips
	the enable bit.
																		*/
/************************************************************************/
void lcdWriteChar(char data)
{
	lcdControl |= (1<<RS);
	lcdControl &= ~(1<<RW);
	lcdData = data;
	lcdFlipEN();
}

/************************************************************************/
/*	lcdWriteTopRow - Write a string to top row.

	topRow : String to put on top row.
	
	Set cursor in order to center message. Loop through string and 
	put each character (increment of cursor is handled by LCD).
																		*/
/************************************************************************/
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

/************************************************************************/
/*	lcdWriteBottomRow - Write a string to bottom row.

	bottomRow : String to put on bottom row.
	
	Set cursor in order to center message. Loop through string and 
	put each character (increment of cursor is handled by LCD).
																		*/
/************************************************************************/
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

/************************************************************************/
/*	lcdClearTopRow - Clear top row.

	Set cursor at beginning of top row and loop through row printing 
	empty spaces at each position.
																		*/
/************************************************************************/
void lcdClearTopRow(void)
{
	lcdCommand(0x80);
	int i;
	for(i = 0; i < 16; i++){
		lcdWriteChar(0x20);
	}
}

/************************************************************************/
/*	lcdClearBottomRow - Clear bottom row.

	Set cursor at beginning of bottom row and loop through row printing 
	empty spaces at each position.
																		*/
/************************************************************************/
void lcdClearBottomRow(void)
{
	lcdCommand(0xC0);
	int i;
	for(i = 0; i < 16; i++){
		lcdWriteChar(0x20);
	}
}

/************************************************************************/
/*	writeTempMessage - Write a temporary message, displaying for
		about ~1 second.
		
	topRow    : Message to display on top row.
	bottomRow : Message to display on bottom row.

	Start the counter and put message from arguments.
																		*/
/************************************************************************/
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

/************************************************************************/
/*	Interrupt: Timer0 Compare A vector - Increment counter.

	Increment counter until counter reaches 100 (approximately 
	1 second). Reset LCD to old messages, stop and reset counter.
																		*/
/************************************************************************/
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

/************************************************************************/
/*	lcdInitCounter - Initiate counter.   								*/
/************************************************************************/
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

/************************************************************************/
/*	initLCD - Initiate LCD.

	Initiate LCD with settings:
		<> 8-bit mode
		<> 2-line mode
		<> 5x7 dots
		<> Cursor off
		<> Blink off
		<> Increment on
		<> Entire shift off
																		*/
/************************************************************************/
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