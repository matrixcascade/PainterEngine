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
px_void PX_ImageFilter_Gray(px_texture *ptexture);
px_void PX_ImageFilter_Binarization(px_texture* ptexture, px_byte threshhold);
px_void PX_ImageFilter_dBinarization(px_texture* ptexture, px_byte threshhold, px_byte dthreshhold);
px_double PX_ImageFilter_MSSIM(px_texture* mod_tex, px_texture* target_tex, px_int channel_0R1G2B3L);

px_void PX_TextureFilter_PriwittX(px_texture* ptexture, px_float out[]);
px_void PX_TextureFilter_PriwittY(px_texture* ptexture, px_float out[]);
px_void PX_TextureFilter_Priwitt(px_texture* ptexture, px_float out[]);
px_void PX_TextureFilter_SobelX(px_texture* ptexture, px_float out[]);
px_void PX_TextureFilter_SobelY(px_texture* ptexture, px_float out[]);
px_void PX_TextureFilter_Sobel(px_texture* ptexture, px_float out[]);
px_void PX_TextureFilter_RobertsX(px_texture* ptexture, px_float out[]);
px_void PX_TextureFilter_RobertsY(px_texture* ptexture, px_float out[]);
px_void PX_TextureFilter_Roberts(px_texture* ptexture, px_float out[]);
px_void PX_TextureFilter_LaplacianX(px_texture* ptexture, px_float out[]);
px_void PX_TextureFilter_LaplacianY(px_texture* ptexture, px_float out[]);
px_void PX_TextureFilter_Laplacian(px_texture* ptexture, px_float out[]);
px_void PX_TextureFilter_Gray(px_texture* ptexture);
px_void PX_TextureFilter_Binarization(px_texture* ptexture, px_byte threshhold);
px_void PX_TextureFilter_dBinarization(px_texture* ptexture, px_byte threshhold, px_byte dthreshhold);
px_double PX_TextureFilter_MSSIM(px_texture* mod_tex, px_texture* target_tex, px_int channel_0R1G2B3L);

#endif
