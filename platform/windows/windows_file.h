
#ifndef PAINTERENGINE_FILE_H
#define PAINTERENGINE_FILE_H

#define PAINTERENGIN_FILE_SUPPORT
//////////////////////////////////////////////////////////////////////////
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

typedef struct
{
	unsigned char *buffer;
	unsigned int size;
}PX_IO_Data;


PX_IO_Data PX_LoadFileToIOData(const char path[]);
const char * PX_GetFileName(const char filePath[]);
int PX_FileExist(const char respath[]);
int PX_CopyFile(const char respath[],const char destpath[]);
int PX_SaveDataToFile(void *buffer,int size,const char path[]);
void PX_FreeIOData(PX_IO_Data *io);
#endif

