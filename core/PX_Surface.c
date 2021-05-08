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
		surface->limit_left=0;
		surface->limit_top=0;
		surface->limit_right=width-1;
		surface->limit_bottom=height-1;
		PX_memdwordset(p,0,height*width);
		return PX_TRUE;
	}
	return PX_FALSE;
}

px_void PX_SurfaceFree(px_surface *psurface)
{
	if (psurface->surfaceBuffer==PX_NULL)
	{
		return;
	}
	MP_Free(psurface->MP,psurface->surfaceBuffer);
	psurface->surfaceBuffer=PX_NULL;
	psurface->MP=0;
	psurface->height=0;
	psurface->width=0;
}


px_int PX_SurfaceMemorySize(px_uint width,px_uint height)
{
	return width*height*sizeof(px_color);
}


PX_SurfaceLimitInfo PX_SurfaceGetLimit(px_surface *ps)
{
	PX_SurfaceLimitInfo info;
	info.limit_bottom=ps->limit_bottom;
	info.limit_left=ps->limit_left;
	info.limit_right=ps->limit_right;
	info.limit_top=ps->limit_top;
	return info;
}


px_void PX_SurfaceSetLimitInfo(px_surface *ps,PX_SurfaceLimitInfo info)
{
	PX_SurfaceSetLimit(ps,info.limit_left,info.limit_top,info.limit_right,info.limit_bottom);
}

px_void PX_SurfaceSetLimit(px_surface *ps,px_int limit_left,px_int limit_top,px_int limit_right,px_int limit_bottom)
{


	if (limit_left<0)
	{
		limit_left=0;
	}

	if (limit_top<0)
	{
		limit_top=0;
	}

	if (limit_right>ps->width-1)
	{
		limit_right=ps->width-1;
	}


	if (limit_bottom>ps->height-1)
	{
		limit_bottom=ps->height-1;
	}

	ps->limit_left=limit_left;
	ps->limit_right=limit_right;
	ps->limit_top=limit_top;
	ps->limit_bottom=limit_bottom;
}


px_void PX_SurfaceSetPixel(px_surface *psurface,px_int X,px_int Y,px_color color)
{
	if(X<=psurface->limit_right&&X>=psurface->limit_left&&Y<=psurface->limit_bottom&&Y>=psurface->limit_top)
	{
		psurface->surfaceBuffer[X+psurface->width*Y]=color;
	}
}

px_void PX_SurfaceDrawPixel(px_surface *psurface,px_int X,px_int Y,px_color COLOR)
{
	px_color c;
	if (COLOR._argb.a==0)
	{
		return;
	}
	if(X<=psurface->limit_right&&X>=psurface->limit_left&&Y<=psurface->limit_bottom&&Y>=psurface->limit_top)
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
