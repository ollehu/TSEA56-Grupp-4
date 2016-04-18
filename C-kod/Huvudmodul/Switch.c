#include <avr/io.h>
#include <asf.h>
#include <util/delay.h>

int sleep = 0;
int autodrive = 0;




int main(void){

	while(1){
		
		if (PIND & (1<<PIND6)) { 
			sleep = 0; // Sleepmode deactivated
		} else {
			sleep = 1; // Sleepmode activated
		}
		
		if (PIND & (1<<PIND5)){
			autodrive = 0; // Manual mode
		} else {
			autodrive = 1; // Autonomous mode 
		}
	}
}