#include "uart.h"
extern volatile uint8_t sys_CLK;
uint8_t UART0Init(void)
{
	GPIO_InitSettingType g;
	UART_InitStruType u;
	/************TXD**************/
	g.Signal = GPIO_Pin_Signal_Digital;
	g.Func = GPIO_Reuse_Func1;
	g.Dir = GPIO_Direction_Output;
	g.ODE = GPIO_ODE_Output_Disable;
	g.DS = GPIO_DS_Output_Strong;
	GPIO_Init(GPIO_Pin_A23,&g);
	
	/*************RXD*****************/
	g.Signal = GPIO_Pin_Signal_Digital;
	g.Func = GPIO_Reuse_Func1;
	g.Dir = GPIO_Direction_Input;
	g.PUE = GPIO_PUE_Input_Disable;
	g.PDE = GPIO_PDE_Input_Disable;
	GPIO_Init(GPIO_Pin_A22,&g);
	
	u.UART_BaudRate = 115200;
	u.UART_ClockSet = UART_Clock_1;
	u.UART_RxMode = UART_DataMode_8;
	u.UART_RxPolar = UART_Polar_Normal;
	u.UART_StopBits = UART_StopBits_1;
	u.UART_TxMode = UART_DataMode_8;
	u.UART_TxPolar = UART_Polar_Normal;
	UART_Init(UART0,&u);
	UART_TBIMConfig(UART0,UART_TBIM_Byte);
	UART_RBIMConfig(UART0,UART_TBIM_Byte);
	UART_ITConfig(UART0,UART_IT_RB,Enable);
	NVIC_Init(NVIC_UART0_IRQn,NVIC_Priority_1,Enable);
	UART0_RxEnable();
	return 1;
}
void UART0Putch(uint8_t Data)
{
	while(UART0->STA.TXBUSY == 1);
	UART0->TBW.Byte[0] = Data;
}

void UART0Put_str(uint8_t *Data, uint16_t NByte)
{
	UART0_TxEnable();
    while (NByte-- > 0)
    {
        UART0Putch(*Data++);
    }
	while(UART0->STA.TXBUSY == 1);
	UART0_TxDisable();
}

void UART0Write_Str(uint8_t *Data)
{
	UART0_TxEnable();
    while (*Data != '\0' )
    {
        UART0Putch(*Data++);
    }
	while(UART0->STA.TXBUSY == 1);
	UART0_TxDisable();
}

uint8_t UART0Getch(void)
{
    uint8_t err;
    err = UART_RecByte(UART0);
    return err;
}

uint8_t UART0GetFun(volatile ErrorStatus *count)
{
	uint8_t err,outtime = 0,old_f = 0;
	while(!UART0->IF.RBIF)
	{	
	   if(old_f != sys_CLK)
	   {
		   old_f = sys_CLK;
		   outtime++;
		   if(outtime >= 5)
		   {
			   *count = ERROR;
			   return 0;
		   }
	   }
	}
	*count = SUCCESS;
    err = UART_RecByte(UART0);
    return err;
}

ErrorStatus UART0GetStr(uint8_t *str)
{
	uint16_t len = 0;

	do{
		*str = UART_RecByte(UART0);
		len++;
		if(len >= (UART0_BUF_LEN-1))
		{
			break;
		}
		str++;
	}while(UART0->STA.RXBUSY);
	if(UART0->STA.RXBUSY)
	{
		return ERROR;
	}else
	{
		while(len < (UART0_BUF_LEN-1))
		{
			*str = '\0';
			str++;
			len++;
		}
	    return SUCCESS;
	}
}
void UART0_IRQHandler()
{
	
}

/*********************************************************************************************************
** 函数名称: deal_string
** 功能描述: 字符串处理
** 输　入: buf-数据缓冲区,str-比较字符串,buf_len-缓冲区长度，timeout-超时时间,timeout_fun-超时回调函数；
** 输　出: 无
********************************************************************************************************/
ErrorStatus deal_string(const char *str,uint16_t str_len)
{
	uint8_t data_temp,slen = 0;
	const char *cp1,*cp2;
	int i=0;
	volatile ErrorStatus count = ERROR;
	cp1 = str;
	cp2 = str;
	slen = str_len;
	while(1)
	{
		data_temp = UART0GetFun(&count);
		if(count == ERROR)
		{
			break;
		}
		if(data_temp >= 32 && data_temp <=127)
		{
			if( *cp1 == data_temp)
			{
				
				cp1++;
				i++;
				if( *cp1 == '\0')
				{
					break;
				}
				if(cp1 == NULL )
				{
					return ERROR;
				}
			}
			else
			{
				cp1 = cp2;
				i = 0;
				if(*cp1 == data_temp)
				{
					
					cp1 ++;
					i++;
					if(*cp1 == '\0')
				    {
					    break;
				    }
					if(cp1 == NULL )
				    {
					    return ERROR;
				    }
					
				}
			}  
			if(i >= slen)
			{
				break;
			}
		}
	}
	if(i>= slen)
	{
		return SUCCESS;
	}else
	{
		return ERROR;
	}
}
ErrorStatus check_ststus(uint8_t *sendstr,char *str,uint16_t str_len,uint16_t resend)
{
	int cnt = 0;
	volatile ErrorStatus err ;
	UART0Write_Str(sendstr);
	err = deal_string(str,str_len);
	while(err == ERROR)
	{
		cnt++;
		if(cnt > resend)
		{
			err = ERROR;
			break;
		}
		UART0Write_Str(sendstr);
		err = deal_string(str,str_len);
	}
	return err;
}


