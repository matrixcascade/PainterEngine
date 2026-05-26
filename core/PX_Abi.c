#include "PX_Abi.h"

// px_dword  PX_ABI_TYPE   px_char*    px_byte*
//   size=sizeof(dword) + sizeof(PX_ABI_TYPE) + name + data
//|----------------------------------------------|
//|   size   |  type  |   name \0|   data        |
//|----------------------------------------------|
px_void PX_AbiCreate_StaticWriter(px_abi* pabi, px_void* pbuffer, px_int size)
{
	PX_memset(pabi, 0, sizeof(px_abi));
	pabi->pstatic_buffer = (px_byte *)pbuffer;
	pabi->static_size = size;
}

px_void PX_AbiCreate_StaticReader(px_abi* pabi,const px_void* pbuffer, px_int size)
{
	PX_memset(pabi, 0, sizeof(px_abi));
	pabi->pstatic_buffer = (px_byte *)pbuffer;
	pabi->static_size = size;
	pabi->static_used_size = size;
}

px_void PX_AbiCreate_DynamicWriter(px_abi* pabi, px_memorypool* mp)
{
	PX_memset(pabi, 0, sizeof(px_abi));
	PX_MemoryInitialize(mp, &pabi->dynamic);
}

px_void PX_AbiCreate_MemoryWriter(px_abi* pabi, px_memory* mem)
{
	pabi->dynamic = *mem;
}

px_void PX_AbiClear(px_abi* pabi)
{
	if (!pabi)
	{
		return;
	}
	if (pabi->dynamic.mp)
	{
		PX_MemoryClear(&pabi->dynamic);
	}
	else
	{
		pabi->static_used_size = 0;
	}
	
}

px_bool PX_AbiCopy_FromBuffer(px_abi* pabi, const px_void* pbuffer, px_int size)
{
	if (!pabi || !pbuffer || size <= 0)
	{
		return PX_FALSE;
	}
	if (pabi->dynamic.mp)
	{
		if (!PX_MemoryResize(&pabi->dynamic, size))
		{
			return PX_FALSE;
		}
		PX_memcpy(pabi->dynamic.buffer, pbuffer, size);
		pabi->dynamic.usedsize = size;
	}
	else
	{
		if (size > pabi->static_size)
		{
			return PX_FALSE;
		}
		PX_memcpy(pabi->pstatic_buffer, pbuffer, size);
		pabi->static_used_size = size;
	}
	return PX_TRUE;
}

px_bool PX_AbiCopy_FromAbi(px_abi* pabi,px_abi *pCopyFrom)
{
	if (!pabi || !pCopyFrom)
	{
		return PX_FALSE;
	}
	return PX_AbiCopy_FromBuffer(pabi, PX_AbiGet_Pointer(pCopyFrom), PX_AbiGet_Size(pCopyFrom));

}

px_bool PX_AbiCopy_FromAbiMember(px_abi* pabi, px_abi* pCopyFrom, const px_char payload[])
{
	PX_ABI_TYPE type;
	px_dword datasize;
	const px_byte* payloaddata;
	if (!pabi || !pCopyFrom || !payload)
	{
		return PX_FALSE;
	}
	payloaddata=PX_AbiGet(pCopyFrom, &type, &datasize, payload);
	if (!payloaddata)
	{
		return PX_FALSE;
	}
	return PX_AbiSet(pabi, type, payload, (px_void*)payloaddata, datasize);
	
}

px_bool PX_AbiCopy_FromAbiMemberAll(px_abi* pabi, px_abi* pCopyFrom)
{
	return PX_AbiCopy_FromBuffer(pabi, PX_AbiGet_Pointer(pCopyFrom), PX_AbiGet_Size(pCopyFrom));
}

px_int  PX_AbiGet_Size(px_abi* pabi)
{
	if (pabi->dynamic.mp)
	{
		return pabi->dynamic.usedsize;
	}
	else
	{
		return pabi->static_used_size;
	}
}

px_byte *PX_AbiGet_Pointer(px_abi* pabi)
{
	if (pabi->dynamic.mp)
	{
		return pabi->dynamic.buffer;
	}
	else
	{
		return pabi->pstatic_buffer;
	}
}

px_int PX_AbiPointer_GetDataOffset(px_byte* pStartBuffer)
{
	const px_char* pname = (px_char*)(pStartBuffer + sizeof(px_dword) + sizeof(PX_ABI_TYPE));
	return sizeof(px_dword) + sizeof(PX_ABI_TYPE) + PX_strlen(pname) + 1;
}

px_byte* PX_AbiPointer_GetDataPointer(px_byte* pStartBuffer)
{
	return pStartBuffer + PX_AbiPointer_GetDataOffset(pStartBuffer);
}

const px_char* PX_AbiPointer_GetName(px_byte* pStartBuffer)
{
	return (const px_char*)(pStartBuffer + sizeof(px_dword) + sizeof(PX_ABI_TYPE));
}

px_dword PX_AbiPointer_GetDataSize(px_byte* pStartBuffer)
{
	return *(px_dword*)(pStartBuffer)-sizeof(px_dword) - sizeof(PX_ABI_TYPE) - PX_strlen((px_char*)(pStartBuffer + sizeof(px_dword) + sizeof(PX_ABI_TYPE)))-1;
}

px_void PX_AbiPointer_SetAbiSize(px_byte* pStartBuffer, px_int size)
{
	*(px_dword*)(pStartBuffer) = size;
}

px_dword PX_AbiPointer_GetAbiSize(px_byte* pStartBuffer)
{
	return *(px_dword*)(pStartBuffer);
}

PX_ABI_TYPE PX_AbiPointer_GetType(px_byte* pStartBuffer)
{
	return *(PX_ABI_TYPE*)(pStartBuffer + sizeof(px_dword));
}

static px_int PX_AbiGet_AbiMemberOffsetWithType(px_abi* pabi, const px_char name[],PX_ABI_TYPE type)
{
	px_dword readoffset = 0;
	px_dword readsize;
	px_byte* pbuffer;
	if (!pabi)
	{
		PX_ASSERTX("pabi is null");
		return -1;
	}
	pbuffer = PX_AbiGet_Pointer(pabi);
	readsize = PX_AbiGet_Size(pabi);

	while (readoffset < readsize)
	{
		px_dword size;
		const px_char* pname;
		PX_ABI_TYPE current_type;
		px_int current_abi_offset = readoffset;

		//size
		PX_memcpy(&size, pbuffer + readoffset, sizeof(size));
		if (size == 0 || readoffset + size > readsize)
		{
			return -1;
		}
		readoffset += sizeof(px_dword);
		if (readoffset >= readsize)
			return -1;

		//type
		current_type = *(PX_ABI_TYPE*)(pbuffer + readoffset);
		readoffset += sizeof(PX_ABI_TYPE);
		if (readoffset >= readsize)
			return -1;

		//name
		pname = (px_char*)(pbuffer + readoffset);
		
		if (PX_strequ(pname, name)&& current_type==type)
		{
			px_int _strlen = PX_strlen(name) + 1;
			if (_strlen == 1)
			{
				return -1;
			}
			if (readoffset + _strlen > readsize)
			{
				return -1;
			}
			return  current_abi_offset;
		}
		else
		{
			readoffset = current_abi_offset + size;
			if (readoffset >= readsize)
			{
				return -1;
			}
		}
	}
	return -1;
}

static px_int PX_AbiGet_AbiMemberOffset(px_abi* pabi, const px_char name[])
{
	px_dword readoffset = 0;
	px_dword readsize;
	px_byte* pbuffer;
	if (!pabi)
	{
		PX_ASSERTX("pabi is null");
		return -1;
	}
	pbuffer = PX_AbiGet_Pointer(pabi);
	readsize = PX_AbiGet_Size(pabi);

	while (readoffset < readsize)
	{
		px_dword size;
		const px_char* pname;
		px_int current_abi_offset = readoffset;

		//size
		PX_memcpy(&size, pbuffer + readoffset, sizeof(size));
		if (size == 0 || readoffset + size > readsize)
		{
			return -1;
		}
		readoffset += sizeof(px_dword);
		if (readoffset >= readsize)
			return -1;

		//type
		readoffset += sizeof(PX_ABI_TYPE);
		if (readoffset >= readsize)
			return -1;

		//name
		pname = (px_char*)(pbuffer + readoffset);
		if (PX_strequ(pname, name))
		{
			px_int _strlen = PX_strlen(name) + 1;
			if (_strlen == 1)
			{
				return -1;
			}
			if (readoffset + _strlen > readsize)
			{
				return -1;
			}
			return  current_abi_offset;
		}
		else
		{
			readoffset = current_abi_offset+size;
			if (readoffset >= readsize)
			{
				return -1;
			}
		}
	}
	return -1;
}

