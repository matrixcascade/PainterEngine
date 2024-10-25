#include "PX_ImageFilter.h"

px_void PX_ImageFilter_Convolution(px_texture *ptexture,px_float out[],px_float matrix_operator[3][3],px_bool raw)
{
	px_int i,j;
	px_float uniform=0;
	px_float maxGray=0;
	for (i=0;i<3;i++)
	{
		for (j=0;j<3;j++)
		{
			if (matrix_operator[i][j]>0)
			{
				uniform+=matrix_operator[i][j];
			}
		}
	}

	for (j=0;j<ptexture->height-1;j++)
	{
		for (i=0;i<ptexture->width-1;i++)
		{
			px_color clr;
			px_float Gray;
			clr=PX_SurfaceGetPixel(ptexture,i-1,j-1);
			Gray = clr._argb.r*0.299f+ clr._argb.g*0.587f + clr._argb.b*0.114f;
			if (Gray>maxGray)
			{
				maxGray=Gray;
			}
		}
	}

	for (j=1;j<ptexture->height-1;j++)
	{
		for (i=1;i<ptexture->width-1;i++)
		{
			px_color clr;
			px_float Gray;
			px_float weight=0;
			clr=PX_SurfaceGetPixel(ptexture,i-1,j-1);
			Gray = clr._argb.r*0.299f+ clr._argb.g*0.587f + clr._argb.b*0.114f;
			Gray*=clr._argb.a/255.f;
			
			weight+=Gray*matrix_operator[0][0];

			clr=PX_SurfaceGetPixel(ptexture,i,j-1);
			Gray = clr._argb.r*0.299f+ clr._argb.g*0.587f + clr._argb.b*0.114f;
			Gray*=clr._argb.a/255.f;
			weight+=Gray*matrix_operator[0][1];

			clr=PX_SurfaceGetPixel(ptexture,i+1,j-1);
			Gray = clr._argb.r*0.299f+ clr._argb.g*0.587f + clr._argb.b*0.114f;
			Gray*=clr._argb.a/255.f;
			weight+=Gray*matrix_operator[0][2];


			clr=PX_SurfaceGetPixel(ptexture,i-1,j);
			Gray = clr._argb.r*0.299f+ clr._argb.g*0.587f + clr._argb.b*0.114f;
			Gray*=clr._argb.a/255.f;
			weight+=Gray*matrix_operator[1][0];

			clr=PX_SurfaceGetPixel(ptexture,i,j);
			Gray = clr._argb.r*0.299f+ clr._argb.g*0.587f + clr._argb.b*0.114f;
			Gray*=clr._argb.a/255.f;
			weight+=Gray*matrix_operator[1][1];

			clr=PX_SurfaceGetPixel(ptexture,i+1,j);
			Gray = clr._argb.r*0.299f+ clr._argb.g*0.587f + clr._argb.b*0.114f;
			Gray*=clr._argb.a/255.f;
			weight+=Gray*matrix_operator[1][2];



			clr=PX_SurfaceGetPixel(ptexture,i-1,j+1);
			Gray = clr._argb.r*0.299f+ clr._argb.g*0.587f + clr._argb.b*0.114f;
			Gray*=clr._argb.a/255.f;
			weight+=Gray*matrix_operator[2][0];

			clr=PX_SurfaceGetPixel(ptexture,i,j+1);
			Gray = clr._argb.r*0.299f+ clr._argb.g*0.587f + clr._argb.b*0.114f;
			Gray*=clr._argb.a/255.f;
			weight+=Gray*matrix_operator[2][1];

			clr=PX_SurfaceGetPixel(ptexture,i+1,j+1);
			Gray = clr._argb.r*0.299f+ clr._argb.g*0.587f + clr._argb.b*0.114f;
			Gray*=clr._argb.a/255.f;
			weight+=Gray*matrix_operator[2][2];

			if (raw)
			{
				out[j*ptexture->width+i]+=(weight)/(uniform*maxGray);
			}
			else
			{
				out[j*ptexture->width+i]+=PX_ABS(weight)/(uniform*maxGray);
			}
			
		}
	}
	
}

px_void PX_ImageFilter_PriwittX(px_texture *ptexture,px_float out[])
{
	px_float Priwitt_h[3][3]={{-1,0,1},{-1,0,1},{-1,0,1}};
	PX_memset(out,0,ptexture->width*ptexture->height*sizeof(px_float));
	PX_ImageFilter_Convolution(ptexture,out,Priwitt_h,PX_TRUE);
}

