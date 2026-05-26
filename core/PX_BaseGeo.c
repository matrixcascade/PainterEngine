#include "PX_BaseGeo.h"

#define PX_ABS_FRAC(x) ((x)>0?PX_FRAC(x):PX_FRAC(-(x)))

static px_void PX_DrawLinePixel_Normal(px_surface *psurface,px_int x,px_int y,px_color color,px_int symmetry_axis)
{
	PX_SurfaceDrawPixel(psurface,x,y,color);
}

static px_void PX_DrawLinePixel_RecK(px_surface *psurface,px_int x,px_int y,px_color color,px_int symmetry_axis)
{
	PX_SurfaceDrawPixel(psurface,y,x,color);
}

static px_void PX_DrawLinePixel_InvK(px_surface *psurface,px_int x,px_int y,px_color color,px_int symmetry_axis)
{
	PX_SurfaceDrawPixel(psurface,2*symmetry_axis-x,y,color);
}

static px_void PX_DrawLinePixel_InvRecK(px_surface *psurface,px_int x,px_int y,px_color color,px_int symmetry_axis)
{
	PX_SurfaceDrawPixel(psurface,2*symmetry_axis-y,x,color);
}
px_void PX_GeoDrawLine(px_surface *psurface, px_int x0, px_int y0, px_int x1, px_int y1 ,px_int lineWidth, px_color color)
{
	px_int i,len,lm,x,y,temp1,temp2,xleft,xright;
	px_float p0x,p0y,p1x,p1y,p2x,p2y,p3x,p3y,vx,vy,rx,ry,xlen,ylen,S,ftemp,ryL,ryR,rxL,rxR;
	px_float k,recK;
	px_color clr;
	px_int symmetry_axis=0;
	px_void (*func_DrawPixel)(px_surface *psurface,px_int x,px_int y,px_color color,px_int symmetry_axis);
	
	if (lineWidth==0)
	{
		return;
	}

	//////////////////////////////////////////////////////////////////////////
	//Trim
	do 
	{
		px_int trimTop,trimRight,trimLeft,trimBottom;
		px_int vector_x,vector_y;
		px_float step;
		trimTop=0-lineWidth;
		trimBottom=psurface->height+lineWidth;
		trimLeft=0-lineWidth;
		trimRight=psurface->width+lineWidth;
		
		//////////////////////////////////////////////////////////////////////////
		//Trim top/bottom
		if (y0>y1)
		{
			//swap p0<-->p1
			lm=x0;
			x0=x1;
			x1=lm;

			lm=y0;
			y0=y1;
			y1=lm;
		} 
		//trim top
		if (y1<trimTop)
		{
			return;
		}

		vector_x=x1-x0;
		vector_y=y1-y0;

		if (vector_y!=0)
		{
			step=(trimTop-y0)*1.0f/vector_y;
			if (step>0&&step<1)
			{
				x0=(px_int)(x0+vector_x*step);
				y0=(px_int)(y0+vector_y*step);
			}
		}

		//trim bottom
		if (y0>trimBottom)
		{
			return;
		}

		vector_x=x1-x0;
		vector_y=y1-y0;

		if (vector_y!=0)
		{
			step=(trimBottom-y0)*1.0f/vector_y;
			if (step>0&&step<1)
			{
				x1=(px_int)(x0+vector_x*step);
				y1=(px_int)(y0+vector_y*step);
			}
		}

		//////////////////////////////////////////////////////////////////////////
		//Trim left/right
		if (x0>x1)
		{
			//swap p0<-->p1
			lm=x0;
			x0=x1;
			x1=lm;

			lm=y0;
			y0=y1;
			y1=lm;
		} 

		//trim left
		if (x1<trimLeft)
		{
			return;
		}

		vector_x=x1-x0;
		vector_y=y1-y0;

		if (vector_x!=0)
		{
			step=(trimLeft-x0)*1.0f/vector_x;
			if (step>0&&step<1)
			{
				x0=(px_int)(x0+vector_x*step);
				y0=(px_int)(y0+vector_y*step);
			}
		}

		//trim right
		if (x0>trimRight)
		{
			return;
		}

		vector_x=x1-x0;
		vector_y=y1-y0;

		if (vector_x!=0)
		{
			step=(trimRight-x0)*1.0f/vector_x;
			if (step>0&&step<1)
			{
				x1=(px_int)(x0+vector_x*step);
				y1=(px_int)(y0+vector_y*step);
			}
		}

	} while (0);

	//////////////////////////////////////////////////////////////////////////



	//////////////////////////////////////////////////////////////////////////
	//Draw Point
	if (x0==x1&&y0==y1)
	{
		PX_SurfaceDrawPixel(psurface,x0,y0,color);
		return;
	}


	if (x0==x1)
	{
		if (y0<0)
		{
			y0=0;
		}
		if (y1<0)
		{
			y1=0;
		}
		if (y0>psurface->height-1)
		{
			y0=psurface->height-1;
		}
		if (y1>psurface->height-1)
		{
			y1=psurface->height-1;
		}
		if (lineWidth&1)
		{
			PX_GeoDrawRect(psurface,x0-lineWidth/2,y0,x1+lineWidth/2,y1,color);
		}
		else
		{
			PX_GeoDrawRect(psurface,x0-lineWidth/2+1,y0,x1+lineWidth/2-1,y1,color);
			len=PX_ABS(y0-y1)+1;
			y0>y1?(lm=y1):(lm=y0);
			color._argb.a=(color._argb.a>>1);
			for (i=0;i<len;i++)
			{
				if (lm+i>=psurface->height)
				{
					break;
				}
				PX_SurfaceDrawPixel(psurface,x0-lineWidth/2,lm+i,color);
				PX_SurfaceDrawPixel(psurface,x0+lineWidth/2,lm+i,color);
			}
		}
		return;
	}

	if (y0==y1)
	{
		if (x0<0)
		{
			x0=0;
		}
		if (x1<0)
		{
			x1=0;
		}
		if (x0>psurface->width-1)
		{
			x0=psurface->width-1;
		}
		if (x1>psurface->width-1)
		{
			x1=psurface->width-1;
		}
		if (lineWidth&1)
		{
			PX_GeoDrawRect(psurface,x0,y0-lineWidth/2,x1,y1+lineWidth/2,color);
		}
		else
		{
			PX_GeoDrawRect(psurface,x0,y0-lineWidth/2+1,x1,y1+lineWidth/2-1,color);
			len=PX_ABS(x0-x1)+1;
			x0>x1?(lm=x1):(lm=x0);
			color._argb.a=(color._argb.a>>1);
			for (i=0;i<len;i++)
			{
				PX_SurfaceDrawPixel(psurface,lm+i,y0-lineWidth/2,color);
				PX_SurfaceDrawPixel(psurface,lm+i,y0+lineWidth/2,color);
			}
		}
		return;
	}

	

	k=((px_float)(y1-y0))/(x1-x0);
	recK=1/k;



	if (k>0)
	{
		if(k<1.f)
		{
			func_DrawPixel=PX_DrawLinePixel_Normal;
		}
		else
		{
			lm=x0;
			x0=y0;
			y0=lm;

			lm=x1;
			x1=y1;
			y1=lm;

			k=((px_float)(y1-y0))/(x1-x0);
			recK=1/k;

			func_DrawPixel=PX_DrawLinePixel_RecK;
		}
	}
	else
	{
		if (k>-1.f)
		{
			lm=x0;
			x0=x1;
			x1=lm;

			lm=y0;
			y0=y1;
			y1=lm;

			symmetry_axis=x0;
			x1=2*x0-x1;

			k=((px_float)(y1-y0))/(x1-x0);
			recK=1/k;

			func_DrawPixel=PX_DrawLinePixel_InvK;
		}
		else
		{
			lm=x0;
			x0=x1;
			x1=lm;

			lm=y0;
			y0=y1;
			y1=lm;

			symmetry_axis=x0;
			x1=2*x0-x1;

			lm=x0;
			x0=y0;
			y0=lm;

			lm=x1;
			x1=y1;
			y1=lm;

			k=((px_float)(y1-y0))/(x1-x0);
			recK=1/k;

			func_DrawPixel=PX_DrawLinePixel_InvRecK;
		}
	}
	
	vx=(lineWidth/(2*PX_sqrt(1+recK*recK)));
	vy=(lineWidth*recK/(2*PX_sqrt(1+recK*recK)));

	p0x=x0+vx+0.5f;
	p0y=y0-vy+0.5f;

	p1x=x0-vx+0.5f;
	p1y=y0+vy+0.5f;

	p2x=x1+vx+0.5f;
	p2y=y1-vy+0.5f;

	p3x=x1-vx+0.5f;
	p3y=y1+vy+0.5f;


	if (p1y<=p2y)
	{

		//p0-->p1 draw triangle
		xlen=k;

		rx=p0x-(1-PX_ABS_FRAC(p0y))*k;
		y=PX_TRUNC(p0y)+1;
		temp1=PX_TRUNC(p0x);
		temp2=PX_TRUNC(p1y);
		while (p1y>y)
		{
			//Draw full color
			for (x=PX_TRUNC(rx)+1;x<=temp1;x++)
			{
				func_DrawPixel(psurface,x,y,color,symmetry_axis);
			}
			//Draw edge point
			ftemp=PX_FRAC(rx);
			x=PX_TRUNC(rx);
			if (PX_TRUNC(rx-xlen)<PX_TRUNC(rx))
			{
				//right
				S=1-(ftemp*ftemp/2/k);
				clr=color;
				clr._argb.a=(px_uchar)(clr._argb.a*S);
				func_DrawPixel(psurface,x,y,clr,symmetry_axis);


				//left
				ftemp=1-PX_FRAC(rx-xlen);
				S=ftemp*ftemp/2/xlen;
				clr=color;
				clr._argb.a=(px_uchar)(clr._argb.a*S);
				func_DrawPixel(psurface,x-1,y,clr,symmetry_axis);

			}
			else
			{
				ftemp=1-PX_FRAC(rx);
				ftemp+=xlen/2;

				clr=color;
				clr._argb.a=(px_uchar)(clr._argb.a*ftemp);

				func_DrawPixel(psurface,x,y,clr,symmetry_axis);

			}
			rx-=xlen;
			y++;
		}
		
		ylen=xlen;
		ry=p0y+(1-PX_ABS_FRAC(p0x))*k;
		x=PX_TRUNC(p0x)+1;
		temp1=PX_TRUNC(p0x+(p1y-p0y)*recK);
		temp2=PX_TRUNC(p1y);
		while (x<=temp1)
		{
			//p0-->p2 draw triangle
			//Draw full color
			y=PX_TRUNC(ry);
			if(PX_TRUNC(ry+ylen)>PX_TRUNC(ry))
			{
				//top
				ftemp=1-PX_FRAC(ry);
				S=ftemp*ftemp/k/2;
				clr=color;
				clr._argb.a=(px_uchar)(clr._argb.a*S);
				func_DrawPixel(psurface,x,y,clr,symmetry_axis);

				if (y<temp2)
				{
					//bottom
					ftemp=PX_FRAC(ry+ylen);
					S=1-ftemp*ftemp/k/2;
					clr=color;
					clr._argb.a=(px_uchar)(clr._argb.a*S);
					func_DrawPixel(psurface,x,y+1,clr,symmetry_axis);
				}

				for (y=PX_TRUNC(ry)+2;y<=temp2;y++)
				{
					func_DrawPixel(psurface,x,y,color,symmetry_axis);
				}
				
				

			}
			else  
			{
				ftemp=1-PX_FRAC(ry+ylen);
				ftemp+=ylen/2;

				clr=color;
				clr._argb.a=(px_uchar)(clr._argb.a*ftemp);

				func_DrawPixel(psurface,x,y,clr,symmetry_axis);

				for (y=PX_TRUNC(ry)+1;y<=temp2;y++)
				{
					func_DrawPixel(psurface,x,y,color,symmetry_axis);
				}
			}
			ry+=ylen;
			x++;
			if (x>PX_TRUNC(p2x))
			{
				break;
			}
		}
		xright=x;
		ryR=ry;

		ryL=p1y+(1-PX_ABS_FRAC(p1x))*k;
		xleft=PX_TRUNC(p1x)+1;

		y=PX_TRUNC(p1y)+1;
		while(PX_TRUNC(ryL)<y) ryL+=ylen,xleft++;
		while(PX_TRUNC(ryR)<y) ryR+=ylen,xright++;
		
		if(PX_TRUNC(p2y)-PX_TRUNC(p1y)!=0)
		{
		//draw horizontal
		while (y<=PX_TRUNC(p2y))
		{
			//draw left 
			ftemp=PX_FRAC(ryL);
			S=ftemp*ftemp/2/k;
			clr=color;
			clr._argb.a=(px_uchar)(clr._argb.a*S);
			func_DrawPixel(psurface,xleft-1,y,clr,symmetry_axis);

			//draw left line
			while (PX_TRUNC(ryL)==y)
			{
				if(PX_TRUNC(ryL+ylen)==y)
				{
					S=PX_FRAC(ryL)+ylen/2;
					clr=color;
					clr._argb.a=(px_uchar)(clr._argb.a*S);
					func_DrawPixel(psurface,xleft,y,clr,symmetry_axis);
				}
				else
				{
					ftemp=1-PX_FRAC(ryL);
					S=1-ftemp*ftemp/2/k;
					clr=color;
					clr._argb.a=(px_uchar)(clr._argb.a*S);
					func_DrawPixel(psurface,xleft,y,clr,symmetry_axis);
				}
				xleft++;
				ryL+=ylen;
			}

			//draw left
			ftemp=PX_FRAC(ryR);
			S=1-ftemp*ftemp/2/k;
			clr=color;
			clr._argb.a=(px_uchar)(clr._argb.a*S);
			func_DrawPixel(psurface,xright-1,y,clr,symmetry_axis);
			temp1=xright;
			//draw right
			if(y==PX_TRUNC(p2y))
			{
				while (ryR<p2y)
				{
					if (xright>=p2x)
					{
						break;
					}

					if(PX_TRUNC(ryR+ylen)==y)
					{
						S=1-PX_FRAC(ryR)-ylen/2;
						clr=color;
						clr._argb.a=(px_uchar)(clr._argb.a*S);
						func_DrawPixel(psurface,xright,y,clr,symmetry_axis);
					}
					else
					{
						ftemp=1-PX_FRAC(ryR);
						S=ftemp*ftemp/2/k;
						clr=color;
						clr._argb.a=(px_uchar)(clr._argb.a*S);
						func_DrawPixel(psurface,xright,y,clr,symmetry_axis);
					}
					xright++;
					ryR+=ylen;
				}

			}
			else
			{
				while (PX_TRUNC(ryR)==y)
				{
					if(PX_TRUNC(ryR+ylen)==y)
					{
						S=1-PX_FRAC(ryR)-ylen/2;
						clr=color;
						clr._argb.a=(px_uchar)(clr._argb.a*S);
						func_DrawPixel(psurface,xright,y,clr,symmetry_axis);
					}
					else
					{
						ftemp=1-PX_FRAC(ryR);
						S=ftemp*ftemp/2/k;
						clr=color;
						clr._argb.a=(px_uchar)(clr._argb.a*S);
						func_DrawPixel(psurface,xright,y,clr,symmetry_axis);
					}
					xright++;
					ryR+=ylen;
				}
			}


			//draw horizontal
			for (x=xleft;x<temp1-1;x++)
			{
				func_DrawPixel(psurface,x,y,color,symmetry_axis);
			}
			y++;
			}
		}
	
		
		//Draw p3--->p1 triangle
		ry=p3y-(1-PX_ABS_FRAC(p3x))*k;
		x=PX_TRUNC(p3x)-1;
		temp2=PX_TRUNC(p2y);
		temp1=PX_TRUNC(p3x-(p3y-p2y)/k);

		while (x>temp1)
		{
			//Draw full color	
			y=PX_TRUNC(ry);
			if(PX_TRUNC(ry-ylen)!=PX_TRUNC(ry))
			{
				//bottom
				ftemp=PX_FRAC(ry);
				S=ftemp*ftemp/k/2;
				clr=color;
				clr._argb.a=(px_uchar)(clr._argb.a*S);
				func_DrawPixel(psurface,x,y,clr,symmetry_axis);

				//top
				ftemp=1-PX_FRAC(ry-ylen);
				S=1-ftemp*ftemp/k/2;
				clr=color;
				clr._argb.a=(px_uchar)(clr._argb.a*S);
				func_DrawPixel(psurface,x,y-1,clr,symmetry_axis);

				for (i=y-2;i>temp2;i--)
				{
					func_DrawPixel(psurface,x,i,color,symmetry_axis);
				}

			}
			else
			{
				ftemp=PX_FRAC(ry-ylen);
				ftemp+=ylen/2;

				clr=color;
				clr._argb.a=(px_uchar)(clr._argb.a*ftemp);

				func_DrawPixel(psurface,x,y,clr,symmetry_axis);

				for (i=y-1;i>temp2;i--)
				{
					func_DrawPixel(psurface,x,i,color,symmetry_axis);
				}
			}
			ry-=ylen;
			x--;

			if (x<PX_TRUNC(p1x))
			{
				break;
			}
		}
	
		///////////////////////////////////////////////////////////
		//p2-->p3 draw triangle
		rx=p2x-(1-PX_ABS_FRAC(p2y))*k;
		y=PX_TRUNC(p2y)+1;
		temp1=PX_TRUNC(p3x);

		//while()

		while (y<PX_TRUNC(p3y))
		{  
			//Draw full color
			for (x=PX_TRUNC(rx)-1;x>temp1-1;x--)
			{
				func_DrawPixel(psurface,x,y,color,symmetry_axis);

			}
			//Draw edge point
			ftemp=PX_FRAC(rx);
			x=PX_TRUNC(rx);
			if (PX_TRUNC(rx-xlen)<PX_TRUNC(rx))
			{
				//right
				S=(ftemp*ftemp/2/k);
				clr=color;
				clr._argb.a=(px_uchar)(clr._argb.a*S);
				func_DrawPixel(psurface,x,y,clr,symmetry_axis);


				//left
				ftemp=1-PX_FRAC(rx-xlen);
				S=1-ftemp*ftemp/2/k;
				clr=color;
				clr._argb.a=(px_uchar)(clr._argb.a*S);
				func_DrawPixel(psurface,x-1,y,clr,symmetry_axis);

			}
			else
			{
				ftemp=PX_FRAC(rx)-xlen/2;
				clr=color;
				clr._argb.a=(px_uchar)(clr._argb.a*ftemp);
				func_DrawPixel(psurface,x,y,clr,symmetry_axis);
			}
			rx-=xlen;
			y++;
		} 
		
	}
	else
	{
		////////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		ylen=k;
		ry=p0y+(1-PX_ABS_FRAC(p0x))*k;
		x=PX_TRUNC(p0x)+1;

		while (ry<p2y)
		{
			//p0-->p2 draw triangle

			//Draw full color
			temp2=PX_TRUNC(p2y);
			y=PX_TRUNC(ry);
			if(PX_TRUNC(ry+ylen)>PX_TRUNC(ry))
			{
				//top
				ftemp=1-PX_FRAC(ry);
				S=ftemp*ftemp/k/2;
				clr=color;
				clr._argb.a=(px_uchar)(clr._argb.a*S);
				func_DrawPixel(psurface,x,y,clr,symmetry_axis);

				//bottom
				if(y<temp2)
				{
				ftemp=PX_FRAC(ry+ylen);
				S=1-ftemp*ftemp/k/2;
				clr=color;
				clr._argb.a=(px_uchar)(clr._argb.a*S);
				func_DrawPixel(psurface,x,y+1,clr,symmetry_axis);
				}
				for (y=PX_TRUNC(ry)+2;y<=temp2;y++)
				{
					func_DrawPixel(psurface,x,y,color,symmetry_axis);
				}

			}
			else
			{
				ftemp=1-PX_FRAC(ry+ylen);
				ftemp+=ylen/2;

				clr=color;
				clr._argb.a=(px_uchar)(clr._argb.a*ftemp);

				func_DrawPixel(psurface,x,y,clr,symmetry_axis);

				for (y=PX_TRUNC(ry)+1;y<=temp2;y++)
				{
					func_DrawPixel(psurface,x,y,color,symmetry_axis);
				}
			}
			ry+=ylen;
			x++;

			if (x>PX_TRUNC(p2x))
			{
				break;
			}
		}
		
		//p0-->p1 draw triangle
		xlen=k;

		rx=p0x-(1-PX_ABS_FRAC(p0y))*k;
		y=PX_TRUNC(p0y)+1;
		temp1=PX_TRUNC(p0x);
		while (y<=PX_TRUNC(p2y))
		{
			//Draw full color
			for (x=PX_TRUNC(rx)+1;x<=temp1;x++)
			{
				func_DrawPixel(psurface,x,y,color,symmetry_axis);

			}
			//Draw edge point
			ftemp=PX_FRAC(rx);
			x=PX_TRUNC(rx);
			if (PX_TRUNC(rx-xlen)<PX_TRUNC(rx))
			{
				//right
				S=1-(ftemp*ftemp/2/k);
				clr=color;
				clr._argb.a=(px_uchar)(clr._argb.a*S);
				func_DrawPixel(psurface,x,y,clr,symmetry_axis);


				//left
				ftemp=1-PX_FRAC(rx-xlen);
				S=ftemp*ftemp/2/xlen;
				clr=color;
				clr._argb.a=(px_uchar)(clr._argb.a*S);
				func_DrawPixel(psurface,x-1,y,clr,symmetry_axis);

			}
			else
			{
				ftemp=1-PX_FRAC(rx);
				ftemp+=xlen/2;

				clr=color;
				clr._argb.a=(px_uchar)(clr._argb.a*ftemp);

				func_DrawPixel(psurface,x,y,clr,symmetry_axis);

			}
			rx-=xlen;
			y++;
		}
		
		
		//draw horizontal
		rxL=rx;
		rxR=p2x-(1-PX_ABS_FRAC(p2y))*k;
		y=PX_TRUNC(p2y)+1;

		if (PX_TRUNC(p1y)-PX_TRUNC(p2y)!=0)
		{
		while (y<=PX_TRUNC(p1y))
		{
			//draw left line
			xleft=PX_TRUNC(rxL);
			ftemp=PX_FRAC(rxL);
			x=PX_TRUNC(rxL);
			if (PX_TRUNC(rxL-xlen)<PX_TRUNC(rxL))
			{
				//right
				S=1-(ftemp*ftemp/2/k);
				clr=color;
				clr._argb.a=(px_uchar)(clr._argb.a*S);
				func_DrawPixel(psurface,x,y,clr,symmetry_axis);


				//left
				ftemp=1-PX_FRAC(rxL-xlen);
				S=ftemp*ftemp/2/xlen;
				clr=color;
				clr._argb.a=(px_uchar)(clr._argb.a*S);
				func_DrawPixel(psurface,x-1,y,clr,symmetry_axis);

			}
			else
			{
				ftemp=1-PX_FRAC(rxL);
				ftemp+=xlen/2;

				clr=color;
				clr._argb.a=(px_uchar)(clr._argb.a*ftemp);

				func_DrawPixel(psurface,x,y,clr,symmetry_axis);

			}
			rxL-=xlen;

			//draw right

			if(PX_TRUNC(rxR-xlen)==PX_TRUNC(rxR))
			{
				xright=PX_TRUNC(rxR);
				S=PX_FRAC(rxR)-xlen/2;
				clr=color;
				clr._argb.a=(px_uchar)(clr._argb.a*S);
				func_DrawPixel(psurface,xright,y,clr,symmetry_axis);

			}
			else
			{
				xright=PX_TRUNC(rxR);

				//draw right pixel
				ftemp=PX_FRAC(rxR);
				S=ftemp*ftemp/2/k;
				clr=color;
				clr._argb.a=(px_uchar)(clr._argb.a*S);
				func_DrawPixel(psurface,xright,y,clr,symmetry_axis);

				xright--;
				//draw left pixel
				ftemp=1-PX_FRAC(rxR-xlen);
				S=1-ftemp*ftemp/2/k;
				clr=color;
				clr._argb.a=(px_uchar)(clr._argb.a*S);
				func_DrawPixel(psurface,xright,y,clr,symmetry_axis);
			}
			rxR-=xlen;

			//draw horizontal
			for (x=xleft+1;x<xright;x++)
			{
				func_DrawPixel(psurface,x,y,color,symmetry_axis);
			}

			y++;
			}
		}
		
		//p2-->p3 draw triangle
		while (y<PX_TRUNC(p3y))
		{  
			if(PX_TRUNC(rxR-xlen)==PX_TRUNC(rxR))
			{
				xright=PX_TRUNC(rxR);
				S=PX_FRAC(rxR)-xlen/2;
				clr=color;
				clr._argb.a=(px_uchar)(clr._argb.a*S);
				func_DrawPixel(psurface,xright,y,clr,symmetry_axis);

			}
			else
			{
				xright=PX_TRUNC(rxR);

				//draw right pixel
				ftemp=PX_FRAC(rxR);
				S=ftemp*ftemp/2/k;
				clr=color;
				clr._argb.a=(px_uchar)(clr._argb.a*S);
				func_DrawPixel(psurface,xright,y,clr,symmetry_axis);

				xright--;
				//draw left pixel
				ftemp=1-PX_FRAC(rxR-xlen);
				S=1-ftemp*ftemp/2/k;
				clr=color;
				clr._argb.a=(px_uchar)(clr._argb.a*S);
				func_DrawPixel(psurface,xright,y,clr,symmetry_axis);
			}
			rxR-=xlen;

			//draw horizontal
			for (x=PX_TRUNC(p3x);x<xright;x++)
			{
				func_DrawPixel(psurface,x,y,color,symmetry_axis);
			}

			y++;
		} 

		
		
		//Draw p3--->p1 triangle
		ry=p3y-(1-PX_ABS_FRAC(p3x))*k;
		x=PX_TRUNC(p3x)-1;

		temp2=PX_TRUNC(p1y)+1;
		while (x>PX_TRUNC(p1x))
		{
			//Draw full color	
			y=PX_TRUNC(ry);
			if(PX_TRUNC(ry-ylen)!=PX_TRUNC(ry))
			{
				//bottom
				ftemp=PX_FRAC(ry);
				S=ftemp*ftemp/k/2;
				clr=color;
				clr._argb.a=(px_uchar)(clr._argb.a*S);
				func_DrawPixel(psurface,x,y,clr,symmetry_axis);

				//top
				if(y>temp2)
				{
				ftemp=1-PX_FRAC(ry-ylen);
				S=1-ftemp*ftemp/k/2;
				clr=color;
				clr._argb.a=(px_uchar)(clr._argb.a*S);
				func_DrawPixel(psurface,x,y-1,clr,symmetry_axis);
				}
				for (i=temp2;i<y-1;i++)
				{
					func_DrawPixel(psurface,x,i,color,symmetry_axis);
				}

			}
			else
			{
				ftemp=PX_FRAC(ry-ylen);
				ftemp+=ylen/2;

				clr=color;
				clr._argb.a=(px_uchar)(clr._argb.a*ftemp);

				func_DrawPixel(psurface,x,y,clr,symmetry_axis);

				for (i=temp2;i<y;i++)
				{
					func_DrawPixel(psurface,x,i,color,symmetry_axis);
				}
			}
			ry-=ylen;
			x--;

			if (x<PX_TRUNC(p1x))
			{
				break;
			}
		}
		
	}

	
}


