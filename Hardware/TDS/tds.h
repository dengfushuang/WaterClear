#ifndef __TDS_H
#define __TDS_H

#include "user_config.h"
#ifdef PCB_V1_00
  #define TDS1 (GPIO->PAPORT.PORT_6)
  #define TDS2 (GPIO->PAPORT.PORT_8)
#endif
#ifdef PCB_V1_01
  #define TDS1 (GPIO->PAPORT.PORT_4)
  #define TDS2 (GPIO->PAPORT.PORT_5)
#endif
ErrorStatus get_Fre1(volatile uint32_t *ct1);
ErrorStatus get_Fre2(volatile uint32_t *ct2);
extern void timer_Init(void);
extern void tds_Init(void);
extern ErrorStatus get_TDS(volatile uint32_t *tds1,volatile uint32_t *tds2);

#endif
