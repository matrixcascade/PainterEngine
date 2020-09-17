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


