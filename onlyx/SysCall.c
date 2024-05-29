#include <string.h>

#include "SysCall.h"

#include "SysDev.h"
#include "SysLock.h"
#include "SysMain.h"
#include "SysEvent.h"
#include "SysSched.h"


int initSysCall(void)
{
	return 0;
}

int enterSysCall(SysCall what, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4)
{
	SysTask* task;
	SysTask* prev;
	SysTask* next;
	SysSem* sem;

	uint32_t event;
	uint32_t* events;

	unsigned tick;

	switch(what)
	{
	case SysCallNone: break;

	case SysCallStartup:
		if(gCurrentTask || !gIdleTask)
			return -999;

		gCurrentTask = gIdleTask;

		schedule();

		if(tickStartup())
			return -999;

		break;

	case SysCallTakeSem:
		sem = (SysSem*)arg1;

		// Block caller
		if(!sem->count)
		{
			gCurrentTask->state = TaskStateBlocking;

			// Insert to wait chain with priority
			if(!sem->waitChain)
			{
				gCurrentTask->nextWait = NULL;
				sem->waitChain = gCurrentTask;
			}
			else if(gCurrentTask->priority > sem->waitChain->priority)
			{
				gCurrentTask->nextWait = sem->waitChain;
				sem->waitChain = gCurrentTask;
			}
			else
			{
				for(prev = sem->waitChain, next = sem->waitChain->nextWait; next != NULL; prev = next, next = next->nextWait)
				{
					if(gCurrentTask->priority > next->priority)
					{
						gCurrentTask->nextWait = next;
						prev->nextWait = gCurrentTask;

						break;
					}
				}
				if(!next)
				{
					gCurrentTask->nextWait = NULL;
					prev->nextWait = gCurrentTask;
				}
			}

			gCurrentTask->sysCallBlock = true;
			gCurrentTask = gIdleTask;

			schedule();
		}
		// OK take it
		else
			sem->count --;

		break;

	case SysCallGiveSem:
		sem = (SysSem*)arg1;

		if(sem->binary)
			sem->count = 1;
		else
			sem->count ++;

		// Unblock waiter with priority
		if(sem->waitChain)
		{
			sem->waitChain->state = TaskStateReady;
			sem->waitChain = sem->waitChain->nextWait;

			schedule();
		}

		break;

	case SysCallPostEvent:
		task = (SysTask*)arg1;
		event = arg2;

		task->usrEvents |= event;

		// Unblock task if waiting
		if(task->waitEvent)
		{
			task->waitEvent = false;
			task->state = TaskStateReady;

			schedule();
		}

		break;

	case SysCallPollEvent:
		events = (uint32_t*)arg1;

		*events = 0;

		if((gCurrentTask->readline != gCurrentTask->writeline) || gCurrentTask->usrEvents)
		{
			// Sys events
			while(gCurrentTask->readline != gCurrentTask->writeline)
			{
				*events |= gCurrentTask->sysEvents[gCurrentTask->readline];

				gCurrentTask->readline = (gCurrentTask->readline + 1) % SYSTASK_EVENT_BUFF_SIZE;
			}

			// Usr events
			*events |= gCurrentTask->usrEvents;
			gCurrentTask->usrEvents = 0;
		}
		else
		{
			gCurrentTask->waitEvent = true;
			gCurrentTask->sysCallBlock = true;

			gCurrentTask->state = TaskStateBlocking;

			gCurrentTask = gIdleTask;

			schedule();
		}

		break;

	case SysCallTaskSleep:
		tick = arg1;

		gCurrentTask->tickCount = -tick;
		gCurrentTask->state = TaskStateSleeping;

		gCurrentTask = gIdleTask;

		schedule();

		break;

	case SysCallTaskYield:
		gCurrentTask->state = TaskStateReady;

		schedule();

		break;

	case SysCallTaskExit:
		if(gSchedChain[gCurrentTask->priority] == gCurrentTask)
		{
			next = gSchedChain[gCurrentTask->priority]->nextSchd;
			gSchedChain[gCurrentTask->priority] = next;

			gCurrentTask->state = TaskStateNone;
		}
		else for(prev = gSchedChain[gCurrentTask->priority], next = gSchedChain[gCurrentTask->priority]->nextSchd; next != NULL; next = next->nextSchd)
		{
			if(next == gCurrentTask)
			{
				prev->nextSchd = next->nextSchd;

				gCurrentTask->state = TaskStateNone;

				break;
			}
		}

		gCurrentTask = gIdleTask;

		schedule();

		break;

	default:
		return -999;
	}
	
	return 0;
}

int leaveSysCall(SysCall what, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4)
{
	SysSem* sem;

	uint32_t* events;
	unsigned writeline;

	switch(what)
	{
	case SysCallTakeSem:
		sem = (SysSem*)arg1;

		// Take it
		if(sem->count)
			sem->count --;
		else
			return -1;

		break;

	case SysCallPollEvent:
		events = (uint32_t*)arg1;

		// System events
		writeline = gCurrentTask->writeline;
		while(gCurrentTask->readline != writeline)
		{
			*events |= gCurrentTask->sysEvents[gCurrentTask->readline];

			gCurrentTask->readline = (gCurrentTask->readline + 1) % SYSTASK_EVENT_BUFF_SIZE;
		}

		// User events
		*events |= gCurrentTask->usrEvents;
		gCurrentTask->usrEvents = 0;

		break;

	default:
		return -2;
	}

	return 0;
}
