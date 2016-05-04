/*
 * Sensormodul.c
 *
 * Created: 4/19/2016 8:43:09 AM
 *  Author: zimin415
 */ 

/************************************************************************/
/*                            DEFINITIONS                               */
/************************************************************************/

#define F_CPU 8000000UL
#define SCK PORTB7
#define MISO PORTB6
#define MOSI PORTB5
#define SS PORTB4

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "sensorInit.h"
#include "I2C_slave.h"


/************************************************************************/
/*                            CONTROLLERS                               */
/************************************************************************/

int slaveAddress = 0xCA;
volatile const uint8_t adc1 = (1<<ADLAR) | (0<<MUX2)|(0<<MUX1)|(1<<MUX0);
volatile const uint8_t adc2 = (1<<ADLAR) | (0<<MUX2)|(1<<MUX1)|(0<<MUX0);
volatile const uint8_t adc3 = (1<<ADLAR) | (0<<MUX2)|(1<<MUX1)|(1<<MUX0);
volatile const uint8_t adc4 = (1<<ADLAR) | (1<<MUX2)|(0<<MUX1)|(0<<MUX0);

/************************************************************************/
/*                            VARIABLES	                                */
/************************************************************************/

int state = 0;
int ADC_conversion = 0;
int target_detected = 0;
int detection = 0;
int findings = 0;
int periodscaler = 0;
volatile unsigned int counter = 0; 
int pwmCount;
volatile uint8_t nextDown = 10;

int SI_IR1;
int SI_IR2;
int SI_IR3;
int SI_IR4;

volatile uint8_t tot_overflow = 2;

double Distance_1 = 0;
double Distance_2 = 0;
double Distance_3 = 0;
double Distance_4 = 0;

uint16_t forwardDistance;
uint16_t forwardDistanceMED;
uint8_t forwardDistanceLOW;
uint8_t forwardDistanceHIGH;
uint16_t up_value, down_value;
volatile int count_1 = 0;
volatile uint8_t lidarCount = 0;
uint8_t answer;

/************************************************************************/
/*                              ARRAYS	                                */
/************************************************************************/

uint8_t sensorData[15];
uint8_t SI_IR1_array[5];
uint8_t SI_IR2_array[5];
uint8_t SI_IR3_array[5];
uint8_t SI_IR4_array[5];
uint16_t SI_LIDAR_array[5];

/************************************************************************/
/*                              METHODS	                                */
/************************************************************************/

void SPI_MasterInit(void);
void initTimer(void);
void initADC(void);
uint8_t getMedianIR(uint8_t arr[]);
uint16_t getMedianLIDAR(uint16_t arr[]);
uint8_t get8Bit(float in);
uint8_t adcCounter;

/************************************************************************/
/*	Interrupt: TWI vector - export data to main module.
	Transfers sensor data to main module and henceforth to control module.
																		*/
/************************************************************************/
ISR(TWI_vect){
	TWCR = (1<<TWEA)|(1<<TWEN)|(0<<TWIE);
	//PORTA = (0<<PORTA0);
	
	int counter = 0;
	
	while(1){
		//SLAVE RECEIVER
		if((TWSR & 0xF8) == 0x60){
			//SLA_W received, ACK returned
			TWCR |= (1<<TWINT)|(1<<TWEA)|(1<<TWEN);
			TWCR &= ~(1<<TWSTA);
			TWCR &= ~(1<<TWSTO);
		} else if ((TWSR & 0xF8) == 0x80){
			//SLA_W, ACK returned, wait for data
			TWCR |= (1<<TWINT)|(1<<TWEA)|(1<<TWEN);
		} else if ((TWSR & 0xF8) == 0x80){
			//SLA_W, NOT ACK returned
			
			//What happens here?
		} else if ((TWSR & 0xF8) == 0xA0){
			//STOP or repeated START
			TWCR |= (1<<TWINT)|(1<<TWEA)|(1<<TWIE);
			break;
			
			//SLAVE TRANSMITTER
		} else if ((TWSR & 0xF8) == 0xA8){
			//SLA_R received, ACK returned, transmit data
			TWDR = sensorData[counter];
			counter = counter + 1;
			TWCR = (1<<TWINT)|(1<<TWEA)|(1<<TWEN);
		} else if ((TWSR & 0xF8) == 0xB8){
			//Data transmitted, ACK received, transmit data
			TWDR = sensorData[counter];
			counter = counter + 1;
			TWCR = (1<<TWINT)|(1<<TWEA)|(1<<TWEN);
		} else if ((TWSR & 0xF8) == 0xC0){
			//SLA_R, last byte transmitted
			TWCR |= (1<<TWINT)|(1<<TWEA)|(1<<TWIE);
			break;
		}
		
		while(!(TWCR & (1<<TWINT)));
	}
}

