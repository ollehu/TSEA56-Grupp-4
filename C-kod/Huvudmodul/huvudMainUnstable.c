#include "I2C_master.h"
//#include "constants.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU 14745600UL
#include <util/delay.h>
#include "searchPath-v18.h"

#include <util/delay.h>



 //Kommunikations-ID, styrkommando, vinkel/sträcka(1)
uint8_t styrManuellt[] = {0x00, 0x02, 0x00}; //Kommunikations-ID, styrkommando, hastighet

int SLA_sensor_R = 0xCB;
int SLA_sensor_W = 0xCA;
int SLA_styr_R = 0xCD;

int counterComputer = 0;
int sendToComputer = 1; //How often sensor data is sent to the computer

int data = 0;
int recieved;
uint8_t computerMessage[3];

volatile int sleep = 0;
volatile int autodrive = 0;
uint8_t switchMode[3] = {0xFC, 0x01, 0x00};

/*void btSend(unsigned char data)
{
	// Wait for empty transmit buffer
	while ( !( UCSR0A & (1<<UDRE0)) );
	// Put data into buffer, sends the data
	UDR0 = data;
}*/

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
	if (data == 0){
		//data = 0;
		computerMessage[data] = UDR0;
		data += 1;
	}  else {
		computerMessage[data] = UDR0;
		data += 1;
		
		if (data == 3){
			if (computerMessage[0] == 252 &&
				computerMessage[1] == 8   &&
				computerMessage[2] == 1){
				
				//for(int i = 0; i<3;i++){
				//	btSend(switchMode[i]);
				//}
				
			} else {
				Master(3,SLA_styr_W,computerMessage);
				
			}
			data = 0;
		}
	}
	
	//recieved = UDR0;
	
	/*if (data == 0){
		if(((autodrive == 0) && (recieved == 255)) || ((autodrive == 1) && (recieved == 252))){
			computerMessage[data] = recieved;
			data = data + 1;
		}
	} else {
		if(((autodrive == 0) & (computerMessage[0] == 255)) | ((autodrive == 1) & (computerMessage[0] == 252))){
			computerMessage[data] = recieved;
			
			if (data < 2){
				data = data + 1;
			} else {
				Master(3,SLA_styr_W,computerMessage);
				data = 0;
			}
		}
	}*/
}

ISR(INT1_vect){ //Interrupt from controller module
	//1. Välj ny modul att utforska
	if(!unexploredPaths() && position[0] == 14 && position[1] == 14){
		//Do nothing...
	} else {
		explore();
	}
	
	
	//This should work when we are interested in testing with target
	/*if((map[position[0]][position[1]] == startPositionValue[0]) && hasFoundTarget() && !unexploredPaths()){
		goTheShortestPath = 1;
	}
	
	if(lastCommand[1] == claw){
		returnStart = nrOfCoordinates - 1;
	}

	if (returnStart != 0xFF){
		returnToStart();
		
	} else if(goTheShortestPath){
		if(!doneShortestPathInit){
			doneShortestPathInit = 1;
			shortestPathInit();
		}
		
		shortestPathToTarget();
	} else {
		explore();
	}*/		
	
	
	
	
	
	
	/*if (sensorData[10]*128 + sensorData[12] < preferredForwardDistance)
	{
		if(sensorData[4] == 245){
			uint8_t autonomousControl[3] = {controlCommandType, commandLeft, 0x03};
			Master(3,SLA_styr_W,autonomousControl);
		} else if(sensorData[2] == 245){
			uint8_t autonomousControl[3] = {controlCommandType, commandRight, 0x03};
			Master(3,SLA_styr_W,autonomousControl);
		} else {
			if (sensorData[2] > sensorData[4]){
				uint8_t autonomousControl[3] = {controlCommandType, commandReverseRight, 0x03};
				Master(3,SLA_styr_W,autonomousControl);
			} else {
				uint8_t autonomousControl[3] = {controlCommandType, commandReverseLeft, 0x03};
				Master(3,SLA_styr_W,autonomousControl);	
			}
		}
	} else {
		uint8_t autonomousControl[3] = {controlCommandType, commandForward, 0x03};
		Master(3,SLA_styr_W,autonomousControl);
	}*/
	
	//SENSORDATA KOMMER I AVBROTTET FRÅN SENSORMODULEN!
}

ISR(INT2_vect){ //Interrupt from sensor module
	counterComputer = counterComputer + 1;

	//Get data from module
	Master(19,SLA_sensor_R,sensorData);
	
	//send the collected data to styr module
	if (autodrive == 1){
		Master(19,SLA_styr_W,sensorData);
	}
	//send the collected data to the computer

		for(int i = 0; i < 19; i++){
			btSend(sensorData[i]);
		}

	
}

void sendMap(void)
{
	uint8_t i;
	uint8_t j;
	for(i = 0; i < 28; ++i){
		for(j = 0; j < 28; ++j){
			btSend(0xFE);
			btSend(i);
			btSend(j);
			btSend(map[i][j]);
		}
	}
}

int main(void)
{
	/*for (int i = 0; i < 8; i++ ) {
		sensorData[ i ] = i+2;
	}*/
	
	TWISetup();
	interruptSetup();
	btInit();
	searchPathInit();
	
	sei();
	
	while(1){
		//Check autonomous mode on/off
		if (PIND & (1<<PIND5)){
			autodrive = 0; // Manual mode
			
			if (switchMode[2] == 1){
				switchMode[2] = 0;
				
				
				//Notify computer
				for(int i = 0; i<3;i++){
					btSend(switchMode[i]);
				}
				
				//Notify controller module
				Master(3,SLA_styr_W,switchMode);
			}
		} else {
			autodrive = 1; // Autonomous mode
			if (switchMode[2] == 0){
				switchMode[2] = 1;
				
				
				//Notify computer
				for(int i = 0; i<3;i++){
					btSend(switchMode[i]);
				}
				
				//Notify controller module
				Master(3,SLA_styr_W,switchMode);
			}
		}
		
		
		if (autodrive == 0) { // Manual mode
			//Do nothing
			} else if (autodrive == 1) { // Autonomous mode
			//sendMap();
		}
		
	}
}
