#ifndef SYSTASK_H
#define SYSTASK_H


#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#include "SynClock.h"
#include "Context.h"


#define SYSTASK_TASK_COUNT 32
#define SYSTASK_PRIORITY_COUNT 16

#define SYSTASK_EVENT_BUFF_SIZE 8


#ifdef __cplusplus
extern "C"
{
#endif


typedef enum
{
	TaskStateNone = 0,

	TaskStateReady,
	TaskStateRunning,

	TaskStateDeathing,
	TaskStateBlocking,
	TaskStateSleeping

} TaskState;

typedef struct SysTask
{
	char name[16];
	unsigned priority;

	TaskState state;
	Context context;
	signed tickCount;

	bool waitEvent;
	unsigned readline;
	unsigned writeline;
	uint32_t sysEvents[SYSTASK_EVENT_BUFF_SIZE];
	uint32_t usrEvents;

	SynTimeStamp syncStamp;

	bool sysCallBlock;

	struct SysTask* nextSchd;
	struct SysTask* nextWait;
	struct SysTask* nextSync;
	struct SysTask* nextTrig;

} SysTask;

typedef int (*TaskEntry)(void* arg);


extern SysTask gSysTask[SYSTASK_TASK_COUNT];


int initSysTask(void);

int prepareTask(TaskEntry entry, void* arg, void* stack, size_t size, Context* context);
int createTask(const char* name, const Context* context, unsigned priority, SysTask** taskid);

int taskYield(void);
int taskSleep(unsigned us);

void finishTask(void);


#ifdef __cplusplus
}
#endif


#endif
