
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

typedef enum
{
	PX_FILEENUM_TYPE_ANY,
	PX_FILEENUM_TYPE_FOLDER,
	PX_FILEENUM_TYPE_FILE,
	PX_FILEENUM_TYPE_DEVICE,
}PX_FILEENUM_TYPE;


PX_IO_Data PX_LoadFileToIOData(const char path[]);
int PX_FileExist(const char respath[]);
int PX_CopyFile(const char respath[],const char destpath[]);
int PX_SaveDataToFile(void *buffer,int size,const char path[]);
void PX_FreeIOData(PX_IO_Data *io);

int PX_FileGetDirectoryFileCount(const char path[],PX_FILEENUM_TYPE type,const char *filter);
int PX_FileGetDirectoryFileName(const char path[],int count,char FileName[][260],PX_FILEENUM_TYPE type,const char *filter);

#endif

