
#include "android_file.h"

extern AAssetManager* PX_assetManager;

int PX_SaveDataToFile(void *buffer,int size,const char path[])
{
	FILE *pf=fopen(path,"rb");
	if (pf)
	{
		fwrite(buffer,1,size,pf);
		fclose(pf);
		return 1;
	}
	return 0;
}

PX_IO_Data PX_LoadAssetsFileToIOData(const char path[])
{
	int len;
	PX_IO_Data io;

	AAsset* asset = AAssetManager_open(PX_assetManager, path, 0);
	if(!asset) goto _ERROR;

	len=AAsset_getLength(asset);

	io.buffer=(unsigned char *)malloc(len+1);
	io.size=len;

	AAsset_read(asset, io.buffer, io.size);

	AAsset_close(asset);
	return  io;

	_ERROR:
	io.buffer=0;
	io.size=0;
	return io;
}

PX_IO_Data PX_LoadFileToIOData(const char path[])
{
	if (memcmp(path,"assets/",7)==0)
	{
		return PX_LoadAssetsFileToIOData(path+7);
	}
	else
	{
		PX_IO_Data io;
		int fileoft=0;
		FILE *pf=fopen(path,"rb");
		int filesize;
		if (!pf)
		{
			goto _ERROR;
		}
		fseek(pf,0,SEEK_END);
		filesize=ftell(pf);
		fseek(pf,0,SEEK_SET);

		io.buffer=(unsigned char *)malloc(filesize+1);
		if (!io.buffer)
		{
			goto _ERROR;
		}

		while (!feof(pf))
		{
			fileoft+=(int)fread(io.buffer+fileoft,1,1024,pf);
		}
		fclose(pf);

		io.buffer[filesize]='\0';
		io.size=filesize;
		return io;
_ERROR:
		io.buffer=0;
		io.size=0;
		return io;
	}
}

void PX_FreeIOData(PX_IO_Data *io)
{
	if (io->size&&io->buffer)
	{
		free(io->buffer);
		io->size=0;
		io->buffer=0;
	}
}

int PX_FileExist(const char path[])
{
	FILE *pf= fopen(path,"rb");
	if (pf)
	{
		fclose(pf);
		return 1;
	}
	return 0;
}