#include "PX_LiveFramework.h"

typedef struct
{
	px_point position;
	px_point normal;
	px_float u,v;
}PX_LiveRenderVertex;
static px_void PX_LiveFramework_RenderListPixelShaderFaster(px_surface *psurface,px_int x,px_int y,px_float z,px_float u,px_float v,px_point normal,px_texture *pTexture,PX_TEXTURERENDER_BLEND *blend)
{
	//texture mapping

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

		PX_SurfaceDrawPixel(psurface,x,y,PX_SURFACECOLOR(pTexture,(px_int)(u*resWidth),(px_int)(v*resHeight)));
	}
}
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
static px_void PX_LiveFramework_RenderListRasterization(px_surface *psurface,PX_LiveFramework *pLiveFramework,PX_LiveRenderVertex p0,PX_LiveRenderVertex p1,PX_LiveRenderVertex p2,px_texture *ptexture,PX_TEXTURERENDER_BLEND *blend)
{
	px_int   ix,iy;
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


	px_float y,xleft, xright; 
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
		PX_LiveRenderVertex t;
		t=p1;
		p1=p0;
		p0=t;
	}

	if (p2.position.y<p0.position.y)
	{
		PX_LiveRenderVertex t;
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
			PX_LiveRenderVertex t;
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
		k01 = 1;
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
		k02 = 1;
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

		for(ix = (px_int)(xleft+0.5);ix < (px_int)(xright+0.5f); ++ix)
		{
			s = soverz / oneoverz;
			t = toverz / oneoverz;
			originalZ=1.0f/oneoverz;

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

	// p1   p2
	//    p0
	if (p1.position.y>p0.position.y)
	{
		PX_LiveRenderVertex t;
		t=p1;
		p1=p0;
		p0=t;
	}

	if (p2.position.y>p0.position.y)
	{
		PX_LiveRenderVertex t;
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
			PX_LiveRenderVertex t;
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

		for(ix = (px_int)(xleft+0.5);ix < (px_int)(xright+0.5f); ++ix)
		{
			s = soverz / oneoverz;
			t = toverz / oneoverz;
			originalZ=1.0f/oneoverz;
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
	if(!PX_VectorInitialize(mp,&plive->liveAnimations,sizeof(PX_LiveAnimation),1))return PX_FALSE;
	plive->width=width;
	plive->height=height;
	plive->showFocusLayer=PX_TRUE;
	plive->currentEditAnimationIndex=-1;
	plive->currentEditFrameIndex=-1;
	plive->currentEditVertexIndex=-1;
	plive->currentEditLayerIndex=-1;
	return PX_TRUE;
}

px_void PX_LiveFrameworkPlay(PX_LiveFramework *plive)
{
	plive->status=PX_LIVEFRAMEWORK_STATUS_PLAYING;
}

px_void PX_LiveFrameworkPause(PX_LiveFramework *plive)
{
	plive->status=PX_LIVEFRAMEWORK_STATUS_STOP;
}

px_void PX_LiveFrameworkReset(PX_LiveFramework *plive)
{
	px_int i;
	plive->reg_duration=0;
	plive->reg_ip=0;
	plive->reg_elapsed=0;
	plive->reg_bp=-1;
	plive->status=PX_LIVEFRAMEWORK_STATUS_STOP;

	for (i=0;i<plive->layers.size;i++)
	{
		px_int j;
		PX_LiveLayer *pLayer=PX_VECTORAT(PX_LiveLayer,&plive->layers,i);
		pLayer->rel_beginRotationAngle=0;
		pLayer->rel_currentRotationAngle=0;
		pLayer->rel_endRotationAngle=0;

		pLayer->rel_beginTranslation=PX_POINT(0,0,0);
		pLayer->rel_currentTranslation=PX_POINT(0,0,0);
		pLayer->rel_endTranslation=PX_POINT(0,0,0);

		pLayer->rel_beginStretch=1;
		pLayer->rel_currentStretch=1;
		pLayer->rel_endStretch=1;

		pLayer->RenderTextureIndex=pLayer->LinkTextureIndex;

		for (j=0;j<pLayer->vertices.size;j++)
		{
			PX_LiveVertex *pVertex=PX_VECTORAT(PX_LiveVertex,&pLayer->vertices,j);
			pVertex->beginTranslation=PX_POINT(0,0,0);
			pVertex->currentTranslation=PX_POINT(0,0,0);
			pVertex->endTranslation=PX_POINT(0,0,0);

			pVertex->currentPosition=pVertex->sourcePosition;
			pVertex->velocity=PX_POINT(0,0,0);
		}
	}
}

px_void PX_LiveFrameworkStop(PX_LiveFramework *plive)
{
	PX_LiveFrameworkReset(plive);
}

static px_void PX_LiveFrameworkUpdateLayerInterpolation(PX_LiveFramework *plive,PX_LiveLayer *pLayer)
{
	px_float schedule;
	px_int i;

	if (plive->status==PX_LIVEFRAMEWORK_STATUS_STOP)
	{
		schedule = 1;
	}
	else if (plive->reg_duration==0)
	{
		schedule=1;
	}
	else
	{
		schedule=plive->reg_elapsed*1.0f/plive->reg_duration;
	}

	if (schedule>1)
	{
		schedule=1;
	}

	//update parameters
	pLayer->panc_currentx= pLayer->panc_beginx + (pLayer->panc_endx - pLayer->panc_beginx) * schedule;
	pLayer->panc_currenty = pLayer->panc_beginy + (pLayer->panc_endy - pLayer->panc_beginy) * schedule;

	//point translation
	for (i=0;i<pLayer->vertices.size;i++)
	{
		px_point *pbegin,*pcurrent,*pend;
		pbegin=&PX_VECTORAT(PX_LiveVertex,&pLayer->vertices,i)->beginTranslation;
		pcurrent=&PX_VECTORAT(PX_LiveVertex,&pLayer->vertices,i)->currentTranslation;
		pend=&PX_VECTORAT(PX_LiveVertex,&pLayer->vertices,i)->endTranslation;

		pcurrent->x=pbegin->x+(pend->x-pbegin->x)*schedule;
		pcurrent->y=pbegin->y+(pend->y-pbegin->y)*schedule;
	}

	if (pLayer->parent_index!=-1)
	{
		//stretch
		pLayer->rel_currentStretch=pLayer->rel_beginStretch+(pLayer->rel_endStretch-pLayer->rel_beginStretch)*schedule;
	}
	else
	{
		//keypoint translation
		pLayer->rel_currentTranslation.x=pLayer->rel_beginTranslation.x+(pLayer->rel_endTranslation.x-pLayer->rel_beginTranslation.x)*schedule;
		pLayer->rel_currentTranslation.y=pLayer->rel_beginTranslation.y+(pLayer->rel_endTranslation.y-pLayer->rel_beginTranslation.y)*schedule;
		pLayer->rel_currentTranslation.z=0;
	}
	
	//rotation
	pLayer->rel_currentRotationAngle=pLayer->rel_beginRotationAngle+(pLayer->rel_endRotationAngle-pLayer->rel_beginRotationAngle)*schedule;

	

}

static px_void PX_LiveFramework_UpdateLayerKeyPoint(PX_LiveFramework *pLive,PX_LiveLayer *pLayer)
{
	px_int i;
	if (pLayer->parent_index!=-1)
	{
		//stretch
		px_point v=PX_PointSub(pLayer->keyPoint,PX_LiveFrameworkGetLayerParent(pLive,pLayer)->keyPoint);
		v=PX_PointMul(v,pLayer->rel_currentStretch);
		v=PX_PointRotate(v,PX_LiveFrameworkGetLayerParent(pLive,pLayer)->rel_currentRotationAngle);
		pLayer->currentKeyPoint=PX_PointAdd(v,PX_LiveFrameworkGetLayerParent(pLive,pLayer)->currentKeyPoint);
	}
	else
	{
		pLayer->currentKeyPoint=PX_PointAdd(pLayer->keyPoint,pLayer->rel_currentTranslation);
	}
	pLayer->currentKeyPoint.z=pLayer->keyPoint.z;

	for (i=0;i<PX_COUNTOF(pLayer->child_index);i++)
	{
		if (pLayer->child_index[i]!=-1)
		{
			PX_LiveFramework_UpdateLayerKeyPoint(pLive,PX_LiveFrameworkGetLayerChild(pLive,pLayer,pLayer->child_index[i]));
		}
		else
		{
			break;
		}
	}
}

static px_void PX_LiveFramework_UpdateLayerVertices(PX_LiveFramework *pLive,PX_LiveLayer *pLayer,px_dword elapsed)
{
	px_int i;
	px_point2D keyDirection;
	px_int k;


	PX_LiveFrameworkUpdateLayerRenderVerticesUV(pLive,pLayer);

	if (pLayer->child_index[0]==-1)
	{
		keyDirection=PX_POINT2D(0,1);
	}
	else
	{
		keyDirection=PX_POINT2D(0,0);
		for (i=0;i<PX_COUNTOF(pLayer->child_index);i++)
		{
			px_point v;
			if (pLayer->child_index[i]==-1)
			{
				break;
			}
			
			v=PX_PointNormalization(PX_PointSub(pLayer->currentKeyPoint,PX_LiveFrameworkGetLayerChild(pLive,pLayer,pLayer->child_index[i])->currentKeyPoint));
			keyDirection=PX_Point2DAdd(keyDirection,PX_POINT2D(v.x,v.y));
		}
		keyDirection=PX_Point2DNormalization(keyDirection);
	}

	//for each vertex
	for (i=0;i<pLayer->vertices.size;i++)
	{
		PX_LiveVertex *plv=PX_VECTORAT(PX_LiveVertex,&pLayer->vertices,i);
		px_point resultPosition;
		
		//get relative position
		resultPosition.x=plv->sourcePosition.x-pLayer->keyPoint.x;
		resultPosition.y=plv->sourcePosition.y-pLayer->keyPoint.y;
		resultPosition.z=plv->sourcePosition.z-pLayer->keyPoint.z;
		//panc translation
		if (pLayer->panc_currentx!=pLayer->panc_sx|| pLayer->panc_currenty != pLayer->panc_sy)
		{
			if (plv->sourcePosition.x>pLayer->panc_x&& plv->sourcePosition.x < pLayer->panc_x+pLayer->panc_width)
			{
				if (plv->sourcePosition.x < pLayer->panc_sx)
				{
					px_float disx = plv->sourcePosition.x - pLayer->panc_x;
					resultPosition.x += disx * (pLayer->panc_currentx-pLayer->panc_x) /(pLayer->panc_sx- pLayer->panc_x) - disx;
				}
				else
				{
					px_float disx =  (pLayer->panc_x+pLayer->panc_width)- plv->sourcePosition.x;
					resultPosition.x -= disx * (pLayer->panc_x + pLayer->panc_width - pLayer->panc_currentx) / (pLayer->panc_x + pLayer->panc_width - pLayer->panc_sx) - disx;
				}
			}

			if (plv->sourcePosition.y > pLayer->panc_y && plv->sourcePosition.y < pLayer->panc_y + pLayer->panc_height)
			{
				if (plv->sourcePosition.y < pLayer->panc_sy)
				{
					px_float disy = plv->sourcePosition.y - pLayer->panc_y;
					resultPosition.y += disy * (pLayer->panc_currenty - pLayer->panc_y) / (pLayer->panc_sy - pLayer->panc_y) - disy;
				}
				else
				{
					px_float disy = (pLayer->panc_y + pLayer->panc_height) - plv->sourcePosition.y;
					resultPosition.y -= disy * (pLayer->panc_y + pLayer->panc_height - pLayer->panc_currenty) / (pLayer->panc_y + pLayer->panc_height - pLayer->panc_sy) - disy;
				}
			}

			
		}
		
		
		//relative translation
		resultPosition.x+=plv->currentTranslation.x;
		resultPosition.y+=plv->currentTranslation.y;
		resultPosition.z+=plv->currentTranslation.z;

		//stretch
		do 
		{
			px_int j;
			for (j=0;j<PX_COUNTOF(pLayer->child_index);j++)
			{
				PX_LiveLayer *pChild=PX_LiveFrameworkGetLayerChild(pLive,pLayer,pLayer->child_index[j]);
				if(!pChild)
					break;

				
				if (pChild->rel_currentStretch!=1)
				{
					px_float cos_v12;
					px_point v1,v2,u1;
					v1=PX_PointSub(pChild->keyPoint,pLayer->keyPoint);
					v2=resultPosition;
					cos_v12=PX_PointDot(v1,v2)/PX_PointMod(v1)/PX_PointMod(v2);
					if (cos_v12>0)
					{
						px_float distance;
						u1=PX_PointNormalization(v1);
						distance=cos_v12*PX_PointMod(v2);
						resultPosition=PX_PointAdd(resultPosition,PX_PointMul(u1,distance*(pChild->rel_currentStretch-1)));
					}
				}
			}
		} while (0);


		//Rotation
		resultPosition=PX_PointRotate(resultPosition,pLayer->rel_currentRotationAngle);

		//absolute translation
		resultPosition.x+=pLayer->currentKeyPoint.x;
		resultPosition.y+=pLayer->currentKeyPoint.y;


		//elastic
		k=plv->k;

		if (k==0)
		{
			plv->currentPosition=resultPosition;
		}
		else
		{
			px_point2D direction,direction_normal;
			px_float distance;
			px_dword updateelapsed=elapsed+elapsed/2;
			px_dword atomelapsed;

			while (updateelapsed)
			{
				if (updateelapsed>50)
				{
					atomelapsed=50;
					updateelapsed-=50;
				}
				else
				{
					atomelapsed=updateelapsed;
					updateelapsed=0;
				}
				direction.x=resultPosition.x-plv->currentPosition.x;
				direction.y=resultPosition.y-plv->currentPosition.y;

				direction_normal=PX_Point2DNormalization(direction);
				distance=PX_Point2DMod(direction);


				if (distance>k)
				{
					plv->currentPosition.x=resultPosition.x-direction_normal.x*(k);
					plv->currentPosition.y=resultPosition.y-direction_normal.y*(k);
					distance=k*1.0f;
				}


				do
				{
					px_point2D incv=PX_Point2DMul(direction_normal,distance*distance);
					px_point2D  velocity;
					px_point velocity_vx,velocity_vy;
					px_float _cos,length;
					incv.x+=pLayer->rel_impulse.x;
					incv.y+=pLayer->rel_impulse.y;
					incv=PX_Point2DMul(incv,atomelapsed/1000.f);
					velocity=PX_Point2DAdd(PX_POINT2D(plv->velocity.x,plv->velocity.y),incv);
					if (velocity.x||velocity.y)
					{
						//resistance
						_cos=PX_Point2DDot(velocity,keyDirection)/PX_Point2DMod(velocity)/PX_Point2DMod(keyDirection);
						length=_cos*PX_Point2DMod(velocity);
						velocity_vx.x=length*keyDirection.x;
						velocity_vx.y=length*keyDirection.y;
						velocity_vx.z=0;

						velocity_vy.x=velocity.x-velocity_vx.x;
						velocity_vy.y=velocity.y-velocity_vx.y;
						velocity_vy.z=0;


						velocity_vx=PX_PointMul(velocity_vx,1.0f-atomelapsed/(k*10.f+50));
						velocity_vy=PX_PointMul(velocity_vy,1.0f-atomelapsed/(k*30.f+50));

						if (velocity_vx.x>10000||velocity_vx.y>10000||velocity_vy.x>10000||velocity_vy.y>10000)
						{
							PX_ASSERT();
						}

						plv->velocity=PX_PointAdd(velocity_vx,velocity_vy);
					}
					plv->currentPosition=PX_PointAdd(plv->currentPosition,PX_PointMul(plv->velocity,atomelapsed/1000.f));
										
				}while(0);
			}
			
		}
		
	}
}

static px_void PX_LiveFrameworkUpdatePhysical(PX_LiveFramework *plive,px_dword elapsed)
{
	px_int i;

	for (i=0;i<plive->layers.size;i++)
	{
		PX_LiveLayer *pLayer=PX_VECTORAT(PX_LiveLayer,&plive->layers,i);
		PX_LiveFrameworkUpdateLayerInterpolation(plive,pLayer);
	}

	//LayerUpdate
	for (i=0;i<plive->layers.size;i++)
	{
		PX_LiveLayer *pLayer=PX_VECTORAT(PX_LiveLayer,&plive->layers,i);
		if (pLayer->parent_index==-1)
		{
			PX_LiveFramework_UpdateLayerKeyPoint(plive,pLayer);
		}
	}

	for (i=0;i<plive->layers.size;i++)
	{
		PX_LiveLayer *pLayer=PX_VECTORAT(PX_LiveLayer,&plive->layers,i);
		PX_LiveFramework_UpdateLayerVertices(plive,pLayer,elapsed);
	}
}

static px_bool PX_LiveFrameworkExecuteInstr(PX_LiveFramework *plive,px_int animation_index,px_int frameindex)
{
	//execute instr
	px_int frame_offset=0,frame_size=0;
	px_int layerindex=0;
	PX_LiveAnimation *pAnimation;
	px_byte *pFrameInstrData;
	PX_LiveAnimationFrameHeader *pFrameHeader;
	if (animation_index<0||animation_index>=plive->liveAnimations.size)
	{
		goto _ERROR;
	}
	pAnimation=PX_VECTORAT(PX_LiveAnimation,&plive->liveAnimations,animation_index);
	if (frameindex<0)
	{
		goto _ERROR;
	}
	if (frameindex >= pAnimation->framesMemPtr.size)
	{
		goto _ERROR;
	}
	pFrameInstrData=*PX_VECTORAT(px_byte *,&pAnimation->framesMemPtr,frameindex);
	pFrameHeader=(PX_LiveAnimationFrameHeader *)pFrameInstrData;
	frame_size=pFrameHeader->size+sizeof(PX_LiveAnimationFrameHeader);
	//////////////////////////////////////////////////////////////////////////
	//time stamp update
	plive->reg_duration=pFrameHeader->duration_ms;
	frame_offset+=sizeof(PX_LiveAnimationFrameHeader);
	//////////////////////////////////////////////////////////////////////////
	//payload
	//////////////////////////////////////////////////////////////////////////
	while (frame_offset<frame_size)
	{
		PX_LiveLayer *pLayer;
		PX_LiveAnimationFramePayload *pPayload=(PX_LiveAnimationFramePayload *)(pFrameInstrData+frame_offset);

		pLayer=PX_VECTORAT(PX_LiveLayer,&plive->layers,layerindex);
		
		//////////////////////////////////////////////////////////////////////////
		//maptexture
		pLayer->RenderTextureIndex=pPayload->mapTexture;
		
		//////////////////////////////////////////////////////////////////////////
		//rotation register
		pLayer->rel_beginRotationAngle=pLayer->rel_endRotationAngle;
		pLayer->rel_currentRotationAngle=pLayer->rel_beginRotationAngle;
		pLayer->rel_endRotationAngle=pPayload->rotation;

		//////////////////////////////////////////////////////////////////////////
		//stretch register
		pLayer->rel_beginStretch=pLayer->rel_endStretch;
		pLayer->rel_currentStretch=pLayer->rel_beginStretch;
		pLayer->rel_endStretch=pPayload->stretch;

		//////////////////////////////////////////////////////////////////////////
		//translation register
		pLayer->rel_beginTranslation=pLayer->rel_endTranslation;
		pLayer->rel_currentTranslation=pLayer->rel_beginTranslation;
		pLayer->rel_endTranslation=pPayload->translation;

		//////////////////////////////////////////////////////////////////////////
		//impulse
		pLayer->rel_impulse=pPayload->impulse;


		//////////////////////////////////////////////////////////////////////////
		//panc
		pLayer->panc_x = pPayload->panc_x;
		pLayer->panc_y = pPayload->panc_y;

		pLayer->panc_width = pPayload->panc_width;
		pLayer->panc_height = pPayload->panc_height;

		pLayer->panc_sx = pPayload->panc_sx;
		pLayer->panc_sy = pPayload->panc_sy;

		pLayer->panc_beginx = pLayer->panc_endx;
		pLayer->panc_beginy = pLayer->panc_endy;

		pLayer->panc_currentx = pLayer->panc_beginx;
		pLayer->panc_currenty = pLayer->panc_beginy;

		pLayer->panc_endx = pPayload->panc_endx;
		pLayer->panc_endy = pPayload->panc_endy;

		//////////////////////////////////////////////////////////////////////////
		//vertices register
		if (pPayload->translationVerticesCount!=pLayer->vertices.size)
		{
			goto _ERROR;
		}
		else
		{
			px_int j;
			px_point *pVertexTranslation=(px_point *)(pFrameInstrData+frame_offset+sizeof(PX_LiveAnimationFramePayload));
		
			for (j=0;j<(px_int)pPayload->translationVerticesCount;j++)
			{
				PX_VECTORAT(PX_LiveVertex,&pLayer->vertices,j)->beginTranslation=PX_VECTORAT(PX_LiveVertex,&pLayer->vertices,j)->endTranslation;
				PX_VECTORAT(PX_LiveVertex,&pLayer->vertices,j)->currentTranslation=PX_VECTORAT(PX_LiveVertex,&pLayer->vertices,j)->beginTranslation;
				PX_VECTORAT(PX_LiveVertex,&pLayer->vertices,j)->endTranslation=pVertexTranslation[j];
			}
		}
		frame_offset+=sizeof(PX_LiveAnimationFramePayload);
		frame_offset+=sizeof(px_point)*pPayload->translationVerticesCount;
		layerindex++;
	}
	if (layerindex!=plive->layers.size)
	{
		goto _ERROR;
	}
	return PX_TRUE;
_ERROR:
	return PX_FALSE;
}

static px_void PX_LiveFrameworkUpdateVM(PX_LiveFramework *plive,px_dword elapsed)
{
	if (plive->status==PX_LIVEFRAMEWORK_STATUS_STOP)
	{
		return;
	}

	plive->reg_elapsed+=elapsed;


	while (PX_TRUE)
	{

		if (plive->reg_elapsed>=plive->reg_duration)
		{
			px_dword duration = plive->reg_duration;
			if (!PX_LiveFrameworkExecuteInstr(plive, plive->reg_animation, plive->reg_ip))
			{
				plive->status = PX_LIVEFRAMEWORK_STATUS_STOP;
				goto _ERROR;
			}

			//update time
			plive->reg_elapsed -= duration;

			//update ip
			plive->reg_ip++;
		}
		else
			break;
	}
	return;
_ERROR:
	return;
}

static px_void PX_LiveFrameworkRenderLayer(px_surface *psurface,PX_LiveFramework *plive,PX_LiveLayer *pLayer,px_int x,px_int y,px_dword elapsed)
{
	PX_TEXTURERENDER_BLEND blend;
	PX_LiveTexture *pLiveTexture;
	if (!pLayer->visible)
	{
		return;
	}
	pLiveTexture=PX_LiveFrameworkGetLiveTexture(plive,pLayer->RenderTextureIndex);
	if (pLiveTexture)
	{
		px_texture *pTexture;		
		pTexture=&pLiveTexture->Texture;

		if (pLayer->triangles.size&&pLayer->vertices.size)
		{
			px_int t;
			PX_Delaunay_Triangle *pTriangleIndex;
			for (t=0;t<pLayer->triangles.size;t++)
			{
				PX_LiveRenderVertex v0,v1,v2;
				PX_LiveVertex *pv0,*pv1,*pv2;
				pTriangleIndex=PX_VECTORAT(PX_Delaunay_Triangle,&pLayer->triangles,t);
				pv0=PX_VECTORAT(PX_LiveVertex,&pLayer->vertices,pTriangleIndex->index1);
				pv1=PX_VECTORAT(PX_LiveVertex,&pLayer->vertices,pTriangleIndex->index2);
				pv2=PX_VECTORAT(PX_LiveVertex,&pLayer->vertices,pTriangleIndex->index3);
				v0.position.x=pv0->currentPosition.x+x;
				v0.position.y=pv0->currentPosition.y+y;
				v0.position.z=1;
				v0.normal=pv0->normal;
				v0.u=pv0->u;
				v0.v=pv0->v;

				v1.position.x=pv1->currentPosition.x+x;
				v1.position.y=pv1->currentPosition.y+y;
				v1.position.z=1;
				v1.normal=pv1->normal;
				v1.u=pv1->u;
				v1.v=pv1->v;

				v2.position.x=pv2->currentPosition.x+x;
				v2.position.y=pv2->currentPosition.y+y;
				v2.position.z=1;
				v2.normal=pv2->normal;
				v2.u=pv2->u;
				v2.v=pv2->v;

				if (plive->currentEditLayerIndex>=0&&plive->currentEditLayerIndex<plive->layers.size)
				{
					if (pLayer==PX_VECTORAT(PX_LiveLayer,&plive->layers,plive->currentEditLayerIndex)||!plive->showFocusLayer)
					{
						PX_LiveFramework_RenderListRasterization(psurface,plive,v0,v1,v2,pTexture,PX_NULL);
					}
					else
					{
						blend.alpha=0.2f;
						blend.hdr_B=1;
						blend.hdr_G=1;
						blend.hdr_R=1;
						PX_LiveFramework_RenderListRasterization(psurface,plive,v0,v1,v2,pTexture,&blend);
					}
				}
				else
				{
					PX_LiveFramework_RenderListRasterization(psurface,plive,v0,v1,v2,pTexture,PX_NULL);
				}
				
			}

			if (pLayer->showMesh)
			{
				for (t=0;t<pLayer->triangles.size;t++)
				{
					PX_LiveRenderVertex v0,v1,v2;
					PX_LiveVertex *pv0,*pv1,*pv2;
					pTriangleIndex=PX_VECTORAT(PX_Delaunay_Triangle,&pLayer->triangles,t);
					pv0=PX_VECTORAT(PX_LiveVertex,&pLayer->vertices,pTriangleIndex->index1);
					pv1=PX_VECTORAT(PX_LiveVertex,&pLayer->vertices,pTriangleIndex->index2);
					pv2=PX_VECTORAT(PX_LiveVertex,&pLayer->vertices,pTriangleIndex->index3);
					v0.position.x=pv0->currentPosition.x+x;
					v0.position.y=pv0->currentPosition.y+y;
					v0.normal=pv0->normal;
					v0.u=pv0->u;
					v0.v=pv0->v;

					v1.position.x=pv1->currentPosition.x+x;
					v1.position.y=pv1->currentPosition.y+y;
					v1.normal=pv1->normal;
					v1.u=pv1->u;
					v1.v=pv1->v;

					v2.position.x=pv2->currentPosition.x+x;
					v2.position.y=pv2->currentPosition.y+y;
					v2.normal=pv2->normal;
					v2.u=pv2->u;
					v2.v=pv2->v;


					PX_GeoDrawLine(psurface,(px_int)v0.position.x,(px_int)v0.position.y,(px_int)v1.position.x,(px_int)v1.position.y,1,PX_COLOR(128,255,0,0));
					PX_GeoDrawLine(psurface,(px_int)v0.position.x,(px_int)v0.position.y,(px_int)v2.position.x,(px_int)v2.position.y,1,PX_COLOR(128,255,0,0));
					PX_GeoDrawLine(psurface,(px_int)v1.position.x,(px_int)v1.position.y,(px_int)v2.position.x,(px_int)v2.position.y,1,PX_COLOR(128,255,0,0));

					PX_GeoDrawSolidCircle(psurface,(px_int)v0.position.x,(px_int)v0.position.y,3,PX_COLOR(128,255,0,0));
					PX_GeoDrawSolidCircle(psurface,(px_int)v1.position.x,(px_int)v1.position.y,3,PX_COLOR(128,255,0,0));
					PX_GeoDrawSolidCircle(psurface,(px_int)v2.position.x,(px_int)v2.position.y,3,PX_COLOR(128,255,0,0));
				}

				if (plive->currentEditLayerIndex>=0&&plive->currentEditLayerIndex<plive->layers.size)
				{
					if (pLayer==PX_VECTORAT(PX_LiveLayer,&plive->layers,plive->currentEditLayerIndex))
					{
						if (plive->currentEditVertexIndex>=0&&plive->currentEditVertexIndex<pLayer->vertices.size)
						{
							PX_LiveVertex *pLiveVertex=PX_VECTORAT(PX_LiveVertex,&pLayer->vertices,plive->currentEditVertexIndex);
							PX_GeoDrawCircle(psurface,(px_int)(pLiveVertex->currentPosition.x+x),(px_int)(pLiveVertex->currentPosition.y+y),5,1,PX_COLOR(255,255,128,0));
						}
					}
				}

			}
		}
		else
		{
			if (plive->currentEditLayerIndex>=0&&plive->currentEditLayerIndex<plive->layers.size)
			{
				if (plive->showFocusLayer)
				{
					if (pLayer==PX_VECTORAT(PX_LiveLayer,&plive->layers,plive->currentEditLayerIndex))
					{
						PX_TextureRender(psurface,pTexture,(px_int)(x+pLiveTexture->textureOffsetX),
							(px_int)(y+pLiveTexture->textureOffsetY),PX_ALIGN_LEFTTOP,PX_NULL);
					}
					else
					{
						blend.alpha=0.2f;
						blend.hdr_B=1;
						blend.hdr_G=1;
						blend.hdr_R=1;
						PX_TextureRender(psurface,pTexture,(px_int)(x+pLiveTexture->textureOffsetX),
							(px_int)(y+pLiveTexture->textureOffsetY),PX_ALIGN_LEFTTOP,&blend);
					}
				}
				else
				{
					PX_TextureRender(psurface,pTexture,(px_int)(x+pLiveTexture->textureOffsetX),
						(px_int)(y+pLiveTexture->textureOffsetY),PX_ALIGN_LEFTTOP,PX_NULL);
				}
				
			}
			else
			{
				PX_TextureRender(psurface,pTexture,(px_int)(x+pLiveTexture->textureOffsetX),
					(px_int)(y+pLiveTexture->textureOffsetY),PX_ALIGN_LEFTTOP,PX_NULL);

			}
		}
	
	}
}

px_void PX_LiveFrameworkRender(px_surface *psurface,PX_LiveFramework *plive,px_int x,px_int y,PX_ALIGN refPoint,px_dword elapsed)
{
	PX_QuickSortAtom sAtom[PX_LIVEFRAMEWORK_MAX_SUPPORT_LAYER];
	px_int i,count;

	PX_LiveFrameworkUpdateVM(plive,elapsed);
	PX_LiveFrameworkUpdatePhysical(plive,elapsed);
	switch (refPoint)
	{
	case PX_ALIGN_LEFTTOP:
		break;
	case PX_ALIGN_MIDTOP:
		x-=plive->width/2;
		break;
	case PX_ALIGN_RIGHTTOP:
		x-=plive->width;
		break;
	case PX_ALIGN_LEFTMID:
		y-=plive->height/2;
		break;
	case PX_ALIGN_CENTER:
		y-=plive->height/2;
		x-=plive->width/2;
		break;
	case PX_ALIGN_RIGHTMID:
		y-=plive->height/2;
		x-=plive->width;
		break;
	case PX_ALIGN_LEFTBOTTOM:
		y-=plive->height;
		break;
	case PX_ALIGN_MIDBOTTOM:
		y-=plive->height;
		x-=plive->width/2;
		break;
	case PX_ALIGN_RIGHTBOTTOM:
		y-=plive->height;
		x-=plive->width;
		break;
	}


	if (plive->layers.size)
	{
		for (i=0;i<plive->layers.size;i++)
		{
			PX_LiveLayer *pLayer=PX_VECTORAT(PX_LiveLayer,&plive->layers,i);
			sAtom[i].weight=pLayer->currentKeyPoint.z;
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
			PX_LiveFrameworkRenderLayer(psurface,plive,pLayer,x,y,elapsed);
		}
	}
	
	if (plive->showKeypoint&&!plive->showlinker)
	{
		for (i=0;i<plive->layers.size;i++)
		{
			PX_LiveLayer *pLayer=PX_VECTORAT(PX_LiveLayer,&plive->layers,i);
			if (pLayer->parent_index!=-1)
			{
				PX_GeoDrawSolidCircle(psurface,(px_int)(pLayer->currentKeyPoint.x+plive->refer_x),(px_int)(pLayer->currentKeyPoint.y+plive->refer_y),3,PX_COLOR(255,0,64,192));
				PX_GeoDrawCircle(psurface,(px_int)(pLayer->currentKeyPoint.x+plive->refer_x),(px_int)(pLayer->currentKeyPoint.y+plive->refer_y),5,1,PX_COLOR(255,0,64,192));
			}
			else
			{
				PX_GeoDrawSolidCircle(psurface,(px_int)(pLayer->currentKeyPoint.x+plive->refer_x),(px_int)(pLayer->currentKeyPoint.y+plive->refer_y),3,PX_COLOR(255,255,0,0));
				PX_GeoDrawCircle(psurface,(px_int)(pLayer->currentKeyPoint.x+plive->refer_x),(px_int)(pLayer->currentKeyPoint.y+plive->refer_y),5,1,PX_COLOR(255,255,0,0));
			}
			
		}
	}

	if (plive->showlinker)
	{
		for (i=0;i<plive->layers.size;i++)
		{
			px_int j;
			PX_LiveLayer *pLayer=PX_VECTORAT(PX_LiveLayer,&plive->layers,i);

			if (pLayer->parent_index!=-1)
			{
				PX_GeoDrawSolidCircle(psurface,(px_int)(pLayer->currentKeyPoint.x+plive->refer_x),(px_int)(pLayer->currentKeyPoint.y+plive->refer_y),3,PX_COLOR(255,0,64,192));
				PX_GeoDrawCircle(psurface,(px_int)(pLayer->currentKeyPoint.x+plive->refer_x),(px_int)(pLayer->currentKeyPoint.y+plive->refer_y),5,1,PX_COLOR(255,0,64,192));
			}
			else
			{
				PX_GeoDrawSolidCircle(psurface,(px_int)(pLayer->currentKeyPoint.x+plive->refer_x),(px_int)(pLayer->currentKeyPoint.y+plive->refer_y),3,PX_COLOR(255,255,0,0));
				PX_GeoDrawCircle(psurface,(px_int)(pLayer->currentKeyPoint.x+plive->refer_x),(px_int)(pLayer->currentKeyPoint.y+plive->refer_y),5,1,PX_COLOR(255,255,0,0));
			}

			for (j=0;j<PX_LIVE_LAYER_MAX_LINK_NODE;j++)
			{
				
				PX_LiveLayer *pLinkLayer=PX_LiveFrameworkGetLayerChild(plive,pLayer,pLayer->child_index[j]);

				if (!pLinkLayer)
				{
					break;
				}

				PX_GeoDrawArrow(psurface,
					PX_POINT2D(pLayer->currentKeyPoint.x+plive->refer_x,pLayer->currentKeyPoint.y+plive->refer_y),
					PX_POINT2D(pLinkLayer->currentKeyPoint.x+plive->refer_x,pLinkLayer->currentKeyPoint.y+plive->refer_y),
					1,
					PX_COLOR(255,255,0,0)
					);

			}
		}
	}


	if (plive->showRange)
	{
		PX_GeoDrawBorder(psurface,x,y,x+plive->width,y+plive->height,1,PX_COLOR(255,0,0,255));
	}

	if (plive->showRootHelperLine)
	{
		PX_LiveLayer *pLayer=PX_LiveFrameworkGetCurrentEditLiveLayer(plive);
		if (pLayer&&pLayer->parent_index==-1)
		{
			PX_GeoDrawLine(psurface,(px_int)(x+pLayer->keyPoint.x),(px_int)(y+pLayer->keyPoint.y),(px_int)(x+pLayer->currentKeyPoint.x),(px_int)(y+pLayer->currentKeyPoint.y),1,PX_COLOR(255,255,0,255));
		}
	}

}

px_void PX_LiveFrameworkRenderRefer(px_surface *psurface,PX_LiveFramework *plive,PX_ALIGN refPoint,px_dword elapsed)
{
	PX_LiveFrameworkRender(psurface,plive,(px_int)plive->refer_x,(px_int)plive->refer_y,refPoint,elapsed);
}



px_bool PX_LiveFrameworkPlayAnimation(PX_LiveFramework *plive,px_int index)
{
	if (index>=0&&index<plive->liveAnimations.size)
	{
		plive->currentEditFrameIndex=-1;
		plive->currentEditLayerIndex=-1;
		plive->currentEditVertexIndex=-1;
		plive->reg_animation=index;
		plive->reg_ip = 0;
		plive->status=PX_LIVEFRAMEWORK_STATUS_PLAYING;
		return PX_TRUE;
	}
	return PX_FALSE;
}

px_bool PX_LiveFrameworkPlayAnimationByName(PX_LiveFramework *plive,const px_char name[])
{
	px_int i;
	for (i=0;i<plive->liveAnimations.size;i++)
	{
		PX_LiveAnimation *pAnimation=PX_VECTORAT(PX_LiveAnimation,&plive->liveAnimations,i);
		if (PX_strequ(name,pAnimation->id))
		{
			PX_LiveFrameworkPlayAnimation(plive,i);
			return PX_TRUE;
		}
	}
	return PX_FALSE;
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
	px_int i;

	if (plive->layers.size>=PX_LIVEFRAMEWORK_MAX_SUPPORT_LAYER)
	{
		return PX_NULL;
	}

	if (PX_LiveFrameworkGetLayerById(plive,id))
	{
		return PX_NULL;
	}


	PX_memset(&layer,0,sizeof(layer));

	if(!PX_VectorInitialize(plive->mp,&layer.triangles,sizeof(PX_Delaunay_Triangle),0))return PX_FALSE;
	if(!PX_VectorInitialize(plive->mp,&layer.vertices,sizeof(PX_LiveVertex),0))return PX_FALSE;


	layer.rotationAngle=0;
	layer.visible=PX_TRUE;
	layer.keyPoint.z=1;

	layer.rel_beginStretch=1;
	layer.rel_currentStretch=1;
	layer.rel_endStretch=1;

	layer.RenderTextureIndex = -1;
	layer.LinkTextureIndex = -1;

	layer.parent_index=-1;
	for (i=0;i<PX_COUNTOF(layer.child_index);i++)
	{
		layer.child_index[i]=-1;
	}

	PX_strcpy(layer.id,id,sizeof(layer.id));

	if(!PX_VectorPushback(&plive->layers,&layer))
		return PX_FALSE;

	return PX_VECTORLAST(PX_LiveLayer,&plive->layers);

}

PX_LiveLayer * PX_LiveFrameworkGetLayerParent(PX_LiveFramework *plive,PX_LiveLayer *pLayer)
{
	if (pLayer->parent_index>=0&&pLayer->parent_index<plive->layers.size)
	{
		return PX_VECTORAT(PX_LiveLayer,&plive->layers,pLayer->parent_index);
	}
	
#ifdef PX_DEBUG_MODE
	if (pLayer->parent_index!=-1)
	{
		//Data Crash
		PX_ASSERT();
	}
#endif
	
	return PX_NULL;
}

PX_LiveLayer * PX_LiveFrameworkGetLayerChild(PX_LiveFramework *plive,PX_LiveLayer *pLayer,px_int childIndex)
{
	if (childIndex>=0&&childIndex<plive->layers.size)
	{
		return PX_VECTORAT(PX_LiveLayer,&plive->layers,childIndex);
	}

	if (childIndex!=-1)
	{
		PX_ASSERT();
	}
	
	return PX_NULL;
}

px_bool PX_LiveFrameworkLinkLayerTexture(PX_LiveFramework *plive,const px_char layer_id[],const px_char texture_id[])
{
	px_int index=PX_LiveFrameworkGetLiveTextureIndexById(plive,texture_id);
	PX_LiveTexture *pLiveTexture=PX_LiveFrameworkGetLiveTextureById(plive,texture_id);
	PX_LiveLayer *pLayer=PX_LiveFrameworkGetLayerById(plive,layer_id);
	if (pLiveTexture&&pLayer)
	{
		pLayer->LinkTextureIndex=index;
		pLayer->RenderTextureIndex=index;
		pLayer->keyPoint.x=pLiveTexture->textureOffsetX+pLiveTexture->Texture.width/2.0f;
		pLayer->keyPoint.y=pLiveTexture->textureOffsetY+pLiveTexture->Texture.height/2.0f;
		pLayer->rel_currentTranslation=PX_POINT(0,0,0);
		return PX_TRUE;
	}
	return PX_FALSE;
}

PX_LiveLayer * PX_LiveFrameworkGetLayer(PX_LiveFramework *plive,px_int i)
{
	if (i>=0&&i<plive->layers.size)
	{
		return PX_VECTORAT(PX_LiveLayer,&plive->layers,i);
	}
	return PX_NULL;
}

px_int PX_LiveFrameworkGetLayerIndex(PX_LiveFramework *plive,PX_LiveLayer *pLayer)
{
	px_int i;
	for (i=0;i<plive->layers.size;i++)
	{
		if (pLayer==PX_VECTORAT(PX_LiveLayer,&plive->layers,i))
		{
			return i;
		}
	}
	return -1;
}

PX_LiveLayer *PX_LiveFrameworkGetLastCreateLayer(PX_LiveFramework *plive)
{
	if (plive->layers.size)
	{
		return PX_VECTORLAST(PX_LiveLayer,&plive->layers);
	}
	return PX_NULL;
}

px_void PX_LiveFrameworkUpdateLayerSourceVerticesUV(PX_LiveFramework *plive,PX_LiveLayer *pLayer)
{
	px_int i;
	for (i=0;i<pLayer->vertices.size;i++)
	{
		PX_LiveVertex *pVertex=PX_VECTORAT(PX_LiveVertex,&pLayer->vertices,i);
		
		if (pLayer->LinkTextureIndex>=0&&pLayer->LinkTextureIndex<plive->livetextures.size)
		{
			PX_LiveTexture *pLiveTexture=PX_VECTORAT(PX_LiveTexture,&plive->livetextures,pLayer->LinkTextureIndex);
			pVertex->u=(pVertex->sourcePosition.x-pLiveTexture->textureOffsetX)*1.0f/pLiveTexture->Texture.width;
			pVertex->v=(pVertex->sourcePosition.y-pLiveTexture->textureOffsetY)*1.0f/pLiveTexture->Texture.height;
		}
	}
}

px_void PX_LiveFrameworkUpdateSourceVerticesUV(PX_LiveFramework *plive)
{
	px_int i;
	for (i=0;i<plive->layers.size;i++)
	{
		PX_LiveLayer *pLayer=PX_VECTORAT(PX_LiveLayer,&plive->layers,i);
		PX_LiveFrameworkUpdateLayerSourceVerticesUV(plive,pLayer);
	}
}

px_void PX_LiveFrameworkUpdateLayerRenderVerticesUV(PX_LiveFramework *plive,PX_LiveLayer *pLayer)
{
	px_int i;
	for (i=0;i<pLayer->vertices.size;i++)
	{
		PX_LiveVertex *pVertex=PX_VECTORAT(PX_LiveVertex,&pLayer->vertices,i);

		if (pLayer->RenderTextureIndex>=0&&pLayer->RenderTextureIndex<plive->livetextures.size)
		{
			PX_LiveTexture *pLiveTexture=PX_VECTORAT(PX_LiveTexture,&plive->livetextures,pLayer->RenderTextureIndex);
			pVertex->u=(pVertex->sourcePosition.x-pLiveTexture->textureOffsetX)*1.0f/pLiveTexture->Texture.width;
			pVertex->v=(pVertex->sourcePosition.y-pLiveTexture->textureOffsetY)*1.0f/pLiveTexture->Texture.height;
		}
	}
}

PX_LiveAnimation * PX_LiveFrameworkGetAnimationById(PX_LiveFramework *plive,const px_char id[])
{
	px_int i;
	for (i=0;i<plive->liveAnimations.size;i++)
	{
		PX_LiveAnimation *pAnimation=PX_LiveFrameworkGetAnimation(plive,i);
		if (pAnimation&&PX_strequ(pAnimation->id,id))
		{
			return pAnimation;
		}
	}
	return PX_NULL;

}

PX_LiveAnimation * PX_LiveFrameworkCreateAnimation(PX_LiveFramework *plive,const px_char id[])
{
	PX_LiveAnimation animation;
	PX_memset(&animation,0,sizeof(animation));
	PX_VectorInitialize(plive->mp,&animation.framesMemPtr,sizeof(px_void *),1);
	PX_strcpy(animation.id,id,sizeof(animation.id));
	if(!PX_VectorPushback(&plive->liveAnimations,&animation))return PX_NULL;
	return PX_VECTORLAST(PX_LiveAnimation,&plive->liveAnimations);
}

PX_LiveAnimation * PX_LiveFrameworkGetAnimation(PX_LiveFramework *plive,px_int index)
{
	if (index>=0&&index<plive->liveAnimations.size)
	{
		return PX_VECTORAT(PX_LiveAnimation,&plive->liveAnimations,index);
	}
	return PX_NULL;
}

PX_LiveAnimation * PX_LiveFrameworkGetLastCreateAnimation(PX_LiveFramework *plive)
{
	if (plive->liveAnimations.size)
	{
		return PX_VECTORLAST(PX_LiveAnimation,&plive->liveAnimations);
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

px_int PX_LiveFrameworkGetLiveTextureIndexById(PX_LiveFramework *plive,const px_char id[])
{
	px_int i;
	for (i=0;i<plive->livetextures.size;i++)
	{
		PX_LiveTexture *pTexture=PX_VECTORAT(PX_LiveTexture,&plive->livetextures,i);
		if (PX_strequ(id,pTexture->id))
		{
			return i;
		}
	}
	return -1;
}

px_int PX_LiveFrameworkGetLiveTextureIndex(PX_LiveFramework *plive,PX_LiveTexture *pCompareTexture)
{
	px_int i;
	for (i=0;i<plive->livetextures.size;i++)
	{
		PX_LiveTexture *pTexture=PX_VECTORAT(PX_LiveTexture,&plive->livetextures,i);
		if (pTexture==pCompareTexture)
		{
			return i;
		}
	}
	return -1;
}

PX_LiveTexture * PX_LiveFrameworkGetLiveTexture(PX_LiveFramework *plive,px_int index)
{
	if (index>=0&&index<plive->livetextures.size)
	{
		return PX_VECTORAT(PX_LiveTexture,&plive->livetextures,index);
	}
	return PX_NULL;
}

px_void PX_LiveFrameworkDeleteLiveTextureById(PX_LiveFramework *plive,const px_char id[])
{
	px_int i;
	for (i=0;i<plive->livetextures.size;i++)
	{
		PX_LiveTexture *pTexture=PX_VECTORAT(PX_LiveTexture,&plive->livetextures,i);
		if (PX_strequ(id,pTexture->id))
		{
			PX_LiveFrameworkDeleteLiveTexture(plive,i);
			return;
		}
	}
}

px_void PX_LiveFrameworkDeleteLiveAnimationById(PX_LiveFramework *plive,const px_char id[])
{
	px_int i;
	for (i=0;i<plive->liveAnimations.size;i++)
	{
		PX_LiveAnimation *pAnimation=PX_VECTORAT(PX_LiveAnimation,&plive->liveAnimations,i);
		if (PX_strequ(id,pAnimation->id))
		{
			PX_LiveFrameworkDeleteLiveAnimation(plive,i);
			return;
		}
	}
}

px_bool PX_LiveFrameworkLinkLayerSearchSubLayer(PX_LiveFramework *plive,PX_LiveLayer *pLayer,PX_LiveLayer *pSearchLayer)
{
	px_int i;
	for (i=0;i<PX_LIVE_LAYER_MAX_LINK_NODE;i++)
	{
		PX_LiveLayer *pSubLinkLayer=PX_LiveFrameworkGetLayerChild(plive,pLayer,pLayer->child_index[i]);

		if (pSubLinkLayer==PX_NULL)
		{
			return PX_FALSE;
		}

		if (pSubLinkLayer==pSearchLayer)
		{
			return PX_TRUE;
		}
		else
		{
			if(pSubLinkLayer!=PX_NULL)
			return PX_LiveFrameworkLinkLayerSearchSubLayer(plive,pSubLinkLayer,pSearchLayer);
		}
	}
	return PX_FALSE;
}

px_void PX_LiveFrameworkLinkLayer(PX_LiveFramework *plive,PX_LiveLayer *pLayer,PX_LiveLayer *linkLayer)
{
	px_int i;
	if (pLayer==linkLayer)
	{
		return;
	}

	if (PX_LiveFrameworkLinkLayerSearchSubLayer(plive,pLayer,pLayer))
	{
		return;
	}

	for (i=0;i<PX_LIVE_LAYER_MAX_LINK_NODE;i++)
	{
		PX_LiveLayer *pSubLinkLayer=PX_LiveFrameworkGetLayerChild(plive,pLayer,pLayer->child_index[i]);

		if (!pSubLinkLayer)
		{
			break;
		}

		if (pSubLinkLayer==linkLayer)
		{
			return;
		}

	}
	if(i<PX_LIVE_LAYER_MAX_LINK_NODE)
	{
		if (linkLayer->parent_index==-1)
		{
			pLayer->child_index[i]=PX_LiveFrameworkGetLayerIndex(plive,linkLayer);
			linkLayer->parent_index=PX_LiveFrameworkGetLayerIndex(plive,pLayer);
		}
	}
}

px_void PX_LiveFrameworkClearLinker(PX_LiveFramework *plive)
{
	px_int i;
	for (i=0;i<plive->layers.size;i++)
	{
		px_int j;
		PX_LiveLayer *pLayer=PX_VECTORAT(PX_LiveLayer,&plive->layers,i);
		for (j=0;j<PX_COUNTOF(pLayer->child_index);j++)
		{
			pLayer->child_index[j]=-1;
		}
		pLayer->parent_index=-1;
	}
}

px_void PX_LiveFrameworkDeleteLayer(PX_LiveFramework *plive,px_int index)
{
	if (index>=0&&index<plive->layers.size)
	{
		px_int i,j;
		PX_LiveLayer *pLayer=PX_VECTORAT(PX_LiveLayer,&plive->layers,index);
		for (i=0;i<plive->layers.size;i++)
		{
			PX_LiveLayer *pSearchLayer=PX_VECTORAT(PX_LiveLayer,&plive->layers,i);

			if (pSearchLayer->parent_index == index)
			{
				pSearchLayer->parent_index = -1;
			}
			else if (pSearchLayer->parent_index > index)
			{
				pSearchLayer->parent_index--;
			}

			for (j=0;j<PX_LIVE_LAYER_MAX_LINK_NODE;j++)
			{
				if (pSearchLayer->child_index[j]==index)
				{
					px_int k;
					for (k=j;k<PX_COUNTOF(pLayer->child_index);k++)
					{
						pSearchLayer->child_index[k]=pSearchLayer->child_index[k+1];
						if (pSearchLayer->child_index[k]==-1)
						{
							break;
						}
					}
				}
				
				if (pSearchLayer->child_index[j] > index)
				{
					pSearchLayer->child_index[j]--;
				}
			}
		}

		PX_VectorFree(&pLayer->vertices);
		PX_VectorFree(&pLayer->triangles);
		PX_VectorErase(&plive->layers,index);
	}
}

px_void PX_LiveFrameworkDeleteLiveTexture(PX_LiveFramework *plive,px_int index)
{
	if (index>=0&&index<plive->livetextures.size)
	{
		px_int i;
		PX_LiveTexture *pTexture=PX_VECTORAT(PX_LiveTexture,&plive->livetextures,index);
		for (i=0;i<plive->layers.size;i++)
		{
			PX_LiveLayer *player=PX_VECTORAT(PX_LiveLayer,&plive->layers,i);
			if (player->LinkTextureIndex>=index)
			{
				if (player->LinkTextureIndex>index)
				{
					player->LinkTextureIndex--;
					player->RenderTextureIndex=player->LinkTextureIndex;
				}
				else
				{
					player->LinkTextureIndex=-1;
					player->RenderTextureIndex=player->LinkTextureIndex;
				}
				
			}
		}
		PX_TextureFree(&pTexture->Texture);
		PX_VectorErase(&plive->livetextures,index);
	}
}

px_void PX_LiveFrameworkDeleteLiveAnimationFrame(PX_LiveFramework *plive,PX_LiveAnimation *pliveAnimation,px_int index)
{
	if (index>=0&&index<pliveAnimation->framesMemPtr.size)
	{
		px_void *pFrameMemories=*PX_VECTORAT(px_void*,&pliveAnimation->framesMemPtr,index);
		if(pFrameMemories)
			MP_Free(plive->mp,pFrameMemories);
		PX_VectorErase(&pliveAnimation->framesMemPtr,index);
	}
}

px_void PX_LiveFrameworkDeleteLiveAnimation(PX_LiveFramework *plive,px_int index)
{
	if (index>=0&&index<plive->liveAnimations.size)
	{
		PX_LiveAnimation *pAnimation=PX_VECTORAT(PX_LiveAnimation,&plive->liveAnimations,index);
		while (pAnimation->framesMemPtr.size)
		{
			PX_LiveFrameworkDeleteLiveAnimationFrame(plive,pAnimation,0);
		}
		PX_VectorFree(&pAnimation->framesMemPtr);
		PX_VectorErase(&plive->liveAnimations,index);

		plive->currentEditAnimationIndex=-1;
		plive->currentEditFrameIndex=-1;
		plive->currentEditLayerIndex=-1;
		plive->currentEditVertexIndex=1;
	}
}

px_void PX_LiveFrameworkDeleteLiveAnimationFrameByIndex(PX_LiveFramework *plive,px_int AnimationIndex,px_int frameIndex)
{
	if (AnimationIndex>=0&&AnimationIndex<plive->liveAnimations.size)
	{
		PX_LiveAnimation *pAnimation=PX_VECTORAT(PX_LiveAnimation,&plive->liveAnimations,AnimationIndex);
		if (frameIndex>=0&&frameIndex<pAnimation->framesMemPtr.size)
		{
			PX_LiveFrameworkDeleteLiveAnimationFrame(plive,pAnimation,frameIndex);
		}
		PX_VectorErase(&pAnimation->framesMemPtr,frameIndex);
	}
}

px_void PX_LiveFrameworkFree(PX_LiveFramework *plive)
{
	while (plive->liveAnimations.size)
	{
		PX_LiveFrameworkDeleteLiveAnimation(plive,0);
	}
	while (plive->layers.size)
	{
		PX_LiveFrameworkDeleteLayer(plive,0);
	}
	while(plive->livetextures.size)
	{
		PX_LiveFrameworkDeleteLiveTexture(plive,0);
	}
	PX_VectorFree(&plive->liveAnimations);
	PX_VectorFree(&plive->layers);
	PX_VectorFree(&plive->livetextures);
}

px_void * PX_LiveFrameworkGetCurrentEditFrame(PX_LiveFramework *plive)
{
	if (plive->currentEditAnimationIndex>=0&&plive->currentEditAnimationIndex<plive->liveAnimations.size)
	{
		PX_LiveAnimation *pAnimation=PX_VECTORAT(PX_LiveAnimation,&plive->liveAnimations,plive->currentEditAnimationIndex);
		if (plive->currentEditFrameIndex>=0&&plive->currentEditFrameIndex<pAnimation->framesMemPtr.size)
		{
			return *PX_VECTORAT(px_void *,&pAnimation->framesMemPtr,plive->currentEditFrameIndex);
		}
	}
	return PX_NULL;
}

PX_LiveLayer * PX_LiveFrameworkGetCurrentEditLiveLayer(PX_LiveFramework *plive)
{
	if (plive->currentEditLayerIndex>=0&&plive->currentEditLayerIndex<plive->layers.size)
	{
		return PX_VECTORAT(PX_LiveLayer,&plive->layers,plive->currentEditLayerIndex);
	}
	return PX_NULL;
}

PX_LiveAnimationFramePayload *PX_LiveFrameworkGetCurrentEditAnimationFramePayloadIndex(PX_LiveFramework *plive,px_int payloadIndex)
{
	px_int boffset=0;
	px_int FramePayloadSize;
	px_byte *pOffset;
	PX_LiveAnimationFrameHeader *pHeader;
	pOffset=(px_byte *)PX_LiveFrameworkGetCurrentEditFrame(plive);
	if (!pOffset)
	{
		return PX_NULL;
	}

	pHeader=(PX_LiveAnimationFrameHeader *)pOffset;
	FramePayloadSize=pHeader->size+sizeof(PX_LiveAnimationFrameHeader);
	boffset+=sizeof(PX_LiveAnimationFrameHeader);
	while (PX_TRUE)
	{
		PX_LiveAnimationFramePayload *pPayloadHeder;
		if (boffset>FramePayloadSize)
		{
			break;
		}
		pPayloadHeder=(PX_LiveAnimationFramePayload *)(pOffset+boffset);
		if (payloadIndex<=0)
		{
			return pPayloadHeder;
		}
		boffset+=sizeof(PX_LiveAnimationFramePayload);
		boffset+=pPayloadHeder->translationVerticesCount*sizeof(px_point);
		payloadIndex--;
	}
	return PX_NULL;
}

PX_LiveAnimationFramePayload * PX_LiveFrameworkGetCurrentEditAnimationFramePayload(PX_LiveFramework *plive)
{
	return PX_LiveFrameworkGetCurrentEditAnimationFramePayloadIndex(plive,plive->currentEditLayerIndex);
}

px_void PX_LiveFrameworkCurrentEditMoveFrameDown(PX_LiveFramework *plive)
{
	PX_LiveAnimation *pAnimation=PX_LiveFrameworkGetCurrentEditAnimation(plive);
	if (!pAnimation)
	{
		return;
	}
	if (plive->currentEditFrameIndex>=0&&plive->currentEditFrameIndex<pAnimation->framesMemPtr.size)
	{
		if (plive->currentEditFrameIndex==pAnimation->framesMemPtr.size-1)
		{
			return;
		}
		else
		{
			px_void **ptr1,**ptr2,*temp;
			ptr1=PX_VECTORAT(px_void *,&pAnimation->framesMemPtr,plive->currentEditFrameIndex);
			ptr2=PX_VECTORAT(px_void *,&pAnimation->framesMemPtr,plive->currentEditFrameIndex+1);
			temp=*ptr1;
			*ptr1=*ptr2;
			*ptr2=temp;
		}
	}
}

px_void PX_LiveFrameworkCurrentEditMoveFrameUp(PX_LiveFramework *plive)
{
	PX_LiveAnimation *pAnimation=PX_LiveFrameworkGetCurrentEditAnimation(plive);
	if (!pAnimation)
	{
		return;
	}
	if (plive->currentEditFrameIndex>=0&&plive->currentEditFrameIndex<pAnimation->framesMemPtr.size)
	{
		if (plive->currentEditFrameIndex==0)
		{
			return;
		}
		else
		{
			px_void **ptr1,**ptr2,*temp;
			ptr1=PX_VECTORAT(px_void *,&pAnimation->framesMemPtr,plive->currentEditFrameIndex);
			ptr2=PX_VECTORAT(px_void *,&pAnimation->framesMemPtr,plive->currentEditFrameIndex-1);
			temp=*ptr1;
			*ptr1=*ptr2;
			*ptr2=temp;
		}
	}
}

PX_LiveAnimation * PX_LiveFrameworkGetCurrentEditAnimation(PX_LiveFramework *plive)
{
	if (plive->currentEditAnimationIndex>=0&&plive->currentEditAnimationIndex<plive->liveAnimations.size)
	{
		PX_LiveAnimation *pAnimation=PX_VECTORAT(PX_LiveAnimation,&plive->liveAnimations,plive->currentEditAnimationIndex);
		return pAnimation;
	}
	return PX_NULL;
}

PX_LiveAnimationFrameHeader * PX_LiveFrameworkGetCurrentEditAnimationFrame(PX_LiveFramework *plive)
{
	PX_LiveAnimation *pAnimation=PX_LiveFrameworkGetCurrentEditAnimation(plive);
	if (pAnimation)
	{
		if (plive->currentEditFrameIndex>=0&&plive->currentEditFrameIndex<pAnimation->framesMemPtr.size)
		{
			return (PX_LiveAnimationFrameHeader *)(*PX_VECTORAT(px_void*,&pAnimation->framesMemPtr,plive->currentEditFrameIndex));
		}
	}
	return PX_NULL;
}

PX_LiveVertex * PX_LiveFrameworkGetCurrentEditLiveVertex(PX_LiveFramework *plive)
{
	PX_LiveLayer *currentEditLayer=PX_LiveFrameworkGetCurrentEditLiveLayer(plive);
	if (currentEditLayer)
	{
		if (plive->currentEditVertexIndex>=0&&plive->currentEditVertexIndex<currentEditLayer->vertices.size)
		{
			return PX_VECTORAT(PX_LiveVertex,&currentEditLayer->vertices,plive->currentEditVertexIndex);
		}
	}
	return PX_NULL;
}

px_point * PX_LiveFrameworkGetCurrentEditAnimationFramePayloadVertex(PX_LiveFramework *plive)
{
	px_byte *pPayloadByte=(px_byte *)PX_LiveFrameworkGetCurrentEditAnimationFramePayload(plive);
	PX_LiveAnimationFramePayload *ppayload=(PX_LiveAnimationFramePayload *)pPayloadByte;
	if (pPayloadByte)
	{
		if (plive->currentEditVertexIndex>=0&&plive->currentEditVertexIndex<(px_int)ppayload->translationVerticesCount)
		{
			return (px_point *)(pPayloadByte+sizeof(PX_LiveAnimationFramePayload)+plive->currentEditVertexIndex*sizeof(px_point));
		}
	}
	return PX_NULL;
}

px_void PX_LiveFrameworkDeleteCurrentEditAnimation(PX_LiveFramework *plive)
{
	PX_LiveFrameworkDeleteLiveAnimation(plive,plive->currentEditAnimationIndex);
}

px_void PX_LiveFrameworkDeleteCurrentEditAnimationFrame(PX_LiveFramework *plive)
{
	PX_LiveAnimation *pAnimation=PX_LiveFrameworkGetCurrentEditAnimation(plive);
	if (pAnimation)
	{
		PX_LiveFrameworkDeleteLiveAnimationFrame(plive,pAnimation,plive->currentEditFrameIndex);
	}
	
}

px_bool PX_LiveFrameworkNewEditFrame(PX_LiveFramework *plive,px_char id[],px_bool bCopyFrame)
{
	PX_LiveAnimation *pAnimation;
	px_void *pFramebuffer;
	pAnimation=PX_LiveFrameworkGetCurrentEditAnimation(plive);
	if (!pAnimation)
	{
		return PX_FALSE;
	}
	pFramebuffer=PX_LiveFrameworkGetCurrentEditAnimationFrame(plive);

	if (pFramebuffer)
	{
		px_uint size;
		PX_LiveAnimationFrameHeader *pHeader;
		px_void *newFramePtr;
		px_void *buffer=pFramebuffer;
		pHeader=(PX_LiveAnimationFrameHeader *)buffer;
		size=pHeader->size+sizeof(PX_LiveAnimationFrameHeader);
		newFramePtr=MP_Malloc(plive->mp,size);
		if (newFramePtr)
		{
			PX_memcpy(newFramePtr,buffer,size);
			PX_memcpy(pHeader->frameid,id,sizeof(pHeader->frameid));
			if (bCopyFrame)
			{
				PX_VectorPushTo(&pAnimation->framesMemPtr,&newFramePtr,plive->currentEditFrameIndex+1);
			}
			else
			{
				PX_VectorPushback(&pAnimation->framesMemPtr,&newFramePtr);
			}
			
			return PX_TRUE;
		}
		else
		{
			return PX_FALSE;
		}
	}
	else
	{
		px_void *newFramePtr;
		//create new frame
		px_int i;
		px_uint size=0;
		//header
		size+=sizeof(PX_LiveAnimationFrameHeader);
		for (i=0;i<plive->layers.size;i++)
		{
			PX_LiveLayer *player=PX_VECTORAT(PX_LiveLayer,&plive->layers,i);
			//payload
			size+=sizeof(PX_LiveAnimationFramePayload);
			//translation
			size+=sizeof(px_point)*player->vertices.size;
		}
		newFramePtr=MP_Malloc(plive->mp,size);

		if (newFramePtr)
		{
			PX_LiveAnimationFrameHeader *pHeader=(PX_LiveAnimationFrameHeader *)newFramePtr;
			PX_LiveAnimationFramePayload *pPayload;
			px_byte *pdata;
			px_int offset=0;
			PX_memset(newFramePtr,0,size);
			pdata=(px_byte *)newFramePtr;
			pHeader->size=size-sizeof(PX_LiveAnimationFrameHeader);
			offset+=sizeof(PX_LiveAnimationFrameHeader);
			for (i=0;i<plive->layers.size;i++)
			{
				PX_LiveLayer *player=PX_VECTORAT(PX_LiveLayer,&plive->layers,i);
				pPayload=(PX_LiveAnimationFramePayload *)(pdata+offset);
				pPayload->translationVerticesCount=player->vertices.size;
				pPayload->mapTexture=player->LinkTextureIndex;
				pPayload->stretch=1;
				offset+=sizeof(PX_LiveAnimationFramePayload)+sizeof(px_point)*player->vertices.size;
			}
			PX_memcpy(pHeader->frameid,id,sizeof(pHeader->frameid));
			return PX_VectorPushback(&pAnimation->framesMemPtr,&newFramePtr);
		}
		else
		{
			return PX_FALSE;
		}
	}


	
	
	return PX_FALSE;
}

px_void PX_LiveFrameworkRunCurrentEditFrame(PX_LiveFramework *plive)
{
	PX_LiveFrameworkExecuteInstr(plive,plive->currentEditAnimationIndex,plive->currentEditFrameIndex);
	PX_LiveFrameworkExecuteInstr(plive,plive->currentEditAnimationIndex,plive->currentEditFrameIndex);
}

px_bool PX_LiveFrameworkExport(PX_LiveFramework *plive,px_memory *exportbuffer)
{
	//header
	if(!PX_MemoryCat(exportbuffer,"PainterEngineLiveDBinary",24))return PX_FALSE;

	//export LiveFramework structure
	do 
	{

		typedef struct  
		{
			px_char id[PX_LIVE_ID_MAX_LEN];
			px_dword version;
			px_int32 width;
			px_int32 height;
			px_int32 layerCount;
			px_int32 animationCount;
			px_int32 textureCount;
		}PX_LiveFrameworkBaseAttributes;

		PX_LiveFrameworkBaseAttributes desc;

		PX_memset(&desc,0,sizeof(PX_LiveFrameworkBaseAttributes));
		
		PX_memcpy(desc.id,plive->id,PX_LIVE_ID_MAX_LEN);
		desc.version = PX_LIVE_VERSION;

		desc.width=plive->width;
		
		desc.height=plive->height;

		desc.layerCount=plive->layers.size;

		desc.animationCount=plive->liveAnimations.size;

		desc.textureCount=plive->livetextures.size;
		
		if(!PX_MemoryCat(exportbuffer,&desc,sizeof(desc)))return PX_FALSE;
	} while (0);

	//export live texture
	do 
	{
		typedef struct  
		{
			px_char id[PX_LIVE_ID_MAX_LEN];
			px_int32 width;
			px_int32 height;
			px_int32 textureOffsetX;
			px_int32 textureOffsetY;
		}PX_LiveTextureExportInfo;

		px_int i;
		for (i=0;i<plive->livetextures.size;i++)
		{
			PX_LiveTexture *pTexture=PX_VECTORAT(PX_LiveTexture,&plive->livetextures,i);
			
			//export live texture structure
			do 
			{
				PX_LiveTextureExportInfo desc;
				desc.height=pTexture->Texture.height;
				desc.width=pTexture->Texture.width;
				PX_memcpy(desc.id,pTexture->id,sizeof(desc.id));
				desc.textureOffsetX=pTexture->textureOffsetX;
				desc.textureOffsetY=pTexture->textureOffsetY;
				if(!PX_MemoryCat(exportbuffer,&desc,sizeof(desc)))return PX_FALSE;
			} while (0);
			
			//export pixels data
			do 
			{
				if(!PX_MemoryCat(exportbuffer,pTexture->Texture.surfaceBuffer,sizeof(px_color)*pTexture->Texture.height*pTexture->Texture.width))return PX_FALSE;
			} while (0);
		}
	} while (0);

	//export layers
	do 
	{
		typedef struct  
		{
			px_char id[PX_LIVE_ID_MAX_LEN];
			px_int32 parent_index;
			px_int32 child_index[PX_LIVE_LAYER_MAX_LINK_NODE];
			px_int32 triangleCount;
			px_int32 verticesCount;
			px_point32 KeyPoint;
			px_int32  LinkTextureIndex;
		}PX_LiveFramework_LayerExportInfo;
		px_int i;
		for (i=0;i<plive->layers.size;i++)
		{
			PX_LiveLayer *pLayer=PX_VECTORAT(PX_LiveLayer,&plive->layers,i);
			PX_LiveFramework_LayerExportInfo desc;
			PX_memcpy(desc.id,pLayer->id,sizeof(desc.id));

			desc.triangleCount=pLayer->triangles.size;
			desc.verticesCount=pLayer->vertices.size;

			desc.parent_index=pLayer->parent_index;

			do 
			{
				px_int j;
				for (j=0;j<PX_COUNTOF(pLayer->child_index);j++)
				{
					desc.child_index[j]=pLayer->child_index[j];
				}
			} while (0);

			desc.KeyPoint=pLayer->keyPoint;
			desc.LinkTextureIndex=pLayer->LinkTextureIndex;

			if(!PX_MemoryCat(exportbuffer,&desc,sizeof(desc)))return PX_FALSE;
			
			
			//export layer triangles
			do 
			{
				if(!PX_MemoryCat(exportbuffer,pLayer->triangles.data,pLayer->triangles.nodesize*pLayer->triangles.size))return PX_FALSE;
			} while (0);

			//export layer vertex
			do 
			{
				if(!PX_MemoryCat(exportbuffer,pLayer->vertices.data,pLayer->vertices.nodesize*pLayer->vertices.size))return PX_FALSE;
			} while (0);
		}
	} while (0);

	//export live animation
	do 
	{
		px_int i;
		for (i=0;i<plive->liveAnimations.size;i++)
		{
			PX_LiveAnimation *pAnimation=PX_VECTORAT(PX_LiveAnimation,&plive->liveAnimations,i);
			//export Live Animation structure
			do 
			{
				typedef struct  
				{
					px_char id[PX_LIVE_ID_MAX_LEN];
					px_int32  size;
				}PX_LiveAnimationExportInfo;

				PX_LiveAnimationExportInfo desc;
				PX_memcpy(desc.id,pAnimation->id,sizeof(desc.id));
				desc.size=pAnimation->framesMemPtr.size;
				if(!PX_MemoryCat(exportbuffer,&desc,sizeof(desc)))return PX_FALSE;
			} while (0);
			
			//export live animation frame
			do 
			{
				px_int j;
				for (j=0;j<pAnimation->framesMemPtr.size;j++)
				{
					//export size
					px_void *pdata;
					px_int32 payloadsize;
					PX_LiveAnimationFrameHeader *pheader;
					pdata=*PX_VECTORAT(px_void*,&pAnimation->framesMemPtr,j);
					pheader=(PX_LiveAnimationFrameHeader *)pdata;
					payloadsize=sizeof(PX_LiveAnimationFrameHeader)+pheader->size;

					if(!PX_MemoryCat(exportbuffer,pdata,payloadsize))return PX_FALSE;
				}
			} while (0);


		}
	} while (0);
	return PX_TRUE;
}

px_bool PX_LiveFrameworkImport(px_memorypool *mp,PX_LiveFramework *plive,px_void *buffer,px_int size)
{
	px_byte*bBuffer = (px_byte *)buffer;
	px_int rOffset=0;
	//////////////////////////////////////////////////////////////////////////
	//check header
	
	do 
	{
		if (!PX_memequ(buffer,"PainterEngineLiveDBinary",24))
		{
			return PX_FALSE;
		}
		rOffset+=24;if(rOffset>size) return PX_FALSE;
		
	} while (0);
	
	//////////////////////////////////////////////////////////////////////////
	//import live framework structure
	do 
	{
		typedef struct  
		{
			px_char id[PX_LIVE_ID_MAX_LEN];
			px_dword version;
			px_int32 width;
			px_int32 height;
			px_int32 layerCount;
			px_int32 animationCount;
			px_int32 textureCount;
		}PX_LiveFrameworkBaseAttributes;

		PX_LiveFrameworkBaseAttributes *pReadLiveFrameworkAttributes=(PX_LiveFrameworkBaseAttributes *)(bBuffer+rOffset);
		rOffset+=sizeof(PX_LiveFrameworkBaseAttributes);if(rOffset>size) return PX_FALSE;

		//////////////////////////////////////////////////////////////////////////
		if (pReadLiveFrameworkAttributes->version != PX_LIVE_VERSION)
		{
			return PX_FALSE;
		}

		PX_memset(plive,0,sizeof(PX_LiveFramework));

		PX_memcpy(plive->id,pReadLiveFrameworkAttributes->id,sizeof(plive->id));
		plive->width=pReadLiveFrameworkAttributes->width;
		plive->height=pReadLiveFrameworkAttributes->height;

		//////////////////////////////////////////////////////////////////////////
		plive->mp=mp;
		if(!PX_VectorInitialize(mp,&plive->layers,sizeof(PX_LiveLayer),pReadLiveFrameworkAttributes->layerCount))return PX_FALSE;
		plive->layers.size=pReadLiveFrameworkAttributes->layerCount;

		if(!PX_VectorInitialize(mp,&plive->livetextures,sizeof(PX_LiveTexture),pReadLiveFrameworkAttributes->textureCount))return PX_FALSE;
		plive->livetextures.size=pReadLiveFrameworkAttributes->textureCount;

		if(!PX_VectorInitialize(mp,&plive->liveAnimations,sizeof(PX_LiveAnimation),pReadLiveFrameworkAttributes->animationCount))return PX_FALSE;
		plive->liveAnimations.size=pReadLiveFrameworkAttributes->animationCount;

		plive->reg_animation=0;
		plive->reg_bp=0;
		plive->reg_duration=0;
		plive->reg_elapsed=0;
		plive->reg_ip=0;

		plive->currentEditAnimationIndex=-1;
		plive->currentEditFrameIndex=-1;
		plive->currentEditLayerIndex=-1;
		plive->currentEditVertexIndex=-1;
	} while (0);

	
	//////////////////////////////////////////////////////////////////////////
	//
	//import live texture
	do 
	{
		typedef struct  
		{
			px_char id[PX_LIVE_ID_MAX_LEN];
			px_int32 width;
			px_int32 height;
			px_int32 textureOffsetX;
			px_int32 textureOffsetY;
		}PX_LiveTextureImportInfo;

		px_int i;
		for (i=0;i<plive->livetextures.size;i++)
		{
			PX_LiveTexture *pTexture=PX_VECTORAT(PX_LiveTexture,&plive->livetextures,i);
			do 
			{
				//import live texture structure
				PX_LiveTextureImportInfo *pLiveTextureImportInfo;
				pLiveTextureImportInfo=((PX_LiveTextureImportInfo *)(bBuffer+rOffset));
				PX_memset(pTexture,0,sizeof(PX_LiveTexture));

				if(!PX_TextureCreate(mp,&pTexture->Texture,pLiveTextureImportInfo->width,pLiveTextureImportInfo->height))
					goto _ERROR;
				PX_memcpy(pTexture->id,pLiveTextureImportInfo->id,sizeof(pTexture->id));
				pTexture->textureOffsetX=pLiveTextureImportInfo->textureOffsetX;
				pTexture->textureOffsetY=pLiveTextureImportInfo->textureOffsetY;

				rOffset+=sizeof(PX_LiveTextureImportInfo);if(rOffset>size) 
					goto _ERROR;
				//import texture data
				PX_memcpy(pTexture->Texture.surfaceBuffer,(bBuffer+rOffset),pTexture->Texture.width*pTexture->Texture.height*sizeof(px_color));
				rOffset+=pTexture->Texture.width*pTexture->Texture.height*sizeof(px_color);if(rOffset>size) 
					goto _ERROR;
			} while (0);
		}
	} while (0);

	//////////////////////////////////////////////////////////////////////////
	//import layer
	do 
	{
		typedef struct  
		{
			px_char id[PX_LIVE_ID_MAX_LEN];
			px_int parent_index;
			px_int child_index[PX_LIVE_LAYER_MAX_LINK_NODE];
			px_int triangleCount;
			px_int verticesCount;
			px_point KeyPoint;
			px_int  LinkTextureIndex;
		}PX_LiveFramework_LayerExportInfo;

		px_int i;
		for (i=0;i<plive->layers.size;i++)
		{
			PX_LiveLayer *pLayer=PX_VECTORAT(PX_LiveLayer,&plive->layers,i);
			PX_LiveFramework_LayerExportInfo *pReadLayer=(PX_LiveFramework_LayerExportInfo *)(bBuffer+rOffset);
			rOffset+=sizeof(PX_LiveFramework_LayerExportInfo);if(rOffset>size) goto _ERROR;

			PX_memset(pLayer,0,sizeof(PX_LiveLayer));
			PX_memcpy(pLayer->id,pReadLayer->id,sizeof(pLayer->id));
			pLayer->keyPoint=pReadLayer->KeyPoint;
			pLayer->LinkTextureIndex=pReadLayer->LinkTextureIndex;
			pLayer->RenderTextureIndex=pLayer->LinkTextureIndex;

			pLayer->rel_beginStretch=1;
			pLayer->rel_currentStretch=1;
			pLayer->rel_endStretch=1;
			pLayer->visible=PX_TRUE;

			if(!PX_VectorInitialize(mp,&pLayer->triangles,sizeof(PX_Delaunay_Triangle),pReadLayer->triangleCount)) 
				goto _ERROR;
			pLayer->triangles.size=pReadLayer->triangleCount;

			if(!PX_VectorInitialize(mp,&pLayer->vertices,sizeof(PX_LiveVertex),pReadLayer->verticesCount)) 
				goto _ERROR;
			pLayer->vertices.size=pReadLayer->verticesCount;


			pLayer->parent_index=pReadLayer->parent_index;

			do 
			{
				px_int j;
				for (j=0;j<PX_COUNTOF(pReadLayer->child_index);j++)
				{
					pLayer->child_index[j]=pReadLayer->child_index[j];
				}
			} while (0);

			
			//import layer triangles
			do 
			{
				PX_memcpy(pLayer->triangles.data,bBuffer+rOffset,pLayer->triangles.nodesize*pLayer->triangles.size);
				rOffset+=pLayer->triangles.nodesize*pLayer->triangles.size;if(rOffset>size) goto _ERROR;
			} while (0);

			//import layer vertex
			do 
			{
				PX_memcpy(pLayer->vertices.data,bBuffer+rOffset,pLayer->vertices.nodesize*pLayer->vertices.size);
				rOffset+=pLayer->vertices.nodesize*pLayer->vertices.size;if(rOffset>size) goto _ERROR;
			} while (0);
		}
	} while (0);

	//import live animation
	do 
	{
		typedef struct  
		{
			px_char id[PX_LIVE_ID_MAX_LEN];
			px_int32  size;
		}PX_LiveAnimationImportInfo;

		px_int i;
		for (i=0;i<plive->liveAnimations.size;i++)
		{
			PX_LiveAnimation *pAnimation=PX_VECTORAT(PX_LiveAnimation,&plive->liveAnimations,i);
			
			//import Live Animation structure
			do 
			{
				PX_LiveAnimationImportInfo *pLiveAnimationImport=(PX_LiveAnimationImportInfo *)(bBuffer+rOffset);
				rOffset+=sizeof(PX_LiveAnimationImportInfo);if(rOffset>size) goto _ERROR;

				PX_memcpy(pAnimation->id,pLiveAnimationImport->id,sizeof(pAnimation->id));

				if(!PX_VectorInitialize(mp,&pAnimation->framesMemPtr,sizeof(px_void *),pLiveAnimationImport->size))goto _ERROR;
				pAnimation->framesMemPtr.size=pLiveAnimationImport->size;
			} while (0);

			//import live animation frame
			do 
			{
				px_int j;
				for (j=0;j<pAnimation->framesMemPtr.size;j++)
				{
					//import size
					px_void **ppdata;
					px_int32 payloadsize;
					PX_LiveAnimationFrameHeader *pheader;
					ppdata=PX_VECTORAT(px_void*,&pAnimation->framesMemPtr,j);
					pheader=(PX_LiveAnimationFrameHeader *)(bBuffer+rOffset);
					payloadsize=sizeof(PX_LiveAnimationFrameHeader)+pheader->size;

					*ppdata=MP_Malloc(mp,payloadsize);
					if(*ppdata==PX_NULL) goto _ERROR;
					PX_memcpy(*ppdata,bBuffer+rOffset,payloadsize);
					rOffset+=payloadsize;if(rOffset>size) goto _ERROR;
				}
			} while (0);
		}
	} while (0);
	PX_LiveFrameworkReset(plive);
	return PX_TRUE;
_ERROR:
	PX_LiveFrameworkFree(plive);
	return PX_FALSE;
}

//////////////////////////////////////////////////////////////////////////
//liveframework mirror

px_bool PX_LiveCreate(px_memorypool *mp,PX_LiveFramework *pLiveFramework,PX_Live *pLive)
{
	px_int i;
	pLive->mp=mp;
	*pLive=*pLiveFramework;
	if(!PX_VectorInitialize(mp,&pLive->layers,sizeof(PX_LiveLayer),0))return PX_FALSE;
	if(!PX_VectorCopy(&pLive->layers,&pLiveFramework->layers)) return PX_FALSE;
	for (i=0;i<pLive->layers.size;i++)
	{
		PX_LiveLayer *pLayer=PX_VECTORAT(PX_LiveLayer,&pLive->layers,i);
		PX_LiveLayer *pFrameworkLayer=PX_VECTORAT(PX_LiveLayer,&pLiveFramework->layers,i);
		if(!PX_VectorInitialize(mp,&pLayer->vertices,sizeof(PX_LiveVertex),0))
		{
			px_int j;
			for (j=0;j<i;j++)
			{
				PX_LiveLayer *pLayer=PX_VECTORAT(PX_LiveLayer,&pLive->layers,j);
				PX_VectorFree(&pLayer->vertices);
			}
			return PX_FALSE;
		}
		if(!PX_VectorCopy(&pLayer->vertices,&pFrameworkLayer->vertices))
		{
			px_int j;
			for (j=0;j<i;j++)
			{
				PX_LiveLayer *pLayer=PX_VECTORAT(PX_LiveLayer,&pLive->layers,j);
				PX_VectorFree(&pLayer->vertices);
			}
			return PX_FALSE;
		}
	}
	return PX_TRUE;
}

px_void PX_LiveFree(PX_Live *pLive)
{
	px_int i;
	for (i=0;i<pLive->layers.size;i++)
	{
		PX_LiveLayer *pLayer=PX_VECTORAT(PX_LiveLayer,&pLive->layers,i);
		PX_VectorFree(&pLayer->vertices);
	}
	PX_VectorFree(&pLive->layers);
}

px_void PX_LivePlay(PX_Live*plive)
{
	PX_LiveFrameworkPlay(plive);
}

px_bool PX_LivePlayAnimation(PX_Live *plive,px_int index)
{
	return PX_LiveFrameworkPlayAnimation(plive,index);
}

px_bool PX_LivePlayAnimationByName(PX_Live *plive,const px_char name[])
{
	return PX_LiveFrameworkPlayAnimationByName(plive,name);
}

px_void PX_LivePause(PX_Live *plive)
{
	PX_LiveFrameworkPause(plive);
}

px_void PX_LiveReset(PX_Live *plive)
{
	PX_LiveFrameworkReset(plive);
}

px_void PX_LiveStop(PX_Live *plive)
{
	PX_LiveFrameworkStop(plive);
}

px_void PX_LiveRender(px_surface *psurface,PX_Live *plive,px_int x,px_int y,PX_ALIGN refPoint,px_dword elapsed)
{
	PX_LiveFrameworkRender(psurface,plive,x,y,refPoint,elapsed);
}
