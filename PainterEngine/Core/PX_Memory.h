#ifndef __PX_MEMORY_H
#define __PX_MEMORY_H

#include "PX_MemoryPool.h"

typedef struct __PX_memroy
{
	px_byte *buffer;
	px_memorypool *mp;
	px_int  usedsize;
	px_int  allocsize;
}px_memory;

px_void PX_MemoryInit(px_memorypool *mp,px_memory *memory);
px_void PX_MemoryClear(px_memory *memory);
px_bool PX_MemoryAlloc(px_memory *memory,px_int size);
px_bool PX_MemoryResize(px_memory *memory,px_int size);
px_bool PX_MemoryCat(px_memory *memory,px_void *buffer,px_int size);
px_bool PX_MemoryCopy(px_memory *memory,px_void *buffer,px_int startoffset,px_int size);
px_byte *PX_MemoryFine(px_memory *memory,px_void *buffer,px_int size);
px_void PX_MemoryFree(px_memory *memory);
px_byte *PX_MemoryData(px_memory *memory);

#endif
