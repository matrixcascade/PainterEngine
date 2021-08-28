#include "PX_Object_RadioBox.h"
//////////////////////////////////////////////////////////////////////////

PX_Object_RadioBox * PX_Object_GetRadioBox(PX_Object *Object)
{
	PX_Object_RadioBox *pcb=(PX_Object_RadioBox *)Object->pObject;
	if (Object->Type==PX_OBJECT_TYPE_RADIOBOX)
	{
		return pcb;
	}
	return PX_NULL;
}


px_void PX_Object_RadioBoxOnMouseMove(PX_Object *Object,PX_Object_Event e,px_void *user_ptr)
{
	PX_Object_RadioBox *pcb=PX_Object_GetRadioBox(Object);

	px_float x=(PX_Object_Event_GetCursorX(e));
	px_float y=(PX_Object_Event_GetCursorY(e));

	if (pcb)
	{
		if(PX_ObjectIsPointInRegion(Object,(px_float)x,(px_float)y))
		{
			if (pcb->state!=PX_OBJECT_BUTTON_STATE_ONPUSH)
			{
				if (pcb->state!=PX_OBJECT_BUTTON_STATE_ONCURSOR)
				{
					PX_Object_Event e;
					e.Event=PX_OBJECT_EVENT_CURSOROVER;
					e.Param_uint[0]=0;
					e.Param_uint[1]=0;
					e.Param_uint[2]=0;
					e.Param_uint[3]=0;
					e.Param_ptr[0]=PX_NULL;
					PX_ObjectExecuteEvent(Object,e);
				}
				pcb->state=PX_OBJECT_RADIOBOX_STATE_ONCURSOR;
			}
		}
		else
		{
			if (pcb->state!=PX_OBJECT_BUTTON_STATE_NORMAL)
			{
				PX_Object_Event e;
				e.Event=PX_OBJECT_EVENT_CURSOROUT;
				e.Param_uint[0]=0;
				e.Param_uint[1]=0;
				e.Param_uint[2]=0;
				e.Param_uint[3]=0;
				PX_ObjectExecuteEvent(Object,e);
			}
			pcb->state=PX_OBJECT_RADIOBOX_STATE_NORMAL;
		}
	}
}

px_void PX_Object_RadioBoxClearOtherCheck(PX_Object *Object)
{
	PX_Object *pObject=Object->pPreBrother;
	while (pObject)
	{
		if (PX_Object_GetRadioBox(pObject))
		{
			PX_Object_GetRadioBox(pObject)->bCheck=PX_FALSE;
		}
		pObject=pObject->pPreBrother;
	}

	pObject=Object->pNextBrother;
	while (pObject)
	{
		if (PX_Object_GetRadioBox(pObject))
		{
			PX_Object_GetRadioBox(pObject)->bCheck=PX_FALSE;
		}
		pObject=pObject->pNextBrother;
	}
}
px_void PX_Object_RadioBoxOnMouseLButtonDown(PX_Object *Object,PX_Object_Event e,px_void *user_ptr)
{
	PX_Object_RadioBox *pcb=PX_Object_GetRadioBox(Object);
	px_float x=(PX_Object_Event_GetCursorX(e));
	px_float y=(PX_Object_Event_GetCursorY(e));

	if (pcb)
	{
		if(PX_ObjectIsPointInRegion(Object,(px_float)x,(px_float)y))
		{
			pcb->state=PX_OBJECT_RADIOBOX_STATE_ONPUSH;
			PX_Object_RadioBoxClearOtherCheck(Object);
			pcb->bCheck=PX_TRUE;
			e.Event=PX_OBJECT_EVENT_VALUECHANGED;
			e.Param_uint[0]=0;
			e.Param_uint[1]=0;
			e.Param_uint[2]=0;
			e.Param_uint[3]=0;
			PX_ObjectExecuteEvent(Object,e);
		}
		else
		{
			if (pcb->state!=PX_OBJECT_BUTTON_STATE_NORMAL)
			{
				PX_Object_Event e;
				e.Event=PX_OBJECT_EVENT_CURSOROUT;
				e.Param_uint[0]=0;
				e.Param_uint[1]=0;
				e.Param_uint[2]=0;
				e.Param_uint[3]=0;
				PX_ObjectExecuteEvent(Object,e);
			}
			pcb->state=PX_OBJECT_RADIOBOX_STATE_NORMAL;
		}
	}
}

px_void PX_Object_RadioBoxOnMouseLButtonUp(PX_Object *Object,PX_Object_Event e,px_void *user_ptr)
{
	PX_Object_RadioBox *pcb=PX_Object_GetRadioBox(Object);

	px_float x,y;

	x=PX_Object_Event_GetCursorX(e);
	y=PX_Object_Event_GetCursorY(e);

	if (pcb)
	{
		if(PX_ObjectIsPointInRegion(Object,(px_float)x,(px_float)y))
			if(pcb->state==PX_OBJECT_RADIOBOX_STATE_ONPUSH)
			{
				pcb->state=PX_OBJECT_RADIOBOX_STATE_ONCURSOR;
			}
	}
}

