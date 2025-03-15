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
	px_int static_write_offset;
}px_abi;


px_void PX_AbiCreateStaticWriter(px_abi *pabi,px_void* pbuffer,px_int size);
px_void PX_AbiCreateStaticReader(px_abi* pabi, px_void* pbuffer, px_int size);
px_void PX_AbiCreateDynamicWriter(px_abi* pabi, px_memorypool* mp);

px_int  PX_AbiGetPtrSize(px_abi* pabi);
px_byte* PX_AbiGetPtr(px_abi* pabi);
px_byte* PX_AbiGetRawDataPtr(px_byte *pStartBuffer);
px_dword PX_AbiGetRawDataSize(px_byte* pStartBuffer);
px_dword PX_AbiGetRawBlockSize(px_byte* pStartBuffer);

px_bool PX_AbiWrite_int(px_abi* pabi,const px_char name[],px_int _int);
px_bool PX_AbiWrite_dword(px_abi* pabi,const px_char name[],px_dword _dword);
px_bool PX_AbiWrite_word(px_abi* pabi,const px_char name[],px_word _word);
px_bool PX_AbiWrite_byte(px_abi* pabi,const px_char name[],px_byte _byte);
px_bool PX_AbiWrite_ptr(px_abi* pabi, const px_char name[], px_void *ptr);
px_bool PX_AbiWrite_float(px_abi* pabi,const px_char name[],px_float _float);
px_bool PX_AbiWrite_double(px_abi* pabi,const px_char name[],px_double _double);
px_bool PX_AbiWrite_string(px_abi* pabi,const px_char name[],const px_char _string[]);
px_bool PX_AbiWrite_point(px_abi* pabi,const px_char name[],px_point point);
px_bool PX_AbiWrite_color(px_abi* pabi,const px_char name[],px_color color);
px_bool PX_AbiWrite_bool(px_abi* pabi,const px_char name[],px_bool _bool);
px_bool PX_AbiWrite_data(px_abi* pabi,const px_char name[],px_void *data,px_int size);
px_bool PX_AbiWrite_Abi(px_abi* pabi, const px_char name[], px_abi* pStoreAbi);

px_bool PX_AbiSet_int(px_abi* pabi, const px_char name[], px_int _int);
px_bool PX_AbiSet_dword(px_abi* pabi, const px_char name[], px_dword _dword);
px_bool PX_AbiSet_word(px_abi* pabi, const px_char name[], px_word _word);
px_bool PX_AbiSet_byte(px_abi* pabi, const px_char name[], px_byte _byte);
px_bool PX_AbiSet_ptr(px_abi* pabi, const px_char name[], px_void* ptr);
px_bool PX_AbiSet_float(px_abi* pabi, const px_char name[], px_float _float);
px_bool PX_AbiSet_double(px_abi* pabi, const px_char name[], px_double _double);
px_bool PX_AbiSet_string(px_abi* pabi, const px_char name[], const px_char _string[]);
px_bool PX_AbiSet_point(px_abi* pabi, const px_char name[], px_point point);
px_bool PX_AbiSet_color(px_abi* pabi, const px_char name[], px_color color);
px_bool PX_AbiSet_bool(px_abi* pabi, const px_char name[], px_bool _bool);
px_bool PX_AbiSet_data(px_abi* pabi, const px_char name[], px_void* data, px_int size);
px_bool PX_AbiSet_Abi(px_abi* pabi, const px_char name[], px_abi* pStoreAbi);

px_bool PX_AbiMemoryWrite_int(px_memory* pmem, const px_char name[], px_int _int);
px_bool PX_AbiMemoryWrite_dword(px_memory* pmem, const px_char name[], px_dword _dword);
px_bool PX_AbiMemoryWrite_word(px_memory* pmem, const px_char name[], px_word _word);
px_bool PX_AbiMemoryWrite_byte(px_memory* pmem, const px_char name[], px_byte _byte);
px_bool PX_AbiMemoryWrite_ptr(px_memory* pmem, const px_char name[], px_void* ptr);
px_bool PX_AbiMemoryWrite_float(px_memory* pmem, const px_char name[], px_float _float);
px_bool PX_AbiMemoryWrite_double(px_memory* pmem, const px_char name[], px_double _double);
px_bool PX_AbiMemoryWrite_string(px_memory* pmem, const px_char name[], const px_char _string[]);
px_bool PX_AbiMemoryWrite_point(px_memory* pmem, const px_char name[], px_point point);
px_bool PX_AbiMemoryWrite_color(px_memory* pmem, const px_char name[], px_color color);
px_bool PX_AbiMemoryWrite_bool(px_memory* pmem, const px_char name[], px_bool _bool);
px_bool PX_AbiMemoryWrite_data(px_memory* pmem, const px_char name[], px_void* data, px_int size);
px_bool PX_AbiMemoryWrite_Abi(px_memory* pmem, const px_char name[], px_abi* pAbi);



px_int PX_AbiRead_int(px_abi* pabi,const px_char name[],px_int *_int);
px_int PX_AbiRead_dword(px_abi* pabi, const px_char name[], px_dword* _dword);
px_int PX_AbiRead_word(px_abi* pabi, const px_char name[], px_word* _word);
px_int PX_AbiRead_byte(px_abi* pabi, const px_char name[], px_byte* _byte);
px_int PX_AbiRead_ptr(px_abi* pabi, const px_char name[], px_void** ptr);
px_int PX_AbiRead_float(px_abi* pabi,const px_char name[],px_float *_float);
px_int PX_AbiRead_double(px_abi* pabi,const px_char name[],px_double *_double);
px_int PX_AbiRead_string(px_abi* pabi, const px_char name[], px_char _string[], px_int size);
px_int PX_AbiRead_string2(px_abi* pabi,const px_char name[],px_char *_string[]);
px_int PX_AbiRead_point(px_abi* pabi,const px_char name[],px_point *point);
px_int PX_AbiRead_color(px_abi* pabi,const px_char name[],px_color *color);
px_int PX_AbiRead_bool(px_abi* pabi,const px_char name[],px_bool *_bool);
px_int PX_AbiRead_data(px_abi* pabi,const px_char name[],px_void *data,px_int size);
px_int PX_AbiRead_Abi(px_abi* pabi, const px_char name[], px_abi* pAbi);

px_int *PX_AbiGet_int(px_abi* pabi, const px_char name[]);
px_dword *PX_AbiGet_dword(px_abi* pabi, const px_char name[]);
px_word* PX_AbiGet_word(px_abi* pabi, const px_char name[]);
px_byte* PX_AbiGet_byte(px_abi* pabi, const px_char name[]);
px_void** PX_AbiGet_ptr(px_abi* pabi, const px_char name[]);
px_float* PX_AbiGet_float(px_abi* pabi, const px_char name[]);
px_double* PX_AbiGet_double(px_abi* pabi, const px_char name[]);
const px_char* PX_AbiGet_string(px_abi* pabi, const px_char name[]);
px_point* PX_AbiGet_point(px_abi* pabi, const px_char name[]);
px_color* PX_AbiGet_color(px_abi* pabi, const px_char name[]);
px_bool* PX_AbiGet_bool(px_abi* pabi, const px_char name[]);
px_void* PX_AbiGet_data(px_abi* pabi, const px_char name[], px_int *size);
px_bool PX_AbiGet_Abi(px_abi* pabi, px_abi* pgetabi, const px_char _payload[]);
px_bool PX_Abi2Json(px_abi* pabi, px_string* pjson);

px_void PX_AbiDynamicFree(px_abi* pabi);
px_void PX_AbiFree(px_abi* pabi);
#endif