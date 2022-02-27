#include "PX_Object_TransformAdapter.h"

PX_Object_TransformAdapter * PX_Object_GetTransformAdapter(PX_Object *Object)
{
	if (Object->Type==PX_OBJECT_TYPE_TRANSFORMADAPTER)
	{
		return (PX_Object_TransformAdapter *)Object->pObject;
	}
	return PX_NULL;
}

px_void PX_Object_TransformAdapterRender(px_surface *rendersurface,PX_Object *pObject,px_dword elapsed)
{
	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;
	PX_Object_TransformAdapter *pTransformAdapter=PX_Object_GetTransformAdapter(pObject);
	px_point2D vres,vnow;
	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;

	//////////////////////////////////////////////////////////////////////////
	//update rotation & stretch
	vres=PX_Point2DSub(pTransformAdapter->sourceAdaptPoint,PX_POINT2D(pObject->x,pObject->y));
	vnow=PX_Point2DRotate(vres,pTransformAdapter->endAngle-pTransformAdapter->startAngle);

	

	if (pTransformAdapter->mode==PX_OBJECT_TRANSFORMADAPTER_MODE_ANY||pTransformAdapter->mode==PX_OBJECT_TRANSFORMADAPTER_MODE_ROTATION)
	{
		//draw circle
		px_color rendercolor=pTransformAdapter->color;
		rendercolor._argb.a/=4;
		PX_GeoDrawCircle(rendersurface,(px_int)pObject->x,(px_int)pObject->y,(px_int)PX_Point2DMod(vres),1,rendercolor);
	}

	if (pTransformAdapter->mode==PX_OBJECT_TRANSFORMADAPTER_MODE_ANY||pTransformAdapter->mode==PX_OBJECT_TRANSFORMADAPTER_MODE_STRETCH)
	{
		//draw line
		px_color rendercolor=pTransformAdapter->color;
		rendercolor._argb.a/=4;

		PX_GeoDrawLine(rendersurface,(px_int)pObject->x,(px_int)pObject->y,(px_int)(vres.x+pObject->x),(px_int)(vres.y+pObject->y),1,rendercolor);
		PX_GeoDrawLine(rendersurface,(px_int)pObject->x,(px_int)pObject->y,(px_int)(vnow.x+pObject->x),(px_int)(vnow.y+pObject->y),1,rendercolor);
	}

	//draw point
	vnow=PX_Point2DMul(vnow,pTransformAdapter->stretch);
	PX_GeoDrawSector(rendersurface,(px_int)objx,(px_int)objy,PX_Point2DMod(vnow),1,PX_COLOR(16,0,255,255),(px_int)pTransformAdapter->startAngle, (px_int)pTransformAdapter->endAngle);

	PX_GeoDrawSolidCircle(rendersurface,(px_int)(vnow.x+pObject->x),(px_int)(vnow.y+pObject->y),5,pTransformAdapter->color);
	if (pTransformAdapter->bSelect)
	{
		PX_GeoDrawCircle(rendersurface,(px_int)(vnow.x+pObject->x),(px_int)(vnow.y+pObject->y),8,1,pTransformAdapter->color);
	}
	PX_GeoDrawCircle(rendersurface,(px_int)pObject->x,(px_int)pObject->y,(px_int)PX_Point2DMod(vnow),1,pTransformAdapter->color);
	PX_GeoDrawLine(rendersurface,(px_int)pObject->x,(px_int)pObject->y,(px_int)(vnow.x+pObject->x),(px_int)(vnow.y+pObject->y),1,pTransformAdapter->color);

	do 
	{
		px_char content[32];
		PX_sprintf2(content,sizeof(content),"rotation:%1.1 stretch:%2.2",PX_STRINGFORMAT_FLOAT(pTransformAdapter->endAngle-pTransformAdapter->startAngle),PX_STRINGFORMAT_FLOAT(pTransformAdapter->stretch));
		PX_FontDrawText(rendersurface,(px_int)(vnow.x+pObject->x),(px_int)(vnow.y+pObject->y-2),PX_ALIGN_MIDBOTTOM,content,pTransformAdapter->fontColor);
	} while (0);


}

px_void PX_Object_TransformAdapterOnCursorDown(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	px_float objx,objy,objWidth,objHeight;

	px_float inheritX,inheritY,x,y;
	PX_Object_TransformAdapter *pTransformAdapter=PX_Object_GetTransformAdapter(pObject);
	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;

	x=PX_Object_Event_GetCursorX(e);
	y=PX_Object_Event_GetCursorY(e);

	do 
	{
		px_point2D vres,vnow;
		vres=PX_Point2DSub(pTransformAdapter->sourceAdaptPoint,PX_POINT2D(pObject->x,pObject->y));
		vnow=PX_Point2DRotate(vres,pTransformAdapter->endAngle-pTransformAdapter->startAngle);
		vnow=PX_Point2DMul(vnow,pTransformAdapter->stretch);
		vnow.x+=pObject->x;
		vnow.y+=pObject->y;
		if (PX_isPoint2DInCircle(PX_POINT2D(x,y),vnow,5))
		{
			pTransformAdapter->bSelect=PX_TRUE;
		}
		else
		{
			pTransformAdapter->bSelect=PX_FALSE;
		}
	} while (0);
}