px_void PX_Object_RadioBoxRender(px_surface *psurface, PX_Object *pObject,px_uint elapsed)
{

	PX_Object_RadioBox *pcb=PX_Object_GetRadioBox(pObject);
	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;

	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;


	if (pcb==PX_NULL)
	{
		return;
	}

	if (!pObject->Visible)
	{
		return;
	}

	PX_GeoDrawRect(psurface,(px_int)objx,(px_int)objy,(px_int)objx+(px_int)objWidth-1,(px_int)objy+(px_int)objHeight-1,pcb->BackgroundColor);
	switch (pcb->state)
	{
	case PX_OBJECT_BUTTON_STATE_NORMAL:
		PX_GeoDrawRect(psurface,(px_int)objx,(px_int)objy,(px_int)objx+(px_int)objWidth-1,(px_int)objy+(px_int)objHeight-1,pcb->BackgroundColor);
		break;
	case PX_OBJECT_BUTTON_STATE_ONPUSH:
		PX_GeoDrawRect(psurface,(px_int)objx,(px_int)objy,(px_int)objx+(px_int)objWidth-1,(px_int)objy+(px_int)objHeight-1,pcb->PushColor);
		break;
	case PX_OBJECT_BUTTON_STATE_ONCURSOR:
		PX_GeoDrawRect(psurface,(px_int)objx,(px_int)objy,(px_int)objx+(px_int)objWidth-1,(px_int)objy+(px_int)objHeight-1,pcb->CursorColor);
		break;
	}

	if (pcb->Border)
	{
		PX_GeoDrawBorder(psurface,(px_int)objx,(px_int)objy,(px_int)objx+(px_int)objWidth-1,(px_int)objy+(px_int)objHeight-1,1,pcb->BorderColor);
	}

	PX_FontModuleDrawText(psurface,pcb->fm,(px_int)objx+20,(px_int)(objy+objHeight/2),PX_ALIGN_LEFTMID,pcb->Text,pcb->TextColor);

	//draw RadioState
	PX_GeoDrawCircle(psurface,(px_int)objx+8,(px_int)(objy+objHeight/2),7,1,pcb->BorderColor);

	if (pcb->bCheck)
	{
		PX_GeoDrawSolidCircle(psurface,(px_int)objx+8,(px_int)(objy+objHeight/2),5,pcb->BorderColor);
	}

}

PX_Object * PX_Object_RadioBoxCreate(px_memorypool *mp, PX_Object *Parent,px_int x,px_int y,px_int Width,px_int Height,const char text[],PX_FontModule *fm)
{
	PX_Object *pObject;
	PX_Object_RadioBox cbx;
	cbx.Align=PX_ALIGN_LEFTMID;
	cbx.BackgroundColor=PX_COLOR(0,0,0,0);
	cbx.bCheck=PX_FALSE;
	cbx.Border=PX_FALSE;
	cbx.BorderColor=PX_OBJECT_UI_DEFAULT_FONTCOLOR;
	cbx.CursorColor=PX_COLOR(0,0,0,0);
	cbx.PushColor=PX_COLOR(0,0,0,0);
	cbx.fm=fm;
	cbx.state=PX_OBJECT_RADIOBOX_STATE_NORMAL;
	PX_strset(cbx.Text,text);
	cbx.TextColor= PX_OBJECT_UI_DEFAULT_FONTCOLOR;
	pObject=PX_ObjectCreateEx(mp,Parent,(px_float)x,(px_float)y,0,(px_float)Width,(px_float)Height,0,PX_OBJECT_TYPE_RADIOBOX,PX_NULL,PX_Object_RadioBoxRender,PX_NULL,&cbx,sizeof(cbx));
	if (!pObject)
	{
		return PX_NULL;
	}
	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_CURSORMOVE,PX_Object_RadioBoxOnMouseMove,PX_NULL);
	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_CURSORDOWN,PX_Object_RadioBoxOnMouseLButtonDown,PX_NULL);
	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_CURSORUP,PX_Object_RadioBoxOnMouseLButtonUp,PX_NULL);
	return pObject;
}

px_bool PX_Object_RadioBoxGetCheck(PX_Object *Object)
{
	return PX_Object_GetRadioBox(Object)->bCheck;
}

px_void PX_Object_RadioBoxSetBackgroundColor(PX_Object *Object,px_color clr)
{
	PX_Object_GetRadioBox(Object)->BackgroundColor=clr;
}

px_void PX_Object_RadioBoxSetBorderColor(PX_Object *Object,px_color clr)
{
	PX_Object_GetRadioBox(Object)->BorderColor=clr;
}

px_void PX_Object_RadioBoxSetPushColor(PX_Object *Object,px_color clr)
{
	PX_Object_GetRadioBox(Object)->PushColor=clr;
}

px_void PX_Object_RadioBoxSetCursorColor(PX_Object *Object,px_color clr)
{
	PX_Object_GetRadioBox(Object)->CursorColor=clr;
}

px_void PX_Object_RadioBoxSetText(PX_Object *Object,const px_char text[])
{
	PX_strset(PX_Object_GetRadioBox(Object)->Text,text);
}

const px_char *PX_Object_RadioBoxGetText(PX_Object *Object)
{
	PX_Object_RadioBox *pDesc=PX_Object_GetRadioBox(Object);
	if (pDesc)
	{
		return pDesc->Text;
	}
	PX_ASSERT();
	return PX_NULL;
}

px_void PX_Object_RadioBoxSetTextColor(PX_Object *Object,px_color clr)
{
	PX_Object_RadioBox *pDesc=PX_Object_GetRadioBox(Object);
	if (pDesc)
	{
		pDesc->TextColor=clr;
	}
	else
	PX_ASSERT();
}

px_void PX_Object_RadioBoxSetCheck(PX_Object *Object,px_bool check)
{
	PX_Object_GetRadioBox(Object)->bCheck=check;
}