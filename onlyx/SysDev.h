#ifndef SYSDEV_H
#define SYSDEV_H


#include <stdint.h>

#include "SysTask.h"


#define SYSDEV_DEV_COUNT 8


#ifdef __cplusplus
extern "C"
{
#endif


typedef int (*DevInspect)(void* arg, uint32_t* event);
typedef int (*DevHandler)(void* arg, uint32_t* event);

typedef struct SysDev
{
	char name[16];
	unsigned irq;
	
	DevInspect inspect;
	DevHandler handler;
	void* arg;

	SysTask* waitChain;

} SysDev;


extern unsigned gDevCount;
extern SysDev gSysDev[SYSDEV_DEV_COUNT];


int initSysDev(void);

int createDev(const char* name, unsigned irq, SysDev** devid);

int setDevHandler(SysDev* devid, DevInspect inspect, DevHandler handler, void* arg);

int startDevService(SysDev* devid, SysTask* taskid);
int ceaseDevService(SysDev* devid, SysTask* taskid);


#ifdef __cplusplus
}
#endif


#endif