static px_float px_clampf(px_float v, px_float lo, px_float hi)
{
	return v < lo ? lo : (v > hi ? hi : v);
}

static px_float px_dist2_point_to_segment(px_float px, px_float py,
	px_float ax, px_float ay,
	px_float bx, px_float by) {
	px_float abx = bx - ax;
	px_float aby = by - ay;
	px_float apx = px - ax;
	px_float apy = py - ay;
	px_float t, cx, cy, dx, dy;
	px_float ab_len2 = abx * abx + aby * aby;
	if (ab_len2 <= 1e-6f) {
		return apx * apx + apy * apy;
	}

	t = (apx * abx + apy * aby) / ab_len2;
	t = px_clampf(t, 0.0f, 1.0f);

	cx = ax + t * abx;
	cy = ay + t * aby;

	dx = px - cx;
	dy = py - cy;
	return dx * dx + dy * dy;
}

static px_int px_interval_linear(px_float a, px_float b, px_float L, px_float U, px_float* out_l, px_float* out_r) {
	const px_float eps = 1e-6f;
	px_float x1, x2;
	if (PX_ABS(a) < eps) {
		if (b >= L && b <= U) {
			*out_l = -1.0e30f;
			*out_r = 1.0e30f;
			return 1;
		}
		return 0;
	}

	x1 = (L - b) / a;
	x2 = (U - b) / a;
	if (x1 <= x2) {
		*out_l = x1;
		*out_r = x2;
	}
	else {
		*out_l = x2;
		*out_r = x1;
	}
	return 1;
}

static px_void px_add_interval(px_float l, px_float r, px_float* minx, px_float* maxx)
{
	if (l > r) return;
	if (l < *minx) *minx = l;
	if (r > *maxx) *maxx = r;
}

// Find x-interval of capsule (segment swept by radius R) for a fixed y.
static px_int px_capsule_xrange_for_y(px_float y,
	px_float ax, px_float ay,
	px_float bx, px_float by,
	px_float R,
	px_float* out_minx,
	px_float* out_maxx) {
	px_float vx = bx - ax;
	px_float vy = by - ay;
	px_float len = PX_sqrt(vx * vx + vy * vy);

	px_float minx = 1.0e30f;
	px_float maxx = -1.0e30f;

	if (len <= 1e-6f) {
		px_float dx, dy = y - ay;
		if (PX_ABS(dy) > R) return 0;
		dx = PX_sqrt(R * R - dy * dy);
		*out_minx = ax - dx;
		*out_maxx = ax + dx;
		return 1;
	}

	px_float ux = vx / len;
	px_float uy = vy / len;
	px_float nx = -uy;
	px_float ny = ux;

	// End cap at A
	do {
		px_float dy = y - ay;
		if (PX_ABS(dy) <= R) {
			px_float dx = PX_sqrt(R * R - dy * dy);
			px_add_interval(ax - dx, ax + dx, &minx, &maxx);
		}
	} while (0);

	// End cap at B
	do {
		px_float dy = y - by;
		if (PX_ABS(dy) <= R) {
			px_float dx = PX_sqrt(R * R - dy * dy);
			px_add_interval(bx - dx, bx + dx, &minx, &maxx);
		}
	} while (0);

	// Middle strip: |t| <= R and 0 <= s <= len
	do {
		px_float ct = ny * (y - ay) - nx * ax;
		px_float cs = uy * (y - ay) - ux * ax;

		px_float t_l, t_r, s_l, s_r;
		if (px_interval_linear(nx, ct, -R, R, &t_l, &t_r) &&
			px_interval_linear(ux, cs, 0.0f, len, &s_l, &s_r))
		{
			px_float l = (t_l > s_l) ? t_l : s_l;
			px_float r = (t_r < s_r) ? t_r : s_r;
			if (l <= r)
			{
				px_add_interval(l, r, &minx, &maxx);
			}
		}
	} while (0);

	if (minx > maxx) return 0;
	*out_minx = minx;
	*out_maxx = maxx;
	return 1;
}

// Anti-aliased round-capped line (scanline-based).
static px_void px_draw_round_line_aa(px_surface* psurface, px_float x1, px_float y1, px_float x2, px_float y2, px_float linewidth, px_byte a, px_byte r, px_byte g, px_byte b)
{
	if (linewidth <= 0.0f || a == 0) return;

	px_float R = linewidth * 0.5f;
	px_float aa = 0.5f; // 1-pixel wide smooth edge
	px_float Rout = R + aa;
	px_float Rin = R - aa;

	px_float miny = PX_MIN((px_float)y1, (px_float)y2) - Rout;
	px_float maxy = PX_MAX((px_float)y1, (px_float)y2) + Rout;

	px_int y0 = (px_int)PX_floor(miny);
	px_int y1i = (px_int)PX_ceil(maxy);

	px_float Rout2 = Rout * Rout;
	px_float Rin2 = (Rin > 0.0f) ? (Rin * Rin) : -1.0f;

	px_color base;
	base._argb.r = r;
	base._argb.g = g;
	base._argb.b = b;
	base._argb.a = a;

	for (px_int y = y0; y <= y1i; ++y) {
		px_float cy = y + 0.5f;
		px_float minx, maxx;
		if (!px_capsule_xrange_for_y(cy,
			(px_float)x1, (px_float)y1,
			(px_float)x2, (px_float)y2,
			Rout,
			&minx, &maxx)) {
			continue;
		}

		px_int x0 = (px_int)PX_ceil(minx - 0.5f);
		px_int x1b = (px_int)PX_floor(maxx - 0.5f);
		if (x0 > x1b) continue;

		for (px_int x = x0; x <= x1b; ++x) {
			px_float cx = x + 0.5f;
			px_float d2 = px_dist2_point_to_segment(cx, cy, (px_float)x1, (px_float)y1, (px_float)x2, (px_float)y2);
			px_color c;
			if (d2 > Rout2) continue;

			px_float cov;
			if (Rin2 > 0.0f && d2 <= Rin2) {
				cov = 1.0f;
			}
			else {
				px_float d = PX_sqrt(d2);
				cov = px_clampf(Rout - d, 0.0f, 1.0f);
			}

			if (cov <= 0.0f) continue;

			c = base;
			c._argb.a = (px_byte)(c._argb.a * cov + 0.5f);
			if (c._argb.a) {
				PX_SurfaceDrawPixel(psurface, x, y, c);
			}
		}
	}
}

px_void PX_GeoDrawRoundLine(px_surface* psurface, px_float x0, px_float y0, px_float x1, px_float y1, px_float lineWidth, px_color color)
{
	if (lineWidth == 0)
	{
		return;
	}

	//////////////////////////////////////////////////////////////////////////
	//Trim
	do
	{
		px_float trimTop, trimRight, trimLeft, trimBottom;
		px_float vector_x, vector_y;
		px_float step;
		trimTop = 0.f - lineWidth;
		trimBottom = psurface->height + lineWidth*1.f;
		trimLeft = 0 - lineWidth*1.f;
		trimRight = psurface->width + lineWidth*1.f;

		//////////////////////////////////////////////////////////////////////////
		//Trim top/bottom
		if (y0 > y1)
		{
			//swap p0<-->p1
			px_float lm = x0;
			x0 = x1;
			x1 = lm;

			lm = y0;
			y0 = y1;
			y1 = lm;
		}
		//trim top
		if (y1 < trimTop)
		{
			return;
		}

		vector_x = x1 - x0;
		vector_y = y1 - y0;

		if (vector_y != 0)
		{
			step = (trimTop - y0) * 1.0f / vector_y;
			if (step > 0 && step < 1)
			{
				x0 = (x0 + vector_x * step);
				y0 = (y0 + vector_y * step);
			}
		}

		//trim bottom
		if (y0 > trimBottom)
		{
			return;
		}

		vector_x = x1 - x0;
		vector_y = y1 - y0;

		if (vector_y != 0)
		{
			step = (trimBottom - y0) * 1.0f / vector_y;
			if (step > 0 && step < 1)
			{
				x1 = (x0 + vector_x * step);
				y1 = (y0 + vector_y * step);
			}
		}

		//////////////////////////////////////////////////////////////////////////
		//Trim left/right
		if (x0 > x1)
		{
			//swap p0<-->p1
			px_float lm = x0;
			x0 = x1;
			x1 = lm;

			lm = y0;
			y0 = y1;
			y1 = lm;
		}

		//trim left
		if (x1 < trimLeft)
		{
			return;
		}

		vector_x = x1 - x0;
		vector_y = y1 - y0;

		if (vector_x != 0)
		{
			step = (trimLeft - x0) * 1.0f / vector_x;
			if (step > 0 && step < 1)
			{
				x0 = (x0 + vector_x * step);
				y0 = (y0 + vector_y * step);
			}
		}

		//trim right
		if (x0 > trimRight)
		{
			return;
		}

		vector_x = x1 - x0;
		vector_y = y1 - y0;

		if (vector_x != 0)
		{
			step = (trimRight - x0) * 1.0f / vector_x;
			if (step > 0 && step < 1)
			{
				x1 = (x0 + vector_x * step);
				y1 = (y0 + vector_y * step);
			}
		}

	} while (0);

	//////////////////////////////////////////////////////////////////////////
	px_draw_round_line_aa(psurface, x0, y0, x1, y1, lineWidth, color._argb.a, color._argb.r, color._argb.g, color._argb.b);
}



px_void PX_GeoDrawBorder(px_surface *psurface, px_int left, px_int top, px_int right, px_int bottom ,px_int lineWidth,px_color color )
{
	if(lineWidth>0)
	lineWidth-=1;
	else
	return;

	

	PX_GeoDrawRect(psurface,left,top,right,top+lineWidth,color);
	PX_GeoDrawRect(psurface,left,bottom-lineWidth,right,bottom,color);
	PX_GeoDrawRect(psurface,left,top+lineWidth+1,left+lineWidth,bottom-lineWidth-1,color);
	PX_GeoDrawRect(psurface,right-lineWidth,top+lineWidth+1,right,bottom-lineWidth-1,color);
}



px_void PX_GeoDrawRect(px_surface *psurface, px_int left, px_int top, px_int right, px_int bottom,px_color color)
{
	px_int i,j,mid;
	if(color._argb.a==0)
	{
		return;
	}
	
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

	if (left<psurface->limit_left)
	{
		left=psurface->limit_left;
	}
	else if (left > psurface->limit_right)
	{
		return;
	}

	if (top<psurface->limit_top)
	{
		top=psurface->limit_top;
	}
	else if (top > psurface->limit_bottom)
	{
		return;
	}

	if (right>psurface->limit_right)
	{
		right=psurface->limit_right;
	}
	else if (right < psurface->limit_left)
	{
		return;
	}

	if (bottom>psurface->limit_bottom)
	{
		bottom=psurface->limit_bottom;
	}
	else if (bottom < psurface->limit_top)
	{
		return;
	}

	if(color._argb.a==0xff)
	{
		for (i=top;i<=bottom;i++)
		{
			PX_memdwordset(psurface->surfaceBuffer+i*psurface->width+left,color._argb.ucolor,right-left+1);
		}
	}
	else
	for (i=top;i<=bottom;i++)
	{
		for (j=left;j<=right;j++)
		{
			PX_SurfaceDrawPixel(psurface,j,i,color);
		}
	}
}

px_void PX_GeoDrawSolidCircleFast(px_surface* psurface, px_int x, px_int y, px_int Radius, px_color color)
{
	px_int _x,_y;
	px_int left, top, right, bottom;
	if (color._argb.a == 0 || Radius <= 0)
	{
		return;
	}

	left = x - Radius;
	top = y - Radius;
	right = x + Radius;
	bottom = y + Radius;


	if (left < psurface->limit_left)
	{
		left = psurface->limit_left;
	}
	else if (left > psurface->limit_right)
	{
		return;
	}

	if (top < psurface->limit_top)
	{
		top = psurface->limit_top;
	}
	else if (top > psurface->limit_bottom)
	{
		return;
	}

	if (right > psurface->limit_right)
	{
		right = psurface->limit_right;
	}
	else if (right < psurface->limit_left)
	{
		return;
	}

	if (bottom > psurface->limit_bottom)
	{
		bottom = psurface->limit_bottom;
	}
	else if (bottom < psurface->limit_top)
	{
		return;
	}

	for ( _y = top; _y <= bottom; _y++)
	{
		for (_x = left; _x <= right; _x++)
		{
			if ((_x - x) * (_x - x) + (_y - y) * (_y - y) <= Radius * Radius)
			{
				PX_SurfaceSetPixel(psurface,_x,_y,color);
			}
		}
	}
	
}


px_void PX_GeoDrawSolidCircle(px_surface* psurface, px_int x, px_int y, px_int Radius, px_color color)
{
	px_int _x, _y;
	px_int left, top, right, bottom;
	if (color._argb.a == 0 || Radius <= 0)
	{
		return;
	}

	left = x - Radius;
	top = y - Radius;
	right = x + Radius;
	bottom = y + Radius;


	if (left < psurface->limit_left)
	{
		left = psurface->limit_left;
	}
	else if (left > psurface->limit_right)
	{
		return;
	}

	if (top < psurface->limit_top)
	{
		top = psurface->limit_top;
	}
	else if (top > psurface->limit_bottom)
	{
		return;
	}

	if (right > psurface->limit_right)
	{
		right = psurface->limit_right;
	}
	else if (right < psurface->limit_left)
	{
		return;
	}

	if (bottom > psurface->limit_bottom)
	{
		bottom = psurface->limit_bottom;
	}
	else if (bottom < psurface->limit_top)
	{
		return;
	}
	
	for (_y = top; _y <= bottom; _y++)
	{
		for (_x = left; _x <= right; _x++)
		{
			px_int id = ((_y - y) * (_y - y) + (_x - x) * (_x - x));
			if (id < Radius* Radius)
			{
				px_float d = PX_sqrt((px_float)id);
				if ((px_float)Radius - d < 2)
				{
					px_color clr = color;
					clr._argb.a = (px_uchar)(clr._argb.a * ((px_float)Radius - d) / 2);
					PX_SurfaceDrawPixelWithoutLimit(psurface, _x, _y, clr);
				}
				else
				{
					PX_SurfaceDrawPixelWithoutLimit(psurface, _x, _y, color);
				}
			}
		}
	}
}
px_void PX_GeoDrawPenCircleDecay(px_surface* psurface, px_float x, px_float y, px_float Radius, px_color color, px_float Decay)
{
	px_int left, top, right, bottom, i, j;
	px_float d;
	px_color clr;
	if (color._argb.a == 0 || Radius <= 0)
	{
		return;
	}
	color._argb.a = (px_uchar)(color._argb.a/Decay);
	left = (px_int)(x - Radius);
	top = (px_int)(y - Radius);
	right = (px_int)(x + Radius);
	bottom = (px_int)(y + Radius);



	if (left < psurface->limit_left)
	{
		left = psurface->limit_left;
	}
	else if (left > psurface->limit_right)
	{
		return;
	}

	if (top < psurface->limit_top)
	{
		top = psurface->limit_top;
	}
	else if (top > psurface->limit_bottom)
	{
		return;
	}

	if (right > psurface->limit_right)
	{
		right = psurface->limit_right;
	}
	else if (right < psurface->limit_left)
	{
		return;
	}

	if (bottom > psurface->limit_bottom)
	{
		bottom = psurface->limit_bottom;
	}
	else if (bottom < psurface->limit_top)
	{
		return;
	}


	for (i = top; i <= bottom; i++)
	{
		for (j = left; j <= right; j++)
		{
			d = PX_sqrt((px_float)((i - y) * (i - y) + (j - x) * (j - x)));
			if (d < Radius)
			{
				if ((px_float)Radius - d < 2)
				{
					clr = color;
					clr._argb.a = (px_uchar)(clr._argb.a * ((px_float)Radius - d) / 2);
					PX_SurfaceDrawPixelWithoutLimit(psurface, j, i, clr);
				}
				else
				{
					PX_SurfaceDrawPixelWithoutLimit(psurface, j, i, color);
				}

			}

		}
	}
}

px_void PX_GeoDrawPenCircle(px_surface *psurface, px_float x,px_float y,px_float Radius,px_color color)
{
	PX_GeoDrawPenCircleDecay(psurface, x, y, Radius, color, 5);
}

px_void PX_GeoDrawPenRing(px_surface* psurface, px_float x, px_float y, px_float Radius,px_float linewidth_inside, px_color color)
{
	px_int left, top, right, bottom, i, j;
	px_float d;
	px_color clr;
	if (color._argb.a == 0 || Radius <= 0)
	{
		return;
	}
	
	left = (px_int)(x - Radius);
	top = (px_int)(y - Radius);
	right = (px_int)(x + Radius);
	bottom = (px_int)(y + Radius);



	if (left < psurface->limit_left)
	{
		left = psurface->limit_left;
	}
	else if (left > psurface->limit_right)
	{
		return;
	}

	if (top < psurface->limit_top)
	{
		top = psurface->limit_top;
	}
	else if (top > psurface->limit_bottom)
	{
		return;
	}

	if (right > psurface->limit_right)
	{
		right = psurface->limit_right;
	}
	else if (right < psurface->limit_left)
	{
		return;
	}

	if (bottom > psurface->limit_bottom)
	{
		bottom = psurface->limit_bottom;
	}
	else if (bottom < psurface->limit_top)
	{
		return;
	}


	for (i = top; i <= bottom; i++)
	{
		for (j = left; j <= right; j++)
		{
			d = PX_sqrt((px_float)((i - y) * (i - y) + (j - x) * (j - x)));
			if (d < Radius&&d>Radius - linewidth_inside)
			{
				
				if ((px_float)Radius - d < 2)
				{
					clr = color;
					clr._argb.a = (px_uchar)(clr._argb.a * ((px_float)Radius - d) / 2);
					PX_SurfaceDrawPixelWithoutLimit(psurface, j, i, clr);
				}
				else
				{
					if (d - Radius + linewidth_inside<2)
					{
						clr = color;
						clr._argb.a = (px_uchar)(clr._argb.a * (d - Radius + linewidth_inside) / 2);
						PX_SurfaceDrawPixelWithoutLimit(psurface, j, i, clr);
					}
					else
					{
						PX_SurfaceDrawPixelWithoutLimit(psurface, j, i, color);
					}
				}

			}

		}
	}
}

