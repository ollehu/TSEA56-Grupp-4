#define F_CPU 8000000UL

#include <avr/io.h>
#include <asf.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>

#include "initiation_ceremony.h"

#define SCK PORTB7
#define MISO PORTB6
#define MOSI PORTB5
#define SS PORTB4

volatile const uint8_t adc1 = (1<<ADLAR) | (0<<MUX2)|(0<<MUX1)|(1<<MUX0);
volatile const uint8_t adc2 = (1<<ADLAR) | (0<<MUX2)|(1<<MUX1)|(0<<MUX0);
volatile const uint8_t adc3 = (1<<ADLAR) | (0<<MUX2)|(1<<MUX1)|(1<<MUX0);
volatile const uint8_t adc4 = (1<<ADLAR) | (1<<MUX2)|(0<<MUX1)|(0<<MUX0);

int state = 0;
int ADC_conversion = 0;
int detected = 0;
volatile unsigned int counter = 0; 
double total = 0;

int SI_IR1;
int SI_IR2;
int SI_IR3;
int SI_IR4;

volatile uint8_t Distance_1 = 0;
volatile uint8_t tot_overflow = 0;

double Distance_2 = 0;
double Distance_3 = 0;
double Distance_4 = 0;

int SI_IR1_array[5];
int SI_IR2_array[5];
int SI_IR3_array[5];
int SI_IR4_array[5];

int SI_IR1_Final = 0;
int SI_IR2_Final = 0;
int SI_IR3_Final = 0;
int SI_IR4_Final = 0;

int SI_lidar;
uint16_t forward_distance;
uint16_t up_value, down_value;
uint16_t pwm16bit = 65535;
int overflow_count = 0;
int count_1 = 0;

uint32_t answer;

int firstRun = 0;

int getMedian(int arr[]);
unsigned char SPI_send(unsigned char output);
void SPI_MasterInit(void);
void initIC(void);
void initADC(void);
unsigned short AR_read(void);

ISR (TIMER1_CAPT_vect)
{

	
	if (TCCR1B & (1<<ICES1)){ //Rising edge
		up_value = ICR1;
		TCCR1B &= ~(1<<ICES1);
		overflow_count = 0;
		} else { //Falling edge
		down_value = ICR1;
		TCCR1B |= (1<<ICES1);
		forward_distance = down_value - up_value; //+ pwm16bit*overflow_count;
	}
}

ISR (TIMER1_OVF_vect)
{
	overflow_count++;
}

ISR(TIMER2_OVF_vect)
{
	// keep a track of number of overflows
	tot_overflow++;
}


ISR (INT0_vect)
{
	if (firstRun <= 1){
	if(!(EICRA & (1<<ISC00))) //Om nedflank
	{
		TCCR0B |= (0<<CS02)|(1<<CS01)|(0<<CS00);
		EICRA |= (1<<ISC00);
		//PORTB |= (1<<PORTB1);
	}  else if(EICRA & (1<<ISC00)){

		TCCR0B &= ~(1<<CS00);
		TCCR0B &= ~(1<<CS01);
		TCCR0B &= ~(1<<CS02);
		EICRA &= ~(1<<ISC00);
		//PORTB &= ~(1<<PORTB1);	
		//counter = 0;
	}
	firstRun = firstRun + 1;
	}

}

ISR (TIMER0_COMPA_vect)
{
	PORTB ^= (1<<PORTB1);
	counter = counter + 1;
	
}


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
	
	ADCSRA |= (1<<ADSC);	
}


