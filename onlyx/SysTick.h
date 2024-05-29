#ifndef SYSTICK_H
#define SYSTICK_H


#include <stdbool.h>

#include "SynClock.h"


#define SYSTICK_FREQ_HZ 10000

#define SYSTICK_TICK_TO_SCHEDULE 10
#define SYSTICK_TICK_TO_DEATHING 50


#ifdef __cplusplus
extern "C"
{
#endif


int initSysTick(void);

int handleSysTick(unsigned counter, SynTimeStamp stamp);


#ifdef __cplusplus
}
#endif


#endif
