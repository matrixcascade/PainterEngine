#ifndef PX_LINKER_H
#define PX_LINKER_H

#include "PX_Typedef.h"

typedef px_int (*PX_LinkerRead_func)(px_void *userPtr,px_void *data,px_int datasize);
typedef px_int (*PX_LinkerWrite_func)(px_void *userPtr,px_void *data,px_int datasize);


typedef struct
{
	PX_LinkerRead_func read;
	px_void *readUserPtr;
	PX_LinkerWrite_func write;
	px_void *writeUserPtr;
}PX_Linker;

px_void PX_LinkerInitialize(PX_Linker *linker,PX_LinkerRead_func read,px_void *readUserPtr,PX_LinkerWrite_func write,px_void *writeUserPtr);
px_int PX_LinkerRead(PX_Linker *linker,px_void *data,px_int datasize);
px_int PX_LinkerWrite(PX_Linker *linker,px_void *data,px_int datasize);


#endif
