#ifndef __PX_IMAGEFILTER_H
#define __PX_IMAGEFILTER_H
#include "PX_Texture.h"

px_void PX_ImageFilter_PriwittX(px_texture *ptexture,px_float out[]);
px_void PX_ImageFilter_PriwittY(px_texture *ptexture,px_float out[]);
px_void PX_ImageFilter_Priwitt(px_texture *ptexture,px_float out[]);
px_void PX_ImageFilter_SobelX(px_texture *ptexture,px_float out[]);
px_void PX_ImageFilter_SobelY(px_texture *ptexture,px_float out[]);
px_void PX_ImageFilter_Sobel(px_texture *ptexture,px_float out[]);
px_void PX_ImageFilter_RobertsX(px_texture *ptexture,px_float out[]);
px_void PX_ImageFilter_RobertsY(px_texture *ptexture,px_float out[]);
px_void PX_ImageFilter_Roberts(px_texture *ptexture,px_float out[]);
px_void PX_ImageFilter_LaplacianX(px_texture *ptexture,px_float out[]);
px_void PX_ImageFilter_LaplacianY(px_texture *ptexture,px_float out[]);
px_void PX_ImageFilter_Laplacian(px_texture *ptexture,px_float out[]);

#endif
