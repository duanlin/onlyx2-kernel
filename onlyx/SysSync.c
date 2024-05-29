#include <string.h>

#include "SysSync.h"

#include "SysLock.h"
#include "SysCall.h"
#include "SysSched.h"


DepartSync gDepartSync[SYSSYNC_DEPART_SYNC_COUNT];
ArriveSync gArriveSync[SYSSYNC_ARRIVE_SYNC_COUNT];

static SysSem gDepartMutex;
static SysSem gArriveMutex;


int initSysSync(void)
{
	memset(gDepartSync, 0, sizeof(DepartSync) * SYSSYNC_DEPART_SYNC_COUNT);
	memset(gArriveSync, 0, sizeof(ArriveSync) * SYSSYNC_ARRIVE_SYNC_COUNT);

	if(initMutex(&gDepartMutex))
		return -1;
	if(initMutex(&gArriveMutex))
		return -2;

	return 0;
}

int joinDepartSync(DepartSync* syncid)
{
	if(lockMutex(&gDepartMutex))
		return -1;

	gCurrentTask->nextSync = syncid->syncChain;
	syncid->syncChain = gCurrentTask;

	if(giveMutex(&gDepartMutex))
		return -2;

	return 0;
}

int quitDepartSync(DepartSync* syncid)
{
	SysTask* prev;
	SysTask* next;

	if(!syncid->syncChain)
		return -1;

	if(lockMutex(&gDepartMutex))
		return -2;

	if(syncid->syncChain == gCurrentTask)
	{
		next = syncid->syncChain->nextSync;
		syncid->syncChain = next;
	}
	else for(prev = syncid->syncChain, next = syncid->syncChain->nextSync; next != NULL; prev = next, next = next->nextSync)
	{
		if(next == gCurrentTask)
		{
			prev->nextSync = next->nextSync;

			break;
		}
	}

	if(giveMutex(&gDepartMutex))
		return -3;

	return 0;
}

int joinArriveSync(ArriveSync* syncid)
{
	if(lockMutex(&gArriveMutex))
		return -1;

	gCurrentTask->nextSync = syncid->syncChain;
	syncid->syncChain = gCurrentTask;

	if(giveMutex(&gArriveMutex))
		return -2;

	return 0;
}

int quitArriveSync(ArriveSync* syncid)
{
	SysTask* prev;
	SysTask* next;

	if(!syncid->syncChain)
		return -1;

	if(lockMutex(&gArriveMutex))
		return -2;

	if(syncid->syncChain == gCurrentTask)
	{
		next = syncid->syncChain->nextSync;
		syncid->syncChain = next;
	}
	else for(prev = syncid->syncChain, next = syncid->syncChain->nextSync; next != NULL; prev = next, next = next->nextSync)
	{
		if(next == gCurrentTask)
		{
			prev->nextSync = next->nextSync;

			break;
		}
	}

	if(giveMutex(&gArriveMutex))
		return -3;

	return 0;
}

int launchArriveSync(ArriveSync* syncid)
{
	return 0;
}

int arriveUpperBarrier(ArriveSync* syncid, unsigned barrier)
{
	// TODO syscall

	return 0;
}

int arriveLocalBarrier(ArriveSync* syncid, unsigned barrier)
{
	// TODO syscall

	return 0;
}
