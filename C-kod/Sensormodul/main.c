#define F_CPU 8000000UL

#include <avr/io.h>
#include <asf.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define SCK PORTB7
#define MISO PORTB6
#define MOSI PORTB5
#define SS PORTB4

volatile const uint8_t adc0 = (1<<ADLAR) | (0<<MUX2)|(0<<MUX1)|(0<<MUX0);
volatile const uint8_t adc1 = (1<<ADLAR) | (0<<MUX2)|(0<<MUX1)|(1<<MUX0);
volatile const uint8_t adc2 = (1<<ADLAR) | (0<<MUX2)|(1<<MUX1)|(0<<MUX0);
volatile const uint8_t adc3 = (1<<ADLAR) | (0<<MUX2)|(1<<MUX1)|(1<<MUX0);
volatile const uint8_t adc4 = (1<<ADLAR) | (1<<MUX2)|(0<<MUX1)|(0<<MUX0);

int state = 0;
int ADC_conversion = 0;
int IRM_value = 0;
int detected = 0;
volatile unsigned int counter = 0; 
double total = 0;

double SI_IR1;
double SI_IR2;
double SI_IR3;
double SI_IR4;

volatile uint8_t Distance_1 = 0;
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


//int angular_velocity = 0;
int SI_lidar;
uint16_t forward_distance;
uint16_t up_value, down_value;
uint16_t pwm16bit = 65535;
int overflow_count = 0;
int count_1 = 0;

uint32_t answer;

int firstRun = 0;

unsigned char SPI_send(unsigned char output);
void SPI_MasterInit(void);
void initIC(void);
void initADC(void);
unsigned short AR_read(void);

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

void init_counter(void)
{
	DDRB |= (1<<DDB3);
	TCCR0A |= (1<<COM0A0)|(1<<WGM01);
	
	TCCR0B &= ~(1<<CS00);
	TCCR0B &= ~(1<<CS01);
	TCCR0B &= ~(1<<CS02);
	OCR0A = 0xFF;
	
	TIMSK0 |= (1<<OCIE0A);
	
}


ISR (INT0_vect)
{
	if (firstRun <= 1){
	if(!(EICRA & (1<<ISC00))) //Om nedflank
	{
		TCCR0B |= (0<<CS02)|(1<<CS01)|(0<<CS00);
		EICRA |= (1<<ISC00);
		PORTB |= (1<<PORTB1);
	}  else if(EICRA & (1<<ISC00)){

		TCCR0B &= ~(1<<CS00);
		TCCR0B &= ~(1<<CS01);
		TCCR0B &= ~(1<<CS02);
		EICRA &= ~(1<<ISC00);
		PORTB &= ~(1<<PORTB1);	
		//counter = 0;
	}
	firstRun = firstRun + 1;
	}

}

ISR (TIMER0_COMPA_vect)
{
	PORTB ^= (1<<PORTB1);
	counter = counter + 1;
	//PORTB &= ~(1<<PORTB1);
	
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


void initIC(void)
{
	TIMSK1 |= (1<<ICIE1)|(1<<TOIE1);
	TCCR1B |= (1<<ICES1);
	TCCR1B |= (1<<CS10);
}

void SPI_MasterInit(void)
{
	

	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);
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
	ADCSRA = 0xCB; // Enables ADC, ADC Start conversion and ADC interrupt enable
	ADMUX = 0x20; // Left adjusted ADC data register (ADLAR = 1)
	EIMSK = (1<<INT0); // Enables interrupt INT0
	EICRA = (1<<ISC01); //Interrupt at low edge
}

unsigned char SPI_send(unsigned char output)
{
	SPDR = output;
	/* Wait for transmission complete */
	while(!(SPSR & (1<<SPIF)));
	return SPDR;
}

void converionStart(void)
{
	PORTB &= ~(1<<PORTB4);
	SPI_send(0x94);
	SPI_send(0x00);
	SPI_send(0x00);
	PORTB |= (1<<PORTB4);
}

