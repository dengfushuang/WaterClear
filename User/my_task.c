#include "user_config.h"
volatile uint8_t task_flag = 1;

void my_Task_Timer_Init(void)
{
	T16Nx_BaseInitStruType T;
	T.T16Nx_ClkS  = T16Nx_ClkS_PCLK;
	T.T16Nx_EDGE = T16Nx_EDGE_Rise;
	T.T16Nx_Mode = T16Nx_Mode_TC0;
	T.T16Nx_PREMAT = 1;
	T.T16Nx_SYNC = Disable;
	T16Nx_BaseInit(T16N1,&T);
	T16Nx_MAT0ITConfig(T16N1,T16Nx_Clr_Int);
	T16Nx_SetCNT0(T16N1,0);
	T16Nx_SetMAT0(T16N1,48000);
	T16Nx_ITConfig(T16N1,T16Nx_IT_MAT0,Enable);
	NVIC_Init(NVIC_T16N1_IRQn,NVIC_Priority_1,Enable);
	T16Nx_Enable(T16N1);
}

void T16N1_IRQHandler(void)
{
	static uint32_t cc = 0;
	if(T16Nx_GetFlagStatus(T16N1,T16Nx_IT_MAT0) != RESET)
	{
		T16Nx_ClearITPendingBit(T16N1,T16Nx_IT_MAT0);
		cc++;
		if(cc >= 499)
		{
			cc  = 0;
			control_Function();
		}
	}
}
