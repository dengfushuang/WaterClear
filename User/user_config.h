#ifndef __USER_CONFIG_H
#define __USER_CONFIG_H


#define PCB_V1_00

/*******************************************************************
                       ϵͳͷ�ļ�
********************************************************************/
#include "stdio.h"
#include "stdint.h"
#include "string.h"
#include "HR8P506.h"
#include "type.h"
#include "system_HR8P506.h"

/*******************************************************************
                       ��ͷ�ļ�
********************************************************************/
#include "lib_gpio.h"
#include "lib_uart.h"
#include "lib_timer.h"
#include "lib_scs.h"
#include "lib_wdt.h"
#include "lib_adc.h"
#include "lib_flashiap.h"

/*******************************************************************
                       �Զ���ͷ�ļ�
********************************************************************/
#include "uart.h"
#include "tds.h"
#include "my_systick.h"
#include "sim800c.h"
#include "msgbuild.h"
#include "water_flow.h"




#define START_ADDR 0x00008BFF
#define PAGE_ADDR (START_ADDR/1024)

#define EEPROM_BASE_ADDR EPROM.IMEI[0]




typedef struct{
    uint8_t IMEI[16];
	uint32_t ServerFlow;
	uint32_t RunFlow;
	uint32_t ServerTime;
	uint32_t RunTime;
    uint8_t  ServerType;
    uint8_t  ValveStatus;
	uint8_t  CircleTime;
	uint8_t  ContinuTime;
}EPROM_DATA;

extern EPROM_DATA EPROM;
extern void reset(void);
extern void write_All_Flash(void);
extern void read_All_Flash(void);
#endif