px_void PX_ImageFilter_PriwittY(px_texture *ptexture,px_float out[])
{
	px_float Priwitt_v[3][3]={{-1,-1,-1},{0,0,0},{1,1,1}};
	PX_memset(out,0,ptexture->width*ptexture->height*sizeof(px_float));
	PX_ImageFilter_Convolution(ptexture,out,Priwitt_v,PX_TRUE);
}


px_void PX_ImageFilter_Priwitt(px_texture *ptexture,px_float out[])
{
	px_float Priwitt_h[3][3]={{-1,0,1},{-1,0,1},{-1,0,1}};
	px_float Priwitt_v[3][3]={{-1,-1,-1},{0,0,0},{1,1,1}};
	PX_memset(out,0,ptexture->width*ptexture->height*sizeof(px_float));
	PX_ImageFilter_Convolution(ptexture,out,Priwitt_h,PX_FALSE);
	PX_ImageFilter_Convolution(ptexture,out,Priwitt_v,PX_FALSE);

	do 
	{
		px_int i;
		for (i=0;i<ptexture->width*ptexture->height;i++)
		{
			out[i]/=2;
		}
	} while (0);
}


px_void PX_ImageFilter_SobelX(px_texture *ptexture,px_float out[])
{
	px_float Sobel_h[3][3]={{-1,0,1},{-2,0,2},{-1,0,1}};
	PX_memset(out,0,ptexture->width*ptexture->height*sizeof(px_float));
	PX_ImageFilter_Convolution(ptexture,out,Sobel_h,PX_TRUE);
}
px_void PX_ImageFilter_SobelY(px_texture *ptexture,px_float out[])
{
	px_float Sobel_v[3][3]={{-1,-2,-1},{0,0,0},{1,2,1}};
	PX_memset(out,0,ptexture->width*ptexture->height*sizeof(px_float));
	PX_ImageFilter_Convolution(ptexture,out,Sobel_v,PX_TRUE);
}
px_void PX_ImageFilter_Sobel(px_texture *ptexture,px_float out[])
{
	px_float Sobel_h[3][3]={{-1,0,1},{-2,0,2},{-1,0,1}};
	px_float Sobel_v[3][3]={{-1,-2,-1},{0,0,0},{1,2,1}};
	PX_memset(out,0,ptexture->width*ptexture->height*sizeof(px_float));
	PX_ImageFilter_Convolution(ptexture,out,Sobel_h,PX_FALSE);
	PX_ImageFilter_Convolution(ptexture,out,Sobel_v,PX_FALSE);
	do 
	{
		px_int i;
		for (i=0;i<ptexture->width*ptexture->height;i++)
		{
			out[i]/=2;
		}
	} while (0);
}

px_void PX_ImageFilter_RobertsX(px_texture *ptexture,px_float out[])
{
	px_float Roberts_h[3][3]={{0,0,0},{0,-1,0},{0,0,1}};
	PX_memset(out,0,ptexture->width*ptexture->height*sizeof(px_float));
	PX_ImageFilter_Convolution(ptexture,out,Roberts_h,PX_TRUE);
}
px_void PX_ImageFilter_RobertsY(px_texture *ptexture,px_float out[])
{
	px_float Roberts_v[3][3]={{0,0,0},{0,0,-1},{0,1,0}};
	PX_memset(out,0,ptexture->width*ptexture->height*sizeof(px_float));
	PX_ImageFilter_Convolution(ptexture,out,Roberts_v,PX_TRUE);
}

px_void PX_ImageFilter_Roberts(px_texture *ptexture,px_float out[])
{
	px_float Roberts_h[3][3]={{0,0,0},{0,-1,0},{0,0,1}};
	px_float Roberts_v[3][3]={{0,0,0},{0,0,-1},{0,1,0}};
	PX_memset(out,0,ptexture->width*ptexture->height*sizeof(px_float));
	PX_ImageFilter_Convolution(ptexture,out,Roberts_h,PX_FALSE);
	PX_ImageFilter_Convolution(ptexture,out,Roberts_v,PX_FALSE);
	do 
	{
		px_int i;
		for (i=0;i<ptexture->width*ptexture->height;i++)
		{
			out[i]/=2;
		}
	} while (0);
}

