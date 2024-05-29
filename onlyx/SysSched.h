#ifndef SYSSCHED_H
#define SYSSCHED_H


#include "SysTask.h"


#ifdef __cplusplus
extern "C"
{
#endif


extern SysTask* gCurrentTask;
extern SysTask* gSchedtoTask;

extern SysTask* gSchedChain[SYSTASK_PRIORITY_COUNT];


int initSysSched(void);

void schedule(void);


#ifdef __cplusplus
}
#endif


#endif
