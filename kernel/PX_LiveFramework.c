#include "PX_LiveFramework.h"

static px_void PX_LiveFramework_RenderListPixelShader(px_surface *psurface,px_int x,px_int y,px_float z,px_float u,px_float v,px_point normal,px_texture *pTexture,PX_TEXTURERENDER_BLEND *blend)
{
	//texture mapping
	px_double SampleX,SampleY,mapX,mapY;
	px_double mixa,mixr,mixg,mixb,Weight;
	px_color sampleColor;
	px_int resWidth;
	px_int resHeight;

	if (u<0||u>1||v<0||v>1)
	{
		return;
	}


	if (pTexture)
	{
		resWidth=pTexture->width;
		resHeight=pTexture->height;
		u=PX_ABS(u);
		v=PX_ABS(v);
		u-=(px_int)u;
		v-=(px_int)v;

		mapX=u*resWidth;
		mapY=v*resHeight;

		if (mapX<-0.5||mapX>resWidth+0.5)
		{
			return;
		}
		if (mapY<-0.5||mapY>resHeight+0.5)
		{
			return;
		}
		mixa=0;
		mixr=0;
		mixg=0;
		mixb=0;
		//Sample 4 points
		//lt

		SampleX=(mapX-0.5f);
		SampleY=(mapY-0.5f);

		if (SampleX>0&&(SampleX)<resWidth&&SampleY>0&&(SampleY)<resHeight)
		{
			//Sample color from resTexture
			sampleColor=PX_SURFACECOLOR(pTexture,PX_TRUNC(SampleX),PX_TRUNC(SampleY));
			Weight=(1-PX_FRAC(SampleX))*(1-PX_FRAC(SampleY));
			mixa+=sampleColor._argb.a*Weight;
			mixr+=sampleColor._argb.r*Weight;
			mixg+=sampleColor._argb.g*Weight;
			mixb+=sampleColor._argb.b*Weight;
		}

		SampleX=(mapX+0.5f);
		SampleY=(mapY-0.5f);
		if (SampleX>0&&(SampleX)<resWidth&&SampleY>0&&(SampleY)<resHeight)
		{
			//Sample color from resTexture
			sampleColor=PX_SURFACECOLOR(pTexture,PX_TRUNC(SampleX),PX_TRUNC(SampleY));
			Weight=PX_FRAC(mapX+0.5f)*(1-PX_FRAC(mapY-0.5f));
			mixa+=sampleColor._argb.a*Weight;
			mixr+=sampleColor._argb.r*Weight;
			mixg+=sampleColor._argb.g*Weight;
			mixb+=sampleColor._argb.b*Weight;
		}

		SampleX=(mapX-0.5f);
		SampleY=(mapY+0.5f);
		if (SampleX>0&&(SampleX)<resWidth&&SampleY>0&&(SampleY)<resHeight)
		{
			//Sample color from resTexture
			sampleColor=PX_SURFACECOLOR(pTexture,PX_TRUNC(SampleX),PX_TRUNC(SampleY));
			Weight=(1-PX_FRAC(SampleX))*(PX_FRAC(SampleY));
			mixa+=sampleColor._argb.a*Weight;
			mixr+=sampleColor._argb.r*Weight;
			mixg+=sampleColor._argb.g*Weight;
			mixb+=sampleColor._argb.b*Weight;
		}

		SampleX=(mapX+0.5f);
		SampleY=(mapY+0.5f);
		if (SampleX>0&&(SampleX)<resWidth&&SampleY>0&&(SampleY)<resHeight)
		{
			//Sample color from resTexture
			sampleColor=PX_SURFACECOLOR(pTexture,PX_TRUNC(SampleX),PX_TRUNC(SampleY));
			Weight=(PX_FRAC(SampleX))*(PX_FRAC(SampleY));
			mixa+=sampleColor._argb.a*Weight;
			mixr+=sampleColor._argb.r*Weight;
			mixg+=sampleColor._argb.g*Weight;
			mixb+=sampleColor._argb.b*Weight;
		}

		if (blend)
		{
			mixa*=blend->alpha;
			mixr*=blend->hdr_R;
			mixg*=blend->hdr_G;
			mixb*=blend->hdr_B;
		}

		mixa>255?mixa=255:0;
		mixr>255?mixr=255:0;
		mixg>255?mixg=255:0;
		mixb>255?mixb=255:0;

		PX_SurfaceDrawPixel(psurface,x,y,PX_COLOR((px_uchar)mixa,(px_uchar)mixr,(px_uchar)mixg,(px_uchar)mixb));

	}
}
static px_void PX_LiveFramework_RenderListRasterization(px_surface *psurface,PX_LiveFramework *pLiveFramework,PX_LiveFramework_Vertex p0,PX_LiveFramework_Vertex p1,PX_LiveFramework_Vertex p2,px_texture *ptexture,PX_TEXTURERENDER_BLEND *blend)
{
	px_int ix,iy;
	px_bool  k01infinite=PX_FALSE;
	px_bool  k02infinite=PX_FALSE;
	px_bool  k12infinite=PX_FALSE;
	px_float k01,b01,k02,b02,k12,b12;
	px_float x0;
	px_float y0;
	px_float z0;
	px_float s0;
	px_float t0;

	px_float x1;
	px_float y1;
	px_float z1;
	px_float s1;
	px_float t1;

	px_float x2;
	px_float y2;
	px_float z2;
	px_float s2;
	px_float t2;


	px_float x, y, xleft, xright; 
	px_float oneoverz_left, oneoverz_right; 
	px_float oneoverz_top, oneoverz_bottom; 
	px_float oneoverz, oneoverz_step;   
	px_float soverz_top, soverz_bottom; 
	px_float toverz_top, toverz_bottom; 
	px_float soverz_left, soverz_right; 
	px_float toverz_left, toverz_right;
	px_float soverz, soverz_step;
	px_float toverz, toverz_step; 
	px_float s, t;
	px_float btmy,midy;
	px_float originalZ;

	px_float a,b,c;
	px_point position;
	a=(px_float)PX_sqrtd((p1.position.x-p2.position.x)*(p1.position.x-p2.position.x));
	b=(px_float)PX_sqrtd((p0.position.x-p2.position.x)*(p0.position.x-p2.position.x));
	c=(px_float)PX_sqrtd((p1.position.x-p0.position.x)*(p1.position.x-p0.position.x));

	position.x=(a*p0.position.x+b*p1.position.x+c*p2.position.x)/(a+b+c);
	position.y=(a*p0.position.y+b*p1.position.y+c*p2.position.y)/(a+b+c);

	//    p0
	// p1   p2

	if (p1.position.y<p0.position.y)
	{
		PX_LiveFramework_Vertex t;
		t=p1;
		p1=p0;
		p0=t;
	}

	if (p2.position.y<p0.position.y)
	{
		PX_LiveFramework_Vertex t;
		t=p2;
		p2=p0;
		p0=t;
	}

	btmy=p1.position.y;
	midy=p2.position.y;
	if (p2.position.y>btmy)
	{
		midy=p1.position.y;
		btmy=p2.position.y;
	}



	do 
	{
		px_float x01m;

		x0=p0.position.x;
		y0=p0.position.y;
		x1=p1.position.x;
		y1=p1.position.y;
		x2=p2.position.x;
		y2=p2.position.y;


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
			PX_LiveFramework_Vertex t;
			t=p2;
			p2=p1;
			p1=t;
		}
	} while (0);



	x0=p0.position.x;
	y0=p0.position.y;
	z0=p0.position.z;
	s0=p0.u;
	t0=p0.v;

	x1=p1.position.x;
	y1=p1.position.y;
	z1=p1.position.z;
	s1=p1.u;
	t1=p1.v;

	x2=p2.position.x;
	y2=p2.position.y;
	z2=p2.position.z;
	s2=p2.u;
	t2=p2.v;

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
		}
		else
		{
			xleft = (y-b01)/k01;
		}

		if (k02infinite)
		{
			xright=b02;
		}
		else
		{
			xright = (y-b02)/k02;
		}


		oneoverz_top = 1.0f / z0;
		oneoverz_bottom = 1.0f/z1;
		oneoverz_left = (y-y0) * (oneoverz_bottom-oneoverz_top) / (y1-y0) + oneoverz_top;
		oneoverz_bottom = 1.0f / z2;
		oneoverz_right = (y-y0) * (oneoverz_bottom-oneoverz_top) / (y2-y0) + oneoverz_top;
		oneoverz_step = (oneoverz_right-oneoverz_left) / (xright-xleft);
		soverz_top = s0 / z0;
		soverz_bottom = s1 / z1;
		soverz_left = (y-y0) * (soverz_bottom-soverz_top) / (y1-y0) + soverz_top;
		soverz_bottom = s2 / z2;
		soverz_right = (y-y0) * (soverz_bottom-soverz_top) / (y2-y0) + soverz_top;
		soverz_step = (soverz_right-soverz_left) / (xright-xleft);
		toverz_top = t0 / z0;
		toverz_bottom = t1 / z1;
		toverz_left = (y-y0) * (toverz_bottom-toverz_top) / (y1-y0) + toverz_top;
		toverz_bottom = t2 / z2;
		toverz_right = (y-y0) * (toverz_bottom-toverz_top) / (y2-y0) + toverz_top;
		toverz_step = (toverz_right-toverz_left) / (xright-xleft);
		oneoverz = oneoverz_left,soverz = soverz_left, toverz = toverz_left;

		for(x = xleft;x < xright; ++x)
		{
			s = soverz / oneoverz;
			t = toverz / oneoverz;
			originalZ=1.0f/oneoverz;

			ix=(px_int)x;
			iy=(px_int)y;

			oneoverz += oneoverz_step;
			soverz += soverz_step;
			toverz += toverz_step;
		}
	}

	// p1   p2
	//    p0
	if (p1.position.y>p0.position.y)
	{
		PX_LiveFramework_Vertex t;
		t=p1;
		p1=p0;
		p0=t;
	}

	if (p2.position.y>p0.position.y)
	{
		PX_LiveFramework_Vertex t;
		t=p2;
		p2=p0;
		p0=t;
	}

	btmy=p1.position.y;
	midy=p2.position.y;
	if (p2.position.y<btmy)
	{
		midy=p1.position.y;
		btmy=p2.position.y;
	}



	do 
	{
		px_float x01m;

		x0=p0.position.x;
		y0=p0.position.y;
		x1=p1.position.x;
		y1=p1.position.y;
		x2=p2.position.x;
		y2=p2.position.y;


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
			PX_LiveFramework_Vertex t;
			t=p2;
			p2=p1;
			p1=t;
		}
	} while (0);



	x0=p0.position.x;
	y0=p0.position.y;
	z0=p0.position.z;
	s0=p0.u;
	t0=p0.v;

	x1=p1.position.x;
	y1=p1.position.y;
	z1=p1.position.z;
	s1=p1.u;
	t1=p1.v;

	x2=p2.position.x;
	y2=p2.position.y;
	z2=p2.position.z;
	s2=p2.u;
	t2=p2.v;

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
		}
		else
		{
			xleft = (y-b01)/k01;
		}

		if (k02infinite)
		{
			xright=b02;
		}
		else
		{
			xright = (y-b02)/k02;
		}


		oneoverz_top = 1.0f / z0;
		oneoverz_bottom = 1.0f/z1;
		oneoverz_left = (y-y0) * (oneoverz_bottom-oneoverz_top) / (y1-y0) + oneoverz_top;
		oneoverz_bottom = 1.0f / z2;
		oneoverz_right = (y-y0) * (oneoverz_bottom-oneoverz_top) / (y2-y0) + oneoverz_top;
		oneoverz_step = (oneoverz_right-oneoverz_left) / (xright-xleft);
		soverz_top = s0 / z0;
		soverz_bottom = s1 / z1;
		soverz_left = (y-y0) * (soverz_bottom-soverz_top) / (y1-y0) + soverz_top;
		soverz_bottom = s2 / z2;
		soverz_right = (y-y0) * (soverz_bottom-soverz_top) / (y2-y0) + soverz_top;
		soverz_step = (soverz_right-soverz_left) / (xright-xleft);
		toverz_top = t0 / z0;
		toverz_bottom = t1 / z1;
		toverz_left = (y-y0) * (toverz_bottom-toverz_top) / (y1-y0) + toverz_top;
		toverz_bottom = t2 / z2;
		toverz_right = (y-y0) * (toverz_bottom-toverz_top) / (y2-y0) + toverz_top;
		toverz_step = (toverz_right-toverz_left) / (xright-xleft);
		oneoverz = oneoverz_left,soverz = soverz_left, toverz = toverz_left;

		for(x = xleft;x < xright; ++x)
		{
			s = soverz / oneoverz;
			t = toverz / oneoverz;
			originalZ=1.0f/oneoverz;

			ix=(px_int)x;
			iy=(px_int)y;

			if (pLiveFramework->pixelShader)
			{
				position.z=originalZ;
				pLiveFramework->pixelShader(psurface,ix,iy,position,s,t,p0.normal,ptexture,blend);
			}
			else
			{
				PX_LiveFramework_RenderListPixelShader(psurface,ix,iy,originalZ,s,t,p0.normal,ptexture,blend);
			}
			oneoverz += oneoverz_step;
			soverz += soverz_step;
			toverz += toverz_step;
		}
	}

}


