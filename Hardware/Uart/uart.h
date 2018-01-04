#ifndef __UART_H
#define __UART_H
#include "user_config.h"
extern uint8_t UART0Init(void);
extern void UART0Putch(uint8_t Data);       
extern void UART0Put_str(uint8_t *Data, uint16_t NByte);
extern void UART0Write_Str(uint8_t *Data);
extern uint8_t UART0Getch(void); 
ErrorStatus UART0GetStr(uint8_t *str);

extern ErrorStatus deal_string(const char *str,uint16_t str_len);
extern ErrorStatus check_ststus(uint8_t *sendstr,char *str,uint16_t str_len,uint16_t resend);
#endif