#include <string.h>

#include "SysEvent.h"

#include "SysSched.h"
#include "SysCall.h"


int initSysEvent(void)
{
	return 0;
}

int postEvent(SysTask* taskid, uint32_t event)
{
	if(event & ~SYSEVENT_USER_MASK)
		return -1;

	if(sysCall(SysCallPostEvent, (uintptr_t)taskid, event, 0, 0))
		return -2;

	return 0;
}

int pollEvent(uint32_t* events)
{
	return sysCall(SysCallPollEvent, (uintptr_t)events, 0, 0, 0);
}