px_bool PX_LiveFrameworkInitialize(px_memorypool *mp,PX_LiveFramework *plive,px_int width,px_int height)
{
	PX_memset(plive,0,sizeof(PX_LiveFramework));
	plive->mp=mp;
	if(!PX_VectorInitialize(mp,&plive->layers,sizeof(PX_LiveLayer),1))return PX_FALSE;
	if(!PX_VectorInitialize(mp,&plive->livetextures,sizeof(PX_LiveTexture),1))return PX_FALSE;
	plive->width=width;
	plive->height=height;
	
	return PX_TRUE;
}

px_bool PX_LiveFrameworkLoad(PX_LiveFramework *plive,const px_char json_content[])
{
	return PX_TRUE;
}

px_void PX_LiveFrameworkUpdate(PX_LiveFramework *plive,px_dword elpased)
{
	
}

px_void PX_LiveFrameworkRenderLayer(px_surface *psurface,PX_LiveFramework *plive,PX_LiveLayer *pLayer,px_int x,px_int y,px_dword elpased)
{
	px_float rotcos;
	px_float rotsin;
	px_point offset;

	if (!pLayer->visible)
	{
		return;
	}

	offset=PX_PointSub(pLayer->translation,pLayer->keyPoint);
	if (pLayer->pLiveTexture)
	{
		px_texture *pTexture;
		
		pTexture=&pLayer->pLiveTexture->Texture;

		rotcos=(px_float)PX_cosd(pLayer->rotationAngle);
		rotsin=(px_float)PX_sind(pLayer->rotationAngle);

		if (pLayer->triangles.size&&pLayer->vertices.size)
		{
			px_int t;
			PX_Delaunay_Triangle *pTriangleIndex;
			for (t=0;t<pLayer->triangles.size;t++)
			{
				PX_LiveFramework_Vertex v0,v1,v2;
				px_point p_vector;
				pTriangleIndex=PX_VECTORAT(PX_Delaunay_Triangle,&pLayer->triangles,t);
				v0=*PX_VECTORAT(PX_LiveFramework_Vertex,&pLayer->vertices,pTriangleIndex->index1);
				v1=*PX_VECTORAT(PX_LiveFramework_Vertex,&pLayer->vertices,pTriangleIndex->index2);
				v2=*PX_VECTORAT(PX_LiveFramework_Vertex,&pLayer->vertices,pTriangleIndex->index3);

				//Transform
				//v0
				p_vector.x=v0.position.x-pLayer->keyPoint.x;
				p_vector.y=v0.position.y-pLayer->keyPoint.y;

				v0.position.x=pLayer->keyPoint.x+p_vector.x*rotcos+p_vector.y*rotsin;
				v0.position.y=pLayer->keyPoint.y-p_vector.x*rotsin+p_vector.y*rotcos;

				v0.position.x+=x+offset.x;
				v0.position.y+=y+offset.y;

				//v1
				p_vector.x=v1.position.x-pLayer->keyPoint.x;
				p_vector.y=v1.position.y-pLayer->keyPoint.y;

				v1.position.x=pLayer->keyPoint.x+p_vector.x*rotcos+p_vector.y*rotsin;
				v1.position.y=pLayer->keyPoint.y-p_vector.x*rotsin+p_vector.y*rotcos;

				v1.position.x+=x+offset.x;
				v1.position.y+=y+offset.y;

				//v2
				p_vector.x=v2.position.x-pLayer->keyPoint.x;
				p_vector.y=v2.position.y-pLayer->keyPoint.y;

				v2.position.x=pLayer->keyPoint.x+p_vector.x*rotcos+p_vector.y*rotsin;
				v2.position.y=pLayer->keyPoint.y-p_vector.x*rotsin+p_vector.y*rotcos;

				v2.position.x+=x+offset.x;
				v2.position.y+=y+offset.y;

				PX_LiveFramework_RenderListRasterization(psurface,plive,v0,v1,v2,pTexture,&pLayer->blend);
			}

			if (pLayer->showMesh)
			{
				for (t=0;t<pLayer->triangles.size;t++)
				{
					PX_LiveFramework_Vertex v0,v1,v2;
					px_point p_vector;
					pTriangleIndex=PX_VECTORAT(PX_Delaunay_Triangle,&pLayer->triangles,t);
					v0=*PX_VECTORAT(PX_LiveFramework_Vertex,&pLayer->vertices,pTriangleIndex->index1);
					v1=*PX_VECTORAT(PX_LiveFramework_Vertex,&pLayer->vertices,pTriangleIndex->index2);
					v2=*PX_VECTORAT(PX_LiveFramework_Vertex,&pLayer->vertices,pTriangleIndex->index3);

					//Transform
					//v0
					p_vector.x=v0.position.x-pLayer->keyPoint.x;
					p_vector.y=v0.position.y-pLayer->keyPoint.y;

					v0.position.x=pLayer->keyPoint.x+p_vector.x*rotcos+p_vector.y*rotsin;
					v0.position.y=pLayer->keyPoint.y-p_vector.x*rotsin+p_vector.y*rotcos;

					v0.position.x+=x+offset.x;
					v0.position.y+=y+offset.y;

					//v1
					p_vector.x=v1.position.x-pLayer->keyPoint.x;
					p_vector.y=v1.position.y-pLayer->keyPoint.y;

					v1.position.x=pLayer->keyPoint.x+p_vector.x*rotcos+p_vector.y*rotsin;
					v1.position.y=pLayer->keyPoint.y-p_vector.x*rotsin+p_vector.y*rotcos;

					v1.position.x+=x+offset.x;
					v1.position.y+=y+offset.y;

					//v2
					p_vector.x=v2.position.x-pLayer->keyPoint.x;
					p_vector.y=v2.position.y-pLayer->keyPoint.y;

					v2.position.x=pLayer->keyPoint.x+p_vector.x*rotcos+p_vector.y*rotsin;
					v2.position.y=pLayer->keyPoint.y-p_vector.x*rotsin+p_vector.y*rotcos;

					v2.position.x+=x+offset.x;
					v2.position.y+=y+offset.y;

					PX_GeoDrawLine(psurface,(px_int)v0.position.x,(px_int)v0.position.y,(px_int)v1.position.x,(px_int)v1.position.y,1,PX_COLOR(128,255,0,0));
					PX_GeoDrawLine(psurface,(px_int)v0.position.x,(px_int)v0.position.y,(px_int)v2.position.x,(px_int)v2.position.y,1,PX_COLOR(128,255,0,0));
					PX_GeoDrawLine(psurface,(px_int)v1.position.x,(px_int)v1.position.y,(px_int)v2.position.x,(px_int)v2.position.y,1,PX_COLOR(128,255,0,0));

					PX_GeoDrawSolidCircle(psurface,(px_int)v0.position.x,(px_int)v0.position.y,3,PX_COLOR(128,255,0,0));
					PX_GeoDrawSolidCircle(psurface,(px_int)v1.position.x,(px_int)v1.position.y,3,PX_COLOR(128,255,0,0));
					PX_GeoDrawSolidCircle(psurface,(px_int)v2.position.x,(px_int)v2.position.y,3,PX_COLOR(128,255,0,0));
				}
			}
		}
		else
		{
			px_point Centre,newCentre;

			Centre.x=(px_float)pTexture->width/2;
			Centre.y=(px_float)pTexture->height/2;
			Centre.z=0;
			newCentre.x=pLayer->keyPoint.x+(Centre.x-pLayer->keyPoint.x)*rotcos+(Centre.y-pLayer->keyPoint.y)*rotsin;
			newCentre.y=pLayer->keyPoint.y-(Centre.x-pLayer->keyPoint.x)*rotsin+(Centre.y-pLayer->keyPoint.y)*rotcos;
			newCentre.z=0;
			newCentre.x=newCentre.x+(pLayer->translation.x-pLayer->keyPoint.x);
			newCentre.y=newCentre.y+(pLayer->translation.y-pLayer->keyPoint.y);

			PX_TextureRenderRotation_sincos(psurface,pTexture,(px_int)(newCentre.x+x+pLayer->pLiveTexture->textureOffsetX),
				(px_int)(newCentre.y+y+pLayer->pLiveTexture->textureOffsetY),PX_TEXTURERENDER_REFPOINT_CENTER,&pLayer->blend,-rotsin,rotcos);
		}

		if (pLayer->showLinker)
		{

			PX_GeoDrawCircle(psurface,(px_int)(x+pLayer->keyPoint.x+pLayer->pLiveTexture->textureOffsetX),
				(px_int)(y+pLayer->keyPoint.y+pLayer->pLiveTexture->textureOffsetY),
				5,
				1,
				PX_COLOR(255,0,0,255));

			PX_GeoDrawSolidCircle(psurface,(px_int)(x+pLayer->keyPoint.x+pLayer->pLiveTexture->textureOffsetX),
				(px_int)(y+pLayer->keyPoint.y+pLayer->pLiveTexture->textureOffsetY),
				3,
				PX_COLOR(255,0,0,255));


			PX_GeoDrawCircle(psurface,(px_int)(x+pLayer->translation.x+pLayer->pLiveTexture->textureOffsetX),
				(px_int)(y+pLayer->translation.y+pLayer->pLiveTexture->textureOffsetY),
				5,
				1,
				PX_COLOR(255,255,0,255));
			PX_GeoDrawSolidCircle(psurface,(px_int)(x+pLayer->translation.x+pLayer->pLiveTexture->textureOffsetX),
				(px_int)(y+pLayer->translation.y+pLayer->pLiveTexture->textureOffsetY),
				3,
				PX_COLOR(255,255,0,255));

		}
	}
}

