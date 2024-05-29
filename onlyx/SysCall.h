#ifndef SYSCALL_H
#define SYSCALL_H


#include <stdint.h>


#ifdef __cplusplus
extern "C"
{
#endif


typedef enum
{
	SysCallNone = 0,

	SysCallStartup,

	SysCallTakeSem,
	SysCallGiveSem,

	SysCallPostEvent,
	SysCallPollEvent,

	SysCallTaskSleep,
	SysCallTaskYield,
	SysCallTaskExit

} SysCall;


int initSysCall(void);

int sysCall(SysCall what, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4);

int enterSysCall(SysCall what, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4);
int leaveSysCall(SysCall what, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4);


#ifdef __cplusplus
}
#endif


#endif
