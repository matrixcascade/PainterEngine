#include "../modules/px_file.h"
#include "px_uefi.h"

EFI_DEVICE_PATH_PROTOCOL  *mCurrentImageDirDp = NULL;

EFI_DEVICE_PATH_PROTOCOL *
GetCurrentImageDirDp (
  EFI_HANDLE  EfiImageHandle
  )
{
  EFI_LOADED_IMAGE_PROTOCOL *LoadedImage;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
  EFI_STATUS                Status;

  Status = gBS->HandleProtocol(
                  EfiImageHandle,
                  &gEfiLoadedImageProtocolGuid,
                  (VOID**)&LoadedImage
                  );
  if (!EFI_ERROR(Status)) {
    CHAR16 *DirPath = AllocateCopyPool (StrSize (((FILEPATH_DEVICE_PATH *)LoadedImage->FilePath)->PathName), ((FILEPATH_DEVICE_PATH *)LoadedImage->FilePath)->PathName);
    if (PathRemoveLastItem (DirPath)) {
      DevicePath = FileDevicePath(LoadedImage->DeviceHandle, DirPath);
      FreePool (DirPath);
      return DevicePath;
    }
  }
  
  return NULL;
}

/**

  Function gets the file information from an open file descriptor, and stores it
  in a buffer allocated from pool.

  @param FHand           File Handle.
  @param InfoType        Info type need to get.

  @retval                A pointer to a buffer with file information or NULL is returned

**/
VOID *
LibFileInfo (
  IN EFI_FILE_HANDLE  FHand,
  IN EFI_GUID         *InfoType
  )
{
  EFI_STATUS     Status;
  EFI_FILE_INFO  *Buffer;
  UINTN          BufferSize;

  Buffer     = NULL;
  BufferSize = 0;

  Status = FHand->GetInfo (
                    FHand,
                    InfoType,
                    &BufferSize,
                    Buffer
                    );
  if (Status == EFI_BUFFER_TOO_SMALL) {
    Buffer = AllocatePool (BufferSize);
    ASSERT (Buffer != NULL);
  }

  Status = FHand->GetInfo (
                    FHand,
                    InfoType,
                    &BufferSize,
                    Buffer
                    );

  return Buffer;
}

PX_IO_Data PX_LoadFileToIOData(const char path[])
{
  PX_IO_Data io;
  EFI_DEVICE_PATH_PROTOCOL  *FileDp;
  EFI_STATUS                Status;
  EFI_FILE_HANDLE           FileHandle;
  CHAR16                    *Char16Path;

  io.buffer=0;
  io.size=0;
  
  if (mCurrentImageDirDp == NULL) {
    mCurrentImageDirDp = GetCurrentImageDirDp (mImageHandle);
  }

  if (mCurrentImageDirDp) {
    Char16Path = AllocateZeroPool (AsciiStrSize(path) * sizeof(CHAR16));
    AsciiStrToUnicodeStrS(path, Char16Path, (AsciiStrSize(path) * sizeof(CHAR16)));
    FileDp = AppendDevicePath(mCurrentImageDirDp, ConvertTextToDevicePath(Char16Path));
    Status = EfiOpenFileByDevicePath (&FileDp, &FileHandle, EFI_FILE_MODE_READ, 0);
    if (!EFI_ERROR(Status)) {
      EFI_FILE_INFO * FileInfo = LibFileInfo (FileHandle, &gEfiFileInfoGuid);
      UINTN FileBufferSize = FileInfo->FileSize;
      UINT8 *FileBuffer = AllocateZeroPool(FileBufferSize);
      Status = FileHandle->Read (FileHandle, &FileBufferSize, FileBuffer);
      if (!EFI_ERROR(Status)) {
        io.buffer = FileBuffer;
        io.size = FileBufferSize;
      }
    }
  }

  return io;
}

void PX_FreeIOData(PX_IO_Data *io)
{
	if (io->size&&io->buffer)
	{
		FreePool(io->buffer);
		io->size=0;
		io->buffer=0;
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

px_bool PX_LoadVMFromScriptFile(px_memorypool *mp,const px_char path[], PX_VM *pvm,const px_char entry[])
{
	PX_Compiler compiler;
	px_memory bin;
	PX_IO_Data io = PX_LoadFileToIOData(path);
	if (!io.size)
	{
		return PX_FALSE;
	}
	PX_MemoryInitialize(mp, &bin);
	
	if (!PX_CompilerInitialize(mp, &compiler))
	{
		PX_FreeIOData(&io);
		return PX_FALSE;
	}
	
	if (!PX_CompilerAddSource(&compiler, (const px_char *)io.buffer))
		goto _ERROR;

	if (!PX_CompilerCompile(&compiler, &bin, 0, entry))
	{
		DEBUG ((DEBUG_ERROR, "%a", compiler.error));
		goto _ERROR;
	}
		

	if (!PX_VMInitialize(pvm, mp, bin.buffer, bin.usedsize))
		goto _ERROR;

	PX_MemoryFree(&bin);
	PX_CompilerFree(&compiler);
	PX_FreeIOData(&io);
	return PX_TRUE;
_ERROR:
	PX_MemoryFree(&bin);
	PX_CompilerFree(&compiler);
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

px_bool PX_LoadJsonToResource(PX_ResourceLibrary* ResourceLibrary, const px_char Path[], const px_char key[])
{
	PX_IO_Data io;
	io = PX_LoadFileToIOData(Path);
	if (!io.size)goto _ERROR;
	if (!PX_ResourceLibraryLoad(ResourceLibrary, PX_RESOURCE_TYPE_JSON, io.buffer, io.size, key)) goto _ERROR;
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

px_bool PX_LoadDataToResource(PX_ResourceLibrary* ResourceLibrary, const px_char Path[], const px_char key[])
{
	PX_IO_Data io;
	io = PX_LoadFileToIOData(Path);
	if (!io.size)goto _ERROR;
	if (!PX_ResourceLibraryLoad(ResourceLibrary, PX_RESOURCE_TYPE_DATA, io.buffer, io.size, key)) goto _ERROR;
	PX_FreeIOData(&io);
	return PX_TRUE;
_ERROR:
	PX_FreeIOData(&io);
	return PX_FALSE;
}
