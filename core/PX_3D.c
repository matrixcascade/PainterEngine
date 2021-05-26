#include "PX_3D.h"

px_void PX_3D_RenderListTransform_LocalToGlobal(PX_3D_RenderList *list,PX_3D_World *world);
px_void PX_3D_RenderListTransform_Camera(PX_3D_RenderList *list,PX_3D_Camera *camera);
px_void PX_3D_RenderListCullRegion(PX_3D_RenderList *list,PX_3D_Camera *camera);
px_void PX_3D_RenderListCalculateNormal(PX_3D_RenderList *list);
px_void PX_3D_RenderListUpdateBackFace(PX_3D_RenderList *list,PX_3D_CULLMODE cullmode);
px_void PX_3D_RenderListTransform_Perspective(PX_3D_RenderList *list,PX_3D_Camera *camera);
px_void PX_3D_RenderListCullOutSide(PX_3D_RenderList *list,PX_3D_Camera *camera);
px_void PX_3D_RenderListTransform_Screen(PX_3D_RenderList *list,PX_3D_Camera *camera);
px_void PX_3D_CameraBuildEulerMatrix(PX_3D_Camera *camera,PX_3D_CAMERAROTMODE rotmode);
px_void PX_3D_CameraBuildUVNMatrix(PX_3D_Camera *camera);


px_void PX_3D_WorldInitialize(PX_3D_World *world,px_float x,px_float y,px_float z,px_float rotX,px_float rotY,px_float rotZ,px_float scale)
{
	px_matrix mrotx,mroty,mrotz,mscale,mtrans;
	PX_MatrixIdentity(&world->mat);
	PX_MatrixRotateX(&mrotx,rotX);
	PX_MatrixRotateY(&mroty,rotY);
	PX_MatrixRotateZ(&mrotz,rotZ);
	PX_MatrixScale(&mscale,scale,scale,scale);
	PX_MatrixTranslation(&mtrans,x,y,z);

	world->mat=PX_MatrixMultiply(world->mat,mscale);
	world->mat=PX_MatrixMultiply(world->mat,mrotx);
	world->mat=PX_MatrixMultiply(world->mat,mroty);
	world->mat=PX_MatrixMultiply(world->mat,mrotz);
	world->mat=PX_MatrixMultiply(world->mat,mtrans);

}

px_bool PX_3D_CameraEulerInitialize(px_memorypool *mp,PX_3D_Camera *camera,px_point4D cameraPosition,px_float rotX,px_float rotY,px_float rotZ,px_float near,px_float far,px_float fov,px_float viewPortWidth,px_float viewPortHeight)
{
	PX_memset(camera,0,sizeof(PX_3D_Camera));
	camera->mp=mp;
	camera->type=PX_3D_CAMERA_TYPE_EULER;
	camera->position=cameraPosition;
	camera->rotDirection.x=rotX;
	camera->rotDirection.y=rotY;
	camera->rotDirection.z=rotZ;

	camera->near_z=near;
	camera->far_z=far;

	camera->viewport_width=viewPortWidth;
	camera->viewport_height=viewPortHeight;

	camera->viewport_center_x=(viewPortWidth-1)/2;
	camera->viewport_center_y=(viewPortHeight-1)/2;
	
	camera->aspect_ratio=viewPortWidth/(px_float)viewPortHeight;

	PX_MatrixIdentity(&camera->mat_cam);
	PX_MatrixIdentity(&camera->mat_per);
	PX_MatrixIdentity(&camera->mat_scr);

	camera->fov=fov;
	camera->viewplane_width=2.0f;
	camera->viewplane_height=2.0f/camera->aspect_ratio;


	if (fov==90)
	{
		camera->d=1;
	}
	else
	{
		camera->d=(0.5f)*(camera->viewplane_width)*PX_tan_angle(fov);
	}
	camera->zbuffer=(px_float *)MP_Malloc(mp,(px_int)(sizeof(px_float)*camera->viewport_height*camera->viewport_width));
	return camera->zbuffer!=PX_NULL;
}

