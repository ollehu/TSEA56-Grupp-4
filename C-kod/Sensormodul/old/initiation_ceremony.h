#include <avr/io.h>

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void init_counter(void);
int8_t SPI_send(unsigned char output);
void SPI_MasterInit(void);
void initTimer(void);
void initADC(void);
uint8_t AR_read(void);
void timer2_init(void);
void converionStart(void);

uint8_t sum;
uint8_t out;
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

/******************Counter used to identify target*************/
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

/************Timer used to measure forward distance (Lidar-lite)********/
void initTimer(void){
	TIMSK1 |= (1<<ICIE1);//|(1<<OCIE1A)|(1<<TOIE1);
	//TCCR1A |= (1<<COM1A0);
	TCCR1B |= (1<<ICES1);//|(1<<WGM12);
	TCCR1B |= (1<<CS10)|(1<<CS11);//(1<<CS10);
	//OCR1A = 0x009F;
}

/*********Initiates the SPI-interface*******/
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

/*****SPI-interface used to send and receive information from gyroscope******/
int8_t SPI_send(unsigned char output)
{
	SPDR = output;
	/* Wait for transmission complete */
	while(!(SPSR & (1<<SPIF)));
	return SPDR;
}

/****Enables ADC-conversion for IR-sensors****/
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

/********Starts conversion**********/
void converionStart(void)
{
	PORTB &= ~(1<<PORTB4);
	SPI_send(0x94);
	SPI_send(0x00);
	SPI_send(0x00);
	PORTB |= (1<<PORTB4);
}

/*********Returns angular velocity to main program*******/
uint8_t AR_read(void){
	
	converionStart();
	
	_delay_us(150);
	uint8_t low, high;
	PORTB &= ~(1<<PORTB4);
	SPI_send(0x80);
	high = SPI_send(0x00);
	low = SPI_send(0x00);
	PORTB |= (1<<PORTB4);
	sum  = (((high & 0x0F) << 4) + ((low & 0xF0) >> 4));
	
	if (sum > 245){
		sum = 245;
	} else if (sum == 0){
		sum = 124;
	}
	
	return sum;
}

/*********Timer that invokes main module every 50 ms*********/
void timer2_init(void)
{
    // set up timer with prescaler = 256
    TCCR2B |= (1 << CS22)|(1 << CS21);
  
    // initialize counter
    TCNT2 = 0;
  
    // enable overflow interrupt
    TIMSK2 |= (1 << TOIE2);
  
    // enable global interrupts
    sei();
  
    // initialize overflow counter variable
   // tot_overflow = 0;
	
	// connect interrupt in main module
	DDRD |= (1<<PORTD7);
}