#include "PX_Object_MessageBox.h"

PX_Object_MessageBox *PX_Object_GetMessageBox(PX_Object *pObject)
{
	if (pObject->Type==PX_OBJECT_TYPE_MESSAGEBOX)
	{
		return (PX_Object_MessageBox *)pObject->pObject;
	}
	return PX_NULL;
}

static px_void PX_Object_MessageBox_BtnYesClick(PX_Object *pObject,PX_Object_Event e,px_void *user)
{
	PX_Object_MessageBox *pm=PX_Object_GetMessageBox((PX_Object *)user);
	PX_Object_MessageBoxClose((PX_Object *)user);
	if (pm->function_yes)
	{
		pm->function_yes((PX_Object *)user,PX_OBJECT_BUILD_EVENT(PX_OBJECT_EVENT_EXECUTE),pm->function_yes_ptr);
	}

}

static px_void PX_Object_MessageBox_BtnNoClick(PX_Object *pObject,PX_Object_Event e,px_void *user)
{
	PX_Object_MessageBox *pm=PX_Object_GetMessageBox((PX_Object *)user);
	PX_Object_MessageBoxClose((PX_Object *)user);
	if (pm->function_no)
	{
		pm->function_no((PX_Object *)user,PX_OBJECT_BUILD_EVENT(PX_OBJECT_EVENT_EXECUTE),pm->function_yes_ptr);
	}
}

static px_void PX_Object_MessageBox_EditOnEnter(PX_Object *pObject,PX_Object_Event e,px_void *user)
{
	if (e.Event==PX_OBJECT_EVENT_KEYDOWN)
	{
		if (PX_Object_Event_GetKeyDown(e)=='\r')
		{
			PX_Object_MessageBox *pm=PX_Object_GetMessageBox((PX_Object *)user);
			PX_Object_MessageBoxClose((PX_Object *)user);
			if (pm->function_yes)
			{
				pm->function_yes((PX_Object *)user,PX_OBJECT_BUILD_EVENT(PX_OBJECT_EVENT_EXECUTE),pm->function_yes_ptr);
			}
			return;
		}
	}
}

px_void PX_Object_MessageBoxClose(PX_Object *pObject)
{
	PX_Object_MessageBox *pMessageBox=PX_Object_GetMessageBox(pObject);
	if (pMessageBox)
	{
		PX_ObjectReleaseFocus(pObject);
		pMessageBox->mode=PX_OBJECT_MESSAGEBOX_MODE_CLOSE;
		pMessageBox->btn_Ok->Visible=PX_FALSE;
		pMessageBox->btn_Cancel->Visible=PX_FALSE;
		pMessageBox->edit_inputbox->Visible=PX_FALSE;
	}
	
}