px_void PX_LiveFrameworkRender(px_surface *psurface,PX_LiveFramework *plive,px_int x,px_int y,PX_TEXTURERENDER_REFPOINT refPoint,px_dword elpased)
{
	PX_QuickSortAtom sAtom[PX_LIVEFRAMEWORK_MAX_SUPPORT_LAYER];
	px_int i,count;
	if (plive->layers.size==0)
	{
		return;
	}

	switch (refPoint)
	{
	case PX_TEXTURERENDER_REFPOINT_LEFTTOP:
		break;
	case PX_TEXTURERENDER_REFPOINT_MIDTOP:
		x-=plive->width/2;
		break;
	case PX_TEXTURERENDER_REFPOINT_RIGHTTOP:
		x-=plive->width;
		break;
	case PX_TEXTURERENDER_REFPOINT_LEFTMID:
		y-=plive->height/2;
		break;
	case PX_TEXTURERENDER_REFPOINT_CENTER:
		y-=plive->height/2;
		x-=plive->width/2;
		break;
	case PX_TEXTURERENDER_REFPOINT_RIGHTMID:
		y-=plive->height/2;
		x-=plive->width;
		break;
	case PX_TEXTURERENDER_REFPOINT_LEFTBOTTOM:
		y-=plive->height;
		break;
	case PX_TEXTURERENDER_REFPOINT_MIDBOTTOM:
		y-=plive->height;
		x-=plive->width/2;
		break;
	case PX_TEXTURERENDER_REFPOINT_RIGHTBOTTOM:
		y-=plive->height;
		x-=plive->width;
		break;
	}

	//clear zbuffer
	//PX_memset(plive->zbuffer,0,plive->surfaceWidth*plive->surfaceHeight*sizeof(px_float));
	
	for (i=0;i<plive->layers.size;i++)
	{
		PX_LiveLayer *pLayer=PX_VECTORAT(PX_LiveLayer,&plive->layers,i);
		sAtom[i].weight=pLayer->keyPoint.z+pLayer->translation.z;
		sAtom[i].pData=pLayer;
		if (i>=PX_LIVEFRAMEWORK_MAX_SUPPORT_LAYER-1)
		{
			break;
		}
	}
	count=i;
	PX_Quicksort_MaxToMin(sAtom,0,count-1);

	for (i=0;i<count;i++)
	{
		PX_LiveLayer *pLayer=(PX_LiveLayer *)sAtom[i].pData;
		PX_LiveFrameworkRenderLayer(psurface,plive,pLayer,x,y,elpased);
	}

	

	if (plive->showRange)
	{
		PX_GeoDrawBorder(psurface,x,y,x+plive->width,y+plive->height,1,PX_COLOR(255,0,0,255));
	}

}

