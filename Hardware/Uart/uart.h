#ifndef __UART_H
#define __UART_H
#include "user_config.h"
extern void clear_Buffer(void);
extern uint8_t UART0Init(void);
extern void UART0Putch(uint8_t Data);       
extern void UART0Put_str(uint8_t *Data, uint16_t NByte);
extern void UART0Write_Str(uint8_t *Data);
extern uint8_t UART0Getch(void); 
extern ErrorStatus get_MSG(char * str);
extern ErrorStatus get_String(uint8_t *sendstr,uint8_t resend);
extern ErrorStatus check_ststus(uint8_t *sendstr,const char *str,uint8_t resend);
#endif