static px_byte* PX_AbiGet_AbiMemberPointer(px_abi* pabi, const px_char name[])
{
	px_int offset = PX_AbiGet_AbiMemberOffset(pabi, name);
	if (offset == -1)
	{
		return PX_NULL;
	}
	return PX_AbiGet_Pointer(pabi) + offset;
	
}

static px_byte* PX_AbiGet_AbiMemberDataPointer(px_abi* pabi, PX_ABI_TYPE* ptype, const px_char name[], px_dword* pdatasize)
{
	px_byte* pmemberptr = (px_byte*)PX_AbiGet_AbiMemberPointer(pabi, name);
	px_byte* pmemberdataptr;
	if (!pmemberptr) return PX_NULL;

	*ptype = PX_AbiPointer_GetType(pmemberptr);
	*pdatasize = PX_AbiPointer_GetDataSize(pmemberptr);
	pmemberdataptr = PX_AbiPointer_GetDataPointer(pmemberptr);

	if (*ptype == PX_ABI_TYPE_STRING)
	{
		px_char* pstr = (px_char*)(pmemberdataptr);
		if (pstr[*pdatasize - 1] == 0)
		{
			return pmemberdataptr;
		}
		else
		{
			return PX_NULL;
		}
	}
	else
		return pmemberdataptr;
}

px_int PX_AbiGet_PayloadOffsetWithType(px_abi* pabi, PX_ABI_TYPE type, px_dword* pdatasize, const px_char _payload[])
{
	px_int r_offset = 0;
	px_int s_offset = 0;
	px_int abs_offset = 0;
	px_char payload[512] = { 0 };
	px_char* lexeme = PX_NULL;
	px_int   i = 0;
	px_abi   current_abi;
	px_byte* abs_ptr = PX_AbiGet_Pointer(pabi);
	current_abi = *pabi;

	if (PX_strlen(_payload) >= sizeof(payload))
	{
		return PX_FALSE;
	}

	if (_payload[0] == '.')
	{
		PX_strset(payload, _payload + 1);
	}
	else
	{
		PX_strset(payload, _payload);
	}


	while (payload[r_offset] != 0)
	{
		px_int ref_offset;

		while (PX_TRUE)
		{
			if (payload[s_offset] == '.')
			{
				payload[s_offset] = '\0';
				s_offset++;
				break;
			}
			if (payload[s_offset] == '\0')
			{
				lexeme = payload + r_offset;
				ref_offset = PX_AbiGet_AbiMemberOffsetWithType(&current_abi, lexeme,type);
				if (ref_offset == -1)
				{
					return -1;
				}

				if (pdatasize)
				{
					*pdatasize = PX_AbiPointer_GetDataSize(abs_ptr + abs_offset + ref_offset);
				}
				return abs_offset + ref_offset;
			}
			s_offset++;
		}

		lexeme = payload + r_offset;

		ref_offset = PX_AbiGet_AbiMemberOffsetWithType(&current_abi, lexeme, PX_ABI_TYPE_ABI);
		if (ref_offset != -1)
		{
			px_dword datasize;
			px_byte* pdata;
			abs_offset += ref_offset;
			datasize = PX_AbiPointer_GetDataSize(abs_ptr + abs_offset);
			if (datasize == 0)
			{
				return -1;
			}
			pdata = PX_AbiPointer_GetDataPointer(abs_ptr + abs_offset);
			ref_offset = PX_AbiPointer_GetDataOffset(abs_ptr + abs_offset);
			abs_offset += ref_offset;
			PX_AbiCreate_StaticReader(&current_abi, pdata, datasize);
		}
		else
		{
			return -1;
		}
		r_offset = s_offset;
	}
	return -1;
}

px_int PX_AbiGet_PayloadOffset(px_abi* pabi, PX_ABI_TYPE* ptype, px_dword* pdatasize, const px_char _payload[])
{
	px_int r_offset = 0;
	px_int s_offset = 0;
	px_int abs_offset = 0;
	px_char payload[512] = { 0 };
	px_char* lexeme = PX_NULL;
	px_int   i = 0;
	px_abi   current_abi;
	px_byte* abs_ptr = PX_AbiGet_Pointer(pabi);
	current_abi = *pabi;
	
	if (PX_strlen(_payload) >= sizeof(payload))
	{
		return PX_FALSE;
	}

	if (_payload[0] == '.')
	{
		PX_strset(payload, _payload+1);
	}
	else
	{
		PX_strset(payload, _payload);
	}
	

	while (payload[r_offset] != 0)
	{
		px_int ref_offset;
		while (PX_TRUE)
		{
			if (payload[s_offset] == '.')
			{
				payload[s_offset] = '\0';
				s_offset++;
				break;
			}
			if (payload[s_offset] == '\0')
			{
				lexeme = payload + r_offset;
				ref_offset=PX_AbiGet_AbiMemberOffset(&current_abi, lexeme);
				if (ref_offset==-1)
				{
					return -1;
				}
				if (ptype)
				{
					*ptype = PX_AbiPointer_GetType(abs_ptr + abs_offset + ref_offset);
				}
				if (pdatasize)
				{
					*pdatasize = PX_AbiPointer_GetDataSize(abs_ptr + abs_offset + ref_offset);
				}
				return abs_offset + ref_offset;
			}
			s_offset++;
		}

		lexeme = payload + r_offset;

		ref_offset = PX_AbiGet_AbiMemberOffset(&current_abi, lexeme);
		if (ref_offset!=-1)
		{
			PX_ABI_TYPE type;
			px_dword datasize;
			px_byte* pdata;
			abs_offset += ref_offset;
			datasize=PX_AbiPointer_GetDataSize(abs_ptr + abs_offset);
			if (datasize==0)
			{
				return -1;
			}
			type = PX_AbiPointer_GetType(abs_ptr + abs_offset);
			pdata = PX_AbiPointer_GetDataPointer(abs_ptr + abs_offset);
			ref_offset = PX_AbiPointer_GetDataOffset(abs_ptr + abs_offset);
			abs_offset += ref_offset;
			if (type==PX_ABI_TYPE_ABI)
			{
				PX_AbiCreate_StaticReader(&current_abi, pdata, datasize);
			}
			else
			{
				return -1;
			}
		}
		else
		{
			return -1;
		}
		r_offset = s_offset;
	}
	return -1;
}

px_int PX_AbiGet_PayloadDataOffset(px_abi* pabi, PX_ABI_TYPE* ptype, px_dword* psize, const px_char _payload[])
{
	px_int offset = PX_AbiGet_PayloadOffset(pabi, ptype, psize, _payload);
	if (offset == -1)
	{
		return -1;
	}
	return offset + sizeof(px_dword) + sizeof(PX_ABI_TYPE) + PX_strlen((px_char*)(PX_AbiGet_Pointer(pabi) + offset + sizeof(px_dword) + sizeof(PX_ABI_TYPE))) + 1;
}

px_int PX_AbiGet_PayloadDataOffsetWithType(px_abi* pabi, PX_ABI_TYPE type, px_dword* psize, const px_char _payload[])
{
	px_int offset = PX_AbiGet_PayloadOffsetWithType(pabi, type, psize, _payload);
	if (offset == -1)
	{
		return -1;
	}
	return offset + sizeof(px_dword) + sizeof(PX_ABI_TYPE) + PX_strlen((px_char*)(PX_AbiGet_Pointer(pabi) + offset + sizeof(px_dword) + sizeof(PX_ABI_TYPE))) + 1;
}

px_byte* PX_AbiGet_PayloadPointer(px_abi* pabi, PX_ABI_TYPE* ptype, px_dword* pdatasize, const px_char _payload[])
{
	px_byte* ptr = PX_AbiGet_Pointer(pabi);
	px_int offset = PX_AbiGet_PayloadOffset(pabi, ptype, pdatasize, _payload);
	if (offset == -1)
	{
		return PX_NULL;
	}
	return ptr + offset;
}