px_void PX_ImageFilter_LaplacianX(px_texture *ptexture,px_float out[])
{
	px_float Roberts_h[3][3]={{0,-1,0},{-1,4,-1},{0,-1,0}};
	PX_memset(out,0,ptexture->width*ptexture->height*sizeof(px_float));
	PX_ImageFilter_Convolution(ptexture,out,Roberts_h,PX_TRUE);
}


px_void PX_ImageFilter_LaplacianY(px_texture *ptexture,px_float out[])
{
	px_float Roberts_v[3][3]={{-1,-1,-1},{-1,8,-1},{-1,-1,-1}};
	PX_memset(out,0,ptexture->width*ptexture->height*sizeof(px_float));
	PX_ImageFilter_Convolution(ptexture,out,Roberts_v,PX_TRUE);
}

px_void PX_ImageFilter_Laplacian(px_texture *ptexture,px_float out[])
{
	px_float Roberts_h[3][3]={{0,-1,0},{-1,4,-1},{0,-1,0}};
	px_float Roberts_v[3][3]={{-1,-1,-1},{-1,8,-1},{-1,-1,-1}};
	PX_memset(out,0,ptexture->width*ptexture->height*sizeof(px_float));
	PX_ImageFilter_Convolution(ptexture,out,Roberts_h,PX_FALSE);
	PX_ImageFilter_Convolution(ptexture,out,Roberts_v,PX_FALSE);
	do 
	{
		px_int i;
		for (i=0;i<ptexture->width*ptexture->height;i++)
		{
			out[i]/=2;
		}
	} while (0);
}

px_void PX_ImageFilter_Gray(px_texture* ptexture)
{
	px_int i,j;
	for (j=0;j<ptexture->height;j++)
	{
		for (i=0;i<ptexture->width;i++)
		{
			px_color clr=PX_SurfaceGetPixel(ptexture,i,j);
			px_uchar gray=(px_uchar)(clr._argb.r*0.299f+ clr._argb.g*0.587f + clr._argb.b*0.114f);
			PX_SurfaceSetPixel(ptexture,i,j,PX_COLOR(clr._argb.a,gray,gray,gray));
		}
	}
	
}

px_void PX_ImageFilter_Binarization(px_texture* ptexture,px_byte threshhold)
{
	px_int i,j;
	for (j=0;j<ptexture->height;j++)
	{
		for (i=0;i<ptexture->width;i++)
		{
			px_color clr=PX_SurfaceGetPixel(ptexture,i,j);
			px_uchar gray=(px_uchar)(clr._argb.r*0.299f+ clr._argb.g*0.587f + clr._argb.b*0.114f);
			if (gray>threshhold)
			{
				PX_SurfaceSetPixel(ptexture,i,j,PX_COLOR(255,255,255,255));
			}
			else
			{
				PX_SurfaceSetPixel(ptexture,i,j,PX_COLOR(255,0,0,0));
			}
		}
	}
}

px_void PX_ImageFilter_dBinarization(px_texture* ptexture, px_byte threshhold, px_byte dthreshhold)
{
	px_int i, j;
	
	
	for (j = 0; j < ptexture->height; j++)
	{
		px_int lastgray = 0;
		px_int pregray = 0;
		for (i = 0; i < ptexture->width; i++)
		{
			px_color clr = PX_SurfaceGetPixel(ptexture, i, j);
			px_int gray = (px_int)(clr._argb.r * 0.299f + clr._argb.g * 0.587f + clr._argb.b * 0.114f);
			px_int d = PX_ABS(gray - pregray);
			if (d >= dthreshhold)
			{
				lastgray = gray;
				if (gray > threshhold)
				{
					PX_SurfaceSetPixel(ptexture, i, j, PX_COLOR(255, 255, 255, 255));
				}
				else
				{
					PX_SurfaceSetPixel(ptexture, i, j, PX_COLOR(255, 0, 0, 0));
				}
			}
			else
			{
				if (lastgray > threshhold)
				{
					PX_SurfaceSetPixel(ptexture, i, j, PX_COLOR(255, 255, 255, 255));
				}
				else
				{
					PX_SurfaceSetPixel(ptexture, i, j, PX_COLOR(255, 0, 0, 0));
				}
			}
			pregray = gray;
		}
	}
}