px_void PX_GeoDrawPenCircleEraser(px_surface* psurface, px_float x, px_float y, px_float Radius, px_bool white_background)
{
	px_int left, top, right, bottom, i, j;
	px_float d;

	left = (px_int)(x - Radius);
	top = (px_int)(y - Radius);
	right = (px_int)(x + Radius);
	bottom = (px_int)(y + Radius);


	if (left < psurface->limit_left)
	{
		left = psurface->limit_left;
	}
	else if (left > psurface->limit_right)
	{
		return;
	}

	if (top < psurface->limit_top)
	{
		top = psurface->limit_top;
	}
	else if (top > psurface->limit_bottom)
	{
		return;
	}

	if (right > psurface->limit_right)
	{
		right = psurface->limit_right;
	}
	else if (right < psurface->limit_left)
	{
		return;
	}

	if (bottom > psurface->limit_bottom)
	{
		bottom = psurface->limit_bottom;
	}
	else if (bottom < psurface->limit_top)
	{
		return;
	}


	for (i = top; i <= bottom; i++)
	{
		for (j = left; j <= right; j++)
		{
			d = PX_sqrt((px_float)((i - y) * (i - y) + (j - x) * (j - x)));
			if (d < Radius)
			{
				px_color clr = PX_SurfaceGetPixel(psurface, j, i);
				if ((px_float)Radius - d < 2)
				{
					px_float r, g, b;
					px_float a = ((px_float)Radius - d) / 2;
					clr._argb.a= (px_uchar)(clr._argb.a*(1-a));
					r=clr._argb.r*(1+a);
					r>255?r=255:r;
					g=clr._argb.g*(1+a);
					g>255?g=255:g;
					b=clr._argb.b*(1+a);
					b>255?b=255:b;
					clr._argb.r=(px_uchar)r;
					clr._argb.g=(px_uchar)g;
					clr._argb.b=(px_uchar)b;
					PX_SurfaceGetPixel(psurface, j, i)= clr;
				}
				else
				{
					if(white_background)
						PX_SurfaceGetPixel(psurface, j, i) = PX_COLOR(0,255,255,255);
					else
						PX_SurfaceGetPixel(psurface, j, i) = PX_COLOR(0,0,0,0);
				}

			}

		}
	}
}

px_void PX_GeoDrawSpray(px_surface* psurface, px_float x, px_float y, px_float Radius, px_color color)
{
	px_int left, top, right, bottom, i, j;
	px_float d;
	px_color clr;
	if (color._argb.a == 0 || Radius <= 0)
	{
		return;
	}

	left = (px_int)(x - Radius);
	top = (px_int)(y - Radius);
	right = (px_int)(x + Radius);
	bottom = (px_int)(y + Radius);


	if (left < psurface->limit_left)
	{
		left = psurface->limit_left;
	}
	else if (left > psurface->limit_right)
	{
		return;
	}

	if (top < psurface->limit_top)
	{
		top = psurface->limit_top;
	}
	else if (top > psurface->limit_bottom)
	{
		return;
	}

	if (right > psurface->limit_right)
	{
		right = psurface->limit_right;
	}
	else if (right < psurface->limit_left)
	{
		return;
	}

	if (bottom > psurface->limit_bottom)
	{
		bottom = psurface->limit_bottom;
	}
	else if (bottom < psurface->limit_top)
	{
		return;
	}

	for (i = top; i <= bottom; i++)
	{
		for (j = left; j <= right; j++)
		{
			d = PX_sqrt((px_float)((i - y) * (i - y) + (j - x) * (j - x)));
			if (d < Radius)
			{
				px_float a = 1-d / Radius;
				a /= 2;
				clr = color;
				clr._argb.a = (px_uchar)(clr._argb.a * a);
				PX_SurfaceDrawPixelWithoutLimit(psurface, j, i, clr);
			}

		}
	}
}

px_void PX_GeoDrawBall(px_surface* psurface, px_float x, px_float y, px_float Radius, px_color color)
{
	px_int left, top, right, bottom, i, j;
	px_float d;
	px_color clr;
	if (color._argb.a == 0)
	{
		return;
	}
	if (Radius<=0)
	{
		return;
	}
	left = (px_int)(x - Radius);
	top = (px_int)(y - Radius);
	right = (px_int)(x + Radius);
	bottom = (px_int)(y + Radius);


	if (left < psurface->limit_left)
	{
		left = psurface->limit_left;
	}
	else if (left > psurface->limit_right)
	{
		return;
	}

	if (top < psurface->limit_top)
	{
		top = psurface->limit_top;
	}
	else if (top > psurface->limit_bottom)
	{
		return;
	}

	if (right > psurface->limit_right)
	{
		right = psurface->limit_right;
	}
	else if (right < psurface->limit_left)
	{
		return;
	}

	if (bottom > psurface->limit_bottom)
	{
		bottom = psurface->limit_bottom;
	}
	else if (bottom < psurface->limit_top)
	{
		return;
	}

	for (i = top; i <= bottom; i++)
	{
		for (j = left; j <= right; j++)
		{

			d = PX_sqrt((px_float)((i - y) * (i - y) + (j - x) * (j - x)));

			if (d <= Radius)
			{
				if (d >= Radius -2)
				{
					if ((px_float)Radius - d < 1.414f)
					{
						clr = color;
						clr._argb.a = (px_uchar)(clr._argb.a * ((px_float)Radius - d) / 1.414f);
						
						PX_SurfaceDrawPixelWithoutLimit(psurface, j, i, clr);
					}
					else
						PX_SurfaceDrawPixelWithoutLimit(psurface, j, i, color);
				}
				else if (Radius*0.6f<d&&d < Radius -2)
				{
					px_float a =  0.2f+0.8f*(d- Radius*0.6f) / (Radius-2- Radius * 0.6f);
					clr = color;
					clr._argb.a = (px_uchar)(clr._argb.a * a);
					
					PX_SurfaceDrawPixelWithoutLimit(psurface, j, i, clr);
				}
				else
				{
					clr = color;
					clr._argb.a /=5;
					
					PX_SurfaceDrawPixelWithoutLimit(psurface, j, i, clr);
				}
				
			}

		}
	}
}


px_void PX_GeoDrawPath(px_surface *psurface, px_point path[],px_int pathCount,px_float linewidth,px_color color)
{
	px_int i;
	if (pathCount==1)
	{
		PX_GeoDrawPenCircle(psurface, path[0].x, path[0].y, linewidth/2,color);
	}
	else
	{
		for (i=0;i<pathCount-1;i++)
		{
			px_point path_unit_vector=PX_PointNormalization(PX_PointSub(path[i+1],path[i]));
			px_int radius=(px_int)PX_PointMod(PX_PointSub(path[i+1],path[i]));
			px_float step=0;
			do 
			{
				px_point drawPoint=PX_PointAdd(path[i],PX_PointMul(path_unit_vector,step));
				PX_GeoDrawPenCircle(psurface,drawPoint.x, drawPoint.y, linewidth/2,color);
				step+=(linewidth/8)<0.5f?0.5f:(linewidth/8);
			} while (step<radius);
		}
	}
}

px_void PX_GeoDrawPath2D(px_surface* psurface, px_point2D path[], px_int pathCount, px_float linewidth, px_color color)
{
	px_int i;
	if (pathCount == 1)
	{
		PX_GeoDrawSolidCircle(psurface, (px_int)path[0].x, (px_int)path[0].y, (px_int)linewidth / 2, color);
	}
	else
	{
		for (i = 0; i < pathCount - 1; i++)
		{
			px_point2D path_unit_vector = PX_Point2DNormalization(PX_Point2DSub(path[i + 1], path[i]));
			px_int radius = (px_int)PX_Point2DMod(PX_Point2DSub(path[i + 1], path[i]));
			px_float step = 0;
			do
			{
				px_point2D drawPoint = PX_Point2DAdd(path[i], PX_Point2DMul(path_unit_vector, step));
				PX_GeoDrawSolidCircle(psurface, (px_int)drawPoint.x, (px_int)drawPoint.y, (px_int)linewidth / 2, color);
				step += (linewidth / 4) < 0.5f ? 0.5f : (linewidth / 4);
			} while (step < radius);
		}
	}
}


px_void PX_GeoDrawPenLine(px_surface* psurface, px_float x0, px_float y0, px_float x1, px_float y1, px_float lineWidth, px_color color)
{
	px_point p[2] = {0};
	p[0].x = x0;
	p[0].y = y0;

	p[1].x = x1;
	p[1].y = y1;
	PX_GeoDrawPath(psurface, p, 2, lineWidth, color);
}

px_void PX_GeoDrawDashLine(px_surface* psurface, px_int x0, px_int y0, px_int x1, px_int y1, px_int lineWidth, px_float dash_width, px_color color)
{
	px_point2D v = PX_Point2DSub(PX_POINT2D((px_float)x1, (px_float)y1), PX_POINT2D((px_float)x0, (px_float)y0));
	px_float d = PX_Point2DMod(v);
	px_int count = (px_int)(d / dash_width);
	px_int i;
	px_float dx0, dy0,dx1,dy1;
	if (count <= 0)
	{
		return;
	}
	v = PX_Point2DNormalization(v);
	v = PX_Point2DMul(v, d/count);
	for (i=0;i<count;i++)
	{
		if ((i&1)==0)
		{
			dx0 = x0 + v.x * i;
			dy0 = y0 + v.y * i;

			dx1 = x0 + v.x * (i+1);
			dy1 = y0 + v.y * (i+1);

			PX_GeoDrawLine(psurface, (px_int)dx0, (px_int)dy0, (px_int)dx1, (px_int)dy1, lineWidth, color);
		}
	}
}

px_void PX_GeoDrawPenDots(px_surface* psurface, px_point _samples[], px_int samplescount, px_float radius, px_color color)
{
	px_int i;
	for (i = 0; i < samplescount; i++)
	{
		PX_GeoDrawPenCircle(psurface, _samples[i].x, _samples[i].y, radius, color);
	}
}


px_void PX_GeoDrawCircle(px_surface *psurface, px_int x,px_int y,px_int Radius, px_int lineWidth,px_color color )
{
	px_int rx,ry,dy,i,xleft,xright,Sy,cY,drx,dry;
	px_float rad,rad2,xoft1,xoft2,xoft3,xoft4,S,fy,fdis;
	px_color clr;

	if (Radius==0)
	{
		return;
	}
	
	//Draw cross
	if (lineWidth>Radius*2)
	{
		lineWidth=Radius*2;
	}

	if (x>=0-Radius&&y>=0-Radius&&x<psurface->width+Radius&&y<psurface->height+Radius)
	{
		i=x*x+y*y;

		if (i<(x-psurface->width)*(x-psurface->width)+y*y)
		{
			i=(x-psurface->width)*(x-psurface->width)+y*y;
		}

		if (i<(x-psurface->width)*(x-psurface->width)+(y-psurface->height)*(y-psurface->height))
		{
			i=(x-psurface->width)*(x-psurface->width)+(y-psurface->height)*(y-psurface->height);
		}

		if (i<x*x+(y-psurface->height)*(y-psurface->height))
		{
			i=x*x+(y-psurface->height)*(y-psurface->height);
		}

		if (i<(Radius-lineWidth/2)*(Radius-lineWidth/2)&&!PX_isPointInRect(PX_POINT((px_float)x,(px_float)y,0),PX_RECT(0,0,(px_float)psurface->width,(px_float)psurface->height)))
		{
			return;
		}
	}
	else
	{
		return;
	}


	
	
	

	if (lineWidth&1)
	{
		ry=Radius+(lineWidth>>1);
		for (rx=Radius-(lineWidth>>1);rx<=ry;rx++)
		{
			PX_SurfaceDrawPixel(psurface,x+rx,y,color);
			PX_SurfaceDrawPixel(psurface,x-rx,y,color);
			PX_SurfaceDrawPixel(psurface,x,rx+y,color);
			PX_SurfaceDrawPixel(psurface,x,-rx+y,color);
		}
	}
	else
	{
		clr=color;
		clr._argb.a>>=1;
		dy=Radius+(lineWidth>>1);
		PX_SurfaceDrawPixel(psurface,x,y+dy,clr);
		PX_SurfaceDrawPixel(psurface,x,y-dy,clr);
		PX_SurfaceDrawPixel(psurface,x+dy,y,clr);
		PX_SurfaceDrawPixel(psurface,x-dy,y,clr);

		ry=Radius-(lineWidth>>1);
		PX_SurfaceDrawPixel(psurface,x,y+ry,clr);
		PX_SurfaceDrawPixel(psurface,x,y-ry,clr);
		PX_SurfaceDrawPixel(psurface,x+ry,y,clr);
		PX_SurfaceDrawPixel(psurface,x-ry,y,clr);

		for (i=ry+1;i<dy;i++)
		{
			PX_SurfaceDrawPixel(psurface,x,y+i,color);
			PX_SurfaceDrawPixel(psurface,x,y-i,color);
			PX_SurfaceDrawPixel(psurface,x+i,y,color);
			PX_SurfaceDrawPixel(psurface,x-i,y,color);
		}
	}
	// Draw 45-degree cross
	//inside
	rad=Radius-lineWidth/2.0f;
	ry=PX_TRUNC(rad/1.4142135623731f+0.5f);
	fy=rad/1.4142135623731f+0.5f;
	if (PX_FRAC(fy)>0.5f)
	{
		fdis=1-PX_FRAC(fy);
		S=fdis*fdis*2;
		clr=color;
		clr._argb.a=(px_uchar)(clr._argb.a*S);
		PX_SurfaceDrawPixel(psurface,x+ry,y+ry,clr);
		PX_SurfaceDrawPixel(psurface,x-ry,y+ry,clr);
		PX_SurfaceDrawPixel(psurface,x+ry,y-ry,clr);
		PX_SurfaceDrawPixel(psurface,x-ry,y-ry,clr);
	}
	else
	{
		fdis=PX_FRAC(fy);
		S=1-fdis*fdis*2;
		clr=color;
		clr._argb.a=(px_uchar)(clr._argb.a*S);
		PX_SurfaceDrawPixel(psurface,x+ry,y+ry,clr);
		PX_SurfaceDrawPixel(psurface,x-ry,y+ry,clr);
		PX_SurfaceDrawPixel(psurface,x+ry,y-ry,clr);
		PX_SurfaceDrawPixel(psurface,x-ry,y-ry,clr);
	}
	
	
	//out side
	rad2=Radius+lineWidth/2.0f;
	dy=PX_TRUNC(rad2/1.4142135623731f+0.5f);
	fy=rad2/1.4142135623731f+0.5f;
	if (PX_FRAC(fy)>0.5f)
	{
		fdis=1-PX_FRAC(fy);
		S=1-fdis*fdis*2;
		clr=color;
		clr._argb.a=(px_uchar)(clr._argb.a*S);
		PX_SurfaceDrawPixel(psurface,x+dy,y+dy,clr);
		PX_SurfaceDrawPixel(psurface,x-dy,y+dy,clr);
		PX_SurfaceDrawPixel(psurface,x+dy,y-dy,clr);
		PX_SurfaceDrawPixel(psurface,x-dy,y-dy,clr);
	}
	else
	{
		fdis=PX_FRAC(fy);
		S=fdis*fdis*2;
		clr=color;
		clr._argb.a=(px_uchar)(clr._argb.a*S);
		PX_SurfaceDrawPixel(psurface,x+dy,y+dy,clr);
		PX_SurfaceDrawPixel(psurface,x-dy,y+dy,clr);
		PX_SurfaceDrawPixel(psurface,x+dy,y-dy,clr);
		PX_SurfaceDrawPixel(psurface,x-dy,y-dy,clr);
	}

	
	for (i=ry+1;i<dy;i++)
	{
		PX_SurfaceDrawPixel(psurface,x+i,y+i,color);
		PX_SurfaceDrawPixel(psurface,x-i,y+i,color);
		PX_SurfaceDrawPixel(psurface,x+i,y-i,color);
		PX_SurfaceDrawPixel(psurface,x-i,y-i,color);
	}

	Sy=1;

	fy=(px_float)Sy-0.5f;
	xoft2=PX_sqrt(rad*rad-fy*fy)+0.5f;
	xoft4=PX_sqrt(rad2*rad2-fy*fy)+0.5f;
	while (Sy<dy)
	{
		cY=ry;
		//inside
		//////////////////////////////////////////////////////////////////////////
		if(Sy<cY)
		{
			xoft1=xoft2;
			fy=(px_float)Sy+0.5f;
			xoft2=PX_sqrt(rad*rad-fy*fy)+0.5f;


			if (PX_TRUNC(xoft1)!=PX_TRUNC(xoft2))
			{
				//draw right
				fdis=xoft1-xoft2;
				S=PX_FRAC(xoft1)/fdis;
				S*=S;
				S*=(fdis/2);
				S=1-S;
				clr=color;
				clr._argb.a=(px_uchar)(clr._argb.a*S);

				drx=PX_TRUNC(xoft1);
				dry=Sy;

				PX_SurfaceDrawPixel(psurface,x+drx,y+dry,clr);
				PX_SurfaceDrawPixel(psurface,x+drx,y-dry,clr);
				PX_SurfaceDrawPixel(psurface,x-drx,y+dry,clr);
				PX_SurfaceDrawPixel(psurface,x-drx,y-dry,clr);

				PX_SurfaceDrawPixel(psurface,x+dry,y+drx,clr);
				PX_SurfaceDrawPixel(psurface,x+dry,y-drx,clr);
				PX_SurfaceDrawPixel(psurface,x-dry,y+drx,clr);
				PX_SurfaceDrawPixel(psurface,x-dry,y-drx,clr);

				//draw left
				S=(1-PX_FRAC(xoft2))/fdis;
				S*=S;
				S*=(fdis/2);
				clr=color;
				clr._argb.a=(px_uchar)(clr._argb.a*S);

				drx=PX_TRUNC(xoft2);
				dry=Sy;

				PX_SurfaceDrawPixel(psurface,x+drx,y+dry,clr);
				PX_SurfaceDrawPixel(psurface,x+drx,y-dry,clr);
				PX_SurfaceDrawPixel(psurface,x-drx,y+dry,clr);
				PX_SurfaceDrawPixel(psurface,x-drx,y-dry,clr);

				PX_SurfaceDrawPixel(psurface,x+dry,y+drx,clr);
				PX_SurfaceDrawPixel(psurface,x+dry,y-drx,clr);
				PX_SurfaceDrawPixel(psurface,x-dry,y+drx,clr);
				PX_SurfaceDrawPixel(psurface,x-dry,y-drx,clr);
			}
			else
			{
				fdis=xoft1-xoft2;
				S=PX_FRAC(xoft2)+fdis/2;
				S=1-S;

				clr=color;
				clr._argb.a=(px_uchar)(clr._argb.a*S);

				drx=PX_TRUNC(xoft1);
				dry=Sy;

				PX_SurfaceDrawPixel(psurface,x+drx,y+dry,clr);
				PX_SurfaceDrawPixel(psurface,x+drx,y-dry,clr);
				PX_SurfaceDrawPixel(psurface,x-drx,y+dry,clr);
				PX_SurfaceDrawPixel(psurface,x-drx,y-dry,clr);

				PX_SurfaceDrawPixel(psurface,x+dry,y+drx,clr);
				PX_SurfaceDrawPixel(psurface,x+dry,y-drx,clr);
				PX_SurfaceDrawPixel(psurface,x-dry,y+drx,clr);
				PX_SurfaceDrawPixel(psurface,x-dry,y-drx,clr);
			}
			xleft=PX_TRUNC(xoft1);
			
		}
		else
		{
			xleft=Sy;
		}
		//////////////////////////////////////////////////////////////////////////
		
		//outside
		//////////////////////////////////////////////////////////////////////////
			xoft3=xoft4;
			fy=(px_float)Sy+0.5f;
			xoft4=PX_sqrt(rad2*rad2-fy*fy)+0.5f;


			if (PX_TRUNC(xoft3)!=PX_TRUNC(xoft4))
			{
				//draw right
				fdis=xoft3-xoft4;
				S=PX_FRAC(xoft3)/fdis;
				S*=S;
				S*=(fdis/2);
				clr=color;
				clr._argb.a=(px_uchar)(clr._argb.a*S);

				drx=PX_TRUNC(xoft3);
				dry=Sy;

				PX_SurfaceDrawPixel(psurface,x+drx,y+dry,clr);
				PX_SurfaceDrawPixel(psurface,x+drx,y-dry,clr);
				PX_SurfaceDrawPixel(psurface,x-drx,y+dry,clr);
				PX_SurfaceDrawPixel(psurface,x-drx,y-dry,clr);

				PX_SurfaceDrawPixel(psurface,x+dry,y+drx,clr);
				PX_SurfaceDrawPixel(psurface,x+dry,y-drx,clr);
				PX_SurfaceDrawPixel(psurface,x-dry,y+drx,clr);
				PX_SurfaceDrawPixel(psurface,x-dry,y-drx,clr);
				//draw left
				S=(1-PX_FRAC(xoft4))/fdis;
				S*=S;
				S*=(fdis/2);
				S=1-S;
				clr=color;
				clr._argb.a=(px_uchar)(clr._argb.a*S);

				drx=PX_TRUNC(xoft4);
				dry=Sy;

				PX_SurfaceDrawPixel(psurface,x+drx,y+dry,clr);
				PX_SurfaceDrawPixel(psurface,x+drx,y-dry,clr);
				PX_SurfaceDrawPixel(psurface,x-drx,y+dry,clr);
				PX_SurfaceDrawPixel(psurface,x-drx,y-dry,clr);

				PX_SurfaceDrawPixel(psurface,x+dry,y+drx,clr);
				PX_SurfaceDrawPixel(psurface,x+dry,y-drx,clr);
				PX_SurfaceDrawPixel(psurface,x-dry,y+drx,clr);
				PX_SurfaceDrawPixel(psurface,x-dry,y-drx,clr);
			}
			else
			{
				fdis=xoft3-xoft4;
				S=PX_FRAC(xoft4)+fdis/2;
				clr=color;
				clr._argb.a=(px_uchar)(clr._argb.a*S);
				drx=PX_TRUNC(xoft3);
				dry=Sy;

				PX_SurfaceDrawPixel(psurface,x+drx,y+dry,clr);
				PX_SurfaceDrawPixel(psurface,x+drx,y-dry,clr);
				PX_SurfaceDrawPixel(psurface,x-drx,y+dry,clr);
				PX_SurfaceDrawPixel(psurface,x-drx,y-dry,clr);

				PX_SurfaceDrawPixel(psurface,x+dry,y+drx,clr);
				PX_SurfaceDrawPixel(psurface,x+dry,y-drx,clr);
				PX_SurfaceDrawPixel(psurface,x-dry,y+drx,clr);
				PX_SurfaceDrawPixel(psurface,x-dry,y-drx,clr);
			}
			xright=PX_TRUNC(xoft4);

		//////////////////////////////////////////////////////////////////////////

			for (i=xleft+1;i<xright;i++)
			{
				PX_SurfaceDrawPixel(psurface,x+i,y+Sy,color);
				PX_SurfaceDrawPixel(psurface,x+i,y-Sy,color);
				PX_SurfaceDrawPixel(psurface,x-i,y+Sy,color);
				PX_SurfaceDrawPixel(psurface,x-i,y-Sy,color);

				PX_SurfaceDrawPixel(psurface,x+Sy,y+i,color);
				PX_SurfaceDrawPixel(psurface,x+Sy,y-i,color);
				PX_SurfaceDrawPixel(psurface,x-Sy,y+i,color);
				PX_SurfaceDrawPixel(psurface,x-Sy,y-i,color);
			}
		Sy++;
	}


}

static px_void PX_GeoDrawRingPoint(px_surface *psurface, px_int x,px_int y,px_color color, px_int cx,px_int cy,px_float s_r,px_float e_r)
{
	px_int vx=x-cx,vy=y-cy;
	px_float recR=PX_SqrtRec((px_float)(vx*vx+vy*vy));
	px_float math_region=recR*vy;

	if (vx>0)
	{
		if (vy<0)
		{
			math_region+=4;
		}
	}
	else
		math_region=2-math_region;

	if ((math_region>s_r&&math_region<e_r)||(math_region+4<e_r))
	{
		PX_SurfaceDrawPixel(psurface,x,y,color);
	}
}

