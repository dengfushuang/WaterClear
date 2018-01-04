#include "my_systick.h"
volatile static unsigned int temp = 0;
volatile static unsigned int count = 0;
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
	if(count >= 500)
	{
		count = 0;
		WDT_Clear();
	}
	else
	{
		count ++;
	}
}