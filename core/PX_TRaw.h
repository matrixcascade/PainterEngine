#ifndef _PX_TRAW_H
#define _PX_TRAW_H

#include "../core/PX_Surface.h"

typedef struct _PX_TRaw_Header
{
	px_byte  Magic[4];//TRAW 0x57415254
	px_int32 Width;
	px_int32 Height;  
}PX_TRaw_Header;

px_bool PX_TRawVerify(px_void *data,px_int size);
px_int PX_TRawGetWidth(px_void *data);
px_int PX_TRawGetHeight(px_void *data);
px_uint PX_TRawGetSize(PX_TRaw_Header *header);
px_void PX_TRawRender(px_surface *psurface,px_void *data,px_int x,px_int y);
px_bool PX_TRawBuild(px_surface *psurface,px_byte *TRawBuffer,px_int *size);
#endif
