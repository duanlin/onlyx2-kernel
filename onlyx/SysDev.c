#include <string.h>

#include "SysDev.h"

#include "SysLock.h"
#include "SysCall.h"


unsigned gDevCount = 0;
SysDev gSysDev[SYSDEV_DEV_COUNT];

static SysSem gDevMutex;


int initSysDev(void)
{
	memset(gSysDev, 0, sizeof(SysDev) * SYSDEV_DEV_COUNT);

	if(initMutex(&gDevMutex))
		return -1;

	return 0;
}

int createDev(const char* name, unsigned irq, SysDev** devid)
{
	SysDev* dev;

	if(lockMutex(&gDevMutex))
		return -1;

	if(gDevCount < SYSDEV_DEV_COUNT)
	{
		dev = &gSysDev[gDevCount];

		strcpy(dev->name, name);
		dev->irq = irq;

		if(devid) *devid = dev;

		gDevCount ++;
	}
	else
	{
		if(giveMutex(&gDevMutex))
			return -2;

		return -3;
	}

	if(giveMutex(&gDevMutex))
		return -4;

	return 0;
}

int setDevHandler(SysDev* devid, DevInspect inspect, DevHandler handler, void* arg)
{
	devid->arg = arg;
	devid->inspect = inspect;
	devid->handler = handler;

	return 0;
}

int startDevService(SysDev* devid, SysTask* taskid)
{
	taskid->nextWait = devid->waitChain;
	devid->waitChain = taskid;

	return 0;
}

int ceaseDevService(SysDev* devid, SysTask* taskid)
{
	SysTask* prev;
	SysTask* next;

	if(!devid->waitChain)
		return -1;

	if(lockMutex(&gDevMutex))
		return -2;

	if(devid->waitChain == taskid)
	{
		next = devid->waitChain->nextWait;
		devid->waitChain = next;
	}
	else for(prev = devid->waitChain, next = devid->waitChain->nextWait; next != NULL; prev = next, next = next->nextWait)
	{
		if(next == taskid)
		{
			prev->nextWait = next->nextWait;

			break;
		}
	}

	if(giveMutex(&gDevMutex))
		return -3;

	return 0;
}