px_byte* PX_AbiGet_PayloadPointerWithType(px_abi* pabi, PX_ABI_TYPE type, px_dword* pdatasize, const px_char _payload[])
{
	px_byte* ptr = PX_AbiGet_Pointer(pabi);
	px_int offset = PX_AbiGet_PayloadOffsetWithType(pabi, type, pdatasize, _payload);
	if (offset == -1)
	{
		return PX_NULL;
	}
	return ptr + offset;
}

 px_byte* PX_AbiGet_PayloadDataPointer(px_abi* pabi, PX_ABI_TYPE* ptype, px_dword* psize, const px_char _payload[])
{
	px_byte* ptr = PX_AbiGet_Pointer(pabi);
	px_int offset = PX_AbiGet_PayloadDataOffset(pabi, ptype, psize, _payload);
	if (offset == -1)
	{
		return PX_NULL;
	}
	return ptr + offset;
}

 px_byte* PX_AbiGet_PayloadDataPointerWithType(px_abi* pabi, PX_ABI_TYPE type, px_dword* psize, const px_char _payload[])
 {
	 px_byte* ptr = PX_AbiGet_Pointer(pabi);
	 px_int offset = PX_AbiGet_PayloadDataOffsetWithType(pabi, type, psize, _payload);
	 if (offset == -1)
	 {
		 return PX_NULL;
	 }
	 return ptr + offset;
 }

px_byte* PX_AbiGet(px_abi* pabi, PX_ABI_TYPE* ptype, px_dword* psize, const px_char _payload[])
{
	return PX_AbiGet_PayloadDataPointer(pabi, ptype, psize, _payload);
}

px_byte* PX_AbiGetWithType(px_abi* pabi, PX_ABI_TYPE type, px_dword* psize, const px_char _payload[])
{
	return PX_AbiGet_PayloadDataPointerWithType(pabi, type, psize, _payload);
}


px_int* PX_AbiGet_int(px_abi* pabi, const px_char payload[])
{
	px_dword datasize;
	px_void* pdata;
	pdata = PX_AbiGet_PayloadDataPointerWithType(pabi, PX_ABI_TYPE_INT, &datasize, payload);
	if (pdata)
	{
		return (px_int*)pdata;
	}
	return 0;
}

px_dword* PX_AbiGet_dword(px_abi* pabi, const px_char payload[])
{
	px_dword datasize;
	px_void* pdata;
	pdata = PX_AbiGet_PayloadDataPointerWithType(pabi, PX_ABI_TYPE_DWORD, &datasize, payload);
	if (pdata)
	{
		return (px_dword*)pdata;
	}
	return 0;
}

px_word* PX_AbiGet_word(px_abi* pabi, const px_char payload[])
{
	px_dword datasize;
	px_void* pdata;

	pdata = PX_AbiGet_PayloadDataPointerWithType(pabi, PX_ABI_TYPE_WORD, &datasize, payload);
	if (pdata)
	{
		return (px_word*)pdata;
	}
	return 0;
}

px_byte* PX_AbiGet_byte(px_abi* pabi, const px_char payload[])
{
	px_dword datasize;
	px_void* pdata;

	pdata = PX_AbiGet_PayloadDataPointerWithType(pabi, PX_ABI_TYPE_BYTE, &datasize, payload);
	if (pdata )
	{
		return (px_byte*)pdata;
	}
	return 0;
}

px_void** PX_AbiGet_ptr(px_abi* pabi, const px_char payload[])
{
	px_dword datasize;
	px_void* pdata;

	pdata = PX_AbiGet_PayloadDataPointerWithType(pabi, PX_ABI_TYPE_PTR, &datasize, payload);
	if (pdata)
	{
		return (px_void**)pdata;
	}
	return 0;
}

px_float* PX_AbiGet_float(px_abi* pabi, const px_char payload[])
{
	px_dword datasize;
	px_void* pdata;

	pdata = PX_AbiGet_PayloadDataPointerWithType(pabi, PX_ABI_TYPE_FLOAT, &datasize, payload);
	if (pdata)
	{
		return (px_float*)pdata;
	}
	return 0;
}

px_double* PX_AbiGet_double(px_abi* pabi, const px_char payload[])
{
	px_dword datasize;
	px_void* pdata;

	pdata = PX_AbiGet_PayloadDataPointerWithType(pabi, PX_ABI_TYPE_DOUBLE, &datasize, payload);
	if (pdata)
	{
		return (px_double*)pdata;
	}
	return 0;
}

px_int PX_AbiGetValue_int(px_abi* pabi, const px_char payload[])
{
	px_int* pvalue = PX_AbiGet_int(pabi, payload);
	if (!pvalue)
	{
		PX_TERMINATE("AbiGetValue_int failed, payload not found");
		return 0;
	}
	return *pvalue;
}

const px_char* PX_AbiGetValue_string(px_abi* pabi, const px_char payload[])
{
	const px_char* pvalue = PX_AbiGet_string(pabi, payload);
	if (!pvalue)
	{
		PX_TERMINATE("AbiGetValue_string failed, payload not found");
		return 0;
	}
	return pvalue;
}

px_dword PX_AbiGetValue_dword(px_abi* pabi, const px_char payload[]) 
{
	px_dword* pvalue = PX_AbiGet_dword(pabi, payload);
	if (!pvalue)
	{
		PX_TERMINATE("AbiGetValue_dword failed, payload not found");
		return 0;
	}
	return *pvalue;

}
px_word PX_AbiGetValue_word(px_abi* pabi, const px_char payload[])
{
	px_word* pvalue = PX_AbiGet_word(pabi, payload);
	if (!pvalue)
	{
		PX_TERMINATE("AbiGetValue_word failed, payload not found");
		return 0;
	}
	return *pvalue;
}
px_byte PX_AbiGetValue_byte(px_abi* pabi, const px_char payload[])
{
	px_byte* pvalue = PX_AbiGet_byte(pabi, payload);
	if (!pvalue)
	{
		PX_TERMINATE("AbiGetValue_byte failed, payload not found");
		return 0;
	}
	return *pvalue;
}
px_void* PX_AbiGetValue_ptr(px_abi* pabi, const px_char payload[])
{
	px_void** pvalue = PX_AbiGet_ptr(pabi, payload);
	if (!pvalue)
	{
		PX_TERMINATE("AbiGetValue_ptr failed, payload not found");
		return 0;
	}
	return *pvalue;
}
px_float PX_AbiGetValue_float(px_abi* pabi, const px_char payload[])
{
	px_float* pvalue = PX_AbiGet_float(pabi, payload);
	if (!pvalue)
	{
		PX_TERMINATE("AbiGetValue_float failed, payload not found");
		return 0;
	}
	return *pvalue;
}
px_double PX_AbiGetValue_double(px_abi* pabi, const px_char payload[])
{
	px_double* pvalue = PX_AbiGet_double(pabi, payload);
	if (!pvalue)
	{
		PX_TERMINATE("AbiGetValue_double failed, payload not found");
		return 0;
	}
	return *pvalue;
}
px_point PX_AbiGetValue_point(px_abi* pabi, const px_char payload[])
{
	px_point* pvalue = PX_AbiGet_point(pabi, payload);
	if (!pvalue)
	{
		PX_TERMINATE("AbiGetValue_point failed, payload not found");
		return PX_POINT(0, 0, 0);
	}
	return *pvalue;
}
px_color PX_AbiGetValue_color(px_abi* pabi, const px_char payload[])
{
	px_color* pvalue = PX_AbiGet_color(pabi, payload);
	if (!pvalue)
	{
		PX_TERMINATE("AbiGetValue_color failed, payload not found");
		return PX_COLOR(0, 0, 0, 0);
	}
	return *pvalue;
}
px_bool PX_AbiGetValue_bool(px_abi* pabi, const px_char payload[])
{
	px_bool* pvalue = PX_AbiGet_bool(pabi, payload);
	if (!pvalue)
	{
		PX_TERMINATE("AbiGetValue_bool failed, payload not found");
		return PX_FALSE;
	}
	return *pvalue;
}

px_abi PX_AbiGetValue_abireadonly(px_abi* pabi,  const px_char payload[])
{
	px_abi rabi = {0};
	px_bool result = PX_AbiGet_AbiReadOnly(pabi, &rabi, payload);
	if (!result)
	{
		PX_TERMINATE("AbiGetValue_abi failed, payload not found");
		return rabi;
	}
	return rabi;
}

const px_char* PX_AbiGet_string(px_abi* pabi, const px_char payload[])
{
	px_dword datasize;
	px_void* pdata;

	pdata = PX_AbiGet_PayloadDataPointerWithType(pabi, PX_ABI_TYPE_STRING, &datasize, payload);
	if (pdata)
	{
		return (const px_char*)pdata;
	}
	return 0;
}

px_int PX_AbiGet_string_to_int(px_abi* pabi, const px_char payload[])
{
	const px_char* pstring = PX_AbiGet_string(pabi, payload);
	if (pstring)
	{
		return PX_atoi(pstring);
	}
	PX_ERROR("AbiGet_string_to_int failed, payload not found");
	return 0;
	
}

