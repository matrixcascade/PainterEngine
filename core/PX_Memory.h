#ifndef __PX_MEMORY_H
#define __PX_MEMORY_H

#include "PX_MemoryPool.h"

typedef struct __PX_memory
{
	px_byte *buffer;
	px_memorypool *mp;
	px_int  usedsize;
	px_int  allocsize;
	px_int  bit_pointer;
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
px_bool PX_MemoryCatBit(px_memory* memory, px_bool b);
px_bool PX_MemoryCatBits(px_memory* memory, px_byte data[],px_int bit_count);
px_void PX_MemoryLeft(px_memory* memory, px_int trimsize);
typedef struct  
{
	px_memorypool* mp;
	px_double *buffer;
	px_int size;
	px_int pointer;
}PX_CircularBuffer;

px_bool PX_CircularBufferInitialize(px_memorypool* mp, PX_CircularBuffer* pcbuffer, px_int size);
px_void PX_CircularBufferPush(PX_CircularBuffer* pcbuffer, px_double v);
px_void PX_CircularBufferAdd(PX_CircularBuffer* pcbuffer, px_int pos,px_double v);
px_void PX_CircularBufferSet(PX_CircularBuffer* pcbuffer, px_int pos,px_double v);
px_double PX_CircularBufferGet(PX_CircularBuffer* pcbuffer, px_int pos);
px_void PX_CircularBufferZeroClear(PX_CircularBuffer* pcbuffer);
px_void PX_CircularBufferFree(PX_CircularBuffer* pcbuffer);
px_double PX_CircularBufferDelay(PX_CircularBuffer* pcbuffer, px_int pos);


typedef px_memory px_fifobuffer;
px_void PX_FifoBufferInitialize(px_memorypool* mp, px_fifobuffer* pfifo);
px_int PX_FifoBufferPop(px_fifobuffer* pfifo, px_void* data, px_int size);
px_bool PX_FifoBufferPush(px_fifobuffer* pfifo, px_void* data, px_int size);
px_int PX_FifoBufferGetPopSize(px_fifobuffer* pfifo);
px_void PX_FifoBufferFree(px_fifobuffer* pfifo);

typedef px_memory px_stack;
px_void PX_StackInitialize(px_memorypool* mp, px_stack* pstack);
px_int PX_StackPop(px_stack* pstack, px_void* data, px_int size);
px_bool PX_StackPush(px_stack* pstack, px_void* data, px_int size);
px_int PX_StackGetPopSize(px_stack* pstack);
px_void PX_StackFree(px_stack* pstack);
#endif
