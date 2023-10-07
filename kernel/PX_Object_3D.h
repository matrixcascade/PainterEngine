#ifndef PX_OBJECT_3D_H
#define PX_OBJECT_3D_H
#include "PX_Object.h"
#include "PX_3D_ObjData.h"

typedef struct 
{
	px_memorypool *mp;
	PX_3D_RenderList renderlist;
	PX_3D_Camera camera;
	PX_3D_World world;
	PX_3D_ObjectData *pObjData;
	px_surface renderSurface;
}PX_Object_3DModel;

PX_Object* PX_Object_3DModelCreate(px_memorypool* mp, PX_Object* parent, px_float x, px_float y, px_float width, px_float height, PX_3D_ObjectData* pdata);
px_void PX_Object_3DModelSetCamera(PX_Object* pObject, px_point4D pos, px_point4D direction);
px_void PX_Object_3DModelSetWorld(PX_Object* pObject, px_float x, px_float y, px_float z, px_float rotX, px_float rotY, px_float rotZ, px_float scale);
#endif



