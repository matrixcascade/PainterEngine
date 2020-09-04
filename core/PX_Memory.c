#include "PX_Memory.h"
px_void PX_MemoryInit(px_memorypool *mp,px_memory *memory)
{
	memory->buffer=PX_NULL;
	memory->allocsize=0;
	memory->usedsize=0;
	memory->mp=mp;
}

px_bool PX_MemoryCat(px_memory *memory,px_void *buffer,px_int size)
{
	px_byte *old;
	px_int length,shl;

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
	return (memory->buffer=(px_byte *)MP_Malloc(memory->mp,size))!=0;
}

px_bool PX_MemoryResize(px_memory *memory,px_int size)
{
	return PX_MemoryAlloc(memory,size);
}


px_byte *PX_MemoryFine(px_memory *memory,px_void *buffer,px_int size)
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

px_void PX_MemoryClear(px_memory *memory)
{
	memory->usedsize=0;
}

px_bool PX_MemoryCopy(px_memory *memory,px_void *buffer,px_int startoffset,px_int size)
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

