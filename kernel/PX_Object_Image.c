#include "PX_Object_Image.h"

PX_OBJECT_RENDER_FUNCTION(PX_Object_ImageRender)
{
	PX_Object_Image *pImage=PX_Object_GetImage(pObject);
	px_int x,y,w,h;
	PX_SurfaceLimitInfo limitInfo;
	px_float inheritX,inheritY;
	px_texture *prenderTexture=PX_NULL;
	PX_TEXTURERENDER_BLEND blend = {0}, * pblend;
	if (!pImage->pTexture&&!pImage->pgif)
	{
		return;
	}

	if (pImage->pgif)
	{
		PX_GifUpdate(pImage->pgif, elapsed);
		prenderTexture=PX_GifGetTexture(pImage->pgif);
	}

	if (pImage->pTexture)
	{
		prenderTexture = pImage->pTexture;
	}

	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	x=(px_int)(pObject->x+inheritX);
	y=(px_int)(pObject->y+inheritY);
	w=(px_int)pObject->Width;
	h=(px_int)pObject->Height;

	limitInfo=PX_SurfaceGetLimit(psurface);
	PX_SurfaceSetLimit(psurface,x,y,x+w-1,y+h-1);

	if (pImage->alpha!=1.0f)
	{
		blend.alpha=pImage->alpha;
		blend.hdr_B = 1;
		blend.hdr_G = 1;
		blend.hdr_R = 1;
		pblend=&blend;
	}
	else
	{
		pblend = PX_NULL;

	}

	switch(pImage->Align)
	{
	case PX_ALIGN_LEFTTOP:
		{
			if (pImage!=PX_NULL)
			{
				

				if(pImage->pmask)
					PX_TextureRenderMask(psurface,pImage->pmask,prenderTexture,x,y,PX_ALIGN_LEFTTOP, pblend);
				else
					PX_TextureRender(psurface,prenderTexture,x,y,PX_ALIGN_LEFTTOP, pblend);
			}
		}
		break;
	case PX_ALIGN_LEFTMID:
		{
			if (pImage!=PX_NULL)
			{
				if(pImage->pmask)
					PX_TextureRenderMask(psurface,pImage->pmask,prenderTexture,x,y+h/2,PX_ALIGN_LEFTMID, pblend);
				else
					PX_TextureRender(psurface,prenderTexture,x,y+h/2,PX_ALIGN_LEFTMID, pblend);
			}
		}
		break;
	case PX_ALIGN_LEFTBOTTOM:
		{
			if (pImage!=PX_NULL)
			{
				if(pImage->pmask)
					PX_TextureRenderMask(psurface,pImage->pmask,prenderTexture,x,y+h,PX_ALIGN_LEFTBOTTOM, pblend);
				else
					PX_TextureRender(psurface,prenderTexture,x,y+h,PX_ALIGN_LEFTBOTTOM, pblend);
			}
		}
		break;
	case PX_ALIGN_MIDTOP:
		{
			if (pImage!=PX_NULL)
			{
				if(pImage->pmask)
					PX_TextureRenderMask(psurface,pImage->pmask,prenderTexture,x+w/2,y,PX_ALIGN_MIDTOP, pblend);
				else
					PX_TextureRender(psurface,prenderTexture,x+w/2,y,PX_ALIGN_MIDTOP, pblend);
			}
		}
		break;
	case PX_ALIGN_CENTER:
		{
			if (pImage!=PX_NULL)
			{
				if(pImage->pmask)
					PX_TextureRenderMask(psurface,pImage->pmask,prenderTexture,x+w/2,y+h/2,PX_ALIGN_CENTER, pblend);
				else
					PX_TextureRender(psurface,prenderTexture,x+w/2,y+h/2,PX_ALIGN_CENTER, pblend);
			}
		}
		break;
	case PX_ALIGN_MIDBOTTOM:
		{
			if (pImage!=PX_NULL)
			{
				if(pImage->pmask)
					PX_TextureRenderMask(psurface,pImage->pmask,prenderTexture,x+w/2,y+h,PX_ALIGN_MIDBOTTOM, pblend);
				else
					PX_TextureRender(psurface,prenderTexture,x+w/2,y+h,PX_ALIGN_MIDBOTTOM, pblend);
			}
		}
		break;
	case PX_ALIGN_RIGHTTOP:
		{
			if (pImage!=PX_NULL)
			{
				if(pImage->pmask)
					PX_TextureRenderMask(psurface,pImage->pmask,prenderTexture,x+w,y,PX_ALIGN_RIGHTTOP, pblend);
				else
					PX_TextureRender(psurface,prenderTexture,x+w,y,PX_ALIGN_RIGHTTOP, pblend);
			}
		}
		break;
	case PX_ALIGN_RIGHTMID:
		{
			if (pImage!=PX_NULL)
			{
				if(pImage->pmask)
					PX_TextureRenderMask(psurface,pImage->pmask,prenderTexture,x+w,y+h/2,PX_ALIGN_RIGHTMID, pblend);
				else
					PX_TextureRender(psurface,prenderTexture,x+w,y+h/2,PX_ALIGN_RIGHTMID, pblend);
			}
		}
		break;
	case PX_ALIGN_RIGHTBOTTOM:
		{
			if (pImage!=PX_NULL)
			{
				if(pImage->pmask)
					PX_TextureRenderMask(psurface,pImage->pmask,prenderTexture,x+w,y+h,PX_ALIGN_RIGHTBOTTOM, pblend);
				else
					PX_TextureRender(psurface,prenderTexture,x+w,y+h,PX_ALIGN_RIGHTBOTTOM, pblend);
			}
		}
		break;
	}

	PX_SurfaceSetLimitInfo(psurface,limitInfo);

}

