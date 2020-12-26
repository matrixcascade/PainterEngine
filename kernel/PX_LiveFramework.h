#ifndef PX_LIVEFRAMEWORK_H
#define PX_LIVEFRAMEWORK_H

#include "PX_LiveLayer.h"

typedef struct
{
	px_point  position;
	px_point normal;
	px_float    u,v;
}PX_LiveFramework_Vertex;

typedef struct
{
	px_int Index1;
	px_int Index2;
	px_int Index3;
}PX_LiveFramework_Triangle;

typedef px_void (*PX_LiveFramework_PixelShader)(px_surface *psurface,px_int x,px_int y,px_point position,px_float u,px_float v,px_point normal,px_texture *pTexture);

typedef struct
{
	px_memorypool *mp;
	px_vector animation;
	px_vector layers;
	px_int surfaceWidth;
	px_int surfaceHeight;
	px_int width;
	px_int height;
	px_float *zbuffer;
	px_bool showRange;
	PX_LiveFramework_PixelShader pixelShader;
}PX_LiveFramework;



px_bool PX_LiveFrameworkInitialize(px_memorypool *mp,PX_LiveFramework *plive,px_int width,px_int height,px_int surfaceWidth,px_int surfaceHeight);
px_bool PX_LiveFrameworkLoad(PX_LiveFramework *plive,const px_char json_content[]);
px_void PX_LiveFrameworkUpdate(PX_LiveFramework *plive,px_dword elpased);
px_void PX_LiveFrameworkRender(px_surface *psurface,PX_LiveFramework *plive,px_int x,px_int y,PX_TEXTURERENDER_REFPOINT refPoint,px_dword elpased);
px_bool PX_LiveFrameworkPlayAnimation(PX_LiveFramework *plive,const px_char name[]);
PX_LiveLayer *PX_LiveFrameworkCreateLayer(PX_LiveFramework *plive);
PX_LiveLayer *PX_LiveFrameworkGetLayer(PX_LiveFramework *plive,px_int i);
PX_LiveLayer *PX_LiveFrameworkGetLastCreateLayer(PX_LiveFramework *plive);
px_void PX_LiveFrameworkFree(PX_LiveFramework *plive);
#endif