px_void PX_GeoDrawRing(px_surface *psurface, px_int x,px_int y, px_float Radius,px_float lineWidth,px_color color,px_int start_angle,px_int end_angle)
{
	if (lineWidth<=0)
	{
		return;
	}
	PX_GeoDrawSector(psurface, x, y, Radius + lineWidth / 2.0f, Radius - lineWidth / 2.0f, color, start_angle, end_angle);
}

px_void PX_GeoDrawSector(px_surface *psurface, px_int x,px_int y,px_float Radius_outside, px_float Radius_inside,px_color color,px_int start_angle,px_int end_angle)
{
	px_int s_quadrant,e_quadrant,trim;
	px_float start_mathRegion,end_mathRegion;
	px_int rx,ry,dy,i,xleft,xright,Sy,cY,drx,dry;
	px_float rad,rad2,xoft1,xoft2,xoft3,xoft4,S,fy,fdis;
	px_color clr;
	px_int repeat=0;
	

	if (Radius_outside<Radius_inside)
	{
		px_float temp=Radius_outside;
		Radius_outside=Radius_inside;
		Radius_inside=temp;
	}

	if(color._argb.a==0)
	{
		return;
	}


	if (Radius_outside==0)
	{
		return;
	}

	if (start_angle>end_angle)
	{
		px_int temp=start_angle;
		start_angle=end_angle;
		end_angle=temp;
	}
	end_angle++;

	if (start_angle<0)
	{
		repeat=start_angle/360;
		repeat-=1;
		start_angle-=repeat*360;
		end_angle-=repeat*360;
	}

	repeat=start_angle/360;
	start_angle-=repeat*360;
	end_angle-=repeat*360;


	s_quadrant=(start_angle%360)/90+1;
	e_quadrant=(end_angle%360)/90+1;

	start_mathRegion=PX_sin_angle((px_float)start_angle);
	end_mathRegion=PX_sin_angle((px_float)end_angle);

	switch (s_quadrant)
	{
	case 1:
		break;
	case 2:
	case 3:
		start_mathRegion=2-start_mathRegion;
		break;
	case 4:
		start_mathRegion+=4;
		break;
	default:
		PX_ASSERT();
	}
	start_mathRegion+=4*(start_angle/360);

	switch (e_quadrant)
	{
	case 1:
		break;
	case 2:
	case 3:
		end_mathRegion=2-end_mathRegion;
		break;
	case 4:
		end_mathRegion+=4;
		break;
	default:
		PX_ASSERT();
	}
	end_mathRegion+=4*(end_angle/360);

	if (end_mathRegion<start_mathRegion)
	{
		px_float temp;
		temp=end_mathRegion;
		end_mathRegion=start_mathRegion;
		start_mathRegion=temp;
	}
	//trim
	trim=(px_int)(start_mathRegion/4);
	start_mathRegion-=trim;
	end_mathRegion-=trim;

	//Draw cross

	if (x>=0-Radius_outside&&y>=0-Radius_outside&&x<psurface->width+Radius_outside&&y<psurface->height+Radius_outside)
	{
		i=x*x+y*y;

		if (i<(x-psurface->width)*(x-psurface->width)+y*y)
		{
			i=(x-psurface->width)*(x-psurface->width)+y*y;
		}

		if (i<(x-psurface->width)*(x-psurface->width)+(y-psurface->height)*(y-psurface->height))
		{
			i=(x-psurface->width)*(x-psurface->width)+(y-psurface->height)*(y-psurface->height);
		}

		if (i<x*x+(y-psurface->height)*(y-psurface->height))
		{
			i=x*x+(y-psurface->height)*(y-psurface->height);
		}

		if (i<Radius_outside*Radius_outside&&!PX_isPointInRect(PX_POINT((px_float)x,(px_float)y,0),PX_RECT(0,0,(px_float)psurface->width,(px_float)psurface->height)))
		{
			return;
		}
	}
	else
	{
		return;
	}

	
	
	do 
	{
		px_color clr = color;
		clr._argb.a >>= 1;

		rx = (px_int)(Radius_inside+0.5f);
		PX_GeoDrawRingPoint(psurface, x + rx, y, clr, x, y, start_mathRegion, end_mathRegion);
		PX_GeoDrawRingPoint(psurface, x - rx, y, clr, x, y, start_mathRegion, end_mathRegion);
		PX_GeoDrawRingPoint(psurface, x, rx + y, clr, x, y, start_mathRegion, end_mathRegion);
		PX_GeoDrawRingPoint(psurface, x, -rx + y, clr, x, y, start_mathRegion, end_mathRegion);

		rx = (px_int)(Radius_outside);
		PX_GeoDrawRingPoint(psurface, x + rx, y, clr, x, y, start_mathRegion, end_mathRegion);
		PX_GeoDrawRingPoint(psurface, x - rx, y, clr, x, y, start_mathRegion, end_mathRegion);
		PX_GeoDrawRingPoint(psurface, x, rx + y, clr, x, y, start_mathRegion, end_mathRegion);
		PX_GeoDrawRingPoint(psurface, x, -rx + y, clr, x, y, start_mathRegion, end_mathRegion);
	} while (0);
	
	ry = (px_int)(Radius_outside+0.5f);
	for (rx= (px_int)Radius_inside+1;rx<=ry-1;rx++)
	{
		PX_GeoDrawRingPoint(psurface,x+rx,y,color,x,y,start_mathRegion,end_mathRegion);
		PX_GeoDrawRingPoint(psurface,x-rx,y,color,x,y,start_mathRegion,end_mathRegion);
		PX_GeoDrawRingPoint(psurface,x,rx+y,color,x,y,start_mathRegion,end_mathRegion);
		PX_GeoDrawRingPoint(psurface,x,-rx+y,color,x,y,start_mathRegion,end_mathRegion);
	}

	// Draw 45-degree cross
	//inside
	rad=(px_float)Radius_inside;
	ry=PX_TRUNC(rad/1.4142135623731f+0.5f);
	fy=rad/1.4142135623731f+0.5f;
	if (PX_FRAC(fy)>0.5f)
	{
		fdis=1-PX_FRAC(fy);
		S=fdis*fdis*2;
		clr=color;
		clr._argb.a=(px_uchar)(clr._argb.a*S);
		PX_GeoDrawRingPoint(psurface,x+ry,y+ry,clr,x,y,start_mathRegion,end_mathRegion);
		PX_GeoDrawRingPoint(psurface,x-ry,y+ry,clr,x,y,start_mathRegion,end_mathRegion);
		PX_GeoDrawRingPoint(psurface,x+ry,y-ry,clr,x,y,start_mathRegion,end_mathRegion);
		PX_GeoDrawRingPoint(psurface,x-ry,y-ry,clr,x,y,start_mathRegion,end_mathRegion);
	}
	else
	{
		fdis=PX_FRAC(fy);
		S=1-fdis*fdis*2;
		clr=color;
		clr._argb.a=(px_uchar)(clr._argb.a*S);
		PX_GeoDrawRingPoint(psurface,x+ry,y+ry,clr,x,y,start_mathRegion,end_mathRegion);
		PX_GeoDrawRingPoint(psurface,x-ry,y+ry,clr,x,y,start_mathRegion,end_mathRegion);
		PX_GeoDrawRingPoint(psurface,x+ry,y-ry,clr,x,y,start_mathRegion,end_mathRegion);
		PX_GeoDrawRingPoint(psurface,x-ry,y-ry,clr,x,y,start_mathRegion,end_mathRegion);
	}


	//outside
	rad2=(px_float)Radius_outside;
	dy=PX_TRUNC(rad2/1.4142135623731f+0.5f);
	fy=rad2/1.4142135623731f+0.5f;
	if (PX_FRAC(fy)>0.5f)
	{
		fdis=1-PX_FRAC(fy);
		S=1-fdis*fdis*2;
		clr=color;
		clr._argb.a=(px_uchar)(clr._argb.a*S);
		PX_GeoDrawRingPoint(psurface,x+dy,y+dy,clr,x,y,start_mathRegion,end_mathRegion);
		PX_GeoDrawRingPoint(psurface,x-dy,y+dy,clr,x,y,start_mathRegion,end_mathRegion);
		PX_GeoDrawRingPoint(psurface,x+dy,y-dy,clr,x,y,start_mathRegion,end_mathRegion);
		PX_GeoDrawRingPoint(psurface,x-dy,y-dy,clr,x,y,start_mathRegion,end_mathRegion);
	}
	else
	{
		fdis=PX_FRAC(fy);
		S=fdis*fdis*2;
		clr=color;
		clr._argb.a=(px_uchar)(clr._argb.a*S);
		PX_GeoDrawRingPoint(psurface,x+dy,y+dy,clr,x,y,start_mathRegion,end_mathRegion);
		PX_GeoDrawRingPoint(psurface,x-dy,y+dy,clr,x,y,start_mathRegion,end_mathRegion);
		PX_GeoDrawRingPoint(psurface,x+dy,y-dy,clr,x,y,start_mathRegion,end_mathRegion);
		PX_GeoDrawRingPoint(psurface,x-dy,y-dy,clr,x,y,start_mathRegion,end_mathRegion);
	}


	for (i=ry+1;i<dy;i++)
	{
		PX_GeoDrawRingPoint(psurface,x+i,y+i,color,x,y,start_mathRegion,end_mathRegion);
		PX_GeoDrawRingPoint(psurface,x-i,y+i,color,x,y,start_mathRegion,end_mathRegion);
		PX_GeoDrawRingPoint(psurface,x+i,y-i,color,x,y,start_mathRegion,end_mathRegion);
		PX_GeoDrawRingPoint(psurface,x-i,y-i,color,x,y,start_mathRegion,end_mathRegion);
	}

	Sy=1;

	fy=(px_float)Sy-0.5f;
	xoft2=PX_sqrt(rad*rad-fy*fy)+0.5f;
	xoft4=PX_sqrt(rad2*rad2-fy*fy)+0.5f;
	while (Sy<dy)
	{
		cY=ry;
		//inside
		//////////////////////////////////////////////////////////////////////////
		if(Sy<cY)
		{
			xoft1=xoft2;
			fy=(px_float)Sy+0.5f;
			xoft2=PX_sqrt(rad*rad-fy*fy)+0.5f;


			if (PX_TRUNC(xoft1)!=PX_TRUNC(xoft2))
			{
				//draw right
				fdis=xoft1-xoft2;
				S=PX_FRAC(xoft1)/fdis;
				S*=S;
				S*=(fdis/2);
				S=1-S;
				clr=color;
				clr._argb.a=(px_uchar)(clr._argb.a*S);

				drx=PX_TRUNC(xoft1);
				dry=Sy;

				PX_GeoDrawRingPoint(psurface,x+drx,y+dry,clr,x,y,start_mathRegion,end_mathRegion);
				PX_GeoDrawRingPoint(psurface,x+drx,y-dry,clr,x,y,start_mathRegion,end_mathRegion);
				PX_GeoDrawRingPoint(psurface,x-drx,y+dry,clr,x,y,start_mathRegion,end_mathRegion);
				PX_GeoDrawRingPoint(psurface,x-drx,y-dry,clr,x,y,start_mathRegion,end_mathRegion);

				PX_GeoDrawRingPoint(psurface,x+dry,y+drx,clr,x,y,start_mathRegion,end_mathRegion);
				PX_GeoDrawRingPoint(psurface,x+dry,y-drx,clr,x,y,start_mathRegion,end_mathRegion);
				PX_GeoDrawRingPoint(psurface,x-dry,y+drx,clr,x,y,start_mathRegion,end_mathRegion);
				PX_GeoDrawRingPoint(psurface,x-dry,y-drx,clr,x,y,start_mathRegion,end_mathRegion);

				//draw left
				S=(1-PX_FRAC(xoft2))/fdis;
				S*=S;
				S*=(fdis/2);
				clr=color;
				clr._argb.a=(px_uchar)(clr._argb.a*S);

				drx=PX_TRUNC(xoft2);
				dry=Sy;

				PX_GeoDrawRingPoint(psurface,x+drx,y+dry,clr,x,y,start_mathRegion,end_mathRegion);
				PX_GeoDrawRingPoint(psurface,x+drx,y-dry,clr,x,y,start_mathRegion,end_mathRegion);
				PX_GeoDrawRingPoint(psurface,x-drx,y+dry,clr,x,y,start_mathRegion,end_mathRegion);
				PX_GeoDrawRingPoint(psurface,x-drx,y-dry,clr,x,y,start_mathRegion,end_mathRegion);

				PX_GeoDrawRingPoint(psurface,x+dry,y+drx,clr,x,y,start_mathRegion,end_mathRegion);
				PX_GeoDrawRingPoint(psurface,x+dry,y-drx,clr,x,y,start_mathRegion,end_mathRegion);
				PX_GeoDrawRingPoint(psurface,x-dry,y+drx,clr,x,y,start_mathRegion,end_mathRegion);
				PX_GeoDrawRingPoint(psurface,x-dry,y-drx,clr,x,y,start_mathRegion,end_mathRegion);
			}
			else
			{
				fdis=xoft1-xoft2;
				S=PX_FRAC(xoft2)+fdis/2;
				S=1-S;

				clr=color;
				clr._argb.a=(px_uchar)(clr._argb.a*S);

				drx=PX_TRUNC(xoft1);
				dry=Sy;

				PX_GeoDrawRingPoint(psurface,x+drx,y+dry,clr,x,y,start_mathRegion,end_mathRegion);
				PX_GeoDrawRingPoint(psurface,x+drx,y-dry,clr,x,y,start_mathRegion,end_mathRegion);
				PX_GeoDrawRingPoint(psurface,x-drx,y+dry,clr,x,y,start_mathRegion,end_mathRegion);
				PX_GeoDrawRingPoint(psurface,x-drx,y-dry,clr,x,y,start_mathRegion,end_mathRegion);

				PX_GeoDrawRingPoint(psurface,x+dry,y+drx,clr,x,y,start_mathRegion,end_mathRegion);
				PX_GeoDrawRingPoint(psurface,x+dry,y-drx,clr,x,y,start_mathRegion,end_mathRegion);
				PX_GeoDrawRingPoint(psurface,x-dry,y+drx,clr,x,y,start_mathRegion,end_mathRegion);
				PX_GeoDrawRingPoint(psurface,x-dry,y-drx,clr,x,y,start_mathRegion,end_mathRegion);
			}
			xleft=PX_TRUNC(xoft1);

		}
		else
		{
			xleft=Sy;
		}
		//////////////////////////////////////////////////////////////////////////

		//outside
		//////////////////////////////////////////////////////////////////////////
		xoft3=xoft4;
		fy=(px_float)Sy+0.5f;
		xoft4=PX_sqrt(rad2*rad2-fy*fy)+0.5f;


		if (PX_TRUNC(xoft3)!=PX_TRUNC(xoft4))
		{
			//draw right
			fdis=xoft3-xoft4;
			S=PX_FRAC(xoft3)/fdis;
			S*=S;
			S*=(fdis/2);
			clr=color;
			clr._argb.a=(px_uchar)(clr._argb.a*S);

			drx=PX_TRUNC(xoft3);
			dry=Sy;

			PX_GeoDrawRingPoint(psurface,x+drx,y+dry,clr,x,y,start_mathRegion,end_mathRegion);
			PX_GeoDrawRingPoint(psurface,x+drx,y-dry,clr,x,y,start_mathRegion,end_mathRegion);
			PX_GeoDrawRingPoint(psurface,x-drx,y+dry,clr,x,y,start_mathRegion,end_mathRegion);
			PX_GeoDrawRingPoint(psurface,x-drx,y-dry,clr,x,y,start_mathRegion,end_mathRegion);

			PX_GeoDrawRingPoint(psurface,x+dry,y+drx,clr,x,y,start_mathRegion,end_mathRegion);
			PX_GeoDrawRingPoint(psurface,x+dry,y-drx,clr,x,y,start_mathRegion,end_mathRegion);
			PX_GeoDrawRingPoint(psurface,x-dry,y+drx,clr,x,y,start_mathRegion,end_mathRegion);
			PX_GeoDrawRingPoint(psurface,x-dry,y-drx,clr,x,y,start_mathRegion,end_mathRegion);
			//draw left
			S=(1-PX_FRAC(xoft4))/fdis;
			S*=S;
			S*=(fdis/2);
			S=1-S;
			clr=color;
			clr._argb.a=(px_uchar)(clr._argb.a*S);

			drx=PX_TRUNC(xoft4);
			dry=Sy;

			PX_GeoDrawRingPoint(psurface,x+drx,y+dry,clr,x,y,start_mathRegion,end_mathRegion);
			PX_GeoDrawRingPoint(psurface,x+drx,y-dry,clr,x,y,start_mathRegion,end_mathRegion);
			PX_GeoDrawRingPoint(psurface,x-drx,y+dry,clr,x,y,start_mathRegion,end_mathRegion);
			PX_GeoDrawRingPoint(psurface,x-drx,y-dry,clr,x,y,start_mathRegion,end_mathRegion);

			PX_GeoDrawRingPoint(psurface,x+dry,y+drx,clr,x,y,start_mathRegion,end_mathRegion);
			PX_GeoDrawRingPoint(psurface,x+dry,y-drx,clr,x,y,start_mathRegion,end_mathRegion);
			PX_GeoDrawRingPoint(psurface,x-dry,y+drx,clr,x,y,start_mathRegion,end_mathRegion);
			PX_GeoDrawRingPoint(psurface,x-dry,y-drx,clr,x,y,start_mathRegion,end_mathRegion);
		}
		else
		{
			fdis=xoft3-xoft4;
			S=PX_FRAC(xoft4)+fdis/2;
			clr=color;
			clr._argb.a=(px_uchar)(clr._argb.a*S);
			drx=PX_TRUNC(xoft3);
			dry=Sy;

			PX_GeoDrawRingPoint(psurface,x+drx,y+dry,clr,x,y,start_mathRegion,end_mathRegion);
			PX_GeoDrawRingPoint(psurface,x+drx,y-dry,clr,x,y,start_mathRegion,end_mathRegion);
			PX_GeoDrawRingPoint(psurface,x-drx,y+dry,clr,x,y,start_mathRegion,end_mathRegion);
			PX_GeoDrawRingPoint(psurface,x-drx,y-dry,clr,x,y,start_mathRegion,end_mathRegion);

			PX_GeoDrawRingPoint(psurface,x+dry,y+drx,clr,x,y,start_mathRegion,end_mathRegion);
			PX_GeoDrawRingPoint(psurface,x+dry,y-drx,clr,x,y,start_mathRegion,end_mathRegion);
			PX_GeoDrawRingPoint(psurface,x-dry,y+drx,clr,x,y,start_mathRegion,end_mathRegion);
			PX_GeoDrawRingPoint(psurface,x-dry,y-drx,clr,x,y,start_mathRegion,end_mathRegion);
		}
		xright=PX_TRUNC(xoft4);

		//////////////////////////////////////////////////////////////////////////

		for (i=xleft+1;i<xright;i++)
		{
			PX_GeoDrawRingPoint(psurface,x+i,y+Sy,color,x,y,start_mathRegion,end_mathRegion);
			PX_GeoDrawRingPoint(psurface,x+i,y-Sy,color,x,y,start_mathRegion,end_mathRegion);
			PX_GeoDrawRingPoint(psurface,x-i,y+Sy,color,x,y,start_mathRegion,end_mathRegion);
			PX_GeoDrawRingPoint(psurface,x-i,y-Sy,color,x,y,start_mathRegion,end_mathRegion);

			PX_GeoDrawRingPoint(psurface,x+Sy,y+i,color,x,y,start_mathRegion,end_mathRegion);
			PX_GeoDrawRingPoint(psurface,x+Sy,y-i,color,x,y,start_mathRegion,end_mathRegion);
			PX_GeoDrawRingPoint(psurface,x-Sy,y+i,color,x,y,start_mathRegion,end_mathRegion);
			PX_GeoDrawRingPoint(psurface,x-Sy,y-i,color,x,y,start_mathRegion,end_mathRegion);
		}
		Sy++;
	}
}

px_void PX_GeoDrawSolidRoundRect(px_surface *psurface, px_int left, px_int top, px_int right, px_int bottom,px_float roundRaduis,px_color color)
{
	px_int y,x,Height,Width;
	px_float r_x,r_y,dis;
	px_int drawWidth;
	px_int drawHeight;
	px_color drawColor;
	if(color._argb.a==0)
	{
		return;
	}
	if (roundRaduis<0)
	{
		roundRaduis=0;
	}

	if (left>right)
	{
		Height=left;
		left=right;
		right=Height;
	}
	if (top>bottom)
	{
		Height=top;
		top=bottom;
		bottom=Height;
	}
	Height=(bottom-top)+1;
	Width=(right-left)+1;

	r_y=roundRaduis;
	r_x=roundRaduis;
	for (y=0;y<(px_int)roundRaduis;y++)
	{
		if (Height&1)
		{
			if (y>Height/2)
			{
				break;
			}
		}
		else
		{
			if (y>Height/2-1)
			{
				break;
			}
		}
		
		for (x = 0;x<(px_int)roundRaduis;x++)
		{
			if (Width&1)
			{
				if (x>Width/2)
				{
					break;
				}
			}
			else
			{
				if (x>Width/2-1)
				{
					break;
				}
			}

			if (x>Width/2)
			{
				break;
			}

			dis=PX_sqrt((x-r_x)*(x-r_x)+(y-r_y)*(y-r_y));
			if (dis<roundRaduis+1.414f)
			{
				drawColor=color;

				if (dis > roundRaduis)
					drawColor._argb.a = (px_uchar)(color._argb.a * (1 - (dis - roundRaduis) / 1.414f));

				PX_SurfaceDrawPixel(psurface,left+x,top+y,drawColor);

				if(2*x<Width-1)
 				{
					PX_SurfaceDrawPixel(psurface,left+Width-x-1,top+y,drawColor);
					if (2*y<Height-1)
					{
						PX_SurfaceDrawPixel(psurface,left+x,top+Height-1-y,drawColor);
						PX_SurfaceDrawPixel(psurface,left+Width-x-1,top+Height-1-y,drawColor);
					}
				}
				else
				{
					PX_SurfaceDrawPixel(psurface,left+x,top+Height-1-y,drawColor);
				}
 				
			}
		}
		
	}
	
	if ((px_int)roundRaduis>Height/2)
	{
		drawHeight=(px_int)roundRaduis-Height/2;
	}
	else
	{
		drawHeight=(px_int)roundRaduis;
	}


	if (Width>2*(px_int)roundRaduis)
	{
		drawWidth=Width-2*(px_int)roundRaduis;

		if(drawWidth>0&&drawHeight>0)
		{
		PX_GeoDrawRect(psurface,left+(px_int)roundRaduis,top,left+(px_int)roundRaduis+drawWidth-1,top+drawHeight-1,color);
		PX_GeoDrawRect(psurface,left+(px_int)roundRaduis,bottom-drawHeight+1,left+(px_int)roundRaduis+drawWidth-1,bottom,color);
		}
	}

	if(Height>2*(px_int)roundRaduis)
		PX_GeoDrawRect(psurface,left,top+(px_int)roundRaduis,right,bottom-(px_int)roundRaduis,color);
		
}