px_bool PX_3D_CameraUVNInitialize(px_memorypool *mp,PX_3D_Camera *camera,px_point4D cameraPosition,px_point4D cameraTarget,px_float near,px_float far,px_float fov,px_float viewPortWidth,px_float viewPortHeight)
{
	PX_memset(camera,0,sizeof(PX_3D_Camera));
	camera->type=PX_3D_CAMERA_TYPE_UVN;
	camera->position=cameraPosition;
	camera->targetDirection=cameraTarget;
	camera->near_z=near;
	camera->far_z=far;
	camera->mp=mp;
	camera->viewport_width=viewPortWidth;
	camera->viewport_height=viewPortHeight;

	camera->viewport_center_x=(viewPortWidth-1)/2;
	camera->viewport_center_y=(viewPortHeight-1)/2;

	camera->aspect_ratio=viewPortWidth/(px_float)viewPortHeight;

	PX_MatrixIdentity(&camera->mat_cam);
	PX_MatrixIdentity(&camera->mat_per);
	PX_MatrixIdentity(&camera->mat_scr);

	camera->fov=fov;
	camera->viewplane_width=2.0f;
	camera->viewplane_height=2.0f/camera->aspect_ratio;

	if (fov==90)
	{
		camera->d=1;
	}
	else
	{
		camera->d=(0.5f)*(camera->viewplane_width)*PX_tan_angle(fov);
	}
	camera->zbuffer=(px_float *)MP_Malloc(mp,(px_int)(sizeof(px_float)*camera->viewport_height*camera->viewport_width));
	return camera->zbuffer!=PX_NULL;
}

px_void PX_3D_CameraSetPosition(PX_3D_Camera *camera,px_point4D cameraPosition,px_point4D cameraTarget)
{
	camera->position=cameraPosition;
	camera->targetDirection=cameraTarget;
}

px_void PX_3D_CameraBuildEulerMatrix(PX_3D_Camera *camera,PX_3D_CAMERAROTMODE rotmode)
{
	px_matrix mt_inv,mx_inv,my_inv,mz_inv,mrot,mtmp;
	px_float theta_x=camera->rotDirection.x;
	px_float theta_y=camera->rotDirection.y;
	px_float theta_z=camera->rotDirection.z;

	PX_MatrixIdentity(&mt_inv);
	mt_inv._41=-camera->position.x;
	mt_inv._42=-camera->position.y;
	mt_inv._43=-camera->position.z;


	PX_MatrixRotateX(&mx_inv,-theta_x);
	PX_MatrixRotateY(&my_inv,-theta_y);
	PX_MatrixRotateZ(&mz_inv,-theta_z);


	switch(rotmode)
	{
	case PX_3D_CAMERAROTMODE_XYZ:
		{
			mtmp=PX_MatrixMultiply(mx_inv,my_inv);
			mrot=PX_MatrixMultiply(mtmp,mz_inv);
		}
		break;
	case PX_3D_CAMERAROTMODE_YXZ:
		{
			mtmp=PX_MatrixMultiply(my_inv,mx_inv);
			mrot=PX_MatrixMultiply(mtmp,mz_inv);
		}
		break;
	case PX_3D_CAMERAROTMODE_XZY:
		{
			mtmp=PX_MatrixMultiply(mx_inv,mz_inv);
			mrot=PX_MatrixMultiply(mtmp,my_inv);
		}
		break;
	case PX_3D_CAMERAROTMODE_YZX:
		{
			mtmp=PX_MatrixMultiply(my_inv,mz_inv);
			mrot=PX_MatrixMultiply(mtmp,mx_inv);
		}
		break;
	case PX_3D_CAMERAROTMODE_ZYX:
		{
			mtmp=PX_MatrixMultiply(mz_inv,my_inv);
			mrot=PX_MatrixMultiply(mtmp,mx_inv);
		}
		break;
	case PX_3D_CAMERAROTMODE_ZXY:
		{
			mtmp=PX_MatrixMultiply(mz_inv,mx_inv);
			mrot=PX_MatrixMultiply(mtmp,my_inv);
		}
		break;
	default:
	{
		PX_MatrixIdentity(&mrot);
	}
	break;
	}
	camera->mat_cam=PX_MatrixMultiply(mt_inv,mrot);
}