px_float PX_AbiGet_string_to_float(px_abi* pabi, const px_char payload[])
{
	const px_char* pstring = PX_AbiGet_string(pabi, payload);
	if (pstring)
	{
		return PX_atof(pstring);
	}
	PX_ERROR("AbiGet_string_to_float failed, payload not found");
	return 0;
}

px_point* PX_AbiGet_point(px_abi* pabi, const px_char payload[])
{
	px_dword datasize;
	px_void* pdata;
	pdata = PX_AbiGet_PayloadDataPointerWithType(pabi, PX_ABI_TYPE_POINT, &datasize, payload);
	if (pdata)
	{
		return (px_point*)pdata;
	}
	return 0;
}

px_color* PX_AbiGet_color(px_abi* pabi, const px_char payload[])
{
	px_dword datasize;
	px_void* pdata;
	pdata = PX_AbiGet_PayloadDataPointerWithType(pabi, PX_ABI_TYPE_COLOR, &datasize, payload);
	if (pdata)
	{
		return (px_color*)pdata;
	}
	return 0;
}

px_bool* PX_AbiGet_bool(px_abi* pabi, const px_char payload[])
{
	px_dword datasize;
	px_void* pdata;

	pdata = PX_AbiGet_PayloadDataPointerWithType(pabi, PX_ABI_TYPE_BOOL, &datasize, payload);
	if (pdata )
	{
		return (px_bool*)pdata;
	}
	return 0;
}

px_void* PX_AbiGet_data(px_abi* pabi, const px_char payload[], px_dword* size)
{
	px_void* pdata;

	pdata = PX_AbiGet_PayloadDataPointerWithType(pabi, PX_ABI_TYPE_DATA, size, payload);
	if (pdata)
	{
		return (px_void*)pdata;
	}
	return 0;
}

px_bool PX_AbiGet_AbiReadOnly(px_abi* pabi, px_abi* prabi, const px_char name[])
{
	px_dword size;
	px_void* ptr = PX_AbiGet_PayloadDataPointerWithType(pabi, PX_ABI_TYPE_ABI, &size, name);
	if (ptr )
	{
		PX_AbiCreate_StaticReader(prabi, ptr, size);
		return PX_TRUE;
	}
	return PX_FALSE;
}

px_bool PX_AbiCheck(px_abi* pabi)
{
	px_dword abi_size;
	px_dword offset=0;
	px_dword target_abi_size, target_data_size;
	PX_ABI_TYPE type;
	px_byte* ptr,*target_data_ptr;
	abi_size = PX_AbiGet_Size(pabi);
	if (abi_size<=sizeof(px_dword) + sizeof(PX_ABI_TYPE))
	{
		return PX_FALSE;
	}
	
	ptr = PX_AbiGet_Pointer(pabi);
	while (offset<abi_size)
	{
		const px_char* pname;
		px_dword namelen;
		if (abi_size-offset < sizeof(px_dword) + sizeof(PX_ABI_TYPE))
		{
			return PX_FALSE;
		}
		target_abi_size = *(px_dword*)(ptr+offset);
		if (target_abi_size<=sizeof(px_dword) + sizeof(PX_ABI_TYPE))
		{
			return PX_FALSE;
		}
		
		type = *(PX_ABI_TYPE*)(ptr + offset+sizeof(px_dword));
		pname = (px_char*)(ptr + offset + sizeof(px_dword) + sizeof(PX_ABI_TYPE));
		namelen = PX_strlen(pname) + 1;
		if (namelen>target_abi_size- sizeof(px_dword) - sizeof(PX_ABI_TYPE))
		{
			return PX_FALSE;
		}
		target_data_ptr = ptr + offset + sizeof(px_dword) + sizeof(PX_ABI_TYPE) + namelen;
		target_data_size = target_abi_size - sizeof(px_dword) - sizeof(PX_ABI_TYPE) - namelen;
		if (type==PX_ABI_TYPE_STRING)
		{
			if (target_data_ptr[target_data_size - 1] != 0)
				return PX_FALSE;
		}
		else if (type == PX_ABI_TYPE_ABI)
		{
			px_abi subabi;
			PX_AbiCreate_StaticReader(&subabi, target_data_ptr, target_data_size);
			if (!PX_AbiCheck(&subabi))
			{
				return PX_FALSE;
			}
		}
		offset += target_abi_size;
	}
	if (offset != abi_size)
	{
		return PX_FALSE;
	}
	return PX_TRUE;
}



PX_ABI_TYPE PX_AbiType(px_abi* pabi)
{
	return *(PX_ABI_TYPE*)(PX_AbiGet_Pointer(pabi) + sizeof(px_dword));
}

px_bool PX_AbiCheckBufferReady(const px_byte pdata[], px_int size)
{
	px_abi abi;
	PX_AbiCreate_StaticReader(&abi, pdata, size);
	return PX_AbiCheck(&abi);
}

px_bool PX_AbiExist(px_abi* pabi, const px_char payload[])
{
	px_dword size;
	PX_ABI_TYPE rtype;
	px_void* ptr = PX_AbiGet_PayloadDataPointer(pabi, &rtype, &size, payload);
	if (ptr)
	{
		return PX_TRUE;
	}
	return PX_FALSE;
}

px_bool PX_AbiExist_Type(px_abi* pabi, const px_char payload[], PX_ABI_TYPE type)
{
	px_dword size;
	px_void* ptr = PX_AbiGet_PayloadDataPointerWithType(pabi, type, &size, payload);
	if (ptr)
	{
		return PX_TRUE;
	}
	return PX_FALSE;
}

px_bool PX_AbiExist_abi(px_abi* pabi, const px_char payload[])
{
	return PX_AbiExist_Type(pabi, payload, PX_ABI_TYPE_ABI);
}

px_bool PX_AbiExist_string(px_abi* pabi, const px_char payload[], const px_char check[])
{
	const px_char* pstring = PX_AbiGet_string(pabi, payload);
	if (pstring)
	{
		if (PX_strequ(pstring, check))
		{
			return PX_TRUE;
		}
	}
	return PX_FALSE;
}

px_bool PX_AbiExist_data(px_abi* pabi, const px_char payload[], const px_byte data[], px_int datasize)
{
	px_dword size;
	const px_byte* pdata = PX_AbiGet_data(pabi, payload, &size);
	if (pdata&&(px_dword)datasize==size)
	{
		if (PX_memequ(data, pdata, datasize))
		{
			return PX_TRUE;
		}
	}
	return PX_FALSE;
}


px_bool PX_AbiExist_int(px_abi* pabi, const px_char payload[], px_int check)
{
	px_int* pint = PX_AbiGet_int(pabi, payload);
	if (pint)
	{
		if (*pint == check)
		{
			return PX_TRUE;
		}
	}
	return PX_FALSE;
}
px_bool PX_AbiExist_dword(px_abi* pabi, const px_char payload[], px_dword check)
{
	px_dword* pdword = PX_AbiGet_dword(pabi, payload);
	if (pdword)
	{
		if (*pdword == check)
		{
			return PX_TRUE;
		}
	}
	return PX_FALSE;
}
px_bool PX_AbiExist_word(px_abi* pabi, const px_char payload[], px_word check)
{
	px_word* pword = PX_AbiGet_word(pabi, payload);
	if (pword)
	{
		if (*pword == check)
		{
			return PX_TRUE;
		}
	}
	return PX_FALSE;
}
px_bool PX_AbiExist_byte(px_abi* pabi, const px_char payload[], px_byte check)
{
	px_byte* pbyte = PX_AbiGet_byte(pabi, payload);
	if (pbyte)
	{
		if (*pbyte == check)
		{
			return PX_TRUE;
		}
	}
	return PX_FALSE;
}
px_bool PX_AbiExist_ptr(px_abi* pabi, const px_char payload[], px_void* check)
{
	px_void** pptr = PX_AbiGet_ptr(pabi, payload);
	if (pptr)
	{
		if (*pptr == check)
		{
			return PX_TRUE;
		}
	}
	return PX_FALSE;
}
px_bool PX_AbiExist_float(px_abi* pabi, const px_char payload[], px_float check)
{
	px_float* pfloat = PX_AbiGet_float(pabi, payload);
	if (pfloat)
	{
		if (*pfloat == check)
		{
			return PX_TRUE;
		}
	}
	return PX_FALSE;
}
px_bool PX_AbiExist_double(px_abi* pabi, const px_char payload[], px_double check)
{
	px_double* pdouble = PX_AbiGet_double(pabi, payload);
	if (pdouble)
	{
		if (*pdouble == check)
		{
			return PX_TRUE;
		}
	}
	return PX_FALSE;
}
px_bool PX_AbiExist_point(px_abi* pabi, const px_char payload[], px_point check)
{
	px_point* ppoint = PX_AbiGet_point(pabi, payload);
	if (ppoint)
	{
		if (PX_memcmp(ppoint, &check, sizeof(px_point)) == 0)
		{
			return PX_TRUE;
		}
	}
	return PX_FALSE;
}
px_bool PX_AbiExist_color(px_abi* pabi, const px_char payload[], px_color check)
{
	px_color* pcolor = PX_AbiGet_color(pabi, payload);
	if (pcolor)
	{
		if (PX_memcmp(pcolor, &check, sizeof(px_color)) == 0)
		{
			return PX_TRUE;
		}
	}
	return PX_FALSE;
}
px_bool PX_AbiExist_bool(px_abi* pabi, const px_char payload[], px_bool check)
{
	px_bool* pbool = PX_AbiGet_bool(pabi, payload);
	if (pbool)
	{
		if (*pbool == check)
		{
			return PX_TRUE;
		}
	}
	return PX_FALSE;
}

