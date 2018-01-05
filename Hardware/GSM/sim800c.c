#include "sim800c.h"
/***GSM SMS***/
uint8_t  GSM_BUF0[] = "AT\r\n";
uint8_t  GSM_BUF1[] = "AT+CMGF=0\r\n";
uint8_t  GSM_BUF3[] = "AT+CMGL=\"REC UNREAD\"\r\n";
uint8_t  GSM_BUF4[] = "AT+CNMI=\"GSM\"\r\n";
uint8_t  GSM_BUF5[] = "AT+CMGF=1\r\n";
/***GSM TCP***/
uint8_t  GSM_BUF6[] = "AT+CSTT=\"CMNET\"\r\n";
uint8_t  GSM_BUF7[] = "AT+CIICR\r\n";
uint8_t  GSM_BUF8[] = "AT+CIFSR\r\n";
uint8_t  GSM_BUF9[] = "AT+CIPSTART=\"TCP\",\"120.78.62.18\",\"6001\"\r\n";
uint8_t  GSM_CIPSRIP[] = "AT+CIPSRIP=1\r\n";
uint8_t  GSM_CIPSEND[] = "AT+CIPSEND\r\n";
uint8_t  GSM_CIPCLOSE[] = "AT+CIPCLOSE\r\n";
uint8_t  GSM_CIPSHUT[] = "AT+CIPSHUT\r\n";

/*********************************************************************************************************
** 函数名称: sim800c_init
** 功能描述: GSM模块初始化
** 输　入: 无
** 输　出: 无
********************************************************************************************************/
void sim800c_init(uint32_t BPS)
{
	int cnt=0;
	volatile ErrorStatus err;
    UART0Init();
	/***SIM800C电源控制脚初始化***/
    SIM800C_PWRKEY;
	PWRKEY_H;
	delay_nms(5000);  //确保GSM模块搜索到网络后再进入系统
	
	UART0Write_Str(GSM_BUF0);
	delay_nms(100);
RECIEVE_READY:
	err = deal_string("Call",4);
	err = deal_string("SMS",3);
	if(err == ERROR)
	{
		cnt ++;
		if(cnt < 5)
		{
			goto RECIEVE_READY;
		}
	}
	delay_nms(100);
	if(check_ststus(GSM_BUF1,"OK",2,10) == ERROR)
	{
		;
	}
	if(check_ststus((uint8_t *)"AT+CMGDA=6\r\n","OK",2,10) == ERROR)
	{
		;
	}
	if(check_ststus(GSM_BUF5,"OK",2,10) == ERROR)
	{
		;
	}
	if(check_ststus((uint8_t *)"AT+CMGDA=\"DEL ALL\"\r\n","OK",2,2) == ERROR)
	{
		;
	}
	delay_nms(5000); 
	delay_nms(3000);
	UART0Write_Str(GSM_CIPSRIP);
	delay_nms(2000);
	get_IMEI();
	delay_nms(1000);
}


void get_IMEI(void)
{
	uint16_t i;
	int temp;
	uint8_t *cp;
	
check:
	
	i=0;
	UART0Write_Str((uint8_t *)"AT+GSN\r\n");
	temp  = deal_string("+GSN",4); 
	if(temp == ERROR)
	{
		i++;
		if(i > 10)
		{
			reset();
		}
		delay_nms(1000);
		goto check;
	}
	i = 0;
	/***目标信息***/
	if(UART0GetStr(RCV_DATA_BUF) == ERROR)
	{
		goto check;
	}
	cp = RCV_DATA_BUF;
	if( *cp >= '0' && *cp <= '9')
	{
		for(i=0;i<15;i++)
		{
			EPROM.IMEI[i] = *cp;
			cp++;
		}
		EPROM.IMEI[15] = '\0';
	}
}
/*********************************************************************************************************
** 函数名称: GSM_TCPC_INIT
** 功能描述: GSM TCP客户端初始化
** 输　入: 无
** 输　出: 无
********************************************************************************************************/
void GSM_TCPC_INIT(void)
{
	
	delay_nms(3000);
	UART0Write_Str(GSM_CIPSRIP);
	delay_nms(2000);
}
/*********************************************************************************************************
** 函数名称: GSM_TCPC
** 功能描述: GSM TCP客户端
** 输　入: 无
** 输　出: 无
********************************************************************************************************/
ErrorStatus GSM_TCP_Connect(void)
{
	static int err_count = 0;
	volatile ErrorStatus temp = ERROR;
	if(check_ststus(GSM_BUF6,"OK",2,0) == ERROR)
	{
		err_count ++;
	}
	if((temp = check_ststus(GSM_BUF7,"OK",2,0))== ERROR)
	{
		err_count ++;
	}
	if(err_count > 10)
	{
		err_count = 0;
		PWRKEY_L;
		delay_nms(5000);
		delay_nms(5000);
		delay_nms(5000);
		NVIC_SystemReset();
	}
	if(temp == SUCCESS)
	{
		    err_count  =0;
			UART0Write_Str(GSM_BUF8);
			delay_nms(200);
			temp = check_ststus(GSM_BUF9,"CONNECT OK",10,0);
			if(temp == SUCCESS)
			{
				;
			}
			else
			{
				UART0Write_Str(GSM_CIPSHUT);
				delay_nms(10);
				temp = ERROR;
			}
	}
	else
	{
			UART0Write_Str(GSM_CIPCLOSE);
			delay_nms(100);
			UART0Write_Str(GSM_CIPSHUT);
			delay_nms(1000);
		    temp = ERROR;
	}
    return temp;
}



ErrorStatus TCP_Recieve(uint8_t  rcv[])
{
	uint32_t i;
	volatile ErrorStatus temp;
	temp = ERROR;
	if(deal_string("RECV FROM:",10) == SUCCESS)
	{
		for( i = 0 ; i < (GSM_BUF_SIZE - 1); i++ )
		{
			rcv[i] = UART0Getch();
			if((i >= 5) && (rcv[i] == '>') && (rcv[i-1] == 'D') && (rcv[i-2] == 'N') && (rcv[i-3] == 'E') && (rcv[i-4] == '<'))
			{
				i++;
				while(i < (GSM_BUF_SIZE - 1))
				{
				    rcv[i] = '\0';
				}
				temp = SUCCESS;
				break;
			}
			else
			{
				temp = ERROR;
			}
		}
	}	
	return temp;
}