px_void PX_3D_CameraBuildUVNMatrix(PX_3D_Camera *camera)
{
	px_matrix mt_inv;
	px_matrix mt_uvn;
	px_point4D u,v,n;

	PX_MatrixIdentity(&mt_inv);
	mt_inv._41=-camera->position.x;
	mt_inv._42=-camera->position.y;
	mt_inv._43=-camera->position.z;

	n.x=camera->targetDirection.x-camera->position.x;
	n.y=camera->targetDirection.y-camera->position.y;
	n.z=camera->targetDirection.z-camera->position.z;
	n.w=1;
	
	n=PX_Point4DUnit(n);
	v=PX_POINT4D(0,1,0);
	u=PX_Point4DCross(v,n);

	PX_MatrixZero(&mt_uvn);
	mt_uvn._11=u.x;
	mt_uvn._12=v.x;
	mt_uvn._13=n.x;

	mt_uvn._21=u.y;
	mt_uvn._22=v.y;
	mt_uvn._23=n.y;

	mt_uvn._31=u.z;
	mt_uvn._32=v.z;
	mt_uvn._33=n.z;

	mt_uvn._44=1;
	camera->mat_cam=PX_MatrixMultiply(mt_inv,mt_uvn);
}

px_void PX_3D_RenderListUpdateBackFace(PX_3D_RenderList *list,PX_3D_CULLMODE cullmode)
{
	px_int i,j;
	PX_3D_Face *pface=PX_NULL;
	px_float dot;
	px_bool bBackFace;
	px_vector4D v1,v2;
	for (i=0;i<list->facestream.size;i++)
	{
		bBackFace=PX_TRUE;
		pface=PX_VECTORAT(PX_3D_Face,&list->facestream,i);
		v1=pface->transform_vertex[0].normal;
		for (j=0;j<3;j++)
		{
			v2=pface->transform_vertex[j].position;
			dot=PX_Point4DDot(v1,v2);
			if (cullmode==PX_3D_CULLMODE_NONE)
			{
				bBackFace=PX_FALSE;
			}
			else
			{
				if (dot>0)
				{
					if (cullmode!=PX_3D_CULLMODE_CCW)
					{
						bBackFace=PX_FALSE;
						break;
					}
				}
				else
				{
					if (cullmode!=PX_3D_CULLMODE_CW)
					{
						bBackFace=PX_FALSE;
						break;
					}
				}
			}
		}
		if (bBackFace)
		{
			pface->state|=PX_3D_FACESTATE_BACKFACE;
		}
		else
		{
			pface->state&=~PX_3D_FACESTATE_BACKFACE;
		}
	}
}

px_void PX_3D_RenderListCalculateNormal(PX_3D_RenderList *list)
{
	px_int i;
	PX_3D_Face *pface=PX_NULL;
	px_vector4D v1,v2;
	for (i=0;i<list->facestream.size;i++)
	{
		pface=PX_VECTORAT(PX_3D_Face,&list->facestream,i);
		v1=PX_Point4DSub(pface->transform_vertex[1].position,pface->transform_vertex[0].position);
		v2=PX_Point4DSub(pface->transform_vertex[2].position,pface->transform_vertex[1].position);
		pface->transform_vertex[0].normal=PX_Point4DUnit(PX_Point4DCross(v1,v2));
		pface->transform_vertex[1].normal=pface->transform_vertex[0].normal;
		pface->transform_vertex[2].normal=pface->transform_vertex[0].normal;
	}
}

px_void PX_3D_RenderListTransform_Screen(PX_3D_RenderList *list,PX_3D_Camera *camera)
{
	px_int i;
	PX_3D_Face *pface=PX_NULL;
	px_float alpha=(0.5f*camera->viewport_width-0.5f);
	px_float beta=(0.5f*camera->viewport_height-0.5f);

	for (i=0;i<list->facestream.size;i++)
	{
		pface=PX_VECTORAT(PX_3D_Face,&list->facestream,i);

		pface->transform_vertex[0].position.x=alpha+alpha*pface->transform_vertex[0].position.x;
		pface->transform_vertex[0].position.y=beta+beta*pface->transform_vertex[0].position.y;

		pface->transform_vertex[1].position.x=alpha+alpha*pface->transform_vertex[1].position.x;
		pface->transform_vertex[1].position.y=beta+beta*pface->transform_vertex[1].position.y;

		pface->transform_vertex[2].position.x=alpha+alpha*pface->transform_vertex[2].position.x;
		pface->transform_vertex[2].position.y=beta+beta*pface->transform_vertex[2].position.y;

		//inv y
		pface->transform_vertex[0].position.y=-pface->transform_vertex[0].position.y+camera->viewport_height;
		pface->transform_vertex[1].position.y=-pface->transform_vertex[1].position.y+camera->viewport_height;
		pface->transform_vertex[2].position.y=-pface->transform_vertex[2].position.y+camera->viewport_height;
	}
}