int getMedian(int arr[])
{
		int i, j, swap;

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

int main (void)
{
	
	DDRB = (1<<PORTB0);
	DDRB |= (1<<PORTB1);

	
	initADC();
	initIC();
	init_counter();
	timer2_init();
	
	
	sei(); // Enables global interrupt

	SPI_MasterInit();
	sei();
	while(1) { //Move all calculations to interrupts??
		
		
		
		PORTB |= (1<<PORTB0);
		
		answer = AR_read();
		total = total + answer - 1996.5;
		
		SI_lidar = 10*(0.0127*forward_distance - 4.3678);
		SI_IR1 = 10*(-0.000021834*Distance_1*Distance_1*Distance_1+0.0065*Distance_1*Distance_1 -0.7227*Distance_1 + 35.016);
		SI_IR2 = 10*(-0.000027779*Distance_2*Distance_2*Distance_2+0.0077*Distance_2*Distance_2 -0.7956*Distance_2 + 35.8363);
		SI_IR3 = 10*(-0.000025789*Distance_3*Distance_3*Distance_3+0.0077*Distance_3*Distance_3 -0.8293*Distance_3 + 37.8186);
		SI_IR4 = 10*(-0.00002338455*Distance_4*Distance_4*Distance_4+0.0071*Distance_4*Distance_4 -0.786*Distance_4 + 37.0525);
		
		
		if ((counter>=15)&(counter<=19)){
			detected = 1;
		} else {
			detected = 0;
		}
		
		if(count_1 == 0){
			SI_IR1_array[0] = SI_IR1;
			SI_IR2_array[0] = SI_IR2;
			SI_IR3_array[0] = SI_IR3;
			SI_IR4_array[0] = SI_IR4;
			count_1 = count_1+1;
		}
		else if(count_1 == 1){
			SI_IR1_array[1] = SI_IR1;
			SI_IR2_array[1] = SI_IR2;
			SI_IR3_array[1] = SI_IR3;
			SI_IR4_array[1] = SI_IR4;
			count_1 = count_1 + 1;
		}
		else if(count_1 == 2){
			SI_IR1_array[2] = SI_IR1;
			SI_IR2_array[2] = SI_IR2;
			SI_IR3_array[2] = SI_IR3;
			SI_IR4_array[2] = SI_IR4;
			count_1 = count_1 + 1;
		}
		else if(count_1 == 3){
			SI_IR1_array[3] = SI_IR1;
			SI_IR2_array[3] = SI_IR2;
			SI_IR3_array[3] = SI_IR3;
			SI_IR4_array[3] = SI_IR4;
			count_1 = count_1 + 1;
		}
		else if(count_1 == 4){
			SI_IR1_array[4] = SI_IR1;
			SI_IR2_array[4] = SI_IR2;
			SI_IR3_array[4] = SI_IR3;
			SI_IR4_array[4] = SI_IR4;
			count_1 = 0;
		}
		
		SI_IR1_Final = getMedian(SI_IR1_array); //Höger fram
		SI_IR2_Final = getMedian(SI_IR2_array); // Höger bak
		SI_IR3_Final = getMedian(SI_IR3_array); //Vänster fram
		SI_IR4_Final = getMedian(SI_IR4_array); //Vänster bak
		
		if (SI_IR1_Final > 230)
		{
			SI_IR1_Final = 245;
		} else if (SI_IR1_Final < 40)
		{
			SI_IR1_Final = 0;
		}
		
		
		if (SI_IR2_Final > 230)
		{
			SI_IR2_Final = 245;
		} else if (SI_IR2_Final < 40)
		{
			SI_IR2_Final = 0;
		}
		
				
		if (SI_IR3_Final > 230)
		{
			SI_IR3_Final = 245;
		} else if (SI_IR3_Final < 40)
		{
			SI_IR3_Final = 0;
		}
			
						
		if (SI_IR4_Final > 230)
		{
			SI_IR4_Final = 245;
		} else if (SI_IR4_Final < 40)
		{
			SI_IR4_Final = 0;
		}
								
		if (SI_IR4_Final > 230)
		{
			SI_IR4_Final = 245;
		} else if (SI_IR4_Final < 40)
		{
			SI_IR4_Final = 0;
		}
		
		
		//Receive 90*-rotation start signal
		//answer = AR_read();
		//total = total + answer - 1996.5;
		
		//When 90* rotation -> send stop signal
		/*if(total < -18300){ //Left
		stop;
		}
		else if(total > 18300){ 
		stop;
		} */
		
		
		//Receive 180*-rotation start signal
		//answer = AR_read();
		//total = total + answer - 1996.5;
		
		//When 180* rotation -> send stop signal
		/*if(total < -36600){ //Left
		stop;
		}
		else if(total > 36600){
		stop;
		} */
	
	
	
	  if (tot_overflow >= 11)  // NOTE: '>=' is used
	        {
		        // check if the timer count reaches 53
		        if (TCNT2 >= 53) // 47155)
		        {
			        PORTD |= (1 << 0);     // send interrupt
			        TCNT2 = 0;            // reset counter
			        tot_overflow = 0;     // reset overflow counter'
					PORTD &= ~(1 << 0);
		        }
	        }
	
	
		

	}

}