px_int PX_AbiGet_MemberCount(px_abi* pabi)
{
	px_int offset = 0;
	px_int count = 0;
	px_byte* ptr = PX_AbiGet_Pointer(pabi);
	px_int abi_size = PX_AbiGet_Size(pabi);
	while (offset < abi_size)
	{
		px_dword size = PX_AbiPointer_GetAbiSize(ptr + offset);
		if (size==0)
		{
			//invalid abi, size should not be 0
			return 0;
		}

		offset += size;
		count++;
	}
	return count;
}

px_int PX_AbiGet_PayloadMemberCount(px_abi* pabi, const px_char payload[])
{
	px_abi subabi;
	if (!PX_AbiGet_AbiReadOnly(pabi, &subabi, payload))
	{
		return 0;
	}
	return PX_AbiGet_MemberCount(&subabi);
}

px_byte* PX_AbiGet_MemberByIndex(px_abi* pabi, px_int index)
{
	px_int offset = 0;
	px_int count = 0;
	px_byte* ptr = PX_AbiGet_Pointer(pabi);
	px_int abi_size = PX_AbiGet_Size(pabi);
	while (offset < abi_size)
	{
		px_dword size;
		if (count==index)
		{
			return ptr + offset;
		}
		size = PX_AbiPointer_GetAbiSize(ptr + offset);
		if(size==0)
		{
			return PX_NULL;
		}
		offset += size;
		count++;
	}
	return PX_NULL;
}

px_byte* PX_AbiGet_Start(px_abi* pabi)
{
	return PX_AbiGet_Pointer(pabi);
}

px_byte* PX_AbiGet_First(px_abi* pabi)
{
	if (!PX_AbiCheck(pabi))
	{
		return PX_NULL;
	}
	return PX_AbiGet_Pointer(pabi);
}

px_byte* PX_AbiGet_Next(px_abi* pabi, px_byte* pstart)
{
	px_int offset = (px_int)(pstart - PX_AbiGet_Pointer(pabi));
	px_int abisize = PX_AbiGet_Size(pabi);
	px_int current_abi_size = PX_AbiPointer_GetAbiSize(pstart);
	PX_ASSERTIFX(offset < 0, "Abi offset < 0");
	if (offset+ current_abi_size < abisize)
	{
		px_byte* ptr = PX_AbiGet_Pointer(pabi);
		return ptr + offset + current_abi_size;
	}
	else
	{
		return PX_NULL;
	}
}

static px_bool PX_AbiNew_Member(px_abi* pabi,const px_char payload[], PX_ABI_TYPE newtype, const px_void *buffer,px_int datasize)
{
	px_char abi_payload[512] = { 0 };
	px_int abi_count;
	px_int i;
	px_int new_alloc_size;
	px_int old_abi_size = PX_AbiGet_Size(pabi);
	px_int old_target_abi_data_offset=0;
	px_dword old_target_abi_data_size = 0;
	px_byte* abiptr;

	abi_count = PX_strsub(payload, '.');
	PX_strsubi(payload, abi_payload,sizeof(abi_payload), '.', abi_count - 1);
	new_alloc_size = sizeof(px_dword) + sizeof(PX_ABI_TYPE) + PX_strlen(abi_payload) + 1 + datasize;
	PX_strsubn(payload, abi_payload, sizeof(abi_payload), '.', abi_count - 1);
	if (abi_payload[0])
	{
		old_target_abi_data_offset = PX_AbiGet_PayloadDataOffsetWithType(pabi, PX_ABI_TYPE_ABI, &old_target_abi_data_size, abi_payload);
		if (old_target_abi_data_offset == -1)
		{
			return PX_FALSE;
		}
	}
	else
	{
		old_target_abi_data_offset = 0;
		old_target_abi_data_size = PX_AbiGet_Size(pabi);
	}
	
	//resize abi
	if (pabi->dynamic.mp)
	{
		if (!PX_MemoryResize(&pabi->dynamic, pabi->dynamic.usedsize + new_alloc_size))
		{
			return PX_FALSE;
		}
	}
	else
	{
		if (pabi->static_used_size + new_alloc_size > pabi->static_size)
		{
			return PX_FALSE;
		}
		pabi->static_used_size += new_alloc_size;
	}

	//resize abi tree
	
	for (i = 0; i < abi_count-1; i++)
	{
		px_byte* ptr;
		px_dword idatasize;
		if (!PX_strsubn( payload, abi_payload, sizeof(abi_payload), '.', i+1))
		{
			return PX_FALSE;
		}
		ptr = PX_AbiGet_PayloadPointerWithType(pabi, PX_ABI_TYPE_ABI, &idatasize, abi_payload);
		if (!ptr)
		{
			return PX_FALSE;
		}
		PX_AbiPointer_SetAbiSize(ptr, PX_AbiPointer_GetAbiSize(ptr) + new_alloc_size);
	}

	

	//rebuild memory
	abiptr = PX_AbiGet_Pointer(pabi);
	PX_memcpy(\
		abiptr + old_target_abi_data_offset + old_target_abi_data_size+ new_alloc_size,\
		abiptr + old_target_abi_data_offset + old_target_abi_data_size,\
		old_abi_size - old_target_abi_data_offset - old_target_abi_data_size\
	);

	//rebuild abi member
	PX_strsubi(payload, abi_payload, sizeof(abi_payload), '.', abi_count - 1);

	PX_memcpy(abiptr + old_target_abi_data_offset + old_target_abi_data_size, &new_alloc_size, sizeof(px_dword));
	PX_memcpy(abiptr + old_target_abi_data_offset + old_target_abi_data_size + sizeof(px_dword), &newtype, sizeof(PX_ABI_TYPE));
	PX_memcpy(abiptr + old_target_abi_data_offset + old_target_abi_data_size + sizeof(px_dword) + sizeof(PX_ABI_TYPE), abi_payload, PX_strlen(abi_payload) + 1);
	if (buffer)
	{
		PX_memcpy(abiptr + old_target_abi_data_offset + old_target_abi_data_size + sizeof(px_dword) + sizeof(PX_ABI_TYPE) + PX_strlen(abi_payload) + 1, buffer, datasize);
	}
	else
	{
		PX_memset(abiptr + old_target_abi_data_offset + old_target_abi_data_size + sizeof(px_dword) + sizeof(PX_ABI_TYPE) + PX_strlen(abi_payload) + 1, 0, datasize);
	}
	return PX_TRUE;
}

