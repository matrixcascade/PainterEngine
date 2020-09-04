#ifndef PX_MFCC_H
#define PX_MFCC_H
#include "PX_MemoryPool.h"

#define  PX_MFCC_FRAME_MAX_SIZE 2048
#define  PX_MFCC_DCT_FACTOR_SIZE 32
typedef struct
{
	px_double w[PX_MFCC_FRAME_MAX_SIZE];
}PX_MFCC_Filter;


typedef struct
{
	PX_MFCC_Filter filter[PX_MFCC_DCT_FACTOR_SIZE];
	px_int N;
}PX_MFCC;

typedef struct  
{
	px_double factor[PX_MFCC_DCT_FACTOR_SIZE];
}PX_MFCC_FEATURE;

px_void PX_MFCCInitialize(PX_MFCC *mfcc,px_int framesize,px_int sampleRate,px_int low,px_int high);
px_bool PX_MFCCParse(PX_MFCC *mfcc,px_double *data,PX_MFCC_FEATURE *out);
#endif


