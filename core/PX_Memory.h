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

px_void PX_MemoryInitialize(px_memorypool *mp,px_memory *memory);
px_void PX_MemoryClear(px_memory *memory);
px_bool PX_MemoryAlloc(px_memory *memory,px_int size);
px_bool PX_MemoryResize(px_memory *memory,px_int size);
px_bool PX_MemoryCat(px_memory *memory,const px_void *buffer,px_int size);
px_bool PX_MemoryCopy(px_memory *memory,const px_void *buffer,px_int startoffset,px_int size);
px_byte *PX_MemoryFind(px_memory *memory,const px_void *buffer,px_int size);
px_void PX_MemoryRemove(px_memory *memory,px_int start,px_int end);
px_void PX_MemoryFree(px_memory *memory);
px_byte *PX_MemoryData(px_memory *memory);


typedef struct  
{
	px_memorypool* mp;
	px_float *buffer;
	px_int size;
	px_int pointer;
}PX_CircularBuffer;

px_bool PX_CircularBufferInitialize(px_memorypool* mp, PX_CircularBuffer* pcbuffer, px_int size);
px_void PX_CircularBufferPush(PX_CircularBuffer* pcbuffer, px_float v);
px_void PX_CircularBufferAdd(PX_CircularBuffer* pcbuffer, px_int pos,px_float v);
px_void PX_CircularBufferSet(PX_CircularBuffer* pcbuffer, px_int pos,px_float v);
px_float PX_CircularBufferGet(PX_CircularBuffer* pcbuffer, px_int pos);
px_void PX_CircularBufferFree(PX_CircularBuffer* pcbuffer);
px_float PX_CircularBufferDelay(PX_CircularBuffer* pcbuffer, px_int pos);

#endif