PX_OBJECT_FREE_FUNCTION(PX_Object_ImageFree)
{
	PX_Object_Image *pImage=PX_Object_GetImage(pObject);
	if (pImage->texture.MP)
	{
		PX_TextureFree(&pImage->texture);
	}
	if (pImage->gif.mp)
	{
		PX_GifFree(&pImage->gif);
	}
}
PX_Object* PX_Object_ImageAttachObject( PX_Object* pObject,px_int attachIndex, px_texture* ptex)
{
	px_memorypool* mp=pObject->mp;
	PX_Object_Image* pImage;
	PX_ASSERTIF(pObject == PX_NULL);
	PX_ASSERTIF(attachIndex < 0 || attachIndex >= PX_COUNTOF(pObject->pObjectDesc));
	PX_ASSERTIF(pObject->pObjectDesc[attachIndex] != PX_NULL);
	pImage = (PX_Object_Image*)PX_ObjectCreateDesc(pObject, attachIndex, PX_OBJECT_TYPE_IMAGE, 0, PX_Object_ImageRender, PX_Object_ImageFree, 0, sizeof(PX_Object_Image));
	PX_ASSERTIF(pImage == PX_NULL);

	pImage->pTexture = ptex;
	pImage->pmask = PX_NULL;
	pImage->Align = PX_ALIGN_CENTER;
	pImage->alpha = 1.0f;
	return pObject;
}

PX_Object * PX_Object_ImageCreate(px_memorypool *mp,PX_Object *Parent,px_int x,px_int y,px_int width,px_int height,px_texture *ptex )
{
	PX_Object *pObject;
	
	pObject=PX_ObjectCreate(mp,Parent,(px_float)x,(px_float)y,0,(px_float)width,(px_float)height,0);
	if (pObject ==PX_NULL)
	{
		return PX_NULL;
	}
	if (!PX_Object_ImageAttachObject(pObject,0,ptex))
	{
		PX_ObjectDelete(pObject);
		return PX_NULL;
	}

	
	return pObject;
}


PX_Object_Image * PX_Object_GetImage( PX_Object *pObject )
{
	return (PX_Object_Image *)PX_ObjectGetDescByType(pObject,PX_OBJECT_TYPE_IMAGE);	
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
		pImg->pgif= PX_NULL;
	}
}

px_void PX_Object_ImageSetGif(PX_Object* pObject, px_gif* pgif)
{
	PX_Object_Image* pImg = PX_Object_GetImage(pObject);
	if (pImg)
	{
		pImg->pgif = pgif;
		pImg->pTexture = PX_NULL;
	}
}

px_void PX_Object_ImageAutoSize(PX_Object* pObject)
{
	PX_Object_Image* pImg = PX_Object_GetImage(pObject);
	if (pImg)
	{
		if (pImg->pTexture)
		{
			PX_ObjectSetSize(pObject, pImg->pTexture->width*1.0f, pImg->pTexture->height*1.0f, 0);
		}
		else if (pImg->pgif)
		{
			PX_ObjectSetSize(pObject, pImg->pgif->width*1.0f, pImg->pgif->height*1.0f, 0);
		}
	}
}

px_bool PX_Object_ImageLoadTexture(PX_Object* pObject, px_void* buffer, px_int size)
{
	PX_Object_Image* pImg = PX_Object_GetImage(pObject);
	if (pImg)
	{
		if (pImg->texture.MP)
		{
			PX_TextureFree(&pImg->texture);
		}
		if (!PX_TextureCreateFromMemory(pObject->mp, buffer, size, &pImg->texture))return PX_FALSE;
		pImg->pTexture=&pImg->texture;
		return PX_TRUE;
	}
	return PX_FALSE;
}

px_bool PX_Object_ImageLoadGif(PX_Object* pObject, px_void* buffer, px_int size)
{
	PX_Object_Image* pImg = PX_Object_GetImage(pObject);
	if (pImg)
	{
		if (pImg->gif.mp)
		{
			PX_GifFree(&pImg->gif);
		}
		if (!PX_GifCreate(pObject->mp, &pImg->gif, buffer, size))return PX_FALSE;
		pImg->pgif=&pImg->gif;
		return PX_TRUE;
	}
	return PX_FALSE;
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

px_void PX_Object_ImageSetAlpha(PX_Object* pObject, px_float alpha)
{
	PX_Object_Image* pImg = PX_Object_GetImage(pObject);
	if (pImg)
	{
		pImg->alpha = alpha;
	}
}

