#ifndef __PX_TEXTURE_H
#define __PX_TEXTURE_H

#include "PX_Surface.h"
#include "PX_Bitmap.h"
#include "PX_TRaw.h"

typedef px_surface px_texture;

typedef enum
{
	PX_TEXTURERENDER_REFPOINT_LEFTTOP,
	PX_TEXTURERENDER_REFPOINT_MIDTOP,
	PX_TEXTURERENDER_REFPOINT_RIGHTTOP,
	PX_TEXTURERENDER_REFPOINT_LEFTMID,
	PX_TEXTURERENDER_REFPOINT_CENTER,
	PX_TEXTURERENDER_REFPOINT_RIGHTMID,
	PX_TEXTURERENDER_REFPOINT_LEFTBOTTOM,
	PX_TEXTURERENDER_REFPOINT_MIDBOTTOM,
	PX_TEXTURERENDER_REFPOINT_RIGHTBOTTOM,
}PX_TEXTURERENDER_REFPOINT;

typedef struct
{
	float hdr_R;
	float hdr_G;
	float hdr_B;
	float alpha;
}PX_TEXTURERENDER_BLEND;

typedef struct
{
	px_uchar *alpha;
	px_int height;
	px_int width;
	px_memorypool *MP;
}px_shape;

px_bool PX_TextureCreate(px_memorypool *mp,px_texture *tex,px_int width,px_int height);
px_bool PX_TextureCreateFromMemory(px_memorypool *mp,px_void *data,px_int size,px_texture *tex);
px_bool PX_TextureCopy(px_memorypool *mp,px_texture *restexture,px_texture *dest);
px_bool PX_TextureCreateScale(px_memorypool *mp,px_texture *resTexture,px_int newWidth,px_int newHeight,px_texture *out);
px_void PX_TextureRender(px_surface *psurface,px_texture *tex,px_int x,px_int y,PX_TEXTURERENDER_REFPOINT refPoint,PX_TEXTURERENDER_BLEND *blend);
px_void PX_TextureCover(px_surface *psurface,px_texture *tex,px_int x,px_int y,PX_TEXTURERENDER_REFPOINT refPoint);
px_void PX_TextureRenderRotation(px_surface *psurface,px_texture *tex,px_int x,px_int y,PX_TEXTURERENDER_REFPOINT refPoint,PX_TEXTURERENDER_BLEND *blend,px_int Angle);
px_void PX_TextureRenderRotation_vector(px_surface *psurface,px_texture *tex,px_int x,px_int y,PX_TEXTURERENDER_REFPOINT refPoint,PX_TEXTURERENDER_BLEND *blend,px_point p_vector);
px_void PX_TextureRenderRotation_sincos(px_surface *psurface,px_texture *tex,px_int x,px_int y,PX_TEXTURERENDER_REFPOINT refPoint,PX_TEXTURERENDER_BLEND *blend,px_float sin,px_float cos);

px_void PX_TextureRenderEx(px_surface *psurface,px_texture *resTexture,px_int x,px_int y,PX_TEXTURERENDER_REFPOINT refPoint,PX_TEXTURERENDER_BLEND *blend,px_float scale,px_float Angle);
px_void PX_TextureRenderEx_sincos(px_surface *psurface,px_texture *resTexture,px_int x,px_int y,PX_TEXTURERENDER_REFPOINT refPoint,PX_TEXTURERENDER_BLEND *blend,px_float scale,px_float sin,px_float cos);
px_void PX_TextureRenderEx_vector(px_surface *psurface,px_texture *resTexture,px_int x,px_int y,PX_TEXTURERENDER_REFPOINT refPoint,PX_TEXTURERENDER_BLEND *blend,px_float scale,px_point p_vector);
px_void PX_TextureRenderMask(px_surface *psurface,px_texture *mask_tex,px_texture *map_tex,px_int x,px_int y,PX_TEXTURERENDER_REFPOINT refPoint,PX_TEXTURERENDER_BLEND *blend);
px_void PX_TextureRenderMaskEx(px_surface *psurface,px_texture *mask_tex,px_texture *map_tex,px_int x,px_int y,PX_TEXTURERENDER_REFPOINT refPoint,PX_TEXTURERENDER_BLEND *blend,px_float scale,px_float Angle);

px_void PX_SurfaceRender(px_surface *pdestSurface,px_surface *pResSurface,px_int x,px_int y,PX_TEXTURERENDER_REFPOINT refPoint,PX_TEXTURERENDER_BLEND *blend);
px_void PX_SurfaceCover(px_surface *pdestSurface,px_surface *pResSurface,px_int x,px_int y,PX_TEXTURERENDER_REFPOINT refPoint);
px_void PX_SurfaceSetRect(px_surface *psurface, px_int left, px_int top, px_int right, px_int bottom,px_color color);
px_bool PX_TextureCreateRotationAngle(px_memorypool *mp,px_texture *resTexture,px_float Angle,px_texture *out);
px_bool PX_TextureRotationAngleToTexture(px_texture *resTexture,px_float Angle,px_texture *out);
px_bool PX_TextureCreateRotationRadian(px_memorypool *mp,px_texture *resTexture,px_float Angle,px_texture *out);
px_void PX_TextureFree(px_texture *tex);

px_bool PX_ShapeCreate(px_memorypool *mp,px_shape *shape,px_int width,px_int height);
px_bool PX_ShapeCreateFromTexture(px_memorypool *mp,px_shape *shape,px_texture *texture);
px_bool PX_ShapeCreateFromMemory(px_memorypool *mp,px_void *data,px_int size,px_shape *shape);
px_void PX_ShapeRender(px_surface *psurface,px_shape *shape,px_int x,px_int y,PX_TEXTURERENDER_REFPOINT refPoint,px_color blendColor);
px_void PX_ShapeRenderEx(px_surface *psurface,px_shape *shape,px_int x,px_int y,PX_TEXTURERENDER_REFPOINT refPoint,px_color blendColor,px_float scale,px_float Angle);
px_void PX_ShapeRenderEx_sincos(px_surface *psurface,px_shape *shape,px_int x,px_int y,PX_TEXTURERENDER_REFPOINT refPoint,px_color blendColor,px_float scale,px_float sinx,px_float cosx);
px_void PX_ShapeRenderEx_vector(px_surface *psurface,px_shape *shape,px_int x,px_int y,PX_TEXTURERENDER_REFPOINT refPoint,px_color blendcolor,px_float scale,px_point p_vector);
px_void PX_ShapeFree(px_shape *shape);
#endif
