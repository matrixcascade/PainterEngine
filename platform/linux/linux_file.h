
#ifndef PAINTERENGINE_FILE_H
#define PAINTERENGINE_FILE_H
#define PAINTERENGIN_FILE_SUPPORT
//////////////////////////////////////////////////////////////////////////
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <errno.h>
#include <android/asset_manager.h>

typedef struct
{
	unsigned char *buffer;
	unsigned int size;
}PX_IO_Data;

PX_IO_Data PX_LoadFileToIOData(const char path[]);
PX_IO_Data PX_LoadAssetsFileToIOData(const char path[]);
int PX_SaveDataToFile(void *buffer,int size,const char path[]);
void PX_FreeIOData(PX_IO_Data *io);
#endif