px_void PX_Object_TransformAdapterOnCursorDrag(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	px_float objx,objy,objWidth,objHeight;
	px_point2D vres;
	px_float inheritX,inheritY,x,y;
	PX_Object_TransformAdapter *pTransformAdapter=PX_Object_GetTransformAdapter(pObject);

	if(pTransformAdapter->bSelect) 
	{
		PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

		objx=(pObject->x+inheritX);
		objy=(pObject->y+inheritY);
		objWidth=pObject->Width;
		objHeight=pObject->Height;

		x=PX_Object_Event_GetCursorX(e);
		y=PX_Object_Event_GetCursorY(e);

		if (pTransformAdapter->mode==PX_OBJECT_TRANSFORMADAPTER_MODE_ANY||pTransformAdapter->mode==PX_OBJECT_TRANSFORMADAPTER_MODE_ROTATION)
		{
			px_float angle=(px_float)PX_RadianToAngle(PX_atan2(y-objy*1.0,x-objx*1.0));
			px_float lastAngleMod=pTransformAdapter->endAngle;
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
			pTransformAdapter->endAngle+=inc;
		}
		if (pTransformAdapter->mode==PX_OBJECT_TRANSFORMADAPTER_MODE_ANY||pTransformAdapter->mode==PX_OBJECT_TRANSFORMADAPTER_MODE_STRETCH)
		{
			vres=PX_POINT2D(pObject->x-x,pObject->y-y);
			pTransformAdapter->stretch=PX_Point2DMod(vres)/PX_Point2DMod(PX_Point2DSub(pTransformAdapter->sourceAdaptPoint,PX_POINT2D(pObject->x,pObject->y)));
		}
		
		PX_ObjectExecuteEvent(pObject,PX_OBJECT_BUILD_EVENT(PX_OBJECT_EVENT_VALUECHANGED));
	}
}


PX_Object * PX_Object_TransformAdapterCreate(px_memorypool *mp, PX_Object *Parent,px_int x,px_int y,px_point2D sourceAdaptPoint)
{
	PX_Object *pObject;
	PX_Object_TransformAdapter TransformAdapter;
	PX_memset(&TransformAdapter,0,sizeof(TransformAdapter));
	TransformAdapter.color=PX_COLOR(255,255,64,64);
	TransformAdapter.fontColor=PX_COLOR(255,255,0,0);
	TransformAdapter.stretch=1;
	TransformAdapter.currentAdaptPoint=sourceAdaptPoint;
	TransformAdapter.sourceAdaptPoint=sourceAdaptPoint;
	TransformAdapter.startAngle=(px_float)PX_RadianToAngle(PX_atan2(sourceAdaptPoint.y-y*1.0,sourceAdaptPoint.x-x*1.0));
	TransformAdapter.endAngle=TransformAdapter.startAngle;
	pObject=PX_ObjectCreateEx(mp,Parent,(px_float)x,(px_float)y,0,0,0,0,PX_OBJECT_TYPE_TRANSFORMADAPTER,0,PX_Object_TransformAdapterRender,0,&TransformAdapter,sizeof(TransformAdapter));
	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_CURSORDOWN,PX_Object_TransformAdapterOnCursorDown,PX_NULL);
	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_CURSORDRAG,PX_Object_TransformAdapterOnCursorDrag,PX_NULL);
	return pObject;
}

px_void PX_Object_TransformAdapterSetMode(PX_Object *Object,PX_OBJECT_TRANSFORMADAPTER_MODE mode)
{
	PX_Object_GetTransformAdapter(Object)->mode=mode;
}

px_float PX_Object_TransformAdapterGetRotation(PX_Object *Object)
{
	return PX_Object_GetTransformAdapter(Object)->endAngle-PX_Object_GetTransformAdapter(Object)->startAngle;
}

px_float PX_Object_TransformAdapterGetStretch(PX_Object *Object)
{
	return PX_Object_GetTransformAdapter(Object)->stretch;
}

px_void PX_Object_TransformAdapterResetState(PX_Object *pObject,px_float x,px_float y,px_point2D sourceAdaptPoint,px_float rotation,px_float stretch)
{
	PX_Object_TransformAdapter *pTransformAdapter=PX_Object_GetTransformAdapter(pObject);
	pObject->x=x;
	pObject->y=y;
	pTransformAdapter->startAngle=(px_float)PX_RadianToAngle(PX_atan2(sourceAdaptPoint.y-y*1.0,sourceAdaptPoint.x-x*1.0));
	pTransformAdapter->endAngle=pTransformAdapter->startAngle+rotation;
	pTransformAdapter->sourceAdaptPoint=sourceAdaptPoint;
	pTransformAdapter->stretch=stretch;
}