unsigned short AR_read(void){
	
	converionStart();
	
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
	
	DDRB = (1<<PORTB0);
	DDRB |= (1<<PORTB1);

	
	initADC();
	initIC();
	init_counter();
	
	
	sei(); // Enables global interrupt
	//_delay_ms(1);
	


	//_delay_ms(0.5);

	
	
	//_delay_ms(1);
	//AR_read();
	

	SPI_MasterInit();
	sei();
	while(1) { 	//waiting for INT0
		PORTB |= (1<<PORTB0);
		
		answer = AR_read();
		total = total + answer - 1996.5;
		
		SI_lidar = 10*(0.0127*forward_distance - 4.3678);
		SI_IR1 = 10*(-0.000021834*Distance_1*Distance_1*Distance_1+0.0065*Distance_1*Distance_1 -0.7227*Distance_1 + 35.016);
		SI_IR2 = 10*(-0.000027779*Distance_2*Distance_2*Distance_2+0.0077*Distance_2*Distance_2 -0.7956*Distance_2 + 35.8363);
		SI_IR3 = 10*(-0.000025789*Distance_3*Distance_3*Distance_3+0.0077*Distance_3*Distance_3 -0.8293*Distance_3 + 37.8186);
		SI_IR4 = 10*(-0.00002338455*Distance_4*Distance_4*Distance_4+0.0071*Distance_4*Distance_4 -0.786*Distance_4 + 37.0525);
		
		
		if ()
		
		
		
		if(count_1 == 0){
			SI_IR11 = SI_IR1;
			count_1 = count_1+1;
		}
		
		
		else if(count_1 == 1){
			SI_IR12 = SI_IR1;
			count_1 = count_1 + 1;
		}
		else if(count_1 == 2){
			SI_IR13 = SI_IR1;
			count_1 = count_1 + 1;
		}
		else if(count_1 == 3){
			SI_IR14 = SI_IR1;
			count_1 = count_1 + 1;
		}
		else if(count_1 == 4){
			SI_IR15 = SI_IR1;
			count_1 = 0;
		}
		SI_IR1_Final = SI_IR11/5 + SI_IR12/5 + SI_IR13/5 + SI_IR14/5 + SI_IR15/5; //Höger fram
		
		
		if(count_1 == 0){
			SI_IR21 = SI_IR2;
			count_1 = count_1+1;
		}
		
		else if(count_1 == 1){
			SI_IR22 = SI_IR2;
			count_1 = count_1 + 1;
		}
		else if(count_1 == 2){
			SI_IR23 = SI_IR2;
			count_1 = count_1 + 1;
		}
		else if(count_1 == 3){
			SI_IR24 = SI_IR2;
			count_1 = count_1 + 1;
		}
		else if(count_1 == 4){
			SI_IR25 = SI_IR2;
			count_1 = 0;
		}
		SI_IR2_Final = SI_IR21/5 + SI_IR22/5 + SI_IR23/5 + SI_IR24/5 + SI_IR25/5; //Höger bak
		
		if(count_1 == 0){
			SI_IR31 = SI_IR3;
			count_1 = count_1+1;
		}
		
		else if(count_1 == 1){
			SI_IR32 = SI_IR3;
			count_1 = count_1 + 1;
		}
		else if(count_1 == 2){
			SI_IR33 = SI_IR3;
			count_1 = count_1 + 1;
		}
		else if(count_1 == 3){
			SI_IR34 = SI_IR3;
			count_1 = count_1 + 1;
		}
		else if(count_1 == 4){
			SI_IR35 = SI_IR3;
			count_1 = 0;
		}
		SI_IR3_Final = SI_IR31/5 + SI_IR32/5 + SI_IR33/5 + SI_IR34/5 + SI_IR35/5; //Vänster fram
		
		
		if(count_1 == 0){
			SI_IR41 = SI_IR4;
			count_1 = count_1+1;
		}
		
		else if(count_1 == 1){
			SI_IR42 = SI_IR4;
			count_1 = count_1 + 1;
		}
		else if(count_1 == 2){
			SI_IR43 = SI_IR4;
			count_1 = count_1 + 1;
		}
		else if(count_1 == 3){
			SI_IR44 = SI_IR4;
			count_1 = count_1 + 1;
		}
		else if(count_1 == 4){
			SI_IR45 = SI_IR4;
			count_1 = 0;
		}
		SI_IR4_Final = SI_IR41/5 + SI_IR42/5 + SI_IR43/5 + SI_IR44/5 + SI_IR45/5; // Vänster bak
		
		
		

		
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
		
		
		
		

	}

}