px_void PX_3D_RenderListPixelShader(px_surface *psurface,px_int x,px_int y,px_float z,px_float u,px_float v,px_point4D normal,px_texture *pTexture,px_color color/*px_float *zbuffer,px_int zw*/)
{
	//texture mapping
	px_double SampleX,SampleY,mapX,mapY;
	px_double mixa,mixr,mixg,mixb,Weight;
	px_color sampleColor;
	px_int resWidth;
	px_int resHeight;

	

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

		mixa>255?mixa=255:0;
		mixr>255?mixr=255:0;
		mixg>255?mixg=255:0;
		mixb>255?mixb=255:0;

		PX_SurfaceDrawPixel(psurface,x,y,PX_COLOR((px_uchar)mixa,(px_uchar)mixr,(px_uchar)mixg,(px_uchar)mixb));
		
	}

	if (color._argb.ucolor!=0)
	{
		PX_SurfaceDrawPixel(psurface,x,y,color);

	}
}


static px_void PX_3D_RenderListRasterization(px_surface *psurface,PX_3D_RenderList *pList,PX_3D_Vertex p0,PX_3D_Vertex p1,PX_3D_Vertex p2,px_texture *ptexture,px_color color,px_int view_width,px_int view_height,px_float zbuffer[],px_int zw)
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


	px_float y, xleft, xright; 
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
		PX_3D_Vertex vt;
		vt =p1;
		p1=p0;
		p0= vt;
	}

	if (p2.position.y<p0.position.y)
	{
		PX_3D_Vertex vt;
		vt =p2;
		p2=p0;
		p0= vt;
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
			PX_3D_Vertex t;
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
		if (k01infinite==PX_TRUE)
		{
			xleft=b01;
		}
		else
		{
			xleft = (y-b01)/k01;
		}

		if (k02infinite==PX_TRUE)
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

			if (ix>0&&ix<view_width&&iy>=0&&iy<view_height)
			{
				if (zbuffer[ix+iy*zw]!=0&&zbuffer[ix+iy*zw]<originalZ)
				{
					continue;;
				}

				zbuffer[ix+iy*zw]=originalZ;
				if (pList->pixelShader)
				{
					position.z=originalZ;
					pList->pixelShader(psurface,ix,iy,position,s,t,p0.normal,ptexture,color/*,zbuffer,zw*/);
				}
				else
				{
					PX_3D_RenderListPixelShader(psurface,ix,iy,originalZ,s,t,p0.normal,ptexture,color/*,zbuffer,zw*/);
				}
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
		PX_3D_Vertex vt;
		vt=p1;
		p1=p0;
		p0=vt;
	}

	if (p2.position.y>p0.position.y)
	{
		PX_3D_Vertex vt;
		vt=p2;
		p2=p0;
		p0=vt;
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
			PX_3D_Vertex t;
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

			if (ix>0&&ix<view_width&&iy>=0&&iy<view_height)
			{
				if (zbuffer[ix+iy*zw]!=0&&zbuffer[ix+iy*zw]<originalZ)
				{
					continue;;
				}

				zbuffer[ix+iy*zw]=originalZ;
				if (pList->pixelShader)
				{
					position.z=originalZ;
					pList->pixelShader(psurface,ix,iy,position,s,t,p0.normal,ptexture,color/*,zbuffer,zw*/);
				}
				else
				{
					PX_3D_RenderListPixelShader(psurface,ix,iy,originalZ,s,t,p0.normal,ptexture,color/*,zbuffer,zw*/);
				}
			}
			oneoverz += oneoverz_step;
			soverz += soverz_step;
			toverz += toverz_step;
		}
	}

}


px_void PX_3D_Scene(PX_3D_RenderList *list,PX_3D_World *world,PX_3D_Camera *camera)
{
	PX_3D_CULLMODE cullmode;
	PX_memset(camera->zbuffer,0,(px_int)(sizeof(px_float)*camera->viewport_height*camera->viewport_width));
	cullmode=list->cullmode;
	PX_3D_RenderListTransform_LocalToGlobal(list,world);
	switch (camera->type)
	{
	case PX_3D_CAMERA_TYPE_EULER:
		PX_3D_CameraBuildEulerMatrix(camera,PX_3D_CAMERAROTMODE_XYZ);
		break;
	case PX_3D_CAMERA_TYPE_UVN:
		PX_3D_CameraBuildUVNMatrix(camera);
		break;
	}
	PX_3D_RenderListTransform_Camera(list,camera);
	PX_3D_RenderListCullRegion(list,camera);
	PX_3D_RenderListCalculateNormal(list);
	PX_3D_RenderListUpdateBackFace(list,cullmode);
	PX_3D_RenderListTransform_Perspective(list,camera);
	PX_3D_RenderListCullOutSide(list,camera);
	PX_3D_RenderListTransform_Screen(list,camera);
}

