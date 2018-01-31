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

u32 count;
void test()
{
    uint8_t c;
	c = uart2.receive();
    uart2.put_char(c);
	PB9.toggle();
}
void test1()
{
    count++;
    PB8.toggle();
}
void setup()
{
    ebox_init();
	//LED 初始化
    PB8.mode(OUTPUT_PP);
//	PB9.mode(OUTPUT_PP);
//	PE0.mode(OUTPUT_PP);
//	PE1.mode(OUTPUT_PP);
	
	//串口初始化
	uart2.begin(115200);
    uart2.attach_rx_interrupt(test);
    uart2.attach_tx_interrupt(test1);
}
//LED 闪烁参数
u16 OnTime=300;
u16 OffTime=10;

int main(void)
{
    setup();
    while(1)
    {
		uart2.printf("uart is ok ! count = %d\r\n", count);
//        PB8.reset();
		delay_ms(OnTime);
//        PB8.set();
//		delay_ms(OffTime);
//		
//        PB9.reset();
//		delay_ms(OnTime);
//		PB9.set();
//		delay_ms(OffTime);
//		
//        PE0.reset();
//		delay_ms(OnTime);
//		PE0.set();
//		delay_ms(OffTime);
//		
//        PE1.reset();
//		delay_ms(OnTime);
//		PE1.set();
//		delay_ms(OffTime);
    }

}


