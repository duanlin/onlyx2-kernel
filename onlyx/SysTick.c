#include "SysTick.h"

#include "SysDev.h"
#include "SysTask.h"
#include "SysSync.h"
#include "SysSched.h"
#include "SysEvent.h"
#include "SysTimer.h"


int initSysTick(void)
{
	return 0;
}

int handleSysTick(unsigned counter, SynTimeStamp stamp)
{
	unsigned i;

	SysTask* next;
	bool needSched;

	SysDev* dev;
	unsigned nextline;
	uint32_t events;

	SysTimer* timer;

	unsigned highestPriority;
	unsigned runningPriority;

	highestPriority = SYSTASK_PRIORITY_COUNT;
	runningPriority = SYSTASK_PRIORITY_COUNT;

	needSched = false;

	// Sync event
	for(i = 0; i < SYSSYNC_DEPART_SYNC_COUNT; i ++)
	{
		if(!gDepartSync[i].counterDiv)
			continue;

		if(!gDepartSync[i].syncChain)
			continue;

		if(counter % gDepartSync[i].counterDiv != gDepartSync[i].trigModulo)
			continue;

		gDepartSync[i].syncStamp = stamp;

		for(next = gDepartSync[i].syncChain; next != NULL; next = next->nextSync)
		{
			nextline = (next->writeline + 1) % SYSTASK_EVENT_BUFF_SIZE;
			if(nextline != next->readline)
			{
				next->syncStamp = stamp;

				next->sysEvents[next->writeline] = SYSEVENT_SYS_SYNC;
				next->writeline = nextline;

				if(next->waitEvent)
				{
					next->state = TaskStateReady;

					needSched = true;
				}
			}
		}
	}

	// Device inspect
	for(i = 0; i < gDevCount; i ++)
	{
		dev = &gSysDev[i];

		if(!dev->inspect)
			continue;

		for(next = dev->waitChain; next != NULL; next = next->nextWait)
		{
			nextline = (next->writeline + 1) % SYSTASK_EVENT_BUFF_SIZE;
			if(nextline != next->readline)
			{
				if(!dev->inspect(dev->arg, &events))
				{
					next->sysEvents[next->writeline] = events;
					next->writeline = nextline;

					if(next->waitEvent)
					{
						next->state = TaskStateReady;

						needSched = true;
					}
				}
			}
		}
	}

	// Tick count
	for(i = 0; i < SYSTASK_PRIORITY_COUNT; i ++)
	{
		for(next = gSchedChain[i]; next != NULL; next = next->nextSchd)
		{
			switch(next->state)
			{
			case TaskStateRunning:
				next->tickCount ++;

				if(next->tickCount >= SYSTICK_TICK_TO_SCHEDULE)
				{
					next->state = TaskStateReady;

					needSched = true;
				}

				runningPriority = next->priority;

				break;

			case TaskStateReady:
				next->tickCount ++;

				if(next->tickCount >= SYSTICK_TICK_TO_DEATHING)
				{
					next->state = TaskStateDeathing;

					needSched = true;
				}

				if(next->priority < highestPriority)
					highestPriority = next->priority;

				break;

			case TaskStateSleeping:
				next->tickCount ++;

				if(next->tickCount >= 0)
				{
					next->state = TaskStateReady;

					needSched = true;
				}

				break;

			default:
				break;
			}
		}
	}

	// Timer event
	for(timer = gTimerChain; timer != NULL; timer = timer->next)
	{
		if(!timer->started)
			continue;

		timer->tickCount ++;

		if(timer->tickCount >= timer->timeout)
		{
			for(next = timer->trigChain; next != NULL; next = next->nextTrig)
			{
				nextline = (next->writeline + 1) % SYSTASK_EVENT_BUFF_SIZE;
				if(nextline != next->readline)
				{
					next->sysEvents[next->writeline] = timer->event;
					next->writeline = nextline;

					if(next->waitEvent)
					{
						next->state = TaskStateReady;

						needSched = true;
					}
				}
			}

			timer->tickCount = 0;

			if(timer->singleShot)
				timer->started = false;
		}
	}

	if(highestPriority < runningPriority)
		needSched = true;

	if(needSched)
		schedule();

	return 0;
}
