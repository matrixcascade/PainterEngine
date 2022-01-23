#ifndef PX_OBJECT_DATABOX_H
#define PX_OBJECT_DATABOX_H
#include "PX_Object.h"
typedef struct
{

}PX_Object_DataBox;

PX_Object_DataBox* PX_Object_GetDataBox(PX_Object* Object);
PX_Object* PX_Object_DataBoxCreate(px_memorypool* mp, PX_Object* Parent, int x, int y, int width, int height, const px_char title[], PX_FontModule* fontmodule);
px_void PX_Object_DataBoxShow(PX_Object* pObject);
px_void PX_Object_DataBoxClose(PX_Object* pObject);

#endif // !PX_OBJECT_DATABOX_H
