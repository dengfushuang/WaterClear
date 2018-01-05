#ifndef __TDS_H
#define __TDS_H

#include "user_config.h"

extern void timer_Init();
extern void start_counter();
extern void tds_Init();
//extern ErrorStatus getFre1(uint32_t *ct1,uint32_t *ct2);
extern ErrorStatus getTDS(volatile uint32_t *tds1,volatile uint32_t *tds2);

#endif