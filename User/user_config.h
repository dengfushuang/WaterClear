#ifndef __USER_CONFIG_H
#define __USER_CONFIG_H
/*******************************************************************
                       系统头文件
********************************************************************/
#include "stdio.h"
#include "stdint.h"
#include "HR8P506.h"
#include "type.h"
#include "system_HR8P506.h"

/*******************************************************************
                       库头文件
********************************************************************/
#include "lib_gpio.h"
#include "lib_uart.h"
#include "lib_timer.h"
#include "lib_scs.h"
#include "lib_wdt.h"

/*******************************************************************
                       自定义头文件
********************************************************************/
#include "uart.h"
#include "tds.h"
#include "my_systick.h"
#include "sim800c.h"
#define UART0_BUF_LEN 50

extern uint8_t RCV_DATA_BUF[];

typedef struct{
	uint8_t VStatu;
    uint8_t IMEI[16];
	uint32_t ServerFlow;
	uint32_t RunFlow;
	uint32_t ServerTime;
	uint32_t RunTime; 
}EPROM_DATA;

extern EPROM_DATA EPROM;
#endif
