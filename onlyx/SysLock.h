#ifndef SYSLOCK_H
#define SYSLOCK_H


#include <stdbool.h>
#include <stdint.h>

#include "SysTask.h"


#ifdef __cplusplus
extern "C"
{
#endif


typedef struct
{
	bool binary;
	unsigned count;
	
	SysTask* waitChain;

} SysSem;


int initSysLock(void);

int initMutex(SysSem* sem);
int lockMutex(SysSem* sem);
int giveMutex(SysSem* sem);

int initSemaphore(SysSem* sem);
int takeSemaphore(SysSem* sem);
int giveSemaphore(SysSem* sem);


#ifdef __cplusplus
}
#endif


#endif
