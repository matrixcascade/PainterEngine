#include "PainterEngine_LiveEditorModules_Display.h"

px_void PX_LiveEditorModule_DisplayUpdate(PX_Object *pObject,px_dword elpased)
{
	PX_LiveEditorModule_Display *pdisplay=(PX_LiveEditorModule_Display *)pObject->pObject;

}


px_void PX_LiveEditorModule_DisplayRender(px_surface *psurface,PX_Object *pObject,px_dword elpased)
{
	PX_LiveEditorModule_Display *pdisplay=(PX_LiveEditorModule_Display *)pObject->pObject;
	PX_LiveFrameworkRenderRefer(psurface,pdisplay->pLiveFramework,PX_ALIGN_LEFTTOP,elpased);

	pdisplay->elpased+=elpased;
	pdisplay->_tRenderFrames++;

	if (pdisplay->elpased>1000)
	{
		pdisplay->fps=pdisplay->_tRenderFrames/(pdisplay->elpased/1000.0f);
		pdisplay->elpased=0;
		pdisplay->_tRenderFrames=0;
	}

	if (pdisplay->showFPS)
	{
		px_char content[32];
		PX_sprintf1(content,sizeof(content),"FPS:%1.2",PX_STRINGFORMAT_FLOAT(pdisplay->fps));
		if (pdisplay->fps<20)
		{
			PX_FontModuleDrawText(psurface,pdisplay->fm,psurface->width-5,psurface->height-5,PX_ALIGN_RIGHTBOTTOM,content,PX_COLOR(255,255,0,0));
		}
		else if(pdisplay->fps<36)
		{
			PX_FontModuleDrawText(psurface,pdisplay->fm,psurface->width-5,psurface->height-5,PX_ALIGN_RIGHTBOTTOM,content,PX_COLOR(255,255,255,0));
		}
		else
		{
			PX_FontModuleDrawText(psurface,pdisplay->fm,psurface->width-5,psurface->height-5,PX_ALIGN_RIGHTBOTTOM,content,PX_COLOR(255,0,255,0));
		}
	}

	if (pdisplay->showHelpline)
	{
		//horizontal
		PX_GeoDrawLine(psurface,0,(px_int)pdisplay->helpPoint.y,(px_int)psurface->width-1,(px_int)pdisplay->helpPoint.y,1,PX_COLOR(128,255,0,0));
		//vertical
		PX_GeoDrawLine(psurface,(px_int)pdisplay->helpPoint.x,0,(px_int)pdisplay->helpPoint.x,(px_int)psurface->height-1,1,PX_COLOR(128,255,0,0));
	}
}

px_void PX_LiveEditorModule_DisplayOnCursorClick(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_LiveEditorModule_Display *pdisplay=(PX_LiveEditorModule_Display *)pObject->pObject;
	if (pdisplay->showHelpline)
	{
		pdisplay->helpPoint.x=PX_Object_Event_GetCursorX(e);
		pdisplay->helpPoint.y=PX_Object_Event_GetCursorY(e);
	}
}


PX_Object* PX_LiveEditorModule_DisplayInstall(PX_Object *pparent,PX_Runtime *pruntime,PX_FontModule *fm,PX_LiveFramework *pLiveFramework,PX_Json *pLanguageJson)
{
	PX_Object *pObject;
	PX_LiveEditorModule_Display display,*pdisplay;
	PX_memset(&display,0,sizeof(display));
	pObject=PX_ObjectCreateEx(&pruntime->mp_ui,pparent,0,0,0,0,0,0,0,PX_LiveEditorModule_DisplayUpdate,PX_LiveEditorModule_DisplayRender,PX_NULL,&display,sizeof(display));
	pdisplay=(PX_LiveEditorModule_Display *)pObject->pObject;
	pdisplay->pLiveFramework=pLiveFramework;
	pdisplay->pruntime=pruntime;
	pdisplay->fm=fm;
	pdisplay->pLanguageJson=pLanguageJson;
	pdisplay->showFPS=PX_TRUE;
	pdisplay->showHelpline=PX_FALSE;

	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_CURSORCLICK,PX_LiveEditorModule_DisplayOnCursorClick,PX_NULL);
	return pObject;
}

px_void PX_LiveEditorModule_DisplayUninstall(PX_Object *pObject)
{
	PX_ObjectDelete(pObject);
}

px_void PX_LiveEditorModule_DisplayShowFPS(PX_Object *pObject)
{
	PX_LiveEditorModule_Display *pdisplay=(PX_LiveEditorModule_Display *)pObject->pObject;
	pdisplay->showFPS=!pdisplay->showFPS;
}

px_void PX_LiveEditorModule_DisplayShowHelperLine(PX_Object *pObject)
{
	PX_LiveEditorModule_Display *pdisplay=(PX_LiveEditorModule_Display *)pObject->pObject;
	pdisplay->showHelpline=!pdisplay->showHelpline;
}

px_void PX_LiveEditorModule_DisplayResetPosition(PX_Object *pObject)
{
	PX_LiveEditorModule_Display *pdisplay=(PX_LiveEditorModule_Display *)pObject->pObject;
	pdisplay->pLiveFramework->refer_x=(px_float)(pdisplay->pruntime->surface_width/2-pdisplay->pLiveFramework->width/2);
	pdisplay->pLiveFramework->refer_y=(px_float)(pdisplay->pruntime->surface_height/2-pdisplay->pLiveFramework->height/2);
}

px_void PX_LiveEditorModule_DisplayEnable(PX_Object *pObject)
{
	pObject->Enabled=PX_TRUE;
	pObject->Visible=PX_TRUE;
}

px_void PX_LiveEditorModule_DisplayDisable(PX_Object *pObject)
{
	pObject->Enabled=PX_FALSE;
	pObject->Visible=PX_FALSE;
}

