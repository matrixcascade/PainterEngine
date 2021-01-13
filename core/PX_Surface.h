#ifndef __PX_SURFACE_H
#define __PX_SURFACE_H
#include "PX_MemoryPool.h"

typedef struct _PX_Surface
{
	px_memorypool *MP;
	px_color *surfaceBuffer;
	px_int height;
	px_int width;
	px_int limit_left;
	px_int limit_top;
	px_int limit_right;
	px_int limit_bottom;
}px_surface;
#define PX_SURFACECOLOR(main_pSurface,X,Y) ((main_pSurface)->surfaceBuffer[(X)+(main_pSurface)->width*(Y)])
#define PX_SurfaceGetPixel(main_pSurface,X,Y)  PX_SURFACECOLOR(main_pSurface,X,Y)

typedef struct
{
	px_int limit_left;
	px_int limit_top;
	px_int limit_right;
	px_int limit_bottom;
}PX_SurfaceLimitInfo;

PX_SurfaceLimitInfo PX_SurfaceGetLimit(px_surface *ps);
px_void PX_SurfaceSetLimitInfo(px_surface *ps,PX_SurfaceLimitInfo info);
px_void PX_SurfaceSetLimit(px_surface *ps,px_int limit_left,px_int limit_top,px_int limit_right,px_int limit_bottom);
px_void PX_SurfaceSetPixel(px_surface *ps,px_int x,px_int y,px_color color);
px_void PX_SurfaceDrawPixel(px_surface *ps,px_int x,px_int y,px_color color);
px_bool PX_SurfaceCreate(px_memorypool *mp,px_uint width,px_uint height,_OUT px_surface *surface);
px_void PX_SurfaceClear(px_surface *ps, px_int left, px_int top, px_int right, px_int bottom,px_color color);
px_void PX_SurfaceFree(px_surface *psurface);
px_int PX_SurfaceMemorySize(px_uint width,px_uint height);
#endif
