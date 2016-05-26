//************************************************************************/
/*	Interrupt: Infrared detector - detection of target.
	Checks if target is found. Responds to any edge.
																		*/
/************************************************************************/
ISR (INT0_vect)
{
	if(lookingForTarget == 1) {
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
		
			if(counter>20 ){
				anyEdge = 1;
				detection++;
			}
		}
	}
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
		//PORTB ^= (1<<PORTB0);
	}
	else if(ADMUX == adc2){
		Distance_2 = ADCH;
		ADMUX = adc3;
	}
	else if(ADMUX == adc3){
 		Distance_3 = ADCH;
		ADMUX = adc4;
	}
	else if(ADMUX == adc4){
		Distance_4 = ADCH;
		ADMUX = adc5;	
	}
	else{
		if ((ADCH >= 160) && (lastWheelSensor == 0)){
			wheel_counter = wheel_counter + 1;
			lastWheelSensor = 245;
		}
		else if ((ADCH <= 60) && (lastWheelSensor == 245)){
			wheel_counter = wheel_counter + 1;
			lastWheelSensor = 0;
		}
		wheel_sensor = ADCH;
		ADMUX = adc1;
	}
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
