#include "PX_Object_CursorButton.h"


px_void PX_Object_CursorButtonRender(px_surface *psurface, PX_Object *pObject,px_uint elapsed)
{
	px_float w,h,_x,_y;
	px_uchar alpha;
	PX_Object_CursorButton *pcb;
	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;

	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;


	w=objWidth/4.f;
	h=objHeight/4.f;
	pcb=PX_Object_GetCursorButton(pObject);
	if (!pcb)
	{
		return;
	}
	pcb->pushbutton->x=0;
	pcb->pushbutton->y=0;
	pcb->pushbutton->z=0;

	if(pcb->enter)
	{
		if(PX_ABS(pcb->c_distance-pcb->c_distance_near)>=1)
			pcb->c_distance+=(pcb->c_distance_near-pcb->c_distance)*(elapsed*1.f/200);
		else
			pcb->c_distance=pcb->c_distance_near;
	}
	else
	{
		if(PX_ABS(pcb->c_distance-pcb->c_distance_far)>=1)
			pcb->c_distance+=(pcb->c_distance_far-pcb->c_distance)*(elapsed*1.f/200);
		else
			pcb->c_distance=pcb->c_distance_far;
	}
	alpha=(px_uchar)(PX_ABS(pcb->c_distance-pcb->c_distance_far)/PX_ABS(pcb->c_distance_far-pcb->c_distance_near)*255);
	pcb->c_color._argb.a=alpha;
	//left top
	_x=objx-pcb->c_distance-1;
	_y=objy-pcb->c_distance-1;
	PX_GeoDrawRect(psurface,(px_int)_x,(px_int)_y,(px_int)(_x+w),(px_int)_y+pcb->c_width,pcb->c_color);
	PX_GeoDrawRect(psurface,(px_int)_x,(px_int)_y+pcb->c_width+1,(px_int)_x+pcb->c_width,(px_int)(_y+h),pcb->c_color);

	//left bottom
	_x=objx-pcb->c_distance-1;
	_y=objy+objHeight+pcb->c_distance;
	PX_GeoDrawRect(psurface,(px_int)_x,(px_int)_y,(px_int)(_x+w),(px_int)_y-pcb->c_width,pcb->c_color);
	PX_GeoDrawRect(psurface,(px_int)_x,(px_int)_y-pcb->c_width-1,(px_int)_x+pcb->c_width,(px_int)(_y-h),pcb->c_color);

	//right top
	_x=objx+objWidth+pcb->c_distance;
	_y=objy-pcb->c_distance-1;
	PX_GeoDrawRect(psurface,(px_int)_x,(px_int)_y,(px_int)(_x-w),(px_int)_y+pcb->c_width,pcb->c_color);
	PX_GeoDrawRect(psurface,(px_int)_x,(px_int)_y+pcb->c_width+1,(px_int)_x-pcb->c_width,(px_int)(_y+h),pcb->c_color);

	//right bottom
	_x=objx+objWidth+pcb->c_distance;
	_y=objy+objHeight+pcb->c_distance;
	PX_GeoDrawRect(psurface,(px_int)_x,(px_int)_y,(px_int)(_x-w),(px_int)_y-pcb->c_width,pcb->c_color);
	PX_GeoDrawRect(psurface,(px_int)_x,(px_int)_y-pcb->c_width-1,(px_int)_x-pcb->c_width,(px_int)(_y-h),pcb->c_color);
}

px_void PX_Object_CursorButtonFree( PX_Object *Obj )
{

}
px_void PX_Object_CursorButtonOnMouseMove(PX_Object *Object,PX_Object_Event e,px_void *user_ptr)
{

	PX_Object_CursorButton *pCB=PX_Object_GetCursorButton(Object);
	px_float x,y;
	x=PX_Object_Event_GetCursorX(e);
	y=PX_Object_Event_GetCursorY(e);

	if(PX_ObjectIsPointInRegion(Object,(px_float)x,(px_float)y))
	{
		if (!pCB->enter)
		{
			pCB->enter=PX_TRUE;
			//pCB->c_distance=pCB->c_distance_far;
		}
	}
	else
	{
		if (pCB->enter)
		{
			pCB->enter=PX_FALSE;
			//pCB->c_distance=pCB->c_distance_near;
		}
	}
}

PX_Object *PX_Object_CursorButtonCreate(px_memorypool *mp,PX_Object *Parent,px_int x,px_int y,px_int Width,px_int Height,const px_char *Text,PX_FontModule *fontmodule,px_color Color)
{
	PX_Object *pObject;
	PX_Object_CursorButton *pCb=(PX_Object_CursorButton *)MP_Malloc(mp,sizeof(PX_Object_CursorButton));
	if (pCb==PX_NULL)
	{
		return PX_NULL;
	}
	pObject=PX_ObjectCreate(mp,Parent,(px_float)x,(px_float)y,0,(px_float)Width,(px_float)Height,0);
	if (pObject==PX_NULL)
	{
		MP_Free(pObject->mp,pCb);
		return PX_NULL;
	}
	pObject->pObject=pCb;
	pObject->Enabled=PX_TRUE;
	pObject->Visible=PX_TRUE;
	pObject->Type=PX_OBJECT_TYPE_CURSORBUTTON;
	pObject->ReceiveEvents=PX_TRUE;
	pObject->Func_ObjectFree=PX_NULL;
	pObject->Func_ObjectUpdate=PX_NULL;
	pObject->Func_ObjectRender=PX_Object_CursorButtonRender;


	pCb->pushbutton=PX_Object_PushButtonCreate(mp,pObject,x,y,Width,Height,Text,fontmodule);
	pCb->c_color= PX_OBJECT_UI_DEFAULT_BORDERCOLOR;
	pCb->c_distance=0;
	pCb->c_distance_far=(px_float)(Width>Height?Height/4:Width/4);
	pCb->c_distance_near=3;
	pCb->enter=PX_FALSE;
	if (Height<16)
	{
		pCb->c_width=1;
	}
	else
		pCb->c_width=Height/16;

	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_CURSORMOVE,PX_Object_CursorButtonOnMouseMove,PX_NULL);
	return pObject;
}
PX_Object_CursorButton * PX_Object_GetCursorButton( PX_Object *Object )
{
	if(Object->Type==PX_OBJECT_TYPE_CURSORBUTTON)
		return (PX_Object_CursorButton *)Object->pObject;
	else
		return PX_NULL;
}

PX_Object * PX_Object_GetCursorButtonPushButton(PX_Object *Object)
{
	PX_Object_CursorButton *pcb;
	pcb=PX_Object_GetCursorButton(Object);
	if (!pcb)
	{
		return PX_NULL;
	}
	return pcb->pushbutton;
}


