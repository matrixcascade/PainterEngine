#ifndef PX_LIVEFRAMEWORK_H
#define PX_LIVEFRAMEWORK_H

#include "PX_LiveLayer.h"

#define PX_LIVEFRAMEWORK_MAX_SUPPORT_LAYER 256
#define PX_LIVEFRAMEWORK_MAX_ID 32
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

typedef px_void (*PX_LiveFramework_PixelShader)(px_surface *psurface,px_int x,px_int y,px_point position,px_float u,px_float v,px_point normal,px_texture *pTexture,PX_TEXTURERENDER_BLEND *blend);

typedef struct
{
	px_memorypool *mp;
	px_char id[PX_LIVEFRAMEWORK_MAX_ID];
	px_vector layers;
	px_vector livetextures;
	px_int width;
	px_int height;
	px_bool showRange;
	PX_LiveFramework_PixelShader pixelShader;
}PX_LiveFramework;



px_bool PX_LiveFrameworkInitialize(px_memorypool *mp,PX_LiveFramework *plive,px_int width,px_int height);
px_bool PX_LiveFrameworkLoad(PX_LiveFramework *plive,const px_char json_content[]);
px_void PX_LiveFrameworkUpdate(PX_LiveFramework *plive,px_dword elpased);
px_void PX_LiveFrameworkRender(px_surface *psurface,PX_LiveFramework *plive,px_int x,px_int y,PX_TEXTURERENDER_REFPOINT refPoint,px_dword elpased);
px_int  PX_LiveFrameworkGetLayerTextureWidth(PX_LiveLayer *pLayer);
px_int  PX_LiveFrameworkGetLayerTextureHeight(PX_LiveLayer *pLayer);
px_bool PX_LiveFrameworkPlayAnimation(PX_LiveFramework *plive,const px_char name[]);
PX_LiveLayer *PX_LiveFrameworkGetLayerById(PX_LiveFramework *plive,const px_char id[]);
PX_LiveLayer *PX_LiveFrameworkCreateLayer(PX_LiveFramework *plive,const px_char id[]);
PX_LiveLayer *PX_LiveFrameworkGetLayer(PX_LiveFramework *plive,px_int i);
PX_LiveLayer *PX_LiveFrameworkGetLastCreateLayer(PX_LiveFramework *plive);
px_bool PX_LiveFrameworkAddLiveTexture(PX_LiveFramework *plive,PX_LiveTexture livetexture);
PX_LiveTexture *PX_LiveFrameworkGetLiveTextureById(PX_LiveFramework *plive,const px_char id[]);
px_void PX_LiveFrameworkFree(PX_LiveFramework *plive);
#endif