static px_double PX_ImageFilter_SSIM(px_color* mod_tex, px_color* target_tex,px_int size,px_int channel_0R1G2B3L)
{
	px_int i;
	px_float ux=0, uy=0;
	px_float sx=0,sy=0,sxy=0;
	px_float ssim;
	px_float counter;
	counter = 0;
	for ( i = 0; i < size; i++)
	{
			px_color clr = mod_tex[i];
			px_float gray = 0;
			switch (channel_0R1G2B3L)
			{
			case 0:
				gray = clr._argb.r*clr._argb.a/255.f;
				break;
			case 1:
				gray = clr._argb.g * clr._argb.a / 255.f;
				break;
			case 2:
				gray = clr._argb.b * clr._argb.a / 255.f;
				break;
			case 3:
			default:
				gray = (clr._argb.r*0.299f + clr._argb.g*0.587f + clr._argb.b*0.114f) * clr._argb.a / 255.f;
				break;
			}
			ux += gray;

	}
	ux /= size;

	for (i = 0; i < size; i++)
	{
			px_color clr = target_tex[i];
			px_float gray = 0;
			switch (channel_0R1G2B3L)
			{
				case 0:
				gray = clr._argb.r*clr._argb.a / 255.f;
				break;
				case 1:
				gray = clr._argb.g * clr._argb.a / 255.f;
				break;
				case 2:
				gray = clr._argb.b * clr._argb.a / 255.f;
				break;
				case 3:
				default:
				gray = (clr._argb.r*0.299f + clr._argb.g*0.587f + clr._argb.b*0.114f) * clr._argb.a / 255.f;
			 break;
			}
			
			uy += gray;
		}

	uy /= size;

	for (i = 0; i < size; i++)
	{
			px_color clr = mod_tex[i];
			px_float gray = 0;
			switch (channel_0R1G2B3L)
			{
			case 0:
				gray = clr._argb.r * clr._argb.a / 255.f;
				break;
			case 1:
				gray = clr._argb.g * clr._argb.a / 255.f;
				break;
			case 2:
				gray = clr._argb.b * clr._argb.a / 255.f;
				break;
			case 3:
			default:
				gray = (clr._argb.r * 0.299f + clr._argb.g * 0.587f + clr._argb.b * 0.114f) * clr._argb.a / 255.f;
				break;
			}
			sx += (gray - ux)*(gray - ux);
	}
	
	sx=sx/size;

	for (i = 0; i < size; i++)
	{
			px_color clr = target_tex[i];
			px_float gray = 0;
			switch (channel_0R1G2B3L)
			{
			case 0:
				gray = clr._argb.r * clr._argb.a / 255.f;
				break;
			case 1:
				gray = clr._argb.g * clr._argb.a / 255.f;
				break;
			case 2:
				gray = clr._argb.b * clr._argb.a / 255.f;
				break;
			case 3:
			default:
				gray = (clr._argb.r * 0.299f + clr._argb.g * 0.587f + clr._argb.b * 0.114f) * clr._argb.a / 255.f;
				break;
			}
			sy += (gray - uy)*(gray - uy);
	}
	sy = sy / size;

	for (i = 0; i < size; i++)
	{
			px_color clr = mod_tex[i];
			px_float gray = 0;
			px_color clr2 = target_tex[i];
			px_float gray2 = 0;
			switch (channel_0R1G2B3L)
			{
			case 0:
				gray = clr._argb.r * clr._argb.a / 255.f;
				break;
			case 1:
				gray = clr._argb.g * clr._argb.a / 255.f;
				break;
			case 2:
				gray = clr._argb.b * clr._argb.a / 255.f;
				break;
			case 3:
			default:
				gray = (clr._argb.r * 0.299f + clr._argb.g * 0.587f + clr._argb.b * 0.114f) * clr._argb.a / 255.f;
				break;
			}
			
			switch (channel_0R1G2B3L)
			{
			case 0:
				gray2 = clr2._argb.r * clr2._argb.a / 255.f;
				break;
			case 1:
				gray2 = clr2._argb.g * clr2._argb.a / 255.f;
				break;
			case 2:
				gray2 = clr2._argb.b * clr2._argb.a / 255.f;
				break;
			case 3:
			default:
				gray2 = (clr2._argb.r * 0.299f + clr2._argb.g * 0.587f + clr2._argb.b * 0.114f) * clr2._argb.a / 255.f;
				break;
			}

			sxy += (gray - ux)*(gray2 - uy);
	}

	sxy = sxy / size;

	ssim = (2 * ux*uy + 6.5f)*(2 * sxy + 58.52f) / ((ux*ux + uy*uy + 6.5f)*(sx + sy + 58.52f));
	return ssim;
}

