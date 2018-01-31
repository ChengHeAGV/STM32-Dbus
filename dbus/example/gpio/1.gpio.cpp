/**
  ******************************************************************************
  * @file    *.cpp
  * @author  shentq
  * @version V1.2
  * @date    2016/08/14
  * @brief   
  ******************************************************************************
  * @attention
  *
  * Copyright 2015 shentq. All Rights Reserved.
  *
  * Copyright Notice
  * No part of this software may be used for any commercial activities by any form 
  * or means, without the prior written consent of shentq.
  * This specification is preliminary and is subject to change at any time without
  * notice. shentq assumes no responsibility for any errors contained herein.
  ******************************************************************************
  */


/* Includes ------------------------------------------------------------------*/
#include "ebox.h"

void setup()
{
    ebox_init();   

    PA15.mode(OUTPUT_PP); 

} 

int main(void)
{
    setup();

    while(1)
    {
			
			 PA15.toggle();
			 delay_ms(1000);
				
    }
}




