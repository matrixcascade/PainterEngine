#include "PX_Object_3D.h"

PX_OBJECT_RENDER_FUNCTION(PX_Object_3DModelRender)
{
	PX_Object_3DModel *pDesc=PX_ObjectGetDesc(PX_Object_3DModel,pObject);
	px_float objx, objy, objWidth, objHeight;
	px_float inheritX, inheritY;
	PX_ObjectGetInheritXY(pObject, &inheritX, &inheritY);
	objx = (pObject->x + inheritX);
	objy = (pObject->y + inheritY);
	objWidth = pObject->Width;
	objHeight = pObject->Height;

	PX_SurfaceClearAll(& pDesc->renderSurface,PX_COLOR(0, 255, 255, 255));
	PX_3D_Scene(&pDesc->renderlist, &pDesc->world, &pDesc->camera);
	PX_3D_Present(&pDesc->renderSurface, &pDesc->renderlist, &pDesc->camera);
	PX_SurfaceRender(psurface, &pDesc->renderSurface, (px_int)objx, (px_int)objy, PX_ALIGN_CENTER, PX_NULL);
}

PX_OBJECT_FREE_FUNCTION(PX_Object_3DModelFree)
{
	PX_Object_3DModel *pDesc=PX_ObjectGetDesc(PX_Object_3DModel,pObject);
	PX_3D_RenderListFree(&pDesc->renderlist);
	PX_TextureFree(&pDesc->renderSurface);
}

px_void PX_Object_3DModel_PixelShader(px_surface* psurface, px_int x, px_int y, px_point position, px_float u, px_float v, px_point4D normal, px_texture* pTexture, px_color color)
{
	px_float alpha;
	px_float cosv = PX_Point4DDot(PX_Point4DUnit(normal), PX_POINT4D(0, 0, 1));

	cosv = -cosv;
	if (cosv > 0)
	{
		alpha = (1 - cosv) * 128;
		PX_SurfaceDrawPixel(psurface, x, y, PX_COLOR(255, (px_uchar)(128 + alpha), (px_uchar)(128 + alpha), (px_uchar)(128 + alpha)));
	}
}

PX_Object* PX_Object_3DModelCreate(px_memorypool* mp, PX_Object* parent, px_float x, px_float y,px_float width,px_float height, PX_3D_ObjectData* pdata)
{
	PX_Object_3DModel* pdesc;
	PX_Object* pObject;
	pObject = PX_ObjectCreateEx(mp, parent, x, y, 0, 0, 0, 0, PX_OBJECT_TYPE_3DMODEL, 0, PX_Object_3DModelRender, PX_Object_3DModelFree, 0, sizeof(PX_Object_3DModel));
	pdesc=PX_ObjectGetDesc(PX_Object_3DModel, pObject);
	pdesc->mp = mp;
	pdesc->pObjData = pdata;
	//create surface
	if (!PX_SurfaceCreate(mp, (px_int)width, (px_int)height, &pdesc->renderSurface))
	{
		PX_ObjectDelete(pObject);
		return PX_NULL;
	}

	if (!PX_3D_RenderListInitialize(mp, &pdesc->renderlist, PX_3D_PRESENTMODE_TEXTURE | PX_3D_PRESENTMODE_PURE, PX_3D_CULLMODE_CCW, PX_NULL))
	{
		PX_ObjectDelete(pObject);
		PX_TextureFree(&pdesc->renderSurface);
		return PX_NULL;
	}
	PX_3D_RenderListSetPixelShader(&pdesc->renderlist, PX_Object_3DModel_PixelShader);
	if (!PX_3D_ObjectDataToRenderList(pdata, &pdesc->renderlist))
	{
		PX_ObjectDelete(pObject);
		PX_TextureFree(&pdesc->renderSurface);
		PX_3D_RenderListFree(&pdesc->renderlist);
		return PX_NULL;
	}
	
	if (!PX_3D_CameraUVNInitialize(mp, &pdesc->camera, PX_POINT4D(0, 0, 0), PX_POINT4D(0, 0, 1), 1, 32767, 90, width, height))
	{
		PX_ObjectDelete(pObject);
		PX_TextureFree(&pdesc->renderSurface);
		PX_3D_RenderListFree(&pdesc->renderlist);
		return PX_NULL;
	}
	PX_3D_WorldInitialize(&pdesc->world, 0, 0, 1.2f, 0, 0, 0, 1);
	return pObject;
}

px_void PX_Object_3DModelSetCamera(PX_Object* pObject,px_point4D pos,px_point4D direction)
{
	PX_Object_3DModel* pdesc;
	if (pObject->Type== PX_OBJECT_TYPE_3DMODEL)
	{
		pdesc = PX_ObjectGetDesc(PX_Object_3DModel, pObject);
		PX_3D_CameraSetPosition(&pdesc->camera, pos, direction);
	}
	
}

px_void PX_Object_3DModelSetWorld(PX_Object* pObject, px_float x, px_float y, px_float z, px_float rotX, px_float rotY, px_float rotZ, px_float scale)
{
	PX_Object_3DModel* pdesc;
	pdesc = PX_ObjectGetDesc(PX_Object_3DModel, pObject);
	if (pObject->Type == PX_OBJECT_TYPE_3DMODEL)
	{
		pdesc = PX_ObjectGetDesc(PX_Object_3DModel, pObject);
		PX_3D_WorldInitialize(&pdesc->world, x, y, z, rotX, rotY, rotZ, scale);
	}

}