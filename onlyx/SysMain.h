#ifndef SYSMAIN_H
#define SYSMAIN_H


#include "SysTask.h"


#define SYSMAIN_TASK_STACK_SIZE (1024 * 8)
#define SYSMAIN_TASK_PRIORITY 13

#define SYSMAIN_IDLE_STACK_SIZE 512


#ifdef __cplusplus
extern "C"
{
#endif


extern SysTask* gIdleTask;


int initSysMain(void);

int mainStartup(void);
int tickStartup(void);


#ifdef __cplusplus
}
#endif


#endif
