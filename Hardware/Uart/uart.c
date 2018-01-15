#include "uart.h"

uint8_t RCV_DATA_BUF[RCV_BUF_LEN];
uint8_t SEND_DATA_BUF[SEND_BUF_LEN];

extern volatile uint8_t sys_CLK;
volatile uint8_t rcvflag = 1;
volatile uint8_t start_rcv_flag = 1;
volatile uint8_t end_msg_flag = 1;
volatile uint8_t msg_rcv_flag = 1;
volatile uint8_t RxCounter = 0,RxCounter1 = 0;
/*********************************************************************************************************
** 函数名称: UART0Init
** 功能描述: UART0初始化
** 输　入: 无
** 输　出: 无
********************************************************************************************************/
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
	UART0_TxEnable();
	return 1;
}
/*********************************************************************************************************
** 函数名称: clear_Buffer
** 功能描述: 清除接收缓存
** 输　入: 无
** 输　出: 无
********************************************************************************************************/
void clear_RCV_Buffer(void)
{
	uint16_t i;
	for( i =0 ; i < (RCV_BUF_LEN-1) ;i++)
	{
		RCV_DATA_BUF[i] = 0;
	}
}
/*********************************************************************************************************
** 函数名称: UART0Putch
** 功能描述: 串口输出一字节
** 输　入: Data
** 输　出: 无
********************************************************************************************************/
void UART0Putch(uint8_t Data)
{
	while(UART0->TB7.TBFF7 == 1);
	UART0->TBW.Byte[0] = Data;
}

/*********************************************************************************************************
** 函数名称: UART0Put_str
** 功能描述: 串口输出NByte字节
** 输　入: 数组Data，长度NByte
** 输　出: 无
********************************************************************************************************/
void UART0Put_str(uint8_t *Data, uint16_t NByte)
{
    while (NByte-- > 0)
    {
        UART0Putch(*Data++);
    }
	while(UART0->STA.TXBUSY == 1);
}

/*********************************************************************************************************
** 函数名称: UART0Write_Str
** 功能描述: 串口输出字符串
** 输　入: 字符串
** 输　出: 无
********************************************************************************************************/
void UART0Write_Str(uint8_t *Data)
{
	uint8_t *cp,i;
	RxCounter = 0;
	RxCounter1 = 0;
	msg_rcv_flag = 1;
	for( i =0 ; i < (SEND_BUF_LEN-1) ;i++)
	{
		SEND_DATA_BUF[i] = 0;
	}
    cp = SEND_DATA_BUF;
	sprintf((char *)SEND_DATA_BUF,"%s",(char *)Data);
    while (*cp != '\0' )
    {
        UART0Putch(*cp);
		cp++;
    }
	while(UART0->STA.TXBUSY == 1);
}

/*********************************************************************************************************
** 函数名称: UART0Getch
** 功能描述: 读取一字节
** 输　入: 无
** 输　出: 读取到的数据
********************************************************************************************************/
uint8_t UART0Getch(void)
{
    uint8_t err;
    err = UART_RecByte(UART0);
    return err;
}

/*********************************************************************************************************
** 函数名称: UART0_Recieve
** 功能描述:串口接收，
** 输　入: 无
** 输　出: 成功SUCCESS,失败ERROR
********************************************************************************************************/
ErrorStatus UART0_Recieve(void)
{
	uint16_t i;
	uint32_t timeout = 4000;
	ErrorStatus err = ERROR;
	start_rcv_flag = 1;
	while(start_rcv_flag)
	{
		/****加入超时处理****/
		Delaynms(1);
		timeout --;
		if(!timeout)
		{
		    return ERROR;
		}
	}
    while(UART0->STA.RXBUSY);
	for(i = RxCounter ; i <(RCV_BUF_LEN -1);i++ )
	{
		RCV_DATA_BUF[i] = 0;
	}
	err = SUCCESS;
	return err;
}
/*********************************************************************************************************
** 函数名称: UART0_IRQHandler
** 功能描述: 串口中断
** 输　入: 无
** 输　出: 无
********************************************************************************************************/
void UART0_IRQHandler()
{
	uint8_t temp;
	start_rcv_flag = 0;
	if((UART_GetITStatus(UART0,UART_IT_RB) != RESET) && (UART_GetFlagStatus(UART0,UART_FLAG_RB) != RESET))
	{
		temp = UART0->RBR.Byte[0];
	    if(msg_rcv_flag)
		{
			if(temp == SEND_DATA_BUF[RxCounter1])
			{
				RxCounter1++;
				if(SEND_DATA_BUF[RxCounter1] == '\r' || SEND_DATA_BUF[RxCounter1] == '\n' || SEND_DATA_BUF[RxCounter1] == '\0')
				{
					msg_rcv_flag = 0;
				}
			}
			else
			{
				RxCounter1 = 0;
			}
		}else
		{
			if(temp >= 32 && temp <= 127)
			{
				RCV_DATA_BUF[RxCounter] = temp;
			    RxCounter++;
				if(temp == '}')
				{
					end_msg_flag = 0;
				}
			}
		}
	}
}

