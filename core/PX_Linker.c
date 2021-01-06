#include "PX_Linker.h"
px_void PX_LinkerInitialize(PX_Linker *linker,PX_LinkerRead_func read,px_void *readUserPtr,PX_LinkerWrite_func write,px_void *writeUserPtr)
{
	linker->read=read;
	linker->write=write;
	linker->readUserPtr=readUserPtr;
	linker->writeUserPtr=writeUserPtr;
}

px_int PX_LinkerRead(PX_Linker *linker,px_void *data,px_int datasize)
{
	return linker->read(linker->readUserPtr,data,datasize);
}

px_int PX_LinkerWrite(PX_Linker *linker,px_void *data,px_int datasize)
{
	return linker->write(linker->readUserPtr,data,datasize);
}

