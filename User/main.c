#include "user_config.h"
uint8_t RCV_DATA_BUF[UART0_BUF_LEN];
EPROM_DATA EPROM;
void HardWare_Init()
{
	timer_Init();
	tds_Init();
	sim800c_init(115200);
}
int main()
{
	SystemInit();
	HardWare_Init();
    while(1)
	{
		
	}
	return 0;
}