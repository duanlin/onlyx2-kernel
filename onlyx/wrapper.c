#include <stddef.h>
#include <reent.h>

#include "SysHeap.h"


void* __wrap_malloc(size_t size)
{
	return heapAlloc(size);
}

void __wrap_free(void* p)
{
	heapFree(p);
}
