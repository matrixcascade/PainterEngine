#ifndef __PX_TEXTURE_H
#define __PX_TEXTURE_H

#include "PX_Vector.h"
#include "PX_Surface.h"
#include "PX_Bitmap.h"
#include "PX_TRaw.h"

typedef px_surface px_texture;


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
px_void PX_TextureRender(px_surface *psurface,px_texture *tex,px_int x,px_int y,PX_ALIGN refPoint,PX_TEXTURERENDER_BLEND *blend);
px_void PX_TextureCover(px_surface *psurface,px_texture *tex,px_int x,px_int y,PX_ALIGN refPoint);
px_void PX_TextureGetVisibleRange(px_texture *ptexture,px_int *pLeft,px_int *pRight,px_int *pTop,px_int *pBottom);
typedef px_void (*PX_TexturePixelShader)(px_surface *psurface,px_int x,px_int y,px_color clr,px_void *ptr);
px_void PX_TextureRenderPixelShader(px_surface *psurface,px_texture *tex,px_int x,px_int y,PX_ALIGN refPoint,PX_TexturePixelShader shader,px_void *ptr);
px_void PX_TextureRenderRotation(px_surface *psurface,px_texture *tex,px_int x,px_int y,PX_ALIGN refPoint,PX_TEXTURERENDER_BLEND *blend,px_int Angle);
px_void PX_TextureRenderRotation_vector(px_surface *psurface,px_texture *tex,px_int x,px_int y,PX_ALIGN refPoint,PX_TEXTURERENDER_BLEND *blend,px_point p_vector);
px_void PX_TextureRenderRotation_sincos(px_surface *psurface,px_texture *tex,px_int x,px_int y,PX_ALIGN refPoint,PX_TEXTURERENDER_BLEND *blend,px_float sin,px_float cos);

px_void PX_TextureRenderEx(px_surface *psurface,px_texture *resTexture,px_int x,px_int y,PX_ALIGN refPoint,PX_TEXTURERENDER_BLEND *blend,px_float scale,px_float Angle);
px_void PX_TextureRenderEx_sincos(px_surface *psurface,px_texture *resTexture,px_int x,px_int y,PX_ALIGN refPoint,PX_TEXTURERENDER_BLEND *blend,px_float scale,px_float sin,px_float cos);
px_void PX_TextureRenderEx_vector(px_surface *psurface,px_texture *resTexture,px_int x,px_int y,PX_ALIGN refPoint,PX_TEXTURERENDER_BLEND *blend,px_float scale,px_point p_vector);
px_void PX_TextureRenderMask(px_surface *psurface,px_texture *mask_tex,px_texture *map_tex,px_int x,px_int y,PX_ALIGN refPoint,PX_TEXTURERENDER_BLEND *blend);
px_void PX_TextureRenderMaskEx(px_surface *psurface,px_texture *mask_tex,px_texture *map_tex,px_int x,px_int y,PX_ALIGN refPoint,PX_TEXTURERENDER_BLEND *blend,px_float scale,px_float Angle);
px_void PX_TextureFill(px_memorypool *mp,px_texture *ptexture,px_int x,px_int y,px_color test_color,px_color fill_color);
px_void PX_TextureRegionRender(px_surface *psurface,px_texture *resTexture,px_int x,px_int y,px_int left,px_int top,px_int right,px_int bottom,PX_ALIGN refPoint,PX_TEXTURERENDER_BLEND *blend);
px_void PX_TextureRegionCopy(px_surface *psurface,px_texture *resTexture,px_int x,px_int y,px_int oft_left,px_int oft_top,px_int oft_right,px_int oft_bottom,PX_ALIGN refPoint,PX_TEXTURERENDER_BLEND *blend);
px_void PX_SurfaceRender(px_surface *pdestSurface,px_surface *pResSurface,px_int x,px_int y,PX_ALIGN refPoint,PX_TEXTURERENDER_BLEND *blend);
px_void PX_SurfaceCover(px_surface *pdestSurface,px_surface *pResSurface,px_int x,px_int y,PX_ALIGN refPoint);
px_void PX_SurfaceSetRect(px_surface *psurface, px_int left, px_int top, px_int right, px_int bottom,px_color color);
px_bool PX_TextureCreateRotationAngle(px_memorypool *mp,px_texture *resTexture,px_float Angle,px_texture *out);
px_bool PX_TextureRotationAngleToTexture(px_texture *resTexture,px_float Angle,px_texture *out);
px_bool PX_TextureCreateRotationRadian(px_memorypool *mp,px_texture *resTexture,px_float Angle,px_texture *out);
px_void PX_TextureFree(px_texture *tex);

px_bool PX_ShapeCreate(px_memorypool *mp,px_shape *shape,px_int width,px_int height);
px_bool PX_ShapeCreateFromTexture(px_memorypool *mp,px_shape *shape,px_texture *texture);
px_bool PX_ShapeCreateFromMemory(px_memorypool *mp,px_void *data,px_int size,px_shape *shape);
px_void PX_ShapeRender(px_surface *psurface,px_shape *shape,px_int x,px_int y,PX_ALIGN refPoint,px_color blendColor);
px_void PX_ShapeRenderEx(px_surface *psurface,px_shape *shape,px_int x,px_int y,PX_ALIGN refPoint,px_color blendColor,px_float scale,px_float Angle);
px_void PX_ShapeRenderEx_sincos(px_surface *psurface,px_shape *shape,px_int x,px_int y,PX_ALIGN refPoint,px_color blendColor,px_float scale,px_float sinx,px_float cosx);
px_void PX_ShapeRenderEx_vector(px_surface *psurface,px_shape *shape,px_int x,px_int y,PX_ALIGN refPoint,px_color blendcolor,px_float scale,px_point p_vector);
px_void PX_ShapeFree(px_shape *shape);



#endif
