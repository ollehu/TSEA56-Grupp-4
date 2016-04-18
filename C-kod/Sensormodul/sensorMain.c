#define F_CPU 8000000UL

#include <avr/io.h>
#include <asf.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define SCK PORTB7
#define MISO PORTB6
#define MOSI PORTB5
#define SS PORTB4

int sensorData[8];

volatile const uint8_t adc0 = (1<<ADLAR) | (0<<MUX2)|(0<<MUX1)|(0<<MUX0);
volatile const uint8_t adc1 = (1<<ADLAR) | (0<<MUX2)|(0<<MUX1)|(1<<MUX0);
volatile const uint8_t adc2 = (1<<ADLAR) | (0<<MUX2)|(1<<MUX1)|(0<<MUX0);
volatile const uint8_t adc3 = (1<<ADLAR) | (0<<MUX2)|(1<<MUX1)|(1<<MUX0);
volatile const uint8_t adc4 = (1<<ADLAR) | (1<<MUX2)|(0<<MUX1)|(0<<MUX0);

	int state = 0;
	int ADC_conversion = 0;
	int IRM_value = 0;
	double total = 0;
	
	double SI_IR1;
	double SI_IR2;
	double SI_IR3;
	double SI_IR4;
	
	double Distance_1 = 0;
	double Distance_2 = 0;
	double Distance_3 = 0;
	double Distance_4 = 0;
	
	double SI_IR11 = 0;
	double SI_IR12 = 0;
	double SI_IR13 = 0;
	double SI_IR14 = 0;
	double SI_IR15 = 0;
	double SI_IR1_Final = 0;
	
	double SI_IR21 = 0;
	double SI_IR22 = 0;
	double SI_IR23 = 0;
	double SI_IR24 = 0;
	double SI_IR25 = 0;
	double SI_IR2_Final = 0;
	
	double SI_IR31 = 0;
	double SI_IR32 = 0;
	double SI_IR33 = 0;
	double SI_IR34 = 0;
	double SI_IR35 = 0;
	double SI_IR3_Final = 0;
	
	double SI_IR41 = 0;
	double SI_IR42 = 0;
	double SI_IR43 = 0;
	double SI_IR44 = 0;
	double SI_IR45 = 0;
	double SI_IR4_Final = 0;

	
	int angular_velocity = 0;
	uint16_t forward_distance;
	uint16_t up_value, down_value;
	uint16_t pwm16bit = 65535;
	int overflow_count = 0;
	int count_1 = 0;
	
	uint32_t answer;

unsigned char SPI_send(unsigned char output);
void SPI_MasterInit(void);
void initIC(void);
void initADC(void);
unsigned short AR_read(void);

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


ISR (TIMER1_CAPT_vect)
{

	
	if (TCCR1B & (1<<ICES1)){
		//Rising edge
		up_value = ICR1;
		TCCR1B &= ~(1<<ICES1);
		overflow_count = 0;
	} else {
		//Falling edge
		down_value = ICR1;
		TCCR1B |= (1<<ICES1);
		forward_distance = down_value - up_value; //+ pwm16bit*overflow_count;
		
		
	}
}

ISR (TIMER1_OVF_vect)
{
	overflow_count++;
}

ISR(ADC_vect){ //IR-SENSOR
	
	CLKPR = 0x06;
	
	if(ADMUX == adc0){
		IRM_value = ADCH;
		ADMUX = adc1;

	}
	else if(ADMUX == adc1){
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
		ADMUX = adc0;
	}
	
	
	
	ADCSRA |= (1<<ADSC);
		
}

void TWISetup()
{
	//Set slave address and start TWI (including the TWI-interrupt)
	TWAR = (1<<TWA6)|(1<<TWA5)|(0<<TWA4)|(0<<TWA3)|(1<<TWA2)|(0<<TWA1)|(1<<TWA0)|(0<<TWGCE);
	TWCR |= (1<<TWEA)|(1<<TWEN)|(1<<TWIE);
}

void initIC(void)
{
	TIMSK1 |= (1<<ICIE1)|(1<<TOIE1);
	TCCR1B |= (1<<ICES1);
	TCCR1B |= (1<<CS10);
}


/*unsigned char SPI_EXCH (unsigned char output)
{
	SPDR = output; /* Start transmission */
	/* Wait till a transmission and reception are completed */
	/*
	while(!(SPSR & (1<<SPIF)));
	angular_velocity = SPDR;
	return angular_velocity; /* Return Data Register 
}
*/

void SPI_MasterInit(void)
{
	
	/* Set MOSI and SCK output, all others input */

	//DDRB = 0xA0;//DDR_SPI = (1<<DD_MOSI)|(1<<DD_SCK);
	/* Enable SPI, Master, set clock rate fck/16 */
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);//|(1<<SPIE)
	DDRB |= (1<<DDB7)|(1<<DDB5)|(1<<DDB4);
	DDRB &= ~(1<<DDB6);
	PORTB |= (1<<PORTB4);
	
	PORTB &= ~(1<<PORTB4);
	SPI_send(0x94);
	SPI_send(0x00);
	SPI_send(0x00);
	PORTB |= (1<<PORTB4);
	
}

