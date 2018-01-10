#include "user_config.h"

uint8_t RCV_DATA_BUF[RCV_BUF_LEN];
uint8_t SEND_DATA_BUF[SEND_BUF_LEN];

EPROM_DATA EPROM;
void BEE_init()
{
	GPIO_InitSettingType g;
	g.Signal = GPIO_Pin_Signal_Digital;
	g.Func = GPIO_Reuse_Func0;
	g.Dir = GPIO_Direction_Output;
	g.ODE = GPIO_ODE_Output_Disable;
	g.DS = GPIO_DS_Output_Normal;
	GPIO_Init(GPIO_Pin_A1,&g);
	GPIO_WriteBit(GPIO_Pin_A1,0);
}
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
	valve_Init();
	timer_Init();
	tds_Init();
//	BEE_init();
	UART0Init();
//	delay_nms(1000);
//	BEE_ON();
//	delay_nms(2000);
//	BEE_OFF();
	sim800c_init(115200);
}
int main()
{
	uint8_t count = 0;
	uint32_t f1 = 0,f2 = 0;
	uint8_t ddd[50] = {0};
	SystemInit();
	HardWare_Init();
    while(1)
	{
//		GSM_TCPC_INIT();
		MSG_Build(RCV_DATA_BUF);
		UART0Write_Str(RCV_DATA_BUF);
//		if(count >5)
//		{
//			reset();
//		}
//		if(GSM_TCP_Connect() == SUCCESS)
//		{
//			UART0Write_Str(RCV_DATA_BUF);
//			UART0Putch(GSM_MSG_STOP_FLAG);
//			if(TCP_Recieve(RCV_DATA_BUF) == SUCCESS)
//			{
//				MSG_Deal(RCV_DATA_BUF);
//			}
//		}
//		else
//		{
//			count++;
//		}
		/*************具体执行过程*****************/
		//RunApplication();
//		BEE_ON();
//		delay_nms(1000);
//		BEE_OFF();
		if(get_Fre1(&f1) == ERROR)
		{
			f1 = 99999;
		}
		if(get_Fre2(&f2)== ERROR)
		{
			f2 = 99999;
		}
		sprintf((char *)ddd,"f1==%u,f2==%u",f1,f2);
		UART0Write_Str(ddd);
		delay_nms(3000);
	}
	return 0;
}