px_int PX_LiveFrameworkGetLayerTextureWidth(PX_LiveLayer *pLayer)
{
	if (pLayer->pLiveTexture)
	{
		return pLayer->pLiveTexture->Texture.width;
	}
	return 0;
}

px_int PX_LiveFrameworkGetLayerTextureHeight(PX_LiveLayer *pLayer)
{
	if (pLayer->pLiveTexture)
	{
		return pLayer->pLiveTexture->Texture.height;
	}
	return 0;
}

px_bool PX_LiveFrameworkPlayAnimation(PX_LiveFramework *plive,const px_char name[])
{
	return PX_TRUE;
}

PX_LiveLayer * PX_LiveFrameworkGetLayerById(PX_LiveFramework *plive,const px_char id[])
{
	px_int i;
	for (i=0;i<plive->layers.size;i++)
	{
		PX_LiveLayer *pLayer=PX_LiveFrameworkGetLayer(plive,i);
		if (pLayer&&PX_memequ(pLayer->id,id,PX_LIVE_ID_MAX_LEN))
		{
			return pLayer;
		}
	}
	return PX_NULL;
}

PX_LiveLayer * PX_LiveFrameworkCreateLayer(PX_LiveFramework *plive,const px_char id[PX_LIVEFRAMEWORK_MAX_SUPPORT_LAYER])
{
	PX_LiveLayer layer;

	if (plive->layers.size>=PX_LIVEFRAMEWORK_MAX_SUPPORT_LAYER)
	{
		return PX_NULL;
	}

	if (PX_LiveFrameworkGetLayerById(plive,id))
	{
		return PX_NULL;
	}


	PX_memset(&layer,0,sizeof(layer));

	if(!PX_VectorInitialize(plive->mp,&layer.children_Nodes,sizeof(PX_LiveLayer *),0))return PX_FALSE;
	if(!PX_VectorInitialize(plive->mp,&layer.parent_Nodes,sizeof(PX_LiveLayer *),0))return PX_FALSE;;
	if(!PX_VectorInitialize(plive->mp,&layer.triangles,sizeof(PX_Delaunay_Triangle),0))return PX_FALSE;;
	if(!PX_VectorInitialize(plive->mp,&layer.vertices,sizeof(PX_LiveFramework_Vertex),0))return PX_FALSE;;

	layer.rotationAngle=0;
	layer.visible=PX_TRUE;
	layer.blend.alpha=1;
	layer.blend.hdr_B=1;
	layer.blend.hdr_G=1;
	layer.blend.hdr_R=1;
	layer.keyPoint.z=1;
	PX_strcpy(layer.id,id,sizeof(layer.id));

	if(!PX_VectorPushback(&plive->layers,&layer))
		return PX_FALSE;

	return PX_VECTORLAST(PX_LiveLayer,&plive->layers);

}

