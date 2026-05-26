#include "PX_InnerHub.h"

px_bool PX_InnerHubInitialize(px_memorypool* mp, PX_INNERHUB_TYPE type, PX_InnerHub* hub)
{
	if (!hub || !mp)
	{
		return PX_FALSE;
	}
	hub->mp = mp;
	PX_MemoryInitialize(mp, &hub->io);
	PX_MemoryInitialize(mp, &hub->p1);
	PX_MemoryInitialize(mp, &hub->p2);
	return PX_TRUE;
}
px_void PX_InnerHubClear(PX_InnerHub* hub)
{
	if (!hub)
	{
		PX_ASSERTX("PX_InnerHubClear: hub is null");
		return;
	}
	PX_MemoryClear(&hub->io);
	PX_MemoryClear(&hub->p1);
	PX_MemoryClear(&hub->p2);
}
px_int PX_InnerHubGetReadSize(PX_InnerHub* hub)
{
	return PX_MemoryGetUsedSize(&hub->io);
}

px_int PX_InnerHubRead(PX_InnerHub* hub, px_void* data, px_int size)
{
	if (size>PX_MemoryGetUsedSize(&hub->io))
	{
		size = PX_MemoryGetUsedSize(&hub->io);
	}
	PX_memcpy(data, hub->io.buffer, size);
	PX_MemoryRemove(&hub->io, 0, size-1);
	return size;
}

px_int PX_InnerHubWrite(PX_InnerHub* hub, px_void* data, px_int size)
{
	return PX_MemoryCat(&hub->io, data, size);
}

px_int PX_InnerHub1GetReadSize(PX_InnerHub* hub)
{
	return PX_MemoryGetUsedSize(&hub->p1);
}

px_bool PX_InnerHub1Read(PX_InnerHub* hub, px_void* data, px_int size)
{
	if (size > PX_MemoryGetUsedSize(&hub->p1))
	{
		size = PX_MemoryGetUsedSize(&hub->p1);
	}
	PX_memcpy(data, hub->p1.buffer, size);
	PX_MemoryRemove(&hub->p1, 0, size - 1);
	return size;
}
px_bool PX_InnerHub1Write(PX_InnerHub* hub, px_void* data, px_int size)
{
	return PX_MemoryCat(&hub->p2, data, size);
}

px_int PX_InnerHub2GetReadSize(PX_InnerHub* hub)
{
	return PX_MemoryGetUsedSize(&hub->p2);
}

px_bool PX_InnerHub2Read(PX_InnerHub* hub, px_void* data, px_int size)
{
	if (size > PX_MemoryGetUsedSize(&hub->p2))
	{
		size = PX_MemoryGetUsedSize(&hub->p2);
	}
	PX_memcpy(data, hub->p2.buffer, size);
	PX_MemoryRemove(&hub->p2, 0, size - 1);
	return size;
}

px_bool PX_InnerHub2Write(PX_InnerHub* hub, px_void* data, px_int size)
{
	return PX_MemoryCat(&hub->p1, data, size);
}