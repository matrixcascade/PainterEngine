#ifndef __PX_LTI_H
#define __PX_LTI_H
#include "PX_Memory.h"
#include "PX_Signal.h"

#define PX_LTI_MAX_ORDER 16
typedef struct 
{
	px_memorypool* mp;
	px_float b[PX_LTI_MAX_ORDER];
	px_float a[PX_LTI_MAX_ORDER];
	px_int b_count;
	px_int a_count;
	PX_CircularBuffer cbuf, cbufout;
	px_bool dirty_grdel;
	px_float grdel;
}PX_LTI;

px_bool PX_LTIInitialize(PX_LTI* pLTI, px_memorypool* mp, px_int sizeB, px_int sizeA);
px_void PX_LTISetKernel(PX_LTI* pLTI, px_float a[], px_float b[]);
px_void PX_LTIPush(PX_LTI* pLTI, px_float v);
px_float PX_LTIFilter(PX_LTI* pLTI, px_float v);
px_float PX_LTIConvol(PX_LTI* pLTI);
px_float PX_LTIGroupDelay(PX_LTI* pLTI, px_float f, px_float fs);
px_float PX_LTIPhaseDelay(PX_LTI* pLTI, px_float f, px_float fs);
px_void PX_LTIFree(PX_LTI* pLTI);

typedef struct 
{
	px_float D;
	px_int N;
	PX_LTI LTI;
}PX_Thirian;
px_bool PX_ThirianInitialize(PX_Thirian* pThi, px_memorypool* mp, px_int N);
px_void PX_ThirianSetCoeffs(PX_Thirian* pThi, px_float D);
px_void PX_ThirianFree(PX_Thirian* pThi);

typedef struct 
{
	px_float freq;
	px_float c1;
	px_float c3;
	PX_LTI LTI;
}PX_FilterC1C3;
px_bool PX_FilterC1C3Initialize(PX_FilterC1C3* pFilter, px_memorypool* mp);
px_void PX_FilterC1C3SetCoeffs(PX_FilterC1C3* pFilter, px_float freq, px_float c1, px_float c3);
px_void PX_FilterC1C3Free(PX_FilterC1C3* pFilter);

typedef enum  {
	PX_BIQUAD_TYPE_PASS = 0,
	PX_BIQUAD_TYPE_LOW,
	PX_BIQUAD_TYPE_HIGH,
	PX_BIQUAD_TYPE_NOTCH
}PX_BIQUAD_TYPE;
typedef struct 
{
	PX_LTI LTI;
}PX_Biquad;

px_bool PX_BiquadInitialize(PX_Biquad* pBiquad, px_memorypool* mp);
px_void PX_BiquadSetCoeffs(PX_Biquad* pBiquad, px_float f0, px_float fs, px_float Q, PX_BIQUAD_TYPE type);
px_void PX_BiquadFree(PX_Biquad* pBiquad);


typedef enum 
{
	PX_DELAY_DATA_TYPE_FLOAT,
	PX_DELAY_DATA_TYPE_INT,
}PX_DELAY_DATA_TYPE;
typedef struct  
{
	px_memorypool* mp;
	px_int	  inv_z;
	px_int cursor;
	PX_DELAY_DATA_TYPE type;
	px_void* buffer;
}PX_Delay;

px_bool PX_DelayInitialize(PX_Delay* pdelay, px_memorypool* mp, px_int inv_z, PX_DELAY_DATA_TYPE type);
px_void  PX_DelayGo_float(PX_Delay* pdelay, px_float in[], px_float out[], px_int size);
px_void  PX_DelayGo_int(PX_Delay* pdelay, px_int in[], px_int out[], px_int size);
px_void PX_DelayFree(PX_Delay* pDelay);
#endif