#include "PX_Abi.h"

px_void PX_AbiCreateStatic(px_abi* pabi, px_void* pbuffer, px_int size)
{
	PX_memset(pabi, 0, sizeof(px_abi));
	pabi->pstatic_buffer = pbuffer;
	pabi->static_size = size;

}
px_void PX_AbiCreateDynamic(px_abi* pabi, px_memorypool* mp)
{
	PX_memset(pabi, 0, sizeof(px_abi));
	PX_MemoryInitialize(mp, &pabi->dynamic);
}

px_bool PX_AbiWriteMemory(px_memory* pmemory, PX_ABI_TYPE type, const px_char name[], px_void* buffer, px_int buffersize)
{
	px_dword size;
	size = PX_strlen(name) + 1 + buffersize;
	if (!PX_MemoryCat(pmemory, &type, sizeof(type)))
	{
		return PX_FALSE;
	}
	if (!PX_MemoryCat(pmemory, &size, sizeof(size)))
	{
		return PX_FALSE;
	}
	if (!PX_MemoryCat(pmemory, name, PX_strlen(name) + 1))
	{
		return PX_FALSE;
	}
	if (!PX_MemoryCat(pmemory, buffer, buffersize))
	{
		return PX_FALSE;
	}
	return PX_TRUE;
}

px_bool PX_AbiWrite(px_abi* pabi, PX_ABI_TYPE type, const px_char name[], px_void* buffer, px_int buffersize)
{
	if (pabi->dynamic.mp)
	{
		return PX_AbiWriteMemory(&pabi->dynamic, type, name, buffer, buffersize);
	}
	else
	{
		px_dword size;
		size = PX_strlen(name) + 1 + buffersize;
		if (pabi->static_write_offset + sizeof(type) + sizeof(size) + PX_strlen(name) + 1 + buffersize > pabi->static_size)
		{
			return PX_FALSE;
		}
		PX_memcpy(pabi->pstatic_buffer + pabi->static_write_offset, &type, sizeof(type));
		pabi->static_write_offset += sizeof(type);
		PX_memcpy(pabi->pstatic_buffer + pabi->static_write_offset, &size, sizeof(size));
		pabi->static_write_offset += sizeof(size);
		PX_memcpy(pabi->pstatic_buffer + pabi->static_write_offset, name, PX_strlen(name) + 1);
		pabi->static_write_offset += PX_strlen(name) + 1;
		PX_memcpy(pabi->pstatic_buffer + pabi->static_write_offset, buffer, buffersize);
		pabi->static_write_offset += buffersize;
		return PX_TRUE;
	}
}




px_bool PX_AbiWrite_int(px_abi* pabi, const px_char name[], px_int _int)
{
	return PX_AbiWrite(pabi, PX_ABI_TYPE_INT, name, &_int, sizeof(_int));
}
px_bool PX_AbiWrite_ptr(px_abi* pabi, const px_char name[], px_void* ptr)
{
	return PX_AbiWrite(pabi, PX_ABI_TYPE_PTR, name, &ptr, sizeof(ptr));
}
px_bool PX_AbiWrite_float(px_abi* pabi, const px_char name[], px_float _float)
{
	return PX_AbiWrite(pabi, PX_ABI_TYPE_FLOAT, name, &_float, sizeof(_float));
}
px_bool PX_AbiWrite_double(px_abi* pabi, const px_char name[], px_double _double)
{
	return PX_AbiWrite(pabi, PX_ABI_TYPE_DOUBLE, name, &_double, sizeof(_double));
}
px_bool PX_AbiWrite_string(px_abi* pabi, const px_char name[], const px_char _string[])
{
	return PX_AbiWrite(pabi, PX_ABI_TYPE_STRING, name, (px_void*)_string, PX_strlen(_string) + 1);
}
px_bool PX_AbiWrite_point(px_abi* pabi, const px_char name[], px_point point)
{
	return PX_AbiWrite(pabi, PX_ABI_TYPE_POINT, name, &point, sizeof(point));
}
px_bool PX_AbiWrite_color(px_abi* pabi, const px_char name[], px_color color)
{
	return PX_AbiWrite(pabi, PX_ABI_TYPE_COLOR, name, &color, sizeof(color));
}
px_bool PX_AbiWrite_bool(px_abi* pabi, const px_char name[], px_bool _bool)
{
	return PX_AbiWrite(pabi, PX_ABI_TYPE_BOOL, name, &_bool, sizeof(_bool));
}
px_bool PX_AbiWrite_data(px_abi* pabi, const px_char name[], px_void* data, px_int size)
{
	return PX_AbiWrite(pabi, PX_ABI_TYPE_DATA, name, data, size);
}

