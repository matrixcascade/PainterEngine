#include "SOD_Message.h"

px_void SOD_Message_BtnYesClick(PX_Object *pObject,PX_Object_Event e,px_void *user)
{
	SOD_Message *pm=(SOD_Message *)user;
	pm->ret=PX_TRUE;
	pm->mode=SOD_MESSAGE_MODE_CLOSE;
}

px_void SOD_Message_BtnNoClick(PX_Object *pObject,PX_Object_Event e,px_void *user)
{
	SOD_Message *pm=(SOD_Message *)user;
	pm->ret=PX_TRUE;
	pm->mode=SOD_MESSAGE_MODE_CLOSE;
}


px_bool SOD_MessageInitialize(PX_Runtime *runtime,SOD_Message *pm,px_int window_Width,px_int window_Height)
{
	pm->schedule=0;
	pm->runtime=runtime;
	pm->window_Width=window_Width;
	pm->window_Height=window_Height;
	if(!(pm->root=PX_ObjectCreate(&runtime->mp_ui,PX_NULL,0,0,0,0,0,0))) return PX_FALSE;
	pm->pShape=PX_NULL;
	if(!(pm->btn_Yes=PX_Object_PushButtonCreate(&runtime->mp_ui,pm->root,window_Width/2+200,window_Height/2+150,84,28,"",PX_COLOR(255,255,255,255)))) return PX_FALSE;
	PX_Object_PushButtonSetBackgroundColor(pm->btn_Yes,PX_COLOR(255,0,0,0));
	PX_Object_PushButtonSetCursorColor(pm->btn_Yes,PX_COLOR(255,32,32,32));
	PX_Object_PushButtonSetPushColor(pm->btn_Yes,PX_COLOR(255,96,96,96));
	PX_Object_PushButtonSetBorderColor(pm->btn_Yes,PX_COLOR(255,32,32,32));
	if(!(pm->btn_No=PX_Object_PushButtonCreate(&runtime->mp_ui,pm->root,window_Width/2+300,window_Height/2+150,84,28,"",PX_COLOR(255,0,0,0)))) return PX_FALSE;
	PX_Object_PushButtonSetBackgroundColor(pm->btn_No,PX_COLOR(255,0,0,0));
	PX_Object_PushButtonSetCursorColor(pm->btn_No,PX_COLOR(255,32,32,32));
	PX_Object_PushButtonSetPushColor(pm->btn_No,PX_COLOR(255,96,96,96));
	PX_Object_PushButtonSetBorderColor(pm->btn_No,PX_COLOR(255,32,32,32));
	pm->show=PX_FALSE;

	if (!PX_LoadShapeToResource(runtime,SOD_PATH_GAMEOVER,SOD_KEY_TEXT_GAMEOVER))
	{
		return PX_FALSE;
	}

	PX_ObjectRegisterEvent(pm->btn_Yes,PX_OBJECT_EVENT_CURSORCLICK,SOD_Message_BtnYesClick,pm);
	PX_ObjectRegisterEvent(pm->btn_No,PX_OBJECT_EVENT_CURSORCLICK,SOD_Message_BtnNoClick,pm);
	return PX_TRUE;
}

px_void SOD_MessagePostEvent(SOD_Message *pm,PX_Object_Event e)
{
	if (pm->show)
	{
		PX_ObjectPostEvent(pm->root,e);
	}
}

px_void SOD_MessageUpdate(SOD_Message *pm,px_dword elpased)
{
	if (pm->show)
	{
		PX_ObjectUpdate(pm->root,elpased);
		
		switch(pm->mode)
		{
		case SOD_MESSAGE_MODE_EXPAND:
			pm->schedule+=elpased;
			if (pm->schedule>SOD_MESSAGE_STAGE_2_TIME)
			{
				pm->schedule=SOD_MESSAGE_STAGE_2_TIME;
			}
			break;
		case SOD_MESSAGE_MODE_CLOSE:
		default:
			{
				pm->schedule-=elpased;
				if (pm->schedule<0)
				{
					pm->schedule=0;
					pm->show=PX_FALSE;
				}
			}
			break;

		}
		
		
	}
}