PX_LiveLayer * PX_LiveFrameworkGetLayer(PX_LiveFramework *plive,px_int i)
{
	if (plive->layers.size)
	{
		return PX_VECTORAT(PX_LiveLayer,&plive->layers,i);
	}
	return PX_NULL;
}

PX_LiveLayer *PX_LiveFrameworkGetLastCreateLayer(PX_LiveFramework *plive)
{
	if (plive->layers.size)
	{
		return PX_VECTORLAST(PX_LiveLayer,&plive->layers);
	}
	return PX_NULL;
}

px_bool PX_LiveFrameworkAddLiveTexture(PX_LiveFramework *plive,PX_LiveTexture livetexture)
{
	return PX_VectorPushback(&plive->livetextures,&livetexture);
}

PX_LiveTexture * PX_LiveFrameworkGetLiveTextureById(PX_LiveFramework *plive,const px_char id[])
{
	px_int i;
	for (i=0;i<plive->livetextures.size;i++)
	{
		PX_LiveTexture *pTexture=PX_VECTORAT(PX_LiveTexture,&plive->livetextures,i);
		if (PX_strequ(id,pTexture->id))
		{
			return pTexture;
		}
	}
	return PX_NULL;
}

px_void PX_LiveFrameworkFree(PX_LiveFramework *plive)
{
	PX_VectorFree(&plive->layers);
	PX_VectorFree(&plive->livetextures);
}