static px_void PX_Object_MessageBoxRender(px_surface *pSurface,PX_Object *pObject,px_dword elapsed)
{

	px_color backGroundColor,frontColor;
	PX_Object_MessageBox *pm=PX_Object_GetMessageBox(pObject);
	if (elapsed>2000)
	{
		return;
	}

	if (pm->fillbackgroundcolor._argb.a)
	{
		PX_SurfaceClear(pSurface,0,0,pSurface->width-1,pSurface->height-1,pm->fillbackgroundcolor);
	}


	pm->btn_Ok->x=pSurface->width/2+200.0f;
	pm->btn_Ok->y=pSurface->height/2+150.0f;

	pm->btn_Cancel->x=pSurface->width/2+300.0f;
	pm->btn_Cancel->y=pSurface->height/2+150.0f;

	pm->edit_inputbox->x=pSurface->width/2-100.0f;
	pm->edit_inputbox->y=pSurface->height/2-12.0f;

	if (pm->edit_inputbox->Visible)
	{
		PX_Object_EditSetFocus(pm->edit_inputbox,PX_TRUE);
	}

	switch(pm->mode)
	{
	case PX_OBJECT_MESSAGEBOX_MODE_EXPAND:
		pm->schedule+=elapsed;
		if (pm->schedule>PX_OBJECT_MESSAGEBOX_DEFAULT_STAGE_2_TIME)
		{
			pm->schedule=PX_OBJECT_MESSAGEBOX_DEFAULT_STAGE_2_TIME;
		}
		break;
	case PX_OBJECT_MESSAGEBOX_MODE_CLOSE:
	default:
		{
			pm->schedule-=elapsed;
			if (pm->schedule<0)
			{
				pm->schedule=0;
				pObject->Visible=PX_FALSE;
			}
		}
		break;

	}

	switch(pm->colormod)
	{
	case PX_MESSAGEBOX_COLORMOD_LIGHT:
		{
			backGroundColor=PX_COLOR(255,0,0,0);
			frontColor=PX_COLOR(255,255,255,255);
		}
		break;
	case PX_MESSAGEBOX_COLORMOD_NIGHT:
	default:
		{
			backGroundColor=PX_COLOR(255,255,255,255);
			frontColor=PX_COLOR(255,0,0,0);
		}
		break;
	}



	if (pm->schedule<PX_OBJECT_MESSAGEBOX_DEFAULT_STAGE_1_TIME)
	{
		px_int width=pSurface->width*pm->schedule*pm->schedule/(PX_OBJECT_MESSAGEBOX_DEFAULT_STAGE_1_TIME*PX_OBJECT_MESSAGEBOX_DEFAULT_STAGE_1_TIME);
		PX_GeoDrawRect(pSurface,(pSurface->width-width)/2,(pSurface->height-pm->PX_MESSAGEBOX_STAGE_1_HEIGHT)/2,(pSurface->width+width)/2,(pSurface->height+pm->PX_MESSAGEBOX_STAGE_1_HEIGHT)/2,backGroundColor);
	}
	else if(pm->schedule<PX_OBJECT_MESSAGEBOX_DEFAULT_STAGE_2_TIME)
	{
		px_int height=PX_OBJECT_MESSAGEBOX_DEFAULT_STAGE_2_HEIGHT*pm->schedule*pm->schedule/(PX_OBJECT_MESSAGEBOX_DEFAULT_STAGE_2_TIME*PX_OBJECT_MESSAGEBOX_DEFAULT_STAGE_2_TIME);
		PX_GeoDrawRect(pSurface,0,(pSurface->height-height)/2,pSurface->width-1,(pSurface->height+height)/2,backGroundColor);
	}
	else
	{
		PX_GeoDrawRect(pSurface,0,(pSurface->height-pm->PX_MESSAGEBOX_STAGE_2_HEIGHT)/2,pSurface->width-1,(pSurface->height+pm->PX_MESSAGEBOX_STAGE_2_HEIGHT)/2,backGroundColor);
		PX_GeoDrawRect(pSurface,0,(pSurface->height-pm->PX_MESSAGEBOX_STAGE_2_HEIGHT)/2-30,pSurface->width-1,(pSurface->height-pm->PX_MESSAGEBOX_STAGE_2_HEIGHT)/2-10,backGroundColor);
		PX_GeoDrawRect(pSurface,0,(pSurface->height+pm->PX_MESSAGEBOX_STAGE_2_HEIGHT)/2+10,pSurface->width-1,(pSurface->height+pm->PX_MESSAGEBOX_STAGE_2_HEIGHT)/2+30,backGroundColor);

		if (pm->edit_inputbox->Visible)
		{
			//inputbox mode
			PX_FontModuleDrawText(pSurface,pm->fontmodule,pSurface->width/2-108,pSurface->height/2,PX_ALIGN_RIGHTMID,pm->Message,frontColor);
		}
		else
		{
			PX_FontModuleDrawText(pSurface,pm->fontmodule,pSurface->width/2,pSurface->height/2,PX_ALIGN_CENTER,pm->Message,frontColor);
		}
	}


}

px_void PX_Object_MessageBoxAlertOk(PX_Object *pObject,const px_char *message,PX_Object_MessageBoxCallBack func_callback,px_void *ptr)
{
	PX_Object_MessageBox *pm=PX_Object_GetMessageBox(pObject);
	pm->function_yes=func_callback;
	pm->function_yes_ptr=ptr;

	pm->schedule=0;
	pObject->Visible=PX_TRUE;
	PX_strcpy(pm->Message,message,sizeof(pm->Message));
	pm->mode=PX_OBJECT_MESSAGEBOX_MODE_EXPAND;
	pm->btn_Cancel->Visible=PX_FALSE;
	pm->btn_Ok->Visible=PX_TRUE;

	pm->edit_inputbox->Visible=PX_FALSE;
	PX_ObjectSetFocus(pObject);
}

px_void PX_Object_MessageBoxAlert(PX_Object *pObject,const px_char *message)
{
	PX_Object_MessageBox *pm=PX_Object_GetMessageBox(pObject);
	if (!pObject->Visible)
	{
		pm->schedule=0;
		pObject->Visible=PX_TRUE;
	}

	PX_strcpy(pm->Message,message,sizeof(pm->Message));
	pm->mode=PX_OBJECT_MESSAGEBOX_MODE_EXPAND;
	pm->btn_Cancel->Visible=PX_FALSE;
	pm->btn_Ok->Visible=PX_FALSE;

	pm->function_yes=PX_NULL;
	pm->function_no_ptr=PX_NULL;

	pm->edit_inputbox->Visible=PX_FALSE;
	PX_ObjectSetFocus(pObject);
}