px_bool PX_AbiMemoryWrite_int(px_memory* pmem, const px_char name[], px_int _int)
{
	return PX_AbiWriteMemory(pmem, PX_ABI_TYPE_INT, name, &_int, sizeof(_int));
}
px_bool PX_AbiMemoryWrite_ptr(px_memory* pmem, const px_char name[], px_void* ptr)
{
	return PX_AbiWriteMemory(pmem, PX_ABI_TYPE_PTR, name, &ptr, sizeof(ptr));
}
px_bool PX_AbiMemoryWrite_float(px_memory* pmem, const px_char name[], px_float _float)
{
	return PX_AbiWriteMemory(pmem, PX_ABI_TYPE_FLOAT, name, &_float, sizeof(_float));
}
px_bool PX_AbiMemoryWrite_double(px_memory* pmem, const px_char name[], px_double _double)
{
	return PX_AbiWriteMemory(pmem, PX_ABI_TYPE_DOUBLE, name, &_double, sizeof(_double));
}
px_bool PX_AbiMemoryWrite_string(px_memory* pmem, const px_char name[], const px_char _string[])
{
	return PX_AbiWriteMemory(pmem, PX_ABI_TYPE_STRING, name, (px_void*)_string, PX_strlen(_string) + 1);
}
px_bool PX_AbiMemoryWrite_point(px_memory* pmem, const px_char name[], px_point point)
{
	return PX_AbiWriteMemory(pmem, PX_ABI_TYPE_POINT, name, &point, sizeof(point));
}
px_bool PX_AbiMemoryWrite_color(px_memory* pmem, const px_char name[], px_color color)
{
	return PX_AbiWriteMemory(pmem, PX_ABI_TYPE_COLOR, name, &color, sizeof(color));
}
px_bool PX_AbiMemoryWrite_bool(px_memory* pmem, const px_char name[], px_bool _bool)
{
	return PX_AbiWriteMemory(pmem, PX_ABI_TYPE_BOOL, name, &_bool, sizeof(_bool));
}
px_bool PX_AbiMemoryWrite_data(px_memory* pmem, const px_char name[], px_void* data, px_int size)
{
	return PX_AbiWriteMemory(pmem, PX_ABI_TYPE_DATA, name, data, size);
}

px_void* PX_AbiRead(px_abi* pabi, PX_ABI_TYPE type, const px_char name[])
{
	PX_ABI_TYPE *ptype;
	px_int readoffset=0;
	px_int readsize;
	px_byte* pbuffer;
	if (!pabi)
	{
		return PX_NULL;
	}
	if (pabi->dynamic.mp)
	{
		pbuffer = pabi->dynamic.buffer;
		readsize = pabi->dynamic.usedsize;
	}
	else
	{
		pbuffer = pabi->pstatic_buffer;
		readsize = pabi->static_size;
	}

	while (readoffset<readsize)
	{
		px_dword size;
		ptype = (PX_ABI_TYPE *)(pbuffer + readoffset);
		readoffset += sizeof(PX_ABI_TYPE);
		PX_memcpy(&size, pbuffer + readoffset, sizeof(size));
		readoffset += sizeof(size);
		if (*ptype == type)
		{
			if (PX_strequ((px_char*)(pbuffer + readoffset), name))
			{
				readoffset += PX_strlen(name) + 1;
				return pbuffer + readoffset;
			}
			else
			{
				readoffset += size;
			}
		}
		else
		{
			readoffset += size;
		}
	}
	return PX_NULL;
}

