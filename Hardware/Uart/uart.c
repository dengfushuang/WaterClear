#include "uart.h"

uint8_t RCV_DATA_BUF[RCV_BUF_LEN];
uint8_t SEND_DATA_BUF[SEND_BUF_LEN];

extern volatile uint8_t sys_CLK;
volatile uint8_t rcvflag = 1;
volatile uint8_t start_rcv_flag = 1;
volatile uint8_t end_rcv_flag = 1;
volatile uint8_t msg_rcv_flag = 1;
volatile uint8_t RxCounter = 0,RxCounter1 = 0;
/*********************************************************************************************************
** ��������: UART0Init
** ��������: UART0��ʼ��
** �䡡��: ��
** �䡡��: ��
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
** ��������: clear_Buffer
** ��������: ������ջ���
** �䡡��: ��
** �䡡��: ��
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
** ��������: UART0Putch
** ��������: �������һ�ֽ�
** �䡡��: Data
** �䡡��: ��
********************************************************************************************************/
void UART0Putch(uint8_t Data)
{
	while(UART0->TB7.TBFF7 == 1);
	UART0->TBW.Byte[0] = Data;
}

/*********************************************************************************************************
** ��������: UART0Put_str
** ��������: �������NByte�ֽ�
** �䡡��: ����Data������NByte
** �䡡��: ��
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
** ��������: UART0Write_Str
** ��������: ��������ַ���
** �䡡��: �ַ���
** �䡡��: ��
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
** ��������: UART0Getch
** ��������: ��ȡһ�ֽ�
** �䡡��: ��
** �䡡��: ��ȡ��������
********************************************************************************************************/
uint8_t UART0Getch(void)
{
    uint8_t err;
    err = UART_RecByte(UART0);
    return err;
}

/*********************************************************************************************************
** ��������: UART0_Recieve
** ��������:���ڽ��գ�
** �䡡��: ��
** �䡡��: �ɹ�SUCCESS,ʧ��ERROR
********************************************************************************************************/
ErrorStatus UART0_Recieve(void)
{
	uint16_t i;
	uint32_t timeout = 4800000;
	ErrorStatus err = ERROR;
	start_rcv_flag = 1;
	while(start_rcv_flag)
	{
		/****���볬ʱ����****/
//		timeout --;
//		if(!timeout)
//		{
//		    return ERROR;
//		}
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
** ��������: UART0_IRQHandler
** ��������: �����ж�
** �䡡��: ��
** �䡡��: ��
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
			}
		}
	}
}

/*********************************************************************************************************
** ��������: get_MSG
** ��������: ���ڽ������ڱȽϴ����Ƿ���ܵ��ַ���str;
** �䡡��: �Ƚ��ַ���str
** �䡡��: �ɹ�SUCCESS,ʧ��ERROR
********************************************************************************************************/

ErrorStatus get_MSG(char * str)
{
	uint32_t tout,tt = 16000000;
	tout = 10;
	sprintf((char *)SEND_DATA_BUF,"%s",str);
	RxCounter = 0;
	RxCounter1 = 0;
	msg_rcv_flag = 1;
	start_rcv_flag = 1;
	while(start_rcv_flag);
	while(msg_rcv_flag)
	{
		/****���볬ʱ����****/
		tt = 48000000;
		tout -- ;
		if(tout <= 0)
		{
		    return ERROR;
		}
		while(tt)
		{
			tt--;
		}
		
	}
	return SUCCESS;		
}
/*********************************************************************************************************
** ��������: get_String
** ��������: ��ȡ����sendstr�ַ���֮�󷵻ص���Ϣ��������Ϣͷ�������sendstr
** �䡡��: ������Ϣ���ظ����ʹ���
** �䡡��: ��
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
** ��������: get_NByte
** ��������: ��ȡ����sendstr�ַ���֮�󷵻ص�NByte�ֽڣ�������Ϣͷ�������sendstr
** �䡡��: ������Ϣ���ظ����ʹ���
** �䡡��: ��
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
** ��������: check_ststus
** ��������: ��ȡ����sendstr�ַ���֮�󷵻ص���Ϣ(ֻ������ӡ�ַ�)�����Ƚ��Ƿ����str�ַ���������Ϣͷ�������sendstr
** �䡡��: �����ַ���sendstr���Ƚ��ַ���str���ط�����resend
** �䡡��: ��
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





