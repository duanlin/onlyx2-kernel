#ifndef SYSTIMER_H
#define SYSTIMER_H


#define SYSTIMER_TIMER_COUNT 8


#include "SysTask.h"


#ifdef __cplusplus
extern "C"
{
#endif


typedef struct SysTimer
{
	bool singleShot;
	unsigned timeout;
	uint32_t event;

	bool started;
	unsigned tickCount;

	SysTask* trigChain;

	struct SysTimer* next;

} SysTimer;


extern SysTimer gSysTimer[SYSTIMER_TIMER_COUNT];
extern SysTimer* gTimerChain;


int initSysTimer(void);

int openTimer(unsigned timeoutMs, uint32_t event, SysTimer** timerid);
int joinTimer(SysTimer* timerid);
int closeTimer(SysTimer* timerid);

int startTimer(SysTimer* timerid, bool singleShort);


#ifdef __cplusplus
}
#endif


#endif