px_void PX_3D_Present(px_surface *psurface, PX_3D_RenderList *list,PX_3D_Camera *camera)
{
	px_int i;
	PX_3D_Face *pface=PX_NULL;
	px_texture *ptex;
	px_color clr;
	

	if (list->PX_3D_PRESENTMODE&PX_3D_PRESENTMODE_PURE)
	{
		if (list->facestream.size)
		{
			pface=PX_VECTORAT(PX_3D_Face,&list->facestream,0);
			clr=pface->transform_vertex[0].clr;
		}
		else
		{
			clr._argb.ucolor=0;
		}
	}
	else
	{
		clr._argb.ucolor=0;
	}

	if (list->PX_3D_PRESENTMODE&PX_3D_PRESENTMODE_TEXTURE)
	{
		ptex=list->ptexture;
	}
	else
	{
		ptex=PX_NULL;
	}

	if (ptex||clr._argb.ucolor)
	{
		for (i=0;i<list->facestream.size;i++)
		{
			pface=PX_VECTORAT(PX_3D_Face,&list->facestream,i);
			if (!(pface->state&PX_3D_FACESTATE_BACKFACE||pface->state&PX_3D_FACESTATE_CLIPPED))
			{
				if (list->PX_3D_PRESENTMODE&PX_3D_PRESENTMODE_PURE)
				{
					PX_3D_RenderListRasterization(psurface,list,pface->transform_vertex[0],pface->transform_vertex[1],pface->transform_vertex[2],list->ptexture,clr,(px_int)camera->viewport_width,(px_int)camera->viewport_height,camera->zbuffer,(px_int)camera->viewport_width);
				}
				else
				{
					PX_3D_RenderListRasterization(psurface,list,pface->transform_vertex[0],pface->transform_vertex[1],pface->transform_vertex[2],ptex,PX_COLOR(0,0,0,0),(px_int)camera->viewport_width,(px_int)camera->viewport_height,camera->zbuffer,(px_int)camera->viewport_width);
				}

			}

		}
	}

	if (list->PX_3D_PRESENTMODE&PX_3D_PRESENTMODE_POINT)
	{
		for (i=0;i<list->facestream.size;i++)
		{
			pface=PX_VECTORAT(PX_3D_Face,&list->facestream,i);
			PX_GeoDrawSolidCircle(psurface,(px_int)pface->transform_vertex[0].position.x,(px_int)pface->transform_vertex[0].position.y,2,pface->transform_vertex[0].clr);
			PX_GeoDrawSolidCircle(psurface,(px_int)pface->transform_vertex[1].position.x,(px_int)pface->transform_vertex[1].position.y,2,pface->transform_vertex[1].clr);
			PX_GeoDrawSolidCircle(psurface,(px_int)pface->transform_vertex[2].position.x,(px_int)pface->transform_vertex[2].position.y,2,pface->transform_vertex[2].clr);

		}
	}

	if (list->PX_3D_PRESENTMODE&PX_3D_PRESENTMODE_LINE)
	{
		for (i=0;i<list->facestream.size;i++)
		{
			pface=PX_VECTORAT(PX_3D_Face,&list->facestream,i);
			if (!(pface->state&PX_3D_FACESTATE_BACKFACE||pface->state&PX_3D_FACESTATE_CLIPPED))
			{
				PX_GeoDrawLine(psurface,(px_int)pface->transform_vertex[0].position.x,(px_int)pface->transform_vertex[0].position.y,(px_int)pface->transform_vertex[1].position.x,(px_int)pface->transform_vertex[1].position.y,1,pface->transform_vertex[0].clr);
				PX_GeoDrawLine(psurface,(px_int)pface->transform_vertex[1].position.x,(px_int)pface->transform_vertex[1].position.y,(px_int)pface->transform_vertex[2].position.x,(px_int)pface->transform_vertex[2].position.y,1,pface->transform_vertex[1].clr);
				PX_GeoDrawLine(psurface,(px_int)pface->transform_vertex[2].position.x,(px_int)pface->transform_vertex[2].position.y,(px_int)pface->transform_vertex[0].position.x,(px_int)pface->transform_vertex[0].position.y,1,pface->transform_vertex[2].clr);
			}

		}
	}
	

}

