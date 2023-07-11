#include "PX_Memory.h"
px_void PX_MemoryInitialize(px_memorypool *mp,px_memory *memory)
{
	PX_memset(memory, 0, sizeof(px_memory));
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
	memory->bit_pointer = 0;
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
	memory->bit_pointer = 0;
}

px_byte * PX_MemoryData(px_memory *memory)
{
	return memory->buffer;
}

px_bool PX_MemoryCatBit(px_memory* memory, px_bool b)
{
	if (memory->bit_pointer==0)
	{
		px_byte bit = (b==1);
		if (!PX_MemoryCat(memory, &bit, 1))
			return PX_FALSE;
		memory->bit_pointer = 1;
	}
	else
	{
		px_byte* plastbyte = memory->buffer + memory->usedsize - 1;
		if(b)
			(*plastbyte) |= (1 << memory->bit_pointer);
		memory->bit_pointer++;
		memory->bit_pointer %= 8;
	}
	return PX_TRUE;
}

px_bool PX_MemoryCatBits(px_memory* memory, px_byte data[], px_int bit_count)
{
	px_int i;
	px_uint bp=0;
	for (i = 0; i < bit_count; i++)
	{
		if (!PX_MemoryCatBit(memory, PX_ReadBitLE(&bp, data)))
			return PX_FALSE;
	}
	return PX_TRUE;
}

px_bool PX_MemoryAlloc(px_memory *memory,px_int size)
{
	PX_MemoryFree(memory);
	memory->allocsize=size;
	memory->usedsize=0;
	memory->bit_pointer = 0;
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
	memory->bit_pointer = 0;
}


px_void PX_MemoryClear(px_memory *memory)
{
	memory->usedsize=0;
	memory->bit_pointer = 0;
}

px_void PX_MemoryLeft(px_memory* memory,px_int trimsize)
{
	if (trimsize>=memory->usedsize)
	{
		return;
	}
	memory->usedsize = trimsize;
	memory->bit_pointer = 0;
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
	if (size==0)
	{
		PX_ASSERT();
	}
	pcbuffer->buffer = (px_double *)MP_Malloc(mp, sizeof(px_double) * size);
	if (!pcbuffer)
	{
		return PX_FALSE;
	}
	PX_memset(pcbuffer->buffer, 0, sizeof(px_double) * size);
	pcbuffer->mp = mp;
	pcbuffer->pointer = 0;
	pcbuffer->size = size;
	return PX_TRUE;
}

px_void PX_CircularBufferPush(PX_CircularBuffer* pcbuffer, px_double v)
{
	pcbuffer->pointer--;
	if (pcbuffer->pointer<0)
	{
		pcbuffer->pointer = pcbuffer->size - 1;
	}
	pcbuffer->buffer[pcbuffer->pointer] = v;
}
px_void PX_CircularBufferAdd(PX_CircularBuffer* pcbuffer, px_int pos, px_double v)
{
	pos = pos % pcbuffer->size;
	if (pos < 0) { pos += pcbuffer->size; }
	pcbuffer->buffer[pos] += v;
}
px_void PX_CircularBufferSet(PX_CircularBuffer* pcbuffer, px_int pos, px_double v)
{
	pos = pos % pcbuffer->size;
	if (pos < 0) { pos += pcbuffer->size; }
	pcbuffer->buffer[pos] = v;
}
px_double  PX_CircularBufferGet(PX_CircularBuffer* pcbuffer, px_int pos)
{
	pos = pos % pcbuffer->size;
	if (pos < 0) { pos += pcbuffer->size; }
	return pcbuffer->buffer[pos];
}

px_void PX_CircularBufferZeroClear(PX_CircularBuffer* pcbuffer)
{
	PX_memset(pcbuffer->buffer, 0, sizeof(px_double) * pcbuffer->size);
}

px_void PX_CircularBufferFree(PX_CircularBuffer* pcbuffer)
{
	if (pcbuffer->mp)
	{
		MP_Free(pcbuffer->mp, pcbuffer->buffer);
		PX_memset(pcbuffer, 0, sizeof(PX_CircularBuffer));
	}
	
}

px_double PX_CircularBufferDelay(PX_CircularBuffer* pcbuffer, px_int pos)
{
	return pcbuffer->buffer[(pcbuffer->pointer + pos) %pcbuffer->size];
}

px_void PX_FifoBufferInitialize(px_memorypool* mp, px_fifobuffer* pfifo)
{
	PX_MemoryInitialize(mp, pfifo);
}

px_int PX_FifoBufferPop(px_fifobuffer* pfifo, px_void* data, px_int size)
{
	if (pfifo->usedsize)
	{
		px_int rsize = *(px_int*)pfifo->buffer;
		if (rsize>pfifo->usedsize-(px_int)sizeof(px_int))
		{
			PX_ASSERT();//fifo error
			return 0;
		}
		if (size<rsize)
		{
			return 0;
		}
		if(data)
			PX_memcpy(data, pfifo->buffer + sizeof(px_int), rsize);
		PX_MemoryRemove(pfifo, 0, rsize + sizeof(px_int) - 1);
		return rsize;

	}
	return 0;
	
}

px_bool PX_FifoBufferPush(px_fifobuffer* pfifo, px_void* data, px_int size)
{
	px_int wsize = size;
	if (wsize<0)
	{
		PX_ASSERT();
	}
	if (wsize==0)
	{
		return PX_TRUE;
	}
	if (PX_MemoryCat(pfifo, &wsize, sizeof(wsize)))
	{
		if (PX_MemoryCat(pfifo,data,size))
		{
			return PX_TRUE;
		}
	}
	return PX_FALSE;

}

px_int PX_FifoBufferGetPopSize(px_fifobuffer* pfifo)
{
	if (pfifo->usedsize>sizeof(px_int))
	{
		return *(px_int*)pfifo->buffer;
	}
	return 0;
}

px_void PX_FifoBufferFree(px_fifobuffer* pfifo)
{
	PX_MemoryFree(pfifo);
}

px_void PX_StackInitialize(px_memorypool* mp, px_stack* pstack)
{
	PX_MemoryInitialize(mp, pstack);
}

px_int PX_StackPop(px_stack* pstack, px_void* data, px_int size)
{
	if (pstack->usedsize>sizeof(px_int))
	{
		px_int rsize = *(px_int*)(pstack->buffer+pstack->usedsize-sizeof(px_int));
		if (rsize > pstack->usedsize - (px_int)sizeof(px_int))
		{
			PX_ASSERT();//stack error
			return 0;
		}
		if (size < rsize)
		{
			return 0;
		}
		PX_memcpy(data, pstack->buffer + pstack->usedsize - sizeof(px_int)- rsize, rsize);
		PX_MemoryRemove(pstack, pstack->usedsize - sizeof(px_int) - rsize, pstack->usedsize - 1);
		return rsize;

	}
	return 0;
}

px_bool PX_StackPush(px_stack* pstack, px_void* data, px_int size)
{
	px_int wsize = size;
	if (wsize < 0)
	{
		PX_ASSERT();
	}
	if (wsize == 0)
	{
		return PX_TRUE;
	}
	
	if (PX_MemoryCat(pstack, data, size))
	{
		if (PX_MemoryCat(pstack, &wsize, sizeof(wsize)))
		{
			return PX_TRUE;
		}
	}
	return PX_FALSE;
}

px_int PX_StackGetPopSize(px_stack* pstack)
{
	return *(px_int*)(pstack->buffer + pstack->usedsize - sizeof(px_int));
}
px_void PX_StackFree(px_stack* pstack)
{
	PX_MemoryFree(pstack);
}