#include "PX_BaseGeo.h"
static  px_int  st_symmetry_axis;

#define PX_ABS_FRAC(x) ((x)>0?PX_FRAC(x):PX_FRAC(-(x)))

static px_void PX_DrawLinePixel_RecK(px_surface *psurface,px_int x,px_int y,px_color color)
{
	PX_SurfaceDrawPixel(psurface,y,x,color);
}

static px_void PX_DrawLinePixel_InvK(px_surface *psurface,px_int x,px_int y,px_color color)
{
	PX_SurfaceDrawPixel(psurface,2*st_symmetry_axis-x,y,color);
}

static px_void PX_DrawLinePixel_InvRecK(px_surface *psurface,px_int x,px_int y,px_color color)
{
	PX_SurfaceDrawPixel(psurface,2*st_symmetry_axis-y,x,color);
}
px_void PX_GeoDrawLine(px_surface *psurface, px_int x0, px_int y0, px_int x1, px_int y1 ,px_int lineWidth, px_color color)
{
	px_int i,len,lm,x,y,temp1,temp2,xleft,xright;
	px_float p0x,p0y,p1x,p1y,p2x,p2y,p3x,p3y,vx,vy,rx,ry,xlen,ylen,S,ftemp,ryL,ryR,rxL,rxR;
	px_float k,recK;
	px_color clr;
	px_void (*func_DrawPixel)(px_surface *psurface,px_int x,px_int y,px_color color);
	
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
				x1=(px_int)(x1+vector_x*step);
				y1=(px_int)(y1+vector_y*step);
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
			func_DrawPixel=PX_SurfaceDrawPixel;
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

			st_symmetry_axis=x0;
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

			st_symmetry_axis=x0;
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
				func_DrawPixel(psurface,x,y,color);
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
				func_DrawPixel(psurface,x,y,clr);


				//left
				ftemp=1-PX_FRAC(rx-xlen);
				S=ftemp*ftemp/2/xlen;
				clr=color;
				clr._argb.a=(px_uchar)(clr._argb.a*S);
				func_DrawPixel(psurface,x-1,y,clr);

			}
			else
			{
				ftemp=1-PX_FRAC(rx);
				ftemp+=xlen/2;

				clr=color;
				clr._argb.a=clr._argb.a=(px_uchar)(clr._argb.a*ftemp);;

				func_DrawPixel(psurface,x,y,clr);

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
				func_DrawPixel(psurface,x,y,clr);

				if (y<temp2)
				{
					//bottom
					ftemp=PX_FRAC(ry+ylen);
					S=1-ftemp*ftemp/k/2;
					clr=color;
					clr._argb.a=(px_uchar)(clr._argb.a*S);
					func_DrawPixel(psurface,x,y+1,clr);
				}

				for (y=PX_TRUNC(ry)+2;y<=temp2;y++)
				{
					func_DrawPixel(psurface,x,y,color);
				}
				
				

			}
			else  
			{
				ftemp=1-PX_FRAC(ry+ylen);
				ftemp+=ylen/2;

				clr=color;
				clr._argb.a=(px_uchar)(clr._argb.a*ftemp);

				func_DrawPixel(psurface,x,y,clr);

				for (y=PX_TRUNC(ry)+1;y<=temp2;y++)
				{
					func_DrawPixel(psurface,x,y,color);
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
			func_DrawPixel(psurface,xleft-1,y,clr);

			//draw left line
			while (PX_TRUNC(ryL)==y)
			{
				if(PX_TRUNC(ryL+ylen)==y)
				{
					S=PX_FRAC(ryL)+ylen/2;
					clr=color;
					clr._argb.a=(px_uchar)(clr._argb.a*S);
					func_DrawPixel(psurface,xleft,y,clr);
				}
				else
				{
					ftemp=1-PX_FRAC(ryL);
					S=1-ftemp*ftemp/2/k;
					clr=color;
					clr._argb.a=(px_uchar)(clr._argb.a*S);
					func_DrawPixel(psurface,xleft,y,clr);
				}
				xleft++;
				ryL+=ylen;
			}

			//draw left
			ftemp=PX_FRAC(ryR);
			S=1-ftemp*ftemp/2/k;
			clr=color;
			clr._argb.a=(px_uchar)(clr._argb.a*S);
			func_DrawPixel(psurface,xright-1,y,clr);
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
						func_DrawPixel(psurface,xright,y,clr);
					}
					else
					{
						ftemp=1-PX_FRAC(ryR);
						S=ftemp*ftemp/2/k;
						clr=color;
						clr._argb.a=(px_uchar)(clr._argb.a*S);
						func_DrawPixel(psurface,xright,y,clr);
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
						func_DrawPixel(psurface,xright,y,clr);
					}
					else
					{
						ftemp=1-PX_FRAC(ryR);
						S=ftemp*ftemp/2/k;
						clr=color;
						clr._argb.a=(px_uchar)(clr._argb.a*S);
						func_DrawPixel(psurface,xright,y,clr);
					}
					xright++;
					ryR+=ylen;
				}
			}


			//draw horizontal
			for (x=xleft;x<temp1-1;x++)
			{
				func_DrawPixel(psurface,x,y,color);
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
				func_DrawPixel(psurface,x,y,clr);

				//top
				ftemp=1-PX_FRAC(ry-ylen);
				S=1-ftemp*ftemp/k/2;
				clr=color;
				clr._argb.a=(px_uchar)(clr._argb.a*S);
				func_DrawPixel(psurface,x,y-1,clr);

				for (i=y-2;i>temp2;i--)
				{
					func_DrawPixel(psurface,x,i,color);
				}

			}
			else
			{
				ftemp=PX_FRAC(ry-ylen);
				ftemp+=ylen/2;

				clr=color;
				clr._argb.a=(px_uchar)(clr._argb.a*ftemp);

				func_DrawPixel(psurface,x,y,clr);

				for (i=y-1;i>temp2;i--)
				{
					func_DrawPixel(psurface,x,i,color);
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
				func_DrawPixel(psurface,x,y,color);

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
				func_DrawPixel(psurface,x,y,clr);


				//left
				ftemp=1-PX_FRAC(rx-xlen);
				S=1-ftemp*ftemp/2/k;
				clr=color;
				clr._argb.a=(px_uchar)(clr._argb.a*S);
				func_DrawPixel(psurface,x-1,y,clr);

			}
			else
			{
				ftemp=PX_FRAC(rx)-xlen/2;
				clr=color;
				clr._argb.a=clr._argb.a=(px_uchar)(clr._argb.a*ftemp);;
				func_DrawPixel(psurface,x,y,clr);
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
				func_DrawPixel(psurface,x,y,clr);

				//bottom
				if(y<temp2)
				{
				ftemp=PX_FRAC(ry+ylen);
				S=1-ftemp*ftemp/k/2;
				clr=color;
				clr._argb.a=(px_uchar)(clr._argb.a*S);
				func_DrawPixel(psurface,x,y+1,clr);
				}
				for (y=PX_TRUNC(ry)+2;y<=temp2;y++)
				{
					func_DrawPixel(psurface,x,y,color);
				}

			}
			else
			{
				ftemp=1-PX_FRAC(ry+ylen);
				ftemp+=ylen/2;

				clr=color;
				clr._argb.a=(px_uchar)(clr._argb.a*ftemp);

				func_DrawPixel(psurface,x,y,clr);

				for (y=PX_TRUNC(ry)+1;y<=temp2;y++)
				{
					func_DrawPixel(psurface,x,y,color);
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
				func_DrawPixel(psurface,x,y,color);

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
				func_DrawPixel(psurface,x,y,clr);


				//left
				ftemp=1-PX_FRAC(rx-xlen);
				S=ftemp*ftemp/2/xlen;
				clr=color;
				clr._argb.a=(px_uchar)(clr._argb.a*S);
				func_DrawPixel(psurface,x-1,y,clr);

			}
			else
			{
				ftemp=1-PX_FRAC(rx);
				ftemp+=xlen/2;

				clr=color;
				clr._argb.a=clr._argb.a=(px_uchar)(clr._argb.a*ftemp);;

				func_DrawPixel(psurface,x,y,clr);

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
				func_DrawPixel(psurface,x,y,clr);


				//left
				ftemp=1-PX_FRAC(rxL-xlen);
				S=ftemp*ftemp/2/xlen;
				clr=color;
				clr._argb.a=(px_uchar)(clr._argb.a*S);
				func_DrawPixel(psurface,x-1,y,clr);

			}
			else
			{
				ftemp=1-PX_FRAC(rxL);
				ftemp+=xlen/2;

				clr=color;
				clr._argb.a=clr._argb.a=(px_uchar)(clr._argb.a*ftemp);;

				func_DrawPixel(psurface,x,y,clr);

			}
			rxL-=xlen;

			//draw right

			if(PX_TRUNC(rxR-xlen)==PX_TRUNC(rxR))
			{
				xright=PX_TRUNC(rxR);
				S=PX_FRAC(rxR)-xlen/2;
				clr=color;
				clr._argb.a=(px_uchar)(clr._argb.a*S);
				func_DrawPixel(psurface,xright,y,clr);

			}
			else
			{
				xright=PX_TRUNC(rxR);

				//draw right pixel
				ftemp=PX_FRAC(rxR);
				S=ftemp*ftemp/2/k;
				clr=color;
				clr._argb.a=(px_uchar)(clr._argb.a*S);
				func_DrawPixel(psurface,xright,y,clr);

				xright--;
				//draw left pixel
				ftemp=1-PX_FRAC(rxR-xlen);
				S=1-ftemp*ftemp/2/k;
				clr=color;
				clr._argb.a=(px_uchar)(clr._argb.a*S);
				func_DrawPixel(psurface,xright,y,clr);
			}
			rxR-=xlen;

			//draw horizontal
			for (x=xleft+1;x<xright;x++)
			{
				func_DrawPixel(psurface,x,y,color);
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
				func_DrawPixel(psurface,xright,y,clr);

			}
			else
			{
				xright=PX_TRUNC(rxR);

				//draw right pixel
				ftemp=PX_FRAC(rxR);
				S=ftemp*ftemp/2/k;
				clr=color;
				clr._argb.a=(px_uchar)(clr._argb.a*S);
				func_DrawPixel(psurface,xright,y,clr);

				xright--;
				//draw left pixel
				ftemp=1-PX_FRAC(rxR-xlen);
				S=1-ftemp*ftemp/2/k;
				clr=color;
				clr._argb.a=(px_uchar)(clr._argb.a*S);
				func_DrawPixel(psurface,xright,y,clr);
			}
			rxR-=xlen;

			//draw horizontal
			for (x=PX_TRUNC(p3x);x<xright;x++)
			{
				func_DrawPixel(psurface,x,y,color);
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
				func_DrawPixel(psurface,x,y,clr);

				//top
				if(y>temp2)
				{
				ftemp=1-PX_FRAC(ry-ylen);
				S=1-ftemp*ftemp/k/2;
				clr=color;
				clr._argb.a=(px_uchar)(clr._argb.a*S);
				func_DrawPixel(psurface,x,y-1,clr);
				}
				for (i=temp2;i<y-1;i++)
				{
					func_DrawPixel(psurface,x,i,color);
				}

			}
			else
			{
				ftemp=PX_FRAC(ry-ylen);
				ftemp+=ylen/2;

				clr=color;
				clr._argb.a=(px_uchar)(clr._argb.a*ftemp);

				func_DrawPixel(psurface,x,y,clr);

				for (i=temp2;i<y;i++)
				{
					func_DrawPixel(psurface,x,i,color);
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

	if (left<0)
	{
		left=0;
	}
	if (left<psurface->limit_left)
	{
		left=psurface->limit_left;
	}

	if (top<0)
	{
		top=0;
	}
	if (top<psurface->limit_top)
	{
		top=psurface->limit_top;
	}

	if (right>psurface->width-1)
	{
		right=psurface->width-1;
	}
	if (right>psurface->limit_right)
	{
		right=psurface->limit_right;
	}

	if (bottom>psurface->height-1)
	{
		bottom=psurface->height-1;
	}
	if (bottom>psurface->limit_bottom)
	{
		bottom=psurface->limit_bottom;
	}

	if (left>psurface->width-1)
	{
		return;
	}

	if (bottom<0)
	{
		return;
	}

	if (right<0)
	{
		return;
	}

	if (top>psurface->height-1)
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

static px_void PX_GeoDrawSolidCircle_Ex1(px_surface *psurface, px_int x,px_int y,px_int Radius,px_color color )
{
	px_int ry,dy,i,xleft,xright,Sy,cY,drx,dry;
	px_float rad2,xoft1,xoft2,S,fy,fdis;
	px_color clr;

	if (Radius==0)
	{
		return;
	}
	//Draw origin
	PX_SurfaceDrawPixel(psurface,x,y,color);

	//Draw cross

	if (x>=0-Radius&&y>=0-Radius&&x<psurface->width+Radius&&y<psurface->height+Radius)
	{
		clr=color;
		clr._argb.a>>=1;
		dy=Radius;
		PX_SurfaceDrawPixel(psurface,x,y+dy,clr);
		PX_SurfaceDrawPixel(psurface,x,y-dy,clr);
		PX_SurfaceDrawPixel(psurface,x+dy,y,clr);
		PX_SurfaceDrawPixel(psurface,x-dy,y,clr);

		for (i=1;i<dy;i++)
		{
			PX_SurfaceDrawPixel(psurface,x,y+i,color);
			PX_SurfaceDrawPixel(psurface,x,y-i,color);
			PX_SurfaceDrawPixel(psurface,x+i,y,color);
			PX_SurfaceDrawPixel(psurface,x-i,y,color);
		}

		// Draw 45-degree cross
		//inside
		ry=0;

		//out side
		rad2=(px_float)Radius;
		dy=PX_TRUNC(rad2/1.4142135623731f+0.5);
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
		xoft2=PX_sqrt(rad2*rad2-fy*fy)+0.5f;

		while (Sy<dy)
		{
			cY=ry;
			xleft=Sy;
			//////////////////////////////////////////////////////////////////////////

			//outside
			//////////////////////////////////////////////////////////////////////////
			xoft1=xoft2;
			fy=(px_float)Sy+0.5f;
			xoft2=PX_sqrt(rad2*rad2-fy*fy)+0.5f;


			if (PX_TRUNC(xoft1)!=PX_TRUNC(xoft2))
			{
				//draw right
				fdis=xoft1-xoft2;
				S=PX_FRAC(xoft1)/fdis;
				S*=S;
				S*=(fdis/2);
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
				S=1-S;
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
			xright=PX_TRUNC(xoft2);

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
}

static px_void PX_GeoDrawSolidCircle_Ex2(px_surface *psurface, px_float x,px_float y,px_float Radius,px_color color )
{
	px_int left,top,right,bottom,mid,i,j;
	px_float d;
	px_color clr;
	if(color._argb.a==0)
	{
		return;
	}

	left=(px_int)(x-Radius);
	top=(px_int)(y-Radius);
	right=(px_int)(x+Radius);
	bottom=(px_int)(y+Radius);


	if (left<0)
	{
		left=0;
	}
	if (top<0)
	{
		top=0;
	}

	if (right>psurface->width-1)
	{
		right=psurface->width-1;
	}

	if (bottom>psurface->height-1)
	{
		bottom=psurface->height-1;
	}

	if (left>psurface->width-1)
	{
		return;
	}

	if (bottom<0)
	{
		return;
	}

	if (right<0)
	{
		return;
	}

	if (top>psurface->height-1)
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

	for (i=top;i<=bottom;i++)
	{
		for (j=left;j<=right;j++)
		{
			d=PX_sqrt((px_float)((i-y)*(i-y)+(j-x)*(j-x)));
			if (d<Radius)
			{
				if ((px_float)Radius-d<1.414f)
				{
					clr=color;
					clr._argb.a=(px_uchar)(clr._argb.a*((px_float)Radius-d)/1.414f);
					PX_SurfaceDrawPixel(psurface,j,i,clr);
				}
				else
				PX_SurfaceDrawPixel(psurface,j,i,color);
			}
			
		}
	}

}



px_void PX_GeoDrawSolidCircle(px_surface *psurface, px_int x,px_int y,px_int Radius,px_color color )
{
	px_int Sc,Sr;
	px_int left,top,right,bottom,mid;
	if(color._argb.a==0)
	{
		return;
	}

	left=x-Radius;
	top=y-Radius;
	right=x+Radius;
	bottom=y+Radius;


	if (left<0)
	{
		left=0;
	}
	if (top<0)
	{
		top=0;
	}

	if (right>psurface->width-1)
	{
		right=psurface->width-1;
	}

	if (bottom>psurface->height-1)
	{
		bottom=psurface->height-1;
	}

	if (left>psurface->width-1)
	{
		return;
	}

	if (bottom<0)
	{
		return;
	}

	if (right<0)
	{
		return;
	}

	if (top>psurface->height-1)
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

	Sc=(px_int)(PX_PI*Radius*Radius);
	Sr=(right-left+1)*(bottom-top+1);
	if (Sr<Sc)
	{
		PX_GeoDrawSolidCircle_Ex2(psurface,(px_float)x,(px_float)y,(px_float)Radius,color);
	}
	else
	{
		PX_GeoDrawSolidCircle_Ex1(psurface,x,y,Radius,color);
	}
}


px_void PX_GeoDrawPenCircle(px_surface *psurface, px_float x,px_float y,px_float Radius,px_color color)
{
	PX_GeoDrawSolidCircle_Ex2(psurface,x,y,Radius,color);
}

px_void PX_GeoDrawPath(px_surface *psurface, px_point path[],px_int pathCount,px_float linewidth,px_color color)
{
	px_int i;
	if (pathCount==1)
	{
		PX_GeoDrawSolidCircle_Ex2(psurface,path[0].x,path[0].y,linewidth/2,color);
	}
	else
	{
		for (i=0;i<pathCount-1;i++)
		{
			px_point path_unit_vector=PX_PointNormalization(PX_PointSub(path[i+1],path[i]));
			px_int length=(px_int)PX_PointMod(PX_PointSub(path[i+1],path[i]));
			px_float step=0;
			do 
			{
				px_point drawPoint=PX_PointAdd(path[i],PX_PointMul(path_unit_vector,step));
				PX_GeoDrawSolidCircle_Ex2(psurface,drawPoint.x,drawPoint.y,linewidth/2,color);
				step+=(linewidth/4)<0.5f?0.5f:(linewidth/4);
			} while (step<length);
		}
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
		
		x=(px_int)roundRaduis-(px_int)(PX_sqrt(roundRaduis*roundRaduis-(roundRaduis-y-1)*(roundRaduis-y-1)))-1;
		if(x<0) x=0;
		for (;x<(px_int)roundRaduis;x++)
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
			if (dis<roundRaduis+0.5f)
			{
				drawColor=color;

				if (dis>roundRaduis)
					drawColor._argb.a=(px_uchar)(color._argb.a*(dis-roundRaduis+0.5f));

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

		x=(px_int)roundRaduis-(px_int)(PX_sqrt(roundRaduis*roundRaduis-(roundRaduis-y-1)*(roundRaduis-y-1)))-1;
		if(x<0)x=0;
		for (;x<(px_int)roundRaduis;x++)
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
			if (dis<roundRaduis+0.5f&&dis>roundRaduis-linewidth-0.5f)
			{
				drawColor=color;

				if (dis>roundRaduis)
					drawColor._argb.a=(px_uchar)(color._argb.a*(dis-roundRaduis+0.5f));

				if (dis<roundRaduis-linewidth+0.5f)
					drawColor._argb.a=(px_uchar)(color._argb.a*(1.0f-(roundRaduis-linewidth+0.5f-dis)));

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

px_void PX_GeoDrawBresenhamLine(px_surface *psurface,int x0, int y0, int x1, int y1,px_color color)
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
		int dx = PX_ABS(x1-x0), sx = x0<x1 ? 1 : -1;
		int dy = PX_ABS(y1-y0), sy = y0<y1 ? 1 : -1; 
		int err = (dx>dy ? dx : -dy)/2, e2;
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



	px_float x, y, xleft, xright; 
	px_float btmy,midy;

	px_float a,b,c;
	a=(px_float)PX_sqrtd((p1.x-p2.x)*(p1.x-p2.x));
	b=(px_float)PX_sqrtd((p0.x-p2.x)*(p0.x-p2.x));
	c=(px_float)PX_sqrtd((p1.x-p0.x)*(p1.x-p0.x));

	x=(a*p0.x+b*p1.x+c*p2.x)/(a+b+c);
	y=(a*p0.y+b*p1.y+c*p2.y)/(a+b+c);

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
		b12=x0;
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
			PX_SurfaceDrawPixel(psurface,ix,iy,color);
		} while (0);
		
		for(ix = (px_int)xleft+1;ix < (px_int)xright; ++ix)
		{
			PX_SurfaceDrawPixel(psurface,ix,iy,color);
		}

// 		do 
// 		{
// 			px_color aClr=color;
// 			aClr._argb.a=(px_byte)(aClr._argb.a*rAlpha);
// 			PX_SurfaceDrawPixel(psurface,ix,iy,color);
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
		b12=x0;
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
			PX_SurfaceDrawPixel(psurface,ix,iy,color);
		} while (0);

		for(ix = (px_int)xleft+1;ix < (px_int)xright; ++ix)
		{
			PX_SurfaceDrawPixel(psurface,ix,iy,color);
		}

// 		do 
// 		{
// 			px_color aClr=color;
// 			aClr._argb.a=(px_byte)(aClr._argb.a*rAlpha);
// 			PX_SurfaceDrawPixel(psurface,ix,iy,color);
// 		} while (0);
	}
	
}

px_void PX_GeoDrawArrow(px_surface *psurface,px_point2D p0,px_point2D p1,px_float size,px_color color)
{
	px_point2D p0_5,v,vn;
	px_float arrowsize,distance;
	
	if (size<1)
	{
		return;
	}

	arrowsize=size*5;



	//draw line
	v=PX_Point2DNormalization(PX_Point2DSub(p1,p0));
	distance=PX_Point2DMod(PX_Point2DSub(p1,p0));
	distance-=arrowsize;
	p0_5=PX_Point2DAdd(p0,PX_Point2DMul(v,distance));
	PX_GeoDrawLine(psurface,(px_int)p0.x,(px_int)p0.y,(px_int)p0_5.x,(px_int)p0_5.y,(px_int)size,color);

	//normal
	vn.x=-v.y;
	vn.y=v.x;

	PX_GeoDrawTriangle(psurface,p0_5,p1,PX_Point2DAdd(p0_5,PX_Point2DMul(vn,arrowsize)),color);
	PX_GeoDrawTriangle(psurface,p0_5,p1,PX_Point2DAdd(p0_5,PX_Point2DMul(vn,-arrowsize)),color);
}

