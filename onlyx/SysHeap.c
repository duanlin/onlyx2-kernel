#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "SysHeap.h"

#include "SysLock.h"


typedef union HeapBlock
{
	uint8_t header[SYSHEAP_ALIGN_SIZE];
	
	struct
	{
		bool assigned;
		
		size_t size;
		union HeapBlock* next;
	};
	
} HeapBlock;


static SysSem gHeapMutex;
static HeapBlock* gHeadBlock;
static HeapBlock* gFreeBlock;


int initSysHeap(void* heap, size_t size)
{
	gHeadBlock = (HeapBlock*)heap;
	memset(gHeadBlock, 0, sizeof(HeapBlock));
	
	gHeadBlock->size = size;
	gFreeBlock = gHeadBlock;
	
	if(initMutex(&gHeapMutex))
		return -1;
	
	return 0;
}

void* heapAlloc(size_t size)
{
	HeapBlock* next;
	HeapBlock* free;
	
	// Block header
	size += sizeof(HeapBlock);
	
	// Alignment
	if(size % SYSHEAP_ALIGN_SIZE)
		size += SYSHEAP_ALIGN_SIZE - size % SYSHEAP_ALIGN_SIZE;
	
	if(lockMutex(&gHeapMutex))
		return NULL;
	
	// Search free
	free = gFreeBlock; while(!free->assigned)
	{
		// Merge fragments
		next = free->next; while(next != NULL)
		{
			if(!next->assigned)
			{
				free->next = next->next;
				free->size += next->size;
				
				next = free->next;
			}
			else
				break;
		}
		
		// Enough
		if(free->size > size)
		{
			gFreeBlock = (HeapBlock*)((uint8_t*)free + size);
			memset(gFreeBlock, 0, sizeof(HeapBlock));
			
			gFreeBlock->size = free->size - size;
			gFreeBlock->next = free->next;
			
			free->size = size;
			free->assigned = true;
			free->next = gFreeBlock;
			
			giveMutex(&gHeapMutex);
			
			return (uint8_t*)free + sizeof(HeapBlock);
		}
		// Just fit
		else if(free->size == size)
		{
			if(free->next != NULL)
				gFreeBlock = free->next;
			else
				gFreeBlock = gHeadBlock;
			
			free->assigned = true;
			
			giveMutex(&gHeapMutex);
			
			return (uint8_t*)free + sizeof(HeapBlock);
		}
		
		// Search next
		if(free->next != NULL)
			free = free->next;
		else
			free = gHeadBlock;
		
		// Not found
		if(free == gFreeBlock)
			break;
	}
	
	giveMutex(&gHeapMutex);
	
	return NULL;
}

int heapFree(void* p)
{
	HeapBlock* free;
	HeapBlock* next;
	
	free = (HeapBlock*)((uint8_t*)p - sizeof(HeapBlock));
	
	if(lockMutex(&gHeapMutex))
		return -1;
	
	free->assigned = false;
	
	// Merge fragments
	next = free->next; while(next != NULL)
	{
		if(!next->assigned)
		{
			free->next = next->next;
			free->size += next->size;
			
			next = free->next;
		}
		else
			break;
	}
	
	giveMutex(&gHeapMutex);
	
	return 0;
}