px_void PX_Object_MessageBoxAlertYesNo(PX_Object *pObject,const char *Message,PX_Object_MessageBoxCallBack func_yescallback,px_void *yesptr,PX_Object_MessageBoxCallBack func_nocallback,px_void *noptr)
{
	PX_Object_MessageBox *pm=PX_Object_GetMessageBox(pObject);
	pm->function_yes=func_yescallback;
	pm->function_no=func_nocallback;
	pm->function_yes_ptr=yesptr;
	pm->function_no_ptr=noptr;

	pm->schedule=0;
	pObject->Visible=PX_TRUE;
	PX_strcpy(pm->Message,Message,sizeof(pm->Message));
	pm->mode=PX_OBJECT_MESSAGEBOX_MODE_EXPAND;
	pm->btn_Cancel->Visible=PX_TRUE;
	pm->btn_Ok->Visible=PX_TRUE;
	pm->edit_inputbox->Visible=PX_FALSE;
	PX_ObjectSetFocus(pObject);
}

px_void PX_Object_MessageBoxInputBox(PX_Object *pObject,const char *Message,PX_Object_MessageBoxCallBack func_yescallback,px_void *yesptr,PX_Object_MessageBoxCallBack func_cancelcallback,px_void *cancelptr)
{
	PX_Object_MessageBox *pm=PX_Object_GetMessageBox(pObject);
	pm->function_yes=func_yescallback;
	pm->function_no=func_cancelcallback;
	pm->function_yes_ptr=yesptr;
	pm->function_no_ptr=cancelptr;

	pm->schedule=0;
	pObject->Visible=PX_TRUE;
	PX_strcpy(pm->Message,Message,sizeof(pm->Message));
	pm->mode=PX_OBJECT_MESSAGEBOX_MODE_EXPAND;
	pm->btn_Cancel->Visible=PX_TRUE;
	pm->btn_Ok->Visible=PX_TRUE;
	pm->edit_inputbox->Visible=PX_TRUE;

	PX_Object_EditSetText(pm->edit_inputbox,"");
	PX_ObjectSetFocus(pObject);
}

px_char * PX_Object_MessageBoxGetInput(PX_Object *pObject)
{
	PX_Object_MessageBox *pm=PX_Object_GetMessageBox(pObject);
	return PX_Object_EditGetText(pm->edit_inputbox);
}

px_void PX_Object_MessageBoxFree(PX_Object *pObject)
{
	
}

PX_Object * PX_Object_MessageBoxCreate(px_memorypool *mp,PX_Object *parent,PX_FontModule *fontmodule)
{
	PX_Object_MessageBox msgbox;
	PX_Object_MessageBox *pm=&msgbox;
	PX_Object *pObject;
	PX_memset(pm,0,sizeof(PX_Object_MessageBox));

	pObject=PX_ObjectCreateEx(mp,parent,0,0,0,0,0,0,PX_OBJECT_TYPE_MESSAGEBOX,PX_NULL,PX_Object_MessageBoxRender,PX_Object_MessageBoxFree,pm,sizeof(PX_Object_MessageBox));
	pObject->Visible=PX_FALSE;
	pm=PX_Object_GetMessageBox(pObject);

	pm->schedule=0;
	pm->function_no=PX_NULL;
	pm->function_yes=PX_NULL;

	if(!(pm->btn_Ok=PX_Object_PushButtonCreate(mp,pObject,0,0,84,28,"OK",PX_NULL))) return PX_FALSE;

	if(!(pm->btn_Cancel=PX_Object_PushButtonCreate(mp,pObject,0,0,84,28,"Cancel",PX_NULL))) return PX_FALSE;


	if(!(pm->edit_inputbox=PX_Object_EditCreate(mp,pObject,0,0,256,32,fontmodule))) return PX_FALSE;

	PX_ObjectSetVisible(pm->edit_inputbox,PX_FALSE);

	pm->fontmodule=fontmodule;

	pm->PX_MESSAGEBOX_STAGE_1_HEIGHT=PX_OBJECT_MESSAGEBOX_DEFAULT_STAGE_1_HEIGHT;
	pm->PX_MESSAGEBOX_STAGE_2_HEIGHT=PX_OBJECT_MESSAGEBOX_DEFAULT_STAGE_2_HEIGHT;
	pm->fillbackgroundcolor=PX_OBJECT_UI_DEFAULT_BACKGROUNDCOLOR;
	PX_ObjectRegisterEvent(pm->edit_inputbox,PX_OBJECT_EVENT_KEYDOWN,PX_Object_MessageBox_EditOnEnter,pObject);
	PX_ObjectRegisterEvent(pm->btn_Ok,PX_OBJECT_EVENT_EXECUTE,PX_Object_MessageBox_BtnYesClick,pObject);
	PX_ObjectRegisterEvent(pm->btn_Cancel,PX_OBJECT_EVENT_EXECUTE,PX_Object_MessageBox_BtnNoClick,pObject);


	return pObject;
}