static px_bool PX_AbiResize_Member(px_abi* pabi, const px_char payload[], px_int datasize)
{
	px_char abi_payload[512] = { 0 };
	px_int namelen;
	px_int abi_count;
	px_int i;
	PX_ABI_TYPE type;
	px_int   inc_data_size;
	px_int old_abi_size = PX_AbiGet_Size(pabi);
	px_dword old_target_abi_offset, old_target_data_offset, old_target_data_size;
	px_byte* abiptr;
	abi_count = PX_strsub(payload, '.');
	PX_strsubi(payload, abi_payload, sizeof(abi_payload), '.', abi_count - 1);
	namelen = PX_strlen(abi_payload) + 1;
	old_target_abi_offset = PX_AbiGet_PayloadOffset(pabi, &type, &old_target_data_size, payload);
	old_target_data_offset = PX_AbiGet_PayloadDataOffset(pabi, &type, &old_target_data_size, payload);

	if (old_target_abi_offset == -1 || old_target_data_offset == -1)
	{
		return PX_FALSE;
	}
	inc_data_size = datasize - old_target_data_size;
	//resize abi size member
	for (i = abi_count; i > 0; i--)
	{
		px_byte* ptr;
		if (!PX_strsubn(payload, abi_payload, sizeof(abi_payload), '.', i))
		{
			return PX_FALSE;
		}
		ptr = PX_AbiGet_PayloadPointer(pabi, 0, 0, abi_payload);
		if (!ptr)
		{
			return PX_FALSE;
		}
		PX_AbiPointer_SetAbiSize(ptr, PX_AbiPointer_GetAbiSize(ptr) + inc_data_size);
	}

	//resize abi tree
	if (pabi->dynamic.mp)
	{
		if (!PX_MemoryResize(&pabi->dynamic, pabi->dynamic.usedsize + inc_data_size))
		{
			PX_ASSERTX( "AbiResize_Member failed, memory resize failed,crash");
			PX_memset(pabi->dynamic.buffer, 0, pabi->dynamic.usedsize);
			return PX_FALSE;
		}
	}
	else
	{
		if (pabi->static_used_size + inc_data_size > pabi->static_size)
		{
			return PX_FALSE;
		}
		pabi->static_used_size += inc_data_size;
	}

	//rebuild memory
	abiptr = PX_AbiGet_Pointer(pabi);

	//move old abi data
	PX_memcpy(\
		abiptr + old_target_data_offset + datasize, \
		abiptr + old_target_data_offset + old_target_data_size, \
		old_abi_size - old_target_data_offset - old_target_data_size\
	);

	return PX_TRUE;
}

static px_bool PX_AbiReset_Member(px_abi* pabi, const px_char payload[], PX_ABI_TYPE newtype, const  px_void* buffer, px_int datasize)
{
	PX_ABI_TYPE type;
	px_int old_abi_size = PX_AbiGet_Size(pabi);
	px_dword old_target_abi_offset,old_target_data_offset, old_target_data_size;
	px_byte* abiptr;

	PX_ASSERTIFX(datasize<0, "AbiReset_Member failed, datasize < 0");

	old_target_abi_offset = PX_AbiGet_PayloadOffset(pabi, &type, &old_target_data_size, payload);
	old_target_data_offset = PX_AbiGet_PayloadDataOffset(pabi, &type, &old_target_data_size, payload);
	
	if(PX_AbiResize_Member(pabi, payload, datasize)==PX_FALSE)
		return PX_FALSE;

	abiptr = PX_AbiGet_Pointer(pabi);
	//rebuild abi type
	PX_memcpy(abiptr + old_target_abi_offset + sizeof(px_dword), &newtype, sizeof(PX_ABI_TYPE));

	//rebuild abi data
	if(datasize)
		PX_memcpy(abiptr + old_target_data_offset, buffer, datasize);
	return PX_TRUE;
}

px_bool PX_AbiDelete(px_abi* pabi, const px_char payload[])
{
	px_char abi_payload[512] = { 0 };
	px_int abi_count;
	px_int i;
	px_dword old_target_abi_size;
	px_int old_abi_size = PX_AbiGet_Size(pabi);
	px_int old_target_abi_offset;
	px_byte* abiptr;
	abiptr = PX_AbiGet_Pointer(pabi);
	old_target_abi_offset = PX_AbiGet_PayloadOffset(pabi, 0, 0, payload);
	if (old_target_abi_offset==-1)
	{
		return PX_FALSE;
	}

	abi_count = PX_strsub(payload, '.');
	old_target_abi_size = PX_AbiPointer_GetAbiSize(abiptr + old_target_abi_offset);
	//resize abi size member

	for (i = abi_count - 1; i >=0 ; i--)
	{
		px_byte* ptr;
		PX_ABI_TYPE type;
		px_dword datasize;
		if (!PX_strsubn(payload, abi_payload, sizeof(abi_payload), '.', i + 1))
		{
			return PX_FALSE;
		}
		ptr = PX_AbiGet_PayloadPointer(pabi, &type, &datasize, abi_payload);
		if (!ptr)
		{
			return PX_FALSE;
		}
		PX_AbiPointer_SetAbiSize(ptr, PX_AbiPointer_GetAbiSize(ptr) - old_target_abi_size);
	}

	//resize abi tree
	if (pabi->dynamic.mp)
	{
		pabi->dynamic.usedsize -= old_target_abi_size;
	}
	else
	{
		pabi->static_used_size -= old_target_abi_size;
	}

	//rebuild memory
	
	PX_memcpy(abiptr + old_target_abi_offset, abiptr + old_target_abi_offset + old_target_abi_size, old_abi_size - old_target_abi_offset - old_target_abi_size);
	PX_memset(abiptr + old_abi_size - old_target_abi_size, 0, old_target_abi_size);
	return PX_TRUE;
}


px_bool PX_AbiSet(px_abi* pabi, PX_ABI_TYPE type, const px_char payload[], const  px_void* buffer, px_dword buffersize)
{
	px_dword datasize;
	switch (type)
	{
	case PX_ABI_TYPE_INT:
	{
		px_int* pint = (px_int *)PX_AbiGet_PayloadDataPointerWithType(pabi, PX_ABI_TYPE_INT, &datasize, payload);
		if (pint)
		{
			*pint = *(px_int*)buffer;
			return PX_TRUE;
		}
	}
	break;
	case PX_ABI_TYPE_DWORD:
	{
		px_dword* pdword = (px_dword*)PX_AbiGet_PayloadDataPointerWithType(pabi, PX_ABI_TYPE_DWORD, &datasize, payload);
		if (pdword)
		{
			*pdword = *(px_dword*)buffer;
			return PX_TRUE;
		}
	}
	break;
	case PX_ABI_TYPE_WORD:
	{
		px_word* pword = (px_word*)PX_AbiGet_PayloadDataPointerWithType(pabi, PX_ABI_TYPE_WORD, &datasize, payload);
		if (pword)
		{
			*pword = *(px_word*)buffer;
			return PX_TRUE;
		}
	}
	break;
	case PX_ABI_TYPE_BYTE:
	{
		px_byte* pbyte = (px_byte*)PX_AbiGet_PayloadDataPointerWithType(pabi, PX_ABI_TYPE_BYTE, &datasize, payload);
		if (pbyte)
		{
			*pbyte = *(px_byte*)buffer;
			return PX_TRUE;
		}
	}
	break;
	case PX_ABI_TYPE_PTR:
	{
		px_void** pptr = (px_void**)PX_AbiGet_PayloadDataPointerWithType(pabi, PX_ABI_TYPE_PTR, &datasize, payload);
		if (pptr)
		{
			*pptr = *(px_void**)buffer;
			return PX_TRUE;
		}
	}
	break;
	case PX_ABI_TYPE_FLOAT:
	{
		px_float* pfloat = (px_float*)PX_AbiGet_PayloadDataPointerWithType(pabi, PX_ABI_TYPE_FLOAT, &datasize, payload);
		if (pfloat)
		{
			*pfloat = *(px_float*)buffer;
			return PX_TRUE;
		}
	}
	break;
	case PX_ABI_TYPE_DOUBLE:
	{
		px_double* pdouble = (px_double*)PX_AbiGet_PayloadDataPointerWithType(pabi, PX_ABI_TYPE_DOUBLE, &datasize, payload);
		if (pdouble)
		{
			*pdouble = *(px_double*)buffer;
			return PX_TRUE;
		}
	}
	break;
	case PX_ABI_TYPE_POINT:
	{
		px_point* ppoint = (px_point*)PX_AbiGet_PayloadDataPointerWithType(pabi, PX_ABI_TYPE_POINT, &datasize, payload);
		if (ppoint )
		{
			*ppoint = *(px_point*)buffer;
			return PX_TRUE;
		}
	}
	break;
	case PX_ABI_TYPE_COLOR:
	{
		px_color* pcolor = (px_color*)PX_AbiGet_PayloadDataPointerWithType(pabi, PX_ABI_TYPE_COLOR, &datasize, payload);
		if (pcolor)
		{
			*pcolor = *(px_color*)buffer;
			return PX_TRUE;
		}
	}
	break;
	case PX_ABI_TYPE_BOOL:
	{
		px_bool* pbool = (px_bool*)PX_AbiGet_PayloadDataPointerWithType(pabi, PX_ABI_TYPE_BOOL, &datasize, payload);
		if (pbool)
		{
			*pbool = *(px_bool*)buffer;
			return PX_TRUE;
		}
	}
	break;
	case PX_ABI_TYPE_STRING:
	{
		px_void* pdata = PX_AbiGet_PayloadDataPointerWithType(pabi, PX_ABI_TYPE_STRING, &datasize, payload);
		if (pdata)
		{
			if (buffersize == datasize)
			{
				PX_memcpy(pdata, buffer, buffersize);
				return PX_TRUE;
			}
			else
			{
				//reset string
				if (!PX_AbiReset_Member(pabi, payload, PX_ABI_TYPE_STRING, buffer, buffersize))
				{
					return PX_FALSE;
				}
			}
		}
	}
	break;
	case PX_ABI_TYPE_ABI:
	{
		px_void* pdata = PX_AbiGet_PayloadDataPointerWithType(pabi, PX_ABI_TYPE_ABI, &datasize, payload);
		if (pdata)
		{
			if (buffersize == datasize)
			{
				PX_memcpy(pdata, buffer, buffersize);
				return PX_TRUE;
			}
			else
			{
				//reset abi
				if (!PX_AbiReset_Member(pabi, payload, PX_ABI_TYPE_ABI, buffer, buffersize))
				{
					return PX_FALSE;
				}
			}
		}
	}
	break;
	case PX_ABI_TYPE_DATA:
	{
		px_void* pdata = PX_AbiGet_PayloadDataPointerWithType(pabi, PX_ABI_TYPE_DATA, &datasize, payload);
		if (pdata)
		{
			if (buffersize == datasize)
			{
				if(buffer)
					PX_memcpy(pdata, buffer, buffersize);
				else
					PX_memset(pdata,0,buffersize);

				return PX_TRUE;
			}
			else
			{
				//reset data
				if (!PX_AbiReset_Member(pabi, payload, PX_ABI_TYPE_DATA, buffer, buffersize))
				{
					return PX_FALSE;
				}
			}
		}
	}
	break;
	default:
		return PX_FALSE;
	}
	do
	{
		px_char abi_payload[512];
		px_int  abi_count;
		px_int  i;
		abi_count = PX_strsub(payload, '.');
		for (i = 0; i < abi_count; i++)
		{
			if (PX_strsubn(payload, abi_payload, sizeof(abi_payload), '.', i + 1))
			{
				if (i != abi_count - 1)
				{
					if (!PX_AbiExist_Type(pabi, abi_payload, PX_ABI_TYPE_ABI))
					{
						if (!PX_AbiNew_Member(pabi, abi_payload, PX_ABI_TYPE_ABI, 0, 0))
						{
							return PX_FALSE;
						}
					}
				}
				else
				{
					if (!PX_AbiExist_Type(pabi, abi_payload, type))
					{
						if (!PX_AbiNew_Member(pabi, abi_payload, type, buffer, buffersize))
						{
							return PX_FALSE;
						}
					}
				}
			}
			else
			{
				return PX_FALSE;
			}
		}
	} while (0);
	return PX_TRUE;
}


