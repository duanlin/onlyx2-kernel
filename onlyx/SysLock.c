#include <string.h>

#include "SysLock.h"

#include "SysCall.h"


int initSysLock(void)
{
	return 0;
}

int initMutex(SysSem* sem)
{
	memset(sem, 0, sizeof(SysSem));
	sem->binary = true;
	sem->count = 1;
	
	return 0;
}

int lockMutex(SysSem* sem)
{
	if(!sem)
		return -1;
	if(!sem->binary)
		return -2;
	
	if(sysCall(SysCallTakeSem, (uintptr_t)sem, 0, 0, 0))
		return -3;
	
	return 0;
}

int giveMutex(SysSem* sem)
{
	if(!sem)
		return -1;
	if(!sem->binary)
		return -2;

	if(sysCall(SysCallGiveSem, (uintptr_t)sem, 0, 0, 0))
		return -3;

	return 0;
}

int initSemaphore(SysSem* sem)
{
	memset(sem, 0, sizeof(SysSem));
	sem->binary = false;
	sem->count = 0;
	
	return 0;
}

int takeSemaphore(SysSem* sem)
{
	if(!sem)
		return -1;
	
	if(sysCall(SysCallTakeSem, (uintptr_t)sem, 0, 0, 0))
		return -2;

	return 0;
}

int giveSemaphore(SysSem* sem)
{
	if(!sem)
		return -1;
	
	if(sysCall(SysCallGiveSem, (uintptr_t)sem, 0, 0, 0))
		return -2;

	return 0;
}
