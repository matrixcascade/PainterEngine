#include "PX_Linker.h"

px_void PX_LinkerInitialize(PX_Linker* linker, PX_LinkerOpen_func open, PX_LinkerRead_func read, PX_LinkerWrite_func write, PX_LinkerClose_func close, px_void* userptr)
{
	linker->open = open;
	linker->read = read;
	linker->write = write;
	linker->close = close;
	linker->userPtr = userptr;
}

px_int PX_LinkerOpen(PX_Linker* linker)
{
	if (!linker->open)
	{
		return PX_TRUE;
	}
	return linker->isOpen=linker->open(linker,linker->userPtr);
}

px_int PX_LinkerRead(PX_Linker *linker,px_void *data,px_int datasize)
{
	if (!linker->read)
	{
		return PX_TRUE;
	}
	return linker->read(linker,data,datasize, linker->userPtr);
}

px_int PX_LinkerWrite(PX_Linker *linker,px_void *data,px_int datasize)
{
	if (!linker->write)
	{
		return PX_TRUE;
	}
	return linker->write(linker,data,datasize, linker->userPtr);
}

px_void PX_LinkerClose(PX_Linker* linker)
{
	if (!linker->close)
	{
		return;
	}
	linker->close(linker, linker->userPtr);
	linker->isOpen = PX_FALSE;
}

