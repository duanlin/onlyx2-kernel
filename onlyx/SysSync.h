#ifndef SYSSYNC_H
#define SYSSYNC_H


#include "SynClock.h"
#include "SysTask.h"


#define SYSSYNC_DEPART_SYNC_COUNT 2
#define SYSSYNC_ARRIVE_SYNC_COUNT 2


#ifdef __cplusplus
extern "C"
{
#endif


typedef struct
{
	unsigned counterDiv;
	unsigned trigModulo;

	SynTimeStamp syncStamp;

	SysTask* syncChain;

} DepartSync;

typedef struct
{
	unsigned upperBarrier;
	unsigned localBarrier;

	SysTask* syncChain;

} ArriveSync;


extern DepartSync gDepartSync[SYSSYNC_DEPART_SYNC_COUNT];
extern ArriveSync gArriveSync[SYSSYNC_ARRIVE_SYNC_COUNT];


int initSysSync(void);

int joinDepartSync(DepartSync* syncid);
int quitDepartSync(DepartSync* syncid);

int joinArriveSync(ArriveSync* syncid);
int quitArriveSync(ArriveSync* syncid);
int launchArriveSync(ArriveSync* syncid);
int arriveUpperBarrier(ArriveSync* syncid, unsigned barrier);
int arriveLocalBarrier(ArriveSync* syncid, unsigned barrier);


#ifdef __cplusplus
}
#endif


#endif
