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
static HeapBlock* gFreeBlock;


int initSysHeap(void* heap, size_t size)
{
	gFreeBlock = (HeapBlock*)heap;
	memset(gFreeBlock, 0, sizeof(HeapBlock));
	
	gFreeBlock->size = size - sizeof(HeapBlock);
	gFreeBlock->next = gFreeBlock;
	
	if(initMutex(&gHeapMutex))
		return 1;
	
	return 0;
}

void* heapAlloc(size_t size)
{
	HeapBlock* block;
	HeapBlock* free;
	HeapBlock* next;
	
	if(!size)
		return NULL;

	if(lockMutex(&gHeapMutex))
		return NULL;
	
	// Merge next free
	next = gFreeBlock->next;
	if(!next->assigned && (next != gFreeBlock))
	{
		gFreeBlock->next = next->next;
		gFreeBlock->size = sizeof(HeapBlock) + next->size;
	}

	// Alignment
	if(size < SYSHEAP_ALIGN_SIZE)
		size = SYSHEAP_ALIGN_SIZE;
	else if(size % SYSHEAP_ALIGN_SIZE)
		size += SYSHEAP_ALIGN_SIZE - size % SYSHEAP_ALIGN_SIZE;

	// Suitable
	if(gFreeBlock->size >= size + sizeof(HeapBlock))
	{
		// Prepare next free
		free = (HeapBlock*)((uint8_t*)gFreeBlock + sizeof(HeapBlock) + size);
		free->assigned = false;
		free->next = gFreeBlock->next;
		free->size = gFreeBlock->size - size - sizeof(HeapBlock);

		// Assignment
		block = gFreeBlock;
		gFreeBlock = free;

		block->size = size;
		block->next = free;
		block->assigned = true;

		giveMutex(&gHeapMutex);
		
		return (uint8_t*)block + sizeof(HeapBlock);
	}
	// Search free
	else
	{
		block = (HeapBlock*)((uint8_t*)gFreeBlock + sizeof(HeapBlock) + size);

		while(block != gFreeBlock)
		{
			if(block->assigned)
			{
				block = block->next;

				continue;
			}

			if(block->size >= size + sizeof(HeapBlock))
			{
				// Prepare next free
				free = (HeapBlock*)((uint8_t*)block + sizeof(HeapBlock) + size);
				free->assigned = false;
				free->next = block->next;
				free->size = block->size - size - sizeof(HeapBlock);

				// Assignment
				gFreeBlock = free;

				block->size = size;
				block->next = free;
				block->assigned = true;
				
				giveMutex(&gHeapMutex);

				return (uint8_t*)block + sizeof(HeapBlock);
			}

			block = block->next;
		}
	}
	
	giveMutex(&gHeapMutex);
	
	return NULL;
}

int heapFree(void* p)
{
	HeapBlock* block;
	HeapBlock* next;

	if(!p)
		return -1;

	block = (HeapBlock*)((uint8_t*)p - sizeof(HeapBlock));
	
	if(lockMutex(&gHeapMutex))
		return -2;
	
	block->assigned = 0;

	// Merge next free
	next = block->next;
	if(!next->assigned && (next != block))
	{
		block->next = next->next;
		block->size = sizeof(HeapBlock) + next->size;
	}

	if(next == gFreeBlock)
		gFreeBlock = block;
	
	giveMutex(&gHeapMutex);

	return 0;
}