/************************************************************************/
/*	Interrupt: LIDAR Lite v2 - Laser Rangefinder.
	Measures forward distance up to 40 m. Data is transferred through a 
	PWM-interface.
																		*/
/************************************************************************/
ISR(TIMER1_CAPT_vect){
	
	if (TCCR1B & (1<<ICES1)){
		//Rising edge
		
		up_value = ICR1;
		//pwmCount = 0;
		TCCR1B &= ~(1<<ICES1);
		PORTB |= (1<<PORTB0);
	} else {
		//Falling edge
		down_value = ICR1;
		if (0.4*(down_value - up_value) > 0){
			forwardDistance = 0.4*(down_value - up_value);
			SI_LIDAR_array[lidarCount] = forwardDistance;
			lidarCount += 1;
			
			if (lidarCount == 5){
				lidarCount = 0;
			}
			if (forwardDistance == 0){
				//Do nothing
			}
		}
		TCCR1B |= (1<<ICES1);
		PORTB &= ~(1<<PORTB0);
	}
}

/************************************************************************/
/*	Interrupt: Infrared detector - detection of target.
	Checks if target is found. Responds to any edge.
																		*/
/************************************************************************/
ISR (INT0_vect)
{
	if(!(EICRA & (1<<ISC00))) //If falling edge
	{
		counter = 0;
		TCCR0B |= (0<<CS02)|(0<<CS01)|(1<<CS00);
		EICRA |= (1<<ISC00);
		
	}  else if(EICRA & (1<<ISC00)){
		TCCR0B &= ~(1<<CS00);
		TCCR0B &= ~(1<<CS01);
		TCCR0B &= ~(1<<CS02);
		EICRA &= ~(1<<ISC00);
		
		if(counter>30){
			detection++;
			if(detection>4){
				detection=4;
			}
			
		} else {
			detection = 0;
		}
	}
}

/************************************************************************/
/*	Interrupt: Counter, infrared detector - detection of target.
	Counts length of falling edge. Necessary in order to assure that
	target has been found.
																		*/
/************************************************************************/
ISR (TIMER0_COMPA_vect)
{
	counter = counter + 1;
}

/************************************************************************/
/*	Interrupt: Overflow counter - will eventually invoke main module
	Keeps track of number of overflows
																		*/
/************************************************************************/
ISR(TIMER2_OVF_vect)
{
	tot_overflow++;
}

/************************************************************************/
/*	Interrupt: ADC conversion - Invokes interrupt once conversion is 
	finished
	Transforms an analog input from the IR-sensors into digital output.
																		*/
/************************************************************************/
ISR(ADC_vect){ //IR-SENSOR
	CLKPR = 0x06;
	
	if(ADMUX == adc1){
		Distance_1 = ADCH;
		ADMUX = adc2;	
	}
	else if(ADMUX == adc2){
		Distance_2 = ADCH;
		ADMUX = adc3;
	}
	else if(ADMUX == adc3){
 		Distance_3 = ADCH;
		ADMUX = adc4;
	}
	else{
		Distance_4 = ADCH;
		ADMUX = adc1;	
	}
}

/************************************************************************/
/*	Subprogram: Get median value - picks the median value of five.
	It is given an array of values from the IR-sensors, returns the
	median.
																		*/
/************************************************************************/
uint8_t getMedianIR(uint8_t arr[])
{
		uint8_t i, j, swap;

		for (i=0; i<4; i++) 
		{
			for (j=0; j<4-i; j++)
			{
				if ( arr[j] > arr[j+1])
				{
					swap = arr[j+1];
					arr[j+1] = arr[j];
					arr[j] = swap;
				}
			}
		}

		if(arr[2] > 180){
			arr[2] = 245;		
		}else if(arr[2] < 40){
			arr[2] = 0;
		}		
		return arr[2];
}

/************************************************************************/
/*	Subprogram: Get median value - picks the median value of five.
	It is given an array of values from LIDAR-Lite, returns the
	median.
																		*/
