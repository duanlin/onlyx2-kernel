#ifndef SYSEVENT_H
#define SYSEVENT_H


#include <stdbool.h>
#include <stdint.h>

#include "SysTask.h"


#define SYSEVENT_SYS_MASK 0xFF000000
#define SYSEVENT_USER_MASK 0x00FFFFFF

#define SYSEVENT_SYS_SYNC 0x80000000


#ifdef __cplusplus
extern "C"
{
#endif


int initSysEvent(void);

int postEvent(SysTask* taskid, uint32_t event);
int pollEvent(uint32_t* events);


#ifdef __cplusplus
}
#endif


#endif
