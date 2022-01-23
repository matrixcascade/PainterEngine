#include "PX_Object_Protractor.h"

PX_Object_Protractor * PX_Object_GetProtractor(PX_Object *Object)
{
	if (Object->Type==PX_OBJECT_TYPE_PROTRACTOR)
	{
		return (PX_Object_Protractor *)Object->pObject;
	}
	return PX_NULL;
}

px_void PX_Object_ProtractorRender(px_surface *rendersurface,PX_Object *pObject,px_dword elapsed)
{
	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;
	PX_Object_Protractor *pProtractor=PX_Object_GetProtractor(pObject);
	px_color acolor;
	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;

	PX_GeoDrawCircle(rendersurface,(px_int)objx,(px_int)objy,(px_int)pProtractor->radius,1,pProtractor->color);
	PX_GeoDrawSolidCircle(rendersurface,(px_int)objx,(px_int)objy,3,pProtractor->color);

	switch (pProtractor->state)
	{
	case PX_OBJECT_PROTRACTOR_STATE_STANDBY:
		{
			PX_GeoDrawLine(rendersurface,(px_int)objx,(px_int)objy,(px_int)(objx+PX_cos_angle(pProtractor->startAngle)*pProtractor->radius),
				(px_int)(objy+PX_sin_angle(pProtractor->startAngle)*pProtractor->radius),1,pProtractor->color);

			PX_GeoDrawSolidCircle(rendersurface,(px_int)(objx+PX_cos_angle(pProtractor->startAngle)*pProtractor->radius),
				(px_int)(objy+PX_sin_angle(pProtractor->startAngle)*pProtractor->radius),3     
				,pProtractor->color);
		}
		break;
	case PX_OBJECT_PROTRACTOR_STATE_ADJUST:
	case PX_OBJECT_PROTRACTOR_STATE_DONE:
		{
			px_char degree[32];
			acolor=pProtractor->color;
			acolor._argb.a/=4;
			
			PX_GeoDrawSector(rendersurface,(px_int)objx,(px_int)objy,pProtractor->radius,1,acolor, (px_int)pProtractor->startAngle,(px_int)pProtractor->endAngle);

			PX_GeoDrawLine(rendersurface,(px_int)objx,(px_int)objy,(px_int)(objx+PX_cos_angle(pProtractor->startAngle)*pProtractor->radius),
				(px_int)(objy+PX_sin_angle(pProtractor->startAngle)*pProtractor->radius),1,pProtractor->color);

			PX_GeoDrawLine(rendersurface,(px_int)objx,(px_int)objy,(px_int)(objx+PX_cos_angle(pProtractor->endAngle)*pProtractor->radius),
				(px_int)(objy+PX_sin_angle(pProtractor->endAngle)*pProtractor->radius),1,pProtractor->color);

			PX_GeoDrawSolidCircle(rendersurface,(px_int)(objx+PX_cos_angle(pProtractor->endAngle)*pProtractor->radius),
				(px_int)(objy+PX_sin_angle(pProtractor->endAngle)*pProtractor->radius),5,pProtractor->color);

			PX_itoa((px_int)PX_Object_ProtractorGetAngle(pObject),degree,sizeof(degree),10);
			PX_FontModuleDrawText(rendersurface,PX_NULL,(px_int)objx,(px_int)objy-2,PX_ALIGN_MIDBOTTOM,degree,pProtractor->fontColor);
		}
		break;
	}

	

}

px_void PX_Object_ProtractorOnCursorDown(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY,x,y;
	PX_Object_Protractor *pProtractor=PX_Object_GetProtractor(pObject);
	x=PX_Object_Event_GetCursorX(e);
	y=PX_Object_Event_GetCursorY(e);

	if (pProtractor->state==PX_OBJECT_PROTRACTOR_STATE_STANDBY)
	{
		PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);
		objx=(pObject->x+inheritX);
		objy=(pObject->y+inheritY);
		objWidth=pObject->Width;
		objHeight=pObject->Height;
		pProtractor->endAngle=pProtractor->startAngle;
		pProtractor->state=PX_OBJECT_PROTRACTOR_STATE_ADJUST;
	}
	else if (pProtractor->state==PX_OBJECT_PROTRACTOR_STATE_ADJUST)
	{
		PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);
		objx=(pObject->x+inheritX);
		objy=(pObject->y+inheritY);
		objWidth=pObject->Width;
		objHeight=pObject->Height;
		pProtractor->state=PX_OBJECT_PROTRACTOR_STATE_DONE;
	}
	
}

px_void PX_Object_ProtractorOnCursorMove(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY,x,y;
	PX_Object_Protractor *pProtractor=PX_Object_GetProtractor(pObject);
	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;

	x=PX_Object_Event_GetCursorX(e);
	y=PX_Object_Event_GetCursorY(e);

	switch (pProtractor->state)
	{
	case PX_OBJECT_PROTRACTOR_STATE_STANDBY:
		{
			pProtractor->startAngle=(px_float)PX_RadianToAngle(PX_atan2(y-objy*1.0,x-objx*1.0));
		}
		break;
	case PX_OBJECT_PROTRACTOR_STATE_ADJUST:
		{
			px_float angle=(px_float)PX_RadianToAngle(PX_atan2(y-objy*1.0,x-objx*1.0));
			px_float lastAngleMod=pProtractor->endAngle;
			px_float inc;
			px_int   round=(px_int)(lastAngleMod/360);
			lastAngleMod=lastAngleMod-round*360;

			inc=angle-lastAngleMod;
			if (inc>180)
			{
				inc-=360;
			}
			if (inc<-180)
			{
				inc+=360;
			}
			pProtractor->endAngle+=inc;
			
		}
		break;
	default:
		break;
	}

}

px_void PX_Object_ProtractorOnCursorRDown(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Object_Protractor *pProtractor=PX_Object_GetProtractor(pObject);
	pProtractor->state=PX_OBJECT_PROTRACTOR_STATE_STANDBY;
}

PX_Object * PX_Object_ProtractorCreate(px_memorypool *mp, PX_Object *Parent,px_int x,px_int y,px_int radius)
{
	PX_Object *pObject;
	PX_Object_Protractor protractor;
	PX_memset(&protractor,0,sizeof(protractor));
	protractor.color=PX_OBJECT_UI_DEFAULT_BORDERCOLOR;
	protractor.fontColor=PX_COLOR(255,255,0,0);
	protractor.radius=(px_float)radius;
	
	pObject=PX_ObjectCreateEx(mp,Parent,(px_float)x,(px_float)y,0,0,0,0,PX_OBJECT_TYPE_PROTRACTOR,0,PX_Object_ProtractorRender,0,&protractor,sizeof(protractor));
	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_CURSORDOWN,PX_Object_ProtractorOnCursorDown,PX_NULL);
	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_CURSORMOVE,PX_Object_ProtractorOnCursorMove,PX_NULL);
	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_CURSORRDOWN,PX_Object_ProtractorOnCursorRDown,PX_NULL);
	return pObject;
}

px_float PX_Object_ProtractorGetAngle(PX_Object *Object)
{
	return PX_Object_GetProtractor(Object)->endAngle-PX_Object_GetProtractor(Object)->startAngle;
}

