#include "main.h"

DigitalOut led4(PB8);
DigitalOut led6(PB9);
	
// DigitalIn pushButton(PA0);
InterruptIn pushButton(PA0);

void Push(void)
{
	led4 = !led4;
}
Serial serial(USART6,PC6,PC7);

char* str="hello world!\r\n";
int main(void)
{
	Systick_Init();
	//pushButton.rise(&Push);
	serial.baud(115200);
  while (1)
  {
		serial.write(str,14);
		led4 = !led4;
		led6 = !led6;
		Delay(100);
		
// 		if(pushButton) led2 = 1;
// 		else led2 = 0;
  }
}
