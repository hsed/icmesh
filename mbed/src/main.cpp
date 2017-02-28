#include "mbed.h"

#define LEDNUM 4

static const float freq = 0.2;

int main(){
DigitalOut leds[LEDNUM]={LED1,LED2,LED3,LED4};


	leds[0] = 1;
	while(true){
		leds[0]= !leds[0];
		wait(freq);
		for(int i =1;i<LEDNUM;i++){
			leds[i]=!leds[i-1];
			wait(freq);
		}
	}


return 0;
}
