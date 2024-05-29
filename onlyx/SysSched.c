#include <stddef.h>
#include <string.h>

#include "SysSched.h"

#include "SysMain.h"


SysTask* gCurrentTask;
SysTask* gSchedtoTask;

SysTask* gSchedChain[SYSTASK_PRIORITY_COUNT];


int initSysSched(void)
{
	gCurrentTask = NULL;
	gSchedtoTask = NULL;

	memset(gSchedChain, 0, sizeof(SysTask*) * SYSTASK_PRIORITY_COUNT);

	return 0;
}

void schedule(void)
{
	unsigned i;

	SysTask* next;

	// Evaluate task to switch
	for(i = 0; i < SYSTASK_PRIORITY_COUNT; i ++)
	{
		// High priority preemption
		if(i < gCurrentTask->priority)
		{
			for(next = gSchedChain[i]; next != NULL; next = next->nextSchd)
			{
				if(next->state != TaskStateReady)
					continue;

				gCurrentTask->state = TaskStateReady;

				gSchedtoTask = next;

				return;
			}

			continue;
		}

		// Same priority rolling
		if(i == gCurrentTask->priority)
		{
			for(next = gSchedChain[i]; next != NULL; next = next->nextSchd)
			{
				if(next->state != TaskStateReady)
					continue;

				// New ready
				if(!next->tickCount)
				{
					gCurrentTask->state = TaskStateReady;

					gSchedtoTask = next;

					return;
				}

				// Rolling
				if(!gSchedtoTask)
					gSchedtoTask = next;
				else if(next->tickCount > gSchedtoTask->tickCount)
					gSchedtoTask = next;
			}
		}

		// Low priority rescue
		for(next = gSchedChain[i]; next != NULL; next = next->nextSchd)
		{
			if(next->state != TaskStateDeathing)
				continue;

			// Rescue
			gCurrentTask->state = TaskStateReady;

			gSchedtoTask = next;

			return;
		}
	}

	// No switch or idle
	switch(gCurrentTask->state)
	{
	// No switch needed
	case TaskStateRunning:
	case TaskStateReady:
		gSchedtoTask = gCurrentTask;

		return;

	default: // Idle
		gSchedtoTask = gIdleTask;
	}
}