void initADC(void)
{
	DDRA = 0xE0; // Pin 0-4 input for ADC conversion
	DDRD = 0xBB; // Pin 2 input for INT0
	
	//SPSR = 0x80;
	ADCSRA = 0xC8; // Enables ADC, ADC Start conversion and ADC interrupt enable
	ADMUX = 0x20; // Left adjusted ADC data register (ADLAR = 1)
	EIMSK = (1<<INT0); // Enables interrupt INT0
	EICRA = 0x01; //Interrupt at any edge
}

unsigned char SPI_send(unsigned char output)
{
	SPDR = output;
	/* Wait for transmission complete */
	while(!(SPSR & (1<<SPIF)));
	return SPDR;
}

void conversionStart(void)
{
	PORTB &= ~(1<<PORTB4);
	SPI_send(0x94);
	SPI_send(0x00);
	SPI_send(0x00);
	PORTB |= (1<<PORTB4);
}

unsigned short AR_read(void){
	
	conversionStart();
	
	_delay_us(150);
	unsigned char low, high;
	PORTB &= ~(1<<PORTB4);
	SPI_send(0x80);
	high = SPI_send(0x00);
	low = SPI_send(0x00);
	PORTB |= (1<<PORTB4);
	
	return ((high & 0x0F) << 8) + low;
}




int main (void)
{
		//Set port as output
		DDRD = (1<<PORTD0);
		//Kommunikations-ID
		sensorData[ 0 ] = 0xFF;
		
		initADC();
		initIC();
		TWISetup();
		
		sei(); // Enables global interrupt
		//_delay_ms(1);
		


		//_delay_ms(0.5);

		
		
		//_delay_ms(1);
		//AR_read();
		

		SPI_MasterInit();
		
		while(1) { 	//waiting for INT0
		
			answer = AR_read();
			total = total + answer - 1996.5;
			
			sensorData[5] = 10*(0.0127*forward_distance - 4.3678); //SI_lidar
			SI_IR1 = 10*(0.0023*Distance_1*Distance_1 -0.4746*Distance_1 + 30.066);
			SI_IR2 = 10*(0.0023*Distance_2*Distance_2 -0.4746*Distance_2 + 30.766);
			SI_IR3 = 10*(0.0023*Distance_3*Distance_3 -0.4746*Distance_3 + 30.266);
			SI_IR4 = 10*(0.0023*Distance_4*Distance_4 -0.4746*Distance_4 + 30.766);
			
					if(count_1 == 0){
						SI_IR11 = SI_IR1;
						SI_IR21 = SI_IR2;
						SI_IR31 = SI_IR3;
						SI_IR41 = SI_IR4;

						count_1 = count_1+1;
					}
					
					else if(count_1 == 1){
						SI_IR12 = SI_IR1;
						SI_IR22 = SI_IR2;
						SI_IR32 = SI_IR3;
						SI_IR42 = SI_IR4;

						count_1 = count_1 + 1;
					}
					else if(count_1 == 2){
						SI_IR13 = SI_IR1;
						SI_IR23 = SI_IR2;
						SI_IR33 = SI_IR3;
						SI_IR43 = SI_IR4;

						count_1 = count_1 + 1;
					}
					else if(count_1 == 3){
						SI_IR14 = SI_IR1;
						SI_IR24 = SI_IR2;
						SI_IR34 = SI_IR3;
						SI_IR44 = SI_IR4;

						count_1 = count_1 + 1;
					}
					else if(count_1 == 4){
						SI_IR15 = SI_IR1;
						SI_IR25 = SI_IR2;
						SI_IR35 = SI_IR3;
						SI_IR45 = SI_IR4;

						count_1 = 0;
					}
					sensorData[1] = SI_IR11/5 + SI_IR12/5 + SI_IR13/5 + SI_IR14/5 + SI_IR15/5; //SI_IR1_Final
					sensorData[3] = SI_IR21/5 + SI_IR22/5 + SI_IR23/5 + SI_IR24/5 + SI_IR25/5; //SI_IR2_Final
					sensorData[2] = SI_IR31/5 + SI_IR32/5 + SI_IR33/5 + SI_IR34/5 + SI_IR35/5; //SI_IR3_Final
					sensorData[4] = SI_IR41/5 + SI_IR42/5 + SI_IR43/5 + SI_IR44/5 + SI_IR45/5; //SI_IR4_Final
					
					
			

			
				//Receive start signal
						/*answer = AR_read();
						total = total + answer - 1996.5;*/
			
				//When 90* rotation -> send stop signal
						/*if(total < -18300){ //Left
							stop;
						}	
						else if(total > 18300){
						stop;	
						} */
						
		//Trigger data transfer	
		PORTD = (1<<PORTD0);
		_delay_ms(10);
		PORTD = (0<<PORTD0);
			
			

		}

}
