#include "tds.h"
#include "math.h"
#define TIME_OUT 1600000
volatile uint32_t TDS1count1,TDS2count1,tcount = 0;
volatile uint32_t tflag = TIME_OUT;
/*********************************************************************************************************
** 函数名称: timer_Init
** 功能描述:16位定时器初始化
** 输　入: 无
** 输　出: 无
********************************************************************************************************/
void tds_Timer_Init(void)
{
	T16Nx_BaseInitStruType T;
	
	T.T16Nx_ClkS  = T16Nx_ClkS_PCLK;
	T.T16Nx_EDGE = T16Nx_EDGE_Rise;
	T.T16Nx_Mode = T16Nx_Mode_TC0;
	T.T16Nx_PREMAT = 1;
	T.T16Nx_SYNC = Disable;
	T16Nx_BaseInit(T16N2,&T);
	T16Nx_MAT0ITConfig(T16N2,T16Nx_Clr_Int);
	T16Nx_SetCNT0(T16N2,0);
	T16Nx_SetMAT0(T16N2,48000);
	T16Nx_ITConfig(T16N2,T16Nx_IT_MAT0,Enable);
	NVIC_Init(NVIC_T16N2_IRQn,NVIC_Priority_1,Enable);
	
}

/*********************************************************************************************************
** 函数名称: tds_Init
** 功能描述:频率输入口初始化
** 输　入: 无
** 输　出: 无
********************************************************************************************************/
void tds_Init(void)
{
   #ifdef PCB_V1_00
	GPIO->PADIR.Word = 0X00000140;
	GPIO->PAINEB.Word &= 0xFFFFFEBF;
   #endif
   #ifdef PCB_V1_01
	GPIO->PADIRBCR.Word |= 0X0000000C;
	delay_nms(50);
	GPIO->PADIR.Word = 0X00000030;
	GPIO->PAINEB.Word &= 0xFFFFFFCF;
   #endif
	tds_Timer_Init();
}
/*********************************************************************************************************
** 函数名称: get_Fre1、get_Fre2
** 功能描述: 读取频率
** 输　入: 无
** 输　出: 成功SUCCESS,失败ERROR
********************************************************************************************************/
void set_TDSEN(uint8_t t)
{
    #ifdef PCB_V1_01
	switch(t)
	{
	
		case 1: GPIO->PADATABCR.Word  = 0x000000004;delay_nms(50);GPIO->PADATABSR.Word  = 0x000000008;break;
		case 2: GPIO->PADATABCR.Word  = 0x000000008;delay_nms(50);GPIO->PADATABSR.Word  = 0x000000004;break;
		default:break;
	}
	#endif
}
/*********************************************************************************************************
** 函数名称: get_Fre1、get_Fre2
** 功能描述: 读取频率
** 输　入: 无
** 输　出: 成功SUCCESS,失败ERROR
********************************************************************************************************/
ErrorStatus get_Fre1(volatile uint32_t *ct1)
{
	volatile uint32_t t1 = 0,t2 = 0 ;
	volatile uint32_t tdscount = 0;
	tflag = 1;
	tcount = 0;
	set_TDSEN(1);
	T16Nx_Enable(T16N2);
	while(tflag)
	{
    #ifdef PCB_V1_00
		t1 = (GPIO->PAPORT.Word>>6)&0x01;
	#endif
	#ifdef PCB_V1_01
		t1 = (GPIO->PAPORT.Word>>4)&0x01;
	#endif
	    if(t1 != t2)
		{
			tdscount ++;
			t2 = t1;
		}
	}
	*ct1 = tdscount;
    return SUCCESS;	
}
ErrorStatus get_Fre2(volatile uint32_t *ct2)
{
	volatile uint32_t t1 = 0,t2 = 0;
	uint32_t tdscount = 0;
	tflag = 1;
	tcount = 0;
	set_TDSEN(2);
	T16Nx_Enable(T16N2);
	while(tflag)
	{
	#ifdef PCB_V1_00
		t1 = (GPIO->PAPORT.Word>>8)&0x01;
	#endif
	#ifdef PCB_V1_01
		t1 = (GPIO->PAPORT.Word>>5)&0x01;
	#endif
	    if(t1 != t2)
		{
			tdscount ++;
			t2 = t1;
		}
	}
	*ct2 = tdscount;
	return SUCCESS;	
}
/*********************************************************************************************************
** 函数名称: get_TDS
** 功能描述:计算水质
** 输　入: 无
** 输　出: 成功SUCCESS,失败ERROR
********************************************************************************************************/
ErrorStatus get_TDS(volatile uint32_t *tds1,volatile uint32_t *tds2)
{
	volatile uint32_t x1 = 0,x2 = 0;
	double R = 0.0,F = 0.0,a = 0.0;
	get_Fre1(&x1);
	get_Fre2(&x2);
	/***********计算水质**************/
    F = x1/2;
	a = 4.85-log10(F);
	R = pow(10,a);
	a = 3600.0/R;
	*tds1 = (uint32_t)a;
	
	F = x2/2;
	a = 4.85-log10(F);
	R = pow(10,a);
	a = 3600.0/R;
	*tds2 = a;
	return SUCCESS;
}
/*********************************************************************************************************
** 函数名称: UART0_Recieve
** 功能描述:串口接收，
** 输　入: 无
** 输　出: 成功SUCCESS,失败ERROR
********************************************************************************************************/
void T16N2_IRQHandler(void)
{
	if(T16Nx_GetFlagStatus(T16N2,T16Nx_IT_MAT0) != RESET)
	{
		T16Nx_ClearITPendingBit(T16N2,T16Nx_IT_MAT0);
		tcount++;
		if(tcount >= 999)
		{
			tflag = 0;
			T16Nx_Disable(T16N2);
		}
	}
}


