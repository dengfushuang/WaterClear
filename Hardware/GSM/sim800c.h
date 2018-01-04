#ifndef __SIM800C_H__
#define __SIM800C_H__	 
#include "user_config.h"
#define SIM800C_PWRKEY GPIO->PADIRBCR.Word = 0X02000000
#define PWRKEY_H GPIO->PADATABSR.Word  = 0x02000000
#define PWRKEY_L GPIO->PADATABCR.Word  = 0x02000000
#define  GSM_MSG_STOP_FLAG  0x1A
#define GSM_BUF_SIZE 50

typedef void (*TimeOut_Fun)(void);

extern uint8_t GSM_BUF9[];
extern uint8_t GSM_BUSY;

void sim800c_init(uint32_t BPS);
void GSM_TCPC_INIT(void);
ErrorStatus GSM_TCP_Connect(void);
void TCP_send(uint32_t serialnum);
ErrorStatus TCP_Recieve(uint8_t  rcv[]);

#endif