px_void SOD_MessageRender(SOD_Message *pm,px_dword elpased)
{
	if (!pm->show)
	{
	   return;
	}

	if (pm->schedule<SOD_MESSAGE_STAGE_1_TIME)
	{
		px_int width=pm->window_Width*pm->schedule*pm->schedule/(SOD_MESSAGE_STAGE_1_TIME*SOD_MESSAGE_STAGE_1_TIME);
		PX_GeoDrawRect(&pm->runtime->RenderSurface,(pm->window_Width-width)/2,(pm->window_Height-SOD_MESSAGE_STAGE_1_HEIGHT)/2,(pm->window_Width+width)/2,(pm->window_Height+SOD_MESSAGE_STAGE_1_HEIGHT)/2,PX_COLOR(255,0,0,0));
	}
	else if(pm->schedule<SOD_MESSAGE_STAGE_2_TIME)
	{
		px_int height=SOD_MESSAGE_STAGE_2_HEIGHT*pm->schedule*pm->schedule/(SOD_MESSAGE_STAGE_2_TIME*SOD_MESSAGE_STAGE_2_TIME);
		PX_GeoDrawRect(&pm->runtime->RenderSurface,0,(pm->window_Height-height)/2,pm->window_Width-1,(pm->window_Height+height)/2,PX_COLOR(255,0,0,0));
	}
	else
	{
		PX_GeoDrawRect(&pm->runtime->RenderSurface,0,(pm->window_Height-SOD_MESSAGE_STAGE_2_HEIGHT)/2,pm->window_Width-1,(pm->window_Height+SOD_MESSAGE_STAGE_2_HEIGHT)/2,PX_COLOR(255,0,0,0));
		PX_GeoDrawRect(&pm->runtime->RenderSurface,0,(pm->window_Height-SOD_MESSAGE_STAGE_2_HEIGHT)/2-30,pm->window_Width-1,(pm->window_Height-SOD_MESSAGE_STAGE_2_HEIGHT)/2-10,PX_COLOR(255,0,0,0));
		PX_GeoDrawRect(&pm->runtime->RenderSurface,0,(pm->window_Height+SOD_MESSAGE_STAGE_2_HEIGHT)/2+10,pm->window_Width-1,(pm->window_Height+SOD_MESSAGE_STAGE_2_HEIGHT)/2+30,PX_COLOR(255,0,0,0));
		if (pm->pShape)
		{
			PX_ShapeRender(&pm->runtime->RenderSurface,pm->pShape,pm->window_Width/2,pm->window_Height/2,PX_TEXTURERENDER_REFPOINT_CENTER,PX_COLOR(255,255,255,255));
		}
		PX_ObjectRender(&pm->runtime->RenderSurface,pm->root,elpased);
	}

	
}

px_void SOD_MessageAlert(SOD_Message *pm,px_char tex_key[])
{
	PX_Resource *pres;
	pm->schedule=0;
	pm->show=PX_TRUE;
	pm->mode=SOD_MESSAGE_MODE_EXPAND;
	pres=PX_ResourceLibraryGet(&pm->runtime->ResourceLibrary,tex_key);
	pm->pShape=PX_NULL;
	if (pres->Type==PX_RESOURCE_TYPE_SHAPE)
	{
		pm->pShape=&pres->shape;
	}
}
px_void SOD_MessageAlert_OK(SOD_Message *pm,px_char tex_key[])
{
	SOD_MessageAlert(pm,tex_key);
	PX_ObjectSetVisible(pm->btn_No,PX_FALSE);
	PX_Object_PushButtonSetText(pm->btn_Yes,SOD_TEXT_OK);
}

px_void SOD_MessageAlert_YESNO(SOD_Message *pm,px_char tex_key[])
{
	SOD_MessageAlert(pm,tex_key);
	PX_ObjectSetVisible(pm->btn_No,PX_TRUE);
	PX_Object_PushButtonSetText(pm->btn_Yes,SOD_TEXT_OK);
	PX_Object_PushButtonSetText(pm->btn_No,SOD_TEXT_OK);
}



