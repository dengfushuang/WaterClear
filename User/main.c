#include "user_config.h"
uint8_t  serverIP[] = "\"140.143.187.125\",\"11222\"\r\n";
EEPROM_DATA EPROM;
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
	Save_To_EPROM((uint32_t *)&EEPROM_BASE_ADDR,(sizeof(EPROM_DATA)/sizeof(uint32_t)));
	delay_nms(200);
	Save_To_EPROM((uint32_t *)&EEPROM_BASE_ADDR,(sizeof(EPROM_DATA)/sizeof(uint32_t)));
	PWRKEY_L;
#ifdef PCB_V1_00

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
	IAP_Read((uint32_t*)&EEPROM_BASE_ADDR,START_ADDR,(sizeof(EEPROM_DATA)/sizeof(uint32_t)));
	delay_nms(200);
	IAP_Read((uint32_t*)&EEPROM_BASE_ADDR,START_ADDR,(sizeof(EEPROM_DATA)/sizeof(uint32_t)));
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
		if(Save_To_EPROM((uint32_t *)&EEPROM_BASE_ADDR,(sizeof(EPROM_DATA)/sizeof(uint32_t)))== SUCCESS)
		{
			break;
		}
	}
}
void HardWare_Init()
{
	pressure_Init();
	valve_Init();
	tds_Init();
	BEE_init();
	UART0Init();
	delay_nms(500);	
	sim800c_init(115200);
	my_Task_Timer_Init();
}
int main()
{
	uint8_t err_count = 0,connected_flag = 0;
	SystemInit();
	read_All_Flash();
	if(EPROM.EPROM_S.CircleTime > 120)
	{
		EPROM.EPROM_S.CircleTime = 60;
	}
    for(err_count = 0 ;err_count < 5 ;err_count ++)
	{
		if(EPROM.EPROM_S.ServerIP[err_count] <32 || EPROM.EPROM_S.ServerIP[err_count] > 127)
		{
			connected_flag ++;
		}
	}
	if(connected_flag != 0 )
	{
		sprintf((char *)&EPROM.EPROM_S.ServerIP[0],"%s",(char *)serverIP);
	}
	err_count = 0;
	connected_flag = 0;
	HardWare_Init();
    while(1)
	{
		
		runApplication();
		if(minutes%29 == 0)
		{
			Save_To_EPROM((uint32_t *)&EEPROM_BASE_ADDR,(sizeof(EPROM_DATA)/sizeof(uint32_t)));
			delay_nms(70);
			Save_To_EPROM((uint32_t *)&EEPROM_BASE_ADDR,(sizeof(EPROM_DATA)/sizeof(uint32_t)));
		}
		if(test_second > EPROM.EPROM_S.CircleTime )
		{
			
			test_second = 0;
			do{
				GSM_SMS_RCV();
				connected_flag = 0;
				if(TCP_Connected() == ERROR)
				{
					GSM_TCPC_INIT();
					if(GSM_TCP_Connect() == SUCCESS)
				    { 
						connected_flag = 1;
					}
					else
					{
						err_count++;
					}
					if(err_count > 5)
					{
						GSM_SMS_RCV();
						reset();
					}
				}
				else
				{
					connected_flag = 1;
				}
				if(connected_flag)
				{
//					EPROM.EPROM_S.ValveStatus = 0x07; //²âÊÔÓï¾ä£¬ÏÂÔØÊ±×¢ÊÍµô
					connected_flag = 0;
					err_count = 0;
					clear_RCV_Buffer();
					msg_Build(RCV_DATA_BUF);
					T16Nx_Disable(T16N1);
					if(TCP_Recieve() == SUCCESS)
					{
						msg_Deal(RCV_DATA_BUF);
					}
					else
					{
						err_count ++;
					}
					
					T16Nx_Enable(T16N1);
				}
				
			}while(err_count);
		}	
	}
	return 0;
}



/*int main()
{
	uint8_t err_count = 0,connected_flag = 0,tt[80];
	SystemInit();
	read_All_Flash();
	if(EPROM.EPROM_S.CircleTime > 120)
	{
		EPROM.EPROM_S.CircleTime = 60;
	}
    for(err_count = 0 ;err_count < 5 ;err_count ++)
	{
		if(EPROM.EPROM_S.ServerIP[err_count] <32 || EPROM.EPROM_S.ServerIP[err_count] > 127)
		{
			connected_flag ++;
		}
	}
	if(connected_flag != 0 )
	{
		sprintf((char *)&EPROM.EPROM_S.ServerIP[0],"%s",(char *)serverIP);
	}
	err_count = 0;
	connected_flag = 0;
	HardWare_Init();
    while(1)
	{
		
		runApplication();
		if(minutes%29 == 0)
		{
			Save_To_EPROM((uint32_t *)&EEPROM_BASE_ADDR,(sizeof(EPROM_DATA)/sizeof(uint32_t)));
			delay_nms(70);
			Save_To_EPROM((uint32_t *)&EEPROM_BASE_ADDR,(sizeof(EPROM_DATA)/sizeof(uint32_t)));
		}
		if(test_second > 6 )
		{
			BEE_ON();
			delay_nms(1000);
			BEE_OFF();
			 sprintf((char *)SEND_DATA_BUF,"%s","RECV FROM:");
			
			clear_RCV_Buffer();
			rcv_enf_flag = 1;
			UART0_Recieve(8000);
			UART0Write_Str(RCV_DATA_BUF);
			delay_nms(100);
			msg_Deal(RCV_DATA_BUF);
			sprintf((char *)tt,"\r\nRUN=%u,SV=%u,VS=%c%c%c",EPROM.EPROM_S.RunTime,EPROM.EPROM_S.ServerTime,((EPROM.EPROM_S.ValveStatus & 0x00000004)+'0'),((EPROM.EPROM_S.ValveStatus & 0x00000002)+'0'),((EPROM.EPROM_S.ValveStatus & 0x00000001)+'0'));
            UART0Write_Str(tt);		
		}	
	}
	return 0;
}*/




