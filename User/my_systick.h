#ifndef __MY_SYSTICK_H
#define __MY_SYSTICK_H

#include "user_config.h"
extern volatile uint32_t seconds,minutes,test_second;
extern volatile uint32_t timeout;
void BEE_ON(void);
void BEE_OFF(void);
extern void delay_nms(unsigned int delay);
extern void Delaynms( int x);
#endif
