
#include "../modules/px_file.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "windows.h"

int PX_SaveDataToFile(void *buffer,int size,const char path[])
{
	char _path[MAX_PATH];
	FILE *pf;
	if (path[0]=='\\'||path[0]=='/')
	{
		strcpy_s(_path,sizeof(_path),path+1);
	}
	else
	{
		strcpy_s(_path,sizeof(_path),path);
	}
	fopen_s(&pf,_path,"wb");
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
		FILE *pf;
		int filesize;
		char _path[MAX_PATH];
		if (path[0]=='\\'||path[0]=='/')
		{
			strcpy_s(_path,sizeof(_path),path+1);
		}
		else
		{
			strcpy_s(_path,sizeof(_path),path);
		}

		fopen_s(&pf,_path,"rb");
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

int PX_FileExist(const char path[])
{
	char _path[MAX_PATH];
	FILE *pf;
	if (path[0]=='\\'||path[0]=='/')
	{
		strcpy_s(_path,sizeof(_path),path+1);
	}
	else
	{
		strcpy_s(_path,sizeof(_path),path);
	}

	fopen_s(&pf,_path,"rb");;
	if (pf)
	{
		fclose(pf);
		return 1;
	}
	return 0;
}




int PX_FileGetDirectoryFileCount(const char path[],PX_FILEENUM_TYPE type,const char *filter)
{
	HANDLE hFind;
	int count=0;
    WIN32_FIND_DATAA FindFileData;
	char _findpath[MAX_PATH];
	
	if (path[0]==0||(path[0]=='\\'&&path[1]=='\0')||(path[0]=='/'&&path[1]=='\0'))
	{
		if (type==PX_FILEENUM_TYPE_FILE)
		{
			return 0;
		}
		if (type==PX_FILEENUM_TYPE_FOLDER)
		{
			char drivers[128];
			int index=0;
			if(GetLogicalDriveStringsA(128,drivers))
			{
				while (drivers[index])
				{
					count++;
					index+= (int)(strlen(drivers))+1;
				}
			}
			return count;
		}
	}
	if (path[0]=='\\'||path[0]=='/')
	{
		strcpy_s(_findpath,sizeof(_findpath),path+1);
	}
	else
	{
		strcpy_s(_findpath,sizeof(_findpath),path);
	}
	
	if (_findpath[strlen(path)-1]!='/'&&_findpath[strlen(path)-1]!='\\')
	{
		strcat_s(_findpath,sizeof(_findpath),"/");
	}
	strcat_s(_findpath,sizeof(_findpath),"*.*");

	hFind = FindFirstFileA(_findpath,&FindFileData);
	if (hFind==INVALID_HANDLE_VALUE)
	{
		return 0;
	}
	do 
	{
		switch (type)
		{
		case PX_FILEENUM_TYPE_ANY:
			{
				if (filter[0]&&FindFileData.dwFileAttributes !=FILE_ATTRIBUTE_DIRECTORY)
				{
					const char *pFilter=filter;
					while (pFilter[0])
					{
						if (strstr(FindFileData.cFileName,pFilter))
						{
							count++;
							break;
						}
						pFilter+=strlen(pFilter)+1;
					}
					
				} else
				{
					count++;
				}
			}
			break;
		case PX_FILEENUM_TYPE_DEVICE:
		case PX_FILEENUM_TYPE_FILE:
			{
				if (FindFileData.dwFileAttributes !=FILE_ATTRIBUTE_DIRECTORY)
				{
					if (filter[0])
					{
						const char *pFilter=filter;
						while (pFilter[0])
						{
							if (strstr(FindFileData.cFileName,pFilter))
							{
								count++;
								break;
							}
							pFilter+=strlen(pFilter)+1;

						}
					} else
					{
						count++;
					}
				}
			}
			break;
		case PX_FILEENUM_TYPE_FOLDER:
			{
				if (FindFileData.dwFileAttributes ==FILE_ATTRIBUTE_DIRECTORY)
				{
					count++;
				}
			}
			break;
		default:
			break;
		}
	} while (FindNextFileA(hFind,&FindFileData));
	FindClose(hFind);
	return count;
}

int PX_FileGetDirectoryFileName(const char path[],int count,char FileName[][260],PX_FILEENUM_TYPE type,const char *filter)
{
	HANDLE hFind;
	int index=0;
	WIN32_FIND_DATAA FindFileData;
	static char _findpath[MAX_PATH];

	if (path[0]==0||(path[0]=='\\'&&path[1]=='\0')||(path[0]=='/'&&path[1]=='\0'))
	{
		int dcount=0;
		if (type==PX_FILEENUM_TYPE_FILE)
		{
			return 0;
		}
		if (type==PX_FILEENUM_TYPE_FOLDER)
		{
			char drivers[128];
			int index=0;
			if(GetLogicalDriveStringsA(128,drivers))
			{
				while (drivers[index])
				{
					strcpy_s(FileName[dcount],260,drivers+index);
					dcount++;
					index+=(int)(strlen(drivers)+1);
				}
			}
			return dcount;
		}
	}

	if (path[0]=='\\'||path[0]=='/')
	{
		strcpy_s(_findpath,sizeof(_findpath),path+1);
	}
	else
	{
		strcpy_s(_findpath,sizeof(_findpath),path);
	}

	if (_findpath[strlen(path)-1]!='/'&&_findpath[strlen(path)-1]!='\\')
	{
		strcat_s(_findpath,sizeof(_findpath),"/");
	}
	strcat_s(_findpath,sizeof(_findpath),"*.*");

	hFind = FindFirstFileA(_findpath,&FindFileData);
	if (hFind==INVALID_HANDLE_VALUE)
	{
		return 0;
	}
	do 
	{
		if (index>=count)
		{
			break;
		}

		switch (type)
		{
		case PX_FILEENUM_TYPE_ANY:
			{
				if (filter[0]&&FindFileData.dwFileAttributes !=FILE_ATTRIBUTE_DIRECTORY)
				{
					const char *pFilter=filter;
					while (pFilter[0])
					{
						if (strstr(FindFileData.cFileName,pFilter))
						{
							strcpy_s(FileName[index],260,FindFileData.cFileName);
							index++;
							break;
						}
						pFilter+=strlen(pFilter)+1;
					}
				} else
				{
					strcpy_s(FileName[index],260,FindFileData.cFileName);
					index++;
				}
			}
			break;
		case PX_FILEENUM_TYPE_DEVICE:
		case PX_FILEENUM_TYPE_FILE:
			{
				if (FindFileData.dwFileAttributes !=FILE_ATTRIBUTE_DIRECTORY)
				{
					if (filter[0])
					{
						const char *pFilter=filter;
						while (pFilter[0])
						{
							if (strstr(FindFileData.cFileName,pFilter))
							{
								strcpy_s(FileName[index],260,FindFileData.cFileName);
								index++;
								break;
							}
							pFilter+=strlen(pFilter)+1;
						}
					} else
					{
						strcpy_s(FileName[index],260,FindFileData.cFileName);
						index++;
					}
				}
			}
			break;
		case PX_FILEENUM_TYPE_FOLDER:
			{
				if (FindFileData.dwFileAttributes ==FILE_ATTRIBUTE_DIRECTORY)
				{
					strcpy_s(FileName[index],260,FindFileData.cFileName);
					index++;
				}
			}
			break;
		default:
			break;
		}
	} while (FindNextFileA(hFind,&FindFileData));
	FindClose(hFind);
	return index;
}