px_void PX_GeoDrawRoundRect(px_surface *psurface, px_int left, px_int top, px_int right, px_int bottom,px_float roundRaduis,px_float linewidth,px_color color)
{
	px_int y,x,Height,Width;
	px_float r_x,r_y,dis;
	px_int drawWidth;
	px_int drawHeight;
	px_int drawlinewidth_int;
	px_color drawColor;
	if(color._argb.a==0)
	{
		return;
	}

	if (left>right)
	{
		Height=left;
		left=right;
		right=Height;
	}
	if (top>bottom)
	{
		Height=top;
		top=bottom;
		bottom=Height;
	}
	Height=(bottom-top)+1;
	Width=(right-left)+1;

	r_y=roundRaduis;
	r_x=roundRaduis;
	for (y=0;y<(px_int)roundRaduis;y++)
	{
		if (y > Height / 2)
		{
			break;
		}

		for (x = 0;x<(px_int)roundRaduis;x++)
		{
			
			if (x>Width/2)
			{
				break;
			}

			dis=PX_sqrt((x-r_x)*(x-r_x)+(y-r_y)*(y-r_y));
			if (dis<roundRaduis+1.414f&&dis>roundRaduis-linewidth)
			{
				drawColor=color;

				if (dis>roundRaduis)
					drawColor._argb.a=(px_uchar)(color._argb.a*(1 - (dis - roundRaduis)/1.414f));
				
				else if (dis>roundRaduis-linewidth-0.5f&&dis<roundRaduis - linewidth+1.414f-0.5f)
					drawColor._argb.a=(px_uchar)(color._argb.a*(dis-roundRaduis+linewidth+0.5f)/1.414f);

				PX_SurfaceDrawPixel(psurface,left+x,top+y,drawColor);

				if(2*x<Width-1)
				{
					PX_SurfaceDrawPixel(psurface,left+Width-x-1,top+y,drawColor);
					if (2*y<Height-1)
					{
						PX_SurfaceDrawPixel(psurface,left+x,top+Height-1-y,drawColor);
						PX_SurfaceDrawPixel(psurface,left+Width-x-1,top+Height-1-y,drawColor);
					}
				}
				else
				{
					PX_SurfaceDrawPixel(psurface,left+x,top+Height-1-y,drawColor);
				}

			}
		}

	}

	if (linewidth<1.0f)
	{
		drawlinewidth_int=1;
	}
	else
	{
		drawlinewidth_int=(px_int)linewidth;
	}

	if ((px_int)roundRaduis>Height/2)
	{
		drawHeight=(px_int)roundRaduis-Height/2;
	}
	else
	{
		drawHeight=(px_int)roundRaduis;
	}
	if (drawHeight>linewidth)
	{
		drawHeight=drawlinewidth_int;
	}
	
	if (Width>2*(px_int)roundRaduis)
	{
		drawWidth=Width-2*(px_int)roundRaduis;
		if(drawWidth>0&&drawHeight>0)
		{
			PX_GeoDrawRect(psurface,left+(px_int)roundRaduis,top,left+(px_int)roundRaduis+drawWidth-1,top+drawHeight-1,color);
			PX_GeoDrawRect(psurface,left+(px_int)roundRaduis,bottom-drawHeight+1,left+(px_int)roundRaduis+drawWidth-1,bottom,color);
		}
	}

 	if(Height>2*(px_int)roundRaduis)
 	{
		PX_GeoDrawRect(psurface,left,top+(px_int)roundRaduis,left+drawlinewidth_int-1,bottom-(px_int)roundRaduis,color);
		PX_GeoDrawRect(psurface,right-drawlinewidth_int+1,top+(px_int)roundRaduis,right,bottom-(px_int)roundRaduis,color);
	}
}

px_void PX_GeoDrawBezierCurvePoint(px_surface *rendersurface,px_point pt[],px_int pt_count,px_float t,px_float radius,px_color clr)
{
	px_int i;

	if (pt_count<=0)
	{
		return;
	}

	if (pt_count==1)
	{
		if(rendersurface)
			PX_GeoDrawPenCircle(rendersurface,pt[0].x,pt[0].y,radius,clr);
		return;
	}

	//update path
	for (i=0;i<pt_count-1;i++)
	{
		px_point vector_unit=PX_PointNormalization(PX_PointSub(pt[i+1],pt[i]));
		px_float distance=PX_PointMod(PX_PointSub(pt[i+1],pt[i]));

		pt[i]=PX_PointAdd(pt[i],PX_PointMul(vector_unit,distance*t));
	}

	PX_GeoDrawBezierCurvePoint(rendersurface,pt,pt_count-1,t,radius,clr);
}

px_point PX_GeoGetBezierCurvePoint(px_point pt[], px_int pt_count, px_float t)
{
	px_int i;

	if (pt_count <= 0)
	{
		PX_ASSERT();
	}

	if (pt_count == 1)
	{
		return pt[0];
	}

	//update path
	for (i = 0; i < pt_count - 1; i++)
	{
		px_point vector_unit = PX_PointNormalization(PX_PointSub(pt[i + 1], pt[i]));
		px_float distance = PX_PointMod(PX_PointSub(pt[i + 1], pt[i]));

		pt[i] = PX_PointAdd(pt[i], PX_PointMul(vector_unit, distance * t));
	}

	return PX_GeoGetBezierCurvePoint( pt, pt_count - 1, t);
}


px_void PX_GeoDrawBresenhamLine(px_surface *psurface,px_int x0, px_int y0, px_int x1, px_int y1,px_color color)
{
	px_point Cross2points[2];
	px_int CrossCount=0;
	px_int lm;
	px_int trimLX,trimRX,trimTY,trimBY;
	px_float ftemp,fconst;
	px_float k,recK;
	

	if (PX_ABS(x0-x1)+PX_ABS(y0-y1)==0)
	{
		return;
	}

	if (x0==x1)
	{
		if (y0<0)
		{
			y0=0;
		}
		if (y1<0)
		{
			y1=0;
		}
		if (y0>psurface->height-1)
		{
			y0=psurface->height-1;
		}
		if (y1>psurface->height-1)
		{
			y1=psurface->height-1;
		}
		PX_GeoDrawRect(psurface,x0,y0,x1,y1,color);
		return;
	}

	if (y0==y1)
	{
		if (x0<0)
		{
			x0=0;
		}
		if (x1<0)
		{
			x1=0;
		}
		if (x0>psurface->width-1)
		{
			x0=psurface->width-1;
		}
		if (x1>psurface->width-1)
		{
			x1=psurface->width-1;
		}
		PX_GeoDrawRect(psurface,x0,y0,x1,y1,color);
		return;
	}

	if (x0>x1)
	{
		lm=x0;
		x0=x1;
		x1=lm;

		lm=y0;
		y0=y1;
		y1=lm;
	} 

	k=((px_float)(y1-y0))/(x1-x0);
	recK=1/k;

	//trim
	trimLX=0-1;
	trimRX=psurface->width+1;
	trimTY=0-1;
	trimBY=psurface->height+1;

	//calculate c
	//TrimLX
	fconst=y0-k*x0;
	ftemp=trimLX*k+fconst;

	do 
	{
		if (ftemp>=trimTY&&ftemp<=trimBY)
		{
			Cross2points[CrossCount].x=(px_float)trimLX;
			Cross2points[CrossCount].y=ftemp;
			CrossCount++;
		}

		//TrimRX
		ftemp=trimRX*k+fconst;

		if (ftemp>=trimTY&&ftemp<=trimBY)
		{
			Cross2points[CrossCount].x=(px_float)trimRX;
			Cross2points[CrossCount].y=ftemp;
			CrossCount++;
			if (CrossCount==2)
			{
				break;
			}
		}

		//trimBY
		ftemp=(trimBY-fconst)/k;
		if (ftemp>trimLX&&ftemp<trimRX)
		{
			Cross2points[CrossCount].x=ftemp;
			Cross2points[CrossCount].y=(px_float)trimBY;
			CrossCount++;
			if (CrossCount==2)
			{
				break;
			}
		}

		//trimTY
		ftemp=(trimTY-fconst)/k;
		if (ftemp>trimLX&&ftemp<trimRX)
		{
			Cross2points[CrossCount].x=ftemp;
			Cross2points[CrossCount].y=(px_float)trimTY;
			CrossCount++;
		}
	} while (0);

	if (CrossCount!=2)
	{
		return;
	}


	if (Cross2points[0].x>Cross2points[1].x)
	{
		ftemp=Cross2points[0].x;
		Cross2points[0].x=Cross2points[1].x;
		Cross2points[1].x=ftemp;

		ftemp=Cross2points[0].y;
		Cross2points[0].y=Cross2points[1].y;
		Cross2points[1].y=ftemp;
	}


	if (x0<Cross2points[0].x)
	{
		x0=(px_int)Cross2points[0].x;
		y0=(px_int)Cross2points[0].y;
	}
	else if(x0>Cross2points[1].x)
	{
		return;
	}


	if (x1<Cross2points[0].x)
	{
		return;
	}
	else if(x1>Cross2points[1].x)
	{
		x1=(px_int)Cross2points[1].x;
		y1=(px_int)Cross2points[1].y;
	}

	if (x1==x0&&y1==y0)
	{
		return;
	}


	do 
	{
		px_int dx = PX_ABS(x1-x0), sx = x0<x1 ? 1 : -1;
		px_int dy = PX_ABS(y1-y0), sy = y0<y1 ? 1 : -1; 
		px_int err = (dx>dy ? dx : -dy)/2, e2;
		while(PX_TRUE)
		{
			PX_SurfaceDrawPixel(psurface,x0,y0,color);
			if (x0==x1 && y0==y1) break;
			e2 = err;
			if (e2 >-dx) { err -= dy; x0 += sx; }
			if (e2 < dy) { err += dx; y0 += sy; }
		}
	} while (0);
	
}

px_void PX_GeoDrawTriangle(px_surface *psurface,px_point2D p0,px_point2D p1,px_point2D p2,px_color color)
{
	px_int ix,iy;
	px_bool  k01infinite=PX_FALSE;
	px_bool  k02infinite=PX_FALSE;
	px_bool  k12infinite=PX_FALSE;
	px_float k01,b01,k02,b02,k12,b12;

	px_float lAlpha=1,rAlpha=1;

	px_float x0;
	px_float y0;

	px_float x1;
	px_float y1;

	px_float x2;
	px_float y2;



	px_float y, xleft, xright; 
	px_float btmy,midy;


	//    p0
	// p1   p2

	if (p1.y<p0.y)
	{
		px_point2D t;
		t=p1;
		p1=p0;
		p0=t;
	}

	if (p2.y<p0.y)
	{
		px_point2D t;
		t=p2;
		p2=p0;
		p0=t;
	}

	btmy=p1.y;
	midy=p2.y;
	if (p2.y>btmy)
	{
		midy=p1.y;
		btmy=p2.y;
	}



	do 
	{
		px_float x01m;

		x0=p0.x;
		y0=p0.y;
		x1=p1.x;
		y1=p1.y;
		x2=p2.x;
		y2=p2.y;


		if (x0==x1)
		{
			x01m=x0;
		}
		else
		{
			k01=(y0-y1)/(x0-x1);
			b01=y0-k01*x0;
			x01m=(y2-b01)/k01;
		}

		if (x01m>x2)
		{
			px_point2D t;
			t=p2;
			p2=p1;
			p1=t;
		}
	} while (0);



	x0=p0.x;
	y0=p0.y;

	x1=p1.x;
	y1=p1.y;

	x2=p2.x;
	y2=p2.y;


	k01infinite=PX_FALSE;
	k02infinite=PX_FALSE;
	k12infinite=PX_FALSE;
	if (x0==x1)
	{
		k01infinite=PX_TRUE;
		b01=x0;
	}
	else
	{
		k01=(y0-y1)/(x0-x1);
		b01=y0-k01*x0;
	}

	if (x0==x2)
	{
		k02infinite=PX_TRUE;
		b02=x0;
	}
	else
	{
		k02=(y0-y2)/(x0-x2);
		b02=y0-k02*x0;
	}

	if (x1==x2)
	{
		k12infinite=PX_TRUE;
		b12=x1;
	}
	else
	{
		k12=(y1-y2)/(x1-x2);
		b12=y1-k12*x1;
	}


	for(y = (px_int)(y0+0.5f)+0.5f; y <=midy; y++)
	{
		if (k01infinite)
		{
			xleft=b01;
			lAlpha=1;
		}
		else
		{
			xleft = (y-b01)/k01;
			lAlpha=PX_ABS_FRAC(xleft);
		}

		if (k02infinite)
		{
			xright=b02;
			rAlpha=1;
		}
		else
		{
			xright = (y-b02)/k02;
			rAlpha=PX_ABS_FRAC(xright);
		}

		ix = (px_int)xleft;
		iy=(px_int)y;
		do
		{
			px_color aClr=color;
			aClr._argb.a=(px_byte)(aClr._argb.a*lAlpha);
			PX_SurfaceDrawPixel(psurface,ix,iy,aClr);
		} while (0);

		for(ix = (px_int)xleft+1;ix < (px_int)xright; ++ix)
		{
			PX_SurfaceDrawPixel(psurface,ix,iy,color);
		}

// 		do
// 		{
// 			px_color aClr=color;
// 			aClr._argb.a=(px_byte)(aClr._argb.a*rAlpha);
// 			PX_SurfaceDrawPixel(psurface,ix,iy,aClr);
// 		} while (0);
	}

	// p1   p2
	//    p0
	if (p1.y>p0.y)
	{
		px_point2D t;
		t=p1;
		p1=p0;
		p0=t;
	}

	if (p2.y>p0.y)
	{
		px_point2D t;
		t=p2;
		p2=p0;
		p0=t;
	}

	btmy=p1.y;
	midy=p2.y;
	if (p2.y<btmy)
	{
		midy=p1.y;
		btmy=p2.y;
	}



	do 
	{
		px_float x01m;

		x0=p0.x;
		y0=p0.y;
		x1=p1.x;
		y1=p1.y;
		x2=p2.x;
		y2=p2.y;


		if (x0==x1)
		{
			x01m=x0;
		}
		else
		{
			k01=(y0-y1)/(x0-x1);
			b01=y0-k01*x0;
			x01m=(y2-b01)/k01;
		}

		if (x01m>x2)
		{
			px_point2D t;
			t=p2;
			p2=p1;
			p1=t;
		}
	} while (0);



	x0=p0.x;
	y0=p0.y;

	x1=p1.x;
	y1=p1.y;

	x2=p2.x;
	y2=p2.y;


	k01infinite=PX_FALSE;
	k02infinite=PX_FALSE;
	k12infinite=PX_FALSE;
	if (x0==x1)
	{
		k01infinite=PX_TRUE;
		b01=x0;
	}
	else
	{
		k01=(y0-y1)/(x0-x1);
		b01=y0-k01*x0;
	}

	if (x0==x2)
	{
		k02infinite=PX_TRUE;
		b02=x0;
	}
	else
	{
		k02=(y0-y2)/(x0-x2);
		b02=y0-k02*x0;
	}

	if (x1==x2)
	{
		k12infinite=PX_TRUE;
		b12=x1;
	}
	else
	{
		k12=(y1-y2)/(x1-x2);
		b12=y1-k12*x1;
	}


	for(y = (px_int)(midy+0.5f)+0.5f; y < y0; y++)
	{
		if (k01infinite)
		{
			xleft=b01;
			lAlpha=1;
		}
		else
		{
			xleft = (y-b01)/k01;
			lAlpha=PX_ABS_FRAC(xleft);
		}

		if (k02infinite)
		{
			xright=b02;
			rAlpha=1;
		}
		else
		{
			xright = (y-b02)/k02;
			rAlpha=PX_ABS_FRAC(xright);
		}

		ix = (px_int)xleft;
		iy=(px_int)y;
		do
		{
			px_color aClr=color;
			aClr._argb.a=(px_byte)(aClr._argb.a*lAlpha);
			PX_SurfaceDrawPixel(psurface,ix,iy,aClr);
		} while (0);

		for(ix = (px_int)xleft+1;ix < (px_int)xright; ++ix)
		{
			PX_SurfaceDrawPixel(psurface,ix,iy,color);
		}

	}

}

px_void PX_GeoRasterizeTriangle(px_surface* psurface,px_int x1, px_int y1, px_int x2, px_int y2, px_int x3, px_int y3,px_color color)
{
	px_int minX = (x1 < x2) ? ((x1 < x3) ? x1 : x3) : ((x2 < x3) ? x2 : x3), maxX = (x1 > x2) ? ((x1 > x3) ? x1 : x3) : ((x2 > x3) ? x2 : x3);
	px_int minY = (y1 < y2) ? ((y1 < y3) ? y1 : y3) : ((y2 < y3) ? y2 : y3), maxY = (y1 > y2) ? ((y1 > y3) ? y1 : y3) : ((y2 > y3) ? y2 : y3);
	px_int xcount, ycount;
	px_int gpu_x1, gpu_y1, gpu_x2, gpu_y2, gpu_x3, gpu_y3;
	px_int x, y;
	
	gpu_x1 = x1 - minX;
	gpu_y1 = y1 - minY;
	gpu_x2 = x2 - minX;
	gpu_y2 = y2 - minY;
	gpu_x3 = x3 - minX;
	gpu_y3 = y3 - minY;

	if (minX < 0)
		minX = 0;

	if (minX >= psurface->width)
		return;
	
	if (maxX < 0)
		return;

	if(maxX>=psurface->width)
		maxX=psurface->width-1;
	
	
	if(minY<0)
		minY=0;

	if (minY >= psurface->height)
		return;

	if(maxY>=psurface->height)
		maxY=psurface->height-1;

	if (maxY < 0)
		return;

	xcount = maxX - minX + 1;
	ycount = maxY - minY + 1;

	if (xcount > 0 && ycount > 0)
	{
		
#ifdef PX_GPU_ENABLE
		px_color *pGPU_dst_addr= psurface->surfaceBuffer+minY*psurface->width+minX;
		PX_GPU_RenderTriangleRasterizer(psurface->surfaceBuffer, xcount,xcount,ycount, pGPU_dst_addr, psurface->width, PX_COLOR_FORMAT,0x80808080, gpu_x1,gpu_y1,gpu_x2,gpu_y2,gpu_x3,gpu_y3, color,PX_COLOR(0,255,255,255));
#else
		for (y = minY; y <= maxY; y++) for (x = minX; x <= maxX; x++)
		{
			px_int area1 = (x2 - x1) * (y - y1) - (y2 - y1) * (x - x1);
			px_int area2 = (x3 - x2) * (y - y2) - (y3 - y2) * (x - x2);
			px_int area3 = (x1 - x3) * (y - y3) - (y1 - y3) * (x - x3);
			px_byte area1_32, area2_32, area3_32;
			area1_32 = (area1 & 0x80000000)?1:0;
			area2_32 = (area2 & 0x80000000) ? 1 : 0;
			area3_32 = (area3 & 0x80000000) ? 1 : 0;
			if (area1_32== area2_32&& area2_32== area3_32)
			{
				PX_SurfaceDrawPixel(psurface,x, y, color);
			}
		}
#endif
	}
}
// Signed distance from point to triangle (negative inside, positive outside)
static px_float px_sd_triangle(px_float px, px_float py,
	px_float ax, px_float ay, px_float bx, px_float by, px_float cx, px_float cy)
{
	px_float e0x = bx - ax, e0y = by - ay;
	px_float e1x = cx - bx, e1y = cy - by;
	px_float e2x = ax - cx, e2y = ay - cy;
	px_float v0x = px - ax, v0y = py - ay;
	px_float v1x = px - bx, v1y = py - by;
	px_float v2x = px - cx, v2y = py - cy;

	px_float pq0x, pq0y, pq1x, pq1y, pq2x, pq2y;
	px_float d0, d1, d2, d, s;
	px_float t;

	// edge 0: A->B
	t = (v0x * e0x + v0y * e0y) / (e0x * e0x + e0y * e0y);
	t = px_clampf(t, 0.0f, 1.0f);
	pq0x = v0x - e0x * t; pq0y = v0y - e0y * t;
	d0 = pq0x * pq0x + pq0y * pq0y;

	// edge 1: B->C
	t = (v1x * e1x + v1y * e1y) / (e1x * e1x + e1y * e1y);
	t = px_clampf(t, 0.0f, 1.0f);
	pq1x = v1x - e1x * t; pq1y = v1y - e1y * t;
	d1 = pq1x * pq1x + pq1y * pq1y;

	// edge 2: C->A
	t = (v2x * e2x + v2y * e2y) / (e2x * e2x + e2y * e2y);
	t = px_clampf(t, 0.0f, 1.0f);
	pq2x = v2x - e2x * t; pq2y = v2y - e2y * t;
	d2 = pq2x * pq2x + pq2y * pq2y;

	// minimum squared distance
	d = d0;
	if (d1 < d) d = d1;
	if (d2 < d) d = d2;

	// sign: negative inside, positive outside (cross product winding)
	s = (e0x * v0y - e0y * v0x > 0.0f &&
		e1x * v1y - e1y * v1x > 0.0f &&
		e2x * v2y - e2y * v2x > 0.0f) ? -1.0f : 1.0f;
	if (s > 0.0f)
	{
		// check opposite winding
		if (e0x * v0y - e0y * v0x < 0.0f &&
			e1x * v1y - e1y * v1x < 0.0f &&
			e2x * v2y - e2y * v2x < 0.0f)
			s = -1.0f;
	}

	return s * PX_sqrt(d);
}

// Signed distance from point to line segment (capsule shape)
static px_float px_sd_segment(px_float px, px_float py,
	px_float ax, px_float ay, px_float bx, px_float by, px_float radius)
{
	px_float d2 = px_dist2_point_to_segment(px, py, ax, ay, bx, by);
	return PX_sqrt(d2) - radius;
}

