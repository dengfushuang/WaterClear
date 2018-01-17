#include "my_systick.h"

volatile uint32_t test_second = 0;

volatile uint32_t timeout = 0;
volatile uint32_t seconds = 0,minutes = 1;
volatile uint8_t beeflag = 0;
volatile static unsigned int delay_temp = 0;
volatile static unsigned int count = 1;
void Delaynms( int x)
{
	unsigned int y;	
	while(x--)
	{
		for(y = 5000;y>0;y--);
	}
}
void delay_nms(unsigned int delay)
{
	delay_temp = delay;
	while(delay_temp);
}
void BEE_ON()
{
	beeflag = 1;
}
void BEE_OFF()
{
	beeflag = 0;
}
void SysTick_IRQHandler()
{
	if(timeout!=0)
	{
		timeout--;
	}
	if(delay_temp != 0)
	{
		delay_temp--;
	}
	if(beeflag == 1)
	{
		GPIO->PADATABRR.Word = 0x00000002;
	}
	if((count%10) == 0)
	{
		
	}
	if(count >= 1000)
	{
		count = 1;
		EPROM.EPROM_S.RunTime ++;
		test_second++;
		seconds++;
		if(seconds >=59)
		{
			minutes++;
			seconds = 0;
		}
		WDT_Clear();
	}
	else
	{
		count ++;
	}
}
