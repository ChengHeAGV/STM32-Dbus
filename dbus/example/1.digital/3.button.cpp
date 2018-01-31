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
    uart1.begin(115200);
    btn.begin();
    led3.begin();
    led4.begin();
}



int main(void)
{
    setup();
    while(1)
    {
        btn.loop();
        if(btn.click())
        {
            led3.on();
            uart1.printf("\r\nclick event!");
        }
        if(btn.release())
        {
            led3.off();
            uart1.printf("\r\nrelease event!");
        }
        if(btn.pressed_for(2000, 5)) //长按两秒，执行5次
        {
            led3.toggle();
            uart1.printf("\r\nlong press event!");
        }
    }
}