#define cov_width  8
#define cov_height  8
px_double PX_ImageFilter_MSSIM(px_texture* mod_tex, px_texture* target_tex, px_int channel_0R1G2B3L)
{
	px_int i, j;
	if (mod_tex->width> cov_width&& mod_tex->height > cov_height)
	{
		px_color kernel1[cov_width * cov_height] = { 0 };
		px_color kernel2[cov_width * cov_height] = { 0 };
		px_double ssim = 0;
		px_int count = 0;
		for (j = 0; j < mod_tex->height- cov_height; j ++)
		{
			for (i = 0; i < mod_tex->width- cov_width; i ++)
			{
				px_int y;
				for (y = 0; y < cov_height; y++)
				{
					PX_memcpy(kernel1 + y*cov_width, mod_tex->surfaceBuffer + (j + y)*mod_tex->width + i, cov_width*sizeof(px_color));
					PX_memcpy(kernel2 + y*cov_width, target_tex->surfaceBuffer + (j + y)*target_tex->width + i, cov_width*sizeof(px_color));
					ssim+= PX_ImageFilter_SSIM((px_color*)kernel1, (px_color*)kernel2, cov_width*cov_height, channel_0R1G2B3L);
					count++;
				}
				
			}
		}
		return ssim / count;
	}
	else
	{
		
		return PX_ImageFilter_SSIM(mod_tex->surfaceBuffer, target_tex->surfaceBuffer, mod_tex->width * mod_tex->height, channel_0R1G2B3L);
	}
}

px_void PX_TextureFilter_PriwittX(px_texture* ptexture, px_float out[])
{
	PX_ImageFilter_PriwittX(ptexture, out);
}
px_void PX_TextureFilter_PriwittY(px_texture* ptexture, px_float out[])
{
	PX_ImageFilter_PriwittY(ptexture, out);
}
px_void PX_TextureFilter_Priwitt(px_texture* ptexture, px_float out[])
{
	PX_ImageFilter_Priwitt(ptexture, out);
}
px_void PX_TextureFilter_SobelX(px_texture* ptexture, px_float out[])
{
	PX_ImageFilter_SobelX(ptexture, out);
}
px_void PX_TextureFilter_SobelY(px_texture* ptexture, px_float out[])
{
	PX_ImageFilter_SobelY(ptexture, out);
}
px_void PX_TextureFilter_Sobel(px_texture* ptexture, px_float out[])
{
	PX_ImageFilter_Sobel(ptexture, out);
}
px_void PX_TextureFilter_RobertsX(px_texture* ptexture, px_float out[])
{
	PX_ImageFilter_RobertsX(ptexture, out);
}
px_void PX_TextureFilter_RobertsY(px_texture* ptexture, px_float out[])
{
	PX_ImageFilter_RobertsY(ptexture, out);
}
px_void PX_TextureFilter_Roberts(px_texture* ptexture, px_float out[])
{
	PX_ImageFilter_Roberts(ptexture, out);
}
px_void PX_TextureFilter_LaplacianX(px_texture* ptexture, px_float out[])
{
	PX_ImageFilter_LaplacianX(ptexture, out);
}
px_void PX_TextureFilter_LaplacianY(px_texture* ptexture, px_float out[])
{
	PX_ImageFilter_LaplacianY(ptexture, out);
}
px_void PX_TextureFilter_Laplacian(px_texture* ptexture, px_float out[])
{
	PX_ImageFilter_Laplacian(ptexture, out);
}
px_void PX_TextureFilter_Gray(px_texture* ptexture)
{
	PX_ImageFilter_Gray(ptexture);
}
px_void PX_TextureFilter_Binarization(px_texture* ptexture, px_byte threshhold)
{
	PX_ImageFilter_Binarization(ptexture, threshhold);
}
px_void PX_TextureFilter_dBinarization(px_texture* ptexture, px_byte threshhold, px_byte dthreshhold)
{
	PX_ImageFilter_dBinarization(ptexture, threshhold, dthreshhold);
}
px_double PX_TextureFilter_MSSIM(px_texture* mod_tex, px_texture* target_tex, px_int channel_0R1G2B3L)
{
	return PX_ImageFilter_MSSIM(mod_tex, target_tex, channel_0R1G2B3L);
}