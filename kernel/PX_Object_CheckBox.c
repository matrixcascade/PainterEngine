#include "PX_Object_CheckBox.h"

PX_Object_CheckBox * PX_Object_GetCheckBox(PX_Object *pObject)
{
	PX_Object_CheckBox *pcb=(PX_Object_CheckBox *)PX_ObjectGetDescByType(pObject,PX_OBJECT_TYPE_CHECKBOX);
	PX_ASSERTIF(!pcb);
	return pcb;
}

px_void PX_Object_CheckBoxOnMouseMove(PX_Object *pObject,PX_Object_Event e,px_void *user_ptr)
{
	PX_Object_CheckBox *pcb=PX_Object_GetCheckBox(pObject);

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
					PX_Object_Event e = {0};
					e.Event=PX_OBJECT_EVENT_CURSOROVER;
					e.Param_ptr[0]=PX_NULL;
					PX_ObjectExecuteEvent(pObject,e);
				}
				pcb->state=PX_OBJECT_CHECKBOX_STATE_ONCURSOR;
			}
		}
		else
		{
			if (pcb->state!=PX_OBJECT_BUTTON_STATE_NORMAL)
			{
				PX_Object_Event e = {0};
				e.Event=PX_OBJECT_EVENT_CURSOROUT;
				PX_ObjectExecuteEvent(pObject,e);
			}
			pcb->state=PX_OBJECT_CHECKBOX_STATE_NORMAL;
		}
	}
}

PX_OBJECT_EVENT_FUNCTION(PX_Object_CheckBoxOnMouseLButtonDown)
{
	PX_Object_CheckBox *pcb=PX_Object_GetCheckBox(pObject);
	px_float x=(PX_Object_Event_GetCursorX(e));
	px_float y=(PX_Object_Event_GetCursorY(e));
	PX_ASSERTIF(!pcb);
	if (pcb)
	{
		if(PX_ObjectIsPointInRegion(pObject,(px_float)x,(px_float)y))
			pcb->state=PX_OBJECT_CHECKBOX_STATE_ONPUSH;
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
			pcb->state=PX_OBJECT_CHECKBOX_STATE_NORMAL;
		}
	}
}

PX_OBJECT_EVENT_FUNCTION(PX_Object_CheckBoxOnMouseLButtonUp)
{
	PX_Object_CheckBox *pcb=PX_Object_GetCheckBox(pObject);
	px_float x,y;
	PX_ASSERTIF(!pcb);
	x=PX_Object_Event_GetCursorX(e);
	y=PX_Object_Event_GetCursorY(e);

	if (pcb)
	{
		if(PX_ObjectIsPointInRegion(pObject,(px_float)x,(px_float)y))
			if(pcb->state==PX_OBJECT_CHECKBOX_STATE_ONPUSH)
			{
				PX_Object_Event e = {0};
				pcb->state=PX_OBJECT_CHECKBOX_STATE_ONCURSOR;
				pcb->bCheck=!pcb->bCheck;

				e.Event=PX_OBJECT_EVENT_VALUECHANGED;
				PX_ObjectExecuteEvent(pObject,e);
			}
	}
}

