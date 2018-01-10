#ifndef __MSG_BUILD_H
#define __MSG_BUILD_H
#include "user_config.h"

void valve_Init(void);
extern void MSG_Build(uint8_t *sendStr);
extern ErrorStatus MSG_Deal(uint8_t *rcv);
extern void RunApplication(void);

#endif
