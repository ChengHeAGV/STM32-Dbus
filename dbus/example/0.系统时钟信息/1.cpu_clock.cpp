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
void setup()
{
    ebox_init();
    uart1.begin(115200);
}
char buf[] = "hello world !\r\n";
int main(void)
{
    setup();
    while(1)
    {
        uart1.printf("sysclock:%d\r\n",cpu.clock.core/1000000);
        uart1.printf("hclk:%d\r\n",cpu.clock.hclk/1000000);
        uart1.printf("pclk1:%d\r\n",cpu.clock.pclk1/1000000);
        uart1.printf("pclk2:%d\r\n",cpu.clock.pclk2/1000000);
        uart1.printf("cpu.ability:%dK\r\n",cpu.ability/1000);
        
        delay_ms(1000);
    }
}




