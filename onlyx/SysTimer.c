#include <string.h>

#include "SysTimer.h"

#include "SysLock.h"
#include "SysSched.h"


SysTimer gSysTimer[SYSTIMER_TIMER_COUNT];
SysTimer* gTimerChain = NULL;

static SysSem gTimerMutex;


int initSysTimer(void)
{
	memset(gSysTimer, 0, sizeof(SysTimer) * SYSTIMER_TIMER_COUNT);

	if(initMutex(&gTimerMutex))
		return -1;

	return 0;
}

int openTimer(unsigned timeoutMs, uint32_t event, SysTimer** timerid)
{
	unsigned i;

	SysTimer* timer;
	SysTimer* next;

	if(lockMutex(&gTimerMutex))
		return -1;

	for(i = 0; i < SYSTIMER_TIMER_COUNT; i ++)
	{
		timer = &gSysTimer[i];

		for(next = gTimerChain; next != NULL; next = next->next)
		{
			if(timer == next)
				break;
		}
		if(next)
			continue;

		timer->timeout = timeoutMs * 10;
		timer->event = event;
		timer->next = gTimerChain;

		gTimerChain = timer;

		if(timerid) *timerid = timer;

		break;
	}
	if(i >= SYSTIMER_TIMER_COUNT)
	{
		if(giveMutex(&gTimerMutex))
			return -2;

		return -3;
	}

	if(giveMutex(&gTimerMutex))
		return -4;

	return 0;
}

int joinTimer(SysTimer* timerid)
{
	if(!timerid)
		return -1;

	if(lockMutex(&gTimerMutex))
		return -2;

	gCurrentTask->nextTrig = timerid->trigChain;
	timerid->trigChain = gCurrentTask;

	if(giveMutex(&gTimerMutex))
		return -3;

	return 0;
}

int closeTimer(SysTimer* timerid)
{
	SysTimer* prev;
	SysTimer* next;

	if(!timerid)
		return -1;

	timerid->started = false;

	if(lockMutex(&gTimerMutex))
		return -2;

	if(timerid == gTimerChain)
	{
		next = gTimerChain->next;
		gTimerChain = next;
	}
	else
	{
		for(prev = gTimerChain, next = gTimerChain->next; next != NULL; prev = next, next = next->next)
		{
			if(timerid == next)
			{
				prev->next = next->next;

				break;
			}
		}
		if(!next)
		{
			if(giveMutex(&gTimerMutex))
				return -3;

			return -4;
		}
	}

	if(giveMutex(&gTimerMutex))
		return -5;

	return 0;
}

int startTimer(SysTimer* timerid, bool singleShort)
{
	if(!timerid)
		return -1;

	timerid->singleShot = singleShort;
	timerid->tickCount = 0;

	timerid->started = true;

	return 0;
}
