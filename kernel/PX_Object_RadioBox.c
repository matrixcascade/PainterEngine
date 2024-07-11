#include "PX_Object_RadioBox.h"
//////////////////////////////////////////////////////////////////////////

PX_Object_RadioBox * PX_Object_GetRadioBox(PX_Object *pObject)
{
	PX_Object_RadioBox*pdesc=(PX_Object_RadioBox*)PX_ObjectGetDescByType(pObject,PX_OBJECT_TYPE_RADIOBOX);
	PX_ASSERTIF(pdesc==PX_NULL);
	return pdesc;
}


PX_OBJECT_EVENT_FUNCTION(PX_Object_RadioBoxOnMouseMove)
{
	PX_Object_RadioBox *pcb=PX_Object_GetRadioBox(pObject);

	px_float x=(PX_Object_Event_GetCursorX(e));
	px_float y=(PX_Object_Event_GetCursorY(e));

	if (pcb)
	{
		if(PX_ObjectIsPointInRegion(pObject,(px_float)x,(px_float)y))
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
					PX_ObjectExecuteEvent(pObject,e);
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
				PX_ObjectExecuteEvent(pObject,e);
			}
			pcb->state=PX_OBJECT_RADIOBOX_STATE_NORMAL;
		}
	}
}

px_void PX_Object_RadioBoxClearOtherCheck(PX_Object *pObject)
{
	PX_Object *pPreBrotherObject=pObject->pPreBrother;
	while (pPreBrotherObject)
	{
		if (PX_Object_GetRadioBox(pPreBrotherObject))
		{
			PX_Object_GetRadioBox(pPreBrotherObject)->bCheck=PX_FALSE;
		}
		pPreBrotherObject = pPreBrotherObject->pPreBrother;
	}

	pPreBrotherObject =pObject->pNextBrother;
	while (pPreBrotherObject)
	{
		if (PX_Object_GetRadioBox(pPreBrotherObject))
		{
			PX_Object_GetRadioBox(pPreBrotherObject)->bCheck=PX_FALSE;
		}
		pPreBrotherObject = pPreBrotherObject->pNextBrother;
	}
}

PX_OBJECT_EVENT_FUNCTION(PX_Object_RadioBoxOnMouseLButtonDown)
{
	PX_Object_RadioBox *pcb=PX_Object_GetRadioBox(pObject);
	px_float x=(PX_Object_Event_GetCursorX(e));
	px_float y=(PX_Object_Event_GetCursorY(e));

	if (pcb)
	{
		if(PX_ObjectIsPointInRegion(pObject,(px_float)x,(px_float)y))
		{
			pcb->state=PX_OBJECT_RADIOBOX_STATE_ONPUSH;
			PX_Object_RadioBoxClearOtherCheck(pObject);
			pcb->bCheck=PX_TRUE;
			e.Event=PX_OBJECT_EVENT_VALUECHANGED;
			e.Param_uint[0]=0;
			e.Param_uint[1]=0;
			e.Param_uint[2]=0;
			e.Param_uint[3]=0;
			PX_ObjectExecuteEvent(pObject,e);
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
				PX_ObjectExecuteEvent(pObject,e);
			}
			pcb->state=PX_OBJECT_RADIOBOX_STATE_NORMAL;
		}
	}
}

PX_OBJECT_EVENT_FUNCTION(PX_Object_RadioBoxOnMouseLButtonUp)
{
	PX_Object_RadioBox *pcb=PX_Object_GetRadioBox(pObject);

	px_float x,y;

	x=PX_Object_Event_GetCursorX(e);
	y=PX_Object_Event_GetCursorY(e);

	if (pcb)
	{
		if(PX_ObjectIsPointInRegion(pObject,(px_float)x,(px_float)y))
			if(pcb->state==PX_OBJECT_RADIOBOX_STATE_ONPUSH)
			{
				pcb->state=PX_OBJECT_RADIOBOX_STATE_ONCURSOR;
			}
	}
}

PX_OBJECT_RENDER_FUNCTION(PX_Object_RadioBoxRender)
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

