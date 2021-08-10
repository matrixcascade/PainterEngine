#include "PX_Object_Image.h"

px_void PX_Object_ImageRender(px_surface *psurface, PX_Object *pObject,px_uint elapsed)
{
	PX_Object_Image *pImage=PX_Object_GetImage(pObject);
	px_int x,y,w,h;
	PX_SurfaceLimitInfo limitInfo;
	px_float inheritX,inheritY;
	if (!pImage->pTexture)
	{
		return;
	}

	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	x=(px_int)(pObject->x+inheritX);
	y=(px_int)(pObject->y+inheritY);
	w=(px_int)pObject->Width;
	h=(px_int)pObject->Height;

	limitInfo=PX_SurfaceGetLimit(psurface);
	PX_SurfaceSetLimit(psurface,x,y,x+w-1,y+h-1);

	switch(pImage->Align)
	{
	case PX_ALIGN_LEFTTOP:
		{
			if (pImage!=PX_NULL)
			{
				if(pImage->pmask)
					PX_TextureRenderMask(psurface,pImage->pmask,pImage->pTexture,x,y,PX_ALIGN_LEFTTOP,PX_NULL);
				else
					PX_TextureRender(psurface,pImage->pTexture,x,y,PX_ALIGN_LEFTTOP,PX_NULL);
			}
		}
		break;
	case PX_ALIGN_LEFTMID:
		{
			if (pImage!=PX_NULL)
			{
				if(pImage->pmask)
					PX_TextureRenderMask(psurface,pImage->pmask,pImage->pTexture,x,y+h/2,PX_ALIGN_LEFTMID,PX_NULL);
				else
					PX_TextureRender(psurface,pImage->pTexture,x,y+h/2,PX_ALIGN_LEFTMID,PX_NULL);
			}
		}
		break;
	case PX_ALIGN_LEFTBOTTOM:
		{
			if (pImage!=PX_NULL)
			{
				if(pImage->pmask)
					PX_TextureRenderMask(psurface,pImage->pmask,pImage->pTexture,x,y+h,PX_ALIGN_LEFTBOTTOM,PX_NULL);
				else
					PX_TextureRender(psurface,pImage->pTexture,x,y+h,PX_ALIGN_LEFTBOTTOM,PX_NULL);
			}
		}
		break;
	case PX_ALIGN_MIDTOP:
		{
			if (pImage!=PX_NULL)
			{
				if(pImage->pmask)
					PX_TextureRenderMask(psurface,pImage->pmask,pImage->pTexture,x+w/2,y,PX_ALIGN_MIDTOP,PX_NULL);
				else
					PX_TextureRender(psurface,pImage->pTexture,x+w/2,y,PX_ALIGN_MIDTOP,PX_NULL);
			}
		}
		break;
	case PX_ALIGN_CENTER:
		{
			if (pImage!=PX_NULL)
			{
				if(pImage->pmask)
					PX_TextureRenderMask(psurface,pImage->pmask,pImage->pTexture,x+w/2,y+h/2,PX_ALIGN_CENTER,PX_NULL);
				else
					PX_TextureRender(psurface,pImage->pTexture,x+w/2,y+h/2,PX_ALIGN_CENTER,PX_NULL);
			}
		}
		break;
	case PX_ALIGN_MIDBOTTOM:
		{
			if (pImage!=PX_NULL)
			{
				if(pImage->pmask)
					PX_TextureRenderMask(psurface,pImage->pmask,pImage->pTexture,x+w/2,y+h,PX_ALIGN_MIDBOTTOM,PX_NULL);
				else
					PX_TextureRender(psurface,pImage->pTexture,x+w/2,y+h,PX_ALIGN_MIDBOTTOM,PX_NULL);
			}
		}
		break;
	case PX_ALIGN_RIGHTTOP:
		{
			if (pImage!=PX_NULL)
			{
				if(pImage->pmask)
					PX_TextureRenderMask(psurface,pImage->pmask,pImage->pTexture,x+w,y,PX_ALIGN_RIGHTTOP,PX_NULL);
				else
					PX_TextureRender(psurface,pImage->pTexture,x+w,y,PX_ALIGN_RIGHTTOP,PX_NULL);
			}
		}
		break;
	case PX_ALIGN_RIGHTMID:
		{
			if (pImage!=PX_NULL)
			{
				if(pImage->pmask)
					PX_TextureRenderMask(psurface,pImage->pmask,pImage->pTexture,x+w,y+h/2,PX_ALIGN_RIGHTMID,PX_NULL);
				else
					PX_TextureRender(psurface,pImage->pTexture,x+w,y+h/2,PX_ALIGN_RIGHTMID,PX_NULL);
			}
		}
		break;
	case PX_ALIGN_RIGHTBOTTOM:
		{
			if (pImage!=PX_NULL)
			{
				if(pImage->pmask)
					PX_TextureRenderMask(psurface,pImage->pmask,pImage->pTexture,x+w,y+h,PX_ALIGN_RIGHTBOTTOM,PX_NULL);
				else
					PX_TextureRender(psurface,pImage->pTexture,x+w,y+h,PX_ALIGN_RIGHTBOTTOM,PX_NULL);
			}
		}
		break;
	}

	PX_SurfaceSetLimitInfo(psurface,limitInfo);

}

