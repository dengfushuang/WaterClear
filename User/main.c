#include "user_config.h"

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
	Save_To_EPROM(&EPROM.IMEI[0],9);
#ifdef PCB_V1_00
	PWRKEY_L;
	delay_nms(5000);
	delay_nms(5000);
	delay_nms(5000);
	delay_nms(5000);
	delay_nms(5000);
	delay_nms(5000);
#endif
#ifdef PCB_V1_01
	sim800c_OFF();
	BEE_ON();
	delay_nms(500);
	BEE_OFF();
#endif
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
	pressure_Init();
	valve_Init();
	timer_Init();
	tds_Init();
	BEE_init();
	UART0Init();
	delay_nms(1000);
	BEE_ON();
	delay_nms(2000);
	BEE_OFF();  
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
		if(count > 10)
		{
			count = 0;
			reset();
		}
		if(GSM_TCP_Connect() == SUCCESS)
		{
			count = 0;
			clear_RCV_Buffer();
			msg_Build(RCV_DATA_BUF);
			UART0Write_Str(RCV_DATA_BUF);
			UART0Putch(GSM_MSG_STOP_FLAG);
			if(TCP_Recieve(RCV_DATA_BUF) == SUCCESS)
			{
				msg_Deal(RCV_DATA_BUF);
			}
		}
		else
		{
			count++;
		}
		/*************具体执行过程*****************/
		runApplication();
	    /****************以下为测试频率的程序***************/
//		BEE_ON();
//		delay_nms(1000);
//		BEE_OFF();
//	    get_Fre1(&f1);
//		get_Fre2(&f2);
//		sprintf((char *)ddd,"f1==%u,f2==%u",f1,f2);
//		UART0Write_Str(ddd);
		delay_nms(3000);
	}
	return 0;
}