px_void PX_GeoDrawArrow(px_surface *psurface,px_point2D p0,px_point2D p1,px_float size,px_color color)
{
	px_float totalLen, headLen, headHalfW, shaftR, aa;
	px_float dx, dy;
	px_point2D v, vn, headBase, headLeft, headRight, shaftEnd;
	px_float minx, miny, maxx, maxy;
	px_int ix0, iy0, ix1, iy1, ix, iy;

	if (size < 0.5f) return;
	if (color._argb.a == 0) return;

	dx = p1.x - p0.x;
	dy = p1.y - p0.y;
	totalLen = PX_sqrt(dx * dx + dy * dy);
	if (totalLen < 1e-4f) return;

	// direction and normal
	v.x = dx / totalLen; v.y = dy / totalLen;
	vn.x = -v.y; vn.y = v.x;

	// proportions
	shaftR = size * 0.5f;
	if (size < 5.0f)
	{
		px_float scale = 1.0f + (5.0f - size) * 0.4f;
		headLen = size * 3.0f * scale;
		headHalfW = size * 1.5f * scale;
	}
	else
	{
		headLen = size * 3.0f;
		headHalfW = size * 1.5f;
	}
	if (headLen > totalLen * 0.6f)
	{
		headLen = totalLen * 0.6f;
		headHalfW = headLen * 0.5f;
	}

	// key points
	headBase.x = p1.x - v.x * headLen;
	headBase.y = p1.y - v.y * headLen;
	shaftEnd.x = headBase.x + v.x * shaftR * 0.3f; // slight overlap to avoid gap
	shaftEnd.y = headBase.y + v.y * shaftR * 0.3f;
	headLeft.x = headBase.x + vn.x * headHalfW;
	headLeft.y = headBase.y + vn.y * headHalfW;
	headRight.x = headBase.x - vn.x * headHalfW;
	headRight.y = headBase.y - vn.y * headHalfW;

	aa = 0.7f; // anti-aliasing smoothing width

	// bounding box
	minx = p0.x; maxx = p0.x;
	miny = p0.y; maxy = p0.y;

	if (p1.x < minx) minx = p1.x; if (p1.x > maxx) maxx = p1.x;
	if (p1.y < miny) miny = p1.y; if (p1.y > maxy) maxy = p1.y;
	if (headLeft.x < minx) minx = headLeft.x; if (headLeft.x > maxx) maxx = headLeft.x;
	if (headLeft.y < miny) miny = headLeft.y; if (headLeft.y > maxy) maxy = headLeft.y;
	if (headRight.x < minx) minx = headRight.x; if (headRight.x > maxx) maxx = headRight.x;
	if (headRight.y < miny) miny = headRight.y; if (headRight.y > maxy) maxy = headRight.y;

	// expand by shaft radius + AA margin
	minx -= shaftR + aa + 1.0f;
	miny -= shaftR + aa + 1.0f;
	maxx += shaftR + aa + 1.0f;
	maxy += shaftR + aa + 1.0f;

	// clip to surface
	ix0 = (px_int)PX_floor(minx);
	iy0 = (px_int)PX_floor(miny);
	ix1 = (px_int)PX_ceil(maxx);
	iy1 = (px_int)PX_ceil(maxy);

	if (ix0 < psurface->limit_left) ix0 = psurface->limit_left;
	if (iy0 < psurface->limit_top) iy0 = psurface->limit_top;
	if (ix1 > psurface->limit_right) ix1 = psurface->limit_right;
	if (iy1 > psurface->limit_bottom) iy1 = psurface->limit_bottom;

	// rasterize: union of shaft capsule and arrowhead triangle
	for (iy = iy0; iy <= iy1; ++iy)
	{
		px_float cy = iy + 0.5f;
		for (ix = ix0; ix <= ix1; ++ix)
		{
			px_float cx = ix + 0.5f;
			px_float dShaft, dHead, dMin, cov;
			px_color c;

			// signed distance to shaft (capsule: line segment + radius)
			dShaft = px_sd_segment(cx, cy, p0.x, p0.y, shaftEnd.x, shaftEnd.y, shaftR);

			// signed distance to arrowhead triangle
			dHead = px_sd_triangle(cx, cy,
				headLeft.x, headLeft.y,
				p1.x, p1.y,
				headRight.x, headRight.y);

			// union: min of signed distances
			dMin = (dShaft < dHead) ? dShaft : dHead;

			if (dMin > aa) continue; // outside AA region

			// coverage: smooth step from 1 (fully inside) to 0 (outside)
			if (dMin < -aa)
				cov = 1.0f;
			else
				cov = (aa - dMin) / (2.0f * aa);

			if (cov <= 0.0f) continue;
			if (cov > 1.0f) cov = 1.0f;

			c = color;
			c._argb.a = (px_byte)(color._argb.a * cov + 0.5f);
			if (c._argb.a)
				PX_SurfaceDrawPixel(psurface, ix, iy, c);
		}
	}
}

// Signed distance from point to a convex/concave polygon (negative inside, positive outside)
// vertices must be ordered (CW or CCW)
static px_float px_sd_polygon(px_float px, px_float py, px_float vx[], px_float vy[], px_int n)
{
	px_float d = (px - vx[0]) * (px - vx[0]) + (py - vy[0]) * (py - vy[0]);
	px_float s = 1.0f;
	px_int i, j;

	for (i = 0, j = n - 1; i < n; j = i, i++)
	{
		px_float ex = vx[j] - vx[i];
		px_float ey = vy[j] - vy[i];
		px_float wx = px - vx[i];
		px_float wy = py - vy[i];
		px_float t, dd;
		px_float bx, by;

		t = (wx * ex + wy * ey) / (ex * ex + ey * ey);
		t = px_clampf(t, 0.0f, 1.0f);
		bx = wx - ex * t;
		by = wy - ey * t;
		dd = bx * bx + by * by;
		if (dd < d) d = dd;

		// winding number contribution
		{
			px_bool c1 = (py >= vy[i]);
			px_bool c2 = (py < vy[j]);
			px_bool c3 = (ex * wy - ey * wx > 0.0f);
			if ( (c1 && c2 && c3) || (!c1 && !c2 && !c3) )
				s = -s;
		}
	}

	return s * PX_sqrt(d);
}

px_void PX_GeoDrawStar(px_surface *psurface,px_float x,px_float y,px_float radius,px_int n,px_float inside_angle,px_float angle,px_color color)
{
	px_float vx[64], vy[64]; // max 32-pointed star
	px_float innerR, aa;
	px_float minx, miny, maxx, maxy;
	px_int ix0, iy0, ix1, iy1, ix, iy;
	px_int i, vertexCount;
	px_float angleRad, halfTipRad, stepRad;

	if (n < 3) n = 3;
	if (n > 32) n = 32;
	if (radius < 1.0f) return;
	if (color._argb.a == 0) return;
	if (inside_angle <= 0.0f) inside_angle = 1.0f;
	if (inside_angle >= 180.0f) inside_angle = 179.0f;

	vertexCount = n * 2;
	stepRad = 2.0f * (px_float)PX_PI / n;
	halfTipRad = (px_float)PX_PI * inside_angle / 360.0f;

	// compute inner radius from the inside angle at tips
	// In the triangle formed by center, outer tip, and inner vertex:
	// innerR = R * sin(halfTip) / sin(halfTip + pi/n)
	innerR = radius * PX_sin_radian(halfTipRad) / PX_sin_radian(halfTipRad + stepRad * 0.5f);
	if (innerR < 0.0f) innerR = 0.0f;

	// convert rotation angle (degrees) to radians, first tip points up
	angleRad = (px_float)PX_PI * angle / 180.0f - (px_float)PX_PI / 2.0f;

	// compute 2*n vertices: alternating outer and inner
	for (i = 0; i < n; i++)
	{
		px_float outerA = angleRad + i * stepRad;
		px_float innerA = outerA + stepRad * 0.5f;

		vx[i * 2]     = x + radius * PX_cos_radian(outerA);
		vy[i * 2]     = y + radius * PX_sin_radian(outerA);
		vx[i * 2 + 1] = x + innerR * PX_cos_radian(innerA);
		vy[i * 2 + 1] = y + innerR * PX_sin_radian(innerA);
	}

	aa = 0.7f;

	// bounding box
	minx = x - radius - aa - 1.0f;
	miny = y - radius - aa - 1.0f;
	maxx = x + radius + aa + 1.0f;
	maxy = y + radius + aa + 1.0f;

	// clip to surface
	ix0 = (px_int)PX_floor(minx);
	iy0 = (px_int)PX_floor(miny);
	ix1 = (px_int)PX_ceil(maxx);
	iy1 = (px_int)PX_ceil(maxy);

	if (ix0 < psurface->limit_left) ix0 = psurface->limit_left;
	if (iy0 < psurface->limit_top) iy0 = psurface->limit_top;
	if (ix1 > psurface->limit_right) ix1 = psurface->limit_right;
	if (iy1 > psurface->limit_bottom) iy1 = psurface->limit_bottom;

	for (iy = iy0; iy <= iy1; ++iy)
	{
		px_float cy = iy + 0.5f;
		for (ix = ix0; ix <= ix1; ++ix)
		{
			px_float cx = ix + 0.5f;
			px_float d, cov;
			px_color c;

			d = px_sd_polygon(cx, cy, vx, vy, vertexCount);

			if (d > aa) continue;

			if (d < -aa)
				cov = 1.0f;
			else
				cov = (aa - d) / (2.0f * aa);

			if (cov <= 0.0f) continue;
			if (cov > 1.0f) cov = 1.0f;

			c = color;
			c._argb.a = (px_byte)(color._argb.a * cov + 0.5f);
			if (c._argb.a)
				PX_SurfaceDrawPixel(psurface, ix, iy, c);
		}
	}
}

px_void PX_GeoDrawFlare(px_surface *psurface,px_float x,px_float y,px_float radius,px_float strength,px_color color)
{
	px_int ix0, iy0, ix1, iy1, ix, iy;
	px_float r2, inv_r2;

	if (radius < 0.5f) return;
	if (color._argb.a == 0) return;
	if (strength <= 0.0f) return;
	if (strength > 10.0f) strength = 10.0f;

	r2 = radius * radius;
	inv_r2 = 1.0f / r2;

	// bounding box
	ix0 = (px_int)PX_floor(x - radius - 1.0f);
	iy0 = (px_int)PX_floor(y - radius - 1.0f);
	ix1 = (px_int)PX_ceil(x + radius + 1.0f);
	iy1 = (px_int)PX_ceil(y + radius + 1.0f);

	// clip to surface
	if (ix0 < psurface->limit_left) ix0 = psurface->limit_left;
	if (iy0 < psurface->limit_top) iy0 = psurface->limit_top;
	if (ix1 > psurface->limit_right) ix1 = psurface->limit_right;
	if (iy1 > psurface->limit_bottom) iy1 = psurface->limit_bottom;

	for (iy = iy0; iy <= iy1; ++iy)
	{
		px_float cy = iy + 0.5f - y;
		px_float cy2 = cy * cy;
		for (ix = ix0; ix <= ix1; ++ix)
		{
			px_float cx = ix + 0.5f - x;
			px_float d2 = cx * cx + cy2;
			px_float intensity;
			px_int a;
			px_color c;

			if (d2 > r2) continue;

			// Gaussian falloff: exp(-3 * d^2 / r^2)
			// The constant 3.0 means intensity drops to ~5% at the edge
			intensity = strength * (px_float)PX_exp(-3.0 * d2 * inv_r2);

			if (intensity < (1.0f / 255.0f)) continue;

			a = (px_int)(color._argb.a * intensity + 0.5f);
			if (a > 255) a = 255;
			if (a <= 0) continue;

			c = color;
			c._argb.a = (px_uchar)a;
			PX_SurfaceDrawPixel(psurface, ix, iy, c);
		}
	}
}

px_void PX_GeoDrawRegularPolygon(px_surface *psurface,px_float x,px_float y,px_float radius,px_int n,px_float angle,px_color color)
{
	px_float vx[64], vy[64];
	px_float aa, angleRad, stepRad;
	px_float minx, miny, maxx, maxy;
	px_int ix0, iy0, ix1, iy1, ix, iy, i;

	if (n < 3) n = 3;
	if (n > 64) n = 64;
	if (radius < 1.0f) return;
	if (color._argb.a == 0) return;

	angleRad = (px_float)PX_PI * angle / 180.0f - (px_float)PX_PI / 2.0f;
	stepRad = 2.0f * (px_float)PX_PI / n;

	for (i = 0; i < n; i++)
	{
		px_float a = angleRad + i * stepRad;
		vx[i] = x + radius * PX_cos_radian(a);
		vy[i] = y + radius * PX_sin_radian(a);
	}

	aa = 0.7f;
	minx = x - radius - aa - 1.0f;
	miny = y - radius - aa - 1.0f;
	maxx = x + radius + aa + 1.0f;
	maxy = y + radius + aa + 1.0f;

	ix0 = (px_int)PX_floor(minx);
	iy0 = (px_int)PX_floor(miny);
	ix1 = (px_int)PX_ceil(maxx);
	iy1 = (px_int)PX_ceil(maxy);

	if (ix0 < psurface->limit_left) ix0 = psurface->limit_left;
	if (iy0 < psurface->limit_top) iy0 = psurface->limit_top;
	if (ix1 > psurface->limit_right) ix1 = psurface->limit_right;
	if (iy1 > psurface->limit_bottom) iy1 = psurface->limit_bottom;

	for (iy = iy0; iy <= iy1; ++iy)
	{
		px_float cy = iy + 0.5f;
		for (ix = ix0; ix <= ix1; ++ix)
		{
			px_float cx = ix + 0.5f;
			px_float d, cov;
			px_color c;

			d = px_sd_polygon(cx, cy, vx, vy, n);
			if (d > aa) continue;

			if (d < -aa)
				cov = 1.0f;
			else
				cov = (aa - d) / (2.0f * aa);

			if (cov <= 0.0f) continue;
			if (cov > 1.0f) cov = 1.0f;

			c = color;
			c._argb.a = (px_byte)(color._argb.a * cov + 0.5f);
			if (c._argb.a)
				PX_SurfaceDrawPixel(psurface, ix, iy, c);
		}
	}
}

px_void PX_GeoDrawEllipse(px_surface *psurface,px_float x,px_float y,px_float rx,px_float ry,px_float angle,px_color color)
{
	px_float aa;
	px_float cosA, sinA;
	px_float maxR;
	px_float minx, miny, maxx, maxy;
	px_int ix0, iy0, ix1, iy1, ix, iy;

	if (rx < 0.5f || ry < 0.5f) return;
	if (color._argb.a == 0) return;

	aa = 0.7f;
	cosA = PX_cos_radian((px_float)PX_PI * angle / 180.0f);
	sinA = PX_sin_radian((px_float)PX_PI * angle / 180.0f);

	maxR = (rx > ry ? rx : ry);
	minx = x - maxR - aa - 1.0f;
	miny = y - maxR - aa - 1.0f;
	maxx = x + maxR + aa + 1.0f;
	maxy = y + maxR + aa + 1.0f;

	ix0 = (px_int)PX_floor(minx);
	iy0 = (px_int)PX_floor(miny);
	ix1 = (px_int)PX_ceil(maxx);
	iy1 = (px_int)PX_ceil(maxy);

	if (ix0 < psurface->limit_left) ix0 = psurface->limit_left;
	if (iy0 < psurface->limit_top) iy0 = psurface->limit_top;
	if (ix1 > psurface->limit_right) ix1 = psurface->limit_right;
	if (iy1 > psurface->limit_bottom) iy1 = psurface->limit_bottom;

	for (iy = iy0; iy <= iy1; ++iy)
	{
		px_float cy = iy + 0.5f - y;
		for (ix = ix0; ix <= ix1; ++ix)
		{
			px_float cx = ix + 0.5f - x;
			// rotate into ellipse-local frame
			px_float lx = cx * cosA + cy * sinA;
			px_float ly = -cx * sinA + cy * cosA;
			// normalized distance in ellipse space
			px_float ex = lx / rx;
			px_float ey = ly / ry;
			px_float e2 = ex * ex + ey * ey;
			px_float e, d, cov;
			px_color c;

			if (e2 > 4.0f) continue; // fast reject

			e = PX_sqrt(e2);
			// approximate signed distance: (e - 1) * min(rx, ry)
			// This is an approximation that works well for moderate eccentricity
			d = (e - 1.0f) * (rx < ry ? rx : ry);

			if (d > aa) continue;

			if (d < -aa)
				cov = 1.0f;
			else
				cov = (aa - d) / (2.0f * aa);

			if (cov <= 0.0f) continue;
			if (cov > 1.0f) cov = 1.0f;

			c = color;
			c._argb.a = (px_byte)(color._argb.a * cov + 0.5f);
			if (c._argb.a)
				PX_SurfaceDrawPixel(psurface, ix, iy, c);
		}
	}
}

px_void PX_GeoDrawCapsule(px_surface *psurface,px_float x,px_float y,px_float width,px_float height,px_float angle,px_color color)
{
	px_float aa, cosA, sinA, halfLen, R;
	px_float maxR;
	px_float minx, miny, maxx, maxy;
	px_int ix0, iy0, ix1, iy1, ix, iy;

	if (width < 1.0f || height < 1.0f) return;
	if (color._argb.a == 0) return;

	aa = 0.7f;
	cosA = PX_cos_radian((px_float)PX_PI * angle / 180.0f);
	sinA = PX_sin_radian((px_float)PX_PI * angle / 180.0f);

	if (width >= height)
	{
		R = height * 0.5f;
		halfLen = width * 0.5f - R;
	}
	else
	{
		R = width * 0.5f;
		halfLen = height * 0.5f - R;
	}
	if (halfLen < 0.0f) halfLen = 0.0f;

	maxR = (width > height ? width : height) * 0.5f;
	minx = x - maxR - aa - 1.0f;
	miny = y - maxR - aa - 1.0f;
	maxx = x + maxR + aa + 1.0f;
	maxy = y + maxR + aa + 1.0f;

	ix0 = (px_int)PX_floor(minx);
	iy0 = (px_int)PX_floor(miny);
	ix1 = (px_int)PX_ceil(maxx);
	iy1 = (px_int)PX_ceil(maxy);

	if (ix0 < psurface->limit_left) ix0 = psurface->limit_left;
	if (iy0 < psurface->limit_top) iy0 = psurface->limit_top;
	if (ix1 > psurface->limit_right) ix1 = psurface->limit_right;
	if (iy1 > psurface->limit_bottom) iy1 = psurface->limit_bottom;

	for (iy = iy0; iy <= iy1; ++iy)
	{
		px_float cy = iy + 0.5f - y;
		for (ix = ix0; ix <= ix1; ++ix)
		{
			px_float cx = ix + 0.5f - x;
			px_float lx, ly, d, cov;
			px_color c;

			// rotate into local frame (capsule along x-axis if width>=height, y-axis otherwise)
			if (width >= height)
			{
				lx = cx * cosA + cy * sinA;
				ly = -cx * sinA + cy * cosA;
			}
			else
			{
				lx = cx * sinA - cy * cosA;
				ly = cx * cosA + cy * sinA;
			}

			// capsule SDF: distance to line segment [-halfLen,0]->[halfLen,0] minus R
			{
				px_float clampX = lx;
				if (clampX < -halfLen) clampX = -halfLen;
				if (clampX > halfLen) clampX = halfLen;
				d = PX_sqrt((lx - clampX) * (lx - clampX) + ly * ly) - R;
			}

			if (d > aa) continue;

			if (d < -aa)
				cov = 1.0f;
			else
				cov = (aa - d) / (2.0f * aa);

			if (cov <= 0.0f) continue;
			if (cov > 1.0f) cov = 1.0f;

			c = color;
			c._argb.a = (px_byte)(color._argb.a * cov + 0.5f);
			if (c._argb.a)
				PX_SurfaceDrawPixel(psurface, ix, iy, c);
		}
	}
}

px_void PX_GeoDrawCross(px_surface *psurface,px_float x,px_float y,px_float size,px_float linewidth,px_float angle,px_color color)
{
	px_float aa, cosA, sinA, halfSize, halfW;
	px_float maxR;
	px_float minx, miny, maxx, maxy;
	px_int ix0, iy0, ix1, iy1, ix, iy;

	if (size < 1.0f || linewidth < 0.5f) return;
	if (color._argb.a == 0) return;

	aa = 0.7f;
	cosA = PX_cos_radian((px_float)PX_PI * angle / 180.0f);
	sinA = PX_sin_radian((px_float)PX_PI * angle / 180.0f);
	halfSize = size * 0.5f;
	halfW = linewidth * 0.5f;

	maxR = PX_sqrt(halfSize * halfSize + halfW * halfW) + aa + 1.0f;
	minx = x - maxR; miny = y - maxR;
	maxx = x + maxR; maxy = y + maxR;

	ix0 = (px_int)PX_floor(minx);
	iy0 = (px_int)PX_floor(miny);
	ix1 = (px_int)PX_ceil(maxx);
	iy1 = (px_int)PX_ceil(maxy);

	if (ix0 < psurface->limit_left) ix0 = psurface->limit_left;
	if (iy0 < psurface->limit_top) iy0 = psurface->limit_top;
	if (ix1 > psurface->limit_right) ix1 = psurface->limit_right;
	if (iy1 > psurface->limit_bottom) iy1 = psurface->limit_bottom;

	for (iy = iy0; iy <= iy1; ++iy)
	{
		px_float cy = iy + 0.5f - y;
		for (ix = ix0; ix <= ix1; ++ix)
		{
			px_float cx = ix + 0.5f - x;
			px_float lx, ly, ax, ay;
			px_float dH, dV, d, cov;
			px_color c;

			// rotate into local frame
			lx = cx * cosA + cy * sinA;
			ly = -cx * sinA + cy * cosA;

			ax = PX_ABS(lx);
			ay = PX_ABS(ly);

			// SDF of horizontal bar: box [-halfSize, -halfW] to [halfSize, halfW]
			{
				px_float dx = ax - halfSize;
				px_float dy = ay - halfW;
				px_float odx = dx > 0.0f ? dx : 0.0f;
				px_float ody = dy > 0.0f ? dy : 0.0f;
				if (dx > 0.0f || dy > 0.0f)
					dH = PX_sqrt(odx * odx + ody * ody);
				else
					dH = (dx > dy ? dx : dy);
			}
			// SDF of vertical bar: box [-halfW, -halfSize] to [halfW, halfSize]
			{
				px_float dx = ax - halfW;
				px_float dy = ay - halfSize;
				px_float odx = dx > 0.0f ? dx : 0.0f;
				px_float ody = dy > 0.0f ? dy : 0.0f;
				if (dx > 0.0f || dy > 0.0f)
					dV = PX_sqrt(odx * odx + ody * ody);
				else
					dV = (dx > dy ? dx : dy);
			}

			// union
			d = (dH < dV) ? dH : dV;

			if (d > aa) continue;

			if (d < -aa)
				cov = 1.0f;
			else
				cov = (aa - d) / (2.0f * aa);

			if (cov <= 0.0f) continue;
			if (cov > 1.0f) cov = 1.0f;

			c = color;
			c._argb.a = (px_byte)(color._argb.a * cov + 0.5f);
			if (c._argb.a)
				PX_SurfaceDrawPixel(psurface, ix, iy, c);
		}
	}
}

px_void PX_GeoDrawHeart(px_surface *psurface,px_float x,px_float y,px_float size,px_color color)
{
	// Generate heart polygon from parametric curve:
	// x(t) = sin(t)^3
	// y(t) = (13cos(t) - 5cos(2t) - 2cos(3t) - cos(4t)) / 16
	// t goes from 0 to 2*PI, 64 vertices
	px_float vx[64], vy[64];
	px_float aa, maxR;
	px_int ix0, iy0, ix1, iy1, ix, iy, i;
	px_float step;

	if (size < 1.0f) return;
	if (color._argb.a == 0) return;

	aa = 0.7f;
	step = 2.0f * (px_float)PX_PI / 64.0f;

	for (i = 0; i < 64; i++)
	{
		px_float t = i * step;
		px_float st = PX_sin_radian(t);
		vx[i] = x + size * st * st * st;
		vy[i] = y - size * (13.0f * PX_cos_radian(t) - 5.0f * PX_cos_radian(2.0f * t) - 2.0f * PX_cos_radian(3.0f * t) - PX_cos_radian(4.0f * t)) / 16.0f;
	}

	maxR = size * 1.2f + aa + 1.0f;
	ix0 = (px_int)PX_floor(x - maxR);
	iy0 = (px_int)PX_floor(y - maxR);
	ix1 = (px_int)PX_ceil(x + maxR);
	iy1 = (px_int)PX_ceil(y + maxR);

	if (ix0 < psurface->limit_left) ix0 = psurface->limit_left;
	if (iy0 < psurface->limit_top) iy0 = psurface->limit_top;
	if (ix1 > psurface->limit_right) ix1 = psurface->limit_right;
	if (iy1 > psurface->limit_bottom) iy1 = psurface->limit_bottom;

	for (iy = iy0; iy <= iy1; ++iy)
	{
		px_float cy = iy + 0.5f;
		for (ix = ix0; ix <= ix1; ++ix)
		{
			px_float cx = ix + 0.5f;
			px_float d, cov;
			px_color c;

			d = px_sd_polygon(cx, cy, vx, vy, 64);

			if (d > aa) continue;

			if (d < -aa)
				cov = 1.0f;
			else
				cov = (aa - d) / (2.0f * aa);

			if (cov <= 0.0f) continue;
			if (cov > 1.0f) cov = 1.0f;

			c = color;
			c._argb.a = (px_byte)(color._argb.a * cov + 0.5f);
			if (c._argb.a)
				PX_SurfaceDrawPixel(psurface, ix, iy, c);
		}
	}
}

