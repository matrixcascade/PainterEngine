#include "PX_Object_RadioBox.h"
//////////////////////////////////////////////////////////////////////////

PX_Object_RadioBox * PX_Object_GetRadioBox(PX_Object *pObject)
{
	PX_Object_RadioBox *pcb=(PX_Object_RadioBox *)pObject->pObjectDesc;
	if (pObject->Type==PX_OBJECT_TYPE_RADIOBOX)
	{
		return pcb;
	}
	return PX_NULL;
}


px_void PX_Object_RadioBoxOnMouseMove(PX_Object *pObject,PX_Object_Event e,px_void *user_ptr)
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
px_void PX_Object_RadioBoxOnMouseLButtonDown(PX_Object *pObject,PX_Object_Event e,px_void *user_ptr)
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

px_void PX_Object_RadioBoxOnMouseLButtonUp(PX_Object *pObject,PX_Object_Event e,px_void *user_ptr)
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
	if (pDesc)
	{
		return pDesc->Text;
	}
	PX_ASSERT();
	return PX_NULL;
}

px_void PX_Object_RadioBoxSetTextColor(PX_Object *pObject,px_color clr)
{
	PX_Object_RadioBox *pDesc=PX_Object_GetRadioBox(pObject);
	if (pDesc)
	{
		pDesc->TextColor=clr;
	}
	else
	PX_ASSERT();
}

px_void PX_Object_RadioBoxSetCheck(PX_Object *pObject,px_bool check)
{
	PX_Object_GetRadioBox(pObject)->bCheck=check;
}


//////////////////////////////////////////////////////////////////////////
//radiobox
//////////////////////////////////////////////////////////////////////////
PX_Object* PX_Designer_RadioBoxCreate(px_memorypool* mp, PX_Object* pparent, px_float x, px_float y, px_float width, px_float height, px_void* ptr)
{
	PX_FontModule* fm = (PX_FontModule*)ptr;
	return PX_Object_RadioBoxCreate(mp, pparent, (px_int)x, (px_int)y, 108, 28, "RadioButton", fm);
}

px_void PX_Designer_RadioBoxSetText(PX_Object* pobject, const px_char text[])
{
	PX_Object_RadioBoxSetText(pobject, text);
}

px_bool PX_Designer_RadioBoxGetText(PX_Object* pobject, px_string* str)
{
	return PX_StringSet(str, PX_Object_RadioBoxGetText(pobject));
}

px_void PX_Designer_RadioBoxSetCheck(PX_Object* pobject, px_bool b)
{
	PX_Object_RadioBoxSetCheck(pobject, b);
}

px_bool PX_Designer_RadioBoxGetCheck(PX_Object* pobject)
{
	return PX_Object_RadioBoxGetCheck(pobject);
}

PX_Designer_ObjectDesc PX_Object_RadioBoxDesignerInstall()
{
	PX_Designer_ObjectDesc sradiobutton;
	px_int i = 0;
	PX_memset(&sradiobutton, 0, sizeof(sradiobutton));
	PX_strcat(sradiobutton.Name, "radiobutton");

	sradiobutton.createfunc = PX_Designer_RadioBoxCreate;
	sradiobutton.type = PX_DESIGNER_OBJECT_TYPE_UI;

	PX_strcat(sradiobutton.properties[i].Name, "id");
	sradiobutton.properties[i].getstring = PX_Designer_GetID;
	sradiobutton.properties[i].setstring = PX_Designer_SetID;
	i++;

	PX_strcat(sradiobutton.properties[i].Name, "x");
	sradiobutton.properties[i].getfloat = PX_Designer_GetX;
	sradiobutton.properties[i].setfloat = PX_Designer_SetX;
	i++;

	PX_strcat(sradiobutton.properties[i].Name, "y");
	sradiobutton.properties[i].getfloat = PX_Designer_GetY;
	sradiobutton.properties[i].setfloat = PX_Designer_SetY;
	i++;

	PX_strcat(sradiobutton.properties[i].Name, "width");
	sradiobutton.properties[i].getfloat = PX_Designer_GetWidth;
	sradiobutton.properties[i].setfloat = PX_Designer_SetWidth;
	i++;

	PX_strcat(sradiobutton.properties[i].Name, "height");
	sradiobutton.properties[i].getfloat = PX_Designer_GetHeight;
	sradiobutton.properties[i].setfloat = PX_Designer_SetHeight;
	i++;

	PX_strcat(sradiobutton.properties[i].Name, "text");
	sradiobutton.properties[i].setstring = PX_Designer_RadioBoxSetText;
	sradiobutton.properties[i].getstring = PX_Designer_RadioBoxGetText;
	i++;

	PX_strcat(sradiobutton.properties[i].Name, "check");
	sradiobutton.properties[i].setbool = PX_Designer_RadioBoxSetCheck;
	sradiobutton.properties[i].getbool = PX_Designer_RadioBoxGetCheck;
	i++;
	return sradiobutton;
}

