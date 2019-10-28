#include "PX_Surface.h"

px_bool PX_SurfaceCreate(px_memorypool *mp,px_uint width,px_uint height,px_surface *surface)
{
	px_void *p=MP_Malloc(mp,height*width*sizeof(px_color));
	if (p!=PX_NULL)
	{
		surface->height=height;
		surface->width=width;
		surface->surfaceBuffer=(px_color *)p;
		surface->MP=mp;
		PX_memdwordset(p,0,height*width);
		return PX_TRUE;
	}
	return PX_FALSE;
}

px_void PX_SurfaceFree(px_surface *psurface)
{
	MP_Free(psurface->MP,psurface->surfaceBuffer);
}

px_void PX_SurfaceDrawPixel(px_surface *psurface,px_int X,px_int Y,px_color COLOR)
{
	px_color c;
	if (COLOR._argb.a==0)
	{
		return;
	}
	if(X<psurface->width&&X>=0&&Y<psurface->height&&Y>=0)
	{
		if(COLOR._argb.a==0xff)
		{
		psurface->surfaceBuffer[X+psurface->width*Y]=COLOR;
		}
		else
		{
		c=psurface->surfaceBuffer[X+psurface->width*Y];
		c._argb.r=(px_uchar)(((255-COLOR._argb.a)*c._argb.r+COLOR._argb.r*COLOR._argb.a)/255);
		c._argb.g=(px_uchar)(((255-COLOR._argb.a)*c._argb.g+COLOR._argb.g*COLOR._argb.a)/255);
		c._argb.b=(px_uchar)(((255-COLOR._argb.a)*c._argb.b+COLOR._argb.b*COLOR._argb.a)/255);
		c._argb.a=255-(255-c._argb.a)*(255-COLOR._argb.a)/255;
		psurface->surfaceBuffer[X+psurface->width*Y]=c;
		}
	}
}

px_void PX_SurfaceDrawPixelFaster(px_surface *psurface,px_int X,px_int Y,px_color COLOR)
{
	px_color c;
	if (COLOR._argb.a==0)
	{
		return;
	}

#ifdef PX_DEBUG_MODE
	if(!(X<psurface->width&&X>=0&&Y<psurface->height&&Y>=0)) PX_ASSERT();
#endif

	if(COLOR._argb.a==0xff)
	{
		psurface->surfaceBuffer[X+psurface->width*Y]=COLOR;
	}
	else
	{
		c=psurface->surfaceBuffer[X+psurface->width*Y];
		c._argb.r=(unsigned char)(((255-COLOR._argb.a)*c._argb.r+COLOR._argb.r*COLOR._argb.a)/255);
		c._argb.g=(unsigned char)(((255-COLOR._argb.a)*c._argb.g+COLOR._argb.g*COLOR._argb.a)/255);
		c._argb.b=(unsigned char)(((255-COLOR._argb.a)*c._argb.b+COLOR._argb.b*COLOR._argb.a)/255);
		c._argb.a=255-(255-c._argb.a)*(255-COLOR._argb.a)/255;
		psurface->surfaceBuffer[X+psurface->width*Y]=c;
	}
}

px_void PX_SurfaceClear(px_surface *psurface, px_int left, px_int top, px_int right, px_int bottom,px_color color)
{
	px_int i,mid;

	if (left>right)
	{
		mid=left;
		left=right;
		right=mid;
	}
	if (top>bottom)
	{
		mid=top;
		top=bottom;
		bottom=mid;
	}
	if (left<0)
	{
		left=0;
	}
	if (top<0)
	{
		top=0;
	}

	if (left>psurface->width-1)
	{
		return;
	}

	if (bottom<0)
	{
		return;
	}

	if (right>psurface->width-1)
	{
		right=psurface->width-1;
	}

	if (bottom>psurface->height-1)
	{
		bottom=psurface->height-1;
	}


	for (i=top;i<=bottom;i++)
	{
		PX_memdwordset(psurface->surfaceBuffer+i*psurface->width+left,color._argb.ucolor,right-left+1);
	}
	
}