px_void PX_GeoDrawCrescent(px_surface *psurface,px_float x,px_float y,px_float radius,px_float offset,px_float angle,px_color color)
{
	px_float aa, cosA, sinA;
	px_float maxR;
	px_float minx, miny, maxx, maxy;
	px_int ix0, iy0, ix1, iy1, ix, iy;

	if (radius < 1.0f) return;
	if (color._argb.a == 0) return;

	aa = 0.7f;
	cosA = PX_cos_radian((px_float)PX_PI * angle / 180.0f);
	sinA = PX_sin_radian((px_float)PX_PI * angle / 180.0f);

	maxR = radius + PX_ABS(offset) + aa + 1.0f;
	minx = x - maxR; miny = y - maxR;
	maxx = x + maxR; maxy = y + maxR;

	ix0 = (px_int)PX_floor(minx);
	iy0 = (px_int)PX_floor(miny);
	ix1 = (px_int)PX_ceil(maxx);
	iy1 = (px_int)PX_ceil(maxy);

	if (ix0 < psurface->limit_left) ix0 = psurface->limit_left;
	if (iy0 < psurface->limit_top) iy0 = psurface->limit_top;
	if (ix1 > psurface->limit_right) ix1 = psurface->limit_right;
	if (iy1 > psurface->limit_bottom) iy1 = psurface->limit_bottom;

	for (iy = iy0; iy <= iy1; ++iy)
	{
		px_float cy = iy + 0.5f - y;
		for (ix = ix0; ix <= ix1; ++ix)
		{
			px_float cx = ix + 0.5f - x;
			px_float lx, ly;
			px_float d1, d2, d, cov;
			px_color c;

			// rotate into local frame (crescent opens along +x direction)
			lx = cx * cosA + cy * sinA;
			ly = -cx * sinA + cy * cosA;

			// outer circle: centered at origin
			d1 = PX_sqrt(lx * lx + ly * ly) - radius;

			// inner circle (subtracted): shifted by offset along local x
			d2 = PX_sqrt((lx - offset) * (lx - offset) + ly * ly) - radius;

			// difference: outer minus inner => max(d1, -d2)
			d = (d1 > -d2) ? d1 : -d2;

			if (d > aa) continue;

			if (d < -aa)
				cov = 1.0f;
			else
				cov = (aa - d) / (2.0f * aa);

			if (cov <= 0.0f) continue;
			if (cov > 1.0f) cov = 1.0f;

			c = color;
			c._argb.a = (px_byte)(color._argb.a * cov + 0.5f);
			if (c._argb.a)
				PX_SurfaceDrawPixel(psurface, ix, iy, c);
		}
	}
}

px_void PX_GeoDrawGear(px_surface *psurface,px_float x,px_float y,px_float outerRadius,px_float innerRadius,px_int teeth,px_float toothDepth,px_color color)
{
	px_float vx[256], vy[256];
	px_float aa, stepRad, halfTooth;
	px_float rOuter, rInner;
	px_float minx, miny, maxx, maxy;
	px_int ix0, iy0, ix1, iy1, ix, iy, i, vertexCount;

	if (teeth < 3) teeth = 3;
	if (teeth > 64) teeth = 64;
	if (outerRadius < 2.0f) return;
	if (color._argb.a == 0) return;

	aa = 0.7f;
	rOuter = outerRadius;
	rInner = outerRadius - toothDepth;
	if (rInner < 1.0f) rInner = 1.0f;

	stepRad = 2.0f * (px_float)PX_PI / teeth;
	halfTooth = stepRad * 0.25f;

	// each tooth: 4 vertices (inner-left, outer-left, outer-right, inner-right)
	vertexCount = teeth * 4;
	if (vertexCount > 256) return;

	for (i = 0; i < teeth; i++)
	{
		px_float baseAngle = i * stepRad - (px_float)PX_PI / 2.0f;
		px_int idx = i * 4;

		vx[idx]     = x + rInner * PX_cos_radian(baseAngle - halfTooth * 2.0f);
		vy[idx]     = y + rInner * PX_sin_radian(baseAngle - halfTooth * 2.0f);
		vx[idx + 1] = x + rOuter * PX_cos_radian(baseAngle - halfTooth);
		vy[idx + 1] = y + rOuter * PX_sin_radian(baseAngle - halfTooth);
		vx[idx + 2] = x + rOuter * PX_cos_radian(baseAngle + halfTooth);
		vy[idx + 2] = y + rOuter * PX_sin_radian(baseAngle + halfTooth);
		vx[idx + 3] = x + rInner * PX_cos_radian(baseAngle + halfTooth * 2.0f);
		vy[idx + 3] = y + rInner * PX_sin_radian(baseAngle + halfTooth * 2.0f);
	}

	// outer gear polygon SDF then subtract inner hole
	minx = x - rOuter - aa - 1.0f;
	miny = y - rOuter - aa - 1.0f;
	maxx = x + rOuter + aa + 1.0f;
	maxy = y + rOuter + aa + 1.0f;

	ix0 = (px_int)PX_floor(minx);
	iy0 = (px_int)PX_floor(miny);
	ix1 = (px_int)PX_ceil(maxx);
	iy1 = (px_int)PX_ceil(maxy);

	if (ix0 < psurface->limit_left) ix0 = psurface->limit_left;
	if (iy0 < psurface->limit_top) iy0 = psurface->limit_top;
	if (ix1 > psurface->limit_right) ix1 = psurface->limit_right;
	if (iy1 > psurface->limit_bottom) iy1 = psurface->limit_bottom;

	for (iy = iy0; iy <= iy1; ++iy)
	{
		px_float cy = iy + 0.5f;
		for (ix = ix0; ix <= ix1; ++ix)
		{
			px_float cx = ix + 0.5f;
			px_float dOuter, dHole, d, cov;
			px_color c;

			// outer gear polygon
			dOuter = px_sd_polygon(cx, cy, vx, vy, vertexCount);

			// inner hole circle
			dHole = PX_sqrt((cx - x) * (cx - x) + (cy - y) * (cy - y)) - innerRadius;

			// difference: outer gear minus inner hole => max(dOuter, -dHole)
			d = (dOuter > -dHole) ? dOuter : -dHole;

			if (d > aa) continue;

			if (d < -aa)
				cov = 1.0f;
			else
				cov = (aa - d) / (2.0f * aa);

			if (cov <= 0.0f) continue;
			if (cov > 1.0f) cov = 1.0f;

			c = color;
			c._argb.a = (px_byte)(color._argb.a * cov + 0.5f);
			if (c._argb.a)
				PX_SurfaceDrawPixel(psurface, ix, iy, c);
		}
	}
}

px_void PX_GeoDrawRoundPolygon(px_surface *psurface,px_float vx[],px_float vy[],px_int n,px_float roundRadius,px_color color)
{
	px_float aa;
	px_float minx, miny, maxx, maxy;
	px_int ix0, iy0, ix1, iy1, ix, iy, i;

	if (n < 3 || n > 256) return;
	if (roundRadius < 0.0f) roundRadius = 0.0f;
	if (color._argb.a == 0) return;

	aa = 0.7f;

	// compute bounding box from vertices
	minx = vx[0]; maxx = vx[0];
	miny = vy[0]; maxy = vy[0];
	for (i = 1; i < n; i++)
	{
		if (vx[i] < minx) minx = vx[i];
		if (vx[i] > maxx) maxx = vx[i];
		if (vy[i] < miny) miny = vy[i];
		if (vy[i] > maxy) maxy = vy[i];
	}
	minx -= roundRadius + aa + 1.0f;
	miny -= roundRadius + aa + 1.0f;
	maxx += roundRadius + aa + 1.0f;
	maxy += roundRadius + aa + 1.0f;

	ix0 = (px_int)PX_floor(minx);
	iy0 = (px_int)PX_floor(miny);
	ix1 = (px_int)PX_ceil(maxx);
	iy1 = (px_int)PX_ceil(maxy);

	if (ix0 < psurface->limit_left) ix0 = psurface->limit_left;
	if (iy0 < psurface->limit_top) iy0 = psurface->limit_top;
	if (ix1 > psurface->limit_right) ix1 = psurface->limit_right;
	if (iy1 > psurface->limit_bottom) iy1 = psurface->limit_bottom;

	for (iy = iy0; iy <= iy1; ++iy)
	{
		px_float cy = iy + 0.5f;
		for (ix = ix0; ix <= ix1; ++ix)
		{
			px_float cx = ix + 0.5f;
			px_float d, cov;
			px_color c;

			// polygon SDF minus round radius = rounded polygon
			d = px_sd_polygon(cx, cy, vx, vy, n) - roundRadius;

			if (d > aa) continue;

			if (d < -aa)
				cov = 1.0f;
			else
				cov = (aa - d) / (2.0f * aa);

			if (cov <= 0.0f) continue;
			if (cov > 1.0f) cov = 1.0f;

			c = color;
			c._argb.a = (px_byte)(color._argb.a * cov + 0.5f);
			if (c._argb.a)
				PX_SurfaceDrawPixel(psurface, ix, iy, c);
		}
	}
}

px_void PX_GeoDrawCheckMark(px_surface *psurface,px_float x,px_float y,px_float size,px_float linewidth,px_color color)
{
	px_float halfW;
	px_float x0, y0, x1, y1, x2, y2;
	px_float d0, d1, d, aa, cov;
	px_float minx, miny, maxx, maxy;
	px_int ix0, iy0, ix1, iy1, ix, iy;

	if (size < 2.0f || linewidth < 0.5f) return;
	if (color._argb.a == 0) return;

	halfW = linewidth * 0.5f;
	aa = 0.7f;

	// check mark shape: two line segments
	// left leg: from bottom-left going to bottom-center
	// right leg: from bottom-center going to top-right
	x0 = x - size * 0.4f;
	y0 = y - size * 0.05f;
	x1 = x - size * 0.1f;
	y1 = y + size * 0.35f;
	x2 = x + size * 0.45f;
	y2 = y - size * 0.4f;

	minx = x0; maxx = x0;
	miny = y0; maxy = y0;
	if (x1 < minx) minx = x1; if (x1 > maxx) maxx = x1;
	if (y1 < miny) miny = y1; if (y1 > maxy) maxy = y1;
	if (x2 < minx) minx = x2; if (x2 > maxx) maxx = x2;
	if (y2 < miny) miny = y2; if (y2 > maxy) maxy = y2;
	minx -= halfW + aa + 1.0f; miny -= halfW + aa + 1.0f;
	maxx += halfW + aa + 1.0f; maxy += halfW + aa + 1.0f;

	ix0 = (px_int)PX_floor(minx);
	iy0 = (px_int)PX_floor(miny);
	ix1 = (px_int)PX_ceil(maxx);
	iy1 = (px_int)PX_ceil(maxy);

	if (ix0 < psurface->limit_left) ix0 = psurface->limit_left;
	if (iy0 < psurface->limit_top) iy0 = psurface->limit_top;
	if (ix1 > psurface->limit_right) ix1 = psurface->limit_right;
	if (iy1 > psurface->limit_bottom) iy1 = psurface->limit_bottom;

	for (iy = iy0; iy <= iy1; ++iy)
	{
		px_float cy = iy + 0.5f;
		for (ix = ix0; ix <= ix1; ++ix)
		{
			px_float cx = ix + 0.5f;
			px_color c;

			// distance to two segments, take min (union of two capsules)
			d0 = PX_sqrt(px_dist2_point_to_segment(cx, cy, x0, y0, x1, y1)) - halfW;
			d1 = PX_sqrt(px_dist2_point_to_segment(cx, cy, x1, y1, x2, y2)) - halfW;
			d = (d0 < d1) ? d0 : d1;

			if (d > aa) continue;

			if (d < -aa)
				cov = 1.0f;
			else
				cov = (aa - d) / (2.0f * aa);

			if (cov <= 0.0f) continue;
			if (cov > 1.0f) cov = 1.0f;

			c = color;
			c._argb.a = (px_byte)(color._argb.a * cov + 0.5f);
			if (c._argb.a)
				PX_SurfaceDrawPixel(psurface, ix, iy, c);
		}
	}
}

px_void PX_GeoDrawCrossMark(px_surface *psurface,px_float x,px_float y,px_float size,px_float linewidth,px_color color)
{
	px_float halfW, halfS;
	px_float d0, d1, d, aa, cov;
	px_float minx, miny, maxx, maxy;
	px_int ix0, iy0, ix1, iy1, ix, iy;

	if (size < 2.0f || linewidth < 0.5f) return;
	if (color._argb.a == 0) return;

	halfW = linewidth * 0.5f;
	halfS = size * 0.5f;
	aa = 0.7f;

	minx = x - halfS - halfW - aa - 1.0f;
	miny = y - halfS - halfW - aa - 1.0f;
	maxx = x + halfS + halfW + aa + 1.0f;
	maxy = y + halfS + halfW + aa + 1.0f;

	ix0 = (px_int)PX_floor(minx);
	iy0 = (px_int)PX_floor(miny);
	ix1 = (px_int)PX_ceil(maxx);
	iy1 = (px_int)PX_ceil(maxy);

	if (ix0 < psurface->limit_left) ix0 = psurface->limit_left;
	if (iy0 < psurface->limit_top) iy0 = psurface->limit_top;
	if (ix1 > psurface->limit_right) ix1 = psurface->limit_right;
	if (iy1 > psurface->limit_bottom) iy1 = psurface->limit_bottom;

	for (iy = iy0; iy <= iy1; ++iy)
	{
		px_float cy = iy + 0.5f;
		for (ix = ix0; ix <= ix1; ++ix)
		{
			px_float cx = ix + 0.5f;
			px_color c;

			// two diagonal segments
			d0 = PX_sqrt(px_dist2_point_to_segment(cx, cy, x - halfS, y - halfS, x + halfS, y + halfS)) - halfW;
			d1 = PX_sqrt(px_dist2_point_to_segment(cx, cy, x - halfS, y + halfS, x + halfS, y - halfS)) - halfW;
			d = (d0 < d1) ? d0 : d1;

			if (d > aa) continue;

			if (d < -aa)
				cov = 1.0f;
			else
				cov = (aa - d) / (2.0f * aa);

			if (cov <= 0.0f) continue;
			if (cov > 1.0f) cov = 1.0f;

			c = color;
			c._argb.a = (px_byte)(color._argb.a * cov + 0.5f);
			if (c._argb.a)
				PX_SurfaceDrawPixel(psurface, ix, iy, c);
		}
	}
}

px_void PX_GeoDrawDiamond(px_surface *psurface,px_float x,px_float y,px_float width,px_float height,px_color color)
{
	px_float vx[4], vy[4];
	px_float aa;
	px_float halfW, halfH;
	px_float minx, miny, maxx, maxy;
	px_int ix0, iy0, ix1, iy1, ix, iy;

	if (width < 1.0f || height < 1.0f) return;
	if (color._argb.a == 0) return;

	halfW = width * 0.5f;
	halfH = height * 0.5f;
	aa = 0.7f;

	// 4 vertices: top, right, bottom, left
	vx[0] = x;          vy[0] = y - halfH;
	vx[1] = x + halfW;  vy[1] = y;
	vx[2] = x;          vy[2] = y + halfH;
	vx[3] = x - halfW;  vy[3] = y;

	minx = x - halfW - aa - 1.0f;
	miny = y - halfH - aa - 1.0f;
	maxx = x + halfW + aa + 1.0f;
	maxy = y + halfH + aa + 1.0f;

	ix0 = (px_int)PX_floor(minx);
	iy0 = (px_int)PX_floor(miny);
	ix1 = (px_int)PX_ceil(maxx);
	iy1 = (px_int)PX_ceil(maxy);

	if (ix0 < psurface->limit_left) ix0 = psurface->limit_left;
	if (iy0 < psurface->limit_top) iy0 = psurface->limit_top;
	if (ix1 > psurface->limit_right) ix1 = psurface->limit_right;
	if (iy1 > psurface->limit_bottom) iy1 = psurface->limit_bottom;

	for (iy = iy0; iy <= iy1; ++iy)
	{
		px_float cy = iy + 0.5f;
		for (ix = ix0; ix <= ix1; ++ix)
		{
			px_float cx = ix + 0.5f;
			px_float d, cov;
			px_color c;

			d = px_sd_polygon(cx, cy, vx, vy, 4);

			if (d > aa) continue;

			if (d < -aa)
				cov = 1.0f;
			else
				cov = (aa - d) / (2.0f * aa);

			if (cov <= 0.0f) continue;
			if (cov > 1.0f) cov = 1.0f;

			c = color;
			c._argb.a = (px_byte)(color._argb.a * cov + 0.5f);
			if (c._argb.a)
				PX_SurfaceDrawPixel(psurface, ix, iy, c);
		}
	}
}

px_void PX_GeoDrawStrokeHeart(px_surface *psurface,px_float x,px_float y,px_float size,px_float linewidth,px_color color)
{
	px_float vx[64], vy[64];
	px_float aa, maxR, halfW, step;
	px_int ix0, iy0, ix1, iy1, ix, iy, i;

	if (size < 1.0f || linewidth < 0.5f) return;
	if (color._argb.a == 0) return;

	aa = 0.7f;
	halfW = linewidth * 0.5f;
	step = 2.0f * (px_float)PX_PI / 64.0f;

	for (i = 0; i < 64; i++)
	{
		px_float t = i * step;
		px_float st = PX_sin_radian(t);
		vx[i] = x + size * st * st * st;
		vy[i] = y - size * (13.0f * PX_cos_radian(t) - 5.0f * PX_cos_radian(2.0f * t) - 2.0f * PX_cos_radian(3.0f * t) - PX_cos_radian(4.0f * t)) / 16.0f;
	}

	maxR = size * 1.2f + halfW + aa + 1.0f;
	ix0 = (px_int)PX_floor(x - maxR); iy0 = (px_int)PX_floor(y - maxR);
	ix1 = (px_int)PX_ceil(x + maxR);  iy1 = (px_int)PX_ceil(y + maxR);
	if (ix0 < psurface->limit_left) ix0 = psurface->limit_left;
	if (iy0 < psurface->limit_top) iy0 = psurface->limit_top;
	if (ix1 > psurface->limit_right) ix1 = psurface->limit_right;
	if (iy1 > psurface->limit_bottom) iy1 = psurface->limit_bottom;

	for (iy = iy0; iy <= iy1; ++iy)
	{
		px_float cy = iy + 0.5f;
		for (ix = ix0; ix <= ix1; ++ix)
		{
			px_float cx = ix + 0.5f;
			px_float d, cov;
			px_color c;
			d = PX_ABS(px_sd_polygon(cx, cy, vx, vy, 64)) - halfW;
			if (d > aa) continue;
			cov = (d < -aa) ? 1.0f : (aa - d) / (2.0f * aa);
			if (cov <= 0.0f) continue;
			if (cov > 1.0f) cov = 1.0f;
			c = color;
			c._argb.a = (px_byte)(color._argb.a * cov + 0.5f);
			if (c._argb.a) PX_SurfaceDrawPixel(psurface, ix, iy, c);
		}
	}
}

px_void PX_GeoDrawStrokeStar(px_surface *psurface,px_float x,px_float y,px_float radius,px_int n,px_float inside_angle,px_float angle,px_float linewidth,px_color color)
{
	px_float vx[64], vy[64];
	px_float innerR, aa, halfW, angleRad, stepRad, halfTipRad;
	px_float maxR;
	px_int ix0, iy0, ix1, iy1, ix, iy, i, vertexCount;

	if (n < 3) n = 3;
	if (n > 32) n = 32;
	if (radius < 1.0f || linewidth < 0.5f) return;
	if (color._argb.a == 0) return;
	if (inside_angle <= 0.0f) inside_angle = 1.0f;
	if (inside_angle >= 180.0f) inside_angle = 179.0f;

	aa = 0.7f;
	halfW = linewidth * 0.5f;
	vertexCount = n * 2;
	stepRad = 2.0f * (px_float)PX_PI / n;
	halfTipRad = (px_float)PX_PI * inside_angle / 360.0f;
	innerR = radius * PX_sin_radian(halfTipRad) / PX_sin_radian(halfTipRad + stepRad * 0.5f);
	if (innerR < 0.0f) innerR = 0.0f;
	angleRad = (px_float)PX_PI * angle / 180.0f - (px_float)PX_PI / 2.0f;

	for (i = 0; i < n; i++)
	{
		px_float outerA = angleRad + i * stepRad;
		px_float innerA = outerA + stepRad * 0.5f;
		vx[i * 2]     = x + radius * PX_cos_radian(outerA);
		vy[i * 2]     = y + radius * PX_sin_radian(outerA);
		vx[i * 2 + 1] = x + innerR * PX_cos_radian(innerA);
		vy[i * 2 + 1] = y + innerR * PX_sin_radian(innerA);
	}

	maxR = radius + halfW + aa + 1.0f;
	ix0 = (px_int)PX_floor(x - maxR); iy0 = (px_int)PX_floor(y - maxR);
	ix1 = (px_int)PX_ceil(x + maxR);  iy1 = (px_int)PX_ceil(y + maxR);
	if (ix0 < psurface->limit_left) ix0 = psurface->limit_left;
	if (iy0 < psurface->limit_top) iy0 = psurface->limit_top;
	if (ix1 > psurface->limit_right) ix1 = psurface->limit_right;
	if (iy1 > psurface->limit_bottom) iy1 = psurface->limit_bottom;

	for (iy = iy0; iy <= iy1; ++iy)
	{
		px_float cy = iy + 0.5f;
		for (ix = ix0; ix <= ix1; ++ix)
		{
			px_float cx = ix + 0.5f;
			px_float d, cov;
			px_color c;
			d = PX_ABS(px_sd_polygon(cx, cy, vx, vy, vertexCount)) - halfW;
			if (d > aa) continue;
			cov = (d < -aa) ? 1.0f : (aa - d) / (2.0f * aa);
			if (cov <= 0.0f) continue;
			if (cov > 1.0f) cov = 1.0f;
			c = color;
			c._argb.a = (px_byte)(color._argb.a * cov + 0.5f);
			if (c._argb.a) PX_SurfaceDrawPixel(psurface, ix, iy, c);
		}
	}
}

