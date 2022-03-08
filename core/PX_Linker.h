#ifndef PX_LINKER_H
#define PX_LINKER_H

#include "PX_Typedef.h"
struct _PX_Linker;
typedef px_bool(*PX_LinkerOpen_func)(struct _PX_Linker* linker, px_void* userPtr);
typedef px_int (*PX_LinkerRead_func)(struct _PX_Linker* linker, px_void *data,px_int datasize, px_void* userPtr);
typedef px_int (*PX_LinkerWrite_func)(struct _PX_Linker* linker, px_void *data,px_int datasize, px_void* userPtr);
typedef px_void(*PX_LinkerClose_func)(struct _PX_Linker* linker, px_void* userptr);

typedef union
{
	px_byte  byte_param[64];
	px_dword dword_param[16];
	px_void* ptr[8];
	px_int32 int_param[16];
	px_qword qword_param[8];
	PX_Network_Addr network_addr;
}PX_LinkerData;

typedef struct _PX_Linker
{
	PX_LinkerOpen_func open;
	PX_LinkerRead_func read;
	PX_LinkerWrite_func write;
	PX_LinkerClose_func close;
	
	px_bool isOpen;
	px_void *userPtr;
	PX_LinkerData linkerData;
	
}PX_Linker;

px_void PX_LinkerInitialize(PX_Linker *linker, PX_LinkerOpen_func open,PX_LinkerRead_func read,PX_LinkerWrite_func write, PX_LinkerClose_func close,px_void *userptr);
px_int PX_LinkerOpen(PX_Linker* linker);
px_int PX_LinkerRead(PX_Linker *linker,px_void *data,px_int datasize);
px_int PX_LinkerWrite(PX_Linker *linker,px_void *data,px_int datasize);
px_void PX_LinkerClose(PX_Linker* linker);

#endif
