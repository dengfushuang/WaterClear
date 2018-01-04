#include "user_config.h"
uint8_t RCV_DATA_BUF[UART0_BUF_LEN];
EPROM_DATA EPROM;
void reset()
{
	PWRKEY_L;
	Save_To_EPROM(&EPROM.IMEI[0],9);
	delay_nms(5000);
	delay_nms(5000);
	delay_nms(5000);
	delay_nms(5000);
	NVIC_SystemReset();
}
void read_Flash()
{
	IAP_Enable();
	IAP_Unlock();
	IAP_Read((uint32_t*)&EPROM,START_ADDR,9);
}
void write_Flash()
{
	IAP_Enable();
	IAP_Unlock();
	IAP_ErasePage(PAGE_ADDR);
	delay_nms(5);
	IAP_Unlock();
	IAP_WriteWord(,PAGE_ADDR,)
}
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