px_void PX_3D_RenderListSetPixelShader(PX_3D_RenderList *list,PX_3D_PixelShader func)
{
	list->pixelShader=func;
}

px_void PX_3D_RenderListTransform_Perspective(PX_3D_RenderList *list,PX_3D_Camera *camera)
{
	px_int i;
	PX_3D_Face *pface=PX_NULL;
	for (i=0;i<list->facestream.size;i++)
	{
		px_float z;
		pface=PX_VECTORAT(PX_3D_Face,&list->facestream,i);
		z=pface->transform_vertex[0].position.z;
		pface->transform_vertex[0].position.x=camera->d*pface->transform_vertex[0].position.x/z;
		pface->transform_vertex[0].position.y=camera->d*pface->transform_vertex[0].position.y/z;

		z=pface->transform_vertex[1].position.z;
		pface->transform_vertex[1].position.x=camera->d*pface->transform_vertex[1].position.x/z;
		pface->transform_vertex[1].position.y=camera->d*pface->transform_vertex[1].position.y/z;

		z=pface->transform_vertex[2].position.z;
		pface->transform_vertex[2].position.x=camera->d*pface->transform_vertex[2].position.x/z;
		pface->transform_vertex[2].position.y=camera->d*pface->transform_vertex[2].position.y/z;

	}
}

px_void PX_3D_RenderListCullOutSide(PX_3D_RenderList *list,PX_3D_Camera *camera)
{
	px_int i;
	PX_3D_Face *pface=PX_NULL;
	px_bool outside;
	for (i=0;i<list->facestream.size;i++)
	{
		pface=PX_VECTORAT(PX_3D_Face,&list->facestream,i);
		outside=PX_FALSE;

		if (pface->transform_vertex[0].position.y>camera->viewplane_height/2&&\
			pface->transform_vertex[1].position.y>camera->viewplane_height/2&&\
			pface->transform_vertex[2].position.y>camera->viewplane_height/2)
		{
					//up
			outside=PX_TRUE;
		}
		else\
		if (pface->transform_vertex[0].position.y<-camera->viewplane_height/2&&\
			pface->transform_vertex[1].position.y<-camera->viewplane_height/2&&\
			pface->transform_vertex[2].position.y<-camera->viewplane_height/2)
		{
					//down
			outside=PX_TRUE;
		}
		else \
		if (pface->transform_vertex[0].position.x<-camera->viewplane_width/2&&\
			pface->transform_vertex[1].position.x<-camera->viewplane_width/2&&\
			pface->transform_vertex[2].position.x<-camera->viewplane_width/2)
		{
					//left
			outside=PX_TRUE;
		}
		else \
		if (pface->transform_vertex[0].position.x>camera->viewplane_width/2&&\
			pface->transform_vertex[1].position.x>camera->viewplane_width/2&&\
			pface->transform_vertex[2].position.x>camera->viewplane_width/2)
		{
					//right
			outside=PX_TRUE;
		}
		

		if (outside)
		{
			pface->state|=PX_3D_FACESTATE_CLIPPED;
		}
		else
		{
			pface->state&=~PX_3D_FACESTATE_CLIPPED;
		}
	}
}

px_void PX_3D_RenderListTransform_Camera(PX_3D_RenderList *list,PX_3D_Camera *camera)
{
	PX_3D_RenderListTransform(list,camera->mat_cam,PX_3D_RENDERLIST_TRANSFORM_GLOBAL_TO_GLOBAL);
}

px_void PX_3D_RenderListCullRegion(PX_3D_RenderList *list,PX_3D_Camera *camera)
{
	px_int i,j;
	PX_3D_Face *pface=PX_NULL;
	px_bool outside;
	for (i=0;i<list->facestream.size;i++)
	{
		pface=PX_VECTORAT(PX_3D_Face,&list->facestream,i);
		outside=PX_FALSE;
		for (j=0;j<3;j++)
		{
			//behind
			if (pface->transform_vertex[j].position.z<=camera->near_z)
			{
				outside=PX_TRUE;
				break;
			}
		}
		if (pface->transform_vertex[0].position.z>camera->far_z&&\
			pface->transform_vertex[1].position.z>camera->far_z&&\
			pface->transform_vertex[2].position.z>camera->far_z)
		{
			outside=PX_TRUE;
		}
		if (outside)
		{
			pface->state|=PX_3D_FACESTATE_CLIPPED;
		}
		else
		{
			pface->state&=~PX_3D_FACESTATE_CLIPPED;
		}
	}
}

