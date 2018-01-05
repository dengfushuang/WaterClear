#include "my_systick.h"
volatile uint8_t sys_CLK = 0;
volatile static unsigned int temp = 0;
volatile static unsigned int count = 0;
volatile static unsigned int rt = 0;
void delay_nms(unsigned int delay)
{
	temp = delay;
	while(temp);
}
void SysTick_IRQHandler()
{
	if(temp != 0)
	{
		temp--;
	}
	if((count % 500) == 0)
	{
		if(count >= 1000)
		{
			count = 0;
			EPROM.RunTime ++;
			sys_CLK = ~sys_CLK;
		}
		WDT_Clear();
	}
	else
	{
		count ++;
	}
}