/*********************************************************************************************************
** 函数名称: get_MSG
** 功能描述: 串口接收用于比较串口是否接受到字符串str;
** 输　入: 比较字符串str
** 输　出: 成功SUCCESS,失败ERROR
********************************************************************************************************/

ErrorStatus get_MSG(char * str)
{
	uint32_t timeout = 10;
	sprintf((char *)SEND_DATA_BUF,"%s",str);
	RxCounter = 0;
	RxCounter1 = 0;
	msg_rcv_flag = 1;
	start_rcv_flag = 1;
	end_msg_flag = 1;
	while(start_rcv_flag)
	{
		/****加入超时处理****/
		delay_nms(500);
		WDT_Clear();
		timeout --;
		if(!timeout)
		{
		    return ERROR;
		}
	}
	while(end_msg_flag)
	{
		/****加入超时处理****/
		delay_nms(500);
		WDT_Clear();
		timeout --;
		if(!timeout)
		{
		    return ERROR;
		}
	}
	delay_nms(100);
	return SUCCESS;		
}
/*********************************************************************************************************
** 函数名称: get_String
** 功能描述: 读取发送sendstr字符串之后返回的信息，返回信息头必须包括sendstr
** 输　入: 发送信息，重复发送次数
** 输　出: 无
********************************************************************************************************/
ErrorStatus get_String(uint8_t *sendstr,uint8_t resend)
{
	uint8_t i;
	ErrorStatus err0 = ERROR;
	for(i = 0 ; i < (resend+1) ; i++)
	{
		UART0Write_Str(sendstr);
		if(UART0_Recieve() == SUCCESS)
		{
			err0 = SUCCESS;
			break;
		}
		delay_nms(5000);
	}
	return err0;
}
/*********************************************************************************************************
** 函数名称: get_NByte
** 功能描述: 读取发送sendstr字符串之后返回的NByte字节，返回信息头必须包括sendstr
** 输　入: 发送信息，重复发送次数
** 输　出: 无
********************************************************************************************************/
ErrorStatus get_NByte(uint8_t *sendstr,uint16_t NByte)
{
	sprintf((char *)SEND_DATA_BUF,"%s",sendstr);
	UART0Write_Str(sendstr);
	RxCounter = 0;
	RxCounter1 = 0;
	msg_rcv_flag = 1;
	start_rcv_flag = 1;
	while(start_rcv_flag);
	while(RxCounter < NByte);
	return SUCCESS;
}
/*********************************************************************************************************
** 函数名称: check_ststus
** 功能描述: 读取发送sendstr字符串之后返回的信息(只包含打印字符)，并比较是否包含str字符串返回信息头必须包括sendstr
** 输　入: 发送字符串sendstr，比较字符串str，重发次数resend
** 输　出: 无
********************************************************************************************************/
ErrorStatus check_ststus(uint8_t *sendstr,const char *str,uint8_t resend)
{
	uint8_t i;
	ErrorStatus err0 = ERROR;
	for(i = 0 ; i < (resend+1) ; i++)
	{
		UART0Write_Str(sendstr);
		if(UART0_Recieve() == SUCCESS)
		{
			delay_nms(200);
			if((strstr((const char *)RCV_DATA_BUF,str) != NULL))
			{
				err0 = SUCCESS;
				BEE_ON();
				delay_nms(1000);
				BEE_OFF();
				break;
			}else
			{
				delay_nms(5000);
				if((strstr((const char *)RCV_DATA_BUF,str) != NULL))
			    {
				    err0 = SUCCESS;
				    BEE_ON();
				    delay_nms(1000);
				    BEE_OFF();
				    break;
			    }
			}
		}
		delay_nms(5000);
	}
	return err0;
}





