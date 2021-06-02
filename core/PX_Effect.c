#include "PX_Effect.h"



px_bool PX_EffectShine(px_surface *s,px_int radius,px_color color,px_float intension)
{
	px_int x,y,sampleX,sampleY;
	px_surface ns;
	px_int alpha;
	px_color bindColor;
	if(!PX_SurfaceCreate(s->MP,s->width,s->height,&ns))
		return PX_FALSE;

	for(y=0;y<s->height;y++)
	for (x=0;x<s->width;x++)
	{
		if(PX_SurfaceGetPixel(s,x,y)._argb.a==0)
		{
			alpha=0;
			for (sampleY=y-radius;sampleY<=y+radius;sampleY++)
			{
				for (sampleX=x-radius;sampleX<=x+radius;sampleX++)
				{
					if(sampleX>=0&&sampleX<s->width&&sampleY>=0&&sampleY<=s->height-1)
						alpha+=PX_SurfaceGetPixel(s,sampleX,sampleY)._argb.a;
				}
			}
			alpha/=((radius*2-1)*(radius*2-1));
			alpha=(px_int)(alpha*intension);
			alpha=alpha>255?255:alpha;
			bindColor=color;
			if(alpha!=0)
			{
			bindColor._argb.a=(px_uchar)alpha;
			PX_SurfaceSetPixel(&ns,x,y,bindColor);
			}
		}
		else
		{
			PX_SurfaceSetPixel(&ns,x,y,PX_SurfaceGetPixel(s,x,y));
		}
	}
	PX_memcpy(s->surfaceBuffer,ns.surfaceBuffer,s->width*s->height*sizeof(px_color));
	PX_SurfaceFree(&ns);
	return PX_TRUE;
}

px_bool PX_EffectOutline(px_surface *s,px_int radius,px_color color)
{
	px_int x,y,sampleX,sampleY;
	px_surface ns;
	px_int alpha;
	px_color bindColor;
	if(!PX_SurfaceCreate(s->MP,s->width,s->height,&ns))
		return PX_FALSE;

	for(y=0;y<s->height;y++)
		for (x=0;x<s->width;x++)
		{
			if(PX_SurfaceGetPixel(s,x,y)._argb.a==0)
			{
				alpha=0;
				for (sampleY=y-radius;sampleY<=y+radius;sampleY++)
				{
					for (sampleX=x-radius;sampleX<=x+radius;sampleX++)
					{
						if(sampleX>=0&&sampleX<s->width&&sampleY>=0&&sampleY<=s->height-1)
							alpha+=PX_SurfaceGetPixel(s,sampleX,sampleY)._argb.a;
					}
				}
				alpha/=((radius*2-1)*(radius-1));
				alpha=alpha>255?255:alpha;
				bindColor=color;
				if(alpha!=0)
				{
					bindColor._argb.a= (px_uchar)alpha;
					PX_SurfaceSetPixel(&ns,x,y,bindColor);
				}
			}
			else
			{
				PX_SurfaceSetPixel(&ns,x,y,PX_SurfaceGetPixel(s,x,y));
			}

		}
		PX_memcpy(s->surfaceBuffer,ns.surfaceBuffer,s->width*s->height*sizeof(px_color));
		PX_SurfaceFree(&ns);
		return PX_TRUE;
}
