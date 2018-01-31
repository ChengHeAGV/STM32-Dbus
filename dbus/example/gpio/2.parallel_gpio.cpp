/*
file   : *.cpp
author : shentq
version: V1.0
date   : 2015/7/5

Copyright 2015 shentq. All Rights Reserved.
*/

//STM32 RUN IN eBox


#include "ebox.h"
#include "parallel_gpio.h"

ParallelGpio p;

void setup()
{
    ebox_init();   
    p.bit[0] = &PA15;
    p.bit[1] = &PB4;

    PA15.mode(OUTPUT_PP); 

    p.mode(OUTPUT_PP);
} 

int main(void)
{
    setup();

    while(1)
    {
			
			 p.write(0xff);
			 delay_ms(1000);
			 p.write(0x00);
			 delay_ms(1000);
				
    }
}