px_void PX_Object_ImageFree( PX_Object *pBitmap )
{

}

PX_Object * PX_Object_ImageCreate(px_memorypool *mp,PX_Object *Parent,px_int x,px_int y,px_int width,px_int height,px_texture *ptex )
{
	PX_Object *pObject;
	PX_Object_Image *pImage=(PX_Object_Image *)MP_Malloc(mp,sizeof(PX_Object_Image));
	if (pImage==PX_NULL)
	{
		return PX_NULL;
	}
	pObject=PX_ObjectCreate(mp,Parent,(px_float)x,(px_float)y,0,(px_float)width,(px_float)height,0);
	if (pObject==PX_NULL)
	{
		MP_Free(pObject->mp,pImage);
		return PX_NULL;
	}

	pObject->pObject=pImage;
	pObject->Enabled=PX_TRUE;
	pObject->Visible=PX_TRUE;
	pObject->Type=PX_OBJECT_TYPE_IMAGE;
	pObject->ReceiveEvents=PX_FALSE;
	pObject->Func_ObjectFree=PX_Object_ImageFree;
	pObject->Func_ObjectRender=PX_Object_ImageRender;
	pImage->pTexture=ptex;
	pImage->pmask=PX_NULL;
	pImage->Align=PX_ALIGN_CENTER;
	return pObject;
}


PX_Object_Image * PX_Object_GetImage( PX_Object *Object )
{
	if(Object->Type==PX_OBJECT_TYPE_IMAGE)
		return (PX_Object_Image *)Object->pObject;
	else
		return PX_NULL;
}

px_void PX_Object_ImageSetAlign( PX_Object *pImage,PX_ALIGN Align)
{
	PX_Object_Image *Bitmap=PX_Object_GetImage(pImage);
	if (Bitmap)
	{
		Bitmap->Align=Align;
	}
}


px_void PX_Object_ImageSetMask(PX_Object *pObject,px_texture *pmask)
{
	PX_Object_Image *pImg=PX_Object_GetImage(pObject);
	if (pImg)
	{
		pImg->pmask=pmask;
	}
}


px_void PX_Object_ImageSetTexture(PX_Object *pObject,px_texture *pTex)
{
	PX_Object_Image *pImg=PX_Object_GetImage(pObject);
	if (pImg)
	{
		pImg->pTexture=pTex;
	}
}


px_void PX_Object_ImageFreeWithTexture(PX_Object *pBitmap)
{
	PX_Object_Image *pImage;
	pImage=PX_Object_GetImage(pBitmap);
	if (pImage)
	{
		PX_TextureFree(pImage->pTexture);
	}
}

