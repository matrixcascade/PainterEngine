#include "architecture/PainterEngine.h"
#include "platform/modules/px_file.h"

px_byte __runtime[1024 * 1024 * 32];
px_byte __runtime2[1024 * 1024 * 16];

px_void _PX_MP_Errorcall(px_memorypool* mp, PX_MEMORYPOOL_ERROR errcode)
{
	printf("Memory pool error code:%d\n", errcode);
	exit(0);
}

px_bool PX_2dx_Make(const char *source,const char *target)
{
	PX_IO_Data io;
	px_memorypool mp=MP_Create(__runtime,sizeof(__runtime));
	px_memorypool mp2=MP_Create(__runtime2,sizeof(__runtime2));
	px_memory bin;
	px_memory out;
	px_vector texinfos, taginfos;
	px_int i;

	MP_ErrorCatch(&mp, _PX_MP_Errorcall, 0);
	MP_ErrorCatch(&mp2, _PX_MP_Errorcall, 0);

	PX_MemoryInitialize(&mp,&bin);
	PX_VectorInitialize(&mp,&texinfos,sizeof(PX_2dxMake_textureInfo),16);
	PX_VectorInitialize(&mp, &taginfos, sizeof(PX_2dxMake_tagInfo), 16);


	io=PX_LoadFileToIOData(source);
	if (io.size == 0)
	{
		return PX_FALSE;
	}

	if (PX_AnimationShellCompile(&mp2,io.buffer,&taginfos,&texinfos,&bin))
	{
		PX_2DX_Header header;
		PX_FreeIOData(&io);

		PX_MemoryInitialize(&mp2, &out);
		header.magic = *(px_dword*)"2DX";
		header.framecount = texinfos.size;
		header.animationCount = taginfos.size;
		PX_MemoryCat(&out, &header, sizeof(PX_2DX_Header));

		for ( i = 0; i < taginfos.size; i++)
		{
			PX_2dxMake_tagInfo* ptag = PX_VECTORAT(PX_2dxMake_tagInfo, &taginfos, i);
			PX_MemoryCat(&out, &ptag->addr, sizeof(ptag->addr));
			PX_MemoryCat(&out, &ptag->tag, PX_strlen(ptag->tag)+1);
		}

		for (i = 0; i < texinfos.size; i++)
		{
			px_char path[260] = { 0 };
			px_dword size;
			PX_2dxMake_textureInfo* ptex = PX_VECTORAT(PX_2dxMake_textureInfo, &texinfos, i);
			PX_FileGetPath(source, path, sizeof(path));
			PX_strcat(path, "/");
			PX_strcat(path, ptex->path);
			io = PX_LoadFileToIOData(path);
			if (io.size == 0)
			{
				printf("Load texture %s failed.\n", path);
				return PX_FALSE;
			}
			size = PX_PngGetSize(io.buffer, io.size);
			if (size == 0)
			{
				size = PX_TRawGetSize((PX_TRaw_Header *)io.buffer);
			}
			if (size)
			{
				PX_MemoryCat(&out, io.buffer, size);
			}
			else
			{
				printf("Load texture %s failed.\n", ptex->path);
			}

			PX_FreeIOData(&io);
		}
		PX_MemoryCat(&out, &bin.usedsize, sizeof(bin.usedsize));
		PX_MemoryCat(&out, bin.buffer, bin.usedsize);
		PX_SaveDataToFile(out.buffer, out.usedsize, target);
	}
	else
	{
		PX_FreeIOData(&io);
		return PX_FALSE;
	}
	return PX_TRUE;
}


int main( int argc, char *argv[])
{
	PX_2dx_Make("D:\\demo\\makefile.txt", "D:\\demo\\out");
	return;
	if (argc!=3)
	{
		printf("parameter error.\n");
		return 1;
	}
	if (PX_2dx_Make(argv[1],argv[2]))
	{
		printf("Compile done.\n");
		return 0;
	}
	else
	{
		printf("Compile error.\n");
		return 1;
	}
	
}