/************************************************************************/
uint16_t getMedianLIDAR(uint16_t arr[])
{
	uint16_t i, j, swap;

	for (i=0; i<4; i++)
	{
		for (j=0; j<4-i; j++)
		{
			if ( arr[j] > arr[j+1])
			{
				swap = arr[j+1];
				arr[j+1] = arr[j];
				arr[j] = swap;
			}
		}
	}
	return arr[2];
}

/************************************************************************/
/*	Subprogram: Converts a float to an unsigned 8-bit integer
																		*/
/************************************************************************/
uint8_t get8Bit(float in)
{
	if (in > 255){
		return 255;
	} else {
		return (uint8_t) in;
	}
}

/************************************************************************/
/*	Subprogram: Puts maximum value from an array to an unsigned 8-bit 
	integer.
																		*/
/************************************************************************/
uint8_t getMax(uint8_t array[])
{
	uint8_t max = 0;
	
	int i;
	
	for(i = 1; i < 14; ++i){
		if (array[i] > max){
			max = array[i];
		}
	}
	return max;
}

/************************************************************************/
/*                             MAIN PROGRAM                             */
/************************************************************************/
int main (void)
{
	
	DDRB |= (1<<PORTB0);
	DDRB |= (1<<PORTB1);
	sensorData[0] = 253;
	
/************************************************************************/
/*					INITIATION OF INTERRUPTS AND SPI					*/
/************************************************************************/	
	TWISetup(slaveAddress);
	initADC();
	initTimer();
	init_counter();
	timer2_init();
	SPI_MasterInit();
	sei();
	
/************************************************************************/
/*                        THE GREAT WHILE-LOOP                          */
/************************************************************************/
	while(1) { 
		
		answer = AR_read();
		
		if (answer == 0){
			answer = 124;
		}
		adcCounter = 1;
		
		if (detection>=4){
			target_detected = 1;
		}
		else{
			target_detected = 0;
		}
		
		SI_IR1 = get8Bit(10*(-0.000021834*Distance_1*Distance_1*Distance_1+0.0065*Distance_1*Distance_1 -0.7227*Distance_1 + 35.016));
		SI_IR2 = get8Bit(10*(-0.000027779*Distance_2*Distance_2*Distance_2+0.0077*Distance_2*Distance_2 -0.7956*Distance_2 + 35.8363));
		SI_IR3 = get8Bit(10*(-0.000025789*Distance_3*Distance_3*Distance_3+0.0077*Distance_3*Distance_3 -0.8293*Distance_3 + 37.8186));
		SI_IR4 = get8Bit(10*(-0.00002338455*Distance_4*Distance_4*Distance_4+0.0071*Distance_4*Distance_4 -0.786*Distance_4 + 37.0525));
			
		SI_IR1_array[count_1] = SI_IR1;
		SI_IR2_array[count_1] = SI_IR2;
		SI_IR3_array[count_1] = SI_IR3;
		SI_IR4_array[count_1] = SI_IR4;
		count_1++;
			
		if (count_1 == 5){
			count_1 = 0;
			
			forwardDistanceMED = getMedianLIDAR(SI_LIDAR_array);
			forwardDistanceLOW = (forwardDistanceMED & 0x7F);
			forwardDistanceHIGH = ((forwardDistanceMED >> 7) & 0x7F);
		
			sensorData[1] = 1; 
			sensorData[2] = getMedianIR(SI_IR2_array);	// Front right 
			sensorData[3] = 2; 
			sensorData[4] = getMedianIR(SI_IR3_array);	// Front left
			sensorData[5] = 3;
			sensorData[6] = getMedianIR(SI_IR1_array);	// Back right
			sensorData[7] = 4;
			sensorData[8] = getMedianIR(SI_IR4_array);	// Back left
			sensorData[9] = 5;
			sensorData[10] = forwardDistanceHIGH;		// LIDAR high
			sensorData[11] = 6;
			sensorData[12] = forwardDistanceLOW;		// LIDAR low
			sensorData[13] = 7;
			sensorData[14] = answer;					// Angular velocity
			sensorData[15] = 8;
			sensorData[16] = target_detected;			// Detection of target
				
			if (getMax(sensorData) > 245){
				forwardDistanceHIGH = 245;
			}
		}

		if (tot_overflow >= 11){						
			if (TCNT2 >= 53){							// Check if the timer count reaches 53
				PORTD |= (1<<PORTD7);					// Send interrupt
			    TCNT2 = 0;								// Reset counter
			    tot_overflow = 0;						// Reset overflow counter
				PORTD &= ~(1<<PORTD7);	
		    }
		}
	
		ADCSRA |= (1<<ADSC);
	}
}