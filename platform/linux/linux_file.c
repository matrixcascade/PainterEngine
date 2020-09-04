
#include "linux_file.h"


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

PX_IO_Data PX_LoadFileToIOData(const char path[])
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

void PX_FreeIOData(PX_IO_Data *io)
{
	if (io->size&&io->buffer)
	{
		free(io->buffer);
		io->size=0;
		io->buffer=0;
	}
}
