/**
  ******************************************************************************
  * @file   : *.cpp
  * @author : shentq
  * @version: V1.2
  * @date   : 2016/08/14

  * @brief   ebox application example .
  *
  * Copyright 2016 shentq. All Rights Reserved.         
  ******************************************************************************
 */

#include "ebox.h"
#include "bsp.h"


void setup()
{
    ebox_init();
    led3.begin();
    led4.begin();
}

int main(void)
{
    setup();
    while(1)
    {
        led3.toggle();
        led4.toggle();
       delay_ms(1000);
    }
}