px_void PX_GeoDrawStrokeGear(px_surface *psurface,px_float x,px_float y,px_float outerRadius,px_float innerRadius,px_int teeth,px_float toothDepth,px_float linewidth,px_color color)
{
	px_float vx[256], vy[256];
	px_float aa, halfW, stepRad, halfTooth, rOuter, rInner;
	px_int ix0, iy0, ix1, iy1, ix, iy, i, vertexCount;

	if (teeth < 3) teeth = 3;
	if (teeth > 64) teeth = 64;
	if (outerRadius < 2.0f || linewidth < 0.5f) return;
	if (color._argb.a == 0) return;

	aa = 0.7f;
	halfW = linewidth * 0.5f;
	rOuter = outerRadius;
	rInner = outerRadius - toothDepth;
	if (rInner < 1.0f) rInner = 1.0f;
	stepRad = 2.0f * (px_float)PX_PI / teeth;
	halfTooth = stepRad * 0.25f;
	vertexCount = teeth * 4;
	if (vertexCount > 256) return;

	for (i = 0; i < teeth; i++)
	{
		px_float baseAngle = i * stepRad - (px_float)PX_PI / 2.0f;
		px_int idx = i * 4;
		vx[idx]     = x + rInner * PX_cos_radian(baseAngle - halfTooth * 2.0f);
		vy[idx]     = y + rInner * PX_sin_radian(baseAngle - halfTooth * 2.0f);
		vx[idx + 1] = x + rOuter * PX_cos_radian(baseAngle - halfTooth);
		vy[idx + 1] = y + rOuter * PX_sin_radian(baseAngle - halfTooth);
		vx[idx + 2] = x + rOuter * PX_cos_radian(baseAngle + halfTooth);
		vy[idx + 2] = y + rOuter * PX_sin_radian(baseAngle + halfTooth);
		vx[idx + 3] = x + rInner * PX_cos_radian(baseAngle + halfTooth * 2.0f);
		vy[idx + 3] = y + rInner * PX_sin_radian(baseAngle + halfTooth * 2.0f);
	}

	{
		px_float maxR = rOuter + halfW + aa + 1.0f;
		ix0 = (px_int)PX_floor(x - maxR); iy0 = (px_int)PX_floor(y - maxR);
		ix1 = (px_int)PX_ceil(x + maxR);  iy1 = (px_int)PX_ceil(y + maxR);
	}
	if (ix0 < psurface->limit_left) ix0 = psurface->limit_left;
	if (iy0 < psurface->limit_top) iy0 = psurface->limit_top;
	if (ix1 > psurface->limit_right) ix1 = psurface->limit_right;
	if (iy1 > psurface->limit_bottom) iy1 = psurface->limit_bottom;

	for (iy = iy0; iy <= iy1; ++iy)
	{
		px_float cy = iy + 0.5f;
		for (ix = ix0; ix <= ix1; ++ix)
		{
			px_float cx = ix + 0.5f;
			px_float dOuter, dHole, dGear, d, cov;
			px_color c;

			dOuter = px_sd_polygon(cx, cy, vx, vy, vertexCount);
			dHole = PX_sqrt((cx - x) * (cx - x) + (cy - y) * (cy - y)) - innerRadius;
			dGear = (dOuter > -dHole) ? dOuter : -dHole;
			d = PX_ABS(dGear) - halfW;

			if (d > aa) continue;
			cov = (d < -aa) ? 1.0f : (aa - d) / (2.0f * aa);
			if (cov <= 0.0f) continue;
			if (cov > 1.0f) cov = 1.0f;
			c = color;
			c._argb.a = (px_byte)(color._argb.a * cov + 0.5f);
			if (c._argb.a) PX_SurfaceDrawPixel(psurface, ix, iy, c);
		}
	}
}

px_void PX_GeoDrawStrokeDiamond(px_surface *psurface,px_float x,px_float y,px_float width,px_float height,px_float linewidth,px_color color)
{
	px_float vx[4], vy[4];
	px_float aa, halfW, halfWd, halfH;
	px_float maxR;
	px_int ix0, iy0, ix1, iy1, ix, iy;

	if (width < 1.0f || height < 1.0f || linewidth < 0.5f) return;
	if (color._argb.a == 0) return;

	aa = 0.7f;
	halfW = linewidth * 0.5f;
	halfWd = width * 0.5f;
	halfH = height * 0.5f;

	vx[0] = x;          vy[0] = y - halfH;
	vx[1] = x + halfWd; vy[1] = y;
	vx[2] = x;          vy[2] = y + halfH;
	vx[3] = x - halfWd; vy[3] = y;

	maxR = (halfWd > halfH ? halfWd : halfH) + halfW + aa + 1.0f;
	ix0 = (px_int)PX_floor(x - maxR); iy0 = (px_int)PX_floor(y - maxR);
	ix1 = (px_int)PX_ceil(x + maxR);  iy1 = (px_int)PX_ceil(y + maxR);
	if (ix0 < psurface->limit_left) ix0 = psurface->limit_left;
	if (iy0 < psurface->limit_top) iy0 = psurface->limit_top;
	if (ix1 > psurface->limit_right) ix1 = psurface->limit_right;
	if (iy1 > psurface->limit_bottom) iy1 = psurface->limit_bottom;

	for (iy = iy0; iy <= iy1; ++iy)
	{
		px_float cy = iy + 0.5f;
		for (ix = ix0; ix <= ix1; ++ix)
		{
			px_float cx = ix + 0.5f;
			px_float d, cov;
			px_color c;
			d = PX_ABS(px_sd_polygon(cx, cy, vx, vy, 4)) - halfW;
			if (d > aa) continue;
			cov = (d < -aa) ? 1.0f : (aa - d) / (2.0f * aa);
			if (cov <= 0.0f) continue;
			if (cov > 1.0f) cov = 1.0f;
			c = color;
			c._argb.a = (px_byte)(color._argb.a * cov + 0.5f);
			if (c._argb.a) PX_SurfaceDrawPixel(psurface, ix, iy, c);
		}
	}
}

static px_point2D PX_GeoDeBoorCoxRecursion(px_int j,  px_int i, px_int k, px_float t, px_point2D controlPoints[], px_float knots[])
{
	px_point2D p1, p2;

	if (j == 0)
		return controlPoints[i];
	else
	{
		px_float param = (t - knots[i]) / (knots[i + k - j] - knots[i]);
		p1 = PX_GeoDeBoorCoxRecursion(j - 1,  i - 1, k, t, controlPoints, knots);
		p1 = PX_Point2DMul(p1, (1 - param));

		p2 = PX_GeoDeBoorCoxRecursion(j - 1,  i, k, t, controlPoints, knots);
		p2 = PX_Point2DMul(p2, param);

		return PX_Point2DAdd(p1,p2);
	}
}

px_void PX_GeoBSpline3(px_point2D controlPoints[4], px_point2D _samples[], px_int samplescount)
{
	px_float knots[] = { 0,0,0,1,2,2,2 };
	px_float step = 2.f / (samplescount);
	px_float t;
	px_point2D point;
	px_int i;
	for (i = 0; i < samplescount; i++)
	{
		t =  i * step;	
		point = PX_GeoDeBoorCoxRecursion(2,  2 + (px_int)t, 3, t, controlPoints, knots);
		_samples[i]=(point);
	}
}

px_void PX_GeoDrawPenSamplesLine(px_surface* psurface, px_point _samples[], px_int samplescount,px_float radius,px_color color,px_float v,px_float filter_factor)
{
	px_int i,dindex=1;
	px_point pen = { 0 }, guider = {0};
	px_float in_factor;
	if (samplescount==0)
	{
		return;
	}

	if (samplescount==1)
	{
		px_color clr = color;
		px_int a = (px_int)(clr._argb.a * _samples[0].z);
		a = a > 255?255 : a;
		clr._argb.a = (px_byte)(a);
		PX_GeoDrawPenCircle(psurface, _samples[0].x, _samples[0].y, radius, clr);
		return;
	}

	pen.x = _samples[0].x;
	pen.y = _samples[0].y;
	pen.z = _samples[0].z;

	guider = pen;

	while(PX_TRUE)
	{
		px_point dir;
		px_float d;
		px_float step_distance = v;
		in_factor = filter_factor;
		if (dindex >= samplescount - 1)
		{
			if (pen.x == _samples[dindex].x && pen.y == _samples[dindex].y)
			{
				break;
			}
		}

		while (step_distance>0)
		{
			if (dindex >= samplescount - 1)
			{
				break;
			}

			dir = PX_POINT(_samples[dindex].x - guider.x, _samples[dindex].y - guider.y, _samples[dindex].z - guider.z);
			d = PX_sqrt(dir.x * dir.x + dir.y * dir.y);

			if (d <= step_distance)
			{
				guider.x = _samples[dindex].x;
				guider.y = _samples[dindex].y;
				guider.z= _samples[dindex].z;
				
				if (dindex < samplescount - 1)
				{
					px_point2D v2[2];
					px_float md[2];
					px_float v_cos;
					v2[0].x= _samples[dindex - 1].x - _samples[dindex].x;
					v2[0].y = _samples[dindex - 1].y - _samples[dindex].y;

					v2[1].x = _samples[dindex + 1].x - _samples[dindex].x;
					v2[1].y = _samples[dindex + 1].y - _samples[dindex].y;

					md[0] = PX_Point2DMod(v2[0]);
					md[1] = PX_Point2DMod(v2[1]);
					if (md[0]<v/5||md[1]<v/5)
					{
						step_distance = 0;
						in_factor = 1;
					}
					else if (md[0]*md[1])
					{
						v_cos = PX_Point2DDot(v2[0], v2[1]) /(md[0]*md[1]);
						if (v_cos>-0.5)
						{
							step_distance = 0;
							in_factor = 1;
						}
						else
						{
							step_distance -= d;
						}
						
					}
					else
					{
						step_distance = 0;
					}
					dindex++;
				}
				else
				{
					step_distance = 0;
					break;
				}
			}
			else
			{
				px_point inc = PX_PointNormalization(dir);
				inc = PX_PointMul(inc, v);
				guider = PX_PointAdd(guider, inc);
				step_distance = 0;
				break;
			}
		}

		
		//draw pen circle
		dir = PX_POINT(guider.x-pen.x,guider.y-pen.y, guider.z - pen.z);
		d = PX_sqrt(dir.x * dir.x + dir.y * dir.y)* in_factor;
		dir = PX_PointNormalization(dir);
		if (d>=1)
		{
			px_int dstep=(px_int)d;
			for (i=0;i<dstep;i++)
			{
				px_color clr = color;
				px_int a = (px_int)(clr._argb.a * pen.z);
				a = a > 255 ? 255 : a;
				clr._argb.a = (px_byte)(a);
				PX_GeoDrawPenCircle(psurface, pen.x, pen.y, radius, clr);
				pen.x += dir.x;
				pen.y += dir.y;
				pen.z += dir.z;
			}
		}
		else
		{
			if (dindex >= samplescount - 1)
			{
				px_color clr = color;
				px_int a = (px_int)(clr._argb.a * pen.z);
				a = a > 255 ? 255 : a;
				clr._argb.a = (px_byte)(a);

				//finish the last
				dir = PX_POINT(guider.x - pen.x, guider.y - pen.y, guider.z - pen.z);
				d = (PX_sqrt(dir.x * dir.x + dir.y * dir.y));
				dir = PX_PointNormalization(dir);
				for (i = 0; i < PX_APO(d); i++)
				{
					px_color clr = color;
					px_int a = (px_int)(clr._argb.a * pen.z);
					a = a > 255 ? 255 : a;
					clr._argb.a = (px_byte)(a);
					PX_GeoDrawPenCircle(psurface, pen.x, pen.y, radius, clr);
					pen.x += dir.x;
					pen.y += dir.y;
					pen.z += dir.z;
				}

				pen = _samples[dindex];
				PX_GeoDrawPenCircle(psurface, pen.x, pen.y, radius, clr);
			}
		}

	}
}


px_void PX_GeoDrawRightTriangle(px_surface* psurface, px_float left, px_float top, px_float right, px_float bottom, px_color color)
{
	//y1=k1x+b1;
	//y2=k2x+b2;
	px_int i, j;
	px_float mid;
	px_float k1 = -((bottom - top) / 2) / (right - left);
	px_float b1 = bottom - k1 * left;
	px_float k2 = -k1;
	px_float b2 = top - k2 * left;
	px_float inv_len1 = 1.0f / PX_sqrt(k1 * k1 + 1);
	px_float inv_len2 = 1.0f / PX_sqrt(k2 * k2 + 1);
	if (color._argb.a == 0)
	{
		return;
	}

	if (left > right)
	{
		mid = left;
		left = right;
		right = mid;
	}
	if (top > bottom)
	{
		mid = top;
		top = bottom;
		bottom = mid;
	}

	if (left < psurface->limit_left)
	{
		left = (px_float)psurface->limit_left;
	}
	else if (left > psurface->limit_right)
	{
		return;
	}

	if (top < psurface->limit_top)
	{
		top = (px_float)psurface->limit_top;
	}
	else if (top > psurface->limit_bottom)
	{
		return;
	}

	if (right > psurface->limit_right)
	{
		right = (px_float)psurface->limit_right;
	}
	else if (right < psurface->limit_left)
	{
		return;
	}

	if (bottom > psurface->limit_bottom)
	{
		bottom = (px_float)psurface->limit_bottom;
	}
	else if (bottom < psurface->limit_top)
	{
		return;
	}

	//line 1
	for (j = (px_int)top; j <= (px_int)bottom; j++)
	{
		for (i = (px_int)left; i <= (px_int)right; i++)
		{
			if (j < (bottom+top) / 2)
			{
				// signed distance from pixel center to upper edge line (positive = inside triangle)
				px_float dist = ((j + 0.5f) - (k2 * (i + 0.5f) + b2)) * inv_len2;
				px_float alpha = dist + 0.5f;
				if (alpha > 0)
				{
					px_color drawColor = color;
					alpha = alpha > 1 ? 1 : alpha;
					drawColor._argb.a = (px_byte)(drawColor._argb.a * alpha);
					PX_SurfaceDrawPixelWithoutLimit(psurface, i, j, drawColor);
				}
				else
				{
					break;
				}

			}
			else
			{
				// signed distance from pixel center to lower edge line (positive = inside triangle)
				px_float dist = ((k1 * (i + 0.5f) + b1) - (j + 0.5f)) * inv_len1;
				px_float alpha = dist + 0.5f;
				if (alpha > 0)
				{
					px_color drawColor = color;
					alpha = alpha > 1 ? 1 : alpha;
					drawColor._argb.a = (px_byte)(drawColor._argb.a * alpha);
					PX_SurfaceDrawPixelWithoutLimit(psurface, i, j, drawColor);
				}
				else
				{
					break;
				}

			}
		}
	}
}

px_void PX_GeoDrawLeftTriangle(px_surface* psurface, px_float left, px_float top, px_float right, px_float bottom, px_color color)
{
	//y1=k1x+b1;
	//y2=k2x+b2;
	px_int i, j;
	px_float mid;
	px_float k1 = ((bottom - top) / 2) / (right - left);
	px_float b1 = bottom - k1 * right;
	px_float k2 = -k1;
	px_float b2 = top - k2 * right;
	px_float inv_len1 = 1.0f / PX_sqrt(k1 * k1 + 1);
	px_float inv_len2 = 1.0f / PX_sqrt(k2 * k2 + 1);
	if (color._argb.a == 0)
	{
		return;
	}

	if (left > right)
	{
		mid = left;
		left = right;
		right = mid;
	}
	if (top > bottom)
	{
		mid = top;
		top = bottom;
		bottom = mid;
	}

	if (left < psurface->limit_left)
	{
		left = (px_float)psurface->limit_left;
	}
	else if (left > psurface->limit_right)
	{
		return;
	}

	if (top < psurface->limit_top)
	{
		top = (px_float)psurface->limit_top;
	}
	else if (top > psurface->limit_bottom)
	{
		return;
	}

	if (right > psurface->limit_right)
	{
		right = (px_float)psurface->limit_right;
	}
	else if (right < psurface->limit_left)
	{
		return;
	}

	if (bottom > psurface->limit_bottom)
	{
		bottom = (px_float)psurface->limit_bottom;
	}
	else if (bottom < psurface->limit_top)
	{
		return;
	}

	//line 1
	for (j = (px_int)top; j <= (px_int)bottom; j++)
	{
		for (i = (px_int)right; i >= (px_int)left; i--)
		{
			if (j < (bottom + top) / 2)
			{
				// signed distance from pixel center to upper edge line (positive = inside triangle)
				px_float dist = ((j + 0.5f) - (k2 * (i + 0.5f) + b2)) * inv_len2;
				px_float alpha = dist + 0.5f;
				if (alpha > 0)
				{
					px_color drawColor = color;
					alpha = alpha > 1 ? 1 : alpha;
					drawColor._argb.a = (px_byte)(drawColor._argb.a * alpha);
					PX_SurfaceDrawPixelWithoutLimit(psurface, i, j, drawColor);
				}
				else
				{
					break;
				}

			}
			else
			{
				// signed distance from pixel center to lower edge line (positive = inside triangle)
				px_float dist = ((k1 * (i + 0.5f) + b1) - (j + 0.5f)) * inv_len1;
				px_float alpha = dist + 0.5f;
				if (alpha > 0)
				{
					px_color drawColor = color;
					alpha = alpha > 1 ? 1 : alpha;
					drawColor._argb.a = (px_byte)(drawColor._argb.a * alpha);
					PX_SurfaceDrawPixelWithoutLimit(psurface, i, j, drawColor);
				}
				else
				{
					break;
				}

			}
		}
	}

}

px_void PX_GeoDrawUpTriangle(px_surface* psurface, px_float left, px_float top, px_float right, px_float bottom, px_color color)
{
	//y1=k1x+b1;
//y2=k2x+b2;
	px_int i, j;
	px_float mid;
	px_float k1 = -(bottom - top) / ((right - left) / 2);
	px_float b1 = bottom - k1 * left;
	px_float k2 = -k1;
	px_float b2 = bottom - k2 * right;
	px_float inv_len1 = 1.0f / PX_sqrt(k1 * k1 + 1);
	px_float inv_len2 = 1.0f / PX_sqrt(k2 * k2 + 1);
	if (color._argb.a == 0)
	{
		return;
	}

	if (left > right)
	{
		mid = left;
		left = right;
		right = mid;
	}
	if (top > bottom)
	{
		mid = top;
		top = bottom;
		bottom = mid;
	}

	if (left < psurface->limit_left)
	{
		left = (px_float)psurface->limit_left;
	}
	else if (left > psurface->limit_right)
	{
		return;
	}

	if (top < psurface->limit_top)
	{
		top = (px_float)psurface->limit_top;
	}
	else if (top > psurface->limit_bottom)
	{
		return;
	}

	if (right > psurface->limit_right)
	{
		right = (px_float)psurface->limit_right;
	}
	else if (right < psurface->limit_left)
	{
		return;
	}

	if (bottom > psurface->limit_bottom)
	{
		bottom = (px_float)psurface->limit_bottom;
	}
	else if (bottom < psurface->limit_top)
	{
		return;
	}

	//line 1
	for (j = (px_int)top; j <= (px_int)bottom; j++)
	{
		for (i = (px_int)left; i <= (px_int)right; i++)
		{
			if (i < (right + left) / 2)
			{
				// signed distance from pixel center to left edge line (positive = inside triangle)
				px_float dist = ((j + 0.5f) - (k1 * (i + 0.5f) + b1)) * inv_len1;
				px_float alpha = dist + 0.5f;
				if (alpha > 0)
				{
					px_color drawColor = color;
					alpha = alpha > 1 ? 1 : alpha;
					drawColor._argb.a = (px_byte)(drawColor._argb.a * alpha);
					PX_SurfaceDrawPixelWithoutLimit(psurface, i, j, drawColor);
				}
			}
			else
			{
				// signed distance from pixel center to right edge line (positive = inside triangle)
				px_float dist = ((j + 0.5f) - (k2 * (i + 0.5f) + b2)) * inv_len2;
				px_float alpha = dist + 0.5f;
				if (alpha > 0)
				{
					px_color drawColor = color;
					alpha = alpha > 1 ? 1 : alpha;
					drawColor._argb.a = (px_byte)(drawColor._argb.a * alpha);
					PX_SurfaceDrawPixelWithoutLimit(psurface, i, j, drawColor);
				}
				else
				{
					break;
				}

			}
		}
	}
}
px_void PX_GeoDrawDownTriangle(px_surface* psurface, px_float left, px_float top, px_float right, px_float bottom, px_color color)
{
	//y1=k1x+b1;
//y2=k2x+b2;
	px_int i, j;
	px_float mid;
	px_float k1 = (bottom - top) / ((right - left) / 2);
	px_float b1 = top - k1 * left;
	px_float k2 = -k1;
	px_float b2 = top - k2 * right;
	px_float inv_len1 = 1.0f / PX_sqrt(k1 * k1 + 1);
	px_float inv_len2 = 1.0f / PX_sqrt(k2 * k2 + 1);
	if (color._argb.a == 0)
	{
		return;
	}

	if (left > right)
	{
		mid = left;
		left = right;
		right = mid;
	}
	if (top > bottom)
	{
		mid = top;
		top = bottom;
		bottom = mid;
	}

	if (left < psurface->limit_left)
	{
		left = (px_float)psurface->limit_left;
	}
	else if (left > psurface->limit_right)
	{
		return;
	}

	if (top < psurface->limit_top)
	{
		top = (px_float)psurface->limit_top;
	}
	else if (top > psurface->limit_bottom)
	{
		return;
	}

	if (right > psurface->limit_right)
	{
		right = (px_float)psurface->limit_right;
	}
	else if (right < psurface->limit_left)
	{
		return;
	}

	if (bottom > psurface->limit_bottom)
	{
		bottom = (px_float)psurface->limit_bottom;
	}
	else if (bottom < psurface->limit_top)
	{
		return;
	}

	//line 1
	for (j = (px_int)top; j <= (px_int)bottom; j++)
	{
		for (i = (px_int)left; i <= (px_int)right; i++)
		{
			if (i < (right + left) / 2)
			{
				// signed distance from pixel center to left edge line (positive = inside triangle)
				px_float dist = (k1 * (i + 0.5f) + b1 - (j + 0.5f)) * inv_len1;
				px_float alpha = dist + 0.5f;
				if (alpha > 0)
				{
					px_color drawColor = color;
					alpha = alpha > 1 ? 1 : alpha;
					drawColor._argb.a = (px_byte)(drawColor._argb.a * alpha);
					PX_SurfaceDrawPixelWithoutLimit(psurface, i, j, drawColor);
				}
			}
			else
			{
				// signed distance from pixel center to right edge line (positive = inside triangle)
				px_float dist = (k2 * (i + 0.5f) + b2 - (j + 0.5f)) * inv_len2;
				px_float alpha = dist + 0.5f;
				if (alpha > 0)
				{
					px_color drawColor = color;
					alpha = alpha > 1 ? 1 : alpha;
					drawColor._argb.a = (px_byte)(drawColor._argb.a * alpha);
					PX_SurfaceDrawPixelWithoutLimit(psurface, i, j, drawColor);
				}
				else
				{
					break;
				}

			}
		}
	}
}