#include "PX_Object.h"


PX_Object  * PX_ObjectGetChild( PX_Object *Object,px_int Index )
{
	PX_Object *pObject;
	if (!Object)
	{
		return PX_NULL;
	}
	pObject=Object->pChilds;
	while (Index>0&&pObject)
	{
		pObject=pObject->pNextBrother;
		Index--;
	}
	return pObject;

}

px_void PX_ObjectSetFocusEx(PX_Object *Object)
{

	while(Object)
	{
		Object->OnFocus=PX_TRUE;
		Object=Object->pParent;
	}
}


px_void PX_ObjectSetFocus(PX_Object *Object)
{
	PX_Object *pRoot=Object;
	while(pRoot->pParent)pRoot=pRoot->pParent;
	if (pRoot->OnFocus)
	{
		return;
	}
	else
	{
		PX_ObjectSetFocusEx(Object);
	}
}

static px_bool PX_ObjectChildHasFocus(PX_Object *Object)
{
	if (Object==PX_NULL)
	{
		return PX_FALSE;
	}

	if (Object->OnFocus)
	{
		return PX_TRUE;
	}

	if(PX_ObjectChildHasFocus(Object->pNextBrother))
	{
		return PX_TRUE;
	}

	if(PX_ObjectChildHasFocus(Object->pChilds))
	{
		return PX_TRUE;
	}

	return PX_FALSE;

}

px_void PX_ObjectClearFocus(PX_Object *Object)
{
	if (!Object->OnFocus)
	{
		return;
	}

	if (!PX_ObjectChildHasFocus(Object->pChilds))
	{
		while(Object)
		{
			Object->OnFocus=PX_FALSE;
			Object=Object->pParent;
		}
	}
}

PX_Object * PX_ObjectGetRoot(PX_Object *Object)
{
	while(Object->pParent)
	{
		Object=Object->pParent;
	}
	return Object;
}

PX_Object_Event PX_Object_Event_CursorOffset(PX_Object_Event e,px_point offset)
{
	switch(e.Event)
	{
	case PX_OBJECT_EVENT_CURSORMOVE:
	case PX_OBJECT_EVENT_CURSORUP:
	case PX_OBJECT_EVENT_CURSORRDOWN:
	case PX_OBJECT_EVENT_CURSORDOWN:
	case PX_OBJECT_EVENT_CURSORRUP:
	case PX_OBJECT_EVENT_CURSOROVER:
	case PX_OBJECT_EVENT_CURSOROUT:
	case PX_OBJECT_EVENT_CURSORWHEEL:
	case PX_OBJECT_EVENT_CURSORCLICK:
	case PX_OBJECT_EVENT_CURSORDRAG:
		PX_Object_Event_SetCursorX(&e,PX_Object_Event_GetCursorX(e)+offset.x);
		PX_Object_Event_SetCursorY(&e,PX_Object_Event_GetCursorY(e)+offset.y);
		return e;
	default:
		return e;
	}
}

px_float PX_Object_Event_GetCursorX(PX_Object_Event e)
{
	return e.Param_float[0];
}

px_float PX_Object_Event_GetCursorY(PX_Object_Event e)
{
	return e.Param_float[1];
}


px_float PX_Object_Event_GetCursorZ(PX_Object_Event e)
{
	return e.Param_float[2];
}

px_int PX_Object_Event_GetCursorIndex(PX_Object_Event e)
{
	return e.Param_int[2];
}

px_float PX_Object_Event_GetWidth(PX_Object_Event e)
{
	return e.Param_float[0];
}

px_float PX_Object_Event_GetHeight(PX_Object_Event e)
{
	return e.Param_float[1];
}

px_int PX_Object_Event_GetIndex(PX_Object_Event e)
{
	return e.Param_int[0];
}

px_void PX_Object_Event_SetWidth(PX_Object_Event *e,px_float w)
{
	e->Param_float[0]=w;
}

px_void PX_Object_Event_SetHeight(PX_Object_Event *e,px_float h)
{
	e->Param_float[1]=h;
}

px_void PX_Object_Event_SetCursorX(PX_Object_Event *e,px_float x)
{
	e->Param_float[0]=x;
}

px_void PX_Object_Event_SetCursorY(PX_Object_Event *e,px_float y)
{
	e->Param_float[1]=y;
}

px_void PX_Object_Event_SetCursorZ(PX_Object_Event *e,px_float z)
{
	e->Param_float[2]=z;
}

px_void PX_Object_Event_SetCursorIndex(PX_Object_Event *e,px_int index)
{
	e->Param_int[2]=index;
}

px_float PX_Object_Event_GetScaleCursorX(PX_Object_Event e)
{
	return e.Param_float[0];
}

px_float PX_Object_Event_GetScaleCursorY(PX_Object_Event e)
{
	return e.Param_float[1];
}

px_float PX_Object_Event_GetScaleCursorZ(PX_Object_Event e)
{
	return e.Param_float[2];
}

px_void PX_Object_Event_SetScaleCursorX(PX_Object_Event *e,px_float x)
{
	e->Param_float[0]=x;
}

px_void PX_Object_Event_SetScaleCursorY(PX_Object_Event *e,px_float y)
{
	e->Param_float[1]=y;
}

px_void PX_Object_Event_SetScaleCursorZ(PX_Object_Event *e,px_float z)
{
	e->Param_float[2]=z;
}

px_uint PX_Object_Event_GetKeyDown(PX_Object_Event e)
{
	return e.Param_uint[0];
}

px_void PX_Object_Event_SetKeyDown(PX_Object_Event *e,px_uint key)
{
	e->Param_uint[0]=key;
}

px_char* PX_Object_Event_GetStringPtr(PX_Object_Event e)
{
	return (px_char *)e.Param_ptr[0];
}

px_void* PX_Object_Event_GetDataPtr(PX_Object_Event e)
{
	return (px_void *)e.Param_ptr[0];
}


px_void PX_Object_Event_SetStringPtr(PX_Object_Event *e,px_void *ptr)
{
	e->Param_ptr[0]=ptr;
}

px_void PX_Object_Event_SetDataPtr(PX_Object_Event *e,px_void *ptr)
{
	e->Param_ptr[0]=ptr;
}

px_void PX_Object_Event_SetIndex(PX_Object_Event *e,px_int index)
{
	e->Param_int[0]=index;
}

PX_Object * PX_ObjectCreate(px_memorypool *mp,PX_Object *Parent,px_float x,px_float y,px_float z,px_float Width,px_float Height,px_float Lenght)
{
	
	PX_Object *pObject=(PX_Object *)MP_Malloc(mp,sizeof(PX_Object));
	if (pObject==PX_NULL)
	{
		return PX_NULL;
	}

	PX_ObjectInit(mp,pObject,Parent,x,y,z,Width,Height,Lenght);

	return pObject;
}



PX_Object * PX_ObjectCreateEx(px_memorypool *mp,PX_Object *Parent,px_float x,px_float y,px_float z,px_float Width,px_float Height,px_float Lenght,px_int type,Function_ObjectUpdate Func_ObjectUpdate,Function_ObjectRender Func_ObjectRender,Function_ObjectFree Func_ObjectFree,px_void *desc,px_int size)
{
	PX_Object *pObject=PX_ObjectCreate(mp,Parent,x,y,z,Width,Height,Lenght);
	if (pObject)
	{
		pObject->pObject=MP_Malloc(mp,size);
		if (!pObject->pObject)
		{
			MP_Free(mp,pObject);
			return PX_NULL;
		}
		PX_memcpy(pObject->pObject,desc,size);
		pObject->Type=type;
		pObject->Func_ObjectFree=Func_ObjectFree;
		pObject->Func_ObjectRender=Func_ObjectRender;
		pObject->Func_ObjectUpdate=Func_ObjectUpdate;
	}
	return pObject;
}

px_void PX_ObjectGetInheritXY(PX_Object *Object,px_float *x,px_float *y)
{
	*x=0;
	*y=0;
	Object=Object->pParent;
	while (Object)
	{
		*x+=Object->x;
		*y+=Object->y;
		Object=Object->pParent;
	}
}

static px_void PX_Object_ObjectEventFree( PX_Object **Object )
{
	PX_OBJECT_EventAction *pNext,*pCur;;
	//PX_Object_Free Events linker
	pCur=(*Object)->pEventActions;
	while (pCur)
	{
		pNext=pCur->pNext;
		MP_Free((*Object)->mp,pCur);
		pCur=pNext;
	}

	(*Object)->pEventActions=PX_NULL;
}



static px_void PX_Object_ObjectFree( PX_Object *Object )
{
	PX_Object_ObjectEventFree(&Object);
	if (Object->Func_ObjectFree!=0)
	{
		Object->Func_ObjectFree(Object);
	}
	if(Object->pObject)
	MP_Free(Object->mp,Object->pObject);

	MP_Free(Object->mp,Object);
}


static px_void PX_Object_DeleteLinkerObject( PX_Object **ppObject )
{   
	PX_Object *Object=(*ppObject);

	if (Object==PX_NULL)
	{
		return;
	}
	PX_Object_DeleteLinkerObject(&Object->pNextBrother);
	PX_Object_DeleteLinkerObject(&Object->pChilds);

	PX_Object_ObjectFree(Object);

	(*ppObject)=PX_NULL;
}

px_void PX_ObjectDelete( PX_Object *pObject )
{
	if (pObject==PX_NULL) 
	{
		return;
	}

	if (pObject->pChilds!=PX_NULL)
	{
		PX_Object_DeleteLinkerObject(&pObject->pChilds);
	}

	if (pObject->pParent!=PX_NULL)
	{
		if (pObject->pParent->pChilds==pObject)
		{
		   pObject->pParent->pChilds=pObject->pNextBrother;
		   if(pObject->pNextBrother)
		   {
		   pObject->pNextBrother->pParent=pObject->pParent;
		   pObject->pNextBrother->pPreBrother=PX_NULL;
		   }
		}
		else
		{
			if (pObject->pPreBrother!=PX_NULL)
			{
				pObject->pPreBrother->pNextBrother=pObject->pNextBrother;
				if(pObject->pNextBrother)
				pObject->pNextBrother->pPreBrother=pObject->pPreBrother;
			}
			else
			{
				PX_ERROR("Invalid GUI Object struct");
			}
		}
	}
	else
	{
		if (pObject->pPreBrother!=PX_NULL)
		{
			pObject->pPreBrother=pObject->pNextBrother;
		}
	}


	PX_Object_ObjectFree(pObject);

}


px_void PX_ObjectDeleteChilds( PX_Object *pObject )
{
	if (pObject==PX_NULL) 
	{
		return;
	}

	if (pObject->pChilds!=PX_NULL)
	{
		PX_Object_DeleteLinkerObject(&pObject->pChilds);
	}
}

px_void PX_Object_ObjectLinkerUpdate( PX_Object *Object,px_uint elpased)
{
	if (Object==PX_NULL)
	{
		return;
	}
	if (Object->Visible!=PX_FALSE)
	{
		if (Object->Func_ObjectUpdate!=0)
		{
			Object->Func_ObjectUpdate(Object,elpased);
		}
		PX_Object_ObjectLinkerUpdate(Object->pChilds,elpased);
	}
	PX_Object_ObjectLinkerUpdate(Object->pNextBrother,elpased);	
}


px_void PX_ObjectUpdate(PX_Object *Object,px_uint elpased )
{
	if (Object==PX_NULL)
	{
		PX_ASSERT();
		return;
	}
	if (Object->Visible==PX_FALSE||Object->Enabled==PX_FALSE)
	{
		return;
	}
	if (Object->Func_ObjectUpdate!=0)
	{
		Object->Func_ObjectUpdate(Object,elpased);
	}
	if (Object->pChilds!=PX_NULL)
	{
		PX_Object_ObjectLinkerUpdate(Object->pChilds,elpased);
	}
}

px_void PX_ObjectRender(px_surface *pSurface, PX_Object *Object,px_uint elpased )
{
	if (Object==PX_NULL)
	{
		return;
	}

	if (Object->OnFocus)
	{
		PX_ObjectRender(pSurface,Object->pNextBrother,elpased);	

		if (Object->Visible!=PX_FALSE)
		{
			if (Object->Func_ObjectBeginRender)
			{
				Object->Func_ObjectBeginRender(pSurface,Object,elpased);
			}
			if (Object->Func_ObjectRender!=0)
			{
				Object->Func_ObjectRender(pSurface,Object,elpased);
			}
			PX_ObjectRender(pSurface,Object->pChilds,elpased);
			if (Object->Func_ObjectEndRender)
			{
				Object->Func_ObjectEndRender(pSurface,Object,elpased);
			}
		}
	}
	else
	{
		if (Object->Visible!=PX_FALSE)
		{
			if (Object->Func_ObjectBeginRender)
			{
				Object->Func_ObjectBeginRender(pSurface,Object,elpased);
			}
			if (Object->Func_ObjectRender!=0)
			{
				Object->Func_ObjectRender(pSurface,Object,elpased);
			}
			PX_ObjectRender(pSurface,Object->pChilds,elpased);
			
			if (Object->Func_ObjectEndRender)
			{
				Object->Func_ObjectEndRender(pSurface,Object,elpased);
			}
		}
		PX_ObjectRender(pSurface,Object->pNextBrother,elpased);	
	}
}



px_bool PX_ObjectIsPointInRegion( PX_Object *pObject,px_float x,px_float y )
{
	px_float objx,objy,objw,objh;
	px_float inheritX,inheritY;

	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objw=pObject->Width;
	objh=pObject->Height;

	return PX_isXYInRegion(x,y,objx,objy,objw,objh);
}

px_bool PX_ObjectIsCursorInRegion(PX_Object *Object,PX_Object_Event e)
{
	return PX_ObjectIsPointInRegion(Object,PX_Object_Event_GetCursorX(e),PX_Object_Event_GetCursorY(e));
}

px_float PX_ObjectGetHeight(PX_Object *Object)
{
	return Object->Height;
}

px_float PX_ObjectGetWidth(PX_Object *Object)
{
	return Object->Width;
}

static px_void PX_ObjectAddClild(PX_Object *Parent,PX_Object *child)
{
	PX_Object *pLinker;
	child->pParent=Parent;
	if(Parent->pChilds==PX_NULL)
	{
		Parent->pChilds=child;
	}
	else
	{
		pLinker=Parent->pChilds;
		while (pLinker->pNextBrother)
		{
			pLinker=pLinker->pNextBrother;
		}
		pLinker->pNextBrother=child;
		child->pPreBrother=pLinker;
	}
	child->pNextBrother=PX_NULL;
}

px_void PX_ObjectInit(px_memorypool *mp,PX_Object *pObject,PX_Object *Parent,px_float x,px_float y,px_float z,px_float Width,px_float Height,px_float Lenght )
{
	PX_memset(pObject,0,sizeof(PX_Object));
	

	pObject->id=0;
	pObject->x=x;
	pObject->y=y;
	pObject->z=z;
	pObject->Width=Width;
	pObject->Height=Height;

	pObject->Enabled=PX_TRUE;
	pObject->Visible=PX_TRUE;
	pObject->pChilds=PX_NULL;
	pObject->pObject=PX_NULL;
	pObject->pNextBrother=PX_NULL;
	pObject->pPreBrother=PX_NULL;
	pObject->Type=PX_OBJECT_TYPE_NULL;
	pObject->ReceiveEvents=PX_TRUE;
	pObject->impact_test_type=0;
	pObject->impact_Object_type=0;
	pObject->pEventActions=PX_NULL;
	pObject->world_index=-1;
	pObject->User_int=0;
	pObject->diameter=0;
	pObject->User_ptr=PX_NULL;
	pObject->OnFocus=PX_FALSE;
	pObject->mp=mp;
	pObject->Func_ObjectFree=PX_NULL;
	pObject->Func_ObjectRender=PX_NULL;
	pObject->Func_ObjectUpdate=PX_NULL;
	pObject->Func_ObjectLinkChild=PX_NULL;
	pObject->Func_ObjectBeginRender=PX_NULL;
	pObject->Func_ObjectEndRender=PX_NULL;

	if (Parent!=PX_NULL)
	{
		if (Parent->Func_ObjectLinkChild!=PX_NULL)
		{
			Parent->Func_ObjectLinkChild(Parent,pObject);
		}
		else
		{
			PX_ObjectAddClild(Parent,pObject);
		}
		
	}
}



px_void PX_ObjectSetRenderLinker(PX_Object *pObject)
{
	if (pObject)
	{
		
		PX_ObjectSetRenderLinker(pObject->pNextBrother);
		PX_ObjectSetRenderLinker(pObject->pChilds);
	}
}



px_void PX_ObjectSetVisible( PX_Object *Object,px_bool visible )
{
	if(Object!=PX_NULL)
	{
		if (visible==Object->Visible)
		{
			return;
		}

	if (visible==PX_FALSE&&Object->Visible==PX_TRUE)
	{
		//PX_Object_ObjectClear(Object);
	}
	else
	{
		if (visible==PX_TRUE&&Object->Visible==PX_FALSE)
		{
			
			PX_ObjectSetRenderLinker(Object->pChilds);
		}
	}
	Object->Visible=visible;
	}
}







px_int PX_ObjectRegisterEvent( PX_Object *Object,px_uint Event,px_void (*ProcessFunc)(PX_Object *,PX_Object_Event e,px_void *user_ptr),px_void *user)
{
	PX_OBJECT_EventAction *pPoint;
	PX_OBJECT_EventAction *pAction=(PX_OBJECT_EventAction *)MP_Malloc(Object->mp,sizeof(PX_OBJECT_EventAction));
	if (pAction==PX_NULL)
	{
		return PX_FALSE;
	}
	pAction->pNext=PX_NULL;
	pAction->pPre=PX_NULL;
	pAction->EventAction=Event;
	pAction->EventActionFunc=ProcessFunc;
	
	pAction->user_ptr=user;

	pPoint=Object->pEventActions;
	if (pPoint==PX_NULL)
	{
		Object->pEventActions=pAction;
		return PX_TRUE;
	}

	while(pPoint->pNext)
	{
		pPoint=pPoint->pNext;
	}
	pAction->pPre=pPoint;
	pPoint->pNext=pAction;

	return PX_TRUE;
}


px_void PX_ObjectExecuteEvent(PX_Object *pPost,PX_Object_Event Event)
{
	PX_OBJECT_EventAction *EventAction;
	if (pPost==PX_NULL)
	{
		return;
	}

	if (pPost->Visible==PX_FALSE||pPost->Enabled==PX_FALSE||pPost->ReceiveEvents==PX_FALSE)
	{
		return;
	}

	EventAction=pPost->pEventActions;
	while(EventAction)
	{
		if (EventAction->EventAction==PX_OBJECT_EVENT_ANY||EventAction->EventAction==Event.Event)
		{
			EventAction->EventActionFunc(pPost,Event,EventAction->user_ptr);
		}
		EventAction=EventAction->pNext;
	}
}


px_bool PX_ObjectPostEventLink( PX_Object *pPost,PX_Object_Event Event )
{
	if (pPost==PX_NULL)
	{
		return PX_TRUE;
	}

	if (pPost->OnFocus)
	{
		if(PX_ObjectPostEventLink(pPost->pChilds,Event)==PX_FALSE)
		{
			return PX_FALSE;
		}
		else
		{
			PX_ObjectExecuteEvent(pPost,Event);
		}

		if (!pPost->OnFocus&&!pPost->OnLostFocusReleaseEvent)
		{
			return PX_ObjectPostEventLink(pPost->pNextBrother,Event);
		}

		return PX_FALSE;
	}
	else
	{
		if(PX_ObjectPostEventLink(pPost->pNextBrother,Event)==PX_FALSE) return PX_FALSE;
		if(PX_ObjectPostEventLink(pPost->pChilds,Event)==PX_FALSE) return PX_FALSE;	
		PX_ObjectExecuteEvent(pPost,Event);
		return PX_TRUE;
	}
	
}


px_void PX_ObjectPostEvent( PX_Object *pPost,PX_Object_Event Event )
{

	if (pPost==PX_NULL)
	{
		return;
	}

	if (pPost->OnFocus)
	{
		if(PX_ObjectPostEventLink(pPost->pChilds,Event)==PX_FALSE)
		{
			if (!pPost->OnFocus)
			{
				PX_ObjectExecuteEvent(pPost,Event);
			}
		}
		else
		{
			PX_ObjectExecuteEvent(pPost,Event);
		}
		return;
	}
	else
	{
		if(PX_ObjectPostEventLink(pPost->pChilds,Event)==PX_FALSE) return;	
		PX_ObjectExecuteEvent(pPost,Event);
		return;
	}
}




px_void PX_Object_LabelRender(px_surface *psurface, PX_Object *pObject,px_uint elpased)
{
	px_int x,y,w,h;
	px_float inheritX,inheritY;
	PX_SurfaceLimitInfo limitInfo;
	PX_Object_Label *pLabel=(PX_Object_Label *)pObject->pObject;
	if (pLabel==PX_NULL)
	{
		return;
	}

	if (!pObject->Visible)
	{
		return;
	}

	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);
	x=(px_int)(pObject->x+inheritX);
	y=(px_int)(pObject->y+inheritY);
	w=(px_int)pObject->Width;
	h=(px_int)pObject->Height;

	PX_GeoDrawRect(psurface,x,y,x+w,y+h-1,pLabel->BackgroundColor);
	if (pLabel->bBorder)
	{
		switch(pLabel->style)
		{
		case PX_OBJECT_LABEL_STYLE_ROUNDRECT:
			PX_GeoDrawRoundRect(psurface,x,y,x+w,y+h-1,h/2-2.0f,1,pLabel->borderColor);
			break;
		default:
			PX_GeoDrawBorder(psurface,x,y,x+w,y+h-1,1,pLabel->borderColor);
		}
	}
	
	limitInfo=PX_SurfaceGetLimit(psurface);
	PX_SurfaceSetLimit(psurface,x,y,x+w-1,y+h-1);

	switch (pLabel->Align)
	{
	case PX_ALIGN_LEFTTOP:
		PX_FontModuleDrawText(psurface,pLabel->fontModule,x,y,PX_ALIGN_LEFTTOP,pLabel->Text,pLabel->TextColor);
		break;
	case PX_ALIGN_LEFTMID:
		PX_FontModuleDrawText(psurface,pLabel->fontModule,x,y+h/2,PX_ALIGN_LEFTMID,pLabel->Text,pLabel->TextColor);
		break;
	case PX_ALIGN_LEFTBOTTOM:
		PX_FontModuleDrawText(psurface,pLabel->fontModule,x,y+h,PX_ALIGN_LEFTBOTTOM,pLabel->Text,pLabel->TextColor);
		break;
	case PX_ALIGN_MIDTOP:
		PX_FontModuleDrawText(psurface,pLabel->fontModule,x+w/2,y,PX_ALIGN_MIDTOP,pLabel->Text,pLabel->TextColor);
		break;
	case PX_ALIGN_CENTER:
		PX_FontModuleDrawText(psurface,pLabel->fontModule,x+w/2,y+h/2,PX_ALIGN_CENTER,pLabel->Text,pLabel->TextColor);
		break;
	case PX_ALIGN_MIDBOTTOM:
		PX_FontModuleDrawText(psurface,pLabel->fontModule,x+w/2,y+h,PX_ALIGN_MIDBOTTOM,pLabel->Text,pLabel->TextColor);
		break;
	case PX_ALIGN_RIGHTTOP:
		PX_FontModuleDrawText(psurface,pLabel->fontModule,x+w,y,PX_ALIGN_RIGHTTOP,pLabel->Text,pLabel->TextColor);
		break;
	case PX_ALIGN_RIGHTMID:
		PX_FontModuleDrawText(psurface,pLabel->fontModule,x+w,y+h/2,PX_ALIGN_RIGHTMID,pLabel->Text,pLabel->TextColor);
		break;
	case PX_ALIGN_RIGHTBOTTOM:
		PX_FontModuleDrawText(psurface,pLabel->fontModule,x+w,y+h,PX_ALIGN_RIGHTBOTTOM,pLabel->Text,pLabel->TextColor);
		break;
	}
	PX_SurfaceSetLimitInfo(psurface,limitInfo);
}

px_void PX_Object_LabelFree( PX_Object *pLabel )
{
	PX_Object_Label *Label=PX_Object_GetLabel(pLabel);
	if (pLabel!=PX_NULL)
	{
		MP_Free(pLabel->mp,Label->Text);
	}
}

PX_Object* PX_Object_LabelCreate(px_memorypool *mp,PX_Object *Parent,px_int x,px_int y,px_int Width,px_int Height,const px_char *Text,PX_FontModule *fm,px_color Color )
{
	px_int TextLen;
	PX_Object *pObject;
	PX_Object_Label *pLable=(PX_Object_Label *)MP_Malloc(mp,sizeof(PX_Object_Label));
	if (pLable==PX_NULL)
	{
		return PX_NULL;
	}
	pObject=PX_ObjectCreate(mp,Parent,(px_float)x,(px_float)y,0,(px_float)Width,(px_float)Height,0);

	if (pObject==PX_NULL)
	{
		return PX_NULL;
	}

	if (!pObject)
	{
		MP_Free(mp,pLable);
		return PX_NULL;
	}
	pObject->pObject=pLable;
	pObject->Type=PX_OBJECT_TYPE_LABEL;
	pObject->ReceiveEvents=PX_FALSE;
	pObject->Func_ObjectFree=PX_Object_LabelFree;
	pObject->Func_ObjectRender=PX_Object_LabelRender;

	TextLen=PX_strlen(Text);
	
	pLable->Text=(px_char *)MP_Malloc(mp,TextLen+1);
	if (pLable->Text==PX_NULL)
	{
		MP_Free(pObject->mp,pObject);
		MP_Free(pObject->mp,pLable);
		return PX_NULL;
	}

	PX_strcpy(pLable->Text,Text,TextLen+1);

	pLable->TextColor=Color;
	pLable->BackgroundColor=PX_COLOR(0,0,0,0);
	pLable->Align=PX_ALIGN_LEFTMID;
	pLable->bBorder=PX_FALSE;
	pLable->borderColor=PX_COLOR(255,0,0,0);
	pLable->fontModule=fm;
	pLable->style=PX_OBJECT_LABEL_STYLE_RECT;
	return pObject;
}


PX_Object_Label  * PX_Object_GetLabel( PX_Object *Object )
{
	if(Object->Type==PX_OBJECT_TYPE_LABEL)
		return (PX_Object_Label *)Object->pObject;
	else
		return PX_NULL;
}

px_char	  * PX_Object_LabelGetText( PX_Object *Label )
{
	PX_Object_Label *pLabel=PX_Object_GetLabel(Label);
	if (pLabel!=PX_NULL)
	{
		return pLabel->Text;
	}
	return PX_NULL;
}
px_void PX_Object_LabelSetText( PX_Object *pObject,const px_char *Text )
{
	PX_Object_Label *pLabel;
	px_int TextLen;
	if (pObject==PX_NULL||Text==PX_NULL)
	{
		return;
	}
	if (pObject->Type!=PX_OBJECT_TYPE_LABEL)
	{
		return;
	}
	
	TextLen=PX_strlen(Text);
	pLabel=(PX_Object_Label *)pObject->pObject;
	if(TextLen>PX_strlen(pLabel->Text))
	{
	MP_Free(pObject->mp,pLabel->Text);
	pLabel->Text=(px_char *)MP_Malloc(pObject->mp,TextLen+1);
		if (!pLabel->Text)
		{
			PX_ERROR("Text create out of memories");
		}
	PX_strcpy(pLabel->Text,Text,TextLen+1);
	}
	else
	{
		PX_strcpy(pLabel->Text,Text,TextLen+1);
	}
}


px_void PX_Object_LabelSetTextColor(PX_Object *pObject,px_color Color)
{
	PX_Object_Label * pLabel=PX_Object_GetLabel(pObject);
	if (pLabel)
	{
		pLabel->TextColor=Color;
	}
}


px_void PX_Object_LabelSetBackgroundColor( PX_Object *pObject,px_color Color )
{
	PX_Object_Label * pLabel=PX_Object_GetLabel(pObject);
	if (pLabel)
	{
		pLabel->BackgroundColor=Color;
	}
}
px_void PX_Object_LabelSetAlign( PX_Object *pObject,PX_ALIGN Align )
{
	PX_Object_Label * pLabel=PX_Object_GetLabel(pObject);
	if (pLabel)
	{
		
		pLabel->Align=Align;
	}
}



px_void PX_Object_LabelSetBorder(PX_Object *pObject,px_bool b)
{
	PX_Object_Label * pLabel=PX_Object_GetLabel(pObject);
	if (pLabel)
	{
		pLabel->bBorder=b;
	}
}

px_void PX_Object_LabelSetBorderColor(PX_Object *pObject,px_color color)
{
	PX_Object_Label * pLabel=PX_Object_GetLabel(pObject);
	if (pLabel)
	{
		pLabel->borderColor=color;
	}
}

px_void PX_Object_LabelSetStyle(PX_Object *pObject,PX_OBJECT_LABEL_STYLE style)
{
	PX_Object_Label * pLabel=PX_Object_GetLabel(pObject);
	if (pLabel)
	{
		pLabel->style=style;
	}
}

px_void PX_Object_ProcessBarRender(px_surface *psurface, PX_Object *pObject,px_uint elpased)
{
	px_int px,py,pl;//Processing start X,Processing Start Y &Processing length
	px_int x,y,w,h;
	px_float inheritX,inheritY;

	PX_Object_ProcessBar *pProcessBar=(PX_Object_ProcessBar *)pObject->pObject;
	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	x=(px_int)(pObject->x+inheritX);
	y=(px_int)(pObject->y+inheritY);
	w=(px_int)pObject->Width;
	h=(px_int)pObject->Height;



	px=(px_int)x+2; //increase line width
	py=(px_int)y+2;

	if (pProcessBar->Value<0)
	{
		pProcessBar->Value=0;
	}
	if (pProcessBar->Value>pProcessBar->MAX)
	{
		pProcessBar->Value=pProcessBar->MAX;
	}

	if (w<=4)
	{
		return;
	}
	if(pProcessBar->Value<pProcessBar->MAX)
		pl=(px_int)((w-4)*((pProcessBar->Value*1.0)/pProcessBar->MAX));
	else
		pl=(px_int)w-2;
	//Draw BackGround
	PX_SurfaceClear(
		psurface,
		(px_int)x,
		(px_int)y,
		(px_int)x+w-1,
		(px_int)y+h-1,
		pProcessBar->BackgroundColor
		);
	//Draw border
	PX_GeoDrawBorder(
		psurface,
		x,
		y,
		x+w-1,
		y+h-1,
		1,
		pProcessBar->Color
		);

	PX_GeoDrawRect(psurface,px,py,px+pl-2-1,py+(px_int)h-4-1,pProcessBar->Color);

}

PX_Object * PX_Object_ProcessBarCreate(px_memorypool *mp,PX_Object *Parent,px_int x,px_int y,px_int Width,px_int Height )
{
	PX_Object *pObject;
	PX_Object_ProcessBar *ProcessBar=(PX_Object_ProcessBar *)MP_Malloc(mp,sizeof(PX_Object_ProcessBar));
	if (ProcessBar==PX_NULL)
	{
		return PX_NULL;
	}
	pObject=PX_ObjectCreate(mp,Parent,(px_float)x,(px_float)y,0,(px_float)Width,(px_float)Height,0);
	if (pObject==PX_NULL)
	{
		MP_Free(pObject->mp,ProcessBar);
		return PX_NULL;
	}

	pObject->pObject=ProcessBar;
	pObject->Enabled=PX_TRUE;
	pObject->Visible=PX_TRUE;
	pObject->Type=PX_OBJECT_TYPE_PROCESSBAR;
	pObject->ReceiveEvents=PX_FALSE;
	pObject->Func_ObjectFree=PX_NULL;
	pObject->Func_ObjectRender=PX_Object_ProcessBarRender;

	ProcessBar->MAX=100;
	ProcessBar->Value=0;
	ProcessBar->Color=PX_COLOR(255,0,0,0);
	ProcessBar->BackgroundColor=PX_COLOR(255,255,255,255);
	return pObject;
}

px_void PX_Object_ProcessBarSetColor( PX_Object *pProcessBar,px_color Color)
{
	PX_Object_ProcessBar *pProcess=PX_Object_GetProcessBar(pProcessBar);
	if (pProcess!=PX_NULL)
	{
		
		pProcess->Color=Color;
	}
}

px_void PX_Object_ProcessBarSetBackgroundColor(PX_Object *pProcessBar,px_color Color)
{
	PX_Object_ProcessBar *pProcess=PX_Object_GetProcessBar(pProcessBar);
	if (pProcess!=PX_NULL)
	{
		pProcess->BackgroundColor=Color;
	}
}

px_void PX_Object_ProcessBarSetValue( PX_Object *pProcessBar,px_int Value )
{
	PX_Object_ProcessBar *pProcess=PX_Object_GetProcessBar(pProcessBar);
	if (pProcess!=PX_NULL)
	{
		pProcess->Value=Value;
	}
}

px_void PX_Object_ProcessBarSetMax( PX_Object *pProcessBar,px_int Max )
{
	PX_Object_ProcessBar *pProcess=PX_Object_GetProcessBar(pProcessBar);
	if (pProcess!=PX_NULL)
	{
		
		pProcess->MAX=Max;
	}
}



PX_Object_ProcessBar * PX_Object_GetProcessBar( PX_Object *Object )
{
	if(Object->Type==PX_OBJECT_TYPE_PROCESSBAR)
		return (PX_Object_ProcessBar *)Object->pObject;
	else
		return PX_NULL;
}



px_int PX_Object_ProcessBarGetValue( PX_Object *pProcessBar )
{
	if (pProcessBar->Type!=PX_OBJECT_TYPE_PROCESSBAR)
	{
		return 0;
	}
	return PX_Object_GetProcessBar(pProcessBar)->Value;
}


px_void PX_Object_ImageRender(px_surface *psurface, PX_Object *pObject,px_uint elpased)
{
	PX_Object_Image *pImage=PX_Object_GetImage(pObject);
	px_int x,y,w,h;
	PX_SurfaceLimitInfo limitInfo;
	px_float inheritX,inheritY;
	if (!pImage->pTexture)
	{
		return;
	}

	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	x=(px_int)(pObject->x+inheritX);
	y=(px_int)(pObject->y+inheritY);
	w=(px_int)pObject->Width;
	h=(px_int)pObject->Height;

	limitInfo=PX_SurfaceGetLimit(psurface);
	PX_SurfaceSetLimit(psurface,x,y,x+w-1,y+h-1);

	switch(pImage->Align)
	{
	case PX_ALIGN_LEFTTOP:
		{
			if (pImage!=PX_NULL)
			{
				if(pImage->pmask)
					PX_TextureRenderMask(psurface,pImage->pmask,pImage->pTexture,x,y,PX_TEXTURERENDER_REFPOINT_LEFTTOP,PX_NULL);
				else
					PX_TextureRender(psurface,pImage->pTexture,x,y,PX_TEXTURERENDER_REFPOINT_LEFTTOP,PX_NULL);
			}
		}
		break;
	case PX_ALIGN_LEFTMID:
		{
			if (pImage!=PX_NULL)
			{
				if(pImage->pmask)
					PX_TextureRenderMask(psurface,pImage->pmask,pImage->pTexture,x,y+h/2,PX_TEXTURERENDER_REFPOINT_LEFTMID,PX_NULL);
				else
					PX_TextureRender(psurface,pImage->pTexture,x,y+h/2,PX_TEXTURERENDER_REFPOINT_LEFTMID,PX_NULL);
			}
		}
		break;
	case PX_ALIGN_LEFTBOTTOM:
		{
			if (pImage!=PX_NULL)
			{
				if(pImage->pmask)
					PX_TextureRenderMask(psurface,pImage->pmask,pImage->pTexture,x,y+h,PX_TEXTURERENDER_REFPOINT_LEFTBOTTOM,PX_NULL);
				else
					PX_TextureRender(psurface,pImage->pTexture,x,y+h,PX_TEXTURERENDER_REFPOINT_LEFTBOTTOM,PX_NULL);
			}
		}
		break;
	case PX_ALIGN_MIDTOP:
		{
			if (pImage!=PX_NULL)
			{
				if(pImage->pmask)
					PX_TextureRenderMask(psurface,pImage->pmask,pImage->pTexture,x+w/2,y,PX_TEXTURERENDER_REFPOINT_MIDTOP,PX_NULL);
				else
					PX_TextureRender(psurface,pImage->pTexture,x+w/2,y,PX_TEXTURERENDER_REFPOINT_MIDTOP,PX_NULL);
			}
		}
		break;
	case PX_ALIGN_CENTER:
		{
			if (pImage!=PX_NULL)
			{
				if(pImage->pmask)
					PX_TextureRenderMask(psurface,pImage->pmask,pImage->pTexture,x+w/2,y+h/2,PX_TEXTURERENDER_REFPOINT_CENTER,PX_NULL);
				else
					PX_TextureRender(psurface,pImage->pTexture,x+w/2,y+h/2,PX_TEXTURERENDER_REFPOINT_CENTER,PX_NULL);
			}
		}
		break;
	case PX_ALIGN_MIDBOTTOM:
		{
			if (pImage!=PX_NULL)
			{
				if(pImage->pmask)
					PX_TextureRenderMask(psurface,pImage->pmask,pImage->pTexture,x+w/2,y+h,PX_TEXTURERENDER_REFPOINT_MIDBOTTOM,PX_NULL);
				else
					PX_TextureRender(psurface,pImage->pTexture,x+w/2,y+h,PX_TEXTURERENDER_REFPOINT_MIDBOTTOM,PX_NULL);
			}
		}
		break;
	case PX_ALIGN_RIGHTTOP:
		{
			if (pImage!=PX_NULL)
			{
				if(pImage->pmask)
					PX_TextureRenderMask(psurface,pImage->pmask,pImage->pTexture,x+w,y,PX_TEXTURERENDER_REFPOINT_RIGHTTOP,PX_NULL);
				else
					PX_TextureRender(psurface,pImage->pTexture,x+w,y,PX_TEXTURERENDER_REFPOINT_RIGHTTOP,PX_NULL);
			}
		}
		break;
	case PX_ALIGN_RIGHTMID:
		{
			if (pImage!=PX_NULL)
			{
				if(pImage->pmask)
					PX_TextureRenderMask(psurface,pImage->pmask,pImage->pTexture,x+w,y+h/2,PX_TEXTURERENDER_REFPOINT_RIGHTMID,PX_NULL);
				else
					PX_TextureRender(psurface,pImage->pTexture,x+w,y+h/2,PX_TEXTURERENDER_REFPOINT_RIGHTMID,PX_NULL);
			}
		}
		break;
	case PX_ALIGN_RIGHTBOTTOM:
		{
			if (pImage!=PX_NULL)
			{
				if(pImage->pmask)
					PX_TextureRenderMask(psurface,pImage->pmask,pImage->pTexture,x+w,y+h,PX_TEXTURERENDER_REFPOINT_RIGHTBOTTOM,PX_NULL);
				else
					PX_TextureRender(psurface,pImage->pTexture,x+w,y+h,PX_TEXTURERENDER_REFPOINT_RIGHTBOTTOM,PX_NULL);
			}
		}
		break;
	}

	PX_SurfaceSetLimitInfo(psurface,limitInfo);
	
}

px_void PX_Object_ImageFree( PX_Object *pBitmap )
{

}

PX_Object * PX_Object_ImageCreate(px_memorypool *mp,PX_Object *Parent,px_int x,px_int y,px_int width,px_int height,px_texture *ptex )
{
	PX_Object *pObject;
	PX_Object_Image *pImage=(PX_Object_Image *)MP_Malloc(mp,sizeof(PX_Object_Image));
	if (pImage==PX_NULL)
	{
		return PX_NULL;
	}
	pObject=PX_ObjectCreate(mp,Parent,(px_float)x,(px_float)y,0,(px_float)width,(px_float)height,0);
	if (pObject==PX_NULL)
	{
		MP_Free(pObject->mp,pImage);
		return PX_NULL;
	}

	pObject->pObject=pImage;
	pObject->Enabled=PX_TRUE;
	pObject->Visible=PX_TRUE;
	pObject->Type=PX_OBJECT_TYPE_IMAGE;
	pObject->ReceiveEvents=PX_FALSE;
	pObject->Func_ObjectFree=PX_Object_ImageFree;
	pObject->Func_ObjectRender=PX_Object_ImageRender;
	pImage->pTexture=ptex;
	pImage->pmask=PX_NULL;
	pImage->Align=PX_ALIGN_CENTER;
	return pObject;
}


PX_Object_Image * PX_Object_GetImage( PX_Object *Object )
{
	if(Object->Type==PX_OBJECT_TYPE_IMAGE)
		return (PX_Object_Image *)Object->pObject;
	else
		return PX_NULL;
}

px_void PX_Object_ImageSetAlign( PX_Object *pImage,PX_ALIGN Align)
{
	PX_Object_Image *Bitmap=PX_Object_GetImage(pImage);
	if (Bitmap)
	{
		Bitmap->Align=Align;
	}
}


px_void PX_Object_ImageSetMask(PX_Object *pObject,px_texture *pmask)
{
	PX_Object_Image *pImg=PX_Object_GetImage(pObject);
	if (pImg)
	{
		pImg->pmask=pmask;
	}
}


px_void PX_Object_ImageSetTexture(PX_Object *pObject,px_texture *pTex)
{
	PX_Object_Image *pImg=PX_Object_GetImage(pObject);
	if (pImg)
	{
		pImg->pTexture=pTex;
	}
}

px_void PX_Object_SliderBarOnMouseLButtonDown(PX_Object *pObject,PX_Object_Event e,px_void *user_ptr)
{
	PX_Object_SliderBar *pSliderBar=PX_Object_GetSliderBar(pObject);
	px_float x,y;
	px_int SliderBtnLen,Sx,Sy;
	px_rect rect;
	px_int Range,relValue;

	px_int objx,objy,objw,objh;
	px_float inheritX,inheritY;

	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(px_int)(pObject->x+inheritX);
	objy=(px_int)(pObject->y+inheritY);
	objw=(px_int)pObject->Width;
	objh=(px_int)pObject->Height;

	x=PX_Object_Event_GetCursorX(e);
	y=PX_Object_Event_GetCursorY(e);


	if (pSliderBar)
	{
		Range=pSliderBar->Max-pSliderBar->Min+1;
		relValue=pSliderBar->Value-pSliderBar->Min;

		if(PX_ObjectIsPointInRegion(pObject,(px_float)x,(px_float)y))
		{
			if (pSliderBar==PX_NULL)
			{
				return;
			}

			if (objw<=10)
			{
				return;
			}

			if (objh<10)
			{
				return;
			}

			SliderBtnLen=pSliderBar->SliderButtonLength;


			if(pSliderBar->Type==PX_OBJECT_SLIDERBAR_TYPE_HORIZONTAL)
			{
				switch(pSliderBar->style)
				{
				case PX_OBJECT_SLIDERBAR_STYLE_LINER:
					{
						Sx=objx+(px_int)((objw-pSliderBar->SliderButtonLength)*(1.0f)*relValue/Range);
						

						rect.x=(px_float)Sx;
						rect.y=(px_float)objy;
						rect.width=(px_float)pSliderBar->SliderButtonLength;

						rect.height=(px_float)objh;
					    
						if (PX_isPointInRect(PX_POINT((px_float)x,(px_float)y,0),rect))
					    {
							pSliderBar->btnDownX=x;
							pSliderBar->btnDownY=y;
							pSliderBar->status=PX_OBJECT_SLIDERBAR_STATUS_ONDRAG;
					    }
					}
					break;
				case PX_OBJECT_SLIDERBAR_STYLE_BOX:
					{
						Sx=(px_int)objx+(px_int)((objw-pSliderBar->SliderButtonLength)*(1.0f)*relValue/Range);
					
						rect.x=(px_float)Sx;
						rect.y=(px_float)objy;
						rect.width=(px_float)pSliderBar->SliderButtonLength;
						rect.height=(px_float)objh;

						if (PX_isPointInRect(PX_POINT((px_float)x,(px_float)y,0),rect))
						{
							pSliderBar->btnDownX=x;
							pSliderBar->btnDownY=y;
							pSliderBar->status=PX_OBJECT_SLIDERBAR_STATUS_ONDRAG;
						}
						else if(x>rect.x)
						{
							pSliderBar->Value+=Range*pSliderBar->SliderButtonLength/((px_int)objw);
							if (pSliderBar->Value>pSliderBar->Max)
							{
								pSliderBar->Value=pSliderBar->Max;
							}
						}
						else
						{
							pSliderBar->Value-=Range*pSliderBar->SliderButtonLength/((px_int)objw);
							if (pSliderBar->Value<0)
							{
								pSliderBar->Value=0;
							}
						}

					}
					break;
				}


			}

			if (pSliderBar->Type==PX_OBJECT_SLIDERBAR_TYPE_VERTICAL)
			{
				switch(pSliderBar->style)
				{
				case PX_OBJECT_SLIDERBAR_STYLE_LINER:
					{
						Sx=(px_int)objx;
						Sy=(px_int)objy+((px_int)objh-pSliderBar->SliderButtonLength)*relValue/Range;

						rect.x=(px_float)Sx;
						rect.y=(px_float)Sy;
						rect.width=(px_float)objh;
						rect.height=(px_float)pSliderBar->SliderButtonLength;

						if (PX_isPointInRect(PX_POINT((px_float)x,(px_float)y,0),rect))
						{
							pSliderBar->btnDownX=x;
							pSliderBar->btnDownY=y;
							pSliderBar->status=PX_OBJECT_SLIDERBAR_STATUS_ONDRAG;
						}
					}
					break;
				case PX_OBJECT_SLIDERBAR_STYLE_BOX:
					{
						Sy=(px_int)objy+(px_int)((objh-pSliderBar->SliderButtonLength)*(1.0f)*relValue/Range);
	

						rect.x=(px_float)objx+2;
						rect.y=(px_float)Sy;

						rect.width=(px_float)objw;
						rect.height=(px_float)pSliderBar->SliderButtonLength;

						if (PX_isPointInRect(PX_POINT((px_float)x,(px_float)y,0),rect))
						{
							pSliderBar->btnDownX=x;
							pSliderBar->btnDownY=y;
							pSliderBar->status=PX_OBJECT_SLIDERBAR_STATUS_ONDRAG;
						}
						else if(y>rect.y)
						{
							pSliderBar->Value+=Range*pSliderBar->SliderButtonLength/((px_int)objh);
							if (pSliderBar->Value>pSliderBar->Max)
							{
								pSliderBar->Value=pSliderBar->Max;
							}
						}
						else
						{
							pSliderBar->Value-=Range*pSliderBar->SliderButtonLength/((px_int)objh);
							if (pSliderBar->Value<pSliderBar->Min)
							{
								pSliderBar->Value=pSliderBar->Min;
							}
						}
					}
					break;
				}

			}
		}
	}
}


px_void PX_Object_SliderBarOnCursorNormal(PX_Object *pObject,PX_Object_Event e,px_void *user_ptr)
{
	PX_Object_SliderBar *pSliderBar=PX_Object_GetSliderBar(pObject);

	if (pSliderBar)
	{
		pSliderBar->status=PX_OBJECT_SLIDERBAR_STATUS_NORMAL;
	}
}

px_void PX_Object_SliderBarOnCursorDrag(PX_Object *pObject,PX_Object_Event e,px_void *user_ptr)
{
	PX_Object_SliderBar *pSliderBar=PX_Object_GetSliderBar(pObject);
	px_float x,y;

	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;

	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;


	x=PX_Object_Event_GetCursorX(e);
	y=PX_Object_Event_GetCursorY(e);

	if (pSliderBar)
	{
		if (pSliderBar->status!=PX_OBJECT_SLIDERBAR_STATUS_ONDRAG)
		{
			return;
		}

		if(pSliderBar->Type==PX_OBJECT_SLIDERBAR_TYPE_HORIZONTAL)
		{
			if (x<objx)
			{
				pSliderBar->DargButtonX=0;
				return;
			}
			if (x>objx+objWidth)
			{
				pSliderBar->DargButtonX=objWidth-pSliderBar->SliderButtonLength;
				return;
			}

			pSliderBar->DargButtonX+=(x-pSliderBar->btnDownX);
			pSliderBar->btnDownX=x;
			if (pSliderBar->DargButtonX<0)
			{
				pSliderBar->DargButtonX=0;
			}
			if (pSliderBar->DargButtonX>=objWidth-pSliderBar->SliderButtonLength)
			{
				pSliderBar->DargButtonX=objWidth-pSliderBar->SliderButtonLength;
			}

		}

		if (pSliderBar->Type==PX_OBJECT_SLIDERBAR_TYPE_VERTICAL)
		{
			if (y<objy)
			{
				pSliderBar->DargButtonY=0;
				return;
			}
			if (y>objy+objHeight)
			{
				pSliderBar->DargButtonY=objHeight-pSliderBar->SliderButtonLength;
				return;
			}

			pSliderBar->DargButtonY+=(y-pSliderBar->btnDownY);
			pSliderBar->btnDownY=y;

			if (pSliderBar->DargButtonY<0)
			{
				pSliderBar->DargButtonY=0;
			}
			if (pSliderBar->DargButtonY>=objHeight-pSliderBar->SliderButtonLength)
			{
				pSliderBar->DargButtonY=objHeight-pSliderBar->SliderButtonLength;
			}
		}
	
	}
}


PX_Object * PX_Object_SliderBarCreate(px_memorypool *mp,PX_Object *Parent,px_int x,px_int y,px_int Width,px_int Height,PX_OBJECT_SLIDERBAR_TYPE Type,PX_OBJECT_SLIDERBAR_STYLE style)
{
	PX_Object *pObject;
	PX_Object_SliderBar *pSliderbar=(PX_Object_SliderBar *)MP_Malloc(mp,sizeof(PX_Object_SliderBar));
	if (pSliderbar==PX_NULL)
	{
		return PX_NULL;
	}
	pObject=PX_ObjectCreate(mp,Parent,(px_float)x,(px_float)y,0,(px_float)Width,(px_float)Height,0);
	if (pObject==PX_NULL)
	{
		MP_Free(pObject->mp,pSliderbar);
		return PX_NULL;
	}

	pObject->pObject=pSliderbar;
	pObject->Enabled=PX_TRUE;
	pObject->Visible=PX_TRUE;
	pObject->Type=PX_OBJECT_TYPE_SLIDERBAR;
	pObject->ReceiveEvents=PX_TRUE;
	pObject->Func_ObjectFree=PX_Object_SliderBarFree;
	pObject->Func_ObjectRender=PX_Object_SliderBarRender;
	pSliderbar->Max=100;
	pSliderbar->Value=0;
	pSliderbar->SliderButtonLength=16;
	pSliderbar->style=style;
	pSliderbar->status=PX_OBJECT_SLIDERBAR_STATUS_NORMAL;
	pSliderbar->Type=Type;
	pSliderbar->color=PX_COLOR(255,80,80,80);
	pSliderbar->BackgroundColor=PX_COLOR(255,255,255,255);
	pSliderbar->btnDownX=0;
	pSliderbar->btnDownY=0;
	pSliderbar->DargButtonX=0;
	pSliderbar->DargButtonY=0;

	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_CURSORDOWN,PX_Object_SliderBarOnMouseLButtonDown,PX_NULL);
	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_CURSORMOVE,PX_Object_SliderBarOnCursorNormal,PX_NULL);
	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_CURSORUP,PX_Object_SliderBarOnCursorNormal,PX_NULL);
	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_CURSORDRAG,PX_Object_SliderBarOnCursorDrag,PX_NULL);
	return pObject;
}

PX_Object_SliderBar * PX_Object_GetSliderBar( PX_Object *Object )
{
	if(Object->Type==PX_OBJECT_TYPE_SLIDERBAR)
		return (PX_Object_SliderBar *)Object->pObject;
	else
		return PX_NULL;
}

px_void PX_Object_SliderBarSetValue( PX_Object *pSliderBar,px_int Value )
{
	PX_Object_SliderBar *SliderBar=PX_Object_GetSliderBar(pSliderBar);
	if (SliderBar!=PX_NULL)
	{
		if (Value>SliderBar->Max)
		{
			Value=SliderBar->Max;
		}
		if (Value<SliderBar->Min)
		{
			Value=SliderBar->Min;
		}
		SliderBar->Value=Value;
	}
}

px_void PX_Object_SliderBarSetRange( PX_Object *pSliderBar,px_int Min,px_int Max )
{
	PX_Object_SliderBar *SliderBar=PX_Object_GetSliderBar(pSliderBar);
	if (SliderBar!=PX_NULL)
	{
		SliderBar->Min=Min;
		SliderBar->Max=Max;
	}
}

px_int PX_Object_SliderBarGetMax( PX_Object *pSliderBar )
{
	PX_Object_SliderBar *SliderBar=PX_Object_GetSliderBar(pSliderBar);
	if (SliderBar!=PX_NULL)
	{
		return SliderBar->Max;
	}
	return 0;
}

px_int PX_Object_SliderBarGetValue( PX_Object *pSliderBar )
{
	PX_Object_SliderBar *SliderBar=PX_Object_GetSliderBar(pSliderBar);
	if (SliderBar!=PX_NULL)
	{
		if (SliderBar->Value>SliderBar->Max)
		{
			SliderBar->Value=SliderBar->Max;
		}
		if (SliderBar->Value<SliderBar->Min)
		{
			SliderBar->Value=SliderBar->Min;
		}
		return SliderBar->Value;
	}
	return 0;
}

px_void PX_Object_SliderBarRender(px_surface *psurface, PX_Object *pObject,px_uint elpased)
{
	PX_Object_SliderBar *pSliderBar=PX_Object_GetSliderBar(pObject);
	PX_Object_Event e;
	px_int SliderBtnLen;
	px_float Sx,Sy;
	px_int Range;
	px_int relValue;
	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;

	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;

	if (pSliderBar==PX_NULL)
	{
		return;
	}

	if (objWidth<=10)
	{
		return;
	}

	if (objHeight<10)
	{
		return;
	}

	Range=pSliderBar->Max-pSliderBar->Min;
	SliderBtnLen=pSliderBar->SliderButtonLength;

	if (pSliderBar->Value>=pSliderBar->Max)
	{
		pSliderBar->Value=pSliderBar->Max;
	}

	if (pSliderBar->Value<pSliderBar->Min)
	{
		pSliderBar->Value=pSliderBar->Min;
	}

	if (pSliderBar->Value<0)
	{
		pSliderBar->Value=0;
	}

	//Clear
	PX_SurfaceClear(
		psurface,
		(px_int)objx,
		(px_int)objy,
		(px_int)objx+(px_int)objWidth-1,
		(px_int)objy+(px_int)objHeight-1,
		pSliderBar->BackgroundColor
		);

	switch(pSliderBar->status)
	{
	case PX_OBJECT_SLIDERBAR_STATUS_NORMAL:
		{
			if (pObject->OnFocus)
			{
				PX_ObjectClearFocus(pObject);
			}
			if (pSliderBar->Value>pSliderBar->Max)
			{
				pSliderBar->Value=pSliderBar->Max;
			}
			if (pSliderBar->Value<pSliderBar->Min)
			{
				pSliderBar->Value=pSliderBar->Min;
			}
			relValue=pSliderBar->Value-pSliderBar->Min;
			if(pSliderBar->Type==PX_OBJECT_SLIDERBAR_TYPE_HORIZONTAL)
			{
				switch(pSliderBar->style)
				{
				case PX_OBJECT_SLIDERBAR_STYLE_LINER:
					{
						Sx=(objWidth-pSliderBar->SliderButtonLength)*relValue/Range;
						pSliderBar->DargButtonX=Sx;
						Sx+=objx;
						Sy=objy;
						//Draw Line
						PX_GeoDrawRect(
							psurface,
							(px_int)objx,
							(px_int)objy+(px_int)objHeight/2,
							(px_int)objx+(px_int)objWidth-1,
							(px_int)objy+(px_int)objHeight/2,
							pSliderBar->color
							);
						//Draw Slider button
						PX_GeoDrawRect(psurface,(px_int)Sx,(px_int)Sy,(px_int)(Sx+SliderBtnLen-1),(px_int)(Sy+objHeight-1),pSliderBar->color);
					}
					break;
				case PX_OBJECT_SLIDERBAR_STYLE_BOX:
					{
						//draw border
						PX_GeoDrawBorder(psurface,(px_int)objx,(px_int)objy,(px_int)objx+(px_int)objWidth-1,(px_int)objy+(px_int)objHeight-1,1,pSliderBar->color);
						Sx=((objWidth-pSliderBar->SliderButtonLength)*(1.0f)*relValue/Range);
						pSliderBar->DargButtonX=Sx;
						Sx+=SliderBtnLen/2;
						//draw slider bar
						PX_GeoDrawRect(psurface,(px_int)(objx+Sx-SliderBtnLen/2+2),(px_int)(objy+2),(px_int)(objx+Sx-SliderBtnLen/2+SliderBtnLen-3),(px_int)(objy+objHeight-3),pSliderBar->color);
					}
					break;
				}


			}

			if (pSliderBar->Type==PX_OBJECT_SLIDERBAR_TYPE_VERTICAL)
			{
				switch(pSliderBar->style)
				{
				case PX_OBJECT_SLIDERBAR_STYLE_LINER:
					{
						Sx=objx;
						Sy=(objHeight-pSliderBar->SliderButtonLength)*relValue/Range;
						pSliderBar->DargButtonY=Sy;
						Sy+=(px_int)objy;
						//Draw Line
						PX_GeoDrawRect(
							psurface,
							(px_int)objx+(px_int)objWidth/2,
							(px_int)objy,
							(px_int)objx+(px_int)objWidth/2,
							(px_int)objy+(px_int)objHeight-1,
							pSliderBar->color
							);
						//Draw Slider button
						PX_GeoDrawRect(psurface,(px_int)Sx,(px_int)Sy,(px_int)(Sx+objWidth-1),(px_int)(Sy+SliderBtnLen-1),pSliderBar->color);
					}
					break;
				case PX_OBJECT_SLIDERBAR_STYLE_BOX:
					{
						//draw border
						PX_GeoDrawBorder(psurface,(px_int)objx,(px_int)objy,(px_int)objx+(px_int)objWidth-1,(px_int)objy+(px_int)objHeight-1,1,pSliderBar->color);
						Sy=((objHeight-pSliderBar->SliderButtonLength)*(1.0f)*relValue/Range);
						pSliderBar->DargButtonY=Sy;
						Sy+=SliderBtnLen/2;
						//draw slider bar
						PX_GeoDrawRect(psurface,(px_int)objx+2,(px_int)(objy+Sy-SliderBtnLen/2+2),(px_int)(objx+objWidth-1-2),(px_int)(objy+Sy+SliderBtnLen/2-3),pSliderBar->color);
					}
					break;
				}

			}
		}
		break;
	case PX_OBJECT_SLIDERBAR_STATUS_ONDRAG:
		{
			if (!pObject->OnFocus)
			{
				PX_ObjectSetFocus(pObject);
			}
			if(pSliderBar->Type==PX_OBJECT_SLIDERBAR_TYPE_HORIZONTAL)
			{
				switch(pSliderBar->style)
				{
				case PX_OBJECT_SLIDERBAR_STYLE_LINER:
					{
						Sx=pSliderBar->DargButtonX;
						Sy=objy;

						pSliderBar->Value=(px_int)(pSliderBar->Min+Range*(Sx)/(objWidth-pSliderBar->SliderButtonLength));
						if (pSliderBar->Value>pSliderBar->Max)
						{
							pSliderBar->Value=pSliderBar->Max;
						}
						if (pSliderBar->Value<pSliderBar->Min)
						{
							pSliderBar->Value=pSliderBar->Min;
						}
						//Draw Line
						PX_GeoDrawRect(
							psurface,
							(px_int)objx,
							(px_int)objy+(px_int)objHeight/2,
							(px_int)objx+(px_int)objWidth-1,
							(px_int)objy+(px_int)objHeight/2,
							pSliderBar->color
							);
						//Draw Slider button
						PX_GeoDrawRect(psurface,(px_int)(objx+Sx),(px_int)Sy,(px_int)(objx+Sx+SliderBtnLen-1),(px_int)(Sy+objHeight-1),pSliderBar->color);
					}
					break;
				case PX_OBJECT_SLIDERBAR_STYLE_BOX:
					{
						//draw border
						PX_GeoDrawBorder(psurface,(px_int)objx,(px_int)objy,(px_int)objx+(px_int)objWidth-1,(px_int)objy+(px_int)objHeight-1,1,pSliderBar->color);

						Sx=pSliderBar->DargButtonX;

						pSliderBar->Value=(px_int)(pSliderBar->Min+Range*(Sx)/(objWidth-pSliderBar->SliderButtonLength));
						if (pSliderBar->Value>pSliderBar->Max)
						{
							pSliderBar->Value=pSliderBar->Max;
						}
						if (pSliderBar->Value<pSliderBar->Min)
						{
							pSliderBar->Value=pSliderBar->Min;
						}
						Sx+=SliderBtnLen/2;
						//draw slider bar
						PX_GeoDrawRect(psurface,(px_int)(objx+Sx-SliderBtnLen/2+2),(px_int)objy+2,(px_int)(objx+Sx+SliderBtnLen/2-3),(px_int)(objy+objHeight-3),pSliderBar->color);
					}
					break;
				}


			}

			if (pSliderBar->Type==PX_OBJECT_SLIDERBAR_TYPE_VERTICAL)
			{
				switch(pSliderBar->style)
				{
				case PX_OBJECT_SLIDERBAR_STYLE_LINER:
					{
						pSliderBar->Value=(px_int)(pSliderBar->Min+Range*(pSliderBar->DargButtonY)/(objHeight-pSliderBar->SliderButtonLength));
						if (pSliderBar->Value>pSliderBar->Max)
						{
							pSliderBar->Value=pSliderBar->Max;
						}
						if (pSliderBar->Value<pSliderBar->Min)
						{
							pSliderBar->Value=pSliderBar->Min;
						}

						Sy=pSliderBar->DargButtonY+objy;
						Sx=objx;

						

						//Draw Line
						PX_GeoDrawRect(
							psurface,
							(px_int)objx+(px_int)objWidth/2,
							(px_int)objy,
							(px_int)objx+(px_int)objWidth/2,
							(px_int)objy+(px_int)objHeight-1,
							pSliderBar->color
							);
						//Draw Slider button
						PX_GeoDrawRect(psurface,(px_int)Sx,(px_int)(Sy),(px_int)(Sx+objWidth-1),(px_int)(Sy+SliderBtnLen-1),pSliderBar->color);
					}
					break;
				case PX_OBJECT_SLIDERBAR_STYLE_BOX:
					{
						//draw border
						PX_GeoDrawBorder(psurface,(px_int)objx,(px_int)objy,(px_int)objx+(px_int)objWidth-1,(px_int)objy+(px_int)objHeight-1,1,pSliderBar->color);
						
						pSliderBar->Value=(px_int)(pSliderBar->Min+Range*(pSliderBar->DargButtonY)/(objHeight-pSliderBar->SliderButtonLength));
						if (pSliderBar->Value>pSliderBar->Max)
						{
							pSliderBar->Value=pSliderBar->Max;
						}
						if (pSliderBar->Value<pSliderBar->Min)
						{
							pSliderBar->Value=pSliderBar->Min;
						}

						Sy=pSliderBar->DargButtonY;
						Sx=objx;
						Sy+=SliderBtnLen/2;
						

						PX_GeoDrawRect(psurface,(px_int)objx+2,(px_int)(objy+Sy-SliderBtnLen/2+2),(px_int)(objx+objWidth-1-2),(px_int)(objy+Sy+SliderBtnLen/2-3),pSliderBar->color);
					}
					break;
				}

			}
		}
		break;
	}

	if (pSliderBar->Value>=pSliderBar->Max)
	{
		pSliderBar->Value=pSliderBar->Max;
	}

	if (pSliderBar->Value<0)
	{
		pSliderBar->Value=0;
	}


	if (pSliderBar->lastValue!=pSliderBar->Value)
	{
		e.Event=PX_OBJECT_EVENT_VALUECHANGED;
		e.Param_uint[0]=0;
		e.Param_uint[1]=0;
		e.Param_uint[2]=0;
		e.Param_uint[3]=0;
		pSliderBar->lastValue=pSliderBar->Value;
		PX_ObjectExecuteEvent(pObject,e);
	}
}


px_void PX_Object_SliderBarSetBackgroundColor(PX_Object *pSliderBar,px_color color)
{
	PX_Object_SliderBar *SliderBar=PX_Object_GetSliderBar(pSliderBar);
	if (SliderBar!=PX_NULL)
	{
		SliderBar->BackgroundColor=color;
	}
}


px_void PX_Object_SliderBarFree( PX_Object *pSliderBar )
{

}

px_void PX_Object_SliderBarSetColor(PX_Object *pSliderBar,px_color color)
{
	PX_Object_SliderBar *SliderBar=PX_Object_GetSliderBar(pSliderBar);
	if (SliderBar!=PX_NULL)
	{
		SliderBar->color=color;
	}
}

px_void PX_Object_SliderBarSetSliderButtonLength(PX_Object *pSliderBar,px_int length)
{
	PX_Object_SliderBar *SliderBar=PX_Object_GetSliderBar(pSliderBar);
	if (SliderBar!=PX_NULL)
	{
		SliderBar->SliderButtonLength=length;
	}
}







px_void PX_Object_ImageFreeWithTexture(PX_Object *pBitmap)
{
	PX_Object_Image *pImage;
	pImage=PX_Object_GetImage(pBitmap);
	if (pImage)
	{
		PX_TextureFree(pImage->pTexture);
	}
}






px_void PX_ObjectSetPosition( PX_Object *Object,px_float x,px_float y,px_float z)
{
	if (Object==PX_NULL)
	{
		PX_ASSERT();
		return;
	}

	Object->x=x;
	Object->y=y;
	Object->z=z;
}




px_void PX_ObjectSetSize( PX_Object *Object,px_float Width,px_float Height,px_float length)
{
	if (Object!=PX_NULL)
	{
		Object->Width=Width;
		Object->Height=Height;

	}
	else
	{
		PX_ASSERT();
	}
}


px_void PX_Object_PushButtonOnMouseMove(PX_Object *Object,PX_Object_Event e,px_void *user_ptr)
{
	PX_Object_PushButton *pPushButton=PX_Object_GetPushButton(Object);
	px_float x,y;

	x=PX_Object_Event_GetCursorX(e);
	y=PX_Object_Event_GetCursorY(e);

	if (pPushButton)
	{
		if(PX_ObjectIsPointInRegion(Object,(px_float)x,(px_float)y))
		{
			if (pPushButton->state!=PX_OBJECT_BUTTON_STATE_ONPUSH)
			{
				if (pPushButton->state!=PX_OBJECT_BUTTON_STATE_ONCURSOR)
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
				pPushButton->state=PX_OBJECT_BUTTON_STATE_ONCURSOR;
			}
		}
		else
		{
			if (pPushButton->state!=PX_OBJECT_BUTTON_STATE_NORMAL)
			{
				PX_Object_Event e;
				e.Event=PX_OBJECT_EVENT_CURSOROUT;
				e.Param_uint[0]=0;
				e.Param_uint[1]=0;
				e.Param_uint[2]=0;
				e.Param_uint[3]=0;
				PX_ObjectExecuteEvent(Object,e);
			}
			pPushButton->state=PX_OBJECT_BUTTON_STATE_NORMAL;
		}
	}
}

px_void PX_Object_PushButtonOnMouseLButtonDown(PX_Object *Object,PX_Object_Event e,px_void *user_ptr)
{
	PX_Object_PushButton *pPushButton=PX_Object_GetPushButton(Object);
	px_float x,y;

	x=PX_Object_Event_GetCursorX(e);
	y=PX_Object_Event_GetCursorY(e);

	if (pPushButton)
	{
		if(PX_ObjectIsPointInRegion(Object,(px_float)x,(px_float)y))
			pPushButton->state=PX_OBJECT_BUTTON_STATE_ONPUSH;
		else
		{
			if (pPushButton->state!=PX_OBJECT_BUTTON_STATE_NORMAL)
			{
				PX_Object_Event e;
				e.Event=PX_OBJECT_EVENT_CURSOROUT;
				e.Param_uint[0]=0;
				e.Param_uint[1]=0;
				e.Param_uint[2]=0;
				e.Param_uint[3]=0;
				PX_ObjectExecuteEvent(Object,e);
			}
			pPushButton->state=PX_OBJECT_BUTTON_STATE_NORMAL;
		}
	}
}

px_void PX_Object_PushButtonOnMouseLButtonUp(PX_Object *Object,PX_Object_Event e,px_void *user_ptr)
{
	PX_Object_PushButton *pPushButton=PX_Object_GetPushButton(Object);
	px_float x,y;

	x=PX_Object_Event_GetCursorX(e);
	y=PX_Object_Event_GetCursorY(e);

	if (pPushButton)
	{
		if(PX_ObjectIsPointInRegion(Object,(px_float)x,(px_float)y))
			if(pPushButton->state==PX_OBJECT_BUTTON_STATE_ONPUSH)
			{
				pPushButton->state=PX_OBJECT_BUTTON_STATE_ONCURSOR;
				e.Event=PX_OBJECT_EVENT_EXECUTE;
				PX_ObjectExecuteEvent(Object,e);
			}
	}
}



PX_Object * PX_Object_PushButtonCreate(px_memorypool *mp,PX_Object *Parent,px_int x,px_int y,px_int Width,px_int Height,const px_char *Text,PX_FontModule *fontmodule,px_color Color)
{
	px_int TextLen;
	PX_Object *pObject;
	PX_Object_PushButton *pPushButton=(PX_Object_PushButton *)MP_Malloc(mp,sizeof(PX_Object_PushButton));
	if (pPushButton==PX_NULL)
	{
		return PX_NULL;
	}
	pObject=PX_ObjectCreate(mp,Parent,(px_float)x,(px_float)y,0,(px_float)Width,(px_float)Height,0);

	if (pObject==PX_NULL)
	{
		return PX_NULL;
	}

	if (!pObject)
	{
		MP_Free(mp,pPushButton);
	}
	pObject->pObject=pPushButton;
	pObject->Type=PX_OBJECT_TYPE_PUSHBUTTON;
	pObject->ReceiveEvents=PX_TRUE;
	pObject->Func_ObjectFree=PX_Object_PushButtonFree;
	pObject->Func_ObjectRender=PX_Object_PushButtonRender;

	TextLen=PX_strlen(Text)+1;

	pPushButton->Text=(px_char *)MP_Malloc(mp,TextLen);
	
	if (pPushButton->Text==PX_NULL)
	{
		MP_Free(pObject->mp,pObject);
		MP_Free(pObject->mp,pPushButton);
		return PX_NULL;
	}

	PX_strcpy(pPushButton->Text,Text,TextLen);

	pPushButton->TextColor=Color;
	pPushButton->CursorColor=PX_COLOR(255,192,192,192);
	pPushButton->PushColor=PX_COLOR(255,128,128,128);
	pPushButton->BackgroundColor=PX_COLOR(255,255,255,255);
	pPushButton->BorderColor=PX_COLOR(255,0,0,0);
	pPushButton->Border=PX_TRUE;
	pPushButton->state=PX_OBJECT_BUTTON_STATE_NORMAL;
	pPushButton->Border=PX_TRUE;
	pPushButton->style=PX_OBJECT_PUSHBUTTON_STYLE_RECT;
	pPushButton->roundradius=PX_OBJECT_PUSHBUTTON_ROUNDRADIUS;
	pPushButton->shape=PX_NULL;
	pPushButton->Texture=PX_NULL;
	pPushButton->fontModule=fontmodule;

	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_CURSORMOVE,PX_Object_PushButtonOnMouseMove,PX_NULL);
	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_CURSORDRAG,PX_Object_PushButtonOnMouseMove,PX_NULL);
	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_CURSORDOWN,PX_Object_PushButtonOnMouseLButtonDown,PX_NULL);
	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_CURSORUP,PX_Object_PushButtonOnMouseLButtonUp,PX_NULL);

	return pObject;
}

PX_Object_PushButton  * PX_Object_GetPushButton( PX_Object *Object )
{
	if(Object->Type==PX_OBJECT_TYPE_PUSHBUTTON)
		return (PX_Object_PushButton *)Object->pObject;
	else
		return PX_NULL;
}

px_char	  * PX_Object_PushButtonGetText( PX_Object *PushButton )
{
	PX_Object_PushButton *pPushButton=PX_Object_GetPushButton(PushButton);
	if (pPushButton!=PX_NULL)
	{
		return pPushButton->Text;
	}
	return PX_NULL;
}

px_void PX_Object_PushButtonSetText( PX_Object *pObject,const px_char *Text )
{
	PX_Object_PushButton *pPushButton;
	px_int TextLen;
	if (pObject==PX_NULL||Text==PX_NULL)
	{
		return;
	}
	if (pObject->Type!=PX_OBJECT_TYPE_PUSHBUTTON)
	{
		return;
	}
	
	TextLen=PX_strlen(Text);
	pPushButton=(PX_Object_PushButton *)pObject->pObject;

	if (TextLen>PX_strlen(pPushButton->Text))
	{
		MP_Free(pObject->mp,pPushButton->Text);

		pPushButton->Text=(px_char *)MP_Malloc(pObject->mp,TextLen+1);
		if (!pPushButton->Text)
		{
			return;
		}
	}
	PX_strcpy(pPushButton->Text,Text,TextLen+1);
	
}


px_void PX_Object_PushButtonSetBackgroundColor( PX_Object *pObject,px_color Color )
{
	PX_Object_PushButton * pPushButton=PX_Object_GetPushButton(pObject);
	if (pPushButton)
	{
		pPushButton->BackgroundColor=Color;
	}
}

px_void PX_Object_PushButtonSetTextColor( PX_Object *pObject,px_color Color )
{
	PX_Object_PushButton * pPushButton=PX_Object_GetPushButton(pObject);
	if (pPushButton)
	{
		
		pPushButton->TextColor=Color;
	}
}



px_void PX_Object_PushButtonSetTexture(PX_Object *pObject,px_texture *texture)
{
	PX_Object_PushButton * pPushButton=PX_Object_GetPushButton(pObject);
	if (pPushButton)
	{
		pPushButton->Texture=texture;
	}
}


px_void PX_Object_PushButtonSetShape(PX_Object *pObject,px_shape *pshape)
{
	PX_Object_PushButton * pPushButton=PX_Object_GetPushButton(pObject);
	if (pPushButton)
	{
		pPushButton->shape=pshape;
	}
}

px_void PX_Object_PushButtonSetBorderColor( PX_Object *pObject,px_color Color )
{
	PX_Object_PushButton * pPushButton=PX_Object_GetPushButton(pObject);
	if (pPushButton)
	{
		
		pPushButton->BorderColor=Color;
	}
}

px_void PX_Object_PushButtonSetCursorColor(PX_Object *pObject,px_color Color)
{
	PX_Object_PushButton * pPushButton=PX_Object_GetPushButton(pObject);
	if (pPushButton)
	{
		
		pPushButton->CursorColor=Color;
	}
}


px_void PX_Object_PushButtonSetStyle(PX_Object *pObject,PX_OBJECT_PUSHBUTTON_STYLE style)
{
	PX_Object_PushButton * pPushButton=PX_Object_GetPushButton(pObject);
	if (pPushButton)
	{
		pPushButton->style=style;
	}
}

px_void PX_Object_PushButtonSetPushColor(PX_Object *pObject,px_color Color)
{
	PX_Object_PushButton * pPushButton=PX_Object_GetPushButton(pObject);
	if (pPushButton)
	{
		
		pPushButton->PushColor=Color;
	}
}


px_void PX_Object_PushButtonSetBorder( PX_Object *Object,px_bool Border )
{
	PX_Object_PushButton *PushButton=PX_Object_GetPushButton(Object);
	if (PushButton)
	{
		
		PushButton->Border=Border;
	}
}

px_void PX_Object_PushButtonRender(px_surface *psurface, PX_Object *pObject,px_uint elpased)
{
	px_int fx,fy;
	PX_Object_PushButton *pPushButton=PX_Object_GetPushButton(pObject);
	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;

	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;

	if (pPushButton==PX_NULL)
	{
		return;
	}

	if (!pObject->Visible)
	{
		return;
	}

	switch(pPushButton->style)
	{
	case PX_OBJECT_PUSHBUTTON_STYLE_RECT:
		PX_GeoDrawRect(psurface,(px_int)objx,(px_int)objy,(px_int)objx+(px_int)objWidth-1,(px_int)objy+(px_int)objHeight-1,pPushButton->BackgroundColor);
		switch (pPushButton->state)
		{
		case PX_OBJECT_BUTTON_STATE_NORMAL:
			PX_GeoDrawRect(psurface,(px_int)objx,(px_int)objy,(px_int)objx+(px_int)objWidth-1,(px_int)objy+(px_int)objHeight-1,pPushButton->BackgroundColor);
			break;
		case PX_OBJECT_BUTTON_STATE_ONPUSH:
			PX_GeoDrawRect(psurface,(px_int)objx,(px_int)objy,(px_int)objx+(px_int)objWidth-1,(px_int)objy+(px_int)objHeight-1,pPushButton->PushColor);
			break;
		case PX_OBJECT_BUTTON_STATE_ONCURSOR:
			PX_GeoDrawRect(psurface,(px_int)objx,(px_int)objy,(px_int)objx+(px_int)objWidth-1,(px_int)objy+(px_int)objHeight-1,pPushButton->CursorColor);
			break;
		}
		break;
	case PX_OBJECT_PUSHBUTTON_STYLE_ROUNDRECT:
		PX_GeoDrawSolidRoundRect(psurface,(px_int)objx,(px_int)objy,(px_int)objx+(px_int)objWidth-1,(px_int)objy+(px_int)objHeight-1,pPushButton->roundradius,pPushButton->BackgroundColor);
		switch (pPushButton->state)
		{
		case PX_OBJECT_BUTTON_STATE_NORMAL:
			PX_GeoDrawSolidRoundRect(psurface,(px_int)objx,(px_int)objy,(px_int)objx+(px_int)objWidth-1,(px_int)objy+(px_int)objHeight-1,pPushButton->roundradius,pPushButton->BackgroundColor);
			break;
		case PX_OBJECT_BUTTON_STATE_ONPUSH:
			PX_GeoDrawSolidRoundRect(psurface,(px_int)objx,(px_int)objy,(px_int)objx+(px_int)objWidth-1,(px_int)objy+(px_int)objHeight-1,pPushButton->roundradius,pPushButton->PushColor);
			break;
		case PX_OBJECT_BUTTON_STATE_ONCURSOR:
			PX_GeoDrawSolidRoundRect(psurface,(px_int)objx,(px_int)objy,(px_int)objx+(px_int)objWidth-1,(px_int)objy+(px_int)objHeight-1,pPushButton->roundradius,pPushButton->CursorColor);
			break;
		}
		break;
	}
	

	
	
	if (pPushButton->Texture)
	{
		PX_TextureRender(psurface,pPushButton->Texture,(px_int)objx+(px_int)objWidth/2,(px_int)objy+(px_int)objHeight/2,PX_TEXTURERENDER_REFPOINT_CENTER,PX_NULL);
	}
	else if (pPushButton->shape)
	{
		PX_ShapeRender(psurface,pPushButton->shape,(px_int)objx+(px_int)objWidth/2,(px_int)objy+(px_int)objHeight/2,PX_TEXTURERENDER_REFPOINT_CENTER,pPushButton->TextColor);
	}

	if (pPushButton->Border)
	{
		switch(pPushButton->style)
		{
		case PX_OBJECT_PUSHBUTTON_STYLE_RECT:
			PX_GeoDrawBorder(psurface,(px_int)objx,(px_int)objy,(px_int)objx+(px_int)objWidth-1,(px_int)objy+(px_int)objHeight-1,1,pPushButton->BorderColor);
			break;
		case PX_OBJECT_PUSHBUTTON_STYLE_ROUNDRECT:
			PX_GeoDrawRoundRect(psurface,(px_int)objx,(px_int)objy,(px_int)objx+(px_int)objWidth-1,(px_int)objy+(px_int)objHeight-1,pPushButton->roundradius,0.6f,pPushButton->BorderColor);
			break;
		}
	}

	fx=(px_int)(objx+objWidth/2);
	fy=(px_int)(objy+objHeight/2);

	PX_FontModuleDrawText(psurface,pPushButton->fontModule,fx,fy,PX_ALIGN_CENTER,pPushButton->Text,pPushButton->TextColor);

	

}

px_void PX_Object_PushButtonFree( PX_Object *Obj )
{
	PX_Object_PushButton *PushButton=PX_Object_GetPushButton(Obj);
	if (Obj!=PX_NULL)
	{
		MP_Free(Obj->mp,PushButton->Text);
	}
}



px_void PX_Object_EditOnMouseMove(PX_Object *Object,PX_Object_Event e,px_void *user_ptr)
{
	PX_Object_Edit *pEdit=PX_Object_GetEdit(Object);
	px_float x,y;
	x=PX_Object_Event_GetCursorX(e);
	y=PX_Object_Event_GetCursorY(e);

	if (pEdit)
	{
		if(PX_ObjectIsPointInRegion(Object,(px_float)x,(px_float)y))
		{
			pEdit->state=PX_OBJECT_EDIT_STATE_ONCURSOR;
		}
		else
		{
			pEdit->state=PX_OBJECT_EDIT_STATE_NORMAL;
		}
	}
}

px_void PX_Object_EditUpdateCursorOnDown(PX_Object *pObject,px_int cx,px_int cy);
px_void PX_Object_EditOnMouseLButtonDown(PX_Object *Object,PX_Object_Event e,px_void *user_ptr)
{
	PX_Object_Edit *pEdit=PX_Object_GetEdit(Object);
	px_float x,y;
	px_float objx,objy;
	px_float inheritX,inheritY;

	PX_ObjectGetInheritXY(Object,&inheritX,&inheritY);

	objx=(Object->x+inheritX);
	objy=(Object->y+inheritY);

	x=PX_Object_Event_GetCursorX(e);
	y=PX_Object_Event_GetCursorY(e);

	if (pEdit)
	{
		if(PX_ObjectIsPointInRegion(Object,(px_float)x,(px_float)y))
		{
			PX_Object_EditSetFocus(Object,PX_TRUE);
			PX_Object_EditUpdateCursorOnDown(Object,(px_int)(x-objx),(px_int)(y-objy));
		}
		else
			PX_Object_EditSetFocus(Object,PX_FALSE);
	}
}

px_void PX_Object_EditOnKeyboardString(PX_Object *Object,PX_Object_Event e,px_void *user_ptr)
{
	PX_Object_Edit *pEdit=PX_Object_GetEdit(Object);

	if (pEdit->onFocus)
	{
		PX_Object_EditAddString(Object,PX_Object_Event_GetStringPtr(e));
	}
}

static px_void PX_Object_EditCheckCursor(PX_Object_Edit*pedit)
{
	if (pedit->cursor_index<0)
	{
		pedit->cursor_index=0;
	}
	if (pedit->cursor_index>PX_strlen(pedit->text.buffer))
	{
		pedit->cursor_index=PX_strlen(pedit->text.buffer);
	}
}

PX_Object* PX_Object_EditCreate(px_memorypool *mp, PX_Object *Parent,px_int x,px_int y,px_int Width,px_int Height,PX_FontModule *fontModule,px_color TextColor )
{
	PX_Object *pObject;
	PX_Object_Edit *pEdit=(PX_Object_Edit *)MP_Malloc(mp,sizeof(PX_Object_Edit));
	if (pEdit==PX_NULL)
	{
		return PX_NULL;
	}
	pObject=PX_ObjectCreate(mp,Parent,(px_float)x,(px_float)y,0,(px_float)Width,(px_float)Height,0);

	if (pObject==PX_NULL)
	{
		return PX_NULL;
	}

	if (!pObject)
	{
		MP_Free(mp,pEdit);
		return PX_NULL;
	}

	if(!PX_SurfaceCreate(mp,Width,Height,&pEdit->EditSurface))
	{
		MP_Free(mp,pEdit);
		MP_Free(mp,pObject);
		return PX_NULL;
	}

	pObject->pObject=pEdit;
	pObject->Type=PX_OBJECT_TYPE_EDIT;
	pObject->ReceiveEvents=PX_TRUE;
	pObject->Func_ObjectFree=PX_Object_EditFree;
	pObject->Func_ObjectRender=PX_Object_EditRender;



	PX_StringInitialize(mp,&pEdit->text);


	pEdit->TextColor=TextColor;

	pEdit->CursorColor=PX_COLOR(255,0,0,0);
	pEdit->BorderColor=PX_COLOR(255,0,0,0);
	pEdit->BackgroundColor=PX_COLOR(0,0,0,0);
	pEdit->XOffset=0;
	pEdit->YOffset=0;
	pEdit->Border=PX_TRUE;
	pEdit->Password=PX_FALSE;
	pEdit->onFocus=PX_FALSE;
	pEdit->state=PX_OBJECT_EDIT_STATE_NORMAL;
	pEdit->AutoNewline=PX_FALSE;
	pEdit->xFontSpacing=0;
	pEdit->yFontSpacing=0;
	pEdit->HorizontalOffset=3;
	pEdit->VerticalOffset=3;
	pEdit->cursor_index=0;
	pEdit->max_length=-1;
	pEdit->fontModule=fontModule;
	pEdit->AutoNewLineSpacing=__PX_FONT_ASCSIZE+2;
	pEdit->style=PX_OBJECT_EDIT_STYLE_RECT;
	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_CURSORMOVE,PX_Object_EditOnMouseMove,PX_NULL);
	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_CURSORDOWN,PX_Object_EditOnMouseLButtonDown,PX_NULL);
	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_STRING,PX_Object_EditOnKeyboardString,PX_NULL);
	
	return pObject;
}


PX_Object_Edit  * PX_Object_GetEdit( PX_Object *Object )
{
	if(Object->Type==PX_OBJECT_TYPE_EDIT)
		return (PX_Object_Edit *)Object->pObject;
	else
		return PX_NULL;
}

px_char	  * PX_Object_EditGetText( PX_Object *Object )
{
	PX_Object_Edit *pEdit=PX_Object_GetEdit(Object);
	if (pEdit!=PX_NULL)
	{
		return pEdit->text.buffer;
	}
	return PX_NULL;
}


px_void PX_Object_EditSetMaxTextLength(PX_Object *pObject,px_int max_length)
{
	PX_Object_Edit *pEdit=PX_Object_GetEdit(pObject);
	if (pEdit!=PX_NULL)
	{
		pEdit->max_length=max_length;
	}
}


px_void PX_Object_EditSetText( PX_Object *pObject,const px_char *Text )
{
	PX_Object_Edit *pEdit=PX_Object_GetEdit(pObject);
	if(pEdit)
	{
		PX_StringClear(&pEdit->text);
		PX_StringCat(&pEdit->text,Text);
		pEdit->cursor_index=PX_strlen(Text);
	}
}

px_void PX_Object_EditAppendText(PX_Object *pObject,const px_char *Text)
{
	PX_Object_Edit *pEdit=PX_Object_GetEdit(pObject);
	if(pEdit)
	{
		PX_StringCat(&pEdit->text,Text);
		pEdit->cursor_index=PX_strlen(Text);
	}
}

px_void PX_Object_EditSetFocus(PX_Object *pObject,px_bool OnFocus)
{
	PX_Object_Edit *pEdit=PX_Object_GetEdit(pObject);
	if(pEdit)
	{
		if (pEdit->onFocus!=OnFocus)
		{
			PX_Object_Event e;
			pEdit->onFocus=OnFocus;
			e.Event=PX_OBJECT_EVENT_ONFOCUSCHANGED;
			e.Param_bool[0]=OnFocus;
			PX_ObjectPostEvent(pObject,e);
		}
		
	}
}


px_void PX_Object_EditSetPasswordStyle( PX_Object *pLabel,px_uchar Enabled )
{
	PX_Object_Edit *pEdit=PX_Object_GetEdit(pLabel);
	if (pEdit)
	{
		pEdit->Password=Enabled;
		pEdit->fontModule=PX_NULL;
	}
}
px_void PX_Object_EditSetBackgroundColor( PX_Object *pObject,px_color Color )
{
	PX_Object_Edit * pEdit=PX_Object_GetEdit(pObject);
	if (pEdit)
	{
		pEdit->BackgroundColor=Color;
	}
}

px_void PX_Object_EditSetBorderColor( PX_Object *pObject,px_color Color )
{
	PX_Object_Edit * pEdit=PX_Object_GetEdit(pObject);
	if (pEdit)
	{
		
		pEdit->BorderColor=Color;
	}
}


px_void PX_Object_EditSetCursorColor( PX_Object *pObject,px_color Color )
{
	PX_Object_Edit * pEdit=PX_Object_GetEdit(pObject);
	if (pEdit)
	{
		
		pEdit->CursorColor=Color;
	}
}


px_void PX_Object_EditSetTextColor(PX_Object *pObject,px_color Color)
{
	PX_Object_Edit * pEdit=PX_Object_GetEdit(pObject);
	if (pEdit)
	{
		pEdit->TextColor=Color;
	}
}



px_void PX_Object_EditSetLimit(PX_Object *pObject,const px_char *Limit)
{
	PX_Object_Edit * pEdit=PX_Object_GetEdit(pObject);
	if (pEdit)
	{
		PX_strcpy(pEdit->Limit,Limit,sizeof(pEdit->Limit));
	}
}

px_void PX_Object_EditSetStyle(PX_Object *pObject,PX_OBJECT_EDIT_STYLE style)
{
	PX_Object_Edit * pEdit=PX_Object_GetEdit(pObject);
	if (pEdit)
	{
		pEdit->style=style;
	}
}

px_void PX_Object_EditSetBorder( PX_Object *pObj,px_bool Border )
{
	PX_Object_Edit *pEdit=PX_Object_GetEdit(pObj);
	if (pEdit)
	{
		
		pEdit->Border=Border;
	}
}

px_void PX_Object_EditGetCursorXY(PX_Object *pObject,px_int *cx,px_int *cy,px_int *height)
{
	px_int x=0,y=0,cursor=0,fsize=0;
	PX_Object_Edit *pEdit=(PX_Object_Edit *)pObject->pObject;
	const px_char *Text=pEdit->text.buffer;
	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;

	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;


	if (pEdit==PX_NULL)
	{
		return;
	}

	x=pEdit->HorizontalOffset;
	y=pEdit->VerticalOffset;

	

	while (PX_TRUE)
	{
		fsize=0;

		if (cursor==pEdit->cursor_index)
		{
			*cx=x;
			*cy=y;
			*height=pEdit->fontModule?pEdit->fontModule->max_Height:__PX_FONT_HEIGHT;
			return;
		}

		if (pEdit->fontModule)
		{
			px_dword code;
			px_int width,height;
			fsize=PX_FontModuleGetCharacterDesc(pEdit->fontModule,Text+cursor,&code,&width,&height);
			if (!fsize)
			{
				break;
			}
			if (code=='\r')
			{
				//skip
			}else if (code=='\n')
			{
				x=0;
				y+=pEdit->fontModule->max_Height+pEdit->yFontSpacing;
			}
			else
			{
				x+=width+pEdit->xFontSpacing;
			}

			if (pEdit->AutoNewline)
			{
				if (x>objWidth-pEdit->AutoNewLineSpacing)
				{
					x=0;
					y+=pEdit->fontModule->max_Height+pEdit->yFontSpacing;
				}
			}

		}
		else
		{
			fsize=1;

			if (Text[cursor]=='\r')
			{
				//skip
			}else if (Text[cursor]=='\n')
			{
				x=0;
				y+=__PX_FONT_HEIGHT+pEdit->yFontSpacing;
			}
			else if(Text[cursor])
			{
				x+=__PX_FONT_ASCSIZE+pEdit->xFontSpacing;
			}
			else
			{
				break;
			}

			if (pEdit->AutoNewline)
			{
				if (x>objWidth-pEdit->AutoNewLineSpacing)
				{
					x=0;
					y+=__PX_FONT_HEIGHT+pEdit->yFontSpacing;
				}
			}
		}
		cursor+=fsize;
	}

	*cx=x;
	*cy=y;
	*height=pEdit->fontModule?pEdit->fontModule->max_Height:__PX_FONT_HEIGHT;
}



px_void PX_Object_EditUpdateCursorOnDown(PX_Object *pObject,px_int cx,px_int cy)
{
	px_int x_draw_oft=0,y_draw_oft=0,x=0,y=0,cursor=0,fsize=0;
	PX_Object_Edit *pEdit=(PX_Object_Edit *)pObject->pObject;
	const px_char *Text=pEdit->text.buffer;
	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;

	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;

	if (pEdit==PX_NULL)
	{
		return;
	}

	x=pEdit->HorizontalOffset;
	y=pEdit->VerticalOffset;

	

	while (PX_TRUE)
	{
		fsize=0;
		x_draw_oft=x-pEdit->XOffset;
		y_draw_oft=y-pEdit->YOffset;

		if (pEdit->fontModule)
		{
			px_dword code;
			px_int width,height;
			fsize=PX_FontModuleGetCharacterDesc(pEdit->fontModule,Text+cursor,&code,&width,&height);
			if (!fsize)
			{
				break;
			}
			if (code=='\r')
			{
				//skip
			}else if (code=='\n')
			{
				x=0;
				y+=pEdit->fontModule->max_Height+pEdit->yFontSpacing;
			}
			else
			{
				if (cx<=x_draw_oft+width/2+pEdit->xFontSpacing/2&&cy<=y_draw_oft+pEdit->fontModule->max_Height+pEdit->yFontSpacing/2)
				{
					pEdit->cursor_index=cursor;
					return;
				}
				else
				{
					pEdit->cursor_index=cursor+fsize;
				}
	
				x+=width+pEdit->xFontSpacing;
			}

			if (pEdit->AutoNewline)
			{
				if (x>objWidth-pEdit->AutoNewLineSpacing)
				{
					x=pEdit->HorizontalOffset;
					y+=pEdit->fontModule->max_Height+pEdit->yFontSpacing;
				}
			}

		}
		else
		{
			fsize=1;

			if (Text[cursor]=='\r')
			{
				//skip
			}else if (Text[cursor]=='\n')
			{
				x=0;
				y+=__PX_FONT_HEIGHT+pEdit->yFontSpacing;
			}
			else if(Text[cursor])
			{
				if (cx<=x_draw_oft+__PX_FONT_ASCSIZE/2+pEdit->xFontSpacing/2&&cy<=y_draw_oft+__PX_FONT_HEIGHT+pEdit->yFontSpacing/2)
				{
					pEdit->cursor_index=cursor;
					return;
				}
				else
				{
					pEdit->cursor_index=cursor+fsize;
				}

				x+=__PX_FONT_ASCSIZE+pEdit->xFontSpacing;
			}
			else
			{
				break;
			}

			if (pEdit->AutoNewline)
			{
				if (x>objWidth-pEdit->AutoNewLineSpacing)
				{
					x=pEdit->HorizontalOffset;
					y+=__PX_FONT_HEIGHT+pEdit->yFontSpacing;
				}
			}
		}
		cursor+=fsize;
	}

}

px_void PX_Object_EditUpdateCursorViewRegion(PX_Object *pObject)
{
	px_int cursorX,cursorY,cursorHeight;
	PX_Object_Edit *pEdit=(PX_Object_Edit *)pObject->pObject;
	px_float objWidth,objHeight;
	objWidth=pObject->Width;
	objHeight=pObject->Height;

	if (pEdit==PX_NULL)
	{
		return;
	}


	PX_Object_EditGetCursorXY(pObject,&cursorX,&cursorY,&cursorHeight);

	
	if (cursorX>objWidth-8)
	{
		pEdit->XOffset=cursorX-(px_int)objWidth+8;
	}else
	{
		pEdit->XOffset=0;
	}
	
	if (cursorY+cursorHeight>objHeight-2)
	{
		pEdit->YOffset=cursorY+cursorHeight-(px_int)objHeight+2;
	}else
	{
		pEdit->YOffset=0;
	}

}
px_void PX_Object_EditRender(px_surface *psurface, PX_Object *pObject,px_uint elpased)
{
	px_int x_draw_oft,y_draw_oft,x,y,cursor,fsize;
	PX_Object_Edit *pEdit=(PX_Object_Edit *)pObject->pObject;
	const px_char *Text=pEdit->text.buffer;
	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;

	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;

	if (pEdit==PX_NULL)
	{
		return;
	}

	if (!pObject->Visible)
	{
		return;
	}

	if (pEdit->EditSurface.width!=(px_int)objWidth||pEdit->EditSurface.height!=(px_int)objHeight)
	{
		PX_SurfaceFree(&pEdit->EditSurface);
		if(!PX_SurfaceCreate(pObject->mp,(px_int)objWidth,(px_int)objHeight,&pEdit->EditSurface))
		{
			return;
		}
	}

	
	if (pEdit->style==PX_OBJECT_EDIT_STYLE_RECT)
	{
		//clear
		PX_SurfaceClear(&pEdit->EditSurface,0,0,(px_int)objWidth-1,(px_int)objHeight-1,pEdit->BackgroundColor);
		if (pEdit->Border)
		{
			if(pEdit->state==PX_OBJECT_EDIT_STATE_NORMAL)
				PX_GeoDrawBorder(&pEdit->EditSurface,0,0,(px_int)objWidth-1,(px_int)objHeight-1,1,pEdit->BorderColor);
			else
				PX_GeoDrawBorder(&pEdit->EditSurface,0,0,(px_int)objWidth-1,(px_int)objHeight-1,1,pEdit->CursorColor);
		}
	}
	else if(pEdit->style==PX_OBJECT_EDIT_STYLE_ROUNDRECT)
	{
		PX_SurfaceClear(&pEdit->EditSurface,0,0,(px_int)objWidth-1,(px_int)objHeight-1,PX_COLOR(0,0,0,0));
		PX_GeoDrawSolidRoundRect(&pEdit->EditSurface,0,0,(px_int)objWidth-1,(px_int)objHeight-1,objHeight/2,pEdit->BackgroundColor);
		if (pEdit->Border)
		{
			if(pEdit->state==PX_OBJECT_EDIT_STATE_NORMAL)
				PX_GeoDrawRoundRect(&pEdit->EditSurface,0,0,(px_int)objWidth-1,(px_int)objHeight-1,objHeight/2,1,pEdit->BorderColor);
			else
				PX_GeoDrawRoundRect(&pEdit->EditSurface,0,0,(px_int)objWidth-1,(px_int)objHeight-1,objHeight/2,1,pEdit->CursorColor);
		}
	}



	x=pEdit->HorizontalOffset;
	y=pEdit->VerticalOffset;

	cursor=0;

	while (PX_TRUE)
	{
		fsize=0;
		x_draw_oft=x-pEdit->XOffset;
		y_draw_oft=y-pEdit->YOffset;

		//Draw Cursor
		if (pEdit->onFocus&&pEdit->cursor_index==cursor)
		{
			pEdit->elpased+=elpased;
			if ((pEdit->elpased/200)&1)
			{
				if (pEdit->fontModule)
				{
					PX_GeoDrawRect(&pEdit->EditSurface,x_draw_oft+1,y_draw_oft,x_draw_oft,y_draw_oft+pEdit->fontModule->max_Height-2,pEdit->CursorColor);
				}
				else
				{
					PX_GeoDrawRect(&pEdit->EditSurface,x_draw_oft+1,y_draw_oft,x_draw_oft,y_draw_oft+__PX_FONT_HEIGHT-1,pEdit->CursorColor);
				}
				
			}
		}


		if (pEdit->fontModule)
		{
			px_dword code;
			px_int width,height;
			fsize=PX_FontModuleGetCharacterDesc(pEdit->fontModule,Text+cursor,&code,&width,&height);
			if (!fsize)
			{
				break;
			}
			if (code=='\r')
			{
				//skip
			}else if (code=='\n')
			{
				x=pEdit->HorizontalOffset;
				y+=pEdit->fontModule->max_Height+pEdit->yFontSpacing;
			}
			else
			{
				PX_FontModuleDrawCharacter(&pEdit->EditSurface,pEdit->fontModule,x_draw_oft,y_draw_oft,code,pEdit->TextColor);
				x+=width+pEdit->xFontSpacing;
			}

			if (pEdit->AutoNewline)
			{
				if (x>objWidth-pEdit->AutoNewLineSpacing)
				{
					x=0;
					y+=pEdit->fontModule->max_Height+pEdit->yFontSpacing;
				}
			}

		}
		else
		{
			fsize=1;

			if (Text[cursor]=='\r')
			{
				//skip
			}else if (Text[cursor]=='\n')
			{
				x=0;
				y+=__PX_FONT_HEIGHT+pEdit->yFontSpacing;
			}
			else if(Text[cursor])
			{
				if (pEdit->Password)
				{
					PX_FontDrawChar(&pEdit->EditSurface,x_draw_oft,y_draw_oft,'*',pEdit->TextColor);
					x+=__PX_FONT_ASCSIZE;
				}
				else
				{
					PX_FontDrawChar(&pEdit->EditSurface,x_draw_oft,y_draw_oft,Text[cursor],pEdit->TextColor);
					x+=__PX_FONT_ASCSIZE;
				}
				
			}
			else
			{
				break;
			}

			if (pEdit->AutoNewline)
			{
				if (x>objWidth-pEdit->AutoNewLineSpacing)
				{
					x=pEdit->HorizontalOffset;
					y+=__PX_FONT_HEIGHT+pEdit->yFontSpacing;
				}
			}
		}
		cursor+=fsize;
	}

	PX_SurfaceRender(psurface,&pEdit->EditSurface,(px_int)objx,(px_int)objy,PX_TEXTURERENDER_REFPOINT_LEFTTOP,PX_NULL);
}


px_void PX_Object_EditFree( PX_Object *pObject )
{
	PX_Object_Edit *pEdit=PX_Object_GetEdit(pObject);
	if (pObject!=PX_NULL)
	{
		PX_StringFree(&pEdit->text);
		PX_TextureFree(&pEdit->EditSurface);
	}
}

px_void PX_Object_EditAddString(PX_Object *pObject,px_char *Text)
{
	PX_Object_Edit *pEdit=PX_Object_GetEdit(pObject);
	PX_Object_Event e;
	if (pObject!=PX_NULL)
	{
		PX_Object_EditCheckCursor(pEdit);
		while (*Text)
		{

			if(*Text!=8)
			{
				if (pEdit->Limit[0])
				{
					px_bool bMatch=PX_FALSE;
					const px_char *limitFilter=pEdit->Limit;
					while (*limitFilter)
					{
						if (*limitFilter==*Text)
						{
							bMatch=PX_TRUE;
							break;
						}
						limitFilter++;
					}
					if (!bMatch)
					{
						Text++;
						continue;
					}
				}

				if (pEdit->max_length>0)
				{
					if (PX_StringLen(&pEdit->text)<pEdit->max_length)
					{
						PX_StringInsertChar(&pEdit->text,pEdit->cursor_index,*Text);
						pEdit->cursor_index++;
					}
				}
				else
				{
					PX_StringInsertChar(&pEdit->text,pEdit->cursor_index,*Text);
					pEdit->cursor_index++;
				}

			}
			else
			{
				PX_Object_EditBackspace(pObject);
			}
			Text++;
		}
	}
	PX_Object_EditUpdateCursorViewRegion(pObject);
	e.Event=PX_OBJECT_EVENT_VALUECHANGED;
	e.Param_ptr[0]=0;
	PX_ObjectExecuteEvent(pObject,e);
}

px_void PX_Object_EditBackspace(PX_Object *pObject)
{
	PX_Object_Event e;
	PX_Object_Edit *pEdit=PX_Object_GetEdit(pObject);
	

	if (pObject!=PX_NULL&&pEdit->onFocus)
	{
		px_char *pText;
		pText=pEdit->text.buffer;

		if (!pEdit->fontModule)
		{
			if (pEdit->cursor_index)
			{
				PX_StringRemoveChar(&pEdit->text,pEdit->cursor_index-1);
				pEdit->cursor_index--;
			}
			
			return;
		}

		switch(pEdit->fontModule->codePage)
		{
		case PX_FONTMODULE_CODEPAGE_GBK:
			{
				px_int idx=0;
				px_int c=0;
				while (PX_TRUE)
				{
					
					if (idx==pEdit->cursor_index)
					{
						break;
					}

					if (pText[idx]==0)
					{
						break;
					}

					if (pText[idx]&0x80)
					{
						c=2;
						idx+=2;
					}
					else
					{
						idx++;
						c=1;
					}
				}

				while (c)
				{
					PX_StringRemoveChar(&pEdit->text,pEdit->cursor_index-1);
					pEdit->cursor_index--;
					c--;
				}
				
			}
			break;
		case PX_FONTMODULE_CODEPAGE_UTF8:
			{
				px_int i;
				for (i=0;i<6;i++)
				{
					if (pEdit->cursor_index==0)
					{
						break;
					}

					if ((pText[pEdit->cursor_index-1]&0x80)==0x00)
					{
						PX_StringRemoveChar(&pEdit->text,pEdit->cursor_index-1);
						pEdit->cursor_index--;
						break;
					}

					if ((pText[pEdit->cursor_index-1]&0xc0)==0x80)
					{
						PX_StringRemoveChar(&pEdit->text,pEdit->cursor_index-1);
						pEdit->cursor_index--;
						continue;
					}

					if ((pText[pEdit->cursor_index-1]&0xc0)==0xc0)
					{
						PX_StringRemoveChar(&pEdit->text,pEdit->cursor_index-1);
						pEdit->cursor_index--;
						break;
					}
					
				}
			}
			break;
		case PX_FONTMODULE_CODEPAGE_UTF16:
			{
				//not support
			}
			break;
		default:
			{
				//not support
			}
			break;
		}
		
		e.Event=PX_OBJECT_EVENT_VALUECHANGED;
		e.Param_ptr[0]=0;
		PX_ObjectExecuteEvent(pObject,e);
	}
}



px_void PX_Object_EditAutoNewLine(PX_Object *pObject,px_bool b,px_int AutoNewLineSpacing)
{
	PX_Object_Edit *pEdit=PX_Object_GetEdit(pObject);
	if (pObject!=PX_NULL)
	{
		pEdit->AutoNewline=b;
		pEdit->AutoNewLineSpacing=AutoNewLineSpacing;
	}
}



px_void PX_Object_EditSetOffset(PX_Object *pObject,px_int TopOffset,px_int LeftOffset)
{
	PX_Object_Edit *pEdit=PX_Object_GetEdit(pObject);
	if (pObject!=PX_NULL)
	{
		pEdit->VerticalOffset=TopOffset;
		pEdit->HorizontalOffset=LeftOffset;
	}
}



px_void PX_Object_ScrollArea_EventDispatcher(PX_Object *Object,PX_Object_Event e,px_void *user_ptr)
{
	PX_Object_ScrollArea *pSA=PX_Object_GetScrollArea(Object);
	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;

	PX_ObjectGetInheritXY(Object,&inheritX,&inheritY);

	objx=(Object->x+inheritX);
	objy=(Object->y+inheritY);
	objWidth=Object->Width;
	objHeight=Object->Height;

	if (!pSA)
	{
		return;
	}

	if (e.Event==PX_OBJECT_EVENT_EXECUTE||e.Event==PX_OBJECT_EVENT_CURSORDRAG||e.Event==PX_OBJECT_EVENT_CURSORDOWN||e.Event==PX_OBJECT_EVENT_CURSORUP||e.Event==PX_OBJECT_EVENT_CURSORMOVE)
	{

		if (!PX_ObjectIsPointInRegion(Object,PX_Object_Event_GetCursorX(e),PX_Object_Event_GetCursorY(e)))
		{
			return;
		}
		PX_Object_Event_SetCursorX(&e,PX_Object_Event_GetCursorX(e)-objx);
		PX_Object_Event_SetCursorY(&e,PX_Object_Event_GetCursorY(e)-objy);
	}
	PX_ObjectPostEvent(pSA->root,e);
}

 px_void  PX_Object_ScrollAreaLinkChild(PX_Object *parent,PX_Object *child)
 {
	 PX_ObjectAddClild(PX_Object_ScrollAreaGetIncludedObjects(parent),child);
	 PX_Object_ScrollAreaUpdateRange(parent);
 }

 px_void PX_Object_ScrollAreaHSliderChanged(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
 {
	 px_int value=PX_Object_SliderBarGetValue(pObject);
	 PX_Object_ScrollArea *pSA=(PX_Object_ScrollArea *)ptr;
	 pSA->root->x=-(px_float)value;
 }

 px_void PX_Object_ScrollAreaVSliderChanged(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
 {
	 px_int value=PX_Object_SliderBarGetValue(pObject);
	 PX_Object_ScrollArea *pSA=(PX_Object_ScrollArea *)ptr;
	 pSA->root->y=-(px_float)value;
 }

PX_Object * PX_Object_ScrollAreaCreate(px_memorypool *mp,PX_Object *Parent,px_int x,px_int y,px_int Width,px_int Height)
{
	PX_Object *pObject;
	PX_Object_ScrollArea *pSA=(PX_Object_ScrollArea *)MP_Malloc(mp,sizeof(PX_Object_ScrollArea));

	if (pSA==PX_NULL)
	{
		return PX_NULL;
	}
	PX_memset(pSA,0,sizeof(PX_Object_ScrollArea));

	if (Height<=24||Width<=24)
	{
		return PX_NULL;
	}

	pObject=PX_ObjectCreate(mp,Parent,(px_float)x,(px_float)y,0,(px_float)Width,(px_float)Height,0);

	if (pObject==PX_NULL)
	{
		return PX_NULL;
	}

	if (!pObject)
	{
		MP_Free(mp,pSA);
		return PX_NULL;
	}

	pObject->pObject=pSA;
	pObject->Type=PX_OBJECT_TYPE_SCROLLAREA;
	pObject->ReceiveEvents=PX_TRUE;
	pObject->Func_ObjectFree=PX_Object_ScrollAreaFree;
	pObject->Func_ObjectRender=PX_Object_ScrollAreaRender;

	pSA->BackgroundColor=PX_COLOR(0,0,0,0);
	pSA->bBorder=PX_TRUE;
	pSA->borderColor=PX_COLOR(255,0,0,0);
	//root
	pSA->root=PX_ObjectCreate(pObject->mp,0,0,0,0,0,0,0);
	pSA->hscroll=PX_Object_SliderBarCreate(mp,pObject,0,0,16,16,PX_OBJECT_SLIDERBAR_TYPE_HORIZONTAL,PX_OBJECT_SLIDERBAR_STYLE_BOX);
	pSA->vscroll=PX_Object_SliderBarCreate(mp,pObject,0,0,16,16,PX_OBJECT_SLIDERBAR_TYPE_VERTICAL,PX_OBJECT_SLIDERBAR_STYLE_BOX);
	if(!PX_SurfaceCreate(mp,Width,Height,&pSA->surface))
	{
		MP_Free(mp,pSA);
		MP_Free(mp,pObject);
		return PX_NULL;
	}
	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_ANY,PX_Object_ScrollArea_EventDispatcher,PX_NULL);
	PX_ObjectRegisterEvent(pSA->hscroll,PX_OBJECT_EVENT_VALUECHANGED,PX_Object_ScrollAreaHSliderChanged,pSA);
	PX_ObjectRegisterEvent(pSA->vscroll,PX_OBJECT_EVENT_VALUECHANGED,PX_Object_ScrollAreaVSliderChanged,pSA);

	pObject->Func_ObjectLinkChild=PX_Object_ScrollAreaLinkChild;
	PX_Object_ScrollAreaUpdateRange(pObject);
	return pObject;
}

PX_Object * PX_Object_ScrollAreaGetIncludedObjects(PX_Object *pObj)
{
	PX_Object_ScrollArea *pSA;
	pSA=PX_Object_GetScrollArea(pObj);
	if (pSA)
	{
		return pSA->root;
	}
	return PX_NULL;
}

px_void PX_Object_ScrollAreaMoveToBottom(PX_Object *pObject)
{
	px_float left=0,top=0,right=0,bottom=0;
	PX_Object_ScrollArea *psa=PX_Object_GetScrollArea(pObject);
	if(psa)
	{
		PX_Object_ScrollAreaGetRegion(psa->root,&left,&top,&right,&bottom);
		if (bottom-top>=pObject->Height)
		{
			psa->root->y=-(bottom-top-pObject->Height);
		}
		else
		{
			psa->root->y=0;
		}
	}
	
}

px_void PX_Object_ScrollAreaMoveToTop(PX_Object *pObject)
{
	px_float left=0,top=0,right=0,bottom=0;
	PX_Object_ScrollArea *psa=PX_Object_GetScrollArea(pObject);
	PX_Object_ScrollAreaGetRegion(psa->root,&left,&top,&right,&bottom);
	if(psa)
	psa->root->y=top;
}




px_void PX_Object_ScrollAreaGetRegion(PX_Object *pObject,px_float *left,px_float *top,px_float *right,px_float *bottom)
{
	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;

	if (pObject==PX_NULL)
	{
		return;
	}

	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;

	if (objx<*left)
	{
		*left=objx;
	}


	if (objx+objWidth-1>*right)
	{
		*right=objx+objWidth;
	}

	if (objy<*top)
	{
		*top=objy;
	}

	if (objy+objHeight-1>*bottom)
	{
		*bottom=objy+objHeight;
	}

	PX_Object_ScrollAreaGetRegion(pObject->pNextBrother,left,top,right,bottom);
	PX_Object_ScrollAreaGetRegion(pObject->pChilds,left,top,right,bottom);
}

px_void PX_Object_ScrollAreaUpdateRange( PX_Object *pObject)
{
	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;
	px_float left=0,top=0,right=0,bottom=0;
	PX_Object_ScrollArea *pSA;

	pSA=PX_Object_GetScrollArea(pObject);
	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);
	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;

	PX_Object_ScrollAreaGetRegion(pSA->root,&left,&top,&right,&bottom);

	do 
	{	
	px_float rWidth=right-left;
	pSA->hscroll->x=0;
	pSA->hscroll->y=objHeight;
	pSA->hscroll->Width=pObject->Width;
	pSA->hscroll->Height=16;
	if (rWidth>pObject->Width)
	{
		if (pObject->Width!=0)
		{
			px_float btnW=pObject->Width/rWidth*pObject->Width;
			pSA->hscroll->Visible=PX_TRUE;
			PX_Object_SliderBarSetSliderButtonLength(pSA->hscroll,(px_int)btnW);
		}
		PX_Object_SliderBarSetRange(pSA->hscroll,(px_int)left,(px_int)(right-objWidth));
	}
	else
	{
		pSA->hscroll->Visible=PX_FALSE;
	}
	} while (0);

	do 
	{	
		px_float rHeight=bottom-top;
		
		pSA->vscroll->x=objWidth;
		pSA->vscroll->y=0;
		pSA->vscroll->Width=16;
		pSA->vscroll->Height=objHeight;
		if (rHeight>pObject->Height)
		{
			if (pObject->Height!=0)
			{
				px_float btnH=pObject->Height/rHeight*pObject->Height;
				pSA->vscroll->Visible=PX_TRUE;
				PX_Object_SliderBarSetSliderButtonLength(pSA->vscroll,(px_int)btnH);
			}

		}
		else
		{
			pSA->vscroll->Visible=PX_FALSE;
		}
		PX_Object_SliderBarSetRange(pSA->vscroll,(px_int)top,(px_int)(bottom-objHeight));
	} while (0);
}

px_void PX_Object_ScrollAreaRender(px_surface *psurface, PX_Object *pObject,px_uint elpased)
{
	PX_Object_ScrollArea *pSA;
	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;


	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);
	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;

	pSA=PX_Object_GetScrollArea(pObject);
	if (!pSA)
	{
		return;
	}


	if (pSA->surface.height!=(px_int)objHeight||pSA->surface.width!=(px_int)objWidth)
	{
		PX_SurfaceFree(&pSA->surface);
		if(!PX_SurfaceCreate(pObject->mp,(px_int)objWidth,(px_int)objHeight,&pSA->surface))
		{
			return;
		}
	}


	PX_SurfaceClear(&pSA->surface,0,0,(px_int)objWidth-1,(px_int)objHeight-1,pSA->BackgroundColor);

	//draw
	PX_ObjectRender(&pSA->surface,pSA->root,elpased);
	PX_SurfaceRender(psurface,&pSA->surface,(px_int)objx,(px_int)objy,PX_TEXTURERENDER_REFPOINT_LEFTTOP,PX_NULL);

	//Draw Border
	if(pSA->bBorder)
	PX_GeoDrawBorder(&pSA->surface,0,0,(px_int)objWidth-1,(px_int)objHeight-1,1,pSA->borderColor);
	PX_SurfaceRender(psurface,&pSA->surface,(px_int)objx,(px_int)objy,PX_TEXTURERENDER_REFPOINT_LEFTTOP,PX_NULL);
}



px_void PX_Object_ScrollAreaFree(PX_Object *pObj)
{
	PX_Object_ScrollArea *pSA;
	pSA=PX_Object_GetScrollArea(pObj);
	if (pSA)
	{
		PX_ObjectDelete(pSA->root);
		PX_SurfaceFree(&pSA->surface);
	}
}

px_void PX_Object_ScrollAreaSetBkColor(PX_Object *pObj,px_color bkColor)
{
	PX_Object_ScrollArea *pSA;
	pSA=PX_Object_GetScrollArea(pObj);
	if (pSA)
	{
		pSA->BackgroundColor=bkColor;
	}
}


px_void PX_Object_ScrollAreaSetBorder(PX_Object *pObj,px_bool Border)
{
	PX_Object_ScrollArea *pSA;
	pSA=PX_Object_GetScrollArea(pObj);
	if (pSA)
	{
		pSA->bBorder=Border;
	}
}


px_void PX_Object_ScrollAreaSetBorderColor(PX_Object *pObj,px_color borderColor)
{
	PX_Object_ScrollArea *pSA;
	pSA=PX_Object_GetScrollArea(pObj);
	if (pSA)
	{
		pSA->borderColor=borderColor;
	}
}

PX_Object_ScrollArea * PX_Object_GetScrollArea(PX_Object *Object)
{
	if(Object->Type==PX_OBJECT_TYPE_SCROLLAREA)
		return (PX_Object_ScrollArea *)Object->pObject;
	else
		return PX_NULL;
}

PX_Object * PX_Object_AutoTextCreate(px_memorypool *mp,PX_Object *Parent,px_int x,px_int y,px_int width,PX_FontModule *fm)
{
	PX_Object *pObject;

	PX_Object_AutoText *pAt=(PX_Object_AutoText *)MP_Malloc(mp,sizeof(PX_Object_AutoText));
	PX_StringInitialize(mp,&pAt->text);
	if (pAt==PX_NULL)
	{
		return PX_NULL;
	}

	if (width<=0)
	{
		return PX_NULL;
	}
	

	pObject=PX_ObjectCreate(mp,Parent,(px_float)x,(px_float)y,0,(px_float)width,0,0);

	if (pObject==PX_NULL)
	{
		return PX_NULL;
	}

	if (!pObject)
	{
		MP_Free(mp,pAt);
		return PX_NULL;
	}

	pObject->pObject=pAt;
	pObject->Type=PX_OBJECT_TYPE_AUTOTEXT;
	pObject->ReceiveEvents=PX_TRUE;
	pObject->Func_ObjectFree=PX_Object_AutoTextFree;
	pObject->Func_ObjectRender=PX_Object_AutoTextRender;

	pAt->TextColor=PX_COLOR(255,0,0,0);
	pAt->fontModule=fm;
	return pObject;
}

px_void PX_Object_AutoTextRender(px_surface *psurface, PX_Object *pObject,px_uint elpased)
{
	px_int x_draw_oft,y_draw_oft,cursor,fsize;
	PX_Object_AutoText *pAt=(PX_Object_AutoText *)pObject->pObject;
	const px_char *Text=pAt->text.buffer;
	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;

	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;

	if (pAt==PX_NULL)
	{
		return;
	}

	if (!pObject->Visible)
	{
		return;
	}

	x_draw_oft=(px_int)objx;
	y_draw_oft=(px_int)objy;

	cursor=0;

	while (PX_TRUE)
	{
		fsize=0;

		if (pAt->fontModule)
		{
			px_dword code;
			px_int width,height;
			fsize=PX_FontModuleGetCharacterDesc(pAt->fontModule,Text+cursor,&code,&width,&height);
			if (!fsize)
			{
				break;
			}
			if (code=='\r')
			{
				//skip
			}else if (code=='\n')
			{
				x_draw_oft=(px_int)objx;
				y_draw_oft+=pAt->fontModule->max_Height;
			}
			else
			{
				PX_FontModuleDrawCharacter(psurface,pAt->fontModule,x_draw_oft,y_draw_oft,code,pAt->TextColor);
				x_draw_oft+=width;
			}

			if (x_draw_oft>objx+objWidth-PX_FontGetAscCharactorWidth()*2)
			{
				x_draw_oft=(px_int)objx;
				y_draw_oft+=pAt->fontModule->max_Height;
			}

		}
		else
		{
			fsize=1;

			if (Text[cursor]=='\r')
			{
				//skip
			}else if (Text[cursor]=='\n')
			{
				x_draw_oft=(px_int)objx;
				y_draw_oft+=__PX_FONT_HEIGHT;
			}
			else if(Text[cursor])
			{
				PX_FontDrawChar(psurface,x_draw_oft,y_draw_oft,Text[cursor],pAt->TextColor);
				x_draw_oft+=__PX_FONT_ASCSIZE;
			}
			else
			{
				break;
			}

			if (x_draw_oft>objx+objWidth-PX_FontGetAscCharactorWidth()*2)
			{
				x_draw_oft=(px_int)objx;
				y_draw_oft+=__PX_FONT_HEIGHT;
			}
		}
		cursor+=fsize;
	}
}

px_void PX_Object_AutoTextFree(PX_Object *Obj)
{
	PX_Object_AutoText * pAt=PX_Object_GetAutoText(Obj);
	if (!pAt)
	{
		return;
	}
	PX_StringFree(&pAt->text);
}



px_int PX_Object_AutoTextGetHeight(PX_Object *pObject)
{
	px_int i;
	px_int w=0;
	px_int h=0;
	PX_Object_AutoText * pAt=PX_Object_GetAutoText(pObject);

	if (!pAt)
	{
		return 0;
	}

	for (i=0;i<PX_strlen(pAt->text.buffer);i++)
	{
		if (pAt->text.buffer[i]=='\n')
		{
			w=0;
			h+=PX_FontGetCharactorHeight();
			continue;
		}
		if (pAt->text.buffer[i]=='\r')
		{
			continue;
		}

		if (pAt->text.buffer[i]=='\t')
		{
			w+=PX_FontGetAscCharactorWidth();
			continue;
		}

		if (pAt->text.buffer[i]&0x80)
		{
			i++;
		}
		else
		{
			if(w+PX_FontGetAscCharactorWidth()>pObject->Width-PX_FontGetAscCharactorWidth()*2)
			{
				w=0;
				h+=PX_FontGetCharactorHeight();
				w+=PX_FontGetAscCharactorWidth();
			}
			else
			{
				w+=PX_FontGetAscCharactorWidth();
			}
		}
	}
	return h+PX_FontGetCharactorHeight();
}

PX_Object_AutoText * PX_Object_GetAutoText(PX_Object *Object)
{
	if(Object->Type==PX_OBJECT_TYPE_AUTOTEXT)
		return (PX_Object_AutoText *)Object->pObject;
	else
		return PX_NULL;
}


px_void PX_Object_AutoTextSetTextColor(PX_Object *pObject,px_color Color)
{
	PX_Object_AutoText * pAt=PX_Object_GetAutoText(pObject);
	if (!pAt)
	{
		return;
	}
	pAt->TextColor=Color;
}


px_void PX_Object_AutoTextSetText(PX_Object *Obj,const px_char *Text)
{
	PX_Object_AutoText * pAt=PX_Object_GetAutoText(Obj);
	if (!pAt)
	{
		return;
	}
	PX_StringClear(&pAt->text);
	PX_StringCat(&pAt->text,Text);
	Obj->Height=(px_float)PX_Object_AutoTextGetHeight(Obj);
}



PX_Object * PX_Object_AnimationCreate(px_memorypool *mp,PX_Object *Parent,px_int x,px_int y,PX_Animationlibrary *lib)
{
	PX_Object *pObject;
	PX_Object_Animation *pAnimation=(PX_Object_Animation *)MP_Malloc(mp,sizeof(PX_Object_Animation));
	if (pAnimation==PX_NULL)
	{
		return PX_NULL;
	}
	pObject=PX_ObjectCreate(mp,Parent,(px_float)x,(px_float)y,0,0,0,0);
	if (pObject==PX_NULL)
	{
		MP_Free(pObject->mp,pAnimation);
		return PX_NULL;
	}

	pObject->pObject=pAnimation;
	pObject->Enabled=PX_TRUE;
	pObject->Visible=PX_TRUE;
	pObject->Type=PX_OBJECT_TYPE_ANIMATION;
	pObject->ReceiveEvents=PX_FALSE;
	pObject->Func_ObjectFree=PX_Object_AnimationFree;
	pObject->Func_ObjectRender=PX_Object_AnimationRender;
	PX_AnimationCreate(&pAnimation->animation,lib);
	pAnimation->Align=PX_ALIGN_CENTER;
	return pObject;
}

px_void PX_Object_AnimationSetLibrary(PX_Object *Object,PX_Animationlibrary *lib)
{
	PX_Object_Animation *pA=PX_Object_GetAnimation(Object);
	if (pA)
	{
		PX_AnimationSetLibrary(&pA->animation,lib);
	}
}

PX_Object_Animation * PX_Object_GetAnimation(PX_Object *Object)
{
	if(Object->Type==PX_OBJECT_TYPE_ANIMATION)
		return (PX_Object_Animation *)Object->pObject;
	else
		return PX_NULL;
}

px_void PX_Object_AnimationSetAlign(PX_Object *panimation,PX_ALIGN Align)
{
	PX_Object_Animation *pA=PX_Object_GetAnimation(panimation);
	if (pA)
	{
		pA->Align=Align;
	}
}

px_void PX_Object_AnimationRender(px_surface *psurface,PX_Object *pObject,px_uint elpased)
{
	PX_Object_Animation *pA=PX_Object_GetAnimation(pObject);
	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;

	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;

	if (pA)
	{
		PX_AnimationUpdate(&pA->animation,elpased);
		
		switch(pA->Align)
		{
		case PX_ALIGN_LEFTTOP:
			{
				PX_AnimationRender(psurface,&pA->animation,(px_int)objx,(px_int)objy,PX_TEXTURERENDER_REFPOINT_LEFTTOP,PX_NULL);
			}
			break;
		case PX_ALIGN_LEFTMID:
			{
				PX_AnimationRender(psurface,&pA->animation,(px_int)objx,(px_int)(objy+objHeight/2),PX_TEXTURERENDER_REFPOINT_LEFTMID,PX_NULL);
			}
			break;
		case PX_ALIGN_LEFTBOTTOM:
			{
				PX_AnimationRender(psurface,&pA->animation,(px_int)objx,(px_int)(objy+objHeight),PX_TEXTURERENDER_REFPOINT_LEFTBOTTOM,PX_NULL);
			}
			break;
		case PX_ALIGN_MIDTOP:
			{
				PX_AnimationRender(psurface,&pA->animation,(px_int)(objx+objWidth/2),(px_int)(objy),PX_TEXTURERENDER_REFPOINT_MIDTOP,PX_NULL);
			}
			break;
		case PX_ALIGN_CENTER:
			{
				PX_AnimationRender(psurface,&pA->animation,(px_int)(objx+objWidth/2),(px_int)(objy+objHeight/2),PX_TEXTURERENDER_REFPOINT_CENTER,PX_NULL);
			}
			break;
		case PX_ALIGN_MIDBOTTOM:
			{
				PX_AnimationRender(psurface,&pA->animation,(px_int)(objx+objWidth/2),(px_int)(objy+objHeight),PX_TEXTURERENDER_REFPOINT_MIDBOTTOM,PX_NULL);
			}
			break;
		case PX_ALIGN_RIGHTTOP:
			{
				PX_AnimationRender(psurface,&pA->animation,(px_int)(objx+objWidth),(px_int)(objy),PX_TEXTURERENDER_REFPOINT_RIGHTTOP,PX_NULL);
			}
			break;
		case PX_ALIGN_RIGHTMID:
			{
				PX_AnimationRender(psurface,&pA->animation,(px_int)(objx+objWidth),(px_int)(objy+objHeight/2),PX_TEXTURERENDER_REFPOINT_RIGHTMID,PX_NULL);
			}
			break;
		case PX_ALIGN_RIGHTBOTTOM:
			{
				PX_AnimationRender(psurface,&pA->animation,(px_int)(objx+objWidth),(px_int)(objy+objHeight),PX_TEXTURERENDER_REFPOINT_RIGHTBOTTOM,PX_NULL);
			}
			break;
		}
	}
	
}

px_void PX_Object_AnimationFree(PX_Object *pObj)
{
	PX_Object_Animation *pA=PX_Object_GetAnimation(pObj);
	if (pA)
	PX_AnimationFree(&pA->animation);
}


px_void PX_ObjectSetUserCode(PX_Object *pObject,px_int user)
{
	pObject->User_int=user;
}

px_void PX_ObjectSetUserPointer(PX_Object *pObject,px_void *user_ptr)
{
	pObject->User_ptr=user_ptr;
}





px_void PX_Object_CursorButtonRender(px_surface *psurface, PX_Object *pObject,px_uint elpased)
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
		pcb->c_distance+=(pcb->c_distance_near-pcb->c_distance)*(elpased*1.f/200);
		else
			pcb->c_distance=pcb->c_distance_near;
	}
	else
	{
		if(PX_ABS(pcb->c_distance-pcb->c_distance_far)>=1)
			pcb->c_distance+=(pcb->c_distance_far-pcb->c_distance)*(elpased*1.f/200);
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


	pCb->pushbutton=PX_Object_PushButtonCreate(mp,pObject,x,y,Width,Height,Text,fontmodule,Color);
	pCb->c_color=PX_COLOR(255,0,0,0);
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





PX_Object_List  * PX_Object_GetList( PX_Object *Object )
{
	if(Object->Type==PX_OBJECT_TYPE_LIST)
	return (PX_Object_List *)Object->pObject;
	else
	return PX_NULL;
}

PX_Object_ListItem  * PX_Object_GetListItem( PX_Object *Object )
{
	if(Object->Type==PX_OBJECT_TYPE_LISTITEM)
		return (PX_Object_ListItem *)Object->pObject;
	else
		return PX_NULL;
}

px_void PX_Object_ListOnButtonDown(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Object_List *pList= PX_Object_GetList(pObject);
	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;

	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;



	if (!PX_ObjectIsCursorInRegion(pObject,e))
	{
		return;
	}

	pList->currentSelectedId=(px_int)(PX_Object_Event_GetCursorY(e)+pList->offsety-objy)/pList->ItemHeight;
	if(pList->currentSelectedId<0||pList->currentSelectedId>=pList->pData.size)
	{
		pList->currentSelectedId=-1;
	}

}

px_void PX_Object_ListOnWheel(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Object_List *pList= PX_Object_GetList(pObject);
	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;

	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;

	if (!PX_ObjectIsCursorInRegion(pObject,e))
	{
		return;
	}
	pList->offsety-=(px_int)PX_Object_Event_GetCursorZ(e)/5;


	if (pList->offsety>(pList->ItemHeight)*pList->pData.size-objHeight)
	{
		pList->offsety=(pList->ItemHeight)*pList->pData.size-(px_int)objHeight;
	}

	if (pList->offsety<0)
	{
		pList->offsety=0;
	}

	PX_Object_SliderBarSetValue(pList->SliderBar,pList->offsety);
}




px_void PX_Object_ListRender(px_surface *psurface, PX_Object *pObject,px_uint elpased)
{
	PX_Object *pItemObject;
	px_int offsetX=0,drawXCenter=0;
	px_int i;
	px_int iy;
	PX_Object_List *pList= PX_Object_GetList(pObject);
	PX_Object_ListItem *pItem;
	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;

	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;


	if (pList->ItemHeight*pList->pData.size>objHeight)
	{
		PX_Object_SliderBar *pSliderBar=PX_Object_GetSliderBar(pList->SliderBar);
		pList->SliderBar->Visible=PX_TRUE;
		pList->SliderBar->y=objy;
		pList->SliderBar->x=objx+objWidth-32;
		pSliderBar->SliderButtonLength=(px_int)(pList->SliderBar->Height*objHeight/(pList->ItemHeight*pList->pData.size));
		if(pSliderBar->SliderButtonLength<32) pSliderBar->SliderButtonLength=32;
		pSliderBar->Max=(pList->ItemHeight)*pList->pData.size-(px_int)objHeight;
		PX_Object_SliderBarSetBackgroundColor(pList->SliderBar,pList->BackgroundColor);
		PX_Object_SliderBarSetColor(pList->SliderBar,pList->BorderColor);
	}
	else
	{
		pList->SliderBar->Visible=PX_FALSE;
	}


	if (pList->offsety<0)
	{
		iy=-pList->offsety;
	}
	else
	{
		iy=-(pList->offsety%pList->ItemHeight);
	}

	for (i=0;i<pList->Items.size;i++)
	{

		pItemObject=*PX_VECTORAT(PX_Object *,&pList->Items,i);
		pItemObject->x=0;
		pItemObject->y=(px_float)iy;
		pItem=PX_Object_GetListItem(pItemObject);
		pItem->status=PX_LISTITEM_STATUS_NORMAL;
		if (pList->offsety<0)
		{
			if(i<pList->pData.size)
			{
				pItem->pdata=*PX_VECTORAT(px_void *,&pList->pData,i);
				PX_ObjectUpdate(pItemObject,elpased);
			}
			else
			{
				pItem->pdata=PX_NULL;
			}
		}
		else
		{
			if(i+pList->offsety/pList->ItemHeight<pList->pData.size)
			{
				if (i+pList->offsety/pList->ItemHeight==pList->currentSelectedId)
				{
					pItem->status=PX_LISTITEM_STATUS_SELECTED;
				}
				pItem->pdata=*PX_VECTORAT(px_void *,&pList->pData,i+pList->offsety/pList->ItemHeight);
				PX_ObjectUpdate(pItemObject,elpased);
			}
			else
			{
				pItem->pdata=PX_NULL;
			}
		}
		iy+=(px_int)pItemObject->Height;
	}


	PX_SurfaceClear(&pList->renderSurface,0,0,pList->renderSurface.width-1,pList->renderSurface.height-1,pList->BackgroundColor);
	for (i=0;i<pList->Items.size;i++)
	{
		pItemObject=*PX_VECTORAT(PX_Object *,&pList->Items,i);
		if (pItemObject->y+pItemObject->Height>0&&pItemObject->y<pObject->Height)
		{
			PX_ObjectRender(&pList->renderSurface,pItemObject,elpased);
		}
	}
	PX_GeoDrawBorder(&pList->renderSurface,0,0,pList->renderSurface.width-1,pList->renderSurface.height-1,1,pList->BorderColor);
	PX_SurfaceCover(psurface,&pList->renderSurface,(px_int)objx,(px_int)objy,PX_TEXTURERENDER_REFPOINT_LEFTTOP);

}

px_void PX_Object_ListClear(PX_Object *pListObj)
{
	PX_Object_List  *pList=PX_Object_GetList(pListObj);
	if(pList)
	PX_VectorClear(&pList->pData);
}

px_int PX_Object_ListAdd(PX_Object *pListObj,px_void *ptr)
{
	PX_Object_List  *pList=PX_Object_GetList(pListObj);
	if(pList)
	{
		if(PX_VectorPushback(&pList->pData,&ptr))
			return pList->pData.size-1;
	}
	return -1;
}

px_void PX_Object_ListFree(PX_Object *pListObj)
{
	PX_Object_List *pList=PX_Object_GetList(pListObj);
	if (!pList)
	{
		PX_ASSERT();
		return;
	}
	PX_Object_ListClear(pListObj);
	PX_SurfaceFree(&pList->renderSurface);
	PX_VectorFree(&pList->Items);
}

px_void PX_Object_ListOnSliderValueChanged(PX_Object  *Object,PX_Object_Event e,px_void *ptr)
{
	px_int value=PX_Object_SliderBarGetValue(Object);
	PX_Object_List *pList=PX_Object_GetList((PX_Object *)ptr);
	pList->offsety=value;
}

px_void PX_Object_ListOnEvent(PX_Object  *Object,PX_Object_Event e,px_void *ptr)
{
	px_int i;
	PX_Object_List  *pList=PX_Object_GetList(Object);

	switch (e.Event)
	{
	case PX_OBJECT_EVENT_CURSORWHEEL:
		if (!PX_ObjectIsPointInRegion(Object,PX_Object_Event_GetCursorX(e),PX_Object_Event_GetCursorY(e)))
		{
			return;
		}
	case PX_OBJECT_EVENT_CURSORDRAG:
	case PX_OBJECT_EVENT_CURSORRDOWN:
	case PX_OBJECT_EVENT_CURSORDOWN:
	case PX_OBJECT_EVENT_CURSORUP:
	case PX_OBJECT_EVENT_CURSORRUP:
	case PX_OBJECT_EVENT_CURSORMOVE:
		PX_Object_Event_SetCursorX(&e,PX_Object_Event_GetCursorX(e)-Object->x);
		PX_Object_Event_SetCursorY(&e,PX_Object_Event_GetCursorY(e)-Object->y);
		break;
	}

	for (i=0;i<pList->Items.size;i++)
	{
		PX_Object *pItem=*PX_VECTORAT(PX_Object *,&pList->Items,i);
		PX_ObjectPostEvent(pItem,e);
	}
}

PX_Object * PX_Object_ListCreate(px_memorypool *mp, PX_Object *Parent,px_int x,px_int y,px_int Width,px_int Height,px_int ItemHeight,PX_Object_ListItemOnCreate _CreateFunc)
{
	px_int i;
	PX_Object_List List,*pList;
	PX_Object_ListItem Item;
	PX_Object *pListObject,*pItemObject;
	Item.status=PX_LISTITEM_STATUS_NORMAL;
	List.mp=mp;
	List.ItemWidth=Width-32;
	List.ItemHeight=ItemHeight;
	List.CreateFunctions=_CreateFunc;
	List.BorderColor=PX_COLOR(255,0,0,0);
	List.BackgroundColor=PX_COLOR(255,255,255,255);
	List.offsetx=0;
	List.currentSelectedId=-1;
	List.offsety=0;
	PX_VectorInitialize(mp,&List.Items,sizeof(PX_Object *),64);
	PX_VectorInitialize(mp,&List.pData,sizeof(px_void *),32);
	if(!PX_SurfaceCreate(mp,Width,Height,&List.renderSurface)) return PX_NULL;

	if(!(pListObject=PX_ObjectCreateEx(mp,Parent,(px_float)x,(px_float)y,0,(px_float)Width,(px_float)Height,0,PX_OBJECT_TYPE_LIST,PX_NULL,PX_Object_ListRender,PX_Object_ListFree,&List,sizeof(PX_Object_List)))) return PX_NULL;
	
	pList=PX_Object_GetList(pListObject);
	pList->SliderBar=PX_Object_SliderBarCreate(mp,pListObject,(px_int)pListObject->Width-32,0,32,(px_int)pListObject->Height,PX_OBJECT_SLIDERBAR_TYPE_VERTICAL,PX_OBJECT_SLIDERBAR_STYLE_BOX);

	for (i=0;i<Height/ItemHeight+2;i++)
	{
		pItemObject=PX_ObjectCreateEx(mp,PX_NULL,0,0,0,(px_float)pListObject->Width-32,(px_float)ItemHeight,0,PX_OBJECT_TYPE_LISTITEM,PX_NULL,PX_NULL,PX_NULL,&Item,sizeof(PX_Object_ListItem));
		if(!_CreateFunc(mp,pItemObject)) return PX_FALSE;
		if(!PX_VectorPushback(&pList->Items,&pItemObject))return PX_FALSE;
	}

	

	PX_ObjectRegisterEvent(pListObject,PX_OBJECT_EVENT_CURSORWHEEL,PX_Object_ListOnWheel,pListObject);
	PX_ObjectRegisterEvent(pListObject,PX_OBJECT_EVENT_CURSORDOWN,PX_Object_ListOnButtonDown,pListObject);
	PX_ObjectRegisterEvent(pList->SliderBar,PX_OBJECT_EVENT_VALUECHANGED,PX_Object_ListOnSliderValueChanged,pListObject);
	PX_ObjectRegisterEvent(pListObject,PX_OBJECT_EVENT_ANY,PX_Object_ListOnEvent,pListObject);
	return pListObject;
}

px_void PX_Object_ListSetBackgroundColor(PX_Object *pListObject,px_color color)
{
	PX_Object_List *pList=PX_Object_GetList(pListObject);
	if (pList)
	{
		pList->BackgroundColor=color;
	}
}

px_void PX_Object_ListSetBorderColor(PX_Object *pListObject,px_color color)
{
	PX_Object_List *pList=PX_Object_GetList(pListObject);
	if (pList)
	{
		pList->BorderColor=color;
	}
}



px_void * PX_Object_ListGetItem(PX_Object *pListObject,px_int index)
{
	PX_Object_List *pList=PX_Object_GetList(pListObject);
	if (index>0&&index<pList->pData.size)
	{
		return PX_VECTORAT(px_void *,&pList->pData,index);
	}
	return PX_NULL;
}

px_void PX_Object_ListRemoveItem(PX_Object *pListObject,px_int index)
{
	PX_Object_List *pList=PX_Object_GetList(pListObject);
	if (index>0&&index<pList->pData.size)
	{
		PX_VectorErase(&pList->pData,index);
	}
	return ;
}


PX_Object_VirtualKeyBoard *PX_Object_GetVirtualKeyBoard(PX_Object *pObject)
{
	if (!pObject)
	{
		return PX_NULL;
	}
	if (pObject->Type==PX_OBJECT_TYPE_VKEYBOARD)
	{
		return (PX_Object_VirtualKeyBoard *)pObject->pObject;
	}
	return PX_NULL;
}

px_void PX_Object_VirtualKeyBoardRender(px_surface *psurface, PX_Object *pObject,px_uint elpased)
{
	px_int i;
	px_int oftx;
	px_int ofty;
	PX_Object_VirtualKeyBoard *keyBoard=(PX_Object_VirtualKeyBoard *)pObject->pObject;
	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;

	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;

	oftx=(px_int)objx;
	ofty=(px_int)objy;

	//clear background
	PX_SurfaceClear(psurface,(px_int)objx,(px_int)objy,(px_int)(objx+objWidth),(px_int)(objy+objHeight),keyBoard->backgroundColor);

	for (i=0;i<PX_COUNTOF(keyBoard->Keys);i++)
	{
		//draw rect
		
		if (keyBoard->Keys[i].bDown||keyBoard->Keys[i].bhold)
		{
			PX_GeoDrawRect(psurface,oftx+keyBoard->Keys[i].x,ofty+keyBoard->Keys[i].y,oftx+keyBoard->Keys[i].x+keyBoard->Keys[i].width,ofty+keyBoard->Keys[i].y+keyBoard->Keys[i].height,keyBoard->pushColor);
		}
		else if (keyBoard->Keys[i].bCursor)
		{
			PX_GeoDrawRect(psurface,oftx+keyBoard->Keys[i].x,ofty+keyBoard->Keys[i].y,oftx+keyBoard->Keys[i].x+keyBoard->Keys[i].width,ofty+keyBoard->Keys[i].y+keyBoard->Keys[i].height,keyBoard->cursorColor);
		}

		PX_GeoDrawBorder(psurface,oftx+keyBoard->Keys[i].x,ofty+keyBoard->Keys[i].y,oftx+keyBoard->Keys[i].x+keyBoard->Keys[i].width,ofty+keyBoard->Keys[i].y+keyBoard->Keys[i].height,1,keyBoard->borderColor);
		
		if (keyBoard->bShift||keyBoard->bUpper)
		{
			PX_FontModuleDrawText(psurface,PX_NULL,oftx+keyBoard->Keys[i].x+2,ofty+keyBoard->Keys[i].y+2,PX_ALIGN_LEFTTOP,keyBoard->Keys[i].u_key,keyBoard->borderColor);
		}
		else
		{
			PX_FontModuleDrawText(psurface,PX_NULL,oftx+keyBoard->Keys[i].x+2,ofty+keyBoard->Keys[i].y+2,PX_ALIGN_LEFTTOP,keyBoard->Keys[i].d_key,keyBoard->borderColor);
		}
	}

}

px_void PX_Object_VirtualKeyBoardOnCursorUp(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	px_float x=PX_Object_Event_GetCursorX(e);
	px_float y=PX_Object_Event_GetCursorY(e);
	px_int i;
	PX_Object_VirtualKeyBoard *pkb=(PX_Object_VirtualKeyBoard *)pObject->pObject;
	if (!PX_ObjectIsPointInRegion(pObject,(px_float)x,(px_float)y))
	{
		return;
	}

	for (i=0;i<PX_COUNTOF(pkb->Keys);i++)
	{
		
			pkb->Keys[i].bDown=PX_FALSE;
	}
}

px_void PX_Object_VirtualKeyBoardOnCursorMove(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	px_float x=PX_Object_Event_GetCursorX(e);
	px_float y=PX_Object_Event_GetCursorY(e);
	px_int i;
	px_int kh=(px_int)(pObject->Height/5);
	PX_Object_VirtualKeyBoard *pkb=(PX_Object_VirtualKeyBoard *)pObject->pObject;
	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;

	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;


	x=(PX_Object_Event_GetCursorX(e)-objx);
	y=(PX_Object_Event_GetCursorY(e)-objy);


	for (i=0;i<PX_COUNTOF(pkb->Keys);i++)
	{
		if(!pkb->Keys[i].bDown)
		{
			pkb->Keys[i].bCursor=PX_FALSE;
		}
		else
		{
			if (!PX_isPointInRect(PX_POINT((px_float)x,(px_float)y,0),PX_RECT((px_float)pkb->Keys[i].x,(px_float)pkb->Keys[i].y,(px_float)pkb->Keys[i].width,(px_float)pkb->Keys[i].height)))
				pkb->Keys[i].bDown=PX_FALSE;
		}
	}

	if (y<kh)
	{
		for (i=0;i<=13;i++)
		{
			if (PX_isPointInRect(PX_POINT((px_float)x,(px_float)y,0),PX_RECT((px_float)pkb->Keys[i].x,(px_float)pkb->Keys[i].y,(px_float)pkb->Keys[i].width,(px_float)pkb->Keys[i].height)))
			{
				pkb->Keys[i].bCursor=PX_TRUE;
				break;
			}
		}
	}
	else if (y<2*kh)
	{
		for (i=14;i<=27;i++)
		{
			if (PX_isPointInRect(PX_POINT((px_float)x,(px_float)y,0),PX_RECT((px_float)pkb->Keys[i].x,(px_float)pkb->Keys[i].y,(px_float)pkb->Keys[i].width,(px_float)pkb->Keys[i].height)))
			{
				pkb->Keys[i].bCursor=PX_TRUE;
				break;
			}
		}
	}
	else if (y<3*kh)
	{
		for (i=28;i<=40;i++)
		{
			if (PX_isPointInRect(PX_POINT((px_float)x,(px_float)y,0),PX_RECT((px_float)pkb->Keys[i].x,(px_float)pkb->Keys[i].y,(px_float)pkb->Keys[i].width,(px_float)pkb->Keys[i].height)))
			{
				pkb->Keys[i].bCursor=PX_TRUE;
				break;
			}
		}
	}
	else if (y<4*kh)
	{
		for (i=41;i<=52;i++)
		{
			if (PX_isPointInRect(PX_POINT((px_float)x,(px_float)y,0),PX_RECT((px_float)pkb->Keys[i].x,(px_float)pkb->Keys[i].y,(px_float)pkb->Keys[i].width,(px_float)pkb->Keys[i].height)))
			{
				pkb->Keys[i].bCursor=PX_TRUE;
				break;
			}
		}
	}
	else
	{
		for (i=53;i<=55;i++)
		{
			if (PX_isPointInRect(PX_POINT((px_float)x,(px_float)y,0),PX_RECT((px_float)pkb->Keys[i].x,(px_float)pkb->Keys[i].y,(px_float)pkb->Keys[i].width,(px_float)pkb->Keys[i].height)))
			{
				pkb->Keys[i].bCursor=PX_TRUE;
				break;
			}
		}
	}

	
	
}

px_void PX_Object_VirtualKeyBoardOnCursorDown(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	px_float x=PX_Object_Event_GetCursorX(e);
	px_float y=PX_Object_Event_GetCursorY(e);
	px_int i;
	px_int kh=(px_int)(pObject->Height/5);
	PX_Object_VirtualKeyBoard *pkb=(PX_Object_VirtualKeyBoard *)pObject->pObject;

	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;

	PX_Object_Event stringEvent;

	px_char strEve[2]={0};

	stringEvent.Event=PX_OBJECT_EVENT_STRING;
	PX_Object_Event_SetStringPtr(&stringEvent,strEve);


	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;

	if (!PX_ObjectIsPointInRegion(pObject,(px_float)x,(px_float)y))
	{
		PX_ObjectClearFocus(pObject);
		return;
	}

	PX_ObjectSetFocus(pObject);

	x=(PX_Object_Event_GetCursorX(e)-objx);
	y=(PX_Object_Event_GetCursorY(e)-objy);


	if (y<kh)
	{
		for (i=0;i<=13;i++)
		{
			if (PX_isPointInRect(PX_POINT((px_float)x,(px_float)y,0),PX_RECT((px_float)pkb->Keys[i].x,(px_float)pkb->Keys[i].y,(px_float)pkb->Keys[i].width,(px_float)pkb->Keys[i].height)))
			{
				pkb->Keys[i].bDown=PX_TRUE;
				break;
			}
		}
	}
	else if (y<2*kh)
	{
		for (i=14;i<=27;i++)
		{
			if (PX_isPointInRect(PX_POINT((px_float)x,(px_float)y,0),PX_RECT((px_float)pkb->Keys[i].x,(px_float)pkb->Keys[i].y,(px_float)pkb->Keys[i].width,(px_float)pkb->Keys[i].height)))
			{
				pkb->Keys[i].bDown=PX_TRUE;
				break;
			}
		}
	}
	else if (y<3*kh)
	{
		for (i=28;i<=40;i++)
		{
			if (PX_isPointInRect(PX_POINT((px_float)x,(px_float)y,0),PX_RECT((px_float)pkb->Keys[i].x,(px_float)pkb->Keys[i].y,(px_float)pkb->Keys[i].width,(px_float)pkb->Keys[i].height)))
			{
				pkb->Keys[i].bDown=PX_TRUE;
				break;
			}
		}
	}
	else if (y<4*kh)
	{
		for (i=41;i<=52;i++)
		{
			if (PX_isPointInRect(PX_POINT((px_float)x,(px_float)y,0),PX_RECT((px_float)pkb->Keys[i].x,(px_float)pkb->Keys[i].y,(px_float)pkb->Keys[i].width,(px_float)pkb->Keys[i].height)))
			{
				pkb->Keys[i].bDown=PX_TRUE;
				break;
			}
		}
	}
	else
	{
		for (i=53;i<=55;i++)
		{
			if (PX_isPointInRect(PX_POINT((px_float)x,(px_float)y,0),PX_RECT((px_float)pkb->Keys[i].x,(px_float)pkb->Keys[i].y,(px_float)pkb->Keys[i].width,(px_float)pkb->Keys[i].height)))
			{
				pkb->Keys[i].bDown=PX_TRUE;
				break;
			}
		}
	}

	if (i==13)
	{
		pkb->functionCode=8;
		if (pkb->LinkerObject)
		{
			strEve[0]=pkb->functionCode;
			PX_ObjectPostEvent(pkb->LinkerObject,stringEvent);
		}
	}
	else if(i==14)
	{
		pkb->functionCode='\t';
		if (pkb->LinkerObject)
		{
			strEve[0]=pkb->functionCode;
			PX_ObjectPostEvent(pkb->LinkerObject,stringEvent);
		}
	}
	else if(i==28)
	{
		pkb->bUpper=!pkb->bUpper;
		if (pkb->bUpper)
		{
			pkb->Keys[28].bhold=PX_TRUE;
		}
		else
		{
			pkb->Keys[28].bhold=PX_FALSE;
		}
		pkb->functionCode=0;
	}
	else if(i==40)
	{
		pkb->functionCode='\r';
		if (pkb->LinkerObject)
		{
			strEve[0]=pkb->functionCode;
			PX_ObjectPostEvent(pkb->LinkerObject,stringEvent);
		}
	}
	else if(i==41)
	{
		pkb->bShift=!pkb->bShift;
		if (pkb->bShift)
		{
			pkb->Keys[41].bhold=PX_TRUE;
		}
		else
		{
			pkb->Keys[41].bhold=PX_FALSE;
		}
		pkb->functionCode=0;
	}
	else if(i==52)
	{
		pkb->bShift=!pkb->bShift;
		if (pkb->bShift)
		{
			pkb->Keys[52].bhold=PX_TRUE;
		}
		else
		{
			pkb->Keys[52].bhold=PX_FALSE;
		}
		pkb->functionCode=0;
	}
	else if(i==53)
	{
		pkb->bCtrl=!pkb->bCtrl;
		if (pkb->bCtrl)
		{
			pkb->Keys[53].bhold=PX_TRUE;
		}
		else
		{
			pkb->Keys[53].bhold=PX_FALSE;
		}
		pkb->functionCode=0;
	}
	else if(i==54)
	{
		pkb->bAlt=!pkb->bAlt;
		if (pkb->bAlt)
		{
			pkb->Keys[54].bhold=PX_TRUE;
		}
		else
		{
			pkb->Keys[54].bhold=PX_FALSE;
		}
		pkb->functionCode=0;
	}
	else if(i==55)
	{
		pkb->functionCode=' ';
		if (pkb->LinkerObject)
		{
			strEve[0]=pkb->functionCode;
			PX_ObjectPostEvent(pkb->LinkerObject,stringEvent);
		}
	}
	else
	{
		if (pkb->bUpper||pkb->bShift)
		{
			pkb->functionCode=pkb->Keys[i].u_key[0];
		}
		else
		{
			pkb->functionCode=pkb->Keys[i].d_key[0];
		}

		pkb->bShift=PX_FALSE;
		pkb->Keys[52].bhold=PX_FALSE;
		pkb->Keys[41].bhold=PX_FALSE;
		if (pkb->LinkerObject)
		{
			strEve[0]=pkb->functionCode;
			PX_ObjectPostEvent(pkb->LinkerObject,stringEvent);
		}
	}

	
}



PX_Object* PX_Object_VirtualKeyBoardCreate(px_memorypool *mp, PX_Object *Parent,px_int x,px_int y,px_int width,px_int height)
{
	px_int i;
	px_int kw,kh;
	px_int xoffset,yoffset;
	PX_Object_VirtualKeyBoard keyboard;
	px_char line1_upper[]="~!@#$%^&*()_+";
	px_char line1_lower[]="`1234567890-=";
	px_char line2_upper[]="QWERTYUIOP{}|";
	px_char line2_lower[]="qwertyuiop[]\\";
	px_char line3_upper[]="ASDFGHJKL:\"";
	px_char line3_lower[]="asdfghjkl;'";
	px_char line4_upper[]="ZXCVBNM<>?";
	px_char line4_lower[]="zxcvbnm,./";


	PX_Object *keyboardObject;

	if (width<24*15||height<16*5)
	{
		return PX_NULL;
	}
	PX_memset(&keyboard,0,sizeof(PX_Object_VirtualKeyBoard));

	kw=width/15;
	kh=height/5;

	xoffset=0;
	yoffset=0;
	for (i=0;i<=12;i++)
	{
		keyboard.Keys[i].d_key[0]=line1_lower[i];
		keyboard.Keys[i].u_key[0]=line1_upper[i];
		keyboard.Keys[i].x=xoffset;
		keyboard.Keys[i].y=yoffset;
		keyboard.Keys[i].width=kw;
		keyboard.Keys[i].height=kh;
		xoffset+=kw;
	}

	PX_strset(keyboard.Keys[13].d_key,"Back");
	PX_strset(keyboard.Keys[13].u_key,"Back");
	keyboard.Keys[13].x=xoffset;
	keyboard.Keys[13].y=0;
	keyboard.Keys[13].width=kw*2;
	keyboard.Keys[13].height=kh;


	xoffset=0;
	yoffset=kh;

	PX_strset(keyboard.Keys[14].d_key,"Tab");
	PX_strset(keyboard.Keys[14].u_key,"Tab");

	keyboard.Keys[14].x=xoffset;
	keyboard.Keys[14].y=kh;
	keyboard.Keys[14].width=(px_int)(kw*1.5);
	keyboard.Keys[14].height=kh;

	xoffset+=keyboard.Keys[14].width;

	for (i=15;i<=26;i++)
	{
		keyboard.Keys[i].d_key[0]=line2_lower[i-15];
		keyboard.Keys[i].u_key[0]=line2_upper[i-15];
		keyboard.Keys[i].x=xoffset;
		keyboard.Keys[i].y=yoffset;
		keyboard.Keys[i].width=kw;
		keyboard.Keys[i].height=kh;
		xoffset+=keyboard.Keys[i].width;
	}
	PX_strset(keyboard.Keys[27].d_key,"\\");
	PX_strset(keyboard.Keys[27].u_key,"|");
	keyboard.Keys[27].x=xoffset;
	keyboard.Keys[27].y=yoffset;
	keyboard.Keys[27].width=(px_int)(kw*1.5);;
	keyboard.Keys[27].height=kh;

	xoffset=0;
	yoffset=2*kh;

	PX_strset(keyboard.Keys[28].d_key,"Upper");
	PX_strset(keyboard.Keys[28].u_key,"Upper");
	keyboard.Keys[28].x=xoffset;
	keyboard.Keys[28].y=yoffset;
	keyboard.Keys[28].width=(px_int)(kw*2);
	keyboard.Keys[28].height=kh;
	xoffset+=keyboard.Keys[28].width;

	for (i=29;i<=39;i++)
	{
		keyboard.Keys[i].d_key[0]=line3_lower[i-29];
		keyboard.Keys[i].u_key[0]=line3_upper[i-29];
		keyboard.Keys[i].x=xoffset;
		keyboard.Keys[i].y=yoffset;
		keyboard.Keys[i].width=kw;
		keyboard.Keys[i].height=kh;
		xoffset+=keyboard.Keys[i].width;
	}

	PX_strset(keyboard.Keys[40].d_key,"Enter");
	PX_strset(keyboard.Keys[40].u_key,"Enter");
	keyboard.Keys[40].x=xoffset;
	keyboard.Keys[40].y=yoffset;
	keyboard.Keys[40].width=(px_int)(kw*2);;
	keyboard.Keys[40].height=kh;
	xoffset+=keyboard.Keys[40].width;

	xoffset=0;
	yoffset=3*kh;

	PX_strset(keyboard.Keys[41].d_key,"Shift");
	PX_strset(keyboard.Keys[41].u_key,"Shift");
	keyboard.Keys[41].x=xoffset;
	keyboard.Keys[41].y=yoffset;
	keyboard.Keys[41].width=(px_int)(kw*2.5);;
	keyboard.Keys[41].height=kh;
	xoffset+=keyboard.Keys[41].width;

	for (i=42;i<=51;i++)
	{
		keyboard.Keys[i].d_key[0]=line4_lower[i-42];
		keyboard.Keys[i].u_key[0]=line4_upper[i-42];
		keyboard.Keys[i].x=xoffset;
		keyboard.Keys[i].y=yoffset;
		keyboard.Keys[i].width=kw;
		keyboard.Keys[i].height=kh;
		xoffset+=keyboard.Keys[i].width;
	}

	PX_strset(keyboard.Keys[52].d_key,"Shift");
	PX_strset(keyboard.Keys[52].u_key,"Shift");
	keyboard.Keys[52].x=xoffset;
	keyboard.Keys[52].y=yoffset;
	keyboard.Keys[52].width=(px_int)(kw*2.5);;
	keyboard.Keys[52].height=kh;
	xoffset+=keyboard.Keys[52].width;

	xoffset=0;
	yoffset=4*kh;
	PX_strset(keyboard.Keys[53].d_key,"Ctrl");
	PX_strset(keyboard.Keys[53].u_key,"Ctrl");
	keyboard.Keys[53].x=xoffset;
	keyboard.Keys[53].y=yoffset;
	keyboard.Keys[53].width=(px_int)(kw*1.5);;
	keyboard.Keys[53].height=kh;
	xoffset+=keyboard.Keys[53].width;

	PX_strset(keyboard.Keys[54].d_key,"Alt");
	PX_strset(keyboard.Keys[54].u_key,"Alt");
	keyboard.Keys[54].x=xoffset;
	keyboard.Keys[54].y=yoffset;
	keyboard.Keys[54].width=(px_int)(kw*1.5);;
	keyboard.Keys[54].height=kh;
	xoffset+=keyboard.Keys[54].width;

	PX_strset(keyboard.Keys[55].d_key,"");
	PX_strset(keyboard.Keys[55].u_key,"");
	keyboard.Keys[55].x=xoffset;
	keyboard.Keys[55].y=yoffset;
	keyboard.Keys[55].width=(px_int)(kw*12);;
	keyboard.Keys[55].height=kh;
	xoffset+=keyboard.Keys[55].width;

	keyboard.cursorColor=PX_COLOR(255,192,192,192);
	keyboard.pushColor=PX_COLOR(255,128,128,128);
	keyboard.backgroundColor=PX_COLOR(255,255,255,255);
	keyboard.borderColor=PX_COLOR(255,0,0,0);

	keyboard.LinkerObject=PX_NULL;

	keyboardObject=PX_ObjectCreateEx(mp,Parent,(px_float)x,(px_float)y,0,(px_float)width,(px_float)height,0,PX_OBJECT_TYPE_VKEYBOARD,PX_NULL,PX_Object_VirtualKeyBoardRender,PX_NULL,&keyboard,sizeof(PX_Object_VirtualKeyBoard));

	PX_ObjectRegisterEvent(keyboardObject,PX_OBJECT_EVENT_CURSORMOVE,PX_Object_VirtualKeyBoardOnCursorMove,PX_NULL);
	PX_ObjectRegisterEvent(keyboardObject,PX_OBJECT_EVENT_CURSORDRAG,PX_Object_VirtualKeyBoardOnCursorMove,PX_NULL);
	PX_ObjectRegisterEvent(keyboardObject,PX_OBJECT_EVENT_CURSORDOWN,PX_Object_VirtualKeyBoardOnCursorDown,PX_NULL);
	PX_ObjectRegisterEvent(keyboardObject,PX_OBJECT_EVENT_CURSORUP,PX_Object_VirtualKeyBoardOnCursorUp,PX_NULL);

	keyboardObject->OnLostFocusReleaseEvent=PX_FALSE;

	return keyboardObject;
}

px_void PX_Object_VirtualKeyBoardSetBackgroundColor(PX_Object *pObject,px_color Color)
{
	PX_Object_VirtualKeyBoard *pkb=PX_Object_GetVirtualKeyBoard(pObject);
	if (pkb)
	{
		pkb->backgroundColor=Color;
	}
}

px_void PX_Object_VirtualKeyBoardSetBorderColor(PX_Object *pObject,px_color Color)
{
	PX_Object_VirtualKeyBoard *pkb=PX_Object_GetVirtualKeyBoard(pObject);
	if (pkb)
	{
		pkb->borderColor=Color;
	}
}

px_void PX_Object_VirtualKeyBoardSetCursorColor(PX_Object *pObject,px_color Color)
{
	PX_Object_VirtualKeyBoard *pkb=PX_Object_GetVirtualKeyBoard(pObject);
	if (pkb)
	{
		pkb->cursorColor=Color;
	}
}

px_void PX_Object_VirtualKeyBoardSetPushColor(PX_Object *pObject,px_color Color)
{
	PX_Object_VirtualKeyBoard *pkb=PX_Object_GetVirtualKeyBoard(pObject);
	if (pkb)
	{
		pkb->pushColor=Color;
	}
}

px_char PX_Object_VirtualKeyBoardGetCode(PX_Object *pObject)
{
	PX_Object_VirtualKeyBoard *pkb=PX_Object_GetVirtualKeyBoard(pObject);
	if (pkb)
	{
		px_char ch= pkb->functionCode;
		pkb->functionCode=0;
		return ch;
	}
	return 0;
}

px_void PX_Object_VirtualKeyBoardSetLinkerObject(PX_Object *pObject,PX_Object *linker)
{
	PX_Object_VirtualKeyBoard *pkb=PX_Object_GetVirtualKeyBoard(pObject);
	if (pkb)
	{
		pkb->LinkerObject=linker;
	}
}

//////////////////////////////////////////////////////////////////////////
//NumberKey
PX_Object_VirtualNumberKeyBoard *PX_Object_GetVirtualNumberKeyBoard(PX_Object *pObject)
{
	if (!pObject)
	{
		return PX_NULL;
	}
	if (pObject->Type==PX_OBJECT_TYPE_VNKEYBOARD)
	{
		return (PX_Object_VirtualNumberKeyBoard *)pObject->pObject;
	}
	return PX_NULL;
}

px_void PX_Object_VirtualNumberKeyBoardRender(px_surface *psurface, PX_Object *pObject,px_uint elpased)
{
	px_int i;
	px_int oftx;
	px_int ofty;
	PX_Object_VirtualNumberKeyBoard *keyBoard=(PX_Object_VirtualNumberKeyBoard *)pObject->pObject;

	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;

	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;

	oftx=(px_int)objx;
	ofty=(px_int)objy;
	//clear background
	PX_SurfaceClear(psurface,(px_int)objx,(px_int)objy,(px_int)(objx+objWidth-1),(px_int)(objy+objHeight-1),keyBoard->backgroundColor);

	for (i=0;i<PX_COUNTOF(keyBoard->Keys);i++)
	{
		//draw rect

		if (keyBoard->Keys[i].bDown||keyBoard->Keys[i].bhold)
		{
			PX_GeoDrawRect(psurface,oftx+keyBoard->Keys[i].x,ofty+keyBoard->Keys[i].y,oftx+keyBoard->Keys[i].x+keyBoard->Keys[i].width,ofty+keyBoard->Keys[i].y+keyBoard->Keys[i].height,keyBoard->pushColor);
		}
		else if (keyBoard->Keys[i].bCursor)
		{
			PX_GeoDrawRect(psurface,oftx+keyBoard->Keys[i].x,ofty+keyBoard->Keys[i].y,oftx+keyBoard->Keys[i].x+keyBoard->Keys[i].width,ofty+keyBoard->Keys[i].y+keyBoard->Keys[i].height,keyBoard->cursorColor);
		}

		PX_GeoDrawBorder(psurface,oftx+keyBoard->Keys[i].x,ofty+keyBoard->Keys[i].y,oftx+keyBoard->Keys[i].x+keyBoard->Keys[i].width,ofty+keyBoard->Keys[i].y+keyBoard->Keys[i].height,1,keyBoard->borderColor);

		PX_FontModuleDrawText(psurface,PX_NULL,oftx+keyBoard->Keys[i].x+2,ofty+keyBoard->Keys[i].y+2,PX_ALIGN_LEFTTOP,keyBoard->Keys[i].d_key,keyBoard->borderColor);
	}

}

px_void PX_Object_VirtualNumberKeyBoardOnCursorUp(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	px_float x=PX_Object_Event_GetCursorX(e);
	px_float y=PX_Object_Event_GetCursorY(e);
	px_int i;
	PX_Object_VirtualNumberKeyBoard *pkb=(PX_Object_VirtualNumberKeyBoard *)pObject->pObject;
	if (!PX_ObjectIsPointInRegion(pObject,(px_float)x,(px_float)y))
	{
		return;
	}

	for (i=0;i<PX_COUNTOF(pkb->Keys);i++)
	{
		pkb->Keys[i].bDown=PX_FALSE;
	}
}

px_void PX_Object_VirtualNumberKeyBoardOnCursorMove(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	px_float x=PX_Object_Event_GetCursorX(e);
	px_float y=PX_Object_Event_GetCursorY(e);
	px_int i;
	px_int kh=(px_int)(pObject->Height/5);
	PX_Object_VirtualNumberKeyBoard *pkb=(PX_Object_VirtualNumberKeyBoard *)pObject->pObject;
	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;

	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;



	x=(PX_Object_Event_GetCursorX(e)-objx);
	y=(PX_Object_Event_GetCursorY(e)-objy);


	for (i=0;i<PX_COUNTOF(pkb->Keys);i++)
	{
		if(!pkb->Keys[i].bDown)
		{
			pkb->Keys[i].bCursor=PX_FALSE;
		}
		else
		{
			if (!PX_isPointInRect(PX_POINT((px_float)x,(px_float)y,0),PX_RECT((px_float)pkb->Keys[i].x,(px_float)pkb->Keys[i].y,(px_float)pkb->Keys[i].width,(px_float)pkb->Keys[i].height)))
				pkb->Keys[i].bDown=PX_FALSE;
		}
	}

	if (y<kh)
	{
		for (i=0;i<=3;i++)
		{
			if (PX_isPointInRect(PX_POINT((px_float)x,(px_float)y,0),PX_RECT((px_float)pkb->Keys[i].x,(px_float)pkb->Keys[i].y,(px_float)pkb->Keys[i].width,(px_float)pkb->Keys[i].height)))
			{
				pkb->Keys[i].bCursor=PX_TRUE;
				break;
			}
		}
	}
	else if (y<2*kh)
	{
		for (i=4;i<=7;i++)
		{
			if (PX_isPointInRect(PX_POINT((px_float)x,(px_float)y,0),PX_RECT((px_float)pkb->Keys[i].x,(px_float)pkb->Keys[i].y,(px_float)pkb->Keys[i].width,(px_float)pkb->Keys[i].height)))
			{
				pkb->Keys[i].bCursor=PX_TRUE;
				break;
			}
		}
	}
	else if (y<3*kh)
	{
		for (i=7;i<=10;i++)
		{
			if (PX_isPointInRect(PX_POINT((px_float)x,(px_float)y,0),PX_RECT((px_float)pkb->Keys[i].x,(px_float)pkb->Keys[i].y,(px_float)pkb->Keys[i].width,(px_float)pkb->Keys[i].height)))
			{
				pkb->Keys[i].bCursor=PX_TRUE;
				break;
			}
		}
	}
	else if (y<4*kh)
	{
		for (i=11;i<=14;i++)
		{
			if (PX_isPointInRect(PX_POINT((px_float)x,(px_float)y,0),PX_RECT((px_float)pkb->Keys[i].x,(px_float)pkb->Keys[i].y,(px_float)pkb->Keys[i].width,(px_float)pkb->Keys[i].height)))
			{
				pkb->Keys[i].bCursor=PX_TRUE;
				break;
			}
		}
	}
	else
	{
		for (i=14;i<=16;i++)
		{
			if (PX_isPointInRect(PX_POINT((px_float)x,(px_float)y,0),PX_RECT((px_float)pkb->Keys[i].x,(px_float)pkb->Keys[i].y,(px_float)pkb->Keys[i].width,(px_float)pkb->Keys[i].height)))
			{
				pkb->Keys[i].bCursor=PX_TRUE;
				break;
			}
		}
	}



}

px_void PX_Object_VirtualNumberKeyBoardOnCursorDown(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	px_float x=PX_Object_Event_GetCursorX(e);
	px_float y=PX_Object_Event_GetCursorY(e);
	px_int i;
	px_int kh=(px_int)(pObject->Height/5);
	PX_Object_Event stringEvent;
	PX_Object_VirtualKeyBoard *pkb=(PX_Object_VirtualKeyBoard *)pObject->pObject;
	PX_Object_Event ke;
	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;

	px_char strEve[2]={0};

	stringEvent.Event=PX_OBJECT_EVENT_STRING;
	PX_Object_Event_SetStringPtr(&stringEvent,strEve);


	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;

	if (!PX_ObjectIsPointInRegion(pObject,(px_float)x,(px_float)y))
	{
		return;
	}

	x=(PX_Object_Event_GetCursorX(e)-objx);
	y=(PX_Object_Event_GetCursorY(e)-objy);
	ke.Event=PX_OBJECT_EVENT_KEYDOWN;

	if (y<kh)
	{
		for (i=0;i<=3;i++)
		{
			if (PX_isPointInRect(PX_POINT((px_float)x,(px_float)y,0),PX_RECT((px_float)pkb->Keys[i].x,(px_float)pkb->Keys[i].y,(px_float)pkb->Keys[i].width,(px_float)pkb->Keys[i].height)))
			{
				pkb->Keys[i].bDown=PX_TRUE;
				if (i!=0)
				{
					
					PX_Object_Event_SetKeyDown(&ke,pkb->Keys[i].u_key[0]);
					PX_ObjectExecuteEvent(pObject,ke);
					strEve[0]=pkb->Keys[i].u_key[0];
					PX_ObjectPostEvent(pkb->LinkerObject,stringEvent);
				}
				else
				{
					PX_Object_Event_SetKeyDown(&ke,8);
					PX_ObjectExecuteEvent(pObject,ke);
					strEve[0]=8;
					PX_ObjectPostEvent(pkb->LinkerObject,stringEvent);
				}
				
				break;
			}
		}
	}
	else if (y<2*kh)
	{
		for (i=4;i<=7;i++)
		{
			if (PX_isPointInRect(PX_POINT((px_float)x,(px_float)y,0),PX_RECT((px_float)pkb->Keys[i].x,(px_float)pkb->Keys[i].y,(px_float)pkb->Keys[i].width,(px_float)pkb->Keys[i].height)))
			{
				PX_Object_Event_SetKeyDown(&ke,pkb->Keys[i].u_key[0]);
				PX_ObjectExecuteEvent(pObject,ke);
				strEve[0]=pkb->Keys[i].u_key[0];
				PX_ObjectPostEvent(pkb->LinkerObject,stringEvent);
				pkb->Keys[i].bDown=PX_TRUE;

				break;
			}
		}
	}
	else if (y<3*kh)
	{
		for (i=7;i<=10;i++)
		{
			if (PX_isPointInRect(PX_POINT((px_float)x,(px_float)y,0),PX_RECT((px_float)pkb->Keys[i].x,(px_float)pkb->Keys[i].y,(px_float)pkb->Keys[i].width,(px_float)pkb->Keys[i].height)))
			{
				PX_Object_Event_SetKeyDown(&ke,pkb->Keys[i].u_key[0]);
				PX_ObjectExecuteEvent(pObject,ke);
				strEve[0]=pkb->Keys[i].u_key[0];
				PX_ObjectPostEvent(pkb->LinkerObject,stringEvent);
				pkb->Keys[i].bDown=PX_TRUE;
				break;
			}
		}
	}
	else if (y<4*kh)
	{
		for (i=11;i<=14;i++)
		{
			if (PX_isPointInRect(PX_POINT((px_float)x,(px_float)y,0),PX_RECT((px_float)pkb->Keys[i].x,(px_float)pkb->Keys[i].y,(px_float)pkb->Keys[i].width,(px_float)pkb->Keys[i].height)))
			{
				if (i!=14)
				{
					PX_Object_Event_SetKeyDown(&ke,pkb->Keys[i].u_key[0]);
					PX_ObjectExecuteEvent(pObject,ke);
					strEve[0]=pkb->Keys[i].u_key[0];
					PX_ObjectPostEvent(pkb->LinkerObject,stringEvent);
				}
				else
				{
					PX_Object_Event_SetKeyDown(&ke,'\r');
					PX_ObjectExecuteEvent(pObject,ke);
					strEve[0]='\r';
					PX_ObjectPostEvent(pkb->LinkerObject,stringEvent);
				}
				pkb->Keys[i].bDown=PX_TRUE;
				break;
			}
		}
	}
	else
	{
		for (i=14;i<=16;i++)
		{
			if (PX_isPointInRect(PX_POINT((px_float)x,(px_float)y,0),PX_RECT((px_float)pkb->Keys[i].x,(px_float)pkb->Keys[i].y,(px_float)pkb->Keys[i].width,(px_float)pkb->Keys[i].height)))
			{
				if (i!=14)
				{
					PX_Object_Event_SetKeyDown(&ke,pkb->Keys[i].u_key[0]);
					PX_ObjectExecuteEvent(pObject,ke);
					strEve[0]=pkb->Keys[i].u_key[0];
					PX_ObjectPostEvent(pkb->LinkerObject,stringEvent);
				}
				else
				{
					PX_Object_Event_SetKeyDown(&ke,'\r');
					PX_ObjectExecuteEvent(pObject,ke);
					strEve[0]='\r';
					PX_ObjectPostEvent(pkb->LinkerObject,stringEvent);
				}
				pkb->Keys[i].bDown=PX_TRUE;
				break;
			}
		}
	}
}
PX_Object* PX_Object_VirtualNumberKeyBoardCreate(px_memorypool *mp, PX_Object *Parent,px_int x,px_int y,px_int width,px_int height)
{
	px_int i;
	px_int kw,kh;
	px_int xoffset,yoffset;
	PX_Object_VirtualNumberKeyBoard keyboard;
	px_char line1_lower[]=" /*-";
	px_char line2_lower[]="789+";
	px_char line3_lower[]="456";
	px_char line4_lower[]="123 ";
	px_char line5_lower[]="0.";

	PX_Object *keyboardObject;

	if (width<16*4||height<16*5)
	{
		return PX_NULL;
	}
	PX_memset(&keyboard,0,sizeof(PX_Object_VirtualNumberKeyBoard));

	kw=width/4;
	kh=height/5;

	xoffset=0;
	yoffset=0;

	for (i=0;i<=3;i++)
	{
		keyboard.Keys[i].d_key[0]=line1_lower[i];
		keyboard.Keys[i].u_key[0]=line1_lower[i];
		keyboard.Keys[i].x=xoffset;
		keyboard.Keys[i].y=yoffset;
		keyboard.Keys[i].width=kw;
		keyboard.Keys[i].height=kh;
		xoffset+=kw;
	}
	PX_strset(keyboard.Keys[0].d_key,"<--");
	PX_strset(keyboard.Keys[0].u_key,"<--");
	xoffset=0;
	yoffset=1*kh;

	for (i=4;i<=7;i++)
	{
		keyboard.Keys[i].d_key[0]=line2_lower[i-4];
		keyboard.Keys[i].u_key[0]=line2_lower[i-4];
		keyboard.Keys[i].x=xoffset;
		keyboard.Keys[i].y=yoffset;
		keyboard.Keys[i].width=kw;
		keyboard.Keys[i].height=kh;
		xoffset+=keyboard.Keys[i].width;
	}
	keyboard.Keys[7].height*=2;

	xoffset=0;
	yoffset=2*kh;
	for (i=8;i<=10;i++)
	{
		keyboard.Keys[i].d_key[0]=line3_lower[i-8];
		keyboard.Keys[i].u_key[0]=line3_lower[i-8];
		keyboard.Keys[i].x=xoffset;
		keyboard.Keys[i].y=yoffset;
		keyboard.Keys[i].width=kw;
		keyboard.Keys[i].height=kh;
		xoffset+=keyboard.Keys[i].width;
	}

	xoffset=0;
	yoffset=3*kh;
	for (i=11;i<=14;i++)
	{
		keyboard.Keys[i].d_key[0]=line4_lower[i-11];
		keyboard.Keys[i].u_key[0]=line4_lower[i-11];
		keyboard.Keys[i].x=xoffset;
		keyboard.Keys[i].y=yoffset;
		keyboard.Keys[i].width=kw;
		keyboard.Keys[i].height=kh;
		xoffset+=keyboard.Keys[i].width;
	}

	
	PX_strset(keyboard.Keys[14].d_key,"Enter");
	PX_strset(keyboard.Keys[14].u_key,"Enter");
	keyboard.Keys[14].height*=2;

	xoffset=0;
	yoffset=4*kh;
	
	keyboard.Keys[15].d_key[0]='0';
	keyboard.Keys[15].u_key[0]='0';
	keyboard.Keys[15].x=xoffset;
	keyboard.Keys[15].y=yoffset;
	keyboard.Keys[15].width=kw*2;
	keyboard.Keys[15].height=kh;
	xoffset+=keyboard.Keys[15].width;


	keyboard.Keys[16].d_key[0]='.';
	keyboard.Keys[16].u_key[0]='.';
	keyboard.Keys[16].x=xoffset;
	keyboard.Keys[16].y=yoffset;
	keyboard.Keys[16].width=kw;
	keyboard.Keys[16].height=kh;
	xoffset+=keyboard.Keys[16].width;

	keyboard.cursorColor=PX_COLOR(255,192,192,192);
	keyboard.pushColor=PX_COLOR(255,128,128,128);
	keyboard.backgroundColor=PX_COLOR(255,255,255,255);
	keyboard.borderColor=PX_COLOR(255,0,0,0);
	keyboard.LinkerObject=PX_NULL;
	keyboardObject=PX_ObjectCreateEx(mp,Parent,(px_float)x,(px_float)y,0,(px_float)width,(px_float)height,0,PX_OBJECT_TYPE_VNKEYBOARD,PX_NULL,PX_Object_VirtualNumberKeyBoardRender,PX_NULL,&keyboard,sizeof(PX_Object_VirtualNumberKeyBoard));

	PX_ObjectRegisterEvent(keyboardObject,PX_OBJECT_EVENT_CURSORMOVE,PX_Object_VirtualNumberKeyBoardOnCursorMove,PX_NULL);
	PX_ObjectRegisterEvent(keyboardObject,PX_OBJECT_EVENT_CURSORDRAG,PX_Object_VirtualNumberKeyBoardOnCursorMove,PX_NULL);
	PX_ObjectRegisterEvent(keyboardObject,PX_OBJECT_EVENT_CURSORDOWN,PX_Object_VirtualNumberKeyBoardOnCursorDown,PX_NULL);
	PX_ObjectRegisterEvent(keyboardObject,PX_OBJECT_EVENT_CURSORUP,PX_Object_VirtualNumberKeyBoardOnCursorUp,PX_NULL);
	return keyboardObject;
}

px_void PX_Object_VirtualNumberKeyBoardSetBackgroundColor(PX_Object *pObject,px_color Color)
{
	PX_Object_VirtualNumberKeyBoard *pkb=PX_Object_GetVirtualNumberKeyBoard(pObject);
	if (pkb)
	{
		pkb->backgroundColor=Color;
	}
}

px_void PX_Object_VirtualNumberKeyBoardSetBorderColor(PX_Object *pObject,px_color Color)
{
	PX_Object_VirtualNumberKeyBoard *pkb=PX_Object_GetVirtualNumberKeyBoard(pObject);
	if (pkb)
	{
		pkb->borderColor=Color;
	}
}

px_void PX_Object_VirtualNumberKeyBoardSetCursorColor(PX_Object *pObject,px_color Color)
{
	PX_Object_VirtualNumberKeyBoard *pkb=PX_Object_GetVirtualNumberKeyBoard(pObject);
	if (pkb)
	{
		pkb->cursorColor=Color;
	}
}

px_void PX_Object_VirtualNumberKeyBoardSetPushColor(PX_Object *pObject,px_color Color)
{
	PX_Object_VirtualNumberKeyBoard *pkb=PX_Object_GetVirtualNumberKeyBoard(pObject);
	if (pkb)
	{
		pkb->pushColor=Color;
	}
}

px_char PX_Object_VirtualNumberKeyBoardGetCode(PX_Object *pObject)
{
	PX_Object_VirtualNumberKeyBoard *pkb=PX_Object_GetVirtualNumberKeyBoard(pObject);
	if (pkb)
	{
		px_char ch= pkb->functionCode;
		pkb->functionCode=0;
		return ch;
	}
	return 0;
}

px_void PX_Object_VirtualNumberKeyBoardSetLinkerObject(PX_Object *pObject,PX_Object *linker)
{
	PX_Object_VirtualNumberKeyBoard *pkb=PX_Object_GetVirtualNumberKeyBoard(pObject);
	if (pkb)
	{
		pkb->LinkerObject=linker;
	}
}

//////////////////////////////////////////////////////////////////////////
//
static px_int PX_Object_CoordinatesDichotomy(px_double *p,px_int Size,px_double Mark)
{
	px_int Minx,Maxx;
	px_int Index;
	if (Mark<=p[0])
	{
		return 0;
	}
	if (Mark>=p[Size-1])
	{
		return Size-1;
	}

	Minx=0;
	Maxx=Size-1;

	

	while (Maxx-Minx>1)
	{
		Index=(Maxx+Minx)/2;
		if (p[Index]>=Mark)
		{
			Maxx=Index;
		}
		else
		{
			Minx=Index;
		}
	}

	if (Index==Size-1)
	{
		return Minx;
	}


	if (Mark-p[Index]<p[Index+1]-Mark)
	{
		return Minx;
	}
	else
	{
		return Minx+1;
	}

}




PX_Object_Coordinates *PX_Object_GetCoordinates(PX_Object *pObject)
{
	if (pObject->Type==PX_OBJECT_TYPE_COORDINATE)
	{
		return (PX_Object_Coordinates *)pObject->pObject;
	}
	return PX_NULL;
}

px_void PX_Object_CoordinatesSetMinVerticalPixelDividing(PX_Object *pObj,px_int val)
{
	PX_Object_Coordinates *pcd=PX_Object_GetCoordinates(pObj);
	if (!pcd)
	{
		PX_ASSERT();
		return;
	}
	pcd->MinVerticalPixelDividing=val;
}

px_void PX_Object_CoordinatesSetMinHorizontalPixelDividing(PX_Object *pObj,px_int val)
{
	PX_Object_Coordinates *pcd=PX_Object_GetCoordinates(pObj);
	if (!pcd)
	{
		PX_ASSERT();
		return;
	}
	pcd->MinHorizontalPixelDividing=val;
}

px_void PX_Object_CoordinatesSetHorizontalDividing(PX_Object *pObj,px_int Count)
{
	PX_Object_Coordinates *pcd=PX_Object_GetCoordinates(pObj);
	if (!pcd)
	{
		PX_ASSERT();
		return;
	}
	pcd->HorizontalDividing=Count;
}

px_void PX_Object_CoordinatesSetLeftVerticalDividing(PX_Object *pObj,px_int Count)
{
	PX_Object_Coordinates *pcd=PX_Object_GetCoordinates(pObj);
	if (!pcd)
	{
		PX_ASSERT();
		return;
	}
	pcd->LeftVerticalDividing=Count;
}

px_void PX_Object_CoordinatesSetRightVerticalDividing(PX_Object *pObj,px_int Count)
{
	PX_Object_Coordinates *pcd=PX_Object_GetCoordinates(pObj);
	if (!pcd)
	{
		PX_ASSERT();
		return;
	}
	pcd->RightVerticalDividing=Count;
}



px_void PX_Object_CoordinatesSetStyle(PX_Object *pObj,PX_OBJECT_COORDINATES_LINEMODE mode)
{
	PX_Object_Coordinates *pcd=PX_Object_GetCoordinates(pObj);
	if (!pcd)
	{
		PX_ASSERT();
		return;
	}
	pcd->LineMode=mode;
}



px_void PX_Object_CoordinatesSetScaleEnabled(PX_Object *pObject,px_bool Enabled)
{
	PX_Object_Coordinates *pcd=PX_Object_GetCoordinates(pObject);
	if (!pcd)
	{
		PX_ASSERT();
		return;
	}
	pcd->ScaleEnabled=Enabled;
}

px_void PX_Object_CoordinatesSetGuidesVisible(PX_Object *pObject,px_bool Visible)
{
	PX_Object_Coordinates *pcd=PX_Object_GetCoordinates(pObject);
	if (!pcd)
	{
		PX_ASSERT();
		return;
	}
	pcd->ShowGuides=Visible;
}

px_void PX_Object_CoordinatesSetGuidesShowMode(PX_Object *pObject,PX_OBJECT_COORDINATES_GUIDESSHOWMODE mode)
{
	PX_Object_Coordinates *pcd=PX_Object_GetCoordinates(pObject);
	if (!pcd)
	{
		PX_ASSERT();
		return;
	}
	pcd->guidesShowMode=mode;
}


px_void PX_Object_CoordinatesShowHelpLine(PX_Object *pObject,px_bool show)
{
	PX_Object_Coordinates *pcd=PX_Object_GetCoordinates(pObject);
	if (!pcd)
	{
		PX_ASSERT();
		return;
	}
	pcd->ShowHelpLine=show;
}

px_void PX_Object_CoordinatesSetDataLineWidth(PX_Object *pObject,px_float linewidth)
{
	PX_Object_Coordinates *pcd=PX_Object_GetCoordinates(pObject);
	if (!pcd)
	{
		PX_ASSERT();
		return;
	}
	pcd->DataLineWidth=linewidth;
}

void PX_Object_CoordinatesSetDataShow(PX_Object *pObject,px_int index,px_bool show)
{
	PX_Object_Coordinates *pcd=PX_Object_GetCoordinates(pObject);
	PX_Object_CoordinateData *pData;
	if (!pcd)
	{
		PX_ASSERT();
		return;
	}
	if (index>=pcd->vData.size)
	{
		return;
	}
	pData=PX_VECTORAT(PX_Object_CoordinateData,&pcd->vData,index);
	pData->Visibled=show;
}

px_void PX_Object_CoordinatesSetGuidesLineWidth(PX_Object *pObject,px_float linewidth)
{
	PX_Object_Coordinates *pcd=PX_Object_GetCoordinates(pObject);
	if (!pcd)
	{
		PX_ASSERT();
		return;
	}
	pcd->GuidesLineWidth=linewidth;
}

px_void PX_Object_CoordinatesSetGuidesLineColor(PX_Object *pObject,px_color clr)
{
	PX_Object_Coordinates *pcd=PX_Object_GetCoordinates(pObject);
	if (!pcd)
	{
		PX_ASSERT();
		return;
	}
	pcd->helpLineColor=clr;
}

px_void PX_Object_CoordinatesSetTitleFontSize(PX_Object *pObject,px_int size)
{
	PX_Object_Coordinates *pcd=PX_Object_GetCoordinates(pObject);
	if (!pcd)
	{
		PX_ASSERT();
		return;
	}
	pcd->FontSize=size;
}

px_void PX_Object_CoordinatesSetTitleFontColor(PX_Object *pObject,px_color clr)
{
	PX_Object_Coordinates *pcd=PX_Object_GetCoordinates(pObject);
	if (!pcd)
	{
		PX_ASSERT();
		return;
	}
	pcd->FontColor=clr;
}

px_void PX_Object_CoordinatesSetDashLineColor(PX_Object *pObject,px_color clr)
{
	PX_Object_Coordinates *pcd=PX_Object_GetCoordinates(pObject);
	if (!pcd)
	{
		PX_ASSERT();
		return;
	}
	pcd->DashColor=clr;
}

px_void PX_Object_CoordinatesSetLeftTextShow(PX_Object *pObject,px_bool bshow)
{
	PX_Object_Coordinates *pcd=PX_Object_GetCoordinates(pObject);
	if (!pcd)
	{
		PX_ASSERT();
		return;
	}
	pcd->LeftTextShow=bshow;
}

px_void PX_Object_CoordinatesSetRightTextShow(PX_Object *pObject,px_bool bshow)
{
	PX_Object_Coordinates *pcd=PX_Object_GetCoordinates(pObject);
	if (!pcd)
	{
		PX_ASSERT();
		return;
	}
	pcd->RightTextShow=bshow;
}

px_void PX_Object_CoordinatesSetHorizontalTextShow(PX_Object *pObject,px_bool bshow)
{
	PX_Object_Coordinates *pcd=PX_Object_GetCoordinates(pObject);
	if (!pcd)
	{
		PX_ASSERT();
		return;
	}
	pcd->HorizontalTextShow=bshow;
}

px_void PX_Object_CoordinatesSetFloatFlagFormatHorizontal(PX_Object *pObject,const char *fmt)
{
	PX_Object_Coordinates *pcd=PX_Object_GetCoordinates(pObject);
	if (!pcd)
	{
		PX_ASSERT();
		return;
	}
	pcd->FloatFlagFormat_H=fmt;
}

px_void PX_Object_CoordinatesSetIntFlagFormatHorizontal(PX_Object *pObject,const char *fmt)
{
	PX_Object_Coordinates *pcd=PX_Object_GetCoordinates(pObject);
	if (!pcd)
	{
		PX_ASSERT();
		return;
	}
	pcd->IntFlagFormat_H=fmt;
}

px_void PX_Object_CoordinatesSetFloatFlagFormatVerticalLeft(PX_Object *pObject,const char *fmt)
{
	PX_Object_Coordinates *pcd=PX_Object_GetCoordinates(pObject);
	if (!pcd)
	{
		PX_ASSERT();
		return;
	}
	pcd->FloatFlagFormat_L=fmt;
}

px_void PX_Object_CoordinatesSetIntFlagFormatVerticalLeft(PX_Object *pObject,const char *fmt)
{
	PX_Object_Coordinates *pcd=PX_Object_GetCoordinates(pObject);
	if (!pcd)
	{
		PX_ASSERT();
		return;
	}
	pcd->IntFlagFormat_L=fmt;
}

px_void PX_Object_CoordinatesSetFloatFlagFormatVerticalRight(PX_Object *pObject,const char *fmt)
{
	PX_Object_Coordinates *pcd=PX_Object_GetCoordinates(pObject);
	if (!pcd)
	{
		PX_ASSERT();
		return;
	}
	pcd->FloatFlagFormat_R=fmt;
}

px_void PX_Object_CoordinatesSetIntFlagFormatVericalRight(PX_Object *pObject,const char *fmt)
{
	PX_Object_Coordinates *pcd=PX_Object_GetCoordinates(pObject);
	if (!pcd)
	{
		PX_ASSERT();
		return;
	}
	pcd->IntFlagFormat_R=fmt;
}

px_void PX_Object_CoordinatesSetLeftTextMode(PX_Object *pObject,PX_OBJECT_COORDINATES_TEXT_DISPLAYMODE mode)
{
	PX_Object_Coordinates *pcd=PX_Object_GetCoordinates(pObject);
	if (!pcd)
	{
		PX_ASSERT();
		return;
	}
	pcd->leftTextDisplayMode=mode;
}

px_void PX_Object_CoordinatesSetRightTextMode(PX_Object *pObject,PX_OBJECT_COORDINATES_TEXT_DISPLAYMODE mode)
{
	PX_Object_Coordinates *pcd=PX_Object_GetCoordinates(pObject);
	if (!pcd)
	{
		PX_ASSERT();
		return;
	}
	pcd->RightTextDisplayMode=mode;
}

px_void PX_Object_CoordinatesSetHorizontalMin(PX_Object *pObject,px_double Min)
{
	PX_Object_Coordinates *pcd=PX_Object_GetCoordinates(pObject);
	if (!pcd)
	{
		PX_ASSERT();
		return;
	}
	pcd->HorizontalRangeMin=Min;
}

px_void PX_Object_CoordinatesSetHorizontalMax(PX_Object *pObject,px_double Max)
{
	PX_Object_Coordinates *pcd=PX_Object_GetCoordinates(pObject);
	if (!pcd)
	{
		PX_ASSERT();
		return;
	}
	pcd->HorizontalRangeMax=Max;
}

px_void PX_Object_CoordinatesSetLeftVerticalMin(PX_Object *pObject,px_double Min)
{
	PX_Object_Coordinates *pcd=PX_Object_GetCoordinates(pObject);
	if (!pcd)
	{
		PX_ASSERT();
		return;
	}
	pcd->LeftVerticalRangeMin=Min;
}

px_void PX_Object_CoordinatesSetLeftVerticalMax(PX_Object *pObject,px_double Max)
{
	PX_Object_Coordinates *pcd=PX_Object_GetCoordinates(pObject);
	if (!pcd)
	{
		PX_ASSERT();
		return;
	}
	pcd->LeftVerticalRangeMax=Max;
}

px_void PX_Object_CoordinatesSetRightVerticalMax(PX_Object *pObject,px_double Max)
{
	PX_Object_Coordinates *pcd=PX_Object_GetCoordinates(pObject);
	if (!pcd)
	{
		PX_ASSERT();
		return;
	}
	pcd->RightVerticalRangeMax=Max;
}

px_void PX_Object_CoordinatesSetRightVerticalMin(PX_Object *pObject,px_double Min)
{
	PX_Object_Coordinates *pcd=PX_Object_GetCoordinates(pObject);
	if (!pcd)
	{
		PX_ASSERT();
		return;
	}
	pcd->RightVerticalRangeMin=Min;
}

px_void PX_Object_CoordinatesSetBorderColor(PX_Object *pObject,px_color clr)
{
	PX_Object_Coordinates *pcd=PX_Object_GetCoordinates(pObject);
	if (pcd)
	{
		pcd->borderColor=clr;
	}
}

PX_Object_CoordinateData * PX_Object_CoordinatesGetCoordinateData(PX_Object *pObject,px_int index)
{
	PX_Object_Coordinates *pcd=PX_Object_GetCoordinates(pObject);
	if (index>pcd->vData.size-1)
	{
		return PX_NULL;
	}
	return PX_VECTORAT(PX_Object_CoordinateData,&pcd->vData,index);
}

px_int PX_Object_CoordinatesGetCoordinateWidth(PX_Object *pObject)
{
	PX_Object_Coordinates *pcd=PX_Object_GetCoordinates(pObject);

	return (px_int)(pObject->Width-pcd->LeftSpacer-pcd->RightSpacer);
}

px_int PX_Object_CoordinatesGetCoordinateHeight(PX_Object *pObject)
{
	PX_Object_Coordinates *pcd=PX_Object_GetCoordinates(pObject);

	return (px_int)(pObject->Height-pcd->TopSpacer-pcd->BottomSpacer);
}

void PX_Object_CoordinatesSetTitleTop(PX_Object *pObject,const px_char * title)
{
	PX_Object_Coordinates *pcd=PX_Object_GetCoordinates(pObject);
	if (pcd)
	{
		pcd->TopTitle=title;
	}
}

void PX_Object_CoordinatesSetTitleBottom(PX_Object *pObject,const px_char * title)
{
	PX_Object_Coordinates *pcd=PX_Object_GetCoordinates(pObject);
	if (pcd)
	{
		pcd->BottomTitle=title;
	}
}

void PX_Object_CoordinatesSetMarkValueEnabled(PX_Object *pObject,px_bool Enabled)
{
	PX_Object_Coordinates *pcd=PX_Object_GetCoordinates(pObject);
	pcd->MarkValueEnabled=Enabled;
}

void PX_Object_CoordinatesSetFontColor(PX_Object *pObject,px_color clr)
{
	PX_Object_Coordinates *pcd=PX_Object_GetCoordinates(pObject);
	if (pcd)
	{
		pcd->FontColor=clr;
	}
}

void PX_Object_CoordinatesClearContext(PX_Object *pObject)
{
	PX_Object_Coordinates *pcd=PX_Object_GetCoordinates(pObject);
	PX_Object_CoordinatesRestoreCoordinates(pObject);
	PX_VectorClear(&pcd->vData);
}

void PX_Object_CoordinatesClearFlagLine(PX_Object *pObject)
{
	PX_Object_Coordinates *pcd=PX_Object_GetCoordinates(pObject);
	PX_VectorClear(&pcd->vFlagLine);
}

void PX_Object_CoordinatesAddData(PX_Object *pObject,PX_Object_CoordinateData data)
{
	PX_Object_Coordinates *pcd=PX_Object_GetCoordinates(pObject);
	PX_VectorPushback(&pcd->vData,&data);
}

void PX_Object_CoordinatesAddCoordinateFlagLine(PX_Object *pObject,PX_Object_CoordinateFlagLine Line)
{
	PX_Object_Coordinates *pcd=PX_Object_GetCoordinates(pObject);
	PX_VectorPushback(&pcd->vFlagLine,&Line);
}


px_void PX_Object_CoordinatesSetMargin(PX_Object *pObject,px_int Left,px_int Right,px_int Top,px_int Bottom)
{
	PX_Object_Coordinates *pcd=PX_Object_GetCoordinates(pObject);
	pcd->LeftSpacer=Left;
	pcd->RightSpacer=Right;
	pcd->TopSpacer=Top;
	pcd->BottomSpacer=Bottom;
}

px_int  PX_Object_CoordinatesMapHorizontalValueToPixel(PX_Object *pObject,px_double val)
{
	PX_Object_Coordinates *pcd=PX_Object_GetCoordinates(pObject);
	px_double scale=PX_Object_CoordinatesGetCoordinateWidth(pObject)/(pcd->HorizontalRangeMax-pcd->HorizontalRangeMin);
	return (px_int)((val-pcd->HorizontalRangeMin)*scale)+pcd->LeftSpacer;
}

px_int  PX_Object_CoordinatesMapVerticalValueToPixel(PX_Object *pObject,px_double val,px_int Map)
{
	PX_Object_Coordinates *pcd=PX_Object_GetCoordinates(pObject);
	px_double scale;

	if(Map==PX_OBJECT_COORDINATEDATA_MAP_LEFT)
	{
		scale=PX_Object_CoordinatesGetCoordinateHeight(pObject)/(pcd->LeftVerticalRangeMax-pcd->LeftVerticalRangeMin);
		return (px_int)(pObject->Height-pcd->BottomSpacer-(px_int)((val-pcd->LeftVerticalRangeMin)*scale));
	}
	else if(Map==PX_OBJECT_COORDINATEDATA_MAP_RIGHT)
	{
		scale=PX_Object_CoordinatesGetCoordinateHeight(pObject)/(pcd->RightVerticalRangeMax-pcd->RightVerticalRangeMin);
		return (px_int)(pObject->Height-pcd->BottomSpacer-(px_int)((val-pcd->RightVerticalRangeMin)*scale));
	}

	return 0;

}

px_double  PX_Object_CoordinatesMapPixelValueToHorizontal(PX_Object *pObject,px_int Pixel)
{
	PX_Object_Coordinates *pcd=PX_Object_GetCoordinates(pObject);
	px_int XDisten=Pixel-pcd->LeftSpacer;
	return pcd->HorizontalRangeMin+XDisten*(pcd->HorizontalRangeMax-pcd->HorizontalRangeMin)/PX_Object_CoordinatesGetCoordinateWidth(pObject);

}

px_double  PX_Object_CoordinatesMapPixelValueToVertical(PX_Object *pObject,px_int Pixel,px_int Map)
{
	PX_Object_Coordinates *pcd=PX_Object_GetCoordinates(pObject);
	px_double Min,Max;
	if (Map==PX_OBJECT_COORDINATEDATA_MAP_LEFT)
	{
		Max=pcd->LeftVerticalRangeMax;
		Min=pcd->LeftVerticalRangeMin;
		return pcd->LeftVerticalRangeMin+(pObject->Height-pcd->BottomSpacer-Pixel)*(Max-Min)/PX_Object_CoordinatesGetCoordinateHeight(pObject);
	}

	if (Map==PX_OBJECT_COORDINATEDATA_MAP_RIGHT)
	{
		Max=pcd->RightVerticalRangeMax;
		Min=pcd->RightVerticalRangeMin;
		return pcd->RightVerticalRangeMin+(pObject->Height-pcd->BottomSpacer-Pixel)*(Max-Min)/PX_Object_CoordinatesGetCoordinateHeight(pObject);
	}

	if (Map==PX_OBJECT_COORDINATEDATA_MAP_HORIZONTAL)
	{
		Max=pcd->HorizontalRangeMax;
		Min=pcd->HorizontalRangeMin;
		return pcd->HorizontalRangeMin+(Pixel-pcd->LeftSpacer)*(Max-Min)/PX_Object_CoordinatesGetCoordinateWidth(pObject);
	}
	return 0;
}


px_void PX_Object_CoordinatesUpdate(PX_Object *pObject,px_uint elpased)
{

}


static px_void PX_Object_CoordinatesDrawFrameLine(px_surface *psurface,PX_Object *pObject)
{
	PX_Object_Coordinates *pcd=PX_Object_GetCoordinates(pObject);
	px_int offsetx;
	px_int offsety;
	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;

	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;

	offsetx=(px_int)objx;
	offsety=(px_int)objy;

	PX_GeoDrawLine(psurface,offsetx+pcd->LeftSpacer,offsety+pcd->TopSpacer,offsetx+(px_int)(objWidth-pcd->RightSpacer),offsety+pcd->TopSpacer,PX_OBJECT_COORDINATES_DEFAULT_FRAMELINE_WIDTH,pcd->borderColor);
	PX_GeoDrawLine(psurface,offsetx+pcd->LeftSpacer,offsety+pcd->TopSpacer,offsetx+pcd->LeftSpacer,offsety+(px_int)(objHeight-pcd->BottomSpacer),PX_OBJECT_COORDINATES_DEFAULT_FRAMELINE_WIDTH,pcd->borderColor);
	PX_GeoDrawLine(psurface,(px_int)(offsetx+objWidth-pcd->RightSpacer),offsety+(px_int)(objHeight-pcd->BottomSpacer),offsetx+(pcd->LeftSpacer),offsety+(px_int)objHeight-pcd->BottomSpacer,PX_OBJECT_COORDINATES_DEFAULT_FRAMELINE_WIDTH,pcd->borderColor);
	PX_GeoDrawLine(psurface,(px_int)(offsetx+objWidth-pcd->RightSpacer),offsety+(px_int)(objHeight-pcd->BottomSpacer),offsetx+(px_int)(objWidth-pcd->RightSpacer),offsety+pcd->TopSpacer,PX_OBJECT_COORDINATES_DEFAULT_FRAMELINE_WIDTH,pcd->borderColor);
}

static px_void PX_Object_CoordinatesDrawDashed(px_surface *psurface,PX_Object *pObject)
{
	PX_Object_Coordinates *pcd=PX_Object_GetCoordinates(pObject);
	px_double HorizontalInc=0;
	px_int i;
	px_double VerticalInc=0;
	px_int offsetx;
	px_int offsety;

	px_int Divid=PX_Object_CoordinatesGetCoordinateWidth(pObject)/pcd->MinHorizontalPixelDividing;

	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;

	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;

	offsetx=(px_int)objx;
	offsety=(px_int)objy;


	if (Divid>pcd->HorizontalDividing)
	{
		Divid=pcd->HorizontalDividing;
	}


	if(Divid>1)
	{
		HorizontalInc=(px_double)PX_Object_CoordinatesGetCoordinateWidth(pObject)/Divid;
		if(pcd->guidesShowMode==PX_OBJECT_COORDINATES_GUIDESSHOWMODE_ALL||pcd->guidesShowMode==PX_OBJECT_COORDINATES_GUIDESSHOWMODE_HORIZONTAL)
		{
			for (i=1;i<=Divid;i++)
			{
				PX_GeoDrawLine(psurface,\
					offsetx+(px_int)(pcd->LeftSpacer+i*HorizontalInc),offsety+pcd->TopSpacer,\
					offsetx+(px_int)(pcd->LeftSpacer+i*HorizontalInc),offsety+pcd->TopSpacer+PX_Object_CoordinatesGetCoordinateHeight(pObject),\
					1,\
					pcd->DashColor
					);
			}
		}
	}



	Divid=PX_Object_CoordinatesGetCoordinateHeight(pObject)/pcd->MinVerticalPixelDividing;
	if (pcd->LeftVerticalDividing>=pcd->RightVerticalDividing)
	{
		if (Divid>pcd->LeftVerticalDividing)
		{
			Divid=pcd->LeftVerticalDividing;
		}
	}
	else
	{
		if (Divid>pcd->RightVerticalDividing)
		{
			Divid=pcd->RightVerticalDividing;
		}
	}

	if(Divid>1)
	{
		VerticalInc=(px_double)PX_Object_CoordinatesGetCoordinateHeight(pObject)/Divid;
		if(pcd->guidesShowMode==PX_OBJECT_COORDINATES_GUIDESSHOWMODE_ALL||pcd->guidesShowMode==PX_OBJECT_COORDINATES_GUIDESSHOWMODE_VERTICAL)
		{	
			for (i=1;i<=Divid;i++)
			{
				PX_GeoDrawLine(psurface,\
					offsetx+(px_int)pcd->LeftSpacer,\
					offsety+(px_int)(objHeight-pcd->BottomSpacer-i*VerticalInc),\
					offsetx+(px_int)(pcd->LeftSpacer+PX_Object_CoordinatesGetCoordinateWidth(pObject)),\
					offsety+(px_int)(objHeight-pcd->BottomSpacer-i*VerticalInc),\
					1,\
					pcd->DashColor\
					);
			}
		}
	}
}

static px_void PX_Object_CoordinatesDrawDashText(px_surface *psurface,PX_Object *pObject)
{

	px_double HorizontalInc=0;
	px_double VerticalInc=0;
	PX_Object_Coordinates *pcd=PX_Object_GetCoordinates(pObject);
	px_double ValInc;
	px_int	   IsFloat;
	px_int i;
	px_int offsetx;
	px_int offsety;

	px_int Divid=PX_Object_CoordinatesGetCoordinateWidth(pObject)/pcd->MinHorizontalPixelDividing;

	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;

	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;

	offsetx=(px_int)objx;
	offsety=(px_int)objy;

	if (Divid>pcd->HorizontalDividing)
	{
		Divid=pcd->HorizontalDividing;
	}

	

	if(Divid>0&&pcd->HorizontalTextShow)
	{
		HorizontalInc=(px_double)PX_Object_CoordinatesGetCoordinateWidth(pObject)/Divid;
		ValInc=(pcd->HorizontalRangeMax-pcd->HorizontalRangeMin)/Divid;
		if (PX_ABS(ValInc-(px_int)ValInc)<0.000001f)
		{
			IsFloat=PX_FALSE;
		}
		else
		{
			IsFloat=PX_TRUE;
		}

		//paint for horizontal coordinates text 
		for (i=0;i<=Divid;i++)
		{
			px_char text[32]={0};

			if(IsFloat)
				PX_sprintf1(text,sizeof(text),pcd->FloatFlagFormat_H,PX_STRINGFORMAT_FLOAT((px_float)(pcd->HorizontalRangeMin+i*ValInc)));
			else
				PX_sprintf1(text,sizeof(text),pcd->IntFlagFormat_H,PX_STRINGFORMAT_INT((px_int)(pcd->HorizontalRangeMin+i*ValInc)));

			PX_FontModuleDrawText(psurface,PX_NULL,offsetx+(px_int)(pcd->LeftSpacer+(i)*HorizontalInc),offsety+(px_int)(pObject->Height-pcd->BottomSpacer+PX_OBJECT_COORDINATES_DEFAULT_FLAGTEXT_SPACER+2),PX_ALIGN_CENTER,text,pcd->FontColor);
		}
	}

	Divid=PX_Object_CoordinatesGetCoordinateHeight(pObject)/pcd->MinVerticalPixelDividing;
	if (pcd->LeftVerticalDividing>=pcd->RightVerticalDividing)
	{
		if (Divid>pcd->LeftVerticalDividing)
		{
			Divid=pcd->LeftVerticalDividing;
		}
	}
	else
	{
		if (Divid>pcd->RightVerticalDividing)
		{
			Divid=pcd->RightVerticalDividing;
		}
	}

	if(Divid>0)
	{
		VerticalInc=(px_double)PX_Object_CoordinatesGetCoordinateHeight(pObject)/Divid;


		//paint for Left vertical coordinates text 
		if (pcd->LeftTextShow)
		{
			ValInc=(pcd->LeftVerticalRangeMax-pcd->LeftVerticalRangeMin)/Divid;
			if (PX_ABS(ValInc-(px_int)ValInc)<0.000001f)
			{
				IsFloat=PX_FALSE;
			}
			else
			{
				IsFloat=PX_TRUE;
			}


			for (i=0;i<=Divid;i++)
			{
				px_char text[32]={0};

				switch (pcd->leftTextDisplayMode)
				{
				case PX_OBJECT_COORDINATES_TEXT_DISPLAYMODE_EXPONENTRAL:
					{
						px_double value=PX_ABS(pcd->LeftVerticalRangeMin)>PX_ABS(pcd->LeftVerticalRangeMax)?PX_ABS(pcd->LeftVerticalRangeMin):PX_ABS(pcd->LeftVerticalRangeMax);
						px_int e=0;
						px_double e10=1;
						while(value>10){value/=10,e++;e10*=10;};
						value=pcd->LeftVerticalRangeMin+i*ValInc;
						value/=e10;

						if (e>=3)
						{
							PX_sprintf2(text,sizeof(text),pcd->Exponential_Format,PX_STRINGFORMAT_FLOAT((px_float)(value)),PX_STRINGFORMAT_INT(e));
						}
						else
						{
							if(IsFloat)
								PX_sprintf1(text,sizeof(text),pcd->FloatFlagFormat_L,PX_STRINGFORMAT_FLOAT((px_float)(pcd->LeftVerticalRangeMin+i*ValInc)));
							else
								PX_sprintf1(text,sizeof(text),pcd->IntFlagFormat_L,PX_STRINGFORMAT_INT((px_int)(pcd->LeftVerticalRangeMin+i*ValInc)));
						}
						

					}
					break;
				default:
				case PX_OBJECT_COORDINATES_TEXT_DISPLAYMODE_NORMAL:
					{
						if(IsFloat)
							PX_sprintf1(text,sizeof(text),pcd->FloatFlagFormat_L,PX_STRINGFORMAT_FLOAT((px_float)(pcd->LeftVerticalRangeMin+i*ValInc)));
						else
							PX_sprintf1(text,sizeof(text),pcd->IntFlagFormat_L,PX_STRINGFORMAT_INT((px_int)(pcd->LeftVerticalRangeMin+i*ValInc)));
					}
					break;
				}

				PX_FontModuleDrawText(psurface,PX_NULL,offsetx+pcd->LeftSpacer-6,offsety+(px_int)(pObject->Height-pcd->BottomSpacer-(i)*VerticalInc-6),PX_ALIGN_RIGHTTOP,text,pcd->FontColor);
			}
		}
			
		
		//paint for Right vertical coordinates text
		if (pcd->RightTextShow)
		{
			ValInc=(pcd->RightVerticalRangeMax-pcd->RightVerticalRangeMin)/Divid;
			if (PX_ABS(ValInc-(px_int)ValInc)<0.000001f)
			{
				IsFloat=PX_FALSE;
			}
			else
			{
				IsFloat=PX_TRUE;
			}


			for (i=0;i<=Divid;i++)
			{
				px_char text[32]={0};

				switch (pcd->leftTextDisplayMode)
				{
				case PX_OBJECT_COORDINATES_TEXT_DISPLAYMODE_EXPONENTRAL:
					{
						px_double value=PX_ABS(pcd->RightVerticalRangeMin)>PX_ABS(pcd->RightVerticalRangeMax)?PX_ABS(pcd->RightVerticalRangeMin):PX_ABS(pcd->RightVerticalRangeMax);
						px_int e=0;
						px_double e10=1;
						while(value>10){value/=10,e++;e10*=10;};
						value=pcd->RightVerticalRangeMin+i*ValInc;
						value/=e10;

						if (e>=3)
						{
							PX_sprintf2(text,sizeof(text),pcd->Exponential_Format,PX_STRINGFORMAT_FLOAT((px_float)(value)),PX_STRINGFORMAT_INT(e));
						}
						else
						{
							if(IsFloat)
								PX_sprintf1(text,sizeof(text),pcd->FloatFlagFormat_L,PX_STRINGFORMAT_FLOAT((px_float)(pcd->LeftVerticalRangeMin+i*ValInc)));
							else
								PX_sprintf1(text,sizeof(text),pcd->IntFlagFormat_L,PX_STRINGFORMAT_INT((px_int)(pcd->LeftVerticalRangeMin+i*ValInc)));
						}
						

					}
					break;
				default:
				case PX_OBJECT_COORDINATES_TEXT_DISPLAYMODE_NORMAL:
					{
						if(IsFloat)
							PX_sprintf1(text,sizeof(text),pcd->FloatFlagFormat_L,PX_STRINGFORMAT_FLOAT((px_float)(pcd->RightVerticalRangeMin+i*ValInc)));
						else
							PX_sprintf1(text,sizeof(text),pcd->IntFlagFormat_L,PX_STRINGFORMAT_INT((px_int)(pcd->RightVerticalRangeMin+i*ValInc)));
					}
					break;
				}
				PX_FontModuleDrawText(psurface,PX_NULL,offsetx+(px_int)(pObject->Width-pcd->RightSpacer+PX_OBJECT_COORDINATES_DEFAULT_FLAGTEXT_SPACER),offsety+(px_int)(pObject->Height-pcd->BottomSpacer-(i)*VerticalInc-6),PX_ALIGN_LEFTTOP,text,pcd->FontColor);
			}
		}
	}
			
	
}

static px_void PX_Object_CoordinatesDrawTitle(px_surface *psurface,PX_Object *pObject)
{
	PX_Object_Coordinates *pcd=PX_Object_GetCoordinates(pObject);
	px_int offsetx;
	px_int offsety;
	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;

	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;

	offsetx=(px_int)objx;
	offsety=(px_int)objy;
	if (pcd->TopTitle[0])
	{
		PX_FontModuleDrawText(psurface,pcd->fontmodule,offsetx+(px_int)(pObject->Width/2),offsety,PX_ALIGN_MIDTOP,pcd->TopTitle,pcd->FontColor);
	}

	if (pcd->BottomTitle[0])
	{
		PX_FontModuleDrawText(psurface,pcd->fontmodule,offsetx+(px_int)(pObject->Width/2),offsety+(px_int)(pObject->Height-pcd->BottomSpacer+32),PX_ALIGN_MIDBOTTOM,pcd->BottomTitle,pcd->FontColor);
	}
	

// 	px_int x = this->width()-20;
// 
// 	px_int y = ( this->height()/ 2);
// 	painter.rotate(90);
// 	painter.drawText(y-30, -x+12,m_RightTitle );
// 
// 	painter.rotate(180);
// 	painter.drawText(-y-70, 45,m_LeftTitle);
}

static px_void PX_Object_CoordinatesDrawDataInfo(px_surface *psurface,PX_Object *pObject,px_double *Horizontal,px_double n,px_double *Vertical,px_int linewidth,px_int Size,px_int Map,px_color Color)
{
	px_double RangeMin,RangeMax;
	px_double x,y,w,btm,zeroy;
	px_int dx1,dy1,dx2,dy2;
	px_int i;
	px_int offsetx;
	px_int offsety;

	PX_Object_Coordinates *pcd=PX_Object_GetCoordinates(pObject);
	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;

	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;

	offsetx=(px_int)objx;
	offsety=(px_int)objy;

	if (Map==PX_OBJECT_COORDINATEDATA_MAP_LEFT)
	{
		RangeMin=pcd->LeftVerticalRangeMin;
		RangeMax=pcd->LeftVerticalRangeMax;
	}

	if (Map==PX_OBJECT_COORDINATEDATA_MAP_RIGHT)
	{
		RangeMin=pcd->RightVerticalRangeMin;
		RangeMax=pcd->RightVerticalRangeMax;
	}
	
	switch(pcd->LineMode)
	{
	case PX_OBJECT_COORDINATES_LINEMODE_PILLAR:
		{
			zeroy=PX_Object_CoordinatesMapVerticalValueToPixel(pObject,0,Map);
			for (i=0;i<Size;i++)
			{
				if (Horizontal[i]>=pcd->HorizontalRangeMin)
				{
					
					x=PX_Object_CoordinatesMapHorizontalValueToPixel(pObject,Horizontal[i-1])-pcd->DataPillarWidth/2;
					y=PX_Object_CoordinatesMapVerticalValueToPixel(pObject,Vertical[i]/n,Map);
					
					w=pcd->DataPillarWidth;
					btm=zeroy;

					if (y>btm)
					{
						double tem;
						tem=btm;
						btm=y;
						y=tem;
					}

					if (!(btm<pcd->TopSpacer||y>pObject->Height-pcd->BottomSpacer))
					{
						if (y<pcd->TopSpacer)
						{
							y=pcd->TopSpacer;
						}
						if (btm>pObject->Height-pcd->BottomSpacer)
						{
							btm=pObject->Height-pcd->BottomSpacer;
						}
						if (x-(px_int)x>0.1)
						{
							PX_GeoDrawRect(psurface,offsetx+(px_int)x,offsety+(px_int)y,offsetx+(px_int)(x+w),offsety+(px_int)(btm),Color);
						}
						else
						{
							PX_GeoDrawRect(psurface,offsetx+(px_int)x,offsety+(px_int)y,offsetx+(px_int)(x+w),offsety+(px_int)(btm),Color);
						}
					}
					
					
					
				}
				if(Horizontal[i]>pcd->HorizontalRangeMax)
					break;
			}
		}
		break;
	case PX_OBJECT_COORDINATES_LINEMODE_LINES:
	default:
		{
			for (i=0;i<Size;i++)
			{

				if (Horizontal[i]>=pcd->HorizontalRangeMin)
				{
					
					if(i>0){
						px_point pt1,pt2;

						dx1=PX_Object_CoordinatesMapHorizontalValueToPixel(pObject,Horizontal[i-1]);
						dy1=PX_Object_CoordinatesMapVerticalValueToPixel(pObject,Vertical[i-1]/n,Map);
						dx2=PX_Object_CoordinatesMapHorizontalValueToPixel(pObject,Horizontal[i]);
						dy2=PX_Object_CoordinatesMapVerticalValueToPixel(pObject,Vertical[i]/n,Map);

						if(PX_isLineCrossRect(PX_POINT((px_float)dx1,(px_float)dy1,0),PX_POINT((px_float)dx2,(px_float)dy2,0),PX_RECT((px_float)pcd->LeftSpacer,(px_float)pcd->TopSpacer,(px_float)PX_Object_CoordinatesGetCoordinateWidth(pObject),(px_float)PX_Object_CoordinatesGetCoordinateHeight(pObject)),&pt1,&pt2))
						{
								PX_GeoDrawLine(psurface,(px_int)(offsetx+pt1.x),(px_int)(offsety+pt1.y),(px_int)(offsetx+pt2.x),(px_int)(offsety+pt2.y),linewidth,Color);
						}
						
					}
				}
				if(Horizontal[i]>pcd->HorizontalRangeMax)
				{
					if(i>0){
						px_point pt1,pt2;

						dx1=PX_Object_CoordinatesMapHorizontalValueToPixel(pObject,Horizontal[i-1]);
						dy1=PX_Object_CoordinatesMapVerticalValueToPixel(pObject,Vertical[i-1]/n,Map);
						dx2=PX_Object_CoordinatesMapHorizontalValueToPixel(pObject,Horizontal[i]);
						dy2=PX_Object_CoordinatesMapVerticalValueToPixel(pObject,Vertical[i]/n,Map);
						if(PX_isLineCrossRect(PX_POINT((px_float)dx1,(px_float)dy1,0),PX_POINT((px_float)dx2,(px_float)dy2,0),PX_RECT((px_float)pcd->TopSpacer,(px_float)pcd->LeftSpacer,(px_float)PX_Object_CoordinatesGetCoordinateWidth(pObject),(px_float)PX_Object_CoordinatesGetCoordinateHeight(pObject)),&pt1,&pt2))
							PX_GeoDrawLine(psurface,(px_int)(offsetx+pt1.x),(px_int)(offsety+pt1.y),(px_int)(offsetx+pt2.x),(px_int)(offsety+pt2.y),linewidth,Color);
					}
					break;
				}
			}
		}
		break;

	}
}


static px_void PX_Object_CoordinatesDrawData(px_surface *psurface,PX_Object *pObject)
{
	PX_Object_Coordinates *pcd=PX_Object_GetCoordinates(pObject);
	px_int i;
	if(pcd->bDataUpdatePainter)
	{
		for ( i=0;i<pcd->vData.size;i++)
		{
			PX_Object_CoordinateData *pData=PX_VECTORAT(PX_Object_CoordinateData,&pcd->vData,i);
			if(pData->Visibled)
			{
				if (pData->Normalization==0)
				{
					PX_Object_CoordinatesDrawDataInfo(psurface,pObject,pData->MapHorizontalArray,1.0,pData->MapVerticalArray,pData->linewidth,pData->Size,pData->Map,pData->Color);
				}
				else
				{
					PX_Object_CoordinatesDrawDataInfo(psurface,pObject,pData->MapHorizontalArray,pData->Normalization,pData->MapVerticalArray,pData->linewidth,pData->Size,pData->Map,pData->Color);
				}
			}

		}
	}
}
static px_void PX_Object_CoordinatesDrawScaleDraging(px_surface *psurface,PX_Object *pObject)
{
	px_int offsetx;
	px_int offsety;
	PX_Object_Coordinates *pcd=PX_Object_GetCoordinates(pObject);
	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;

	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;

	offsetx=(px_int)objx;
	offsety=(px_int)objy;

	if (!pcd->bScaleDrag)
	{
		return;
	}

	if(pcd->DragingPoint.y<pcd->DragStartPoint.y&&pcd->DragingPoint.x<pcd->DragStartPoint.x)
		PX_GeoDrawRect(psurface,offsetx+(px_int)pcd->DragStartPoint.x,offsety+(px_int)pcd->DragStartPoint.y,offsetx+(px_int)pcd->DragingPoint.x,offsety+(px_int)pcd->DragingPoint.y,PX_COLOR(96,171,81,128));	
	else
		PX_GeoDrawRect(psurface,offsetx+(px_int)pcd->DragStartPoint.x,offsety+(px_int)pcd->DragStartPoint.y,offsetx+(px_int)pcd->DragingPoint.x,offsety+(px_int)pcd->DragingPoint.y,PX_COLOR(96,167,233,128));

}

static px_void PX_Object_CoordinatesDrawHelpLine(px_surface *psurface,PX_Object *pObject)
{
	px_int x,y;
	px_double value;
	PX_Object_Coordinates *pcd=PX_Object_GetCoordinates(pObject);
	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;

	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;

	if (!pcd->ShowHelpLine)
	{
		return;
	}
	//Draw X line

	
		value=PX_Object_CoordinatesMapPixelValueToVertical(pObject,pcd->helpLineX,PX_OBJECT_COORDINATEDATA_MAP_HORIZONTAL);

		if (value>pcd->HorizontalRangeMin&&value<pcd->HorizontalRangeMax)
		{
			px_int IsFloat;
			px_int X,Y;
			px_double ValInc=value;
			px_char text[16];
			x=PX_Object_CoordinatesMapHorizontalValueToPixel(pObject,value);

			PX_GeoDrawLine(psurface,(px_int)objx+x,(px_int)(objy+objHeight-pcd->BottomSpacer),(px_int)objx+x,(px_int)objy+pcd->TopSpacer,(px_int)1,pcd->helpLineColor);
			
			
				//Draw text

				if (PX_ABS(ValInc-(px_int)ValInc)<0.000001f)
				{
					IsFloat=PX_FALSE;
				}
				else
				{
					IsFloat=PX_TRUE;
				}


				X=PX_Object_CoordinatesMapHorizontalValueToPixel(pObject,value);
				Y=(px_int)(objHeight-pcd->BottomSpacer);

				if(IsFloat)
					PX_sprintf1(text,sizeof(text),"%1",PX_STRINGFORMAT_FLOAT((px_float)ValInc));
				else
					PX_sprintf1(text,sizeof(text),"%1",PX_STRINGFORMAT_INT((px_int)ValInc));

				PX_FontModuleDrawText(psurface,PX_NULL,(px_int)(objx+X-PX_OBJECT_COORDINATES_DEFAULT_FLAGTEXT_SPACER*1.5),(px_int)objy+Y,PX_ALIGN_LEFTTOP,text,pcd->helpLineColor);
			
		}
	


	//Draw YL line

	
		value=PX_Object_CoordinatesMapPixelValueToVertical(pObject,pcd->helpLineY,PX_OBJECT_COORDINATEDATA_MAP_LEFT);

		if (value>pcd->LeftVerticalRangeMin&&value<pcd->LeftVerticalRangeMax)
		{
			px_int IsFloat;
			px_int X,Y;
			px_double ValInc=value;
			px_char text[16];

			y=pcd->helpLineY;//PX_Object_CoordinatesMapVerticalValueToPixel(pObject,value,PX_OBJECT_COORDINATEDATA_MAP_LEFT);

			PX_GeoDrawLine(psurface,(px_int)objx+pcd->LeftSpacer,(px_int)objy+y,(px_int)(objx+objWidth-pcd->RightSpacer),(px_int)objy+y,1,pcd->helpLineColor);
			//Draw text
			
				if (PX_ABS(ValInc-(px_int)ValInc)<0.000000001)
				{
					IsFloat=PX_FALSE;
				}
				else
				{
					IsFloat=PX_TRUE;
				}


				X=pcd->LeftSpacer;
				Y=y;//PX_Object_CoordinatesMapVerticalValueToPixel(pObject,ValInc,PX_OBJECT_COORDINATEDATA_MAP_LEFT);

				if(IsFloat)
					PX_sprintf1(text,sizeof(text),"%1",PX_STRINGFORMAT_FLOAT((px_float)ValInc));
				else
					PX_sprintf1(text,sizeof(text),"%1",PX_STRINGFORMAT_INT((px_int)ValInc));

				PX_FontModuleDrawText(psurface,PX_NULL,(px_int)objx+X,(px_int)objy+Y-PX_OBJECT_COORDINATES_DEFAULT_FLAGTEXT_SPACER*2-1,PX_ALIGN_LEFTTOP,text,pcd->helpLineColor);
			
	}

	
	//Draw YR line
	
		value=PX_Object_CoordinatesMapPixelValueToVertical(pObject,pcd->helpLineY,PX_OBJECT_COORDINATEDATA_MAP_RIGHT);

		if (value>pcd->RightVerticalRangeMin&&value<pcd->RightVerticalRangeMax)
		{
			px_int IsFloat;
			px_int X,Y;
			px_double ValInc=value;
			px_char text[16];

			y=pcd->helpLineY;//PX_Object_CoordinatesMapVerticalValueToPixel(pObject,pflgl->Y,PX_OBJECT_COORDINATEDATA_MAP_RIGHT);
			PX_GeoDrawLine(psurface,(px_int)objx+pcd->LeftSpacer,(px_int)objy+y,(px_int)(objx+objWidth-pcd->RightSpacer),(px_int)objy+y,(px_int)(1),pcd->helpLineColor);

			
				if (PX_ABS(ValInc-(px_int)ValInc)<0.000001f)
				{
					IsFloat=PX_FALSE;
				}
				else
				{
					IsFloat=PX_TRUE;
				}

				X=(px_int)(objWidth-pcd->RightSpacer);
				Y=y;//PX_Object_CoordinatesMapVerticalValueToPixel(pObject,ValInc,PX_OBJECT_COORDINATEDATA_MAP_RIGHT);

				if(IsFloat)
					PX_sprintf1(text,sizeof(text),"%1",PX_STRINGFORMAT_FLOAT((px_float)ValInc));
				else
					PX_sprintf1(text,sizeof(text),"%1",PX_STRINGFORMAT_INT((px_int)ValInc));

				PX_FontModuleDrawText(psurface,PX_NULL,(px_int)objx+X,(px_int)objy+Y-PX_OBJECT_COORDINATES_DEFAULT_FLAGTEXT_SPACER*2-1,PX_ALIGN_LEFTTOP,text,pcd->helpLineColor);
			
	}
}



static px_void PX_Object_CoordinatesDrawFlagLine(px_surface *psurface,PX_Object *pObject)
{
	px_int x,y,i;
	PX_Object_Coordinates *pcd=PX_Object_GetCoordinates(pObject);
	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;

	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;

	for (i=0;i<pcd->vFlagLine.size;i++)
	{
		//Draw X line
		PX_Object_CoordinateFlagLine *pflgl=PX_VECTORAT(PX_Object_CoordinateFlagLine,&pcd->vFlagLine,i);
		if(pflgl->XYshow&PX_OBJECT_COORDINATEFLAGLINE_XSHOW)
		{
			if (pflgl->X>=pcd->HorizontalRangeMin&&pflgl->X<=pcd->HorizontalRangeMax)
			{
				px_int IsFloat;
				px_int X,Y;
				px_double ValInc=pflgl->X;
				px_char text[16];
				x=PX_Object_CoordinatesMapHorizontalValueToPixel(pObject,pflgl->X);

				PX_GeoDrawLine(psurface,(px_int)objx+x,(px_int)((px_int)objy+objHeight-pcd->BottomSpacer),(px_int)objx+x,(px_int)objy+pcd->TopSpacer,(px_int)pflgl->LineWidth,pflgl->color);
				//Draw text

				if (PX_ABS(ValInc-(px_int)ValInc)<0.000001f)
				{
					IsFloat=PX_FALSE;
				}
				else
				{
					IsFloat=PX_TRUE;
				}

				
				X=PX_Object_CoordinatesMapHorizontalValueToPixel(pObject,pflgl->X);
				Y=(px_int)(objHeight-pcd->BottomSpacer);

				if(IsFloat)
					PX_sprintf1(text,sizeof(text),"%1",PX_STRINGFORMAT_FLOAT((px_float)ValInc));
				else
					PX_sprintf1(text,sizeof(text),"%1",PX_STRINGFORMAT_INT((px_int)ValInc));

				PX_FontModuleDrawText(psurface,PX_NULL,(px_int)((px_int)objx+X-PX_OBJECT_COORDINATES_DEFAULT_FLAGTEXT_SPACER*1.5),(px_int)objy+Y,PX_ALIGN_LEFTTOP,text,pcd->FontColor);
			}
		}


		//Draw YL line

		if(pflgl->XYshow&PX_OBJECT_COORDINATEFLAGLINE_XSHOW)
		{
			if (pflgl->Y>=pcd->LeftVerticalRangeMin&&pflgl->Y<=pcd->LeftVerticalRangeMax)
			{
				px_int IsFloat;
				px_int X,Y;
				px_double ValInc=pflgl->Y;
				px_char text[16];
				y=PX_Object_CoordinatesMapVerticalValueToPixel(pObject,pflgl->Y,PX_OBJECT_COORDINATEDATA_MAP_LEFT);

				PX_GeoDrawLine(psurface,(px_int)objx+pcd->LeftSpacer,(px_int)objy+y,(px_int)((px_int)objx+objWidth-pcd->RightSpacer),(px_int)objy+y,(px_int)(pflgl->LineWidth),pflgl->color);
				//Draw text

				if (PX_ABS(ValInc-(px_int)ValInc)<0.000000001)
				{
					IsFloat=PX_FALSE;
				}
				else
				{
					IsFloat=PX_TRUE;
				}


				X=pcd->LeftSpacer;
				Y=y;//PX_Object_CoordinatesMapVerticalValueToPixel(pObject,ValInc,PX_OBJECT_COORDINATEDATA_MAP_LEFT);

				if(IsFloat)
					PX_sprintf1(text,sizeof(text),"%1",PX_STRINGFORMAT_FLOAT((px_float)ValInc));
				else
					PX_sprintf1(text,sizeof(text),"%1",PX_STRINGFORMAT_INT((px_int)ValInc));

				PX_FontModuleDrawText(psurface,PX_NULL,(px_int)objx+X-4*PX_OBJECT_COORDINATES_DEFAULT_FLAGTEXT_SPACER,(px_int)objy+Y-PX_OBJECT_COORDINATES_DEFAULT_FLAGTEXT_SPACER,PX_ALIGN_LEFTTOP,text,pcd->FontColor);
			}
		}


		//Draw YR line
		if(pflgl->XYshow&PX_OBJECT_COORDINATEFLAGLINE_YRSHOW)
		{
			if (pflgl->Y>=pcd->RightVerticalRangeMin&&pflgl->Y<=pcd->RightVerticalRangeMax)
			{
				px_int IsFloat;
				px_int X,Y;
				px_double ValInc=pflgl->Y;
				px_char text[16];

				y=PX_Object_CoordinatesMapVerticalValueToPixel(pObject,pflgl->Y,PX_OBJECT_COORDINATEDATA_MAP_RIGHT);
				PX_GeoDrawLine(psurface,(px_int)objx+pcd->LeftSpacer,(px_int)objy+y,(px_int)((px_int)objx+objWidth-pcd->RightSpacer),(px_int)objy+y,(px_int)(pflgl->LineWidth),pflgl->color);

				if (PX_ABS(ValInc-(px_int)ValInc)<0.000001f)
				{
					IsFloat=PX_FALSE;
				}
				else
				{
					IsFloat=PX_TRUE;
				}

				X=(px_int)(objWidth-pcd->RightSpacer);
				Y=y;//PX_Object_CoordinatesMapVerticalValueToPixel(pObject,ValInc,PX_OBJECT_COORDINATEDATA_MAP_RIGHT);

				PX_FontModuleDrawText(psurface,PX_NULL,(px_int)objx+X,(px_int)objy+Y-PX_OBJECT_COORDINATES_DEFAULT_FLAGTEXT_SPACER,PX_ALIGN_LEFTTOP,text,pcd->FontColor);
			}
		}
	}
}


px_void PX_Object_CoordinatesDrawMarkLine(px_surface *psurface,PX_Object *pObject)
{
	PX_Object_Coordinates *pcd=PX_Object_GetCoordinates(pObject);

	if (pcd->MarkValueEnabled&&pcd->OnMarkStatus)
	{
		if(pcd->MarkLineX!=-1)
		{
			PX_GeoDrawLine(psurface,pcd->MarkLineX,pcd->TopSpacer,pcd->MarkLineX,(px_int)(pObject->Height-pcd->BottomSpacer),1,PX_COLOR(255,255,0,0));
		}
	}
}



px_void PX_Object_CoordinatesRestoreCoordinates(PX_Object *pObject)
{
	PX_Object_Coordinates *pcd=PX_Object_GetCoordinates(pObject);
	if (!pcd->bScaling)
	{
		return;
	}
	PX_Object_CoordinatesSetHorizontalMin(pObject,pcd->ResHorizontalRangeMin);
	PX_Object_CoordinatesSetHorizontalMax(pObject,pcd->ResHorizontalRangeMax);
	PX_Object_CoordinatesSetLeftVerticalMin(pObject,pcd->ResLeftVerticalRangeMin);
	PX_Object_CoordinatesSetLeftVerticalMax(pObject,pcd->ResLeftVerticalRangeMax);
	PX_Object_CoordinatesSetRightVerticalMin(pObject,pcd->ResRightVerticalRangeMin);
	PX_Object_CoordinatesSetRightVerticalMax(pObject,pcd->ResRightVerticalRangeMax);
	pcd->bScaling=PX_FALSE;
}
px_void PX_Object_CoordinatesScaleCoordinates(PX_Object *pObject)
{
	PX_Object_Coordinates *pcd=PX_Object_GetCoordinates(pObject);
	px_double Min,Max;
	px_double dMin;
	px_double dMax;
	if(!pcd->ScaleEnabled)
	{
		return;
	}

	if (pcd->DragingPoint.x==pcd->DragStartPoint.x)
	{
		return;
	}
	if (pcd->DragingPoint.y==pcd->DragStartPoint.y)
	{
		return;
	}
	if (!pcd->bScaling)
	{
		pcd->ResHorizontalRangeMax=pcd->HorizontalRangeMax;
		pcd->ResHorizontalRangeMin=pcd->HorizontalRangeMin;

		pcd->ResLeftVerticalRangeMax=pcd->LeftVerticalRangeMax;
		pcd->ResLeftVerticalRangeMin=pcd->LeftVerticalRangeMin;
		pcd->ResRightVerticalRangeMin=pcd->RightVerticalRangeMin;
		pcd->ResRightVerticalRangeMax=pcd->RightVerticalRangeMax;

		pcd->bScaling=PX_TRUE;
	}

	
	if (pcd->DragStartPoint.x<pcd->DragingPoint.x)
	{
		Min=pcd->DragStartPoint.x;
		Max=pcd->DragingPoint.x;
	}
	else
	{
		Max=pcd->DragStartPoint.x;
		Min=pcd->DragingPoint.x;
	}
	dMin=PX_Object_CoordinatesMapPixelValueToHorizontal(pObject,(px_int)Min);
	dMax=PX_Object_CoordinatesMapPixelValueToHorizontal(pObject,(px_int)Max);

	PX_Object_CoordinatesSetHorizontalMin(pObject,dMin);
	PX_Object_CoordinatesSetHorizontalMax(pObject,dMax);

	if (pcd->DragStartPoint.y<pcd->DragingPoint.y)
	{
		Max=pcd->DragStartPoint.y;
		Min=pcd->DragingPoint.y;
	}
	else
	{
		Min=pcd->DragStartPoint.y;
		Max=pcd->DragingPoint.y;
	}

	dMin=PX_Object_CoordinatesMapPixelValueToVertical(pObject,(px_int)Min,PX_OBJECT_COORDINATEDATA_MAP_LEFT);
	dMax=PX_Object_CoordinatesMapPixelValueToVertical(pObject,(px_int)Max,PX_OBJECT_COORDINATEDATA_MAP_LEFT);
	PX_Object_CoordinatesSetLeftVerticalMin(pObject,dMin);
	PX_Object_CoordinatesSetLeftVerticalMax(pObject,dMax);

	dMin=PX_Object_CoordinatesMapPixelValueToVertical(pObject,(px_int)Min,PX_OBJECT_COORDINATEDATA_MAP_RIGHT);
	dMax=PX_Object_CoordinatesMapPixelValueToVertical(pObject,(px_int)Max,PX_OBJECT_COORDINATEDATA_MAP_RIGHT);

	PX_Object_CoordinatesSetRightVerticalMin(pObject,dMin);
	PX_Object_CoordinatesSetRightVerticalMax(pObject,dMax);
}


px_void PX_Object_CoordinatesRender(px_surface *psurface, PX_Object *pObject,px_uint elpased)
{
	PX_Object_Coordinates *pcd=PX_Object_GetCoordinates(pObject);
	if (!pcd)
	{
		PX_ASSERT();
		return;
	}

// 
// 	PX_Object_CoordinatesDrawMarkLine(psurface,pObject);

	if(pcd->ShowGuides)
		PX_Object_CoordinatesDrawDashed(psurface,pObject);

	PX_Object_CoordinatesDrawDashText(psurface,pObject);

	PX_Object_CoordinatesDrawTitle(psurface,pObject);

	PX_Object_CoordinatesDrawData(psurface,pObject);

	PX_Object_CoordinatesDrawFrameLine(psurface,pObject);

	if(pcd->ScaleEnabled)
		PX_Object_CoordinatesDrawScaleDraging(psurface,pObject);

	PX_Object_CoordinatesDrawHelpLine(psurface,pObject);

	PX_Object_CoordinatesDrawFlagLine(psurface,pObject);
}

px_void PX_Object_CoordinatesFree(PX_Object *pObject)
{
	PX_Object_Coordinates *pcd=PX_Object_GetCoordinates(pObject);
	PX_VectorFree(&pcd->vData);
	PX_VectorFree(&pcd->vFlagLine);
}

px_void PX_Object_CoordinatesCursorPressEvent(PX_Object *pObject, PX_Object_Event e,px_void *ptr)
{
	px_float x;
	px_float y;
	PX_Object_Coordinates *pcd=PX_Object_GetCoordinates(pObject);
	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;

	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;

	x=(PX_Object_Event_GetCursorX(e)-objx);
	y=(PX_Object_Event_GetCursorY(e)-objy);
    if(!PX_ObjectIsPointInRegion(pObject,PX_Object_Event_GetCursorX(e),PX_Object_Event_GetCursorY(e)))
	{
		return;
	}

	if (x<pcd->LeftSpacer)
	{
		x=(px_float)pcd->LeftSpacer;
	}
	if (x>objWidth-pcd->RightSpacer)
	{
		x=(objWidth-pcd->RightSpacer);
	}
	if (y<pcd->TopSpacer)
	{
		y=(px_float)pcd->TopSpacer;
	}
	if (y>objHeight-pcd->BottomSpacer)
	{
		y=(objHeight-pcd->BottomSpacer);
	}

	pcd->DragStartPoint.x=(px_float)(x);
	pcd->DragStartPoint.y=(px_float)(y);

	pcd->DragingPoint=pcd->DragStartPoint;

	pcd->bScaleDrag=PX_TRUE;
}

px_void PX_Object_CoordinatesCursorReleaseEvent( PX_Object *pObject, PX_Object_Event e,px_void *ptr )
{
	
	px_float x=PX_Object_Event_GetCursorX(e);
	px_float y=PX_Object_Event_GetCursorY(e);

	PX_Object_Coordinates *pcd=PX_Object_GetCoordinates(pObject);
	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;

	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;

	pcd->OnMarkStatus=PX_FALSE;
	pcd->MarkLineX=-1;

    if (pcd->bScaleDrag)
	{
		pcd->bScaleDrag=PX_FALSE;
		pcd->DragingPoint.x=(px_float)(PX_Object_Event_GetCursorX(e)-objx);
		pcd->DragingPoint.y=(px_float)(PX_Object_Event_GetCursorY(e)-objy);

		if(pcd->DragingPoint.y<pcd->DragStartPoint.y&&pcd->DragingPoint.x<pcd->DragStartPoint.x)
		{
			PX_Object_CoordinatesRestoreCoordinates(pObject);
		}
		else
		{
			PX_Object_CoordinatesScaleCoordinates(pObject);
		}
	}
	
}

px_void PX_Object_CoordinatesCursorDragEvent(PX_Object *pObject, PX_Object_Event e,px_void *ptr )
{
	PX_Object_Coordinates *pcd=PX_Object_GetCoordinates(pObject);
	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;

	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;

	if (pcd->bScaleDrag)
	{
		px_float x=(PX_Object_Event_GetCursorX(e)-objx);
		px_float y=(PX_Object_Event_GetCursorY(e)-objy);

		pcd->OnMarkStatus=PX_FALSE;
		pcd->MarkLineX=-1;

		// 	emit SignalOnMousePosition(this->MapPixelValueToHorizontal(e->x()),
		// 		e->x(),
		// 		this->MapPixelValueToVertical(e->y(),COORDINATEDATA_MAP_RIGHT));
		if (x<pcd->LeftSpacer)
		{
			x=(px_float)pcd->LeftSpacer;
		}
		if (x>objWidth-pcd->RightSpacer)
		{
			x=(objWidth-pcd->RightSpacer);
		}
		if (y<pcd->TopSpacer)
		{ 
			y=(px_float)pcd->TopSpacer;
		}
		if (y>objHeight-pcd->BottomSpacer)
		{
			y=(objHeight-pcd->BottomSpacer);
		}

		pcd->DragingPoint.x=(px_float)(x);
		pcd->DragingPoint.y=(px_float)(y);
	}

}

px_void PX_Object_CoordinatesCursorMoveEvent(PX_Object *pObject, PX_Object_Event e,px_void *ptr )
{
	PX_Object_Coordinates *pcd=PX_Object_GetCoordinates(pObject);
	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;

	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;

	if (pcd->ShowHelpLine)
	{
		px_float x=(PX_Object_Event_GetCursorX(e)-objx);
		px_float y=(PX_Object_Event_GetCursorY(e)-objy);

		pcd->OnMarkStatus=PX_FALSE;
		pcd->MarkLineX=-1;

		// 	emit SignalOnMousePosition(this->MapPixelValueToHorizontal(e->x()),
		// 		e->x(),
		// 		this->MapPixelValueToVertical(e->y(),COORDINATEDATA_MAP_RIGHT));
		if (x<pcd->LeftSpacer)
		{
			x=(px_float)pcd->LeftSpacer;
		}
		if (x>objWidth-pcd->RightSpacer)
		{
			x=(objWidth-pcd->RightSpacer);
		}
		if (y<pcd->TopSpacer)
		{ 
			y=(px_float)pcd->TopSpacer;
		}
		if (y>objHeight-pcd->BottomSpacer)
		{
			y=(objHeight-pcd->BottomSpacer);
		}

		if (pcd->bScaleDrag)
		{
			pcd->DragingPoint.x=(px_float)(x);
			pcd->DragingPoint.y=(px_float)(y);
		}

		pcd->helpLineX=(px_int)(x);
		pcd->helpLineY=(px_int)(y);
	}
}




PX_Object *PX_Object_CoordinatesCreate(px_memorypool *mp, PX_Object *Parent,px_int x,px_int y,px_int Width,px_int Height,PX_FontModule *fontmodule)
{
	PX_Object *pObject;
	PX_Object_Coordinates Coordinates;
	PX_memset(&Coordinates,0,sizeof(Coordinates));
	Coordinates.mp=mp;
	Coordinates.MinHorizontalPixelDividing=PX_OBJECT_COORDINATES_DEFAULE_MINHORIZONTALPIXELDIVIDING;
	Coordinates.MinVerticalPixelDividing=PX_OBJECT_COORDINATES_DEFAULE_MINVERTICALPIXELDIVIDING;
	Coordinates.HorizontalDividing=PX_OBJECT_COORDINATES_DEFAULE_DIVIDING;
	Coordinates.LeftVerticalDividing=PX_OBJECT_COORDINATES_DEFAULE_DIVIDING;
	Coordinates.RightVerticalDividing=PX_OBJECT_COORDINATES_DEFAULE_DIVIDING;
	Coordinates.bScaleDrag=PX_FALSE;
	Coordinates.guidesShowMode=PX_OBJECT_COORDINATES_GUIDESSHOWMODE_ALL;
	Coordinates.helpLineColor=PX_COLOR(255,255,0,0);
	Coordinates.ShowHelpLine=PX_TRUE;

	Coordinates.LeftSpacer=(px_int)(1.5f*PX_OBJECT_COORDINATES_DEFAULT_SPACER);
	Coordinates.RightSpacer=(px_int)(1.5*PX_OBJECT_COORDINATES_DEFAULT_SPACER);
	Coordinates.TopSpacer=PX_OBJECT_COORDINATES_DEFAULT_SPACER;
	Coordinates.BottomSpacer=PX_OBJECT_COORDINATES_DEFAULT_SPACER;

	Coordinates.HorizontalRangeMin=-150;
	Coordinates.HorizontalRangeMax=150;

	Coordinates.LeftVerticalRangeMin=0;
	Coordinates.LeftVerticalRangeMax=100;

	Coordinates.RightVerticalRangeMax=1.0;
	Coordinates.RightVerticalRangeMin=0;

	Coordinates.FloatFlagFormat_H=PX_OBJECT_COORDINATES_DEFAULT_FLOAT_FLAGFORMAT_H;
	Coordinates.IntFlagFormat_H=PX_OBJECT_COORDINATES_DEFAULT_INT_FLAGFORMAT_H;

	Coordinates.FloatFlagFormat_L=PX_OBJECT_COORDINATES_DEFAULT_FLOAT_FLAGFORMAT_L;
	Coordinates.IntFlagFormat_L=PX_OBJECT_COORDINATES_DEFAULT_INT_FLAGFORMAT_L;

	Coordinates.FloatFlagFormat_R=PX_OBJECT_COORDINATES_DEFAULT_FLOAT_FLAGFORMAT_R;
	Coordinates.IntFlagFormat_R=PX_OBJECT_COORDINATES_DEFAULT_INT_FLAGFORMAT_R;

	Coordinates.Exponential_Format=PX_OBJECT_COORDINATES_DEFAULT_EXPONENTIAL_FORMAT;

	Coordinates.bScaling=PX_FALSE;

	Coordinates.FontSize=PX_OBJECT_COORDINATES_DEFAULT_FONT_SIZE;
	Coordinates.DataLineWidth=PX_OBJECT_COORDINATES_DEFAULT_LINE_WIDTH;
	Coordinates.FontColor=PX_COLOR(255,0,0,0);
	Coordinates.borderColor=PX_COLOR(255,0,0,0);
	Coordinates.DashColor=PX_COLOR(PX_OBJECT_COORDINATES_DEFAULT_DASH_RGB);
	Coordinates.OnMarkStatus=PX_FALSE;
	Coordinates.MarkValueEnabled=PX_TRUE;
	Coordinates.MarkLineX=-1;

	Coordinates.bDataUpdatePainter=PX_TRUE;

	Coordinates.LineMode=PX_OBJECT_COORDINATES_LINEMODE_LINES;

	Coordinates.LeftTextShow=PX_TRUE;
	Coordinates.RightTextShow=PX_TRUE;
	Coordinates.HorizontalTextShow=PX_TRUE;
	Coordinates.ScaleEnabled=PX_TRUE;
	Coordinates.ShowGuides=PX_TRUE;

	Coordinates.LeftTitle="";
	Coordinates.RightTitle="";
	Coordinates.TopTitle="";
	Coordinates.BottomTitle="";
	Coordinates.fontmodule=fontmodule;
	Coordinates.leftTextDisplayMode=PX_OBJECT_COORDINATES_TEXT_DISPLAYMODE_NORMAL;
	Coordinates.RightTextDisplayMode=PX_OBJECT_COORDINATES_TEXT_DISPLAYMODE_NORMAL;

	PX_VectorInitialize(mp,&Coordinates.vData,sizeof(PX_Object_CoordinateData),16);
	PX_VectorInitialize(mp,&Coordinates.vFlagLine,sizeof(PX_Object_CoordinateFlagLine),16);

	pObject=PX_ObjectCreateEx(mp,Parent,(px_float)x,(px_float)y,0,(px_float)Width,(px_float)Height,0,PX_OBJECT_TYPE_COORDINATE,PX_Object_CoordinatesUpdate,PX_Object_CoordinatesRender,PX_Object_CoordinatesFree,&Coordinates,sizeof(PX_Object_Coordinates));

	//PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_CURSORMOVE,PX_Object_CoordinatesCursorMoveEvent,PX_NULL);
	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_CURSORMOVE,PX_Object_CoordinatesCursorMoveEvent,PX_NULL);
	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_CURSORDRAG,PX_Object_CoordinatesCursorDragEvent,PX_NULL);
	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_CURSORDOWN,PX_Object_CoordinatesCursorPressEvent,PX_NULL);
	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_CURSORUP,PX_Object_CoordinatesCursorReleaseEvent,PX_NULL);

	return pObject;
}

PX_Object_FilterEditor * PX_Object_GetFilterEditor(PX_Object *Object)
{
	if (Object->Type==PX_OBJECT_TYPE_FILTEREDITOR)
	{
		return (PX_Object_FilterEditor *)Object->pObject;
	}
	return PX_NULL;
}

static px_void PX_Object_FilterEditorDrawFrameLine(px_surface *psurface,PX_Object *pObject)
{
	PX_Object_FilterEditor *pfe=PX_Object_GetFilterEditor(pObject);
	px_int offsetx;
	px_int offsety;
	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;

	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;

	offsetx=(px_int)objx;
	offsety=(px_int)objy;

	PX_GeoDrawLine(psurface,offsetx,offsety,offsetx+(px_int)(objWidth-1),offsety,PX_OBJECT_FILTEREDITOR_DEFAULT_FRAMELINE_WIDTH,pfe->borderColor);
	PX_GeoDrawLine(psurface,offsetx,offsety,offsetx,offsety+(px_int)(objHeight-1),PX_OBJECT_FILTEREDITOR_DEFAULT_FRAMELINE_WIDTH,pfe->borderColor);
	PX_GeoDrawLine(psurface,(px_int)(offsetx+objWidth-1),offsety+(px_int)(objHeight-1),offsetx,offsety+(px_int)objHeight-1,PX_OBJECT_COORDINATES_DEFAULT_FRAMELINE_WIDTH,pfe->borderColor);
	PX_GeoDrawLine(psurface,(px_int)(offsetx+objWidth-1),offsety+(px_int)(objHeight-1),offsetx+(px_int)(objWidth-1),offsety,PX_OBJECT_COORDINATES_DEFAULT_FRAMELINE_WIDTH,pfe->borderColor);
}

static px_void PX_Object_FilterEditorDrawSelectDraging(px_surface *psurface,PX_Object *pObject)
{
	PX_Object_FilterEditor *pfe=PX_Object_GetFilterEditor(pObject);
	px_int offsetx;
	px_int offsety;
	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;

	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;

	offsetx=(px_int)objx;
	offsety=(px_int)objy;

	if (!pfe->bSelectDrag)
	{
		return;
	}

	PX_GeoDrawRect(psurface,(px_int)(offsetx+pfe->DragStartPoint.x),(px_int)(offsety+(px_int)pfe->DragStartPoint.y),(px_int)(offsetx+(px_int)pfe->DragingPoint.x),(px_int)(offsety+(px_int)pfe->DragingPoint.y),PX_COLOR(96,171,81,128));	
}

static px_void PX_Object_FilterEditorDrawPt(px_surface *psurface,PX_Object *pObject)
{
	PX_Object_FilterEditor *pfe=PX_Object_GetFilterEditor(pObject);
	px_int i;
	px_float oftx;
	px_float ofty;
	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;

	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;

	oftx=objx;
	ofty=objy;

	for (i=0;i<pfe->opCount;i++)
	{
		PX_GeoDrawSolidCircle(psurface,(px_int)(oftx+pfe->pt[i].x),(px_int)(ofty+pfe->pt[i].y),pfe->radius,pfe->ptColor);
		if (pfe->pt[i].bselect||pfe->pt[i].bCursor)
		{
			PX_GeoDrawCircle(psurface,(px_int)(oftx+pfe->pt[i].x),(px_int)(ofty+pfe->pt[i].y),pfe->radius+2,1,pfe->ptColor);
		}
	}

	for (i=0;i<pfe->opCount-1;i++)
	{
		PX_GeoDrawLine(psurface,(px_int)(oftx+pfe->pt[i].x),(px_int)(ofty+pfe->pt[i].y),(px_int)(oftx+pfe->pt[i+1].x),(px_int)(ofty+pfe->pt[i+1].y),2,pfe->ptColor);
	}

}

static px_void PX_Object_FilterEditorDrawHelpLine(px_surface *psurface,PX_Object *pObject)
{
	PX_Object_FilterEditor *pfe=PX_Object_GetFilterEditor(pObject);
	px_char text[16];
	px_double midy,incx,incy,x,y,val;
	px_double oftx,ofty;
	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;

	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;


	if (!pfe->ShowHelpLine)
	{
		return;
	}
	oftx=objx;
	ofty=objy;
	midy=objHeight/2;
	incy=objHeight/2/(pfe->VerticalDividing/2);

	//up
	for (y=midy;y>=0;y-=incy)
	{
		//line
		PX_GeoDrawLine(psurface,(px_int)(oftx),(px_int)(ofty+y),(px_int)(oftx+objWidth),(px_int)(ofty+y),1,pfe->helpLineColor);
		val=(midy-y)/(objHeight/2)*pfe->rangedb;
		switch (pfe->FilterType)
		{
		case PX_OBJECT_FILTER_TYPE_dB:
			PX_sprintf1(text,sizeof(text),"+%1.2db",PX_STRINGFORMAT_FLOAT((px_float)val));
			break;
		case PX_OBJECT_FILTER_TYPE_Liner:
			PX_sprintf1(text,sizeof(text),"+%1.2",PX_STRINGFORMAT_FLOAT((px_float)val));
			break;
		}
		
		//text
		PX_FontModuleDrawText(psurface,PX_NULL,(px_int)(oftx-1),(px_int)(ofty+y-5),PX_ALIGN_RIGHTTOP,text,pfe->FontColor);
	}
	//down
	for (y=midy;y<=objHeight;y+=incy)
	{
		//line
		PX_GeoDrawLine(psurface,(px_int)(oftx),(px_int)(ofty+y),(px_int)(oftx+objWidth),(px_int)(ofty+y),1,pfe->helpLineColor);
		val=(midy-y)/(objHeight/2)*pfe->rangedb;
		switch (pfe->FilterType)
		{
		case PX_OBJECT_FILTER_TYPE_dB:
			PX_sprintf1(text,sizeof(text),"%1.2db",PX_STRINGFORMAT_FLOAT((px_float)val));
			break;
		case PX_OBJECT_FILTER_TYPE_Liner:
			PX_sprintf1(text,sizeof(text),"%1.2",PX_STRINGFORMAT_FLOAT((px_float)val));
			break;
		}
		//text
		PX_FontModuleDrawText(psurface,PX_NULL,(px_int)(oftx-1),(px_int)(ofty+y-5),PX_ALIGN_RIGHTTOP,text,pfe->FontColor);
	}

	//horizontal
	incx=objWidth/pfe->HorizontalDividing;
	for (x=0;x<objWidth;x+=incx)
	{
		//line
		PX_GeoDrawLine(psurface,(px_int)(oftx+x),(px_int)(ofty+0),(px_int)(oftx+x),(px_int)(ofty+objHeight-1),1,pfe->helpLineColor);
		val=x/objWidth;
		//text
		if (pfe->showHorizontal)
		{
			PX_sprintf1(text,sizeof(text),"%1.2",PX_STRINGFORMAT_FLOAT((px_float)val));
			PX_FontModuleDrawText(psurface,PX_NULL,(px_int)(oftx+x-1),(px_int)(ofty+objHeight+3),PX_ALIGN_CENTER,text,pfe->FontColor);
		}
	}

}

px_void PX_Object_FilterEditorCursorReleaseEvent( PX_Object *pObject, PX_Object_Event e,px_void *ptr )
{
	PX_Object_FilterEditor *pfe=PX_Object_GetFilterEditor(pObject);

	pfe->bSelectDrag=PX_FALSE;
	pfe->bAdjust=PX_FALSE;
}


px_void PX_Object_FilterEditorRender(px_surface *psurface, PX_Object *pObject,px_uint elpased)
{
	PX_Object_FilterEditorDrawFrameLine(psurface,pObject);
	PX_Object_FilterEditorDrawHelpLine(psurface,pObject);
	PX_Object_FilterEditorDrawPt(psurface,pObject);
	PX_Object_FilterEditorDrawSelectDraging(psurface,pObject);
}

px_void PX_Object_FilterEditorCursorPressEvent(PX_Object *pObject, PX_Object_Event e,px_void *ptr)
{
	px_int i,j;
	px_bool bSelectlge1=PX_FALSE;
	px_float x;
	px_float y;
	PX_Object_FilterEditor *pfe=PX_Object_GetFilterEditor(pObject);
	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;

	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;

	x=(PX_Object_Event_GetCursorX(e)-objx);
	y=(PX_Object_Event_GetCursorY(e)-objy);


	if(!PX_ObjectIsPointInRegion(pObject,PX_Object_Event_GetCursorX(e),PX_Object_Event_GetCursorY(e)))
	{
		return;
	}

	for (i=0;i<pfe->opCount;i++)
	{
		if(PX_isPointInCircle(PX_POINT(x,y,0),PX_POINT((px_float)pfe->pt[i].x,(px_float)pfe->pt[i].y,0),(px_float)pfe->radius))
		{
			if (!pfe->pt[i].bselect)
			{
				for (j=0;j<pfe->opCount;j++)
				{
					pfe->pt[j].bselect=PX_FALSE;
				}
				pfe->pt[i].bselect=PX_TRUE;
			}
			bSelectlge1=PX_TRUE;
		}

	}

	if (bSelectlge1)
	{
		pfe->bSelectDrag=PX_FALSE;
		pfe->bAdjust=PX_TRUE;
		pfe->lastAdjustPoint.x=(x);
		pfe->lastAdjustPoint.y=(y);
	}
	else
	{
		for (i=0;i<pfe->opCount;i++)
		{
			pfe->pt[i].bselect=PX_FALSE;
		}
		pfe->DragStartPoint.x=(x);
		pfe->DragStartPoint.y=(y);
		pfe->DragingPoint=pfe->DragStartPoint;
		pfe->bSelectDrag=PX_TRUE;
		pfe->bAdjust=PX_FALSE;
	}
	
}

px_void PX_Object_FilterEditorCursorMoveEvent(PX_Object *pObject, PX_Object_Event e,px_void *ptr )
{
	PX_Object_FilterEditor *pfe=PX_Object_GetFilterEditor(pObject);
	px_int i;
	px_float x,y;
	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;

	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;

	x=(PX_Object_Event_GetCursorX(e)-objx);
	y=(PX_Object_Event_GetCursorY(e)-objy);


	if(!PX_ObjectIsPointInRegion(pObject,PX_Object_Event_GetCursorX(e),PX_Object_Event_GetCursorY(e)))
	{
		return;
	}

	for (i=0;i<pfe->opCount;i++)
	{
		if(PX_isPointInCircle(PX_POINT(x,y,0),PX_POINT((px_float)pfe->pt[i].x,(px_float)pfe->pt[i].y,0),(px_float)pfe->radius))
		{
			pfe->pt[i].bCursor=PX_TRUE;
		}
		else
		{
			pfe->pt[i].bCursor=PX_FALSE;
		}
	}

}


px_void PX_Object_FilterEditorCursorDragEvent(PX_Object *pObject, PX_Object_Event e,px_void *ptr )
{
	px_int i;
	PX_Object_FilterEditor *pfe=PX_Object_GetFilterEditor(pObject);
	px_float x,y;
	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;

	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;

	x=(PX_Object_Event_GetCursorX(e)-objx);
	y=(PX_Object_Event_GetCursorY(e)-objy);

	if (pfe->bSelectDrag)
	{
		px_rect rect;

		if (x<0)
		{
			x=0;
		}
		if (x>objWidth)
		{
			x=(objWidth);
		}
		if (y<0)
		{ 
			y=0;
		}
		if (y>objHeight)
		{
			y=(objHeight);
		}

		pfe->DragingPoint.x=(px_float)(x);
		pfe->DragingPoint.y=(px_float)(y);
		rect=PX_RECT(pfe->DragStartPoint.x,pfe->DragStartPoint.y,x-pfe->DragStartPoint.x,y-pfe->DragStartPoint.y);
		for (i=0;i<pfe->opCount;i++)
		{
			if(PX_isPointInRect(PX_POINT((px_float)pfe->pt[i].x,(px_float)pfe->pt[i].y,0),rect))
			{
				pfe->pt[i].bselect=PX_TRUE;
			}
			else
			{
				pfe->pt[i].bselect=PX_FALSE;
			}
		}
	}

	else if (pfe->bAdjust)
	{
		for (i=0;i<pfe->opCount;i++)
		{
			if(pfe->pt[i].bselect==PX_TRUE)
			{
				pfe->pt[i].y+=(px_int)(y-pfe->lastAdjustPoint.y);
				if (pfe->pt[i].y<0)
				{
					pfe->pt[i].y=0;
				}
				if (pfe->pt[i].y>objHeight-1)
				{
					pfe->pt[i].y=(px_int)objHeight-1;
				}
			}
		}
		pfe->lastAdjustPoint=PX_POINT(x,y,0);
	}
}


px_void PX_Object_FilterEditorSetOperateCount(PX_Object *pObject,px_int count)
{
	px_int i;
	PX_Object_FilterEditor *pfe=PX_Object_GetFilterEditor(pObject);
	px_float Inc;
	px_int y;
	if (count<2)
	{
		count=2;
	}
	if (count>PX_COUNTOF(pfe->pt))
	{
		count=PX_COUNTOF(pfe->pt);
	}
	Inc=pObject->Width/(count-1);
	y=(px_int)(pObject->Height/2);
	for (i=0;i<count;i++)
	{
		pfe->pt[i].x=(px_int)(Inc*i);
		pfe->pt[i].y=y;
	}
	pfe->opCount=count;
}

px_void PX_Object_FilterEditorSetType(PX_Object *Object,PX_OBJECT_FILTER_TYPE type)
{
	PX_Object_FilterEditor *pfe=PX_Object_GetFilterEditor(Object);
	if (pfe)
	{
		pfe->FilterType=type;
	}
}

px_void PX_Object_FilterEditorSetHorizontalShow(PX_Object *Object,px_bool HorizontalShow)
{
	PX_Object_FilterEditor *pfe=PX_Object_GetFilterEditor(Object);
	if (pfe)
	{
		pfe->showHorizontal=HorizontalShow;
	}
}

px_void PX_Object_FilterEditorReset(PX_Object *Object)
{
	px_int i,y;
	PX_Object_FilterEditor *pfe=PX_Object_GetFilterEditor(Object);
	y=(px_int)(Object->Height/2);
	for (i=0;i<pfe->opCount;i++)
	{
		pfe->pt[i].y=y;
	}
}

px_void PX_Object_FilterEditorSetRange(PX_Object *Object,px_double range)
{
	
	PX_Object_FilterEditor *pfe=PX_Object_GetFilterEditor(Object);
	if (pfe)
	{
		pfe->rangedb=range;
	}
}

px_void PX_Object_FilterEditorSetFontColor(PX_Object *Object,px_color clr)
{
	PX_Object_FilterEditor *pfe=PX_Object_GetFilterEditor(Object);
	if (pfe)
	{
		pfe->FontColor=clr;
	}
}

px_void PX_Object_FilterEditorSetBorderColor(PX_Object *Object,px_color clr)
{
	PX_Object_FilterEditor *pfe=PX_Object_GetFilterEditor(Object);
	if (pfe)
	{
		pfe->borderColor=clr;
	}
}

px_void PX_Object_FilterEditorSethelpLineColor(PX_Object *Object,px_color clr)
{
	PX_Object_FilterEditor *pfe=PX_Object_GetFilterEditor(Object);
	if (pfe)
	{
		pfe->helpLineColor=clr;
	}
}

px_void PX_Object_FilterEditorSetFontSize(PX_Object *Object,px_int size)
{
	PX_Object_FilterEditor *pfe=PX_Object_GetFilterEditor(Object);
	if (pfe)
	{
		pfe->FontSize=size;
	}
}

px_void PX_Object_FilterEditorSetHorizontalDividing(PX_Object *Object,px_int div)
{
	PX_Object_FilterEditor *pfe=PX_Object_GetFilterEditor(Object);
	if (pfe)
	{
		pfe->HorizontalDividing=div;
	}
}

px_void PX_Object_FilterEditorSetVerticalDividing(PX_Object *Object,px_int div)
{
	PX_Object_FilterEditor *pfe=PX_Object_GetFilterEditor(Object);
	if (pfe)
	{
		pfe->VerticalDividing=div;
	}
}

px_void PX_Object_FilterEditorMapData(PX_Object *Object,px_double data[],px_int size)
{
	px_int i,mapIndex;
	px_double ptValue[PX_OBJECT_FILTER_EDITOR_MAX_PT];
	px_double step,frac,d2,d1,dm;
	PX_Object_FilterEditor *pfe=PX_Object_GetFilterEditor(Object);
	if (!pfe)
	{
		return;
	}
	//pt to value
	for (i=0;i<pfe->opCount;i++)
	{
		if (pfe->pt[i].y>Object->Height-10)
		{
			ptValue[i]=-1000;
		}
		else
		{
			ptValue[i]=((Object->Height/2)-pfe->pt[i].y)/(Object->Height/2)*pfe->rangedb;
		}
	}

	step=1.0/(pfe->opCount-1);

	for (i=0;i<size;i++)
	{
		mapIndex=(px_int)((1.0*i/size)/step);
		frac=((1.0*i/size)-mapIndex*step)/step;
		d2=ptValue[mapIndex+1];
		d1=ptValue[mapIndex];
		dm=d1+frac*(d2-d1);
		switch(pfe->FilterType)
		{
		case PX_OBJECT_FILTER_TYPE_dB:
			dm=PX_pow_dd(10,dm/20.0);
			break;
		case PX_OBJECT_FILTER_TYPE_Liner:
			break;
		}
		data[i]=dm;
	}
}

px_double PX_Object_FilterEditorMapValue(PX_Object *pObject,px_double precent)
{
	px_int mapIndex;
	px_double step,frac,d2,d1,dm;
	PX_Object_FilterEditor *pfe=PX_Object_GetFilterEditor(pObject);
	if (!pfe)
	{
		return 1;
	}

	step=1.0/(pfe->opCount-1);
	mapIndex=(px_int)(precent/step);
	if (mapIndex>pfe->opCount-1)
	{
		return 1;
	}
	frac=(precent-mapIndex*step)/step;
	d2=((pObject->Height/2)-pfe->pt[mapIndex+1].y)/(pObject->Height/2)*pfe->rangedb;
	d1=((pObject->Height/2)-pfe->pt[mapIndex].y)/(pObject->Height/2)*pfe->rangedb;
	dm=d1+frac*(d2-d1);
	switch(pfe->FilterType)
	{
	case PX_OBJECT_FILTER_TYPE_dB:
		dm=PX_pow_dd(10,dm/20.0);
		break;
	case PX_OBJECT_FILTER_TYPE_Liner:
		break;
	}
	return dm;
}

PX_Object * PX_Object_FilterEditorCreate(px_memorypool *mp, PX_Object *Parent,px_int x,px_int y,px_int Width,px_int Height,PX_OBJECT_FILTER_TYPE type)
{
	PX_Object *pObject;
	PX_Object_FilterEditor FilterEditor;
	PX_memset(&FilterEditor,0,sizeof(FilterEditor));
	FilterEditor.showHorizontal=PX_TRUE;
	FilterEditor.borderColor=PX_COLOR(255,0,0,0);
	FilterEditor.FontColor=PX_COLOR(255,0,0,0);
	FilterEditor.bSelectDrag=PX_FALSE;
	FilterEditor.bAdjust=PX_FALSE;
	FilterEditor.DragingPoint=PX_POINT(0,0,0);
	FilterEditor.DragStartPoint=PX_POINT(0,0,0);
	FilterEditor.FontSize=16;
	FilterEditor.helpLineColor=PX_COLOR(255,0,0,0);
	FilterEditor.ptColor=PX_COLOR(255,0,0,0);
	FilterEditor.HorizontalDividing=16;
	FilterEditor.VerticalDividing=16;
	FilterEditor.ShowHelpLine=PX_TRUE;
	FilterEditor.showHorizontal=PX_FALSE;
	FilterEditor.FilterType=type;
	FilterEditor.rangedb=6;
	FilterEditor.radius=PX_OBJECT_FILTER_EDITOR_DEFAULT_RADIUS;
	pObject=PX_ObjectCreateEx(mp,Parent,(px_float)x,(px_float)y,0,(px_float)Width,(px_float)Height,0,PX_OBJECT_TYPE_FILTEREDITOR,PX_NULL,PX_Object_FilterEditorRender,PX_NULL,&FilterEditor,sizeof(PX_Object_FilterEditor));
	PX_Object_FilterEditorSetOperateCount(pObject,8);

	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_CURSORMOVE,PX_Object_FilterEditorCursorMoveEvent,PX_NULL);
	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_CURSORDRAG,PX_Object_FilterEditorCursorDragEvent,PX_NULL);
	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_CURSORDOWN,PX_Object_FilterEditorCursorPressEvent,PX_NULL);
	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_CURSORUP,PX_Object_FilterEditorCursorReleaseEvent,PX_NULL);
	return pObject;
}

px_void PX_Object_FilterEditorSetborderColor(PX_Object *Object,px_color clr)
{
	PX_Object_FilterEditor *pfe=PX_Object_GetFilterEditor(Object);
	pfe->borderColor=clr;
}



px_void PX_Object_FilterEditorSetptColor(PX_Object *Object,px_color clr)
{
	PX_Object_FilterEditor *pfe=PX_Object_GetFilterEditor(Object);
	pfe->ptColor=clr;
}


PX_Object_CheckBox * PX_Object_GetCheckBox(PX_Object *Object)
{
	PX_Object_CheckBox *pcb=(PX_Object_CheckBox *)Object->pObject;
	if (Object->Type==PX_OBJECT_TYPE_CHECKBOX)
	{
		return pcb;
	}
	return PX_NULL;
}


px_void PX_Object_CheckBoxOnMouseMove(PX_Object *Object,PX_Object_Event e,px_void *user_ptr)
{
	PX_Object_CheckBox *pcb=PX_Object_GetCheckBox(Object);

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
				pcb->state=PX_OBJECT_CHECKBOX_STATE_ONCURSOR;
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
			pcb->state=PX_OBJECT_CHECKBOX_STATE_NORMAL;
		}
	}
}

px_void PX_Object_CheckBoxOnMouseLButtonDown(PX_Object *Object,PX_Object_Event e,px_void *user_ptr)
{
	PX_Object_CheckBox *pcb=PX_Object_GetCheckBox(Object);
	px_float x=(PX_Object_Event_GetCursorX(e));
	px_float y=(PX_Object_Event_GetCursorY(e));

	if (pcb)
	{
		if(PX_ObjectIsPointInRegion(Object,(px_float)x,(px_float)y))
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
				PX_ObjectExecuteEvent(Object,e);
			}
			pcb->state=PX_OBJECT_CHECKBOX_STATE_NORMAL;
		}
	}
}

px_void PX_Object_CheckBoxOnMouseLButtonUp(PX_Object *Object,PX_Object_Event e,px_void *user_ptr)
{
	PX_Object_CheckBox *pcb=PX_Object_GetCheckBox(Object);

	px_float x,y;

	x=PX_Object_Event_GetCursorX(e);
	y=PX_Object_Event_GetCursorY(e);

	if (pcb)
	{
		if(PX_ObjectIsPointInRegion(Object,(px_float)x,(px_float)y))
			if(pcb->state==PX_OBJECT_CHECKBOX_STATE_ONPUSH)
			{
				PX_Object_Event e;
				pcb->state=PX_OBJECT_CHECKBOX_STATE_ONCURSOR;
				pcb->bCheck=!pcb->bCheck;
				
				e.Event=PX_OBJECT_EVENT_VALUECHANGED;
				e.Param_uint[0]=0;
				e.Param_uint[1]=0;
				e.Param_uint[2]=0;
				e.Param_uint[3]=0;
				PX_ObjectExecuteEvent(Object,e);
			}
	}
}

px_void PX_Object_CheckBoxRender(px_surface *psurface, PX_Object *pObject,px_uint elpased)
{
	PX_Object_CheckBox *pcb=PX_Object_GetCheckBox(pObject);
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

	PX_FontModuleDrawText(psurface,pcb->fm,(px_int)objx+18,(px_int)(objy+objHeight/2),PX_ALIGN_LEFTMID,pcb->Text,pcb->TextColor);

	//draw CheckState
	PX_GeoDrawBorder(psurface,(px_int)objx,(px_int)(objy+objHeight/2-7),(px_int)objx+14,(px_int)(objy+objHeight/2+7),1,pcb->BorderColor);
	if (pcb->bCheck)
	{
		PX_GeoDrawRect(psurface,(px_int)objx+2,(px_int)(objy+objHeight/2-5),(px_int)objx+12,(px_int)(objy+objHeight/2+5),pcb->BorderColor);
	}

}

PX_Object * PX_Object_CheckBoxCreate(px_memorypool *mp, PX_Object *Parent,px_int x,px_int y,px_int Width,px_int Height,const char text[],PX_FontModule *fm)
{
	PX_Object *pObject;
	PX_Object_CheckBox cbx;
	cbx.Align=PX_ALIGN_LEFTMID;
	cbx.BackgroundColor=PX_COLOR(0,0,0,0);
	cbx.bCheck=PX_FALSE;
	cbx.Border=PX_FALSE;
	cbx.BorderColor=PX_COLOR(255,0,0,0);
	cbx.CursorColor=PX_COLOR(255,204,204,204);
	cbx.PushColor=PX_COLOR(255,192,192,192);
	cbx.fm=fm;
	cbx.state=PX_OBJECT_CHECKBOX_STATE_NORMAL;
	PX_strset(cbx.Text,text);
	cbx.TextColor=PX_COLOR(255,0,0,0);
	pObject=PX_ObjectCreateEx(mp,Parent,(px_float)x,(px_float)y,0,(px_float)Width,(px_float)Height,0,PX_OBJECT_TYPE_CHECKBOX,PX_NULL,PX_Object_CheckBoxRender,PX_NULL,&cbx,sizeof(cbx));
	if (!pObject)
	{
		return PX_NULL;
	}
	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_CURSORMOVE,PX_Object_CheckBoxOnMouseMove,PX_NULL);
	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_CURSORDOWN,PX_Object_CheckBoxOnMouseLButtonDown,PX_NULL);
	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_CURSORUP,PX_Object_CheckBoxOnMouseLButtonUp,PX_NULL);
	return pObject;
}

px_bool PX_Object_CheckBoxGetCheck(PX_Object *Object)
{
	return PX_Object_GetCheckBox(Object)->bCheck;
}

px_void PX_Object_CheckBoxSetBackgroundColor(PX_Object *Object,px_color clr)
{
	PX_Object_GetCheckBox(Object)->BackgroundColor=clr;
}

px_void PX_Object_CheckBoxSetBorderColor(PX_Object *Object,px_color clr)
{
	PX_Object_GetCheckBox(Object)->BorderColor=clr;
}

px_void PX_Object_CheckBoxSetPushColor(PX_Object *Object,px_color clr)
{
	PX_Object_GetCheckBox(Object)->PushColor=clr;
}

px_void PX_Object_CheckBoxSetCursorColor(PX_Object *Object,px_color clr)
{
	PX_Object_GetCheckBox(Object)->CursorColor=clr;
}

px_void PX_Object_CheckBoxSetText(PX_Object *Object,const px_char text[])
{
	PX_strset(PX_Object_GetCheckBox(Object)->Text,text);
}

px_void PX_Object_CheckBoxSetTextColor(PX_Object *Object,px_color clr)
{
	PX_Object_GetCheckBox(Object)->TextColor=clr;
}

px_void PX_Object_CheckBoxSetCheck(PX_Object *Object,px_bool check)
{
	PX_Object_GetCheckBox(Object)->bCheck=check;
}


static PX_Object_Menu *PX_Object_GetMenu(PX_Object *pMenuObject)
{
	if (pMenuObject->Type==PX_OBJECT_TYPE_MENU)
	{
		return (PX_Object_Menu *)pMenuObject->pObject;
	}
	return PX_NULL;
}
static px_void PX_MenuSubMenuUpdateEx(PX_Object_Menu *pMenu,PX_Object_Menu_Item *pItem,px_int startX,px_int startY)
{
	px_int index=0;
	px_int maxWidth=0,maxHeight=0;
	px_list_node *pNode=PX_NULL;


	for (pNode=PX_ListNodeAt(&pItem->Items,0);pNode;pNode=PX_ListNodeNext(pNode))
	{
		px_int cWidth,cHeight;
		PX_Object_Menu_Item *pSubItem=PX_LIST_NODETDATA(PX_Object_Menu_Item,pNode);
		PX_FontModuleTextGetRenderWidthHeight(pMenu->fontmodule,pSubItem->Text,&cWidth,&cHeight);
		if (cWidth>maxWidth)
		{
			maxWidth=cWidth;
		}
		if (cHeight>maxHeight)
		{
			maxHeight=cHeight;
		}
	}

	maxWidth+=PX_MENU_ITEM_SPACER_SIZE*2+16;
	maxHeight+=PX_MENU_ITEM_SPACER_SIZE*2;


	if (maxWidth<pMenu->minimumSize)
	{
		maxWidth=pMenu->minimumSize;
	}

	for (pNode=PX_ListNodeAt(&pItem->Items,0);pNode;pNode=PX_ListNodeNext(pNode))
	{
		PX_Object_Menu_Item *pSubItem=PX_LIST_NODETDATA(PX_Object_Menu_Item,pNode);
		pSubItem->x=startX;
		pSubItem->y=startY+index*maxHeight;
		pSubItem->width=maxWidth;
		pSubItem->height=maxHeight;
		index++;
	}


	for (pNode=PX_ListNodeAt(&pItem->Items,0);pNode;pNode=PX_ListNodeNext(pNode))
	{
		PX_Object_Menu_Item *pSubItem=PX_LIST_NODETDATA(PX_Object_Menu_Item,pNode);
		PX_MenuSubMenuUpdateEx(pMenu,pSubItem,pSubItem->x+pSubItem->width,pSubItem->y);
	}
}
static px_void PX_MenuSubMenuUpdate(px_int x,px_int y,PX_Object_Menu *pMenu)
{
	px_int index=0;
	px_int maxWidth=0,maxHeight=0;
	px_list_node *pNode=PX_NULL;


	for (pNode=PX_ListNodeAt(&pMenu->root.Items,0);pNode;pNode=PX_ListNodeNext(pNode))
	{
		px_int cWidth,cHeight;
		PX_Object_Menu_Item *pSubItem=PX_LIST_NODETDATA(PX_Object_Menu_Item,pNode);
		PX_FontModuleTextGetRenderWidthHeight(pMenu->fontmodule,pSubItem->Text,&cWidth,&cHeight);
		if (cWidth>maxWidth)
		{
			maxWidth=cWidth;
		}
		if (cHeight>maxHeight)
		{
			maxHeight=cHeight;
		}
	}

	maxWidth+=PX_MENU_ITEM_SPACER_SIZE*2;
	maxHeight+=PX_MENU_ITEM_SPACER_SIZE*2;


	if (maxWidth<pMenu->minimumSize)
	{
		maxWidth=pMenu->minimumSize;
	}

	for (pNode=PX_ListNodeAt(&pMenu->root.Items,0);pNode;pNode=PX_ListNodeNext(pNode))
	{
		PX_Object_Menu_Item *pSubItem=PX_LIST_NODETDATA(PX_Object_Menu_Item,pNode);
		pSubItem->x=x+index*maxWidth;
		pSubItem->y=y;
		pSubItem->width=maxWidth;
		pSubItem->height=maxHeight;
		index++;
	}


	for (pNode=PX_ListNodeAt(&pMenu->root.Items,0);pNode;pNode=PX_ListNodeNext(pNode))
	{
		PX_Object_Menu_Item *pSubItem=PX_LIST_NODETDATA(PX_Object_Menu_Item,pNode);
		PX_MenuSubMenuUpdateEx(pMenu,pSubItem,pSubItem->x,pSubItem->y+pSubItem->height);
	}
}
static px_bool PX_Object_MenuInitialize(px_memorypool *mp,PX_Object_Menu *pMenu,px_int x,int y,px_int width,PX_FontModule *fontmodule)
{
	PX_memset(pMenu,0,sizeof(PX_Object_Menu));
	pMenu->fontmodule=fontmodule;
	pMenu->minimumSize=width;
	pMenu->mp=mp;
	pMenu->activating=PX_FALSE;
	pMenu->backgroundColor=PX_COLOR(255,255,255,255);
	pMenu->cursorColor=PX_COLOR(255,192,192,192);
	pMenu->fontColor=PX_COLOR(255,0,0,0);
	PX_ListInitialize(mp,&pMenu->root.Items);
	pMenu->root.Activated=PX_TRUE;
	return PX_TRUE;
}
typedef enum
{
	PX_MENU_CURSORACTION_MOVE,
	PX_MENU_CURSORACTION_DOWN,
}PX_MENU_CURSORACTION;
static px_void PX_MenuClearParent(PX_Object_Menu *pMenu,PX_Object_Menu_Item *pItem)
{
	px_list_node *pNode=PX_NULL;

	for (pNode=PX_ListNodeAt(&pItem->pParent->Items,0);pNode;pNode=PX_ListNodeNext(pNode))
	{
		PX_Object_Menu_Item *pSubItem=PX_LIST_NODETDATA(PX_Object_Menu_Item,pNode);
		pSubItem->onCursor=PX_FALSE;
		pSubItem->Activated=PX_FALSE;
	}
}
static px_void PX_MenuClearChild(PX_Object_Menu *pMenu,PX_Object_Menu_Item *pItem)
{
	px_list_node *pNode=PX_NULL;

	for (pNode=PX_ListNodeAt(&pItem->Items,0);pNode;pNode=PX_ListNodeNext(pNode))
	{
		PX_Object_Menu_Item *pSubItem=PX_LIST_NODETDATA(PX_Object_Menu_Item,pNode);
		pSubItem->onCursor=PX_FALSE;
		pSubItem->Activated=PX_FALSE;
	}
}
static px_void PX_MenuClearState(PX_Object *pMenuObject)
{
	PX_Object_Menu *pMenu=PX_Object_GetMenu(pMenuObject);
	px_list_node *pNode=PX_NULL;

	for (pNode=PX_ListNodeAt(&pMenu->root.Items,0);pNode;pNode=PX_ListNodeNext(pNode))
	{
		PX_Object_Menu_Item *pSubItem=PX_LIST_NODETDATA(PX_Object_Menu_Item,pNode);
		pSubItem->onCursor=PX_FALSE;
		pSubItem->Activated=PX_FALSE;
	}
	PX_ObjectClearFocus(pMenuObject);
	pMenu->activating=PX_FALSE;
}
px_bool PX_Menu_CursorAction(PX_Object *pObject,PX_Object_Menu_Item *pItem,px_float x,px_float y,PX_MENU_CURSORACTION action)
{
	px_list_node *pNode=PX_NULL;
	PX_Object_Menu *pMenu=PX_Object_GetMenu(pObject);

	if (PX_isXYInRegion(x,y,(px_float)pItem->x,(px_float)pItem->y,(px_float)pItem->width,(px_float)pItem->height))
	{
		PX_MenuClearParent(pMenu,pItem);
		switch (action)
		{
		case PX_MENU_CURSORACTION_MOVE:
			{
				pItem->onCursor=PX_TRUE;
				if (pMenu->activating&&pItem->Items.size!=0)
				{
					pItem->Activated=PX_TRUE;
					PX_MenuClearChild(pMenu,pItem);
				}
			}
			break;
		case PX_MENU_CURSORACTION_DOWN:
			{
				pItem->onCursor=PX_TRUE;
				pItem->Activated=PX_TRUE;

				if (pItem->func_callback)
				{
					pItem->func_callback(pItem->user_ptr);
				}
				if (pItem->Items.size==0)
				{
					PX_MenuClearState(pObject);
				}
				else
				{
					PX_MenuClearChild(pMenu,pItem);
				}
				return PX_TRUE;
			}
			break;
		}
	}
	else
	{
		pItem->onCursor=PX_FALSE;
	}

	if (!pItem->Activated)
	{
		return PX_FALSE;
	}

	for (pNode=PX_ListNodeAt(&pItem->Items,0);pNode;pNode=PX_ListNodeNext(pNode))
	{
		PX_Object_Menu_Item *pSubItem=PX_LIST_NODETDATA(PX_Object_Menu_Item,pNode);
		if(PX_Menu_CursorAction(pObject,pSubItem,x,y,action))
			return PX_TRUE;
	}
	return PX_FALSE;
}
px_void PX_MenuPostEvent_OnCursorDown(PX_Object *pObject,PX_Object_Event e)
{
	px_list_node *pNode=PX_NULL;
	px_float x=PX_Object_Event_GetCursorX(e);
	px_float y=PX_Object_Event_GetCursorY(e);
	PX_Object_Menu *pMenu=PX_Object_GetMenu(pObject);

	if (!pMenu->activating)
	{
		for (pNode=PX_ListNodeAt(&pMenu->root.Items,0);pNode;pNode=PX_ListNodeNext(pNode))
		{
			PX_Object_Menu_Item *pSubItem=PX_LIST_NODETDATA(PX_Object_Menu_Item,pNode);
			if (PX_isXYInRegion(x,y,(px_float)pSubItem->x,(px_float)pSubItem->y,(px_float)pSubItem->width,(px_float)pSubItem->height))
			{
				pMenu->activating=PX_TRUE;
				PX_ObjectSetFocus(pObject);
				pSubItem->Activated=PX_TRUE;
				PX_MenuClearChild(pMenu,pSubItem);
				return;
			}
		}
		PX_ObjectClearFocus(pObject);
		pMenu->activating=PX_FALSE;
		return;
	}
	else
	{
		if (!PX_Menu_CursorAction(pObject,&pMenu->root,x,y,PX_MENU_CURSORACTION_DOWN))
		{
			PX_MenuClearState(pObject);
		}
	}
}
px_void PX_MenuPostEvent_OnCursorMove(PX_Object *pObject,PX_Object_Event e)
{
	PX_Object_Menu *pMenu=PX_Object_GetMenu(pObject);
	px_float x=PX_Object_Event_GetCursorX(e);
	px_float y=PX_Object_Event_GetCursorY(e);
	PX_Menu_CursorAction(pObject,&pMenu->root,x,y,PX_MENU_CURSORACTION_MOVE);	
}
px_void PX_Object_MenuOnCursorEventEx(PX_Object *pObject,PX_Object_Event e)
{
	
	switch(e.Event)
	{
	case PX_OBJECT_EVENT_CURSORMOVE:
	case PX_OBJECT_EVENT_CURSORDRAG:
		{
			PX_MenuPostEvent_OnCursorMove(pObject,e);
		}
		break;
	case PX_OBJECT_EVENT_CURSORDOWN:
		{
			PX_MenuPostEvent_OnCursorDown(pObject,e);
		}
		break;
	default:
		break;
	}

}
static px_void PX_MenuRenderItem(px_surface *pSurface,PX_Object_Menu *pMenu,PX_Object_Menu_Item *pItem,px_bool broot,px_dword elpased)
{
	if (pItem->width&&pItem->height)
	{
		if (pItem->onCursor||pItem->Activated)
		{
			PX_GeoDrawRect(pSurface,pItem->x,pItem->y,pItem->x+pItem->width-1,pItem->y+pItem->height-1,pMenu->cursorColor);
		}
		else
		{
			PX_GeoDrawRect(pSurface,pItem->x,pItem->y,pItem->x+pItem->width-1,pItem->y+pItem->height-1,pMenu->backgroundColor);
		}

		if (broot)
			PX_FontModuleDrawText(pSurface,pMenu->fontmodule,pItem->x+pItem->width/2,pItem->y+pItem->height/2,PX_ALIGN_CENTER,pItem->Text,pMenu->fontColor);
		else
		{
			PX_FontModuleDrawText(pSurface,pMenu->fontmodule,pItem->x,pItem->y+pItem->height/2,PX_ALIGN_LEFTMID,pItem->Text,pMenu->fontColor);
			if (pItem->Items.size!=0)
			{
				PX_GeoDrawTriangle(pSurface,
					PX_POINT2D((px_float)(pItem->x+pItem->width-6),(px_float)(pItem->y+pItem->height/2)),
					PX_POINT2D((px_float)(pItem->x+pItem->width-12),(px_float)(pItem->y+pItem->height/2-4)),
					PX_POINT2D((px_float)(pItem->x+pItem->width-12),(px_float)(pItem->y+pItem->height/2+4)),
					pMenu->fontColor);
			}
		}

	}


	if (pItem->Activated)
	{
		if(pItem->Items.size)
		{
			px_list_node *pNode=PX_NULL;
			for (pNode=PX_ListNodeAt(&pItem->Items,0);pNode;pNode=PX_ListNodeNext(pNode))
			{
				PX_Object_Menu_Item *pSubItem=PX_LIST_NODETDATA(PX_Object_Menu_Item,pNode);
				PX_MenuRenderItem(pSurface,pMenu,pSubItem,PX_FALSE,elpased);
			}
		}
	}
}
px_void PX_Object_MenuRenderEx(px_surface *pSurface,PX_Object_Menu *pMenu,px_dword elpased)
{
	px_list_node *pNode=PX_NULL;

	for (pNode=PX_ListNodeAt(&pMenu->root.Items,0);pNode;pNode=PX_ListNodeNext(pNode))
	{
		PX_Object_Menu_Item *pSubItem=PX_LIST_NODETDATA(PX_Object_Menu_Item,pNode);
		PX_MenuRenderItem(pSurface,pMenu,pSubItem,PX_TRUE,elpased);
	}

}
static px_void PX_MenuItemFree(PX_Object_Menu_Item *pItem)
{
	px_list_node *pNode=PX_NULL;

	for (pNode=PX_ListNodeAt(&pItem->Items,0);pNode;pNode=PX_ListNodeNext(pNode))
	{
		PX_Object_Menu_Item *pSubItem=PX_LIST_NODETDATA(PX_Object_Menu_Item,pNode);
		PX_MenuItemFree(pSubItem);
	}
	PX_ListFree(&pItem->Items);
}
px_void PX_Object_MenuFreeEx(PX_Object_Menu *pMenu)
{
	PX_MenuItemFree(&pMenu->root);
}
PX_Object_Menu_Item * PX_Object_MenuAddItem(PX_Object *pObject,PX_Object_Menu_Item *parent,const px_char Text[],PX_MenuExecuteFunc _callback,px_void *ptr)
{
	PX_Object_Menu_Item item,*ret;
	PX_Object_Menu *pMenu=PX_Object_GetMenu(pObject);
	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;

	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;

	PX_memset(&item,0,sizeof(PX_Object_Menu_Item));
	PX_strcpy(item.Text,Text,sizeof(item.Text));
	item.func_callback=_callback;
	PX_ListInitialize(pMenu->mp,&item.Items);
	item.pParent=parent;
	item.user_ptr=ptr;
	ret=(PX_Object_Menu_Item *)PX_ListPush(&parent->Items,&item,sizeof(item));
	if (ret!=PX_NULL)
	{
		PX_MenuSubMenuUpdate((px_int)objx,(px_int)objy,pMenu);
	}
	return ret;
}



px_void PX_Object_MenuFree(PX_Object *Obj)
{
	PX_Object_MenuFreeEx(PX_Object_GetMenu(Obj));
}

px_void PX_Object_MenuRender(px_surface *psurface, PX_Object *Obj,px_uint elpased)
{
	PX_Object_MenuRenderEx(psurface,PX_Object_GetMenu(Obj),elpased);
}

px_void PX_Object_MenuOnCursorEvent(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Object_MenuOnCursorEventEx(pObject,e);
}

PX_Object * PX_Object_MenuCreate(px_memorypool *mp,PX_Object *Parent,px_int x,int y,px_int width,PX_FontModule *fontmodule)
{
	PX_Object *pObject;
	PX_Object_Menu *pMenu=(PX_Object_Menu *)MP_Malloc(mp,sizeof(PX_Object_Menu));
	
	if (pMenu==PX_NULL)
	{
		return PX_NULL;
	}

	if (!PX_Object_MenuInitialize(mp,pMenu,x,y,width,fontmodule))
	{
		return PX_NULL;
	}


	pObject=PX_ObjectCreate(mp,Parent,(px_float)x,(px_float)y,0,0,0,0);
	if (pObject==PX_NULL)
	{
		MP_Free(pObject->mp,pMenu);
		return PX_NULL;
	}
	

	pObject->pObject=pMenu;
	pObject->Enabled=PX_TRUE;
	pObject->Visible=PX_TRUE;
	pObject->Type=PX_OBJECT_TYPE_MENU;
	pObject->Func_ObjectFree=PX_Object_MenuFree;
	pObject->Func_ObjectRender=PX_Object_MenuRender;
	
	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_CURSORDOWN,PX_Object_MenuOnCursorEvent,PX_NULL);
	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_CURSORMOVE,PX_Object_MenuOnCursorEvent,PX_NULL);
	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_CURSORDRAG,PX_Object_MenuOnCursorEvent,PX_NULL);
	return pObject;
}

PX_Object_Menu_Item * PX_Object_MenuGetRootItem(PX_Object *pMenuObject)
{
	return &PX_Object_GetMenu(pMenuObject)->root;
}

//////////////////////////////////////////////////////////////////////////
//
PX_Object_SelectBar *PX_Object_GetSelectBar(PX_Object *pSelecrBar)
{
	if (pSelecrBar->Type==PX_OBJECT_TYPE_SELECTBAR)
	{
		return (PX_Object_SelectBar *)pSelecrBar->pObject;
	}
	return PX_NULL;
}
static px_void PX_SelecrbarClearCurrentCursor(PX_Object_SelectBar *pSelectbar)
{
	px_int i;
	for (i=0;i<pSelectbar->Items.size;i++)
	{
		PX_VECTORAT(PX_Object_SelectBar_Item,&pSelectbar->Items,i)->onCursor=PX_FALSE;
	}
}
static px_void PX_SelectbarOnCursorMove(PX_Object *pObject,px_float x,px_float y)
{
	PX_Object_SelectBar *pSelectbar=PX_Object_GetSelectBar(pObject);
	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;

	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;


	if(pSelectbar->activating)
	{
		px_int index;
		if (pSelectbar->Items.size>pSelectbar->maxDisplayCount)
		{
			if (x<objx||x>objx+objWidth-16)
			{
				return;
			}
		}
		else
		{
			if (x<objx||x>objx+objWidth)
			{
				return;
			}
		}
		
		index=(px_int)((y-objy-objHeight)/pSelectbar->ItemHeight);
		PX_SelecrbarClearCurrentCursor(pSelectbar);

		if (y<objy+objHeight||index<0||index>=pSelectbar->maxDisplayCount||index>=pSelectbar->Items.size)
		{
			return;
		}
		else
		{
			PX_VECTORAT(PX_Object_SelectBar_Item,&pSelectbar->Items,index+pSelectbar->currentDisplayOffsetIndex)->onCursor=PX_TRUE;
		}
	}
	else
	{
		if(PX_isPointInRect(PX_POINT(x,y,0),PX_RECT(objx,objy,objWidth,objHeight)))
		{
			pSelectbar->onCursor=PX_TRUE;
		}
		else
		{
			pSelectbar->onCursor=PX_FALSE;
		}
	}
}
static px_void PX_SelectbarOnCursorDown(PX_Object *pObject,px_float x,px_float y)
{
	PX_Object_SelectBar *pSelectbar=PX_Object_GetSelectBar(pObject);
	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;

	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;

	if(pSelectbar->activating)
	{
		px_int index;
		if (pSelectbar->Items.size>pSelectbar->maxDisplayCount)
		{
			if (x<objx||x>objx+objWidth)
			{
				PX_SelecrbarClearCurrentCursor(pSelectbar);
				pSelectbar->onCursor=PX_FALSE;
				pSelectbar->activating=PX_FALSE;
				PX_ObjectClearFocus(pObject);
				return;
			}
			if (x>=objx+objWidth-16&&x<=objx+objWidth)
			{
				return;
			}
		}
		else
		{
			if (x<objx||x>objx+objWidth)
			{
				PX_SelecrbarClearCurrentCursor(pSelectbar);
				pSelectbar->activating=PX_FALSE;
				PX_ObjectClearFocus(pObject);
				pSelectbar->onCursor=PX_FALSE;
				return;
			}
		}

		index=(px_int)((y-objy-objHeight)/pSelectbar->ItemHeight);
		PX_SelecrbarClearCurrentCursor(pSelectbar);
		if (y<objy+objHeight||index<0||index>=pSelectbar->maxDisplayCount||index>=pSelectbar->Items.size)
		{
			pSelectbar->activating=PX_FALSE;
			PX_ObjectClearFocus(pObject);
			pSelectbar->onCursor=PX_FALSE;
			return;
		}
		else
		{
			pSelectbar->activating=PX_FALSE;
			PX_ObjectClearFocus(pObject);
			pSelectbar->selectIndex=index+pSelectbar->currentDisplayOffsetIndex;
			do 
			{
				PX_Object_Event e;
				e.Event=PX_OBJECT_EVENT_VALUECHANGED;
				PX_Object_Event_SetIndex(&e,index);
				PX_ObjectPostEvent(pObject,e);
			} while (0);
		}
		pSelectbar->onCursor=PX_FALSE;
	}
	else
	{
		if (pSelectbar->Items.size==0)
		{
			return;
		}
		
		if(PX_isPointInRect(PX_POINT(x,y,0),PX_RECT(objx,objy,objWidth,objHeight)))
		{
			pSelectbar->activating=PX_TRUE;
			PX_ObjectSetFocus(pObject);
		}
	}
	
}

static px_void PX_SelectbarOnCursorWheel(PX_Object *pObject,px_float z)
{
	PX_Object_SelectBar *pSelectbar=PX_Object_GetSelectBar(pObject);
	if (pSelectbar->activating)
	{
		if (pSelectbar->Items.size>pSelectbar->maxDisplayCount)
		{
			if (z<0)
			{
				PX_Object_SliderBarSetValue(pSelectbar->sliderBar,PX_Object_SliderBarGetValue(pSelectbar->sliderBar)+1);
			}
			else
			{
				PX_Object_SliderBarSetValue(pSelectbar->sliderBar,PX_Object_SliderBarGetValue(pSelectbar->sliderBar)-1);
			}
		}
	}
}
static px_void PX_SelectbarOnCursorEvent(PX_Object *pSelectBarObject,PX_Object_Event e,px_void *ptr)
{
	switch(e.Event)
	{
	case PX_OBJECT_EVENT_CURSORMOVE:
	case PX_OBJECT_EVENT_CURSORDRAG:
		{
			PX_SelectbarOnCursorMove(pSelectBarObject,PX_Object_Event_GetCursorX(e),PX_Object_Event_GetCursorY(e));
		}
		break;
	case PX_OBJECT_EVENT_CURSORDOWN:
		{
			PX_SelectbarOnCursorDown(pSelectBarObject,PX_Object_Event_GetCursorX(e),PX_Object_Event_GetCursorY(e));
		}
		break;
	case PX_OBJECT_EVENT_CURSORWHEEL:
		{
			PX_SelectbarOnCursorWheel(pSelectBarObject,PX_Object_Event_GetCursorZ(e));
		}
		break;
	default:
		break;
	}
}
static px_void PX_SelectbarFree(PX_Object *pObject)
{
	PX_VectorFree(&PX_Object_GetSelectBar(pObject)->Items);
}
static px_void PX_SelectbarRender(px_surface *pRenderSurface,PX_Object *pObject,px_dword elpase)
{
	PX_Object_SelectBar *pSelectBar=PX_Object_GetSelectBar(pObject);
	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;

	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;


	

	//border
	if (pSelectBar->style==PX_OBJECT_SELECTBAR_STYLE_RECT)
	{
		//background
		if (pSelectBar->onCursor)
		{
			PX_GeoDrawRect(pRenderSurface,(px_int)objx,(px_int)objy,(px_int)(objx+objWidth-1),(px_int)(objy+objHeight-1),pSelectBar->cursorColor);
		}
		else
		{
			PX_GeoDrawRect(pRenderSurface,(px_int)objx,(px_int)objy,(px_int)(objx+objWidth-1),(px_int)(objy+objHeight-1),pSelectBar->backgroundColor);
		}

		PX_GeoDrawBorder(pRenderSurface,(px_int)objx,(px_int)objy,(px_int)(objx+objWidth-1),(px_int)(objy+objHeight-1),1,pSelectBar->borderColor);
		
	}
	else if (pSelectBar->style==PX_OBJECT_SELECTBAR_STYLE_ROUNDRECT)
	{
		if (pSelectBar->onCursor)
		{
			PX_GeoDrawSolidRoundRect(pRenderSurface,(px_int)objx,(px_int)objy,(px_int)(objx+objWidth-1),(px_int)(objy+objHeight-1),objHeight/2,pSelectBar->cursorColor);
		}
		else
		{
			PX_GeoDrawSolidRoundRect(pRenderSurface,(px_int)objx,(px_int)objy,(px_int)(objx+objWidth-1),(px_int)(objy+objHeight-1),objHeight/2,pSelectBar->backgroundColor);
		}

		PX_GeoDrawRoundRect(pRenderSurface,(px_int)objx,(px_int)objy,(px_int)(objx+objWidth-1),(px_int)(objy+objHeight-1),objHeight/2,1,pSelectBar->borderColor);
	}

	//font
	do 
	{
		if (pSelectBar->selectIndex>=0&&pSelectBar->selectIndex<pSelectBar->Items.size)
		{
			PX_Object_SelectBar_Item *pItem=PX_VECTORAT(PX_Object_SelectBar_Item,&pSelectBar->Items,pSelectBar->selectIndex);
			PX_FontModuleDrawText(pRenderSurface,pSelectBar->fontmodule,(px_int)(objx+objHeight/2+1),(px_int)(objy+objHeight/2),PX_ALIGN_LEFTMID,pItem->Text,pSelectBar->fontColor);
		}
	} while (0);
	
	//triangle
	PX_GeoDrawTriangle(pRenderSurface,
		PX_POINT2D(objx+objWidth-16,objy+objHeight/2-3),
		PX_POINT2D(objx+objWidth-4,objy+objHeight/2-3),
		PX_POINT2D(objx+objWidth-10,objy+objHeight/2+3),
		pSelectBar->borderColor
		);

	if (pSelectBar->activating)
	{
		px_int i;
		px_int count=pSelectBar->maxDisplayCount>pSelectBar->Items.size?pSelectBar->Items.size:pSelectBar->maxDisplayCount;

		for (i=0;i<count;i++)
		{
			PX_Object_SelectBar_Item *pItem=PX_VECTORAT(PX_Object_SelectBar_Item,&pSelectBar->Items,i+pSelectBar->currentDisplayOffsetIndex);
			
			if (pItem->onCursor)
			{
				PX_GeoDrawRect(pRenderSurface,
					(px_int)(objx),
					(px_int)(objy+objHeight+i*pSelectBar->ItemHeight),
					(px_int)(objx+objWidth),
					(px_int)(objy+objHeight+(i+1)*pSelectBar->ItemHeight),
					pSelectBar->cursorColor
					);
			}
			else
			{
				PX_GeoDrawRect(pRenderSurface,
					(px_int)(objx),
					(px_int)(objy+objHeight+i*pSelectBar->ItemHeight),
					(px_int)(objx+objWidth),
					(px_int)(objy+objHeight+(i+1)*pSelectBar->ItemHeight),
					pSelectBar->backgroundColor
					);
			}
			
			PX_FontModuleDrawText(pRenderSurface,pSelectBar->fontmodule,(px_int)(objx+objHeight/2+1),
				(px_int)(objy+objHeight+(i*pSelectBar->ItemHeight)+pSelectBar->ItemHeight/2),
				PX_ALIGN_LEFTMID,
				pItem->Text,
				pSelectBar->fontColor
				);
		}
		PX_GeoDrawBorder(pRenderSurface,
			(px_int)objx,
			(px_int)(objy+objHeight),
			(px_int)(objx+objWidth-1),
			(px_int)(objy+objHeight+count*pSelectBar->ItemHeight),
			1,
			pSelectBar->borderColor
			);

		if (pSelectBar->Items.size>pSelectBar->maxDisplayCount)
		{
			pSelectBar->sliderBar->Visible=PX_TRUE;
			pSelectBar->sliderBar->x=objWidth-16;
			pSelectBar->sliderBar->y=objHeight;
			pSelectBar->sliderBar->Width=16;
			pSelectBar->sliderBar->Height=count*pSelectBar->ItemHeight*1.0f;
			PX_Object_SliderBarSetBackgroundColor(pSelectBar->sliderBar,pSelectBar->backgroundColor);
			PX_Object_SliderBarSetColor(pSelectBar->sliderBar,pSelectBar->borderColor);
			PX_Object_SliderBarSetRange(pSelectBar->sliderBar,0,pSelectBar->Items.size-pSelectBar->maxDisplayCount);
			PX_Object_SliderBarSetSliderButtonLength(pSelectBar->sliderBar,(px_int)(pSelectBar->sliderBar->Height*pSelectBar->maxDisplayCount/pSelectBar->Items.size));
		}
		else
		{
			pSelectBar->sliderBar->Visible=PX_FALSE;
		}
	}
	else
	{
		pSelectBar->sliderBar->Visible=PX_FALSE;
	}

}

px_void PX_Object_SelectBarSliderOnChanged(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Object_SelectBar *pSelectbar=(PX_Object_SelectBar *)ptr;
	pSelectbar->currentDisplayOffsetIndex=PX_Object_SliderBarGetValue(pObject);
}

PX_Object * PX_Object_SelectBarCreate(px_memorypool *mp,PX_Object *Parent,px_int x,int y,px_int width,px_int height,PX_FontModule *fontmodule)
{
	PX_Object *pObject;
	PX_Object_SelectBar *pSelectbar=(PX_Object_SelectBar *)MP_Malloc(mp,sizeof(PX_Object_SelectBar));

	if (pSelectbar==PX_NULL)
	{
		return PX_NULL;
	}

	PX_memset(pSelectbar,0,sizeof(PX_Object_SelectBar));
	pSelectbar->fontmodule=fontmodule;
	pSelectbar->mp=mp;
	pSelectbar->activating=PX_FALSE;
	pSelectbar->currentDisplayOffsetIndex=0;
	if (fontmodule)
	{
		pSelectbar->ItemHeight=fontmodule->max_Height;
	}
	else
	{
		pSelectbar->ItemHeight=__PX_FONT_HEIGHT;
	}
	
	pSelectbar->maxDisplayCount=16;
	pSelectbar->backgroundColor=PX_COLOR(255,255,255,255);
	pSelectbar->cursorColor=PX_COLOR(255,192,192,192);
	pSelectbar->fontColor=PX_COLOR(255,0,0,0);
	pSelectbar->borderColor=PX_COLOR(255,0,0,0);
	pSelectbar->activating=PX_FALSE;
	pSelectbar->style=PX_OBJECT_SELECTBAR_STYLE_RECT;
	PX_VectorInitialize(mp,&pSelectbar->Items,sizeof(PX_Object_SelectBar_Item),16);
	

	pObject=PX_ObjectCreate(mp,Parent,(px_float)x,(px_float)y,0,(px_float)width,(px_float)height,0);
	if (pObject==PX_NULL)
	{
		MP_Free(pObject->mp,pSelectbar);
		return PX_NULL;
	}


	pObject->pObject=pSelectbar;
	pObject->Enabled=PX_TRUE;
	pObject->Visible=PX_TRUE;
	pObject->Type=PX_OBJECT_TYPE_SELECTBAR;
	pObject->Func_ObjectFree=PX_SelectbarFree;
	pObject->Func_ObjectRender=PX_SelectbarRender;
	pObject->OnLostFocusReleaseEvent=PX_TRUE;

	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_CURSORDOWN,PX_SelectbarOnCursorEvent,PX_NULL);
	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_CURSORMOVE,PX_SelectbarOnCursorEvent,PX_NULL);
	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_CURSORDRAG,PX_SelectbarOnCursorEvent,PX_NULL);
	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_CURSORWHEEL,PX_SelectbarOnCursorEvent,PX_NULL);

	pSelectbar->sliderBar=PX_Object_SliderBarCreate(mp,pObject,0,0,0,0,PX_OBJECT_SLIDERBAR_TYPE_VERTICAL,PX_OBJECT_SLIDERBAR_STYLE_BOX);

	if (!pSelectbar->sliderBar)
	{
		PX_ObjectDelete(pObject);
		return PX_NULL;
	}
	PX_ObjectRegisterEvent(pSelectbar->sliderBar,PX_OBJECT_EVENT_VALUECHANGED,PX_Object_SelectBarSliderOnChanged,pSelectbar);
	return pObject;
}

px_int PX_Object_SelectBarAddItem(PX_Object *pSelectBarObject,const px_char Text[])
{
	PX_Object_SelectBar *pSelectBar=PX_Object_GetSelectBar(pSelectBarObject);
	PX_Object_SelectBar_Item item;
	PX_strcpy(item.Text,Text,sizeof(item.Text));
	item.onCursor=PX_FALSE;
	if(PX_VectorPushback(&pSelectBar->Items,&item))
		return pSelectBar->Items.size-1;
	else
		return -1;
}

px_void PX_Object_SelectBarRemoveItem(PX_Object *pObject,px_int index)
{
	PX_Object_SelectBar *pSelectBar=PX_Object_GetSelectBar(pObject);
	if (index>0&&index<pSelectBar->Items.size)
	{
		PX_VectorErase(&pSelectBar->Items,index);
	}
}

px_int PX_Object_SelectBarGetItemIndexByText(PX_Object *pObject,const px_char Text[])
{
	PX_Object_SelectBar *pSelectBar=PX_Object_GetSelectBar(pObject);
	px_int i;
	for (i=0;i<pSelectBar->Items.size;i++)
	{
		PX_Object_SelectBar_Item *pItem=PX_VECTORAT(PX_Object_SelectBar_Item,&pSelectBar->Items,i);
		if (PX_strequ(Text,pItem->Text))
		{
			return i;
		}
	}
	return -1;
}

const px_char * PX_Object_SelectBarGetCurrentText(PX_Object *pObject)
{
	PX_Object_SelectBar *pSelectBar=PX_Object_GetSelectBar(pObject);
	if (pSelectBar)
	{
		return PX_VECTORAT(PX_Object_SelectBar_Item,&pSelectBar->Items,pSelectBar->selectIndex)->Text;
	}
	return "";
}

px_void PX_Object_SelectBarSetDisplayCount(PX_Object *pObject,px_int count)
{
	PX_Object_SelectBar *pSelectBar=PX_Object_GetSelectBar(pObject);
	if (pSelectBar)
	{
		pSelectBar->maxDisplayCount=count;
	}
}

px_int PX_Object_SelectBarGetCurrentIndex(PX_Object *pObject)
{
	PX_Object_SelectBar *pSelectBar=PX_Object_GetSelectBar(pObject);
	if (pSelectBar)
	{
		return pSelectBar->selectIndex;
	}
	return -1;
}

px_void PX_Object_SelectBarSetCurrentIndex(PX_Object *pObject,px_int index)
{
	PX_Object_SelectBar *pSelectBar=PX_Object_GetSelectBar(pObject);
	if (pSelectBar)
	{
		if (index>0&&index<pSelectBar->Items.size)
		{
			pSelectBar->selectIndex=index;
		}
	}
}

px_void PX_Object_SelectBarSetStyle(PX_Object *pObject,PX_OBJECT_SELECTBAR_STYLE style)
{
	PX_Object_SelectBar *pSelectBar=PX_Object_GetSelectBar(pObject);
	if (pSelectBar)
	{
		pSelectBar->style=style;
	}
}

px_void PX_Object_SelectBarSetFontColor(PX_Object *pObject,px_color color)
{
	PX_Object_SelectBar *pSelectBar=PX_Object_GetSelectBar(pObject);
	if (pSelectBar)
	{
		pSelectBar->fontColor=color;
	}
}

px_void PX_Object_SelectBarSetCursorColor(PX_Object *pObject,px_color color)
{
	PX_Object_SelectBar *pSelectBar=PX_Object_GetSelectBar(pObject);
	if (pSelectBar)
	{
		pSelectBar->cursorColor=color;
	}
}

px_void PX_Object_SelectBarSetBorderColor(PX_Object *pObject,px_color color)
{
	PX_Object_SelectBar *pSelectBar=PX_Object_GetSelectBar(pObject);
	if (pSelectBar)
	{
		pSelectBar->borderColor=color;
	}
}

px_void PX_Object_SelectBarSetBackgroundColor(PX_Object *pObject,px_color color)
{
	PX_Object_SelectBar *pSelectBar=PX_Object_GetSelectBar(pObject);
	if (pSelectBar)
	{
		pSelectBar->backgroundColor=color;
	}
}


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

px_void PX_Object_RadioBoxRender(px_surface *psurface, PX_Object *pObject,px_uint elpased)
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
	cbx.BorderColor=PX_COLOR(255,0,0,0);
	cbx.CursorColor=PX_COLOR(0,0,0,0);
	cbx.PushColor=PX_COLOR(0,0,0,0);
	cbx.fm=fm;
	cbx.state=PX_OBJECT_RADIOBOX_STATE_NORMAL;
	PX_strset(cbx.Text,text);
	cbx.TextColor=PX_COLOR(255,0,0,0);
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

px_void PX_Object_RadioBoxSetTextColor(PX_Object *Object,px_color clr)
{
	PX_Object_GetRadioBox(Object)->TextColor=clr;
}

px_void PX_Object_RadioBoxSetCheck(PX_Object *Object,px_bool check)
{
	PX_Object_GetRadioBox(Object)->bCheck=check;
}
//////////////////////////////////////////////////////////////////////////
//explorer

PX_Object_Explorer * PX_Object_GetExplorer(PX_Object *Object)
{
	if(Object->Type==PX_OBJECT_TYPE_EXPLORER)
	{
		return (PX_Object_Explorer *)Object->pObject;
	}
	return PX_NULL;
}

px_void PX_Object_ExplorerOnCursorMove(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	px_float x,y;
	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;
	
	PX_Object_Explorer *pExp=PX_Object_GetExplorer(pObject);
	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;

	x=PX_Object_Event_GetCursorX(e);
	y=PX_Object_Event_GetCursorY(e);
	if (PX_isXYInRegion(x,y,objx,objy+PX_OBJECT_EXPOLRER_MENU_HEIGHT,objWidth-PX_OBJECT_EXPOLRER_SLIDERBAR_WIDTH,objHeight-PX_OBJECT_EXPOLRER_MENU_HEIGHT*2))
	{
		//get cursor index
		px_int i;
		px_int ofty;
		px_int index;
		ofty=PX_Object_SliderBarGetValue(pExp->SliderBar);
		index=(px_int)(y+ofty-objy-PX_OBJECT_EXPOLRER_MENU_HEIGHT)/PX_OBJECT_EXPOLRER_ITEM_HEIGHT;
		//clear
		for (i=0;i<pExp->ItemCount;i++)
		{
			pExp->Items[i].bcursor=PX_FALSE;
		}
		if (index<pExp->ItemCount)
		{
			pExp->Items[index].bcursor=PX_TRUE;
		}
		
	}
}

px_void PX_Object_ExplorerOnButtonBack(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	px_char path[260];
	px_int index;
	PX_Object_Explorer *pExp=PX_Object_GetExplorer((PX_Object *)ptr);
	PX_strcpy(path,PX_Object_EditGetText(pExp->edit_Path),sizeof(path));
	index=PX_strlen(path);
	while (index)
	{
		if (path[index]=='/'||path[index]=='\\')
		{
			break;
		}
		index--;
	}
	path[index]='\0';
	PX_Object_EditSetText(pExp->edit_Path,path);
	PX_Object_ExplorerRefresh((PX_Object *)ptr);
}
px_void PX_Object_ExplorerOnButtonGo(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Object_ExplorerRefresh((PX_Object *)ptr);
}
px_void PX_Object_ExplorerOnButtonOk(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Object_Explorer *pExp=PX_Object_GetExplorer((PX_Object *)ptr);
	PX_ObjectClearFocus((PX_Object *)ptr);
	pExp->returnType=PX_OBJECT_EXPLORER_RETURN_CONFIRM;
	PX_ObjectExecuteEvent((PX_Object *)ptr,e);
	((PX_Object *)ptr)->Visible=PX_FALSE;
	
}
px_void PX_Object_ExplorerOnButtonCancel(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Object_Explorer *pExp=PX_Object_GetExplorer((PX_Object *)ptr);
	PX_ObjectClearFocus((PX_Object *)ptr);
	pExp->returnType=PX_OBJECT_EXPLORER_RETURN_CANCEL;
	((PX_Object *)ptr)->Visible=PX_FALSE;
}

px_void PX_Object_ExplorerOnCursorDown(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	px_float x,y;
	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;
	PX_Object_Explorer *pExp=PX_Object_GetExplorer(pObject);
	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;

	x=PX_Object_Event_GetCursorX(e);
	y=PX_Object_Event_GetCursorY(e);
	if (PX_isXYInRegion(x,y,objx,objy+PX_OBJECT_EXPOLRER_MENU_HEIGHT,objWidth-PX_OBJECT_EXPOLRER_SLIDERBAR_WIDTH,objHeight-PX_OBJECT_EXPOLRER_MENU_HEIGHT*2))
	{
		px_int i=0,selectcount=0;
		px_int ofty;
		//get cursor index
		
		px_int index;
		ofty=PX_Object_SliderBarGetValue(pExp->SliderBar);
		index=(px_int)(y+ofty-objy-PX_OBJECT_EXPOLRER_MENU_HEIGHT)/PX_OBJECT_EXPOLRER_ITEM_HEIGHT;
		if (index<pExp->ItemCount)
		{
			if (pExp->Items[index].bselect&&pExp->Items[index].bFolder)
			{

				//enter path
				px_char *pEditPath=PX_Object_EditGetText(pExp->edit_Path);
				if (pEditPath[PX_strlen(pEditPath)-1]!='/'&&pEditPath[PX_strlen(pEditPath)-1]!='\\')
				{
					PX_Object_EditAppendText(pExp->edit_Path,"\\");
				}
				PX_Object_EditAppendText(pExp->edit_Path,pExp->Items[index].name);

				PX_Object_ExplorerRefresh(pObject);
			}
			else
			{
				if (pExp->MaxSelectedCount!=1)
				{
					for (i=0;i<pExp->ItemCount;i++)
					{
						if(pExp->Items[i].bselect)
						{
							selectcount++;
						}
					}
					if (pExp->Items[index].bselect)
					{
						pExp->Items[index].bselect=PX_FALSE;
					}
					else
					{
						if (selectcount<pExp->MaxSelectedCount)
						{
							pExp->Items[index].bselect=PX_TRUE;
						}
					}
					
				}
				else
				{
					if (pExp->Items[index].bselect)
					{
						pExp->Items[index].bselect=PX_FALSE;
					}
					else
					{
						for (i=0;i<pExp->ItemCount;i++)
						{
							if(pExp->Items[i].bselect)
							{
								pExp->Items[i].bselect=PX_FALSE;
							}
						}
						pExp->Items[index].bselect=PX_TRUE;
					}
					
				}
				
			}
			
		}

		if (PX_Object_ExplorerGetSelectedCount(pObject)==0)
		{
			pExp->btn_Ok->Visible=PX_FALSE;
		}
		else
		{
			pExp->btn_Ok->Visible=PX_TRUE;
		}
	}
}

static px_void PX_Object_ExplorerOnCursorWheel(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Object_Explorer *pExp=PX_Object_GetExplorer(pObject);
	px_float x=PX_Object_Event_GetCursorX(e);
	px_float y=PX_Object_Event_GetCursorY(e);
	px_float z=PX_Object_Event_GetCursorZ(e);
	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;

	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;

	if (PX_isXYInRegion(x,y,objx,objy+PX_OBJECT_EXPOLRER_MENU_HEIGHT,objWidth-PX_OBJECT_EXPOLRER_SLIDERBAR_WIDTH,objHeight-PX_OBJECT_EXPOLRER_MENU_HEIGHT*2))
	{
		if (z<0)
		{
			PX_Object_SliderBarSetValue(pExp->SliderBar,PX_Object_SliderBarGetValue(pExp->SliderBar)+PX_OBJECT_EXPOLRER_ITEM_HEIGHT/2);
		}
		else
		{
			PX_Object_SliderBarSetValue(pExp->SliderBar,PX_Object_SliderBarGetValue(pExp->SliderBar)-PX_OBJECT_EXPOLRER_ITEM_HEIGHT/2);
		}
	}
	
}


px_void PX_Object_ExplorerRender(px_surface *psurface, PX_Object *pObject,px_uint elpased)
{
	PX_Object_Explorer *pExp=PX_Object_GetExplorer(pObject);
	//background
	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;
	px_int val,ofty;

	if (pObject->Width<256)
	{
		pObject->Width=256;
	}
	if (pObject->Height<128)
	{
		pObject->Height=128;
	}


	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;

	val=PX_Object_SliderBarGetValue(pExp->SliderBar);
	ofty=PX_Object_SliderBarGetValue(pExp->SliderBar);

	PX_GeoDrawRect(psurface,(px_int)objx,(px_int)objy,(px_int)(objx+objWidth),(px_int)(objy+objHeight),pExp->backgroundcolor);

	//button back
	pExp->btn_Back->x=8;
	pExp->btn_Back->y=8;
	pExp->btn_Back->Width=PX_OBJECT_EXPOLRER_BUTTON_WIDTH;
	pExp->btn_Back->Height=32;
	PX_Object_PushButtonSetBackgroundColor(pExp->btn_Back,pExp->backgroundcolor);
	PX_Object_PushButtonSetBorderColor(pExp->btn_Back,pExp->bordercolor);
	PX_Object_PushButtonSetTextColor(pExp->btn_Back,pExp->fontcolor);
	PX_Object_PushButtonSetCursorColor(pExp->btn_Back,pExp->cursorcolor);
	PX_Object_PushButtonSetPushColor(pExp->btn_Back,pExp->pushcolor);

	pExp->edit_Path->x=PX_OBJECT_EXPOLRER_BUTTON_WIDTH+8+8;
	pExp->edit_Path->Width=objWidth-PX_OBJECT_EXPOLRER_BUTTON_WIDTH-PX_OBJECT_EXPOLRER_BUTTON_WIDTH-8*4;
	if (pExp->fontmodule)
	{
		pExp->edit_Path->Height=pExp->fontmodule->max_Height+6.0f;
	}
	else
	{
		pExp->edit_Path->Height=20;
	}
	pExp->edit_Path->y=24-pExp->edit_Path->Height/2;
	PX_Object_EditSetBackgroundColor(pExp->edit_Path,pExp->backgroundcolor);
	PX_Object_EditSetBorderColor(pExp->edit_Path,pExp->bordercolor);
	PX_Object_EditSetTextColor(pExp->edit_Path,pExp->fontcolor);
	PX_Object_EditSetCursorColor(pExp->edit_Path,pExp->fontcolor);

	pExp->btn_go->x=objWidth-8-PX_OBJECT_EXPOLRER_BUTTON_WIDTH;
	pExp->btn_go->y=8;
	pExp->btn_go->Width=PX_OBJECT_EXPOLRER_BUTTON_WIDTH;
	pExp->btn_go->Height=32;
	PX_Object_PushButtonSetBackgroundColor(pExp->btn_go,pExp->backgroundcolor);
	PX_Object_PushButtonSetBorderColor(pExp->btn_go,pExp->bordercolor);
	PX_Object_PushButtonSetTextColor(pExp->btn_go,pExp->fontcolor);
	PX_Object_PushButtonSetCursorColor(pExp->btn_go,pExp->cursorcolor);
	PX_Object_PushButtonSetPushColor(pExp->btn_go,pExp->pushcolor);

	
	pExp->SliderBar->x=objx+objWidth-24;
	pExp->SliderBar->Width=24;
	pExp->SliderBar->y=PX_OBJECT_EXPOLRER_MENU_HEIGHT;
	pExp->SliderBar->Height=objHeight-PX_OBJECT_EXPOLRER_MENU_HEIGHT-PX_OBJECT_EXPOLRER_MENU_HEIGHT;
	PX_Object_SliderBarSetBackgroundColor(pExp->SliderBar,pExp->backgroundcolor);
	PX_Object_SliderBarSetColor(pExp->SliderBar,pExp->bordercolor);
	if (pExp->ItemCount*PX_OBJECT_EXPOLRER_ITEM_HEIGHT>objHeight-PX_OBJECT_EXPOLRER_MENU_HEIGHT-PX_OBJECT_EXPOLRER_MENU_HEIGHT)
	{
		pExp->SliderBar->Visible=PX_TRUE;
		PX_Object_SliderBarSetRange(pExp->SliderBar,0,pExp->ItemCount*PX_OBJECT_EXPOLRER_ITEM_HEIGHT-((px_int)objHeight-PX_OBJECT_EXPOLRER_MENU_HEIGHT-PX_OBJECT_EXPOLRER_MENU_HEIGHT));
		PX_Object_SliderBarSetSliderButtonLength(pExp->SliderBar,(px_int)(pExp->SliderBar->Height*pExp->SliderBar->Height/pExp->ItemCount/PX_OBJECT_EXPOLRER_ITEM_HEIGHT));
	}
	else
	{
		PX_Object_SliderBarSetRange(pExp->SliderBar,0,0);
		pExp->SliderBar->Visible=PX_FALSE;
	}
	

	do 
	{
		PX_SurfaceLimitInfo lmInfo;
		px_int startIndex=ofty/PX_OBJECT_EXPOLRER_ITEM_HEIGHT;
		px_int drawy=PX_OBJECT_EXPOLRER_MENU_HEIGHT-(ofty)%PX_OBJECT_EXPOLRER_ITEM_HEIGHT;
		lmInfo=PX_SurfaceGetLimit(psurface);
		PX_SurfaceSetLimit(psurface,(px_int)objx,(px_int)objy+PX_OBJECT_EXPOLRER_MENU_HEIGHT,(px_int)(objx+objWidth),(px_int)(objy+objHeight-PX_OBJECT_EXPOLRER_MENU_HEIGHT));
		while (startIndex<pExp->ItemCount&&drawy<objHeight-PX_OBJECT_EXPOLRER_MENU_HEIGHT)
		{
			PX_Object_Explorer_Item *pItem=&pExp->Items[startIndex];
			
			if (pItem->bcursor)
			{
				PX_GeoDrawRect(psurface,(px_int)objx,(px_int)objy+drawy,(px_int)(objx+objWidth),(px_int)objy+drawy+PX_OBJECT_EXPOLRER_ITEM_HEIGHT-1,pExp->cursorcolor);
			}

			if (pItem->bFolder)
			{
				PX_ShapeRender(psurface,&pExp->folder,(px_int)objx+16+3,(px_int)objy+drawy+PX_OBJECT_EXPOLRER_ITEM_HEIGHT/2,PX_TEXTURERENDER_REFPOINT_CENTER,pExp->fontcolor);
			}
			else
			{
				PX_ShapeRender(psurface,&pExp->file,(px_int)objx+16+3,(px_int)objy+drawy+PX_OBJECT_EXPOLRER_ITEM_HEIGHT/2,PX_TEXTURERENDER_REFPOINT_CENTER,pExp->fontcolor);
			}
			
			PX_FontModuleDrawText(psurface,pExp->fontmodule,(px_int)objx+32+8,(px_int)objy+drawy+PX_OBJECT_EXPOLRER_ITEM_HEIGHT/2,PX_ALIGN_LEFTMID,pItem->name,pExp->fontcolor);

			if (pItem->bselect)
			{
				PX_GeoDrawSolidCircle(psurface,(px_int)(objx+objWidth)-24-16,(px_int)objy+drawy+PX_OBJECT_EXPOLRER_ITEM_HEIGHT/2,8,pExp->fontcolor);
			}
			
			drawy+=PX_OBJECT_EXPOLRER_ITEM_HEIGHT;
			startIndex++;
		}

		PX_SurfaceSetLimitInfo(psurface,lmInfo);

	} while (0);


	PX_GeoDrawLine(psurface,(px_int)objx,PX_OBJECT_EXPOLRER_MENU_HEIGHT,(px_int)(objx+objWidth),PX_OBJECT_EXPOLRER_MENU_HEIGHT,1,pExp->fontcolor);
	PX_GeoDrawLine(psurface,(px_int)objx,(px_int)(objy+objHeight-PX_OBJECT_EXPOLRER_MENU_HEIGHT),(px_int)(objx+objWidth),(px_int)(objy+objHeight-PX_OBJECT_EXPOLRER_MENU_HEIGHT),1,pExp->fontcolor);

	pExp->btn_Ok->x=objWidth-8-PX_OBJECT_EXPOLRER_BUTTON_WIDTH-8-PX_OBJECT_EXPOLRER_BUTTON_WIDTH;
	pExp->btn_Ok->y=objHeight-40;
	pExp->btn_Ok->Width=PX_OBJECT_EXPOLRER_BUTTON_WIDTH;
	pExp->btn_Ok->Height=32;
	PX_Object_PushButtonSetBackgroundColor(pExp->btn_Ok,pExp->backgroundcolor);
	PX_Object_PushButtonSetBorderColor(pExp->btn_Ok,pExp->bordercolor);
	PX_Object_PushButtonSetTextColor(pExp->btn_Ok,pExp->fontcolor);
	PX_Object_PushButtonSetCursorColor(pExp->btn_Ok,pExp->cursorcolor);
	PX_Object_PushButtonSetPushColor(pExp->btn_Ok,pExp->pushcolor);

	pExp->btn_Cancel->x=objx+objWidth-8-PX_OBJECT_EXPOLRER_BUTTON_WIDTH;
	pExp->btn_Cancel->y=objHeight-40;
	pExp->btn_Cancel->Width=PX_OBJECT_EXPOLRER_BUTTON_WIDTH;
	pExp->btn_Cancel->Height=32;
	PX_Object_PushButtonSetBackgroundColor(pExp->btn_Cancel,pExp->backgroundcolor);
	PX_Object_PushButtonSetBorderColor(pExp->btn_Cancel,pExp->bordercolor);
	PX_Object_PushButtonSetTextColor(pExp->btn_Cancel,pExp->fontcolor);
	PX_Object_PushButtonSetCursorColor(pExp->btn_Cancel,pExp->cursorcolor);
	PX_Object_PushButtonSetPushColor(pExp->btn_Cancel,pExp->pushcolor);
}

px_void PX_Object_ExplorerFree(PX_Object *pObject)
{
	PX_Object_Explorer *pExp=PX_Object_GetExplorer(pObject);
	PX_ShapeFree(&pExp->file);
	PX_ShapeFree(&pExp->folder);
}

px_byte PX_Object_Explorer_fileLogoData[]={
#include "PX_object_explorer_filelogo.h"
};

px_byte PX_Object_Explorer_folderLogoData[]={
#include "PX_object_explorer_folderlogo.h"
};

PX_Object * PX_Object_ExplorerCreate(px_memorypool *mp, PX_Object *Parent,px_int x,px_int y,px_int Width,px_int Height,PX_FontModule *fm, PX_ExplorerGetPathFolderCount _func_gpfdc, PX_ExplorerGetPathFileCount _func_gpfec, PX_ExplorerGetPathFolderName _func_gpfdn, PX_ExplorerGetPathFileName _func_gpfcn,const px_char path[])
{
	PX_Object_Explorer explorer;
	PX_Object *pObject;
	PX_memset(&explorer,0,sizeof(explorer));
	if (Width<256)
	{
		Width=256;
	}
	if (Height<128)
	{
		Height=128;
	}
	explorer.backgroundcolor=PX_COLOR(255,64,64,64);
	explorer.cursorcolor=PX_COLOR(255,32,32,32);
	explorer.fontcolor=PX_COLOR(255,192,192,192);
	explorer.bordercolor=PX_COLOR(255,255,255,255);

	explorer.func_getpathfilecount=_func_gpfec;
	explorer.func_getpathfilename=_func_gpfcn;
	explorer.func_getpathfoldercount=_func_gpfdc;
	explorer.func_getpathfoldername=_func_gpfdn;
	explorer.returnType=PX_OBJECT_EXPLORER_RETURN_CANCEL;
	explorer.MaxSelectedCount=1;
	explorer.fontmodule=fm;

	if(!PX_ShapeCreateFromMemory(mp,PX_Object_Explorer_fileLogoData,sizeof(PX_Object_Explorer_fileLogoData),&explorer.file))return PX_NULL;
	if(!PX_ShapeCreateFromMemory(mp,PX_Object_Explorer_folderLogoData,sizeof(PX_Object_Explorer_folderLogoData),&explorer.folder))return PX_NULL;

	pObject=PX_ObjectCreateEx(mp,Parent,(px_float)x,(px_float)y,0,(px_float)Width,(px_float)Height,0,PX_OBJECT_TYPE_EXPLORER,PX_NULL,PX_Object_ExplorerRender,PX_Object_ExplorerFree,&explorer,sizeof(PX_Object_Explorer));

	if (!pObject)
	{
		return PX_NULL;
	}

	do 
	{
		PX_Object_Explorer *pExp=PX_Object_GetExplorer(pObject);
		pExp->btn_Back=PX_Object_PushButtonCreate(mp,pObject,0,0,1,1,"<-Back",pExp->fontmodule,pExp->fontcolor);
		pExp->btn_go=PX_Object_PushButtonCreate(mp,pObject,0,0,1,1,"Go->",pExp->fontmodule,pExp->fontcolor);
		pExp->btn_Ok=PX_Object_PushButtonCreate(mp,pObject,0,0,1,1,"Ok",pExp->fontmodule,pExp->fontcolor);
		pExp->btn_Cancel=PX_Object_PushButtonCreate(mp,pObject,0,0,1,1,"Cancel",pExp->fontmodule,pExp->fontcolor);
		pExp->edit_Path=PX_Object_EditCreate(mp,pObject,0,0,1,1,pExp->fontmodule,pExp->fontcolor);
		PX_Object_EditSetText(pExp->edit_Path,path);
		pExp->SliderBar=PX_Object_SliderBarCreate(mp,pObject,0,0,24,24,PX_OBJECT_SLIDERBAR_TYPE_VERTICAL,PX_OBJECT_SLIDERBAR_STYLE_BOX);
		pExp->btn_Ok->Visible=PX_FALSE;
		PX_ObjectRegisterEvent(pExp->btn_Back,PX_OBJECT_EVENT_EXECUTE,PX_Object_ExplorerOnButtonBack,pObject);
		PX_ObjectRegisterEvent(pExp->btn_go,PX_OBJECT_EVENT_EXECUTE,PX_Object_ExplorerOnButtonGo,pObject);
		PX_ObjectRegisterEvent(pExp->btn_Ok,PX_OBJECT_EVENT_EXECUTE,PX_Object_ExplorerOnButtonOk,pObject);
		PX_ObjectRegisterEvent(pExp->btn_Cancel,PX_OBJECT_EVENT_EXECUTE,PX_Object_ExplorerOnButtonCancel,pObject);

		PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_CURSORMOVE,PX_Object_ExplorerOnCursorMove,pObject);
		PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_CURSORDOWN,PX_Object_ExplorerOnCursorDown,pObject);
		PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_CURSORWHEEL,PX_Object_ExplorerOnCursorWheel,pObject);
	} while (0);

	PX_Object_ExplorerRefresh(pObject);
	pObject->Visible=PX_FALSE;
	pObject->OnLostFocusReleaseEvent=PX_TRUE;
	return pObject;
}

px_void PX_Object_ExplorerSetBorderColor(PX_Object *Object,px_color clr)
{
	PX_Object_Explorer *pExp=PX_Object_GetExplorer(Object);
	if (pExp)
	{
		pExp->bordercolor=clr;
	}
}

px_void PX_Object_ExplorerSetPushColor(PX_Object *Object,px_color clr)
{
	PX_Object_Explorer *pExp=PX_Object_GetExplorer(Object);
	if (pExp)
	{
		pExp->pushcolor=clr;
	}
}

px_void PX_Object_ExplorerSetCursorColor(PX_Object *Object,px_color clr)
{
	PX_Object_Explorer *pExp=PX_Object_GetExplorer(Object);
	if (pExp)
	{
		pExp->cursorcolor=clr;
	}
}

px_void PX_Object_ExplorerSetTextColor(PX_Object *Object,px_color clr)
{
	PX_Object_Explorer *pExp=PX_Object_GetExplorer(Object);
	if (pExp)
	{
		pExp->fontcolor=clr;
	}
}


px_void PX_Object_ExplorerRefresh(PX_Object *Object)
{
	PX_Object_Explorer *pExp=PX_Object_GetExplorer(Object);
	px_char FileNames[PX_EXPLORER_MAX_ITEMS][260];
	
	if (pExp)
	{
		px_int i;
		px_int count;
		px_char *path=PX_Object_EditGetText(pExp->edit_Path);

		if (!pExp->func_getpathfilecount||!pExp->func_getpathfilename||!pExp->func_getpathfoldercount||!pExp->func_getpathfoldername)
		{
			return;
		}

		
		pExp->ItemCount=0;

		//folder
		count=pExp->func_getpathfoldercount(path,pExp->filter);
		if (count>PX_EXPLORER_MAX_ITEMS)
		{
			count=PX_EXPLORER_MAX_ITEMS;
		}
		pExp->func_getpathfoldername(path,count,FileNames,pExp->filter);
		for (i=0;i<count;i++)
		{
			pExp->Items[i].bcursor=PX_FALSE;
			pExp->Items[i].bFolder=PX_TRUE;
			pExp->Items[i].bselect=PX_FALSE;
			PX_strcpy(pExp->Items[i].name,FileNames[i],sizeof(pExp->Items[i].name));
		}
		pExp->ItemCount+=count;

		count=pExp->func_getpathfilecount(path,pExp->filter);
		if (count>PX_EXPLORER_MAX_ITEMS-pExp->ItemCount)
		{
			count=PX_EXPLORER_MAX_ITEMS-pExp->ItemCount;
		}
		pExp->func_getpathfilename(path,count,FileNames,pExp->filter);
		for (i=0;i<count;i++)
		{
			pExp->Items[pExp->ItemCount+i].bcursor=PX_FALSE;
			pExp->Items[pExp->ItemCount+i].bFolder=PX_FALSE;
			pExp->Items[pExp->ItemCount+i].bselect=PX_FALSE;
			PX_strcpy(pExp->Items[pExp->ItemCount+i].name,FileNames[i],sizeof(pExp->Items[0].name));
		}
		pExp->ItemCount+=count;
		PX_Object_SliderBarSetValue(pExp->SliderBar,0);
	}
}
px_int PX_Object_ExplorerGetSelectedCount(PX_Object *Object)
{
	PX_Object_Explorer *pExp=PX_Object_GetExplorer(Object);
	px_int selectCount=0;
	px_int i;
	for (i=0;i<pExp->ItemCount;i++)
	{
		if (pExp->Items[i].bselect)
		{
			selectCount++;
		}
	}
	return selectCount;
}
px_void PX_Object_ExplorerGetPath(PX_Object *Object,px_char path[PX_EXPLORER_MAX_PATH_LEN],px_int index)
{
	PX_Object_Explorer *pExp=PX_Object_GetExplorer(Object);
	px_int selectCount=0,selectIndex=0;
	px_int i;
	for (i=0;i<pExp->ItemCount;i++)
	{
		if (pExp->Items[i].bselect)
		{
			selectCount++;
		}
	}
	if (index<selectCount)
	{
		for (i=0;i<pExp->ItemCount;i++)
		{
			if (pExp->Items[i].bselect)
			{
				if (selectIndex==index)
				{
					PX_strcpy(path,PX_Object_EditGetText(pExp->edit_Path),PX_EXPLORER_MAX_PATH_LEN);
					if (path[PX_strlen(path)-1]!='/'&&path[PX_strlen(path)-1]!='\\')
					{
						PX_strcat(path,"/");
					}
					PX_strcat(path,pExp->Items[i].name);
				}
				selectIndex++;
			}
		}
	}
	else
	{
		path[0]='\0';
	}

}

px_void PX_Object_ExplorerOpen(PX_Object *Object)
{
	PX_ObjectSetFocus(Object);
	Object->Visible=PX_TRUE;
}

px_void PX_Object_ExplorerClose(PX_Object *Object)
{
	PX_ObjectClearFocus(Object);
	Object->Visible=PX_FALSE;
}