px_void PX_3D_RenderListTransform(PX_3D_RenderList *list,px_matrix mat,PX_3D_RENDERLIST_TRANSFORM t)
{
	switch (t)
	{
	case PX_3D_RENDERLIST_TRANSFORM_LOCAL_TO_LOCAL:
		{
			px_int i;
			PX_3D_Face *pface=PX_NULL;
			for (i=0;i<list->facestream.size;i++)
			{
				pface=PX_VECTORAT(PX_3D_Face,&list->facestream,i);
				pface->vertex[0].position=PX_Point4DMulMatrix(pface->vertex[0].position,mat);
				pface->vertex[1].position=PX_Point4DMulMatrix(pface->vertex[1].position,mat);
				pface->vertex[2].position=PX_Point4DMulMatrix(pface->vertex[2].position,mat);
			}
		}
		break;
	case PX_3D_RENDERLIST_TRANSFORM_LOACL_TO_GLOBAL:
		{
			px_int i;
			PX_3D_Face *pface=PX_NULL;
			
			for (i=0;i<list->facestream.size;i++)
			{
				pface=PX_VECTORAT(PX_3D_Face,&list->facestream,i);
				pface->transform_vertex[0]=pface->vertex[0];
				pface->transform_vertex[1]=pface->vertex[1];
				pface->transform_vertex[2]=pface->vertex[2];
				pface->transform_vertex[0].position=PX_Point4DMulMatrix(pface->vertex[0].position,mat);
				pface->transform_vertex[1].position=PX_Point4DMulMatrix(pface->vertex[1].position,mat);
				pface->transform_vertex[2].position=PX_Point4DMulMatrix(pface->vertex[2].position,mat);
			}
		}
		break;
	case PX_3D_RENDERLIST_TRANSFORM_GLOBAL_TO_GLOBAL:
		{
			px_int i;
			PX_3D_Face *pface=PX_NULL;

			for (i=0;i<list->facestream.size;i++)
			{
				pface=PX_VECTORAT(PX_3D_Face,&list->facestream,i);
				pface->transform_vertex[0].position=PX_Point4DMulMatrix(pface->transform_vertex[0].position,mat);
				pface->transform_vertex[1].position=PX_Point4DMulMatrix(pface->transform_vertex[1].position,mat);
				pface->transform_vertex[2].position=PX_Point4DMulMatrix(pface->transform_vertex[2].position,mat);
			}
		}
		break;
	}
}



px_void PX_3D_RenderListReset(PX_3D_RenderList *list)
{
	PX_VectorClear(&list->facestream);
}

px_void PX_3D_RenderListFree(PX_3D_RenderList *list)
{
	PX_VectorFree(&list->facestream);
}

px_bool PX_3D_RenderListInitialize(px_memorypool *mp,PX_3D_RenderList *list,px_dword PX_3D_PRESENTMODE,PX_3D_CULLMODE cullmode,px_texture *ptexture)
{
	list->mp=mp;
	list->ptexture=ptexture;
	list->PX_3D_PRESENTMODE=PX_3D_PRESENTMODE;
	list->cullmode=cullmode;
	list->pixelShader=PX_NULL;
	if(!PX_VectorInitialize(mp,&list->facestream,sizeof(PX_3D_Face),64))
	{
		return PX_FALSE;
	}
	return PX_TRUE;
	
}

px_void PX_3D_CameraFree(PX_3D_Camera *camera)
{
	MP_Free(camera->mp,camera->zbuffer);
}

px_bool PX_3D_RenderListPush(PX_3D_RenderList *list,PX_3D_Face face)
{
	if(!PX_VectorPushback(&list->facestream,&face))return PX_FALSE;
	return PX_TRUE;
}

px_void PX_3D_RenderListTransform_LocalToGlobal(PX_3D_RenderList *list,PX_3D_World *world)
{
	PX_3D_RenderListTransform(list,world->mat,PX_3D_RENDERLIST_TRANSFORM_LOACL_TO_GLOBAL);
}

