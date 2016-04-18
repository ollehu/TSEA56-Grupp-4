/*
* Master.c
*
* Created: 4/8/2016 9:28:26 AM
*  Author: eletr654
*/

#include "I2C_master.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU 14745600UL
#include <util/delay.h>


int sensorData[8];
int styrAutonomt[] = {0x00, 0x02}; //Kommunikations-ID, styrkommando
int styrManuellt[] = {0x00, 0x02, 0x00}; //Kommunikations-ID, styrkommando, hastighet

int SLA_sensor_R = 0xCB;
int SLA_sensor_W = 0xCA;
int SLA_styr_R = 0xCD;
int SLA_styr_W = 0xCC;

int counterComputer = 0;
int sendToComputer = 10; //Hur ofta sensordata ska skickas till datorn

int data = 0;
int recieved;
int controlCommand[3];

volatile int sleep = 0;
volatile int autodrive = 0;
int switchMode[3] = {0xFC, 0x01, 0x00};

void btSend(unsigned char data)
{
	// Wait for empty transmit buffer
	while ( !( UCSR0A & (1<<UDRE0)) );
	// Put data into buffer, sends the data
	UDR0 = data;
}

void btInit(void){
	//16Mhz, Baud rate = 115.2kbps
	unsigned char baud = 7;

	UBRR0H = (unsigned char)(baud >> 8);
	UBRR0L = (unsigned char)(baud);
	
	// Enable reciever, transmitter and interrupt
	UCSR0B = (1<<RXEN0)|(1<<TXEN0)|(1<<RXCIE0);
	
	//8 bit data, no parity and 1 stop bit
	UCSR0C = (1<<UCSZ01)|(1<<UCSZ00)|(0<<USBS0);
	sei();
}

ISR(USART0_RX_vect){
	recieved = UDR0;
	if (data == 0){
		if(((autodrive == 0) & (recieved == 255)) | ((autodrive == 1) & (recieved == 252))){
			controlCommand[data] = recieved;
			data = data + 1;
		}
	} else {
		if(((autodrive == 0) & (controlCommand[1] == 255)) | ((autodrive == 1) & (controlCommand[1] == 252))){
			controlCommand[data] = recieved;
	
			if (data < 2){
				data = data + 1;
			} else {
				Master(3,SLA_styr_W,controlCommand);
				data = 0;
			}
		}
	}
}

ISR(INT1_vect){ //Avbrott styr
	//1. Välj ny modul att utforska
	
	//2. Skicka styrkommando till styrmodulen enligt beslut i 1.
	Master(2,SLA_styr_W,styrAutonomt);
	
	//SENSORDATA KOMMER I AVBROTTET FRÅN SENSORMODULEN!
	
	
}

ISR(INT2_vect){ //Avbrott sensor
	counterComputer = counterComputer + 1;

	//1. Hämta sensordata från sensormodulen
	Master(8,SLA_sensor_R,sensorData);
	
	//2. Skicka sensordata till styrmodulen
	Master(8,SLA_styr_W,sensorData);
	
	//3. Skicka sensordata till datormodulen
	if(counterComputer == sendToComputer){
		for(int i = 0; i < 8; i++){
			btSend(sensorData[i]);
		}
		counterComputer = 0;
	}
}

int main(void)
{
	for (int i = 0; i < 8; i++ ) {
		sensorData[ i ] = i+2;
	}
	
	TWISetup();
	interruptSetup();
	btInit();
	
	sei();
	
	while(1){		
		//Check autonomous mode on/off
		if (PIND & (1<<PIND5)){
			autodrive = 0; // Manual mode
			switchMode[3] = 0;
			
			//Notify computer
			for(int i = 0; i<3;i++){
				btSend(switchMode[i]);
			}
			
			//Meddela styrmodulen
			Master(3,SLA_styr_W,switchMode);
		} else {
			autodrive = 1; // Autonomous mode 
			switchMode[3] = 1;
			
			//Notify computer
			for(int i = 0; i<3;i++){
				btSend(switchMode[i]);
			}
			
			//Meddela styrmodulen
			Master(3,SLA_styr_W,switchMode);
		}
		
		
		if (autodrive == 0) { // Manual mode
			//Do nothing...?
		} else if (autodrive == 1) { // Autonomous mode 
			//Send map to computer
		}
		
	}
}