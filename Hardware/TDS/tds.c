#include "tds.h"
#define TIME_OUT 1600000
volatile uint32_t TDS1count1,TDS2count1,tcount = 0;
volatile uint32_t tflag = TIME_OUT;
void timer_Init(void)
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

void tds_Init(void)
{
	GPIO->PADIR.Word = 0X00000142;
	GPIO->PAINEB.Word = 0xFFFFFEBD;
}
ErrorStatus get_Fre1(volatile uint32_t *ct1)
{
	volatile uint32_t t1 = 0,t2 = 0 ;
	volatile uint32_t tdscount = 0;
	tflag = 1;
	tcount = 0;
	T16Nx_Enable(T16N2);
	while(tflag)
	{
		t1 = GPIO_ReadBit(GPIO_Pin_A6);
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
	volatile uint32_t tdscount = 0;
	tflag = 1;
	tcount = 0;
	T16Nx_Enable(T16N2);
	while(tflag)
	{
		t1 = GPIO_ReadBit(GPIO_Pin_A8);
	    if(t1 != t2)
		{
			tdscount ++;
			t2 = t1;
		}
	}
	*ct2 = tdscount;
	return SUCCESS;	
}
ErrorStatus get_TDS(volatile uint32_t *tds1,volatile uint32_t *tds2)
{
	volatile uint32_t x1 = 0,x2 = 0;
	/***********¼ÆËãË®ÖÊ**************/
	get_Fre1(&x1);
	get_Fre2(&x2);
	*tds1 = x1;
	*tds2 = x2;
	return SUCCESS;
}
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

