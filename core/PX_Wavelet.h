#ifndef PX_WAVELET_H
#define PX_WAVELET_H
#include "PX_LTI.h"

#define  PX_DWT_FILTER_MAX_ORDER 32
typedef enum
{
	PX_DWT_BASE_TYPE_HAAR,
	PX_DWT_BASE_TYPE_DB2,
	PX_DWT_BASE_TYPE_DB3,
	PX_DWT_BASE_TYPE_DB4,
	PX_DWT_BASE_TYPE_DB5,
	PX_DWT_BASE_TYPE_DB6,
	PX_DWT_BASE_TYPE_DB7,
	PX_DWT_BASE_TYPE_DB8,
	PX_DWT_BASE_TYPE_SYM2,
	PX_DWT_BASE_TYPE_SYM3,
	PX_DWT_BASE_TYPE_SYM4,
	PX_DWT_BASE_TYPE_SYM5,
	PX_DWT_BASE_TYPE_SYM6,
	PX_DWT_BASE_TYPE_SYM7,
	PX_DWT_BASE_TYPE_SYM8,
}PX_DWT_BASE_TYPE;

typedef struct 
{
	px_memorypool* mp;
	px_char name[8];
	PX_LTI h0,h1,g0,g1;
	PX_DWT_BASE_TYPE type;
}PX_DWT;

typedef PX_DWT PX_FWT;
px_bool PX_DWTInitialize(px_memorypool* mp, PX_DWT* dwt, px_char name[8]);
px_bool PX_DWTGo(PX_DWT* dwt, px_double in[], px_int in_size, px_double out[]);
px_bool PX_IDWTGo(PX_DWT* dwt, px_double in[], px_int in_size, px_double out[]);
px_void PX_DWTFree(PX_DWT* dwt);
#endif