PX_Object* PX_Object_RadioBoxAttachObject(px_memorypool* mp, PX_Object* pObject,px_int attachIndex, px_int x, px_int y, px_int Width, px_int Height, const char text[], PX_FontModule* fm)
{

	PX_Object_RadioBox cbx,*pdesc;
	cbx.Align = PX_ALIGN_LEFTMID;
	cbx.BackgroundColor = PX_COLOR(0, 0, 0, 0);
	cbx.bCheck = PX_FALSE;
	cbx.Border = PX_FALSE;
	cbx.BorderColor = PX_OBJECT_UI_DEFAULT_FONTCOLOR;
	cbx.CursorColor = PX_COLOR(0, 0, 0, 0);
	cbx.PushColor = PX_COLOR(0, 0, 0, 0);
	cbx.fm = fm;
	cbx.state = PX_OBJECT_RADIOBOX_STATE_NORMAL;
	PX_strset(cbx.Text, text);
	cbx.TextColor = PX_OBJECT_UI_DEFAULT_FONTCOLOR;
	
	PX_ASSERTIF(pObject == PX_NULL);
	PX_ASSERTIF(attachIndex < 0 || attachIndex >= PX_COUNTOF(pObject->pObjectDesc));
	PX_ASSERTIF(pObject->pObjectDesc[attachIndex] != PX_NULL);
	pdesc = (PX_Object_RadioBox*)PX_ObjectCreateDesc(pObject, attachIndex, PX_OBJECT_TYPE_RADIOBOX, 0, PX_Object_RadioBoxRender, 0, &cbx, sizeof(cbx));
	PX_ASSERTIF(pdesc == PX_NULL);

	PX_ObjectRegisterEventEx(pObject, PX_OBJECT_EVENT_CURSORMOVE, PX_OBJECT_TYPE_RADIOBOX, PX_Object_RadioBoxOnMouseMove, PX_NULL);
	PX_ObjectRegisterEventEx(pObject, PX_OBJECT_EVENT_CURSORDOWN, PX_OBJECT_TYPE_RADIOBOX, PX_Object_RadioBoxOnMouseLButtonDown, PX_NULL);
	PX_ObjectRegisterEventEx(pObject, PX_OBJECT_EVENT_CURSORUP, PX_OBJECT_TYPE_RADIOBOX, PX_Object_RadioBoxOnMouseLButtonUp, PX_NULL);
	return pObject;
}

PX_Object * PX_Object_RadioBoxCreate(px_memorypool *mp, PX_Object *Parent,px_int x,px_int y,px_int Width,px_int Height,const char text[],PX_FontModule *fm)
{
	PX_Object *pObject;
	pObject=PX_ObjectCreate(mp,Parent,(px_float)x,(px_float)y,0,(px_float)Width,(px_float)Height,0);
	if (pObject==PX_NULL)
	{
		return PX_NULL;
	}
	if (!PX_Object_RadioBoxAttachObject(mp,pObject,0,x,y,Width,Height,text,fm))
	{
		PX_ObjectDelete(pObject);
		return PX_NULL;
	}
	
	return pObject;
}

px_bool PX_Object_RadioBoxGetCheck(PX_Object *pObject)
{
	return PX_Object_GetRadioBox(pObject)->bCheck;
}

px_void PX_Object_RadioBoxSetBackgroundColor(PX_Object *pObject,px_color clr)
{
	PX_Object_GetRadioBox(pObject)->BackgroundColor=clr;
}

px_void PX_Object_RadioBoxSetBorderColor(PX_Object *pObject,px_color clr)
{
	PX_Object_GetRadioBox(pObject)->BorderColor=clr;
}

px_void PX_Object_RadioBoxSetPushColor(PX_Object *pObject,px_color clr)
{
	PX_Object_GetRadioBox(pObject)->PushColor=clr;
}

px_void PX_Object_RadioBoxSetCursorColor(PX_Object *pObject,px_color clr)
{
	PX_Object_GetRadioBox(pObject)->CursorColor=clr;
}

px_void PX_Object_RadioBoxSetText(PX_Object *pObject,const px_char text[])
{
	PX_strset(PX_Object_GetRadioBox(pObject)->Text,text);
}

const px_char *PX_Object_RadioBoxGetText(PX_Object *pObject)
{
	PX_Object_RadioBox *pDesc=PX_Object_GetRadioBox(pObject);
	return pDesc->Text;

}

px_void PX_Object_RadioBoxSetTextColor(PX_Object *pObject,px_color clr)
{
	PX_Object_RadioBox *pDesc=PX_Object_GetRadioBox(pObject);
	pDesc->TextColor=clr;

}

px_void PX_Object_RadioBoxSetCheck(PX_Object *pObject,px_bool check)
{
	PX_Object_GetRadioBox(pObject)->bCheck=check;
}

