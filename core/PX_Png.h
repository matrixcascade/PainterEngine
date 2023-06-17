#ifndef PX_PNG_H
#define PX_PNG_H

#include "PX_MemoryPool.h"
#include "PX_RFC1951.h"
#include "PX_Surface.h"

typedef struct _px_shape px_shape;
extern px_void PX_ShapeSetPixel(px_shape* shape, px_int x, px_int y, px_uchar value);


typedef enum
{
	PX_PNG_COLOR_TYPE_LUM = 0,
	PX_PNG_COLOR_TYPE_RGB = 2,
	PX_PNG_COLOR_TYPE_LUMA = 4,
	PX_PNG_COLOR_TYPE_RGBA = 6,
}PX_PNG_COLOR_TYPE;

typedef enum
{
	PX_PNG_FORMAT_LUMINANCE1,
	PX_PNG_FORMAT_LUMINANCE2,
	PX_PNG_FORMAT_LUMINANCE4,
	PX_PNG_FORMAT_LUMINANCE8,
	PX_PNG_FORMAT_LUMINANCE_ALPHA1,
	PX_PNG_FORMAT_LUMINANCE_ALPHA2,
	PX_PNG_FORMAT_LUMINANCE_ALPHA4,
	PX_PNG_FORMAT_LUMINANCE_ALPHA8,
	PX_PNG_FORMAT_R8G8B8,
	PX_PNG_FORMAT_R16G16B16,
	PX_PNG_FORMAT_R8G8B8A8,
	PX_PNG_FORMAT_R16G16B16A16,
	PX_PNG_FORMAT_ERROR,
}PX_PNG_FORMAT;

px_bool PX_PngVerify(px_byte* ppngbuffer, px_int size, px_int* width, px_int* height, PX_PNG_FORMAT* format);
px_bool PX_PngToRenderBuffer(px_memorypool* calcBuffer, px_byte* ppngbuffer, px_int size, px_surface* prenderbuffer);
px_bool PX_PngToShapeBuffer(px_memorypool* calcBuffer, px_byte* ppngbuffer, px_int size, px_shape* pshapebuffer);
px_int  PX_PngGetSize(px_byte* ppngbuffer, px_int in_size);
px_bool PX_PngSurfaceToBuffer(px_surface* prenderbuffer,px_memory *out);
#endif