px_bool PX_AbiRead_int(px_abi* pabi, const px_char name[], px_int* _int)
{
	px_int *pint=(px_int*)PX_AbiRead(pabi, PX_ABI_TYPE_INT, name);
	if (pint)
	{
		*_int = *pint;
		return PX_TRUE;
	}
	return PX_FALSE;
}

px_bool PX_AbiRead_ptr(px_abi* pabi, const px_char name[], px_void** pptr)
{
	px_void** ptr = (px_void**)PX_AbiRead(pabi, PX_ABI_TYPE_PTR, name);
	if (ptr)
	{
		*pptr = *ptr;
		return PX_TRUE;
	}
	return PX_FALSE;

}
px_bool PX_AbiRead_float(px_abi* pabi, const px_char name[], px_float* _float)
{
	px_float *pfloat = (px_float*)PX_AbiRead(pabi, PX_ABI_TYPE_FLOAT, name);
	if (pfloat)
	{
		*_float = *pfloat;
		return PX_TRUE;
	}
	return PX_FALSE;
}
px_bool PX_AbiRead_double(px_abi* pabi, const px_char name[], px_double* _double)
{
	px_double *pdouble = (px_double*)PX_AbiRead(pabi, PX_ABI_TYPE_DOUBLE, name);
	if (pdouble)
	{
		*_double = *pdouble;
		return PX_TRUE;
	}
	return PX_FALSE;

}
px_bool PX_AbiRead_string(px_abi* pabi, const px_char name[], px_char *_string[])
{
	px_char *pstring = (px_char*)PX_AbiRead(pabi, PX_ABI_TYPE_STRING, name);
	if (pstring)
	{
		*_string = pstring;
		return PX_TRUE;
	}
	return PX_FALSE;
}
px_bool PX_AbiRead_point(px_abi* pabi, const px_char name[], px_point* point)
{
	px_point *ppoint = (px_point*)PX_AbiRead(pabi, PX_ABI_TYPE_POINT, name);
	if (ppoint)
	{
		*point = *ppoint;
		return PX_TRUE;
	}
	return PX_FALSE;

}

px_bool PX_AbiRead_color(px_abi* pabi, const px_char name[], px_color* color)
{
	px_color *pcolor = (px_color*)PX_AbiRead(pabi, PX_ABI_TYPE_COLOR, name);
	if (pcolor)
	{
		*color = *pcolor;
		return PX_TRUE;
	}
	return PX_FALSE;

}
px_bool PX_AbiRead_bool(px_abi* pabi, const px_char name[], px_bool* _bool)
{
	px_bool *pbool = (px_bool*)PX_AbiRead(pabi, PX_ABI_TYPE_BOOL, name);
	if (pbool)
	{
		*_bool = *pbool;
		return PX_TRUE;
	}
	return PX_FALSE;
}
px_bool PX_AbiRead_data(px_abi* pabi, const px_char name[], px_void* data, px_int size)
{
	px_void *pdata = PX_AbiRead(pabi, PX_ABI_TYPE_DATA, name);
	if (pdata)
	{
		PX_memcpy(data, pdata, size);
		return PX_TRUE;
	}
	return PX_FALSE;
}

px_byte *PX_AbiReadNext_desc(px_byte* pbuffer, px_abi_desc *pdesc)
{
	pdesc->type = *(PX_ABI_TYPE*)(pbuffer);
	pbuffer += sizeof(PX_ABI_TYPE);
	PX_memcpy(&pdesc->size, pbuffer, sizeof(pdesc->size));
	pbuffer += sizeof(pdesc->size);
	pdesc->name = (px_char*)(pbuffer);
	pdesc->buffer = pbuffer+ PX_strlen(pdesc->name) + 1;
	pbuffer += pdesc->size;
	return pbuffer;
}

px_void PX_AbiDynamicFree(px_abi* pabi)
{
	if (pabi->dynamic.mp)
	{
		PX_MemoryFree(&pabi->dynamic);
	}
	PX_memset(pabi, 0, sizeof(px_abi));
}