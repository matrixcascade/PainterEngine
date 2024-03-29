
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

	fopen_s(&pf,_path,"rb");
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
	if (!filter)
	{
		filter = "";
	}
	
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

	if (!filter)
	{
		filter = "";
	}

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

char* PX_OpenFileDialog(const char Filter[])
{
	OPENFILENAMEA ofn;
	static char szFile[MAX_PATH];
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.lpstrFile = szFile;
	ofn.lpstrFile[0] = TEXT('\0');
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = Filter;//TEXT("Mirror(.mirror)\0*.mirror");
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.hwndOwner = GetActiveWindow();
	ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;

	if (GetOpenFileNameA(&ofn))
	{
		return szFile;
	}
	return NULL;
}

char* PX_MultFileDialog(const char Filter[])
{
	OPENFILENAMEA ofn;
	static char szFile[MAX_PATH * 64];
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.lpstrFile = szFile;
	ofn.lpstrFile[0] = TEXT('\0');
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = Filter;//TEXT("MirrorÎÄ¼þ(.mirror)\0*.mirror");
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.hwndOwner = GetActiveWindow();
	ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_ALLOWMULTISELECT | OFN_NOCHANGEDIR;

	if (GetOpenFileNameA(&ofn))
	{
		if (szFile[strlen(szFile) + 1] == '\0')
		{
			int oft = (int)strlen(szFile) - 1;
			while (oft >= 0)
			{
				if (szFile[oft] == '\\' || szFile[oft] == '/')
				{
					szFile[oft] = 0;
					break;
				}
				oft--;
			}
		}
		return szFile;
	}
	return NULL;
}

char* PX_SaveFileDialog(const char Filter[], const char ext[])
{
	OPENFILENAMEA ofn;
	static char szFile[MAX_PATH];
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.lpstrFile = szFile;
	ofn.lpstrFile[0] = TEXT('\0');
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = Filter;//TEXT("MirrorÎÄ¼þ(.mirror)\0*.mirror");
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.hwndOwner = GetActiveWindow();
	ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;

	if (GetSaveFileNameA(&ofn))
	{
		if (ext)
		{
			char uprFile[MAX_PATH] = { 0 };
			char uprExt[MAX_PATH] = { 0 };
			strcpy_s(uprExt, MAX_PATH, ext);
			_strupr_s(uprExt, MAX_PATH);
			strcpy_s(uprFile, MAX_PATH, szFile);
			_strupr_s(uprFile, MAX_PATH);
			if (!strstr(uprFile, uprExt))
			{
				strcat_s(szFile, sizeof(szFile), ext);
			}
		}
		return szFile;
	}
	return NULL;
}

px_char PX_RequestData_Extern[128] = {0};

void PX_RequestData(const char url[], void* buffer, int size, void* ptr, void (*func_callback)(void* buffer, int size, void* ptr))
{
	if (strstr(url,"open"))
	{
		char* ppath = PX_OpenFileDialog("");
		if (ppath&&ppath[0])
		{
			PX_IO_Data io = PX_LoadFileToIOData(ppath);
			if (io.size > 0 && io.buffer && io.size <= (unsigned int)size)
			{
				PX_FileGetName(ppath, PX_RequestData_Extern, sizeof(PX_RequestData_Extern));
				memcpy(buffer, io.buffer, io.size);
				func_callback(buffer, io.size, ptr);
			}
			else
			{
				func_callback(buffer, 0, ptr);
			}
			PX_FreeIOData(&io);
		}
		else
		{
			func_callback(buffer, 0, ptr);
		}
	}
	else if (memcmp(url, "save:", 5) == 0)
	{
		char* ppath = PX_SaveFileDialog("", url+5);
		if (ppath && ppath[0])
		{
			PX_SaveDataToFile(buffer, size, ppath);
		}
		else
		{
			func_callback(buffer, 0, ptr);
		}
	}
	else if (memcmp(url, "download:", 9)==0)
	{
		PX_SaveDataToFile(buffer,size, (char*)url + 9);
	}
	else
	{
		PX_IO_Data io = PX_LoadFileToIOData(url);
		if (io.size > 0 && io.buffer && io.size <= (unsigned int)size)
		{
			memcpy(buffer, io.buffer, io.size);
			func_callback(buffer, io.size, ptr);
		}
		else
		{
			func_callback(buffer, 0, ptr);
		}
		PX_FreeIOData(&io);
	}
    
}



//////////////////////////////////////////////////////////////////////////
//commons

