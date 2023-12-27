#ifndef __PX_ABI_H
#define __PX_ABI_H
#include "PX_Memory.h"

typedef enum
{
	PX_ABI_TYPE_NONE,
	PX_ABI_TYPE_INT,
	PX_ABI_TYPE_PTR,
	PX_ABI_TYPE_STRING,
	PX_ABI_TYPE_FLOAT,
	PX_ABI_TYPE_DOUBLE,
	PX_ABI_TYPE_POINT,
	PX_ABI_TYPE_COLOR,
	PX_ABI_TYPE_BOOL,
	PX_ABI_TYPE_DATA,
}PX_ABI_TYPE;


typedef struct
{
	px_memory dynamic;
	px_byte* pstatic_buffer;
	px_int static_size;
	px_int static_write_offset;
}px_abi;

typedef struct
{
	PX_ABI_TYPE type;
	px_int size;
	px_char *name;
	px_byte *buffer;
}px_abi_desc;

#define px_abi_type_int_size(name) (sizeof(PX_ABI_TYPE)+sizeof(px_int)+sizeof(name)+sizeof(px_int))
#define px_abi_type_ptr_size(name) (sizeof(PX_ABI_TYPE)+sizeof(px_int)+sizeof(name)+sizeof(px_void*))
#define px_abi_type_float_size(name) (sizeof(PX_ABI_TYPE)+sizeof(px_int)+sizeof(name)+sizeof(px_float))
#define px_abi_type_double_size(name) (sizeof(PX_ABI_TYPE)+sizeof(px_int)+sizeof(name)+sizeof(px_double))
#define px_abi_type_string_size(name,string) (sizeof(PX_ABI_TYPE)+sizeof(px_int)+sizeof(name)+sizeof(string))
#define px_abi_type_point_size(name) (sizeof(PX_ABI_TYPE)+sizeof(px_int)+sizeof(name)+sizeof(px_point))
#define px_abi_type_color_size(name) (sizeof(PX_ABI_TYPE)+sizeof(px_int)+sizeof(name)+sizeof(px_color))
#define px_abi_type_bool_size(name) (sizeof(PX_ABI_TYPE)+sizeof(px_int)+sizeof(name)+sizeof(px_bool))
#define px_abi_type_data_size(name,size) (sizeof(PX_ABI_TYPE)+sizeof(px_int)+sizeof(name)+sizeof(px_int)+size)

px_void PX_AbiCreateStatic(px_abi *pabi,px_void* pbuffer,px_int size);
px_void PX_AbiCreateDynamic(px_abi* pabi, px_memorypool* mp);

px_bool PX_AbiWrite_int(px_abi* pabi,const px_char name[],px_int _int);
px_bool PX_AbiWrite_ptr(px_abi* pabi, const px_char name[], px_void *ptr);
px_bool PX_AbiWrite_float(px_abi* pabi,const px_char name[],px_float _float);
px_bool PX_AbiWrite_double(px_abi* pabi,const px_char name[],px_double _double);
px_bool PX_AbiWrite_string(px_abi* pabi,const px_char name[],const px_char _string[]);
px_bool PX_AbiWrite_point(px_abi* pabi,const px_char name[],px_point point);
px_bool PX_AbiWrite_color(px_abi* pabi,const px_char name[],px_color color);
px_bool PX_AbiWrite_bool(px_abi* pabi,const px_char name[],px_bool _bool);
px_bool PX_AbiWrite_data(px_abi* pabi,const px_char name[],px_void *data,px_int size);

px_bool PX_AbiMemoryWrite_int(px_memory* pmem, const px_char name[], px_int _int);
px_bool PX_AbiMemoryWrite_ptr(px_memory* pmem, const px_char name[], px_void* ptr);
px_bool PX_AbiMemoryWrite_float(px_memory* pmem, const px_char name[], px_float _float);
px_bool PX_AbiMemoryWrite_double(px_memory* pmem, const px_char name[], px_double _double);
px_bool PX_AbiMemoryWrite_string(px_memory* pmem, const px_char name[], const px_char _string[]);
px_bool PX_AbiMemoryWrite_point(px_memory* pmem, const px_char name[], px_point point);
px_bool PX_AbiMemoryWrite_color(px_memory* pmem, const px_char name[], px_color color);
px_bool PX_AbiMemoryWrite_bool(px_memory* pmem, const px_char name[], px_bool _bool);
px_bool PX_AbiMemoryWrite_data(px_memory* pmem, const px_char name[], px_void* data, px_int size);



px_bool PX_AbiRead_int(px_abi* pabi,const px_char name[],px_int *_int);
px_bool PX_AbiRead_float(px_abi* pabi,const px_char name[],px_float *_float);
px_bool PX_AbiRead_double(px_abi* pabi,const px_char name[],px_double *_double);
px_bool PX_AbiRead_string(px_abi* pabi,const px_char name[],px_char *_string[]);
px_bool PX_AbiRead_point(px_abi* pabi,const px_char name[],px_point *point);
px_bool PX_AbiRead_color(px_abi* pabi,const px_char name[],px_color *color);
px_bool PX_AbiRead_bool(px_abi* pabi,const px_char name[],px_bool *_bool);
px_bool PX_AbiRead_data(px_abi* pabi,const px_char name[],px_void *data,px_int size);

px_byte* PX_AbiReadNext_desc(px_byte* pbuffer, px_abi_desc* pdesc);


px_void PX_AbiDynamicFree(px_abi* pabi);

#endif