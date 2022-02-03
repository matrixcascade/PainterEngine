#ifndef PX_OBJECT_PROTRACTOR_H
#define PX_OBJECT_PROTRACTOR_H
#include "PX_Object.h"

typedef enum
{
	PX_OBJECT_PROTRACTOR_STATE_STANDBY=0,
	PX_OBJECT_PROTRACTOR_STATE_ADJUST,
	PX_OBJECT_PROTRACTOR_STATE_DONE
}PX_OBJECT_PROTRACTOR_STATE;


typedef struct  
{
	px_float radius;
	px_float startAngle;
	px_float endAngle;
	px_bool bselect;
	px_color color,fontColor;
	PX_OBJECT_PROTRACTOR_STATE state;
}PX_Object_Protractor;

PX_Object_Protractor *PX_Object_GetProtractor(PX_Object *Object);
PX_Object * PX_Object_ProtractorCreate(px_memorypool *mp, PX_Object *Parent,px_int x,px_int y,px_int radius);
px_float PX_Object_ProtractorGetAngle(PX_Object *Object);

#endif
