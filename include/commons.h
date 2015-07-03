/* commons.h */
#pragma once

#include <stdint.h>
#include "stm32f4xx.h"


extern uint32_t SysTickCount;


#define SYSTICK_FREQ 1000


void Console(char string[], int32_t value);


/* commons.h */