px_bool PX_LoadTextureFromFile(px_memorypool *mp,px_texture *tex,const px_char path[])
{
	PX_IO_Data io;

	io=PX_LoadFileToIOData(path);
	if (!io.size)
	{
		return PX_FALSE;
	}
	if (PX_TextureCreateFromMemory(mp,io.buffer,io.size,tex))
	{
		PX_FreeIOData(&io);
		return PX_TRUE;
	}
	PX_FreeIOData(&io);
	return PX_FALSE;
}

px_bool PX_LoadStringFromFile(px_memorypool* mp, px_string* _string, const px_char path[])
{
	PX_IO_Data io;

	io = PX_LoadFileToIOData(path);
	if (!io.size)
	{
		return PX_FALSE;
	}
	PX_StringInitialize(mp, _string);
	if(PX_StringCat(_string, (const px_char *)io.buffer))
	{
		PX_FreeIOData(&io);
		return PX_TRUE;
	}
	PX_FreeIOData(&io);
	return PX_FALSE;
}


px_bool PX_LoadShapeFromFile(px_memorypool *mp,px_shape *shape,const px_char path[])
{
	PX_IO_Data io=PX_LoadFileToIOData(path);
	if (!io.size)
	{
		return PX_FALSE;
	}
	if (PX_ShapeCreateFromMemory(mp,io.buffer,io.size,shape))
	{
		PX_FreeIOData(&io);
		return PX_TRUE;
	}
	PX_FreeIOData(&io);
	return PX_FALSE;

}


px_bool PX_LoadSoundFromFile(px_memorypool *mp,PX_SoundData *sounddata,const px_char path[])
{
	PX_IO_Data io=PX_LoadFileToIOData(path);
	if (!io.size)
	{
		return PX_FALSE;
	}
	if (PX_WaveVerify(io.buffer,io.size))
	{
		px_uint offset=0,pcmSize,woffset;
		pcmSize=PX_WaveGetPCMSize(io.buffer,io.size);

		if (pcmSize!=0)
		{
			PX_WAVE_DATA_BLOCK *pBlock;
			PX_WAVE_RIFF_HEADER *pHeader=(PX_WAVE_RIFF_HEADER *)io.buffer;
			PX_WAVE_FMT_BLOCK  *pfmt_block;
			sounddata->mp=mp;
			sounddata->buffer=(px_byte *)MP_Malloc(mp,pcmSize);
			sounddata->size=pcmSize;
			sounddata->channel=PX_WaveGetChannel(io.buffer,pcmSize)==1?PX_SOUND_CHANNEL_ONE:PX_SOUND_CHANNEL_DOUBLE;
			if (!sounddata->buffer)
			{
				return PX_FALSE;
			}
			pfmt_block=(PX_WAVE_FMT_BLOCK  *)(io.buffer+sizeof(PX_WAVE_RIFF_HEADER));
			offset+=sizeof(PX_WAVE_RIFF_HEADER);
			offset+=8;
			offset+=pfmt_block->dwFmtSize;

			pcmSize=0;
			woffset=0;
			while (offset<io.size)
			{
				pBlock=(PX_WAVE_DATA_BLOCK*)(io.buffer+offset);
				if(!PX_memequ(pBlock->szDataID,"data",4))
				{
					offset+=pBlock->dwDataSize+sizeof(PX_WAVE_DATA_BLOCK);
					continue;
				}
				offset+=sizeof(PX_WAVE_DATA_BLOCK);
				PX_memcpy(sounddata->buffer+woffset,io.buffer+offset,pBlock->dwDataSize);
				offset+=pBlock->dwDataSize;
				woffset+=pBlock->dwDataSize;
			}
		}
		else
		{
			PX_FreeIOData(&io);
			return PX_FALSE;
		}
	}
	else
	{
		PX_FreeIOData(&io);
		return PX_FALSE;
	}
	PX_FreeIOData(&io);
	return PX_TRUE;
}

px_bool PX_LoadMidiFromFile( PX_Midi* midi, const px_char path[])
{
	PX_IO_Data io = PX_LoadFileToIOData(path);
	if (!io.size)
	{
		return PX_FALSE;
	}
	if (!PX_MidiLoad(midi, io.buffer, io.size))
	{
		PX_FreeIOData(&io);
		return PX_FALSE;
	}
	PX_FreeIOData(&io);
	return PX_TRUE;
}


px_bool PX_LoadAnimationLibraryFromFile(px_memorypool *mp,PX_AnimationLibrary *lib,const px_char path[])
{
	PX_IO_Data io;
	io=PX_LoadFileToIOData(path);
	if (!io.size)
	{
		return PX_FALSE;
	}
	if (PX_AnimationLibraryCreateFromMemory(mp,lib,io.buffer,io.size))
	{
		PX_FreeIOData(&io);
		return PX_TRUE;
	}
	PX_FreeIOData(&io);
	return PX_FALSE;

}

