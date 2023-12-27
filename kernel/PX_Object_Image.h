#ifndef PX_OBJECT_IMAGE_H
#define PX_OBJECT_IMAGE_H
#include "PX_Object.h"


typedef struct 
{
	PX_ALIGN Align;
	px_texture *pTexture;
	px_texture *pmask;
	px_gif* pgif;
	px_texture texture;
	px_gif gif;
}PX_Object_Image;

PX_Object * PX_Object_ImageCreate(px_memorypool *mp,PX_Object *Parent,px_int x,px_int y,px_int width,px_int height,px_texture *ptex );
PX_Object_Image *PX_Object_GetImage(PX_Object *pObject);
px_void	   PX_Object_ImageSetAlign(PX_Object *pObject,PX_ALIGN Align);
px_void	   PX_Object_ImageSetMask(PX_Object *pObject,px_texture *pmask);
px_void    PX_Object_ImageSetTexture(PX_Object *pObject,px_texture *pTex);
px_void	   PX_Object_ImageSetGif(PX_Object *pObject,px_gif *pgif);
px_void    PX_Object_ImageAutoSize(PX_Object *pObject);
px_bool	   PX_Object_ImageLoadTexture(PX_Object* pObject, px_void *buffer,px_int size);
px_bool	   PX_Object_ImageLoadGif(PX_Object* pObject, px_void *buffer,px_int size);
px_void	   PX_Object_ImageFreeWithTexture(PX_Object *pObject);



#endif



