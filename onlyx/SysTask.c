#include <string.h>

#include "SysTask.h"

#include "SysLock.h"
#include "SysCall.h"
#include "SysSched.h"


SysTask gSysTask[SYSTASK_TASK_COUNT];

static SysSem gTaskMutex;


int initSysTask(void)
{
	memset(gSysTask, 0, sizeof(SysTask) * SYSTASK_TASK_COUNT);

	if(initMutex(&gTaskMutex))
		return -1;

	return 0;
}

int createTask(const char* name, const Context* context, unsigned priority, SysTask** taskid)
{
	unsigned i;

	SysTask* task;

	if(!name || !context)
		return -1;
	if(priority >= SYSTASK_PRIORITY_COUNT)
		return -2;

	if(lockMutex(&gTaskMutex))
		return -3;

	for(i = 0; i < SYSTASK_TASK_COUNT; i ++)
	{
		task = &gSysTask[i];

		if(task->state == TaskStateNone)
		{
			strcpy(task->name, name);
			task->priority = priority;
			task->context = *context;

			// Insert to sched chain
			task->nextSchd = gSchedChain[priority];
			gSchedChain[priority] = task;

			task->readline = task->writeline;

			task->tickCount = 0;
			task->state = TaskStateReady;

			if(taskid) *taskid = task;

			break;
		}
	}
	if(i >= SYSTASK_TASK_COUNT)
		return -4;

	if(giveMutex(&gTaskMutex))
		return -5;

	return 0;
}

int taskYield(void)
{
	if(sysCall(SysCallTaskYield, 0, 0, 0, 0))
		return -1;

	return 0;
}

int taskSleep(unsigned us)
{
	unsigned tick;

	tick = us / 100;
	if(sysCall(SysCallTaskSleep, tick, 0, 0, 0))
		return -1;

	return 0;
}