px_bool PX_LoadScriptFromFile(px_memory *code,const px_char path[])
{
	PX_IO_Data io=PX_LoadFileToIOData(path);
	if (!io.size)
	{
		return PX_FALSE;
	}
	if(!PX_MemoryCopy(code,io.buffer,0,io.size))
		goto _ERROR;

	PX_FreeIOData(&io);
	return PX_TRUE;
_ERROR:
	PX_FreeIOData(&io);
	return PX_FALSE;

}

px_bool PX_LoadLiveFromFile(px_memorypool *mp,PX_LiveFramework *pliveframework, const px_char path[])
{
	PX_IO_Data io = PX_LoadFileToIOData(path);
	if (!io.size)
	{
		return PX_FALSE;
	}
	if (!PX_LiveFrameworkImport(mp, pliveframework,io.buffer,io.size))
	{
		PX_FreeIOData(&io);
		return PX_FALSE;
	}
	PX_FreeIOData(&io);
	return PX_TRUE;

}

px_bool PX_LoadUIFormFile(px_memorypool* mp, PX_Object* designerObject, PX_Object* proot,PX_FontModule *fm, const px_char path[])
{
	px_bool ret = PX_FALSE;
	PX_IO_Data io = PX_LoadFileToIOData(path);
	if (!io.size)
	{
		return PX_FALSE;
	}
	if (PX_Object_DesignerImportToUIObject(mp, designerObject,proot, (const px_char *)io.buffer,fm))
		ret = PX_TRUE;
	PX_FreeIOData(&io);
	return ret;
}



px_bool PX_LoadScriptInstanceFromFile(px_memorypool *mp,PX_VM *ins,const px_char path[])
{
	PX_IO_Data io=PX_LoadFileToIOData(path);
	if (!io.size)
	{
		return PX_FALSE;
	}
	if(!PX_VMInitialize(ins,mp,io.buffer,io.size))
		goto _ERROR;

	PX_FreeIOData(&io);
	return PX_TRUE;
_ERROR:
	PX_FreeIOData(&io);
	return PX_FALSE;
}

px_bool PX_LoadTextureToResource(PX_ResourceLibrary *ResourceLibrary,const px_char Path[],const px_char key[])
{
	PX_IO_Data io;
	io=PX_LoadFileToIOData(Path);
	if (!io.size)goto _ERROR;
	if(!PX_ResourceLibraryLoad(ResourceLibrary,PX_RESOURCE_TYPE_TEXTURE,io.buffer,io.size,key)) goto _ERROR;
	PX_FreeIOData(&io);
	return PX_TRUE;
_ERROR:
	PX_FreeIOData(&io);
	return PX_FALSE;
}

px_bool PX_LoadStringToResource(PX_ResourceLibrary* ResourceLibrary, const px_char Path[], const px_char key[])
{
	PX_IO_Data io;
	io = PX_LoadFileToIOData(Path);
	if (!io.size)goto _ERROR;
	if (!PX_ResourceLibraryLoad(ResourceLibrary, PX_RESOURCE_TYPE_STRING, io.buffer, io.size, key)) goto _ERROR;
	PX_FreeIOData(&io);
	return PX_TRUE;
_ERROR:
	PX_FreeIOData(&io);
	return PX_FALSE;
}


px_bool PX_LoadShapeToResource(PX_ResourceLibrary *ResourceLibrary,const px_char Path[],const px_char key[])
{
	PX_IO_Data io;
	io=PX_LoadFileToIOData(Path);
	if (!io.size)goto _ERROR;
	if(!PX_ResourceLibraryLoad(ResourceLibrary,PX_RESOURCE_TYPE_SHAPE,io.buffer,io.size,key)) goto _ERROR;
	PX_FreeIOData(&io);
	return PX_TRUE;
_ERROR:
	PX_FreeIOData(&io);
	return PX_FALSE;
}

px_bool PX_LoadAnimationToResource(PX_ResourceLibrary *ResourceLibrary,const px_char Path[],const px_char key[])
{
	PX_IO_Data io;
	io=PX_LoadFileToIOData(Path);
	if (!io.size)goto _ERROR;
	if(!PX_ResourceLibraryLoad(ResourceLibrary,PX_RESOURCE_TYPE_ANIMATIONLIBRARY,io.buffer,io.size,key)) goto _ERROR;
	PX_FreeIOData(&io);
	return PX_TRUE;
_ERROR:
	PX_FreeIOData(&io);
	return PX_FALSE;
}

