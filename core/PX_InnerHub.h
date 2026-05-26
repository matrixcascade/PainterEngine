#ifndef PX_INNERHUB_H
#define PX_INNERHUB_H
#include "PX_Memory.h"

typedef enum
{
	PX_INNERHUB_TYPE_SIMPLEX = 0,
	PX_INNERHUB_TYPE_DUPLEX
}PX_INNERHUB_TYPE;

typedef struct 
{
	px_memorypool* mp;
	px_memory io;
	px_memory p1, p2;
}PX_InnerHub;

px_bool PX_InnerHubInitialize(px_memorypool* mp, PX_INNERHUB_TYPE type, PX_InnerHub* hub);
px_void PX_InnerHubClear(PX_InnerHub* hub);

px_int PX_InnerHubGetReadSize(PX_InnerHub* hub);
px_int PX_InnerHubRead(PX_InnerHub* hub, px_void* data, px_int size);
px_int PX_InnerHubWrite(PX_InnerHub* hub, px_void* data, px_int size);

px_int PX_InnerHub1GetReadSize(PX_InnerHub* hub);
px_bool PX_InnerHub1Read(PX_InnerHub* hub, px_void* data, px_int size);
px_bool PX_InnerHub1Write(PX_InnerHub* hub, px_void* data, px_int size);

px_int PX_InnerHub2GetReadSize(PX_InnerHub* hub);
px_bool PX_InnerHub2Read(PX_InnerHub* hub, px_void* data, px_int size);
px_bool PX_InnerHub2Write(PX_InnerHub* hub, px_void* data, px_int size);
#endif