px_point *PX_3D_CreateTextureNormal(px_memorypool *mp,px_texture *pTexture)
{
	px_int i,j;
	px_point *pmatrix=(px_point *)MP_Malloc(mp,sizeof(px_point)*pTexture->width*pTexture->height);
	px_float *gradient_x=(px_float *)MP_Malloc(mp,sizeof(px_float)*pTexture->width*pTexture->height);
	px_float *gradient_y=(px_float *)MP_Malloc(mp,sizeof(px_float)*pTexture->width*pTexture->height);

	PX_ImageFilter_SobelX(pTexture,gradient_x);
	PX_ImageFilter_SobelY(pTexture,gradient_y);

	for(j=0;j<pTexture->height;j++)
	{
		for (i=0;i<pTexture->width;i++)
		{
 			pmatrix[j*pTexture->width+i].x=(px_float)(-gradient_x[j*pTexture->width+i]/PX_sqrtd(gradient_x[j*pTexture->width+i]*gradient_x[j*pTexture->width+i]+gradient_y[j*pTexture->width+i]*gradient_y[j*pTexture->width+i]+1));
 			pmatrix[j*pTexture->width+i].y=(px_float)(-gradient_y[j*pTexture->width+i]/PX_sqrtd(gradient_x[j*pTexture->width+i]*gradient_x[j*pTexture->width+i]+gradient_y[j*pTexture->width+i]*gradient_y[j*pTexture->width+i]+1));
 			pmatrix[j*pTexture->width+i].z=(px_float)(1/PX_sqrtd(gradient_x[j*pTexture->width+i]*gradient_x[j*pTexture->width+i]+gradient_y[j*pTexture->width+i]*gradient_y[j*pTexture->width+i]+1));
		}
	}
	MP_Free(mp,gradient_x);
	MP_Free(mp,gradient_y);
	return pmatrix;
}


px_point *PX_3D_CreateBumpTextureNormal(px_memorypool *mp,px_texture *pTexture)
{
	px_int i,j;
	px_point *pmatrix=(px_point *)MP_Malloc(mp,sizeof(px_point)*pTexture->width*pTexture->height);
	px_float *gradient_x=(px_float *)MP_Malloc(mp,sizeof(px_float)*pTexture->width*pTexture->height);
	px_float *gradient_y=(px_float *)MP_Malloc(mp,sizeof(px_float)*pTexture->width*pTexture->height);

	PX_ImageFilter_SobelX(pTexture,gradient_x);
	PX_ImageFilter_SobelY(pTexture,gradient_y);

	for(j=0;j<pTexture->height;j++)
	{
		for (i=0;i<pTexture->width;i++)
		{
			px_point v;
			if (gradient_x[j*pTexture->width+i]==0&&gradient_y[j*pTexture->width+i]==0)
			{
				
				v=PX_POINT(0,0,-1);

			}
			else
			{
				px_float rad,scale;
				px_color clr;

				clr=pTexture->surfaceBuffer[j*pTexture->width+i];
				v.x=-gradient_x[j*pTexture->width+i];
				v.y=-gradient_y[j*pTexture->width+i];
				v=PX_PointNormalization(v);

				rad=(clr._argb.r+clr._argb.g+clr._argb.b)/3/255.0f;
				v.z=-(px_float)PX_sind(rad*PX_PI/2);
				scale=(px_float)PX_cosd(rad*PX_PI/2);
				v.x*=scale;
				v.y*=scale;
			}
			
			
			pmatrix[j*pTexture->width+i]=v;
// 			px_point v1,v2;
// 			v1.z=0;
// 			v1.x=(px_float)PX_cosd(gradient_x[j*pTexture->width+i]*PX_PI/2);
// 			v1.y=(px_float)PX_sind(gradient_x[j*pTexture->width+i]*PX_PI/2);
// 
// 			v2.z=-(px_float)PX_sind(gradient_y[j*pTexture->width+i]*PX_PI/2);
// 			v2.x=0;
// 			v2.y=-(px_float)PX_cosd(gradient_y[j*pTexture->width+i]*PX_PI/2);
// 
// 			pmatrix[j*pTexture->width+i]=PX_PointUnit(PX_PointCross(v1,v2));
		}
	}
	MP_Free(mp,gradient_x);
	MP_Free(mp,gradient_y);
	return pmatrix;
}