px_bool PX_LoadScriptToResource(PX_ResourceLibrary *ResourceLibrary,const px_char Path[],const px_char key[])
{
	PX_IO_Data io;
	io=PX_LoadFileToIOData(Path);
	if (!io.size)goto _ERROR;
	if(!PX_ResourceLibraryLoad(ResourceLibrary,PX_RESOURCE_TYPE_SCRIPT,io.buffer,io.size,key)) goto _ERROR;
	PX_FreeIOData(&io);
	return PX_TRUE;
_ERROR:
	PX_FreeIOData(&io);
	return PX_FALSE;
}


px_bool PX_LoadSoundToResource(PX_ResourceLibrary *ResourceLibrary,const px_char Path[],const px_char key[])
{
	PX_IO_Data io;
	io=PX_LoadFileToIOData(Path);
	if (!io.size)goto _ERROR;
	if(!PX_ResourceLibraryLoad(ResourceLibrary,PX_RESOURCE_TYPE_SOUND,io.buffer,io.size,key)) goto _ERROR;
	PX_FreeIOData(&io);
	return PX_TRUE;
_ERROR:
	PX_FreeIOData(&io);
	return PX_FALSE;
}

px_bool PX_LoadFontModuleFromFile(PX_FontModule *fm,const px_char Path[])
{
	PX_IO_Data io;
	io=PX_LoadFileToIOData(Path);
	if (!io.size)goto _ERROR;
	if(!PX_FontModuleLoad(fm,io.buffer,io.size)) goto _ERROR;
	PX_FreeIOData(&io);
	return PX_TRUE;
_ERROR:
	PX_FreeIOData(&io);
	return PX_FALSE;
}

px_bool PX_LoadJsonFromFile(PX_Json *json,const px_char *path)
{
	PX_IO_Data io=PX_LoadFileToIOData((px_char *)path);
	if (!io.size)
	{
		return PX_FALSE;
	}

	if(!PX_JsonParse(json,(px_char *)io.buffer))goto _ERROR;


	PX_FreeIOData(&io);
	return PX_TRUE;
_ERROR:
	PX_FreeIOData(&io);
	return PX_FALSE;
}


px_bool PX_LoadGifFromFile(px_memorypool* mp, px_gif* gif, const px_char path[])
{
	PX_IO_Data io = PX_LoadFileToIOData((px_char*)path);
	if (!io.size)
	{
		return PX_FALSE;
	}

	if(!PX_GifCreate(mp,gif,io.buffer, io.size))goto _ERROR;

	PX_FreeIOData(&io);
	return PX_TRUE;
_ERROR:
	PX_FreeIOData(&io);
	return PX_FALSE;
}

px_bool PX_LoadLive2DFromFile(px_memorypool* mp, PX_LiveFramework* liveframework, const px_char path[])
{
	PX_IO_Data io = PX_LoadFileToIOData((px_char*)path);
	if (!io.size)
	{
		return PX_FALSE;
	}

	if (!PX_LiveFrameworkImport(mp,liveframework,io.buffer,io.size))goto _ERROR;


	PX_FreeIOData(&io);
	return PX_TRUE;
_ERROR:
	PX_FreeIOData(&io);
	return PX_FALSE;
}

px_bool PX_LoadFontModuleFromTTF(px_memorypool *mp,PX_FontModule* fm, const px_char Path[], PX_FONTMODULE_CODEPAGE codepage, px_int fontsize)
{
	PX_IO_Data io;
	io = PX_LoadFileToIOData(Path);
	if (!io.size)goto _ERROR;
	if (!PX_FontModuleInitializeTTF(mp,fm, codepage, fontsize,io.buffer,io.size)) goto _ERROR;
	PX_FreeIOData(&io);
	return PX_TRUE;
_ERROR:
	PX_FreeIOData(&io);
	return PX_FALSE;
}

px_bool PX_LoadJsonFromJsonFile(px_memorypool* mp, PX_Json* json, const px_char* path)
{
	PX_IO_Data io = PX_LoadFileToIOData((px_char*)path);
	if (!io.size)
	{
		return PX_FALSE;
	}
	if(!PX_JsonInitialize(mp, json))goto _ERROR;
	if (!PX_JsonParse(json, (px_char*)io.buffer))goto _ERROR;
	PX_FreeIOData(&io);
	return PX_TRUE;
_ERROR:
	PX_FreeIOData(&io);
	return PX_FALSE;
}