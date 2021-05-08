#ifndef PX_OBJECT_TRANSFORMADAPTER_H
#define PX_OBJECT_TRANSFORMADAPTER_H
#include "PX_Object.h"

typedef enum
{
	PX_OBJECT_TRANSFORMADAPTER_MODE_ANY,
	PX_OBJECT_TRANSFORMADAPTER_MODE_ROTATION,
	PX_OBJECT_TRANSFORMADAPTER_MODE_STRETCH
}PX_OBJECT_TRANSFORMADAPTER_MODE;


typedef struct  
{
	px_point2D sourceAdaptPoint,currentAdaptPoint,cursor;
	px_float startAngle;
	px_float endAngle;
	px_float stretch;
	px_bool bSelect;
	px_color color,fontColor;
	PX_OBJECT_TRANSFORMADAPTER_MODE mode;
}PX_Object_TransformAdapter;

PX_Object_TransformAdapter *PX_Object_GetTransformAdapter(PX_Object *Object);
PX_Object * PX_Object_TransformAdapterCreate(px_memorypool *mp, PX_Object *Parent,px_int x,px_int y,px_point2D sourceAdaptPoint);
px_void PX_Object_TransformAdapterSetMode(PX_Object *Object,PX_OBJECT_TRANSFORMADAPTER_MODE mode);
px_float PX_Object_TransformAdapterGetRotation(PX_Object *Object);
px_float PX_Object_TransformAdapterGetStretch(PX_Object *Object);
px_void PX_Object_TransformAdapterResetState(PX_Object *pObject,px_float x,px_float y,px_point2D sourceAdaptPoint,px_float rotation,px_float stretch);
#endif
