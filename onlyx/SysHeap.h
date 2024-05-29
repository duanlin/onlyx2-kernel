#ifndef SYSHEAP_H
#define SYSHEAP_H


#include <stddef.h>


#define SYSHEAP_ALIGN_SIZE 16


#ifdef __cplusplus
extern "C"
{
#endif


int initSysHeap(void* heap, size_t size);

void* heapAlloc(size_t size);
int heapFree(void* p);


#ifdef __cplusplus
}
#endif


#endif
