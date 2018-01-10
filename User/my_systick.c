#include "my_systick.h"
volatile uint8_t sys_CLK = 0;
volatile uint8_t beeflag = 0;
volatile static unsigned int temp = 0;
volatile static unsigned int count = 0;
volatile static unsigned int rt = 0;

void Delaynms(unsigned int x)
{
	unsigned int y;
	while(x--)
	{
		for(y=47000;y>0;y--);

	}
}
void delay_nms(unsigned int delay)
{
	temp = delay;
	while(temp);
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
	if(temp != 0)
	{
		temp--;
	}
	if(beeflag == 1)
	{
		GPIO->PADATABRR.Word = 0x00000002;
	}
	if((count%500) == 0)
	{
		if(count >= 1000)
		{
			count = 0;
			EPROM.RunTime ++;
			sys_CLK = ~sys_CLK;
		}
		WDT_Clear();
		count ++;
	}
	else
	{
		count ++;
	}
}