PX_OBJECT_RENDER_FUNCTION(PX_Object_CheckBoxRender)
{
	PX_Object_CheckBox *pcb=PX_ObjectGetDesc(PX_Object_CheckBox,pObject);
	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;
	PX_ASSERTIF(!pcb);
	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;

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

	PX_FontModuleDrawText(psurface,pcb->fm,(px_int)objx+18,(px_int)(objy+objHeight/2),PX_ALIGN_LEFTMID,pcb->Text,pcb->TextColor);

	//draw CheckState
	PX_GeoDrawBorder(psurface,(px_int)objx,(px_int)(objy+objHeight/2-7),(px_int)objx+14,(px_int)(objy+objHeight/2+7),1,pcb->BorderColor);
	if (pcb->bCheck)
	{
		PX_GeoDrawRect(psurface,(px_int)objx+2,(px_int)(objy+objHeight/2-5),(px_int)objx+12,(px_int)(objy+objHeight/2+5),pcb->BorderColor);
	}

}
PX_Object* PX_Object_CheckBoxAttachObject(px_memorypool* mp, PX_Object* pObject, px_int attachIndex, px_int x, px_int y, px_int Width, px_int Height, const char text[], PX_FontModule* fm)
{
	PX_Object_CheckBox *pcbx;
	
	PX_ASSERTIF(pObject == PX_NULL);
	PX_ASSERTIF(attachIndex < 0 || attachIndex >= PX_COUNTOF(pObject->pObjectDesc));
	PX_ASSERTIF(pObject->pObjectDesc[attachIndex] != PX_NULL);
	pcbx = (PX_Object_CheckBox*)PX_ObjectCreateDesc(pObject, attachIndex, PX_OBJECT_TYPE_CHECKBOX, 0, PX_Object_CheckBoxRender, 0, 0, sizeof(PX_Object_CheckBox));
	PX_ASSERTIF(pcbx == PX_NULL);

	pcbx=PX_ObjectGetDescIndex(PX_Object_CheckBox,pObject,attachIndex);
	pcbx->Align = PX_ALIGN_LEFTMID;
	pcbx->BackgroundColor = PX_COLOR(0, 0, 0, 0);
	pcbx->bCheck = PX_FALSE;
	pcbx->Border = PX_FALSE;
	pcbx->BorderColor = PX_OBJECT_UI_DEFAULT_FONTCOLOR;
	pcbx->CursorColor = PX_OBJECT_UI_DEFAULT_CURSORCOLOR;
	pcbx->PushColor = PX_OBJECT_UI_DEFAULT_PUSHCOLOR;
	pcbx->fm = fm;
	pcbx->state = PX_OBJECT_CHECKBOX_STATE_NORMAL;
	PX_strset(pcbx->Text, text);
	pcbx->TextColor = PX_OBJECT_UI_DEFAULT_FONTCOLOR;
	PX_ObjectRegisterEventEx(pObject, PX_OBJECT_EVENT_CURSORMOVE, PX_OBJECT_TYPE_CHECKBOX, PX_Object_CheckBoxOnMouseMove, PX_NULL);
	PX_ObjectRegisterEventEx(pObject, PX_OBJECT_EVENT_CURSORDOWN, PX_OBJECT_TYPE_CHECKBOX, PX_Object_CheckBoxOnMouseLButtonDown, PX_NULL);
	PX_ObjectRegisterEventEx(pObject, PX_OBJECT_EVENT_CURSORUP, PX_OBJECT_TYPE_CHECKBOX, PX_Object_CheckBoxOnMouseLButtonUp, PX_NULL);
	return pObject;
}

PX_Object * PX_Object_CheckBoxCreate(px_memorypool *mp, PX_Object *Parent,px_int x,px_int y,px_int Width,px_int Height,const char text[],PX_FontModule *fm)
{
	PX_Object *pObject;
	pObject=PX_ObjectCreate(mp,Parent,(px_float)x,(px_float)y,0,(px_float)Width,(px_float)Height,0);
	if (!pObject)
	{
		return PX_NULL;
	}
	if(!PX_Object_CheckBoxAttachObject(mp,pObject,0,x,y,Width,Height,text,fm))
	{
		PX_ObjectDelete(pObject);
		return PX_NULL;
	}
	
	return pObject;
}

px_bool PX_Object_CheckBoxGetCheck(PX_Object *pObject)
{
	return PX_Object_GetCheckBox(pObject)->bCheck;
}

px_void PX_Object_CheckBoxSetBackgroundColor(PX_Object *pObject,px_color clr)
{
	PX_Object_GetCheckBox(pObject)->BackgroundColor=clr;
}

px_void PX_Object_CheckBoxSetBorderColor(PX_Object *pObject,px_color clr)
{
	PX_Object_GetCheckBox(pObject)->BorderColor=clr;
}

px_void PX_Object_CheckBoxSetPushColor(PX_Object *pObject,px_color clr)
{
	PX_Object_GetCheckBox(pObject)->PushColor=clr;
}

px_void PX_Object_CheckBoxSetCursorColor(PX_Object *pObject,px_color clr)
{
	PX_Object_GetCheckBox(pObject)->CursorColor=clr;
}

px_void PX_Object_CheckBoxSetText(PX_Object *pObject,const px_char text[])
{
	PX_strset(PX_Object_GetCheckBox(pObject)->Text,text);
}

px_char *PX_Object_CheckBoxGetText(PX_Object *pObject)
{
	PX_Object_CheckBox *pCheckBox=PX_Object_GetCheckBox(pObject);
	PX_ASSERTIF(!pCheckBox);
	return pCheckBox->Text;
}

px_void PX_Object_CheckBoxSetTextColor(PX_Object *pObject,px_color clr)
{
	PX_Object_GetCheckBox(pObject)->TextColor=clr;
}

px_void PX_Object_CheckBoxSetCheck(PX_Object *pObject,px_bool check)
{
	PX_Object_GetCheckBox(pObject)->bCheck=check;
}
