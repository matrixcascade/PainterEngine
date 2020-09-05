
#include "windows_file.h"

FILE *PX_Windows_fopen(const char path[], const char mode[])
{
	FILE *pf;
	errno_t err;
	err = fopen_s(&pf, path, mode);
	if (err != 0)
		return 0;
	else
		return pf;
}


int PX_SaveDataToFile(void *buffer,int size,const char path[])
{
	FILE *pf= PX_Windows_fopen(path,"wb");
	if (pf)
	{
		fwrite(buffer,1,size,pf);
		fclose(pf);
		return 1;
	}
	return 0;
}

PX_IO_Data PX_LoadFileToIOData(const char path[])
{
	PX_IO_Data io;
	int fileoft=0;
	FILE *pf= PX_Windows_fopen(path,"rb");
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

int PX_CopyFile(const char respath[],const char destpath[])
{
	PX_IO_Data io;
	if (strcmp(respath,destpath)==0)
	{
		return 1;
	}
	io=PX_LoadFileToIOData(respath);
	if (io.size==0)
	{
		return 0;
	}
	if (PX_SaveDataToFile(io.buffer,io.size,destpath))
	{
		PX_FreeIOData(&io);
		return 1;
	}
	PX_FreeIOData(&io);
	return 0;
}

const char * PX_GetFileName(const char filePath[])
{
	int offset=strlen(filePath)-1;
	while (offset)
	{
		if (filePath[offset]=='/'||filePath[offset]=='\\')
		{
			offset++;
			break;
		}
		offset--;
	}
	return (char *)filePath+offset;
}

int PX_FileExist(const char path[])
{
	FILE *pf= PX_Windows_fopen(path,"rb");
	if (pf)
	{
		fclose(pf);
		return 1;
	}
	return 0;
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
