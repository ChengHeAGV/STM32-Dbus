/*
file   : *.cpp
author : shentq
version: V1.0
date   : 2015/7/5

Copyright 2015 shentq. All Rights Reserved.
*/

//STM32 RUN IN eBox


#include "ebox.h"
#include "ebox_dac.h"

DACCLASS Dac1(DAC1);

FLASHCLASS flash;//??????flash????

uint8_t wbuf[10];
uint8_t rbuf[10];

TIM timer2(TIM2);

void dac_update(void)
{ 
 Dac1.dac_sin_out_update();
 }

void setup()
{
    ebox_init();
    uart1.begin(9600);

    PE3.mode(OUTPUT_PP); 

    Dac1.begin();


	  PA6.mode(AIN);	 
	
	 timer2.begin(320000);
	 timer2.attach_interrupt(dac_update);
	 timer2.interrupt(ENABLE);
	 timer2.start();
} 

int main(void)
{
    setup();

    while(1)
    {
			
			 PE3.toggle();
			 delay_ms(1000);
			 //uart1.printf("DAC VALUE = %d\r\n",dac);
			 uart1.printf("PA6 vol=%.2f, adc=%d\r\n",((float)analog_read_voltage(&PA6))/1000,analog_read(&PA6));
				
    }
}




