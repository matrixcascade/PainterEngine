#ifndef __PX_ABI_H
#define __PX_ABI_H
#include "PX_Memory.h"
#include "PX_String.h"

typedef enum
{
	PX_ABI_TYPE_NONE,
	PX_ABI_TYPE_INT,
	PX_ABI_TYPE_DWORD,
	PX_ABI_TYPE_WORD,
	PX_ABI_TYPE_BYTE,
	PX_ABI_TYPE_PTR,
	PX_ABI_TYPE_STRING,
	PX_ABI_TYPE_FLOAT,
	PX_ABI_TYPE_DOUBLE,
	PX_ABI_TYPE_POINT,
	PX_ABI_TYPE_COLOR,
	PX_ABI_TYPE_BOOL,
	PX_ABI_TYPE_DATA,
	PX_ABI_TYPE_ABI,
}PX_ABI_TYPE;

typedef struct
{
	px_memory dynamic;
	px_byte* pstatic_buffer;
	px_int static_size;
	px_int static_used_size;
}px_abi;


px_void PX_AbiCreate_StaticWriter(px_abi *pabi,px_void* pbuffer,px_int size);
px_void PX_AbiCreate_StaticReader(px_abi* pabi,const px_void* pbuffer, px_int size);
px_void PX_AbiCreate_DynamicWriter(px_abi* pabi, px_memorypool* mp);

px_int  PX_AbiGet_Size(px_abi* pabi);
px_byte* PX_AbiGet_Pointer(px_abi* pabi);
px_int PX_AbiPointer_GetDataOffset(px_byte* pStartBuffer);
px_byte* PX_AbiPointer_GetDataPointer(px_byte *pStartBuffer);
px_dword PX_AbiPointer_GetDataSize(px_byte* pStartBuffer);
px_dword PX_AbiPointer_GetAbiSize(px_byte* pStartBuffer);

px_byte* PX_AbiGet(px_abi* pabi, PX_ABI_TYPE* ptype, px_dword* psize, const px_char _payload[]);
px_int *PX_AbiGet_int(px_abi* pabi, const px_char payload[]);
px_dword *PX_AbiGet_dword(px_abi* pabi, const px_char payload[]);
px_word* PX_AbiGet_word(px_abi* pabi, const px_char payload[]);
px_byte* PX_AbiGet_byte(px_abi* pabi, const px_char payload[]);
px_void** PX_AbiGet_ptr(px_abi* pabi, const px_char payload[]);
px_float* PX_AbiGet_float(px_abi* pabi, const px_char payload[]);
px_double* PX_AbiGet_double(px_abi* pabi, const px_char payload[]);
const px_char* PX_AbiGet_string(px_abi* pabi, const px_char payload[]);
px_point* PX_AbiGet_point(px_abi* pabi, const px_char payload[]);
px_color* PX_AbiGet_color(px_abi* pabi, const px_char payload[]);
px_bool* PX_AbiGet_bool(px_abi* pabi, const px_char payload[]);
px_void* PX_AbiGet_data(px_abi* pabi, const px_char payload[], px_dword* size);
px_bool PX_AbiGet_Abi(px_abi* pabi, px_abi* prabi, const px_char payload[]);
px_int PX_AbiGet_MemberCount(px_abi* pabi);
px_byte* PX_AbiGet_Member(px_abi* pabi, px_int index);
px_byte* PX_AbiGet_First(px_abi* pabi);
px_byte* PX_AbiGet_Next(px_abi* pabi, px_byte* pstart);
px_bool PX_AbiDelete(px_abi* pabi, const px_char payload[]);

px_bool PX_AbiCheck(px_abi* pabi);
px_bool PX_AbiExist(px_abi* pabi, const px_char payload[]);
px_bool PX_AbiExist_Type(px_abi* pabi, const px_char payload[], PX_ABI_TYPE type);
px_bool PX_AbiExist_string(px_abi* pabi, const px_char payload[], const px_char check[]);
px_bool PX_AbiExist_int(px_abi* pabi, const px_char payload[], px_int check);
px_bool PX_AbiExist_dword(px_abi* pabi, const px_char payload[], px_dword check);
px_bool PX_AbiExist_word(px_abi* pabi, const px_char payload[], px_word check);
px_bool PX_AbiExist_byte(px_abi* pabi, const px_char payload[], px_byte check);
px_bool PX_AbiExist_ptr(px_abi* pabi, const px_char payload[], px_void* check);
px_bool PX_AbiExist_float(px_abi* pabi, const px_char payload[], px_float check);
px_bool PX_AbiExist_double(px_abi* pabi, const px_char payload[], px_double check);
px_bool PX_AbiExist_point(px_abi* pabi, const px_char payload[], px_point check);
px_bool PX_AbiExist_color(px_abi* pabi, const px_char payload[], px_color check);
px_bool PX_AbiExist_bool(px_abi* pabi, const px_char payload[], px_bool check);


px_bool PX_AbiSet(px_abi* pabi, PX_ABI_TYPE type, const px_char payload[], px_void* buffer, px_dword buffersize);
px_bool PX_AbiSet_int(px_abi* pabi, const px_char payload[], px_int _int);
px_bool PX_AbiSet_dword(px_abi* pabi, const px_char payload[], px_dword _dword);
px_bool PX_AbiSet_word(px_abi* pabi, const px_char payload[], px_word _word);
px_bool PX_AbiSet_byte(px_abi* pabi, const px_char payload[], px_byte _byte);
px_bool PX_AbiSet_ptr(px_abi* pabi, const px_char payload[], px_void* ptr);
px_bool PX_AbiSet_float(px_abi* pabi, const px_char payload[], px_float _float);
px_bool PX_AbiSet_double(px_abi* pabi, const px_char payload[], px_double _double);
px_bool PX_AbiSet_string(px_abi* pabi, const px_char payload[], const px_char _string[]);
px_bool PX_AbiSet_point(px_abi* pabi, const px_char payload[], px_point point);
px_bool PX_AbiSet_color(px_abi* pabi, const px_char payload[], px_color color);
px_bool PX_AbiSet_bool(px_abi* pabi, const px_char payload[], px_bool _bool);
px_bool PX_AbiSet_data(px_abi* pabi, const px_char payload[], px_void* data, px_int size);
px_bool PX_AbiSet_Abi(px_abi* pabi, const px_char payload[], px_abi* pStoreAbi);

px_bool PX_Abi2Json(px_abi* pabi, px_string* pjson);
px_void PX_AbiDynamicFree(px_abi* pabi);
px_void PX_AbiFree(px_abi* pabi);
#endif