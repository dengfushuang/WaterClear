#ifndef __MSG_BUILD_H
#define __MSG_BUILD_H
#include "user_config.h"

void pressure_Init(void);
void valve_Init(void);
extern void msg_Build(uint8_t *sendStr);
extern ErrorStatus msg_Deal(uint8_t *rcv);
extern void runApplication(void);

#endif