px_bool PX_AbiSet_int(px_abi* pabi, const px_char payload[], px_int _int)
{
	return PX_AbiSet(pabi, PX_ABI_TYPE_INT, payload, &_int, sizeof(_int));
}
px_bool PX_AbiSet_dword(px_abi* pabi, const px_char payload[], px_dword _dword)
{
	return PX_AbiSet(pabi, PX_ABI_TYPE_DWORD, payload, &_dword, sizeof(_dword));
}
px_bool PX_AbiSet_word(px_abi* pabi, const px_char payload[], px_word _word)
{
	return PX_AbiSet(pabi, PX_ABI_TYPE_WORD, payload, &_word, sizeof(_word));
}
px_bool PX_AbiSet_byte(px_abi* pabi, const px_char payload[], px_byte _byte)
{
	return PX_AbiSet(pabi, PX_ABI_TYPE_BYTE, payload, &_byte, sizeof(_byte));
}
px_bool PX_AbiSet_ptr(px_abi* pabi, const px_char payload[], px_void* ptr)
{
	return PX_AbiSet(pabi, PX_ABI_TYPE_PTR, payload, &ptr, sizeof(ptr));
}
px_bool PX_AbiSet_float(px_abi* pabi, const px_char payload[], px_float _float)
{
	return PX_AbiSet(pabi, PX_ABI_TYPE_FLOAT, payload, &_float, sizeof(_float));
}
px_bool PX_AbiSet_double(px_abi* pabi, const px_char payload[], px_double _double)
{
	return PX_AbiSet(pabi, PX_ABI_TYPE_DOUBLE, payload, &_double, sizeof(_double));
}
px_bool PX_AbiSet_string(px_abi* pabi, const px_char payload[], const px_char _string[])
{
	return PX_AbiSet(pabi, PX_ABI_TYPE_STRING, payload, (px_void*)_string, PX_strlen(_string) + 1);
}
px_bool PX_AbiAppend_string(px_abi* pabi, const px_char payload[], const px_char _string[])
{
	px_char* pstring = (px_char*)PX_AbiGet_string(pabi, payload);
	if (!pstring)
	{
		return PX_AbiSet_string(pabi, payload, _string);
	}
	else
	{
		if(!PX_AbiResize_Member(pabi, payload, PX_strlen(pstring) + PX_strlen(_string) + 1))
			return PX_FALSE;
		pstring = (px_char*)PX_AbiGet_string(pabi, payload);
		PX_strcat(pstring, _string);
		return PX_TRUE;
	}

}
px_bool PX_AbiInsert_string(px_abi* pabi, const px_char payload[],px_int insert_pos, const px_char _string[])
{
	px_char* pstring = (px_char*)PX_AbiGet_string(pabi, payload);
	if (!pstring)
	{
		return PX_AbiSet_string(pabi, payload, _string);
	}
	else
	{
		px_int old_len = PX_strlen(pstring);
		px_int ins_len = PX_strlen(_string);
		px_int new_len = old_len + ins_len;
		if (insert_pos < 0) insert_pos = 0;
		if (insert_pos > old_len) insert_pos = old_len;
		if (!PX_AbiResize_Member(pabi, payload, new_len + 1))
			return PX_FALSE;
		pstring = (px_char*)PX_AbiGet_string(pabi, payload);
		/* shift tail right to make room */
		PX_memcpy(pstring + insert_pos + ins_len, pstring + insert_pos, old_len - insert_pos + 1);
		/* copy new string into gap */
		PX_memcpy(pstring + insert_pos, _string, ins_len);
		return PX_TRUE;
	}
}
px_bool PX_AbiSet_point(px_abi* pabi, const px_char payload[], px_point point)
{
	return PX_AbiSet(pabi, PX_ABI_TYPE_POINT, payload, &point, sizeof(point));
}
px_bool PX_AbiSet_color(px_abi* pabi, const px_char payload[], px_color color)
{
	return PX_AbiSet(pabi, PX_ABI_TYPE_COLOR, payload, &color, sizeof(color));
}
px_bool PX_AbiSet_bool(px_abi* pabi, const px_char payload[], px_bool _bool)
{
	return PX_AbiSet(pabi, PX_ABI_TYPE_BOOL, payload, &_bool, sizeof(_bool));
}
px_bool PX_AbiSet_data(px_abi* pabi, const px_char payload[],const px_void* data, px_int size)
{
	return PX_AbiSet(pabi, PX_ABI_TYPE_DATA, payload, data, size);
}
px_bool PX_AbiAppend_data(px_abi* pabi, const px_char payload[], const px_void* data, px_int size)
{
	px_dword oldsize,newsize;
	px_byte* pdata = PX_AbiGet_data(pabi, payload, &oldsize);
	if (!pdata)
	{
		return PX_AbiSet_data(pabi, payload, data, size);
	}
	else
	{
		if (!PX_AbiResize_Member(pabi, payload, oldsize + size))
			return PX_FALSE;
		pdata = PX_AbiGet_data(pabi, payload, &newsize);
		if(data)
			PX_memcpy(pdata + oldsize, data, size);
		return PX_TRUE;
	}
}
px_bool PX_AbiSet_Abi(px_abi* pabi, const px_char payload[], px_abi* pAbi)
{
	if(pAbi)
		return PX_AbiSet(pabi, PX_ABI_TYPE_ABI, payload, PX_AbiGet_Pointer(pAbi), PX_AbiGet_Size(pAbi));
	else
		return PX_AbiSet(pabi, PX_ABI_TYPE_ABI, payload, PX_NULL, 0);//empty abi
}

