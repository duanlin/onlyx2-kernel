#include <stdbool.h>
#include <stddef.h>
#include <malloc.h>

#include "SysMain.h"

#include "Context.h"
#include "SysHeap.h"
#include "SysTask.h"
#include "SysCall.h"


extern int main();


SysTask* gIdleTask = NULL;


static int idleEntry(void* arg)
{
	(void)arg;

	while(true)
		taskYield();

	return 0;
}

static int mainEntry(void* arg)
{
	(void)arg;

	return main();
}

int initSysMain(void)
{
	void* stack;
	Context context;

	stack = malloc(SYSMAIN_IDLE_STACK_SIZE);
	if(!stack)
		return -1;
	if(prepareTask(idleEntry, NULL, stack, SYSMAIN_IDLE_STACK_SIZE, &context))
		return -2;
	if(createTask("idle", &context, SYSTASK_PRIORITY_COUNT - 1, &gIdleTask))
		return -3;

	stack = malloc(SYSMAIN_TASK_STACK_SIZE);
	if(!stack)
		return -4;
	if(prepareTask(mainEntry, NULL, stack, SYSMAIN_TASK_STACK_SIZE, &context))
		return -5;
	if(createTask("main", &context, SYSMAIN_TASK_PRIORITY, NULL))
		return -6;

	return 0;
}

int mainStartup(void)
{
	if(sysCall(SysCallStartup, 0, 0, 0, 0))
		return -1;

	return 0;
}
