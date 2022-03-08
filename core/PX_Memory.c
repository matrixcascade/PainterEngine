#include "PX_Memory.h"
px_void PX_MemoryInitialize(px_memorypool *mp,px_memory *memory)
{
	memory->buffer=PX_NULL;
	memory->allocsize=0;
	memory->usedsize=0;
	memory->mp=mp;
}

px_bool PX_MemoryCat(px_memory *memory,const px_void *buffer,px_int size)
{
	px_byte *old;
	px_int length,shl;

	if (size==0)
	{
		return PX_TRUE;
	}

	if (memory->usedsize+size>memory->allocsize)
	{
		shl=0;
		old=memory->buffer;
		length=memory->usedsize+size;
		while ((px_int)(1<<++shl)<=length);
		memory->allocsize=(1<<shl);
		memory->buffer=(px_byte*)MP_Malloc(memory->mp,memory->allocsize);
		if(!memory->buffer) return PX_FALSE;
		if(old)
		PX_memcpy(memory->buffer,old,memory->usedsize);

		PX_memcpy(memory->buffer+memory->usedsize,buffer,size);

		if(old)
		MP_Free(memory->mp,old);
	}
	else
	{
		PX_memcpy(memory->buffer+memory->usedsize,buffer,size);
	}
	memory->usedsize+=size;
	return PX_TRUE;
}

px_void PX_MemoryFree(px_memory *memory)
{
	if (memory->allocsize==0||memory->buffer==PX_NULL)
	{
		return;
	}
	MP_Free(memory->mp,memory->buffer);
	memory->buffer=PX_NULL;
	memory->usedsize=0;
	memory->allocsize=0;
}

px_byte * PX_MemoryData(px_memory *memory)
{
	return memory->buffer;
}

px_bool PX_MemoryAlloc(px_memory *memory,px_int size)
{
	PX_MemoryFree(memory);
	memory->allocsize=size;
	memory->usedsize=0;
	if (size==0)
	{
		memory->buffer=PX_NULL;
		return PX_TRUE;
	}
	else
	{
		return (memory->buffer=(px_byte *)MP_Malloc(memory->mp,size))!=0;
	}
	
}

px_bool PX_MemoryResize(px_memory *memory,px_int size)
{
	return PX_MemoryAlloc(memory,size);
}


px_byte *PX_MemoryFind(px_memory *memory,const px_void *buffer,px_int size)
{
	px_int offest;
	if (memory->usedsize<size)
	{
		return PX_NULL;
	}
	for (offest=0;offest<memory->usedsize-size+1;offest++)
	{
		if (PX_memequ(memory->buffer+offest,buffer,size))
		{
			return (memory->buffer+offest);
		}
	}
	return PX_NULL;
}


px_void PX_MemoryRemove(px_memory *memory,px_int start,px_int end)
{
	if (start>end)
	{
		px_int t = end;
		end = start;
		start = t;
	}
	if (start<0)
	{
		PX_ASSERT();
		return;
	}
	if (end >= memory->usedsize)
	{
		PX_ASSERT();
		return;
	}
	PX_memcpy(memory->buffer+start,memory->buffer+end+1,memory->usedsize-end-1);
	memory->usedsize += (start - end - 1);
}


px_void PX_MemoryClear(px_memory *memory)
{
	memory->usedsize=0;
}

px_bool PX_MemoryCopy(px_memory *memory,const px_void *buffer,px_int startoffset,px_int size)
{
	px_byte *old;
	px_int length,shl;

	if (startoffset+size>memory->allocsize)
	{
		shl=0;
		old=memory->buffer;
		length=startoffset+size;
		while ((px_int)(1<<++shl)<=length);
		memory->allocsize=(1<<shl);
		memory->buffer=(px_byte*)MP_Malloc(memory->mp,memory->allocsize);
		if (!memory->buffer)
		{
			MP_Free(memory->mp,old);
			return PX_FALSE;
		}
		if(old)
			PX_memcpy(memory->buffer,old,memory->usedsize);

		PX_memcpy(memory->buffer+startoffset,buffer,size);

		if(old)
			MP_Free(memory->mp,old);

		memory->usedsize=startoffset+size;
	}
	else
	{
		if (startoffset+size>memory->usedsize)
		{
			memory->usedsize=startoffset+size;
		}

		PX_memcpy(memory->buffer+startoffset,buffer,size);

	}
	return PX_TRUE;
}

px_bool PX_CircularBufferInitialize(px_memorypool* mp, PX_CircularBuffer* pcbuffer, px_int size)
{
	PX_memset(pcbuffer, 0, sizeof(PX_CircularBuffer));
	pcbuffer->buffer = (px_float *)MP_Malloc(mp, sizeof(px_float) * size);
	if (!pcbuffer)
	{
		return PX_FALSE;
	}
	PX_memset(pcbuffer->buffer, 0, sizeof(px_float) * size);
	pcbuffer->mp = mp;
	pcbuffer->pointer = 0;
	pcbuffer->size = size;
	return PX_TRUE;
}

px_void PX_CircularBufferPush(PX_CircularBuffer* pcbuffer, px_float v)
{
	pcbuffer->pointer--;
	if (pcbuffer->pointer<0)
	{
		pcbuffer->pointer = pcbuffer->size - 1;
	}
	pcbuffer->buffer[pcbuffer->pointer] = v;
}
px_void PX_CircularBufferAdd(PX_CircularBuffer* pcbuffer, px_int pos, px_float v)
{
	pos = pos % pcbuffer->size;
	if (pos < 0) { pos += pcbuffer->size; }
	pcbuffer->buffer[pos] += v;
}
px_void PX_CircularBufferSet(PX_CircularBuffer* pcbuffer, px_int pos, px_float v)
{
	pos = pos % pcbuffer->size;
	if (pos < 0) { pos += pcbuffer->size; }
	pcbuffer->buffer[pos] = v;
}
px_float  PX_CircularBufferGet(PX_CircularBuffer* pcbuffer, px_int pos)
{
	pos = pos % pcbuffer->size;
	if (pos < 0) { pos += pcbuffer->size; }
	return pcbuffer->buffer[pos];
}

px_void PX_CircularBufferFree(PX_CircularBuffer* pcbuffer)
{
	if (pcbuffer->mp)
	{
		MP_Free(pcbuffer->mp, pcbuffer->buffer);
		PX_memset(pcbuffer, 0, sizeof(PX_CircularBuffer));
	}
	
}

px_float PX_CircularBufferDelay(PX_CircularBuffer* pcbuffer, px_int pos)
{
	return pcbuffer->buffer[(pcbuffer->pointer + pos) %pcbuffer->size];
}
