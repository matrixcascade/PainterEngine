#ifndef PX_3D_OBJDATA_H
#define PX_3D_OBJDATA_H
#include "PX_Lexer.h"

typedef struct 
{
	px_int  v,vn,vt;
}PX_3D_ObjectDataVertex;


typedef struct  
{
	px_uint  mtlFileNameIndex;
	px_uint  mtlNameIndex;
	PX_3D_ObjectDataVertex v[3];
}PX_3D_ObjectDataFace;


typedef struct
{
	px_float x,y,z;
}PX_3D_ObjectData_v;

typedef struct
{
	px_float x,y,z;
}PX_3D_ObjectData_vn;

typedef struct
{
	px_float u,v;
}PX_3D_ObjectData_vt;

typedef struct
{
	px_memorypool *mp;
	px_vector v;
	px_vector vt;
	px_vector vn;
	px_vector face;
	px_vector mtlFile;
	px_vector mtlName;
}PX_3D_ObjectData;

px_bool PX_3D_ObjectDataInitialize(px_memorypool *mp,PX_3D_ObjectData *ObjectData);
px_bool PX_3D_ObjectDataLoad(PX_3D_ObjectData *ObjectData,const px_byte *data,px_int size);
px_bool PX_3D_ObjectDataToRenderList(PX_3D_ObjectData *ObjectData,PX_3D_RenderList *renderList);
px_void PX_3D_ObjectDataFree(PX_3D_ObjectData *ObjectData);

#endif
