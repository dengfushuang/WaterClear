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
	volatile ErrorStatus err;
	/***SIM800C电源控制脚初始化***/
    SIM800C_PWRKEY;
	delay_nms(100);
	PWRKEY_H;
//	while(!GPIO_ReadBit(GPIO_Pin_A8));
	UART0Write_Str(GSM_BUF0);
	delay_nms(1000);
	get_MSG("Call");
	get_MSG("SMS");
	delay_nms(5000);  //确保GSM模块搜索到网络后再进入系统
	delay_nms(5000);
	delay_nms(5000);
	delay_nms(5000);
	delay_nms(5000);
	BEE_ON();
	delay_nms(1000);
	BEE_OFF();
	delay_nms(5000);
	delay_nms(5000);
	clear_Buffer();
	UART0Write_Str(GSM_BUF0);
	delay_nms(1000);
	delay_nms(100);
	if(check_ststus(GSM_BUF1,"OK",10) == ERROR)
	{
		;
	}
	if(check_ststus((uint8_t *)"AT+CMGDA=6\r\n","OK",10) == ERROR)
	{
		;
	}
	if(check_ststus(GSM_BUF5,"OK",10) == ERROR)
	{
		;
	}
	if(check_ststus((uint8_t *)"AT+CMGDA=\"DEL ALL\"\r\n","OK",2) == ERROR)
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
	temp  = get_String((uint8_t *)"AT+GSN\r\n",5); 
	if(temp == ERROR)
	{
			reset();
	}
	i = 0;
	/***目标信息***/
	cp = RCV_DATA_BUF;
	while(i < (RCV_BUF_LEN -1))
	{
		if( *cp >= '0' && *cp <= '9')
		{
			break;
		}
		cp++;
		i++;
	}
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
	if(check_ststus(GSM_BUF6,"OK",0) == ERROR)
	{
		err_count ++;
	}
	if((temp = check_ststus(GSM_BUF7,"",0)) == ERROR)
	{
	    err_count ++;
	}
	if(err_count > 10)
	{
		err_count = 0;
		//NVIC_SystemReset();
	}
	if(temp == SUCCESS)
	{
		    err_count  =0;
			UART0Write_Str(GSM_BUF8);
			delay_nms(200);
			temp = check_ststus(GSM_BUF9,"CONNECT OK",0);
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
	uint8_t i;
	uint8_t start_cnt;
	uint8_t end_cnt;
	if(get_MSG("RECV FROM:") == SUCCESS)
	{
		i = 0;
		start_cnt = 0;
		end_cnt = 0;
		while((i < (RCV_BUF_LEN -1))&&(RCV_DATA_BUF[i]!= 0))
		{
			if( RCV_DATA_BUF[i] == '{')
			{
				start_cnt ++;
			}
			if(RCV_DATA_BUF[i] == '}')
			{
				end_cnt ++;
			}
			i++;
		}
		if(start_cnt != end_cnt)
		{
			return ERROR;
		}else
		{
			return SUCCESS;
		}
	}
    else
	{
		return ERROR;
	}		
}

