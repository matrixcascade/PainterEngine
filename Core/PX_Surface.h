#ifndef __PX_SURFACE_H
#define __PX_SURFACE_H
#include "PX_MemoryPool.h"

typedef struct _PX_Surface
{
	px_color *surfaceBuffer;
	px_int height;
	px_int width;
	px_memorypool *MP;
}px_surface;
#define PX_SURFACECOLOR(main_pSurface,X,Y) ((main_pSurface)->surfaceBuffer[(X)+(main_pSurface)->width*(Y)])
#define PX_SurfaceGetPixel(main_pSurface,X,Y)  PX_SURFACECOLOR(main_pSurface,X,Y)
#define PX_SurfaceSetPixel(main_pSurface,X,Y,COLOR) ((main_pSurface)->surfaceBuffer[(X)+(main_pSurface)->width*(Y)]=COLOR)
px_void PX_SurfaceDrawPixel(px_surface *ps,px_int x,px_int y,px_color color);
px_void PX_SurfaceDrawPixelFaster(px_surface *psurface,px_int X,px_int Y,px_color COLOR);
px_bool PX_SurfaceCreate(px_memorypool *mp,px_uint width,px_uint height,_OUT px_surface *surface);
px_void PX_SurfaceClear(px_surface *ps, px_int left, px_int top, px_int right, px_int bottom,px_color color);
px_void PX_SurfaceFree(px_surface *psurface);
#endif
