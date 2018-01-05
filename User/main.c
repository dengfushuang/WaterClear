#include "user_config.h"
uint8_t RCV_DATA_BUF[UART0_BUF_LEN];
EPROM_DATA EPROM;
void reset()
{
	PWRKEY_L;
	Save_To_EPROM(&EPROM.IMEI[0],9);
	delay_nms(1500);
	PWRKEY_H;
	delay_nms(5000);
	delay_nms(5000);
	delay_nms(5000);
	NVIC_SystemReset();
}
void read_All_Flash()
{
	IAP_Enable();
	IAP_Unlock();
	IAP_Read((uint32_t*)&EPROM,START_ADDR,9);
}
void write_All_Flash()
{
	uint8_t i = 0;
	for(i = 0 ; i < 10 ;i++)
	{
		IAP_Enable();
		IAP_Unlock();
		IAP_ErasePage(PAGE_ADDR);
		delay_nms(5);
		if(Save_To_EPROM((uint32_t *)&EPROM.IMEI[0],(sizeof(EPROM_DATA)/sizeof(uint32_t)) == SUCCESS))
		{
			break;
		}
	}
}
void HardWare_Init()
{
	timer_Init();
	tds_Init();
	sim800c_init(115200);
}
int main()
{
	uint8_t count = 0;
	SystemInit();
	HardWare_Init();
    while(1)
	{
		GSM_TCPC_INIT();
		MSG_Build(RCV_DATA_BUF);
		if(count >5)
		{
			reset();
		}
		if(GSM_TCP_Connect() == SUCCESS)
		{
			UART0Write_Str(RCV_DATA_BUF);
			UART0Putch(GSM_MSG_STOP_FLAG);
			if(TCP_Recieve(RCV_DATA_BUF) == SUCCESS)
			{
				MSG_Deal(RCV_DATA_BUF);
				
			}
		}
		else
		{
			count++;
		}
		
	}
	return 0;
}