px_byte* PX_AbiGetDataPtr(px_abi* pabi, const px_char name[])
{
	px_int readoffset = 0;
	px_byte* pbuffer,*pstart;
	pbuffer=PX_AbiGet_Pointer(pabi);
	pstart =pbuffer;
	while (PX_TRUE)
	{
		px_dword size;
		const px_char* pname;
		px_byte* databuffer;
		PX_memcpy(&size, pbuffer, sizeof(size));
		pbuffer += sizeof(px_dword);
		pbuffer += sizeof(PX_ABI_TYPE);
		pname = (px_char*)(pbuffer);
		databuffer = pbuffer + PX_strlen(pname) + 1;
		pbuffer = databuffer + size - PX_strlen(pname) - 1 - sizeof(px_dword) - sizeof(PX_ABI_TYPE);

		if (pbuffer - pstart >= PX_AbiGet_Size(pabi))
		{
			break;
		}

		if (PX_strequ(pname, name))
		{
			return databuffer;
		}
		
	}
	return PX_NULL;
}


px_bool PX_Abi2Json(px_abi* pabi, px_string* pjson)
{
	px_int offset = 0;
	px_byte* pbuffer = PX_AbiGet_Pointer(pabi);
	px_int size = PX_AbiGet_Size(pabi);
	PX_StringCat(pjson, "{\n");

	while (PX_TRUE)
	{
		px_dword datasize;
		px_char* pname;
		px_byte* pdata;
		PX_ABI_TYPE type;
		PX_memcpy(&datasize, pbuffer + offset, sizeof(datasize));
		PX_memcpy(&type, pbuffer + offset+ sizeof(datasize), sizeof(type));
		pname = (px_char*)(pbuffer + offset + sizeof(datasize) + sizeof(type));
		pdata = pbuffer + offset+ PX_strlen(pname) + sizeof(datasize) + sizeof(type) + 1;
		offset += datasize;
		switch (type)
		{
		case PX_ABI_TYPE_INT:
		{
			px_int _int;
			PX_memcpy(&_int, pdata, sizeof(_int));
			if (!PX_StringCatFormat2(pjson, "\"%1\":\n{\n\"type\":\"int\",\n\"value\":%2\n}", PX_STRINGFORMAT_STRING(pname), PX_STRINGFORMAT_INT(_int)))
				return PX_FALSE;
		}
		break;
		case PX_ABI_TYPE_DWORD:
		{
			px_dword _dword;
			PX_memcpy(&_dword, pdata, sizeof(_dword));
			if (!PX_StringCatFormat2(pjson, "\"%1\":\n{\n\"type\":\"dword\",\n\"value\":%2\n}", PX_STRINGFORMAT_STRING(pname), PX_STRINGFORMAT_INT(_dword)))
				return PX_FALSE;

		}
		break;
		case PX_ABI_TYPE_WORD:
		{
			px_word _word;
			PX_memcpy(&_word, pdata, sizeof(_word));
			if (!PX_StringCatFormat2(pjson, "\"%1\":\n{\n\"type\":\"word\",\n\"value\":%2\n}", PX_STRINGFORMAT_STRING(pname), PX_STRINGFORMAT_INT(_word)))
				return PX_FALSE;

		}
		break;
		case PX_ABI_TYPE_BYTE:
		{
			px_byte _byte;
			PX_memcpy(&_byte, pdata, sizeof(_byte));
			if (!PX_StringCatFormat2(pjson, "\"%1\":\n{\n\"type\":\"byte\",\n\"value\":%2\n}", PX_STRINGFORMAT_STRING(pname), PX_STRINGFORMAT_INT(_byte)))
				return PX_FALSE;
		}
		break;
		case PX_ABI_TYPE_PTR:
		{
			px_void* ptr;
			PX_memcpy(&ptr, pdata, sizeof(ptr));
			if (!PX_StringCatFormat2(pjson, "\"%1\":\n{\n\"type\":\"ptr\",\n\"value\":%2\n}", PX_STRINGFORMAT_STRING(pname), PX_STRINGFORMAT_INT(datasize - PX_strlen(pname) - 1)))
				return PX_FALSE;
		}
		break;
		case PX_ABI_TYPE_FLOAT:
		{
			px_float _float;
			PX_memcpy(&_float, pdata, sizeof(_float));
			if (!PX_StringCatFormat2(pjson, "\"%1\":\n{\n\"type\":\"float\",\n\"value\":%2\n}", PX_STRINGFORMAT_STRING(pname), PX_STRINGFORMAT_FLOAT(_float)))
				return PX_FALSE;
		}
		break;
		case PX_ABI_TYPE_DOUBLE:
		{
			px_double _double;
			PX_memcpy(&_double, pdata, sizeof(_double));
			if (!PX_StringCatFormat2(pjson, "\"%1\":\n{\n\"type\":\"double\",\n\"value\":%2\n}", PX_STRINGFORMAT_STRING(pname), PX_STRINGFORMAT_FLOAT((px_float)_double)))
				return PX_FALSE;
		}
		break;
		case PX_ABI_TYPE_STRING:
		{
			px_char* _string;
			_string = (px_char*)pdata;
			if (!PX_StringCatFormat2(pjson, "\"%1\":\n{\n\"type\":\"string\",\n\"value\":\"%2\"\n}", PX_STRINGFORMAT_STRING(pname), PX_STRINGFORMAT_STRING(_string)))
				return PX_FALSE;
		}
		break;
		case PX_ABI_TYPE_POINT:
		{
			px_point point;
			PX_memcpy(&point, pdata, sizeof(point));
			if (!PX_StringCatFormat4(pjson, "\"%1\":\n{\n\"type\":\"point\",\n\"value\":{\"x\":%2,\"y\":%3,\"z\":%4}\n}",\
				PX_STRINGFORMAT_STRING(pname),\
				PX_STRINGFORMAT_FLOAT(point.x),\
				PX_STRINGFORMAT_FLOAT(point.y),\
				PX_STRINGFORMAT_FLOAT(point.z)\
			))
				return PX_FALSE;
		}
		break;
		case PX_ABI_TYPE_COLOR:
		{
			px_color color;
			PX_memcpy(&color, pdata, sizeof(color));
			if (!PX_StringCatFormat5(pjson, "\"%1\":\n{\n\"type\":\"color\",\n\"value\":{\"a\":%2,\"r\":%3,\"g\":%4,\"b\":%5}\n}", \
				PX_STRINGFORMAT_STRING(pname), \
				PX_STRINGFORMAT_INT(color._argb.a), \
				PX_STRINGFORMAT_INT(color._argb.r), \
				PX_STRINGFORMAT_INT(color._argb.g), \
				PX_STRINGFORMAT_INT(color._argb.b)))
				return PX_FALSE;
		}
		break;
		case PX_ABI_TYPE_BOOL:
		{
			px_bool _bool;
			PX_memcpy(&_bool, pdata, sizeof(_bool));
			if (!PX_StringCatFormat2(pjson, "\"%1\":\n{\n\"type\":\"bool\",\n\"value\":%2\n}", PX_STRINGFORMAT_STRING(pname), PX_STRINGFORMAT_INT(_bool)))
			{
				return PX_FALSE;
			}
		}
		break;
		case PX_ABI_TYPE_DATA:
		{
			if(!PX_StringCatFormat2(pjson, "\"%1\":\n{\n\"type\":\"data\",\n\"value\":\"%2\"\n}", PX_STRINGFORMAT_STRING(pname), PX_STRINGFORMAT_INT(datasize - PX_strlen(pname) - 1)))
			{
				return PX_FALSE;
			}
		}
		break;
		case PX_ABI_TYPE_ABI:
		{
			px_abi abi;
			if (!PX_StringCatFormat1(pjson, "\"%1\":\n", PX_STRINGFORMAT_STRING(pname)))
			{
				return PX_FALSE;
			}
			PX_AbiCreate_StaticReader(&abi, pdata, datasize-sizeof(px_dword)-sizeof(PX_ABI_TYPE) - PX_strlen(pname) - 1);
			if (!PX_Abi2Json(&abi, pjson))
			{
				return PX_FALSE;
			}
		}
		break;
		default:
			break;

		}
		if (offset >= size)
		{
			break;
		}
		else
		{
			if(!PX_StringCat(pjson, ",\n"))
			{
				return PX_FALSE;
			}
		}
	}
	if(!PX_StringCat(pjson, "\n}"))
	{
		return PX_FALSE;
	}
	return PX_TRUE;
	
}


px_void PX_AbiDynamicFree(px_abi* pabi)
{
	if (pabi->dynamic.mp)
	{
		PX_MemoryFree(&pabi->dynamic);
	}
	PX_memset(pabi, 0, sizeof(px_abi));
}

px_void PX_AbiFree(px_abi* pabi)
{
	if (pabi->dynamic.mp)
	{
		PX_AbiDynamicFree(pabi);
	}
}

