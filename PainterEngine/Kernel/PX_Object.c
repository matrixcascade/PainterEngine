#include "PX_Object.h"


/*
PX_Object * PX_ObjectRootCreate(px_memorypool *mp)
{
	PX_Object *pObject=(PX_Object *)MP_Malloc(mp,sizeof(PX_Object));

	if (pObject==PX_NULL)
	{
		return PX_NULL;
	}
	pObject->x=0;
	pObject->y=0;

	pObject->Width=0;
	pObject->Height=0;
	pObject->Enabled=PX_TRUE;
	pObject->Visible=PX_TRUE;
	pObject->pChilds=PX_NULL;
	pObject->pObject=PX_NULL;
	pObject->pNextBrother=PX_NULL;
	pObject->pPreBrother=PX_NULL;
	pObject->Type=PX_OBJECT_TYPE_NULL;
	pObject->ReceiveEvents=PX_TRUE;

	pObject->pEventActions=PX_NULL;
	pObject->User_int=0;
	pObject->mp=mp;
	pObject->Func_ObjectUpdate=PX_NULL;
	pObject->Func_ObjectFree=PX_NULL;
	pObject->Func_ObjectRender=PX_NULL;


	return pObject;
}
*/

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
		px_memcpy(pObject->pObject,desc,size);
		pObject->Type=type;
		pObject->Func_ObjectFree=Func_ObjectFree;
		pObject->Func_ObjectRender=Func_ObjectRender;
		pObject->Func_ObjectUpdate=Func_ObjectUpdate;
	}
	return pObject;
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




px_void PX_Object_ObjectLinkerRender(px_surface *pSurface, PX_Object *Object,px_uint elpased)
{
	if (Object==PX_NULL)
	{
		return;
	}
	if (Object->Visible!=PX_FALSE)
	{
		if (Object->Func_ObjectRender!=0)
		{
			Object->Func_ObjectRender(pSurface,Object,elpased);
		}
		PX_Object_ObjectLinkerRender(pSurface,Object->pChilds,elpased);
	}
	PX_Object_ObjectLinkerRender(pSurface,Object->pNextBrother,elpased);	
}


px_void PX_ObjectRender(px_surface *pSurface, PX_Object *Object,px_uint elpased )
{
	if (Object==PX_NULL)
	{
		return;
	}
	if (Object->Visible==PX_FALSE)
	{
		return;
	}
	if (Object->Func_ObjectRender!=0)
	{
		Object->Func_ObjectRender(pSurface,Object,elpased);
	}
	if (Object->pChilds!=PX_NULL)
	{
		PX_Object_ObjectLinkerRender(pSurface,Object->pChilds,elpased);
	}
}



px_bool PX_ObjectIsPointInRegion( PX_Object *Object,px_float x,px_float y )
{
	return PX_isXYInRegion(x,y,Object->x,Object->y,Object->Width,Object->Height);
}

px_float PX_ObjectGetHeight(PX_Object *Object)
{
	return Object->Height;
}

px_float PX_ObjectGetWidth(PX_Object *Object)
{
	return Object->Width;
}

/*
px_void PX_ObjectRootInit(PX_Object *pObject,px_float x,px_float y,px_float z,px_float Width,px_float Height,px_float Lenght)
{
	pObject->pParent=PX_NULL;
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
	pObject->impact_type=0;
	pObject->pEventActions=PX_NULL;

	pObject->User_int=0;
	pObject->User_ptr=PX_NULL;
	pObject->Func_ObjectFree=PX_NULL;
	pObject->Func_ObjectRender=PX_NULL;
}

*/

px_void PX_ObjectInit(px_memorypool *mp,PX_Object *pObject,PX_Object *Parent,px_float x,px_float y,px_float z,px_float Width,px_float Height,px_float Lenght )
{
	PX_Object *pLinker;
	pObject->pParent=Parent;


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

	pObject->mp=mp;
	pObject->Func_ObjectFree=PX_NULL;
	pObject->Func_ObjectRender=PX_NULL;

	if (Parent!=PX_NULL)
	{
		if(Parent->pChilds==PX_NULL)
		{
			Parent->pChilds=pObject;
		}
		else
		{
			pLinker=Parent->pChilds;
			while (pLinker->pNextBrother)
			{
				pLinker=pLinker->pNextBrother;
			}
			pLinker->pNextBrother=pObject;
			pObject->pPreBrother=pLinker;
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

	EventAction=pPost->pEventActions;
	while(EventAction)
	{
		if (EventAction->EventAction==Event.Event)
		{
			EventAction->EventActionFunc(pPost,Event,EventAction->user_ptr);
		}
		EventAction=EventAction->pNext;
	}
}


px_void PX_ObjectPostEvent( PX_Object *pPost,PX_Object_Event Event )
{
	PX_OBJECT_EventAction *EventAction;
	
	if (pPost==PX_NULL)
	{
		return;
	}

	if (pPost->Visible==PX_FALSE||pPost->Enabled==PX_FALSE||pPost->ReceiveEvents==PX_FALSE)
	{
		PX_ObjectPostEvent(pPost->pNextBrother,Event);
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

	PX_ObjectPostEvent(pPost->pNextBrother,Event);
	PX_ObjectPostEvent(pPost->pChilds,Event);
}





PX_Object* PX_Object_LabelCreate(px_memorypool *mp,PX_Object *Parent,px_int x,px_int y,px_int Width,px_int Height,px_char *Text,px_color Color )
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

	TextLen=px_strlen(Text);
	
	pLable->Text=(px_char *)MP_Malloc(mp,TextLen+1);
	if (pLable->Text==PX_NULL)
	{
		MP_Free(pObject->mp,pObject);
		MP_Free(pObject->mp,pLable);
		return PX_NULL;
	}

	px_strcpy(pLable->Text,Text,TextLen+1);

	pLable->TextColor=Color;
	pLable->BackgroundColor=PX_COLOR(0,0,0,0);
	pLable->Border=PX_FALSE;
	pLable->Align=PX_OBJECT_ALIGN_HCENTER|PX_OBJECT_ALIGN_VCENTER;

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
px_void PX_Object_LabelSetText( PX_Object *pObject,px_char *Text )
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
	
	TextLen=px_strlen(Text);
	pLabel=(PX_Object_Label *)pObject->pObject;
	if(TextLen>px_strlen(pLabel->Text))
	{
	MP_Free(pObject->mp,pLabel->Text);
	pLabel->Text=(px_char *)MP_Malloc(pObject->mp,TextLen+1);
		if (!pLabel->Text)
		{
			PX_ERROR("Text create out of memories");
		}
	px_strcpy(pLabel->Text,Text,TextLen+1);
	}
	else
	{
		px_strcpy(pLabel->Text,Text,TextLen+1);
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
px_void PX_Object_LabelSetAlign( PX_Object *pObject,px_uchar Align )
{
	PX_Object_Label * pLabel=PX_Object_GetLabel(pObject);
	if (pLabel)
	{
		
		pLabel->Align=Align;
	}
}
px_void PX_Object_LabelSetBorder( PX_Object *pLabel,px_bool Border )
{
	PX_Object_Label *Label=PX_Object_GetLabel(pLabel);
	if (Label)
	{
		Label->Border=Border;
	}
}

px_void PX_Object_LabelRender(px_surface *psurface, PX_Object *pObject,px_uint elpased)
{
	px_int TextLen,i;
	px_int x,y;
	PX_Object_Label *pLabel=(PX_Object_Label *)pObject->pObject;
	if (pLabel==PX_NULL)
	{
		return;
	}

	if (!pObject->Visible)
	{
		return;
	}

	TextLen=0;
	for (i=0;i<px_strlen(pLabel->Text);i++)
	{
		if (pLabel->Text[i]&0x80)
		{
			TextLen+=__PX_FONT_GBKSIZE;
			i++;
			continue;
		}
		else
		{
			TextLen+=__PX_FONT_ASCSIZE;
		}
	}
	x=(px_int)pObject->x;
	y=(px_int)pObject->y;

	if (pLabel->Align&PX_OBJECT_ALIGN_HCENTER)
	{
		x=(px_int)pObject->x;
		x=x+(px_int)(pObject->Width-TextLen)/2;
	}

	if (pLabel->Align&PX_OBJECT_ALIGN_VCENTER)
	{
		y=(px_int)pObject->y;
		y=y+(px_int)(pObject->Height-__PX_FONT_HEIGHT)/2;
	}


	if (pLabel->Align&PX_OBJECT_ALIGN_LEFT)
	{
		x=(px_int)pObject->x;
	}

	if (pLabel->Align&PX_OBJECT_ALIGN_RIGHT)
	{
		x=(px_int)pObject->x+(px_int)pObject->Width-TextLen;
	}

	if (pLabel->Align&PX_OBJECT_ALIGN_TOP)
	{
		y=(px_int)pObject->y;
	}

	if (pLabel->Align&PX_OBJECT_ALIGN_BOTTOM)
	{
		y=(px_int)pObject->y+(px_int)pObject->Height-__PX_FONT_HEIGHT;
	}
	

	PX_SurfaceClear(psurface,(px_int)pObject->x,(px_int)pObject->y,(px_int)pObject->x+(px_int)pObject->Width-1,(px_int)pObject->y+(px_int)pObject->Height-1,pLabel->BackgroundColor);

	if (pLabel->Border)
	{
		PX_GeoDrawBorder(psurface,(px_int)pObject->x,(px_int)pObject->y,(px_int)pObject->x+(px_int)pObject->Width-1,(px_int)pObject->y+(px_int)pObject->Height-1,1,pLabel->TextColor);
	}

	PX_FontDrawText(psurface,x,y,pLabel->Text,pLabel->TextColor,PX_FONT_ALIGN_XLEFT);
}

px_void PX_Object_LabelFree( PX_Object *pLabel )
{
	PX_Object_Label *Label=PX_Object_GetLabel(pLabel);
	if (pLabel!=PX_NULL)
	{
		MP_Free(pLabel->mp,Label->Text);
	}
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
	pObject->Func_ObjectFree=PX_Object_ProcessBarFree;
	pObject->Func_ObjectRender=PX_Object_ProcessBarRender;

	ProcessBar->MAX=100;
	ProcessBar->Value=0;
	ProcessBar->Color=PX_COLOR(255,0,0,128);
	ProcessBar->BackgroundColor=PX_COLOR(0,0,0,0);
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

px_void PX_Object_ProcessBarRender(px_surface *psurface, PX_Object *pObject,px_uint elpased)
{
	px_int px,py,pl;//Processing start X,Processing Start Y &Processing length

	PX_Object_ProcessBar *pProcessBar=(PX_Object_ProcessBar *)pObject->pObject;

	px=(px_int)pObject->x+2; //increase line width
	py=(px_int)pObject->y+2;

	if (pProcessBar->Value<0)
	{
		pProcessBar->Value=0;
	}
	if (pProcessBar->Value>pProcessBar->MAX)
	{
		pProcessBar->Value=pProcessBar->MAX;
	}

	if (pObject->Width<=4)
	{
		return;
	}
	if(pProcessBar->Value<pProcessBar->MAX)
		pl=(px_int)((pObject->Width-4)*((pProcessBar->Value*1.0)/pProcessBar->MAX));
	else
		pl=(px_int)pObject->Width-2;
	//Draw BackGround
	PX_SurfaceClear(
		psurface,
		(px_int)pObject->x,
		(px_int)pObject->y,
		(px_int)pObject->x+(px_int)pObject->Width-1,
		(px_int)pObject->y+(px_int)pObject->Height-1,
		pProcessBar->BackgroundColor
		);
	//Draw border
	PX_GeoDrawBorder(
		psurface,
		(px_int)pObject->x,
		(px_int)pObject->y,
		(px_int)pObject->x+(px_int)pObject->Width-1,
		(px_int)pObject->y+(px_int)pObject->Height-1,
		1,
		pProcessBar->Color
		);

	PX_GeoDrawRect(psurface,px,py,px+pl-2-1,py+(px_int)pObject->Height-4-1,pProcessBar->Color);

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



PX_Object * PX_Object_RadiusICOCreate(px_memorypool *mp,PX_Object*Parent,px_int x,px_int y,px_int Radius )
{
	PX_Object *pObject;
	PX_Object_RadiusICO *RadiusICO=(PX_Object_RadiusICO *)MP_Malloc(mp,sizeof(PX_Object_RadiusICO));
	if (RadiusICO==PX_NULL)
	{
		return PX_NULL;
	}
	pObject=PX_ObjectCreate(mp,Parent,(px_float)(x-Radius),(px_float)(y-Radius),0,(px_float)(2*Radius),(px_float)(2*Radius),0);
	if (pObject==PX_NULL)
	{
		MP_Free(pObject->mp,RadiusICO);
		return PX_NULL;
	}

	pObject->pObject=RadiusICO;
	pObject->Enabled=PX_TRUE;
	pObject->Visible=PX_TRUE;
	pObject->Type=PX_OBJECT_TYPE_RADIUSICO;
	pObject->ReceiveEvents=PX_FALSE;
	pObject->Func_ObjectFree=PX_Object_RadiusICOFree;
	pObject->Func_ObjectRender=PX_Object_RadiusICORender;
	RadiusICO->Radius=Radius;
	RadiusICO->Color=PX_COLOR(255,0,255,0);
	return pObject;
}

PX_Object_RadiusICO * PX_Object_GetRadiusICO( PX_Object *Object )
{
	if(Object->Type==PX_OBJECT_TYPE_RADIUSICO)
		return (PX_Object_RadiusICO *)Object->pObject;
	else
		return PX_NULL;
}

px_void PX_Object_RadiusICOSetColor( PX_Object *RadiusICO,px_color Color )
{
	PX_Object_RadiusICO *pRadius=PX_Object_GetRadiusICO(RadiusICO);
	if (pRadius!=PX_NULL)
	{
		
		pRadius->Color=Color;
	}
}

px_void PX_Object_RadiusICORender(px_surface *psurface, PX_Object *RadiusICO,px_uint elpased)
{
	px_int x,y,r;
	px_color BorderColor;
	PX_Object_RadiusICO *pRadius=PX_Object_GetRadiusICO(RadiusICO);
	if (pRadius==PX_NULL)
	{
		return;
	}
	if(pRadius->Radius<=2)
	{
		return;
	}
	BorderColor=pRadius->Color;
	BorderColor._argb.a=32;
	r=pRadius->Radius;
	x=(px_int)RadiusICO->x+r;
	y=(px_int)RadiusICO->y+r;
	PX_GeoDrawSolidCircle(psurface,x,y,r,BorderColor);
	PX_GeoDrawSolidCircle(psurface,x,y,r-2,pRadius->Color);
}


PX_Object * PX_Object_ImageCreate(px_memorypool *mp,PX_Object *Parent,px_int x,px_int y,px_texture *ptex )
{
	PX_Object *pObject;
	PX_Object_Image *pImage=(PX_Object_Image *)MP_Malloc(mp,sizeof(PX_Object_Image));
	if (pImage==PX_NULL)
	{
		return PX_NULL;
	}
	pObject=PX_ObjectCreate(mp,Parent,(px_float)x,(px_float)y,0,0,0,0);
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
	pImage->Align=PX_OBJECT_ALIGN_HCENTER|PX_OBJECT_ALIGN_VCENTER;
	return pObject;
}


PX_Object_Image * PX_Object_GetImage( PX_Object *Object )
{
	if(Object->Type==PX_OBJECT_TYPE_IMAGE)
		return (PX_Object_Image *)Object->pObject;
	else
		return PX_NULL;
}

px_void PX_Object_ImageSetAlign( PX_Object *pImage,px_uchar Align)
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


px_void PX_Object_ImageRender(px_surface *psurface, PX_Object *im,px_uint elpased)
{
	px_int x;
	px_int y;
	PX_Object_Image *pImage=PX_Object_GetImage(im);
	x=(px_int)im->x;
	y=(px_int)im->y;

	if (pImage->Align&PX_OBJECT_ALIGN_BOTTOM)
	{
		y=(px_int)im->y+(px_int)im->Height-pImage->pTexture->height;
	}
	if (pImage->Align&PX_OBJECT_ALIGN_TOP)
	{
		y=(px_int)im->y;
	}
	if (pImage->Align&PX_OBJECT_ALIGN_LEFT)
	{
		x=(px_int)im->x;
	}
	if (pImage->Align&PX_OBJECT_ALIGN_RIGHT)
	{
		x=(px_int)im->x+(px_int)im->Width-pImage->pTexture->width;
	}

	if (pImage->Align&PX_OBJECT_ALIGN_HCENTER)
	{
		x=(px_int)im->x+((px_int)im->Width-pImage->pTexture->width)/2;
	}
	if (pImage->Align&PX_OBJECT_ALIGN_VCENTER)
	{
		y=(px_int)im->y+((px_int)im->Height-pImage->pTexture->height)/2;
	}

	if (pImage!=PX_NULL)
	{
		if(pImage->pmask)
			PX_TextureRenderMask(psurface,pImage->pmask,pImage->pTexture,x,y,PX_TEXTURERENDER_REFPOINT_LEFTTOP,PX_NULL);
		else
			PX_TextureRender(psurface,pImage->pTexture,x,y,PX_TEXTURERENDER_REFPOINT_LEFTTOP,PX_NULL);
	}
}

px_void PX_Object_SliderBarOnMouseLButtonDown(PX_Object *pObject,PX_Object_Event e,px_void *user_ptr)
{
	PX_Object_SliderBar *pSliderBar=PX_Object_GetSliderBar(pObject);
	px_int x,y;
	px_int SliderBtnLen,Sx,Sy;
	px_rect rect;
	x=e.Param_int[0];
	y=e.Param_int[1];


	if (pSliderBar)
	{
		
		if(PX_ObjectIsPointInRegion(pObject,(px_float)x,(px_float)y))
		{
			if (pSliderBar==PX_NULL)
			{
				return;
			}

			if (pObject->Width<=10)
			{
				return;
			}

			if (pObject->Height<10)
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
						Sx=(px_int)pObject->x+(px_int)((pObject->Width-4-pSliderBar->SliderButtonLength)*(1.0f)*pSliderBar->Value/pSliderBar->Max);
						

						rect.x=(px_float)Sx;
						rect.y=(px_float)pObject->y;
						rect.width=(px_float)pSliderBar->SliderButtonLength;

						rect.height=(px_float)pObject->Height;
					    
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
						Sx=(px_int)pObject->x+(px_int)((pObject->Width-4-pSliderBar->SliderButtonLength)*(1.0f)*pSliderBar->Value/pSliderBar->Value);
					
						rect.x=(px_float)Sx;
						rect.y=(px_float)pObject->y+2;
						rect.width=(px_float)pSliderBar->SliderButtonLength;
						rect.height=(px_float)pObject->Height/2-4;

						if (PX_isPointInRect(PX_POINT((px_float)x,(px_float)y,0),rect))
						{
							pSliderBar->btnDownX=x;
							pSliderBar->btnDownY=y;
							pSliderBar->status=PX_OBJECT_SLIDERBAR_STATUS_ONDRAG;
						}
						else if(x>rect.x)
						{
							pSliderBar->Value+=pSliderBar->Max*pSliderBar->SliderButtonLength/((px_int)pObject->Width-4);
							if (pSliderBar->Value>pSliderBar->Max)
							{
								pSliderBar->Value=pSliderBar->Max;
							}
						}
						else
						{
							pSliderBar->Value-=pSliderBar->Max*pSliderBar->SliderButtonLength/((px_int)pObject->Width-4);
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
						Sx=(px_int)pObject->x;
						Sy=(px_int)pObject->y+((px_int)pObject->Height-pSliderBar->SliderButtonLength)*pSliderBar->Value/pSliderBar->Max;

						rect.x=(px_float)Sx;
						rect.y=(px_float)Sy;
						rect.width=(px_float)pObject->Height;
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
						Sy=(px_int)pObject->y+(px_int)((pObject->Height-4-pSliderBar->SliderButtonLength)*(1.0f)*pSliderBar->Value/pSliderBar->Max);
	

						rect.x=(px_float)pObject->x+2;
						rect.y=(px_float)Sy;

						rect.width=(px_float)pObject->Width-4;
						rect.height=(px_float)pSliderBar->SliderButtonLength;

						if (PX_isPointInRect(PX_POINT((px_float)x,(px_float)y,0),rect))
						{
							pSliderBar->btnDownX=x;
							pSliderBar->btnDownY=y;
							pSliderBar->status=PX_OBJECT_SLIDERBAR_STATUS_ONDRAG;
						}
						else if(y>rect.y)
						{
							pSliderBar->Value+=pSliderBar->Max*pSliderBar->SliderButtonLength/((px_int)pObject->Height-4);
							if (pSliderBar->Value>pSliderBar->Max)
							{
								pSliderBar->Value=pSliderBar->Max;
							}
						}
						else
						{
							pSliderBar->Value-=pSliderBar->Max*pSliderBar->SliderButtonLength/((px_int)pObject->Height-4);
							if (pSliderBar->Value<0)
							{
								pSliderBar->Value=0;
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
	px_int x,y;

	x=e.Param_int[0];
	y=e.Param_int[1];

	if (pSliderBar)
	{
		if (pSliderBar->status!=PX_OBJECT_SLIDERBAR_STATUS_ONDRAG)
		{
			return;
		}

		if(pSliderBar->Type==PX_OBJECT_SLIDERBAR_TYPE_HORIZONTAL)
		{
			pSliderBar->DargButtonX+=(x-pSliderBar->btnDownX);
			pSliderBar->btnDownX=x;
			if (pSliderBar->DargButtonX<0)
			{
				pSliderBar->DargButtonX=0;
			}
			if (pSliderBar->DargButtonX>=pObject->Width-pSliderBar->SliderButtonLength-2)
			{
				pSliderBar->DargButtonX=(px_int)pObject->Width-pSliderBar->SliderButtonLength-2;
			}

		}

		if (pSliderBar->Type==PX_OBJECT_SLIDERBAR_TYPE_VERTICAL)
		{
			
			pSliderBar->DargButtonY+=(y-pSliderBar->btnDownY);
			pSliderBar->btnDownY=y;

			if (pSliderBar->DargButtonY<0)
			{
				pSliderBar->DargButtonY=0;
			}
			if (pSliderBar->DargButtonY>=pObject->Height-pSliderBar->SliderButtonLength-2)
			{
				pSliderBar->DargButtonY=(px_int)pObject->Height-pSliderBar->SliderButtonLength-2;
			}
		}
	
	}
}


PX_Object * PX_Object_SliderBarCreate(px_memorypool *mp,PX_Object *Parent,px_int x,px_int y,px_int Width,px_int Height,enum PX_OBJECT_SLIDERBAR_TYPE Type,enum PX_OBJECT_SLIDERBAR_STYLE style)
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
	pSliderbar->SliderButtonLength=10;
	pSliderbar->style=style;
	pSliderbar->status=PX_OBJECT_SLIDERBAR_STATUS_NORMAL;
	pSliderbar->Type=Type;
	pSliderbar->color=PX_COLOR(255,80,80,80);
	pSliderbar->BackgroundColor=PX_COLOR(0,0,0,0);
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
		
		SliderBar->Value=Value;
	}
}

px_void PX_Object_SliderBarSetMax( PX_Object *pSliderBar,px_int Max )
{
	PX_Object_SliderBar *SliderBar=PX_Object_GetSliderBar(pSliderBar);
	if (SliderBar!=PX_NULL)
	{
		
		SliderBar->Max=Max;
	}
}

px_int PX_Object_SliderBarGetValue( PX_Object *pSliderBar )
{
	PX_Object_SliderBar *SliderBar=PX_Object_GetSliderBar(pSliderBar);
	if (SliderBar!=PX_NULL)
	{
		return SliderBar->Value;
	}
	return 0;
}

px_void PX_Object_SliderBarRender(px_surface *psurface, PX_Object *pObject,px_uint elpased)
{
	PX_Object_SliderBar *pSliderBar=PX_Object_GetSliderBar(pObject);
	PX_Object_Event e;
	px_int SliderBtnLen,Sx,Sy;
	if (pSliderBar==PX_NULL)
	{
		return;
	}

	if (pObject->Width<=10)
	{
		return;
	}

	if (pObject->Height<10)
	{
		return;
	}

	SliderBtnLen=pSliderBar->SliderButtonLength;

	if (pSliderBar->Value>=pSliderBar->Max)
	{
		pSliderBar->Value=pSliderBar->Max;
	}

	if (pSliderBar->Value<0)
	{
		pSliderBar->Value=0;
	}

	//Clear
	PX_SurfaceClear(
		psurface,
		(px_int)pObject->x,
		(px_int)pObject->y,
		(px_int)pObject->x+(px_int)pObject->Width-1,
		(px_int)pObject->y+(px_int)pObject->Height-1,
		pSliderBar->BackgroundColor
		);

	switch(pSliderBar->status)
	{
	case PX_OBJECT_SLIDERBAR_STATUS_NORMAL:
		{
			if (pSliderBar->Value>pSliderBar->Max)
			{
				pSliderBar->Value=pSliderBar->Max;
			}
			if (pSliderBar->Value<0)
			{
				pSliderBar->Value=0;
			}

			if(pSliderBar->Type==PX_OBJECT_SLIDERBAR_TYPE_HORIZONTAL)
			{
				switch(pSliderBar->style)
				{
				case PX_OBJECT_SLIDERBAR_STYLE_LINER:
					{
						Sx=((px_int)pObject->Width-pSliderBar->SliderButtonLength)*pSliderBar->Value/pSliderBar->Max;
						pSliderBar->DargButtonX=Sx;
						Sx+=(px_int)pObject->x;
						Sy=(px_int)pObject->y;
						//Draw Line
						PX_GeoDrawRect(
							psurface,
							(px_int)pObject->x,
							(px_int)pObject->y+(px_int)pObject->Height/2,
							(px_int)pObject->x+(px_int)pObject->Width-1,
							(px_int)pObject->y+(px_int)pObject->Height/2,
							pSliderBar->color
							);
						//Draw Slider button
						PX_GeoDrawRect(psurface,(px_int)Sx,(px_int)Sy,Sx+SliderBtnLen-1,Sy+(px_int)pObject->Height-1,pSliderBar->color);
					}
					break;
				case PX_OBJECT_SLIDERBAR_STYLE_BOX:
					{
						//draw border
						PX_GeoDrawBorder(psurface,(px_int)pObject->x,(px_int)pObject->y,(px_int)pObject->x+(px_int)pObject->Width-1,(px_int)pObject->y+(px_int)pObject->Height-1,1,pSliderBar->color);
						Sx=(px_int)((pObject->Width-4-pSliderBar->SliderButtonLength)*(1.0f)*pSliderBar->Value/pSliderBar->Max);
						pSliderBar->DargButtonX=Sx;
						Sx+=SliderBtnLen/2;
						//draw slider bar
						PX_GeoDrawRect(psurface,(px_int)pObject->x+Sx-SliderBtnLen/2+2,(px_int)pObject->y+2,(px_int)pObject->x+Sx-SliderBtnLen/2+SliderBtnLen-1,(px_int)pObject->y+(px_int)pObject->Height-2,pSliderBar->color);
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
						Sx=(px_int)pObject->x;
						Sy=(px_int)(pObject->Height-pSliderBar->SliderButtonLength)*pSliderBar->Value/pSliderBar->Max;
						pSliderBar->DargButtonY=Sy;
						Sy+=(px_int)pObject->y;
						//Draw Line
						PX_GeoDrawRect(
							psurface,
							(px_int)pObject->x+(px_int)pObject->Width/2,
							(px_int)pObject->y,
							(px_int)pObject->x+(px_int)pObject->Width/2,
							(px_int)pObject->y+(px_int)pObject->Height-1,
							pSliderBar->color
							);
						//Draw Slider button
						PX_GeoDrawRect(psurface,Sx,Sy,Sx+(px_int)pObject->Width-1,Sy+SliderBtnLen-1,pSliderBar->color);
					}
					break;
				case PX_OBJECT_SLIDERBAR_STYLE_BOX:
					{
						//draw border
						PX_GeoDrawBorder(psurface,(px_int)pObject->x,(px_int)pObject->y,(px_int)pObject->x+(px_int)pObject->Width-1,(px_int)pObject->y+(px_int)pObject->Height-1,1,pSliderBar->color);
						Sy=(px_int)((pObject->Height-4-pSliderBar->SliderButtonLength)*(1.0f)*pSliderBar->Value/pSliderBar->Max);
						pSliderBar->DargButtonY=Sy;
						Sy+=SliderBtnLen/2;
						//draw slider bar
						PX_GeoDrawRect(psurface,(px_int)pObject->x+2,(px_int)pObject->y+Sy-SliderBtnLen/2+2,(px_int)pObject->x+(px_int)pObject->Width-1-2,(px_int)pObject->y+Sy+SliderBtnLen/2-1+2,pSliderBar->color);
					}
					break;
				}

			}
		}
		break;
	case PX_OBJECT_SLIDERBAR_STATUS_ONDRAG:
		{
			if(pSliderBar->Type==PX_OBJECT_SLIDERBAR_TYPE_HORIZONTAL)
			{
				switch(pSliderBar->style)
				{
				case PX_OBJECT_SLIDERBAR_STYLE_LINER:
					{
						Sx=pSliderBar->DargButtonX;
						Sy=(px_int)pObject->y;

						pSliderBar->Value=pSliderBar->Max*(Sx)/((px_int)pObject->Width-pSliderBar->SliderButtonLength);

						//Draw Line
						PX_GeoDrawRect(
							psurface,
							(px_int)pObject->x,
							(px_int)pObject->y+(px_int)pObject->Height/2,
							(px_int)pObject->x+(px_int)pObject->Width-1,
							(px_int)pObject->y+(px_int)pObject->Height/2,
							pSliderBar->color
							);
						//Draw Slider button
						PX_GeoDrawRect(psurface,Sx,Sy,Sx+SliderBtnLen-1,Sy+(px_int)pObject->Height-1,pSliderBar->color);
					}
					break;
				case PX_OBJECT_SLIDERBAR_STYLE_BOX:
					{
						//draw border
						PX_GeoDrawBorder(psurface,(px_int)pObject->x,(px_int)pObject->y,(px_int)pObject->x+(px_int)pObject->Width-1,(px_int)pObject->y+(px_int)pObject->Height-1,1,pSliderBar->color);

						Sx=pSliderBar->DargButtonX;

						pSliderBar->Value=pSliderBar->Max*(Sx)/((px_int)pObject->Width-pSliderBar->SliderButtonLength-4);
						if (pSliderBar->Value>pSliderBar->Max)
						{
							pSliderBar->Value=pSliderBar->Max;
						}
						if (pSliderBar->Value<0)
						{
							pSliderBar->Value=0;
						}
						Sx+=SliderBtnLen/2;
						//draw slider bar
						PX_GeoDrawRect(psurface,(px_int)pObject->x+Sx-SliderBtnLen/2+2,(px_int)pObject->y+2,(px_int)pObject->x+Sx-SliderBtnLen/2+SliderBtnLen-1,(px_int)pObject->y+(px_int)pObject->Height-2,pSliderBar->color);
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
						pSliderBar->Value=pSliderBar->Max*(pSliderBar->DargButtonY)/((px_int)pObject->Height-pSliderBar->SliderButtonLength);
						if (pSliderBar->Value>pSliderBar->Max)
						{
							pSliderBar->Value=pSliderBar->Max;
						}
						if (pSliderBar->Value<0)
						{
							pSliderBar->Value=0;
						}

						Sy=pSliderBar->DargButtonY;
						Sx=(px_int)pObject->x;

						

						//Draw Line
						PX_GeoDrawRect(
							psurface,
							(px_int)pObject->x+(px_int)pObject->Width/2,
							(px_int)pObject->y,
							(px_int)pObject->x+(px_int)pObject->Width/2,
							(px_int)pObject->y+(px_int)pObject->Height-1,
							pSliderBar->color
							);
						//Draw Slider button
						PX_GeoDrawRect(psurface,Sx,Sy,Sx+(px_int)pObject->Width-1,Sy+SliderBtnLen-1,pSliderBar->color);
					}
					break;
				case PX_OBJECT_SLIDERBAR_STYLE_BOX:
					{
						//draw border
						PX_GeoDrawBorder(psurface,(px_int)pObject->x,(px_int)pObject->y,(px_int)pObject->x+(px_int)pObject->Width-1,(px_int)pObject->y+(px_int)pObject->Height-1,1,pSliderBar->color);
						
						pSliderBar->Value=pSliderBar->Max*(pSliderBar->DargButtonY-2)/((px_int)pObject->Height-pSliderBar->SliderButtonLength-4);
						
						Sy=pSliderBar->DargButtonY;
						Sx=(px_int)pObject->x;
						Sy+=SliderBtnLen/2;
						


						//draw slider bar
						PX_GeoDrawRect(psurface,(px_int)pObject->x+2,(px_int)pObject->y+Sy-SliderBtnLen/2+2,(px_int)pObject->x+(px_int)pObject->Width-1-2,(px_int)pObject->y+Sy+SliderBtnLen/2-1+2,pSliderBar->color);
					}
					break;
				}

			}
		}
		break;
	}

	if (pSliderBar->lastValue!=pSliderBar->Value)
	{
		e.Event=PX_OBJECT_EVENT_VALUECHAGE;
		e.Param_uint[0]=pSliderBar->Value;
		e.Param_uint[1]=pSliderBar->Value;
		e.Param_uint[2]=pSliderBar->Value;
		e.Param_uint[3]=pSliderBar->Value;
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

px_void PX_Object_ProcessBarFree(PX_Object *pProcessBar)
{

}

px_void PX_Object_RadiusICOFree( PX_Object *pRadiusICO )
{

}

px_void PX_Object_ImageFree( PX_Object *pBitmap )
{

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
	px_int x,y;
	x=e.Param_int[0];
	y=e.Param_int[1];

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
					e.param_ptr[0]=PX_NULL;
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
	px_int x,y;

	x=e.Param_int[0];
	y=e.Param_int[1];

	if (pPushButton)
	{
		if(PX_ObjectIsPointInRegion(Object,(px_float)x,(px_float)y))
			pPushButton->state=PX_OBJECT_BUTTON_STATE_ONPUSH;
	}
}

px_void PX_Object_PushButtonOnMouseLButtonUp(PX_Object *Object,PX_Object_Event e,px_void *user_ptr)
{
	PX_Object_PushButton *pPushButton=PX_Object_GetPushButton(Object);

	px_int x,y;

	x=e.Param_uint[0];
	y=e.Param_uint[1];

	if (pPushButton)
	{
		if(PX_ObjectIsPointInRegion(Object,(px_float)x,(px_float)y))
			if(pPushButton->state==PX_OBJECT_BUTTON_STATE_ONPUSH)
			{
				pPushButton->state=PX_OBJECT_BUTTON_STATE_ONCURSOR;
				e.Event=PX_OBJECT_EVENT_CURSORCLICK;
				PX_ObjectExecuteEvent(Object,e);
			}
	}
}



PX_Object * PX_Object_PushButtonCreate(px_memorypool *mp,PX_Object *Parent,px_int x,px_int y,px_int Width,px_int Height,px_char *Text,px_color Color)
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

	TextLen=px_strlen(Text)+1;

	pPushButton->Text=(px_char *)MP_Malloc(mp,TextLen);
	
	if (pPushButton->Text==PX_NULL)
	{
		MP_Free(pObject->mp,pObject);
		MP_Free(pObject->mp,pPushButton);
		return PX_NULL;
	}

	px_strcpy(pPushButton->Text,Text,TextLen);

	pPushButton->TextColor=Color;
	pPushButton->CursorColor=PX_COLOR(255,192,192,192);
	pPushButton->PushColor=PX_COLOR(255,128,128,128);
	pPushButton->BackgroundColor=PX_COLOR(255,255,255,255);
	pPushButton->BorderColor=PX_COLOR(255,0,0,0);
	pPushButton->Border=PX_TRUE;
	pPushButton->Align=PX_OBJECT_ALIGN_HCENTER|PX_OBJECT_ALIGN_VCENTER;
	pPushButton->state=PX_OBJECT_BUTTON_STATE_NORMAL;
	pPushButton->Border=PX_TRUE;
	pPushButton->style=PX_OBJECT_PUSHBUTTON_STYLE_RECT;
	pPushButton->roundradius=PX_OBJECT_PUSHBUTTON_ROUNDRADIUS;


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

px_void PX_Object_PushButtonSetText( PX_Object *pObject,px_char *Text )
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
	
	TextLen=px_strlen(Text);
	pPushButton=(PX_Object_PushButton *)pObject->pObject;

	if (TextLen>px_strlen(pPushButton->Text))
	{
		MP_Free(pObject->mp,pPushButton->Text);

		pPushButton->Text=(px_char *)MP_Malloc(pObject->mp,TextLen+1);
		if (!pPushButton->Text)
		{
			return;
		}
	}
	px_strcpy(pPushButton->Text,Text,TextLen+1);
	
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



px_void PX_Object_PushButtonSetImage(PX_Object *pObject,px_texture *texture)
{
	PX_Object_PushButton * pPushButton=PX_Object_GetPushButton(pObject);
	if (pPushButton)
	{
		pPushButton->Texture=texture;
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

px_void PX_Object_PushButtonSetAlign( PX_Object *pObject,px_uchar Align )
{
	PX_Object_PushButton * pPushButton=PX_Object_GetPushButton(pObject);
	if (pPushButton)
	{
		
		pPushButton->Align=Align;
	}
}
px_void PX_Object_PushButtonSetBorder( PX_Object *Object,px_uchar Border )
{
	PX_Object_PushButton *PushButton=PX_Object_GetPushButton(Object);
	if (PushButton)
	{
		
		PushButton->Border=Border;
	}
}

px_void PX_Object_PushButtonRender(px_surface *psurface, PX_Object *pObject,px_uint elpased)
{
	px_int TextLen,i;
	px_int fx,fy;
	PX_Object_PushButton *pPushButton=PX_Object_GetPushButton(pObject);

	if (pPushButton==PX_NULL)
	{
		return;
	}

	if (!pObject->Visible)
	{
		return;
	}

	TextLen=0;
	for (i=0;i<px_strlen(pPushButton->Text);i++)
	{
		if (pPushButton->Text[i]&0x80)
		{
			TextLen+=__PX_FONT_GBKSIZE;
			i++;
			continue;
		}
		else
		{
			TextLen+=__PX_FONT_ASCSIZE;
		}
	}

	if (pPushButton->Align&PX_OBJECT_ALIGN_HCENTER)
	{
		fx=(px_int)pObject->x;
		fx=fx+((px_int)pObject->Width-TextLen)/2;
	}

	if (pPushButton->Align&PX_OBJECT_ALIGN_VCENTER)
	{
		fy=(px_int)pObject->y;
		fy=fy+((px_int)pObject->Height-__PX_FONT_HEIGHT)/2;
	}


	if (pPushButton->Align&PX_OBJECT_ALIGN_LEFT)
	{
		fx=(px_int)pObject->x;
	}

	if (pPushButton->Align&PX_OBJECT_ALIGN_RIGHT)
	{
		fx=(px_int)pObject->x+(px_int)pObject->Width-TextLen;
	}

	if (pPushButton->Align&PX_OBJECT_ALIGN_TOP)
	{
		fy=(px_int)pObject->y;
	}

	if (pPushButton->Align&PX_OBJECT_ALIGN_BOTTOM)
	{
		fy=(px_int)pObject->y+(px_int)pObject->Height-__PX_FONT_HEIGHT;
	}

	switch(pPushButton->style)
	{
	case PX_OBJECT_PUSHBUTTON_STYLE_RECT:
		PX_SurfaceClear(psurface,(px_int)pObject->x,(px_int)pObject->y,(px_int)pObject->x+(px_int)pObject->Width-1,(px_int)pObject->y+(px_int)pObject->Height-1,pPushButton->BackgroundColor);
		switch (pPushButton->state)
		{
		case PX_OBJECT_BUTTON_STATE_NORMAL:
			PX_GeoDrawRect(psurface,(px_int)pObject->x,(px_int)pObject->y,(px_int)pObject->x+(px_int)pObject->Width-1,(px_int)pObject->y+(px_int)pObject->Height-1,pPushButton->BackgroundColor);
			break;
		case PX_OBJECT_BUTTON_STATE_ONPUSH:
			PX_GeoDrawRect(psurface,(px_int)pObject->x,(px_int)pObject->y,(px_int)pObject->x+(px_int)pObject->Width-1,(px_int)pObject->y+(px_int)pObject->Height-1,pPushButton->PushColor);
			break;
		case PX_OBJECT_BUTTON_STATE_ONCURSOR:
			PX_GeoDrawRect(psurface,(px_int)pObject->x,(px_int)pObject->y,(px_int)pObject->x+(px_int)pObject->Width-1,(px_int)pObject->y+(px_int)pObject->Height-1,pPushButton->CursorColor);
			break;
		}
		break;
	case PX_OBJECT_PUSHBUTTON_STYLE_ROUNDRECT:
		PX_GeoDrawSolidRoundRect(psurface,(px_int)pObject->x,(px_int)pObject->y,(px_int)pObject->x+(px_int)pObject->Width-1,(px_int)pObject->y+(px_int)pObject->Height-1,pPushButton->roundradius,pPushButton->BackgroundColor);
		switch (pPushButton->state)
		{
		case PX_OBJECT_BUTTON_STATE_NORMAL:
			PX_GeoDrawSolidRoundRect(psurface,(px_int)pObject->x,(px_int)pObject->y,(px_int)pObject->x+(px_int)pObject->Width-1,(px_int)pObject->y+(px_int)pObject->Height-1,pPushButton->roundradius,pPushButton->BackgroundColor);
			break;
		case PX_OBJECT_BUTTON_STATE_ONPUSH:
			PX_GeoDrawSolidRoundRect(psurface,(px_int)pObject->x,(px_int)pObject->y,(px_int)pObject->x+(px_int)pObject->Width-1,(px_int)pObject->y+(px_int)pObject->Height-1,pPushButton->roundradius,pPushButton->PushColor);
			break;
		case PX_OBJECT_BUTTON_STATE_ONCURSOR:
			PX_GeoDrawSolidRoundRect(psurface,(px_int)pObject->x,(px_int)pObject->y,(px_int)pObject->x+(px_int)pObject->Width-1,(px_int)pObject->y+(px_int)pObject->Height-1,pPushButton->roundradius,pPushButton->CursorColor);
			break;
		}
		break;
	}
	

	
	
	if (pPushButton->Texture)
	{
		PX_TextureRender(psurface,pPushButton->Texture,(px_int)pObject->x+(px_int)pObject->Width/2,(px_int)pObject->y+(px_int)pObject->Height/2,PX_TEXTURERENDER_REFPOINT_CENTER,PX_NULL);
	}

	if (pPushButton->Border)
	{
		switch(pPushButton->style)
		{
		case PX_OBJECT_PUSHBUTTON_STYLE_RECT:
			PX_GeoDrawBorder(psurface,(px_int)pObject->x,(px_int)pObject->y,(px_int)pObject->x+(px_int)pObject->Width-1,(px_int)pObject->y+(px_int)pObject->Height-1,1,pPushButton->BorderColor);
			break;
		case PX_OBJECT_PUSHBUTTON_STYLE_ROUNDRECT:
			PX_GeoDrawRoundRect(psurface,(px_int)pObject->x,(px_int)pObject->y,(px_int)pObject->x+(px_int)pObject->Width-1,(px_int)pObject->y+(px_int)pObject->Height-1,pPushButton->roundradius,0.6f,pPushButton->BorderColor);
			break;
		}
	}


	PX_FontDrawText(psurface,fx,fy,pPushButton->Text,pPushButton->TextColor,PX_FONT_ALIGN_XLEFT);

}

px_void PX_Object_PushButtonFree( PX_Object *Obj )
{
	PX_Object_PushButton *PushButton=PX_Object_GetPushButton(Obj);
	if (Obj!=PX_NULL)
	{
		MP_Free(Obj->mp,PushButton->Text);
	}
}

px_void PX_Object_RotationFree(PX_Object *Obj)
{
}


px_void PX_Object_RotationRender(px_surface *psurface, PX_Object *Obj,px_uint elpased)
{
	PX_Object_Rotation *pRot=PX_Object_GetRotation(Obj);
	if (pRot)
	{
		if(!pRot->bstop)
			pRot->angle+=(pRot->angle_per_second*(elpased/1000.0f));
		PX_TextureRenderRotation(psurface,pRot->pTexture,(px_int)Obj->x,(px_int)Obj->y,PX_TEXTURERENDER_REFPOINT_CENTER,PX_NULL,(px_int)pRot->angle);
	}
	
}



PX_Object * PX_Object_RotationCreate(px_memorypool *mp,PX_Object *Parent,px_int angle_per_second,px_int x,px_int y,px_texture *ptexture)
{
	PX_Object *pObject;
	PX_Object_Rotation *pRotation=(PX_Object_Rotation *)MP_Malloc(mp,sizeof(PX_Object_Rotation));
	if (pRotation==PX_NULL)
	{
		return PX_NULL;
	}
	pObject=PX_ObjectCreate(mp,Parent,(px_float)x,(px_float)y,0,0,0,0);
	if (pObject==PX_NULL)
	{
		MP_Free(pObject->mp,pRotation);
		return PX_NULL;
	}
	

	pRotation->angle=0;
	pRotation->angle_per_second=angle_per_second;

	pObject->pObject=pRotation;
	pObject->Enabled=PX_TRUE;
	pObject->Visible=PX_TRUE;
	pObject->Type=PX_OBJECT_TYPE_ROTATION;
	pObject->ReceiveEvents=PX_FALSE;
	pObject->Func_ObjectFree=PX_Object_RotationFree;
	pObject->Func_ObjectRender=PX_Object_RotationRender;
	pRotation->pTexture=ptexture;
	return pObject;
}

PX_Object_Rotation * PX_Object_GetRotation(PX_Object *Object)
{
	if (Object->Type==PX_OBJECT_TYPE_ROTATION)
	{
		return (PX_Object_Rotation *)Object->pObject;
	}
	return PX_NULL;
}

px_void PX_Object_RotationSetSpeed(PX_Object *rot,px_int Angle_per_second)
{
	PX_Object_Rotation *pRot;
	pRot=PX_Object_GetRotation(rot);
	if (pRot)
	{
		pRot->angle_per_second=Angle_per_second;
	}
}

px_void PX_Object_RotationStop(PX_Object *rot,px_bool bstop)
{
	PX_Object_Rotation *pRot;
	pRot=PX_Object_GetRotation(rot);
	if (pRot)
	{
		pRot->bstop=bstop;
	}
}

px_void PX_Object_EditOnMouseMove(PX_Object *Object,PX_Object_Event e,px_void *user_ptr)
{
	PX_Object_Edit *pEdit=PX_Object_GetEdit(Object);
	px_int x,y;
	x=e.Param_int[0];
	y=e.Param_int[1];

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
	px_int x,y;

	x=e.Param_int[0];
	y=e.Param_int[1];

	if (pEdit)
	{
		if(PX_ObjectIsPointInRegion(Object,(px_float)x,(px_float)y))
		{
			pEdit->onFocus=PX_TRUE;
			PX_Object_EditUpdateCursorOnDown(Object,x-(px_int)Object->x,y-(px_int)Object->y);
		}
		else
			pEdit->onFocus=PX_FALSE;
	}
}

px_void PX_Object_EditOnKeyboardString(PX_Object *Object,PX_Object_Event e,px_void *user_ptr)
{
	PX_Object_Edit *pEdit=PX_Object_GetEdit(Object);

	if (pEdit->onFocus)
	{
		PX_Object_EditAddString(Object,(px_char *)e.param_ptr[0]);
	}
}

static px_void PX_Object_EditCheckCursor(PX_Object_Edit*pedit)
{
	if (pedit->cursor_index<0)
	{
		pedit->cursor_index=0;
	}
	if (pedit->cursor_index>px_strlen(pedit->text.buffer))
	{
		pedit->cursor_index=px_strlen(pedit->text.buffer);
	}
}

PX_Object* PX_Object_EditCreate(px_memorypool *mp, PX_Object *Parent,px_int x,px_int y,px_int Width,px_int Height,px_color TextColor )
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



	PX_StringInit(mp,&pEdit->text);


	pEdit->TextColor=TextColor;

	pEdit->CursorColor=PX_COLOR(255,0,0,0);
	pEdit->BorderColor=PX_COLOR(255,0,0,0);
	pEdit->BackgroundColor=PX_COLOR(0,0,0,0);
	pEdit->XOffset=0;
	pEdit->YOffset=0;
	pEdit->Border=PX_TRUE;
	pEdit->Password=PX_FALSE;
	pEdit->onFocus=0;
	pEdit->state=PX_OBJECT_EDIT_STATE_NORMAL;
	pEdit->AutoNewline=PX_FALSE;
	pEdit->xSpacing=0;
	pEdit->ySpacing=0;
	pEdit->HorizontalOffset=3;
	pEdit->VerticalOffset=3;
	pEdit->cursor_index=0;
	pEdit->max_length=-1;

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


px_void PX_Object_EditSetText( PX_Object *pObject,px_char *Text )
{
	PX_Object_Edit *pEdit=PX_Object_GetEdit(pObject);
	if(pEdit)
	{
		PX_StringClear(&pEdit->text);
		PX_StringCat(&pEdit->text,Text);
		pEdit->cursor_index=px_strlen(Text);
	}
}

px_void PX_Object_EditSetFocus(PX_Object *pObject)
{
	PX_Object_Edit *pEdit=PX_Object_GetEdit(pObject);
	if(pEdit)
	{
		pEdit->onFocus=PX_TRUE;
	}
}


px_void PX_Object_EditSetPasswordStyle( PX_Object *pLabel,px_uchar Enabled )
{
	PX_Object_Edit *pEdit=PX_Object_GetEdit(pLabel);
	if (pEdit)
	{
		pEdit->Password=Enabled;
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



px_void PX_Object_EditSetBorder( PX_Object *pObj,px_uchar Border )
{
	PX_Object_Edit *pEdit=PX_Object_GetEdit(pObj);
	if (pEdit)
	{
		
		pEdit->Border=Border;
	}
}

px_void PX_Object_EditUpdateCursorOnDown(PX_Object *pObject,px_int cx,px_int cy)
{
	px_int charIndex;
	px_int textLen,x,y;
	PX_Object_Edit *pEdit=(PX_Object_Edit *)pObject->pObject;
	if (pEdit==PX_NULL)
	{
		return;
	}

	x=pEdit->HorizontalOffset;
	y=pEdit->VerticalOffset;
	cx+=pEdit->XOffset;
	cy+=pEdit->YOffset;

	textLen=px_strlen(pEdit->text.buffer);

	for (charIndex=0;charIndex<textLen;charIndex++)
	{

		if (pEdit->text.buffer[charIndex]&0x80)
		{
			if (cx>=x&&cx<=x+__PX_FONT_GBKSIZE+pEdit->xSpacing)
			{
				if (cx<=x+(__PX_FONT_GBKSIZE+pEdit->xSpacing)/2)
				{
					if (cy>=y&&cy<=y+__PX_FONT_HEIGHT+pEdit->YOffset)
					{
						pEdit->cursor_index=charIndex;
						return;
					}
				}
				else
				{
					if (cy>=y&&cy<=y+__PX_FONT_HEIGHT+pEdit->YOffset)
					{
						if (pEdit->text.buffer[charIndex+1])
						{
							pEdit->cursor_index=charIndex+2;
							return;
						}
						return;
					}
				}
			}
		}
		else
		{
			if (cx>=x&&cx<=x+__PX_FONT_ASCSIZE+pEdit->xSpacing)
			{
				if (cx<=x+(__PX_FONT_ASCSIZE+pEdit->xSpacing)/2)
				{
					if (cy>=y&&cy<=y+__PX_FONT_HEIGHT+pEdit->YOffset)
					{
						pEdit->cursor_index=charIndex;
						return;
					}
				}
				else
				{
					if (cy>=y&&cy<=y+__PX_FONT_HEIGHT+pEdit->YOffset)
					{
						pEdit->cursor_index=charIndex+1;
						return;
					}
				}
			}
		}

		if(pEdit->text.buffer[charIndex]=='\r'||pEdit->text.buffer[charIndex]=='\n')
		{
			x=pEdit->HorizontalOffset;
			y+=__PX_FONT_HEIGHT+pEdit->ySpacing;
		}
		else if(pEdit->text.buffer[charIndex]=='\t')
		{
			x+=__PX_FONT_ASCSIZE+pEdit->xSpacing;
		}
		else
		{
			if (pEdit->text.buffer[charIndex]&0x80)
			{
				if (pEdit->Password)
				{
					x+=__PX_FONT_ASCSIZE+pEdit->xSpacing;
					charIndex++;
				}
				else
				{
					x+=__PX_FONT_GBKSIZE+pEdit->xSpacing;
					charIndex++;
				}
			}
			else
			{
				x+=__PX_FONT_ASCSIZE+pEdit->xSpacing;
			}
		}

		if (pEdit->AutoNewline)
		{
			if (x>=pObject->Width-__PX_FONT_GBKSIZE-pEdit->xSpacing-pEdit->HorizontalOffset)
			{
				x=pEdit->HorizontalOffset;
				y+=__PX_FONT_HEIGHT+pEdit->ySpacing;
			}
		}
	}

	pEdit->cursor_index=textLen;

}

px_void PX_Object_EditUpdateCursorViewRegion(PX_Object *pObject)
{
	px_int charIndex;
	px_int textLen,x,y;
	PX_Object_Edit *pEdit=(PX_Object_Edit *)pObject->pObject;
	if (pEdit==PX_NULL)
	{
		return;
	}

	x=pEdit->HorizontalOffset;
	y=pEdit->VerticalOffset;

	textLen=px_strlen(pEdit->text.buffer);

	for (charIndex=0;charIndex<textLen;charIndex++)
	{

		if (pEdit->onFocus&&pEdit->cursor_index==charIndex)
		{
			break;
		}

		if(pEdit->text.buffer[charIndex]=='\r'||pEdit->text.buffer[charIndex]=='\n')
		{
			x=pEdit->HorizontalOffset;
			y+=__PX_FONT_HEIGHT+pEdit->ySpacing;
		}
		else if(pEdit->text.buffer[charIndex]=='\t')
		{
			x+=__PX_FONT_ASCSIZE+pEdit->xSpacing;
		}
		else
		{
			if (pEdit->text.buffer[charIndex]&0x80)
			{
				if (pEdit->Password)
				{
					x+=__PX_FONT_ASCSIZE+pEdit->xSpacing;
					charIndex++;
				}
				else
				{
					x+=__PX_FONT_GBKSIZE+pEdit->xSpacing;
					charIndex++;
				}
			}
			else
			{
				x+=__PX_FONT_ASCSIZE+pEdit->xSpacing;
			}
		}

		if (pEdit->AutoNewline)
		{
			if (x>=pObject->Width-__PX_FONT_GBKSIZE-pEdit->xSpacing-pEdit->HorizontalOffset)
			{
				x=pEdit->HorizontalOffset;
				y+=__PX_FONT_HEIGHT+pEdit->ySpacing;
			}
		}
	}
	

	if (x!=pEdit->XOffset+(px_int)pObject->Width-pEdit->HorizontalOffset-__PX_FONT_ASCSIZE)
	{
		pEdit->XOffset=x-((px_int)pObject->Width-pEdit->HorizontalOffset-__PX_FONT_ASCSIZE);
		if (pEdit->XOffset<0)
		{
			pEdit->XOffset=0;
		}
	}

	if (x!=pEdit->YOffset+(px_int)pObject->Height-pEdit->VerticalOffset-__PX_FONT_HEIGHT)
	{
		pEdit->YOffset=y-((px_int)pObject->Height-pEdit->VerticalOffset-__PX_FONT_HEIGHT);
		if (pEdit->YOffset<0)
		{
			pEdit->YOffset=0;
		}
	}

}
px_void PX_Object_EditRender(px_surface *psurface, PX_Object *pObject,px_uint elpased)
{
	px_int charIndex;
	px_int x_draw_oft,y_draw_oft,textLen,x,y;
	PX_Object_Edit *pEdit=(PX_Object_Edit *)pObject->pObject;
	if (pEdit==PX_NULL)
	{
		return;
	}

	if (!pObject->Visible)
	{
		return;
	}

	//clear
	PX_SurfaceClear(&pEdit->EditSurface,0,0,(px_int)pObject->Width-1,(px_int)pObject->Height-1,pEdit->BackgroundColor);

	if (pEdit->Border)
	{
		if(pEdit->state==PX_OBJECT_EDIT_STATE_NORMAL)
			PX_GeoDrawBorder(&pEdit->EditSurface,0,0,(px_int)pObject->Width-1,(px_int)pObject->Height-1,1,pEdit->BorderColor);
		else
			PX_GeoDrawBorder(&pEdit->EditSurface,0,0,(px_int)pObject->Width-1,(px_int)pObject->Height-1,1,pEdit->CursorColor);
	}

	x=pEdit->HorizontalOffset;
	y=pEdit->VerticalOffset;

	textLen=px_strlen(pEdit->text.buffer);

	

	for (charIndex=0;charIndex<textLen;charIndex++)
	{
		//Draw Cursor
		x_draw_oft=x-pEdit->XOffset;
		y_draw_oft=y-pEdit->YOffset;

		if (pEdit->onFocus&&pEdit->cursor_index==charIndex)
		{
			pEdit->elpased+=elpased;
			if ((pEdit->elpased/200)&1)
			{
				PX_GeoDrawRect(&pEdit->EditSurface,x_draw_oft,y_draw_oft,x_draw_oft+1,y_draw_oft+__PX_FONT_HEIGHT-1,pEdit->CursorColor);
			}
		}

		if(pEdit->text.buffer[charIndex]=='\r'||pEdit->text.buffer[charIndex]=='\n')
		{
			x=pEdit->HorizontalOffset;
			y+=__PX_FONT_HEIGHT+pEdit->ySpacing;
		}
		else if(pEdit->text.buffer[charIndex]=='\t')
		{
			x+=__PX_FONT_ASCSIZE+pEdit->xSpacing;
		}
		else
		{
			//draw Text
			
			if (y_draw_oft>pObject->Height)
			{
				break;
			}

			if (x_draw_oft+__PX_FONT_GBKSIZE>0&&y_draw_oft+__PX_FONT_HEIGHT>0)
			{
				if (pEdit->Password)
				{
					PX_FontDrawChar(&pEdit->EditSurface,x_draw_oft,y_draw_oft,"*",pEdit->TextColor);
				}
				else
				{
					PX_FontDrawChar(&pEdit->EditSurface,x_draw_oft,y_draw_oft,pEdit->text.buffer+charIndex,pEdit->TextColor);
				}
			}


			if (pEdit->text.buffer[charIndex]&0x80)
			{
				if (pEdit->Password)
				{
					x+=__PX_FONT_ASCSIZE+pEdit->xSpacing;
					charIndex++;
				}
				else
				{
					x+=__PX_FONT_GBKSIZE+pEdit->xSpacing;
					charIndex++;
				}
			}
			else
			{
				x+=__PX_FONT_ASCSIZE+pEdit->xSpacing;
			}
		}

		if (pEdit->AutoNewline)
		{
			if (x>=pObject->Width-__PX_FONT_GBKSIZE-pEdit->xSpacing-pEdit->HorizontalOffset)
			{
				x=pEdit->HorizontalOffset;
				y+=__PX_FONT_HEIGHT+pEdit->ySpacing;
			}
		}
	}
	if (textLen==0)
	{
		//just draw cursor
		if (pEdit->onFocus)
		{
			pEdit->elpased+=elpased;
			if ((pEdit->elpased/200)&1)
			{
				PX_GeoDrawRect(&pEdit->EditSurface,x,y,x+__PX_FONT_ASCSIZE-1,y+__PX_FONT_HEIGHT-1,pEdit->CursorColor);
			}
		}
	}
	else
	if (pEdit->onFocus&&pEdit->cursor_index==charIndex)
	{
		x_draw_oft=x-pEdit->XOffset;
		y_draw_oft=y-pEdit->YOffset;

		pEdit->elpased+=elpased;
		if ((pEdit->elpased/200)&1)
		{
			PX_GeoDrawRect(&pEdit->EditSurface,x_draw_oft,y_draw_oft,x_draw_oft+__PX_FONT_ASCSIZE-1,y_draw_oft+__PX_FONT_HEIGHT-1,pEdit->CursorColor);
		}
	}

	PX_SurfaceRender(psurface,&pEdit->EditSurface,(px_int)pObject->x,(px_int)pObject->y,PX_TEXTURERENDER_REFPOINT_LEFTTOP,PX_NULL);
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
	e.Event=PX_OBJECT_EVENT_VALUECHAGE;
	e.param_ptr[0]=(px_void *)(pEdit->text.buffer);
	PX_ObjectExecuteEvent(pObject,e);
}

px_void PX_Object_EditBackspace(PX_Object *pObject)
{
	px_int oft;
	PX_Object_Event e;
	PX_Object_Edit *pEdit=PX_Object_GetEdit(pObject);
	if (pObject!=PX_NULL&&pEdit->onFocus)
	{
		oft=pEdit->cursor_index-1;
		if(oft>=0)
		{
			if(pEdit->text.buffer[oft]&0x80)
			{
				if (PX_StringRemoveChar(&pEdit->text,oft))
				{
					pEdit->cursor_index--;
					oft--;
				}
			}
			if (PX_StringRemoveChar(&pEdit->text,oft))
			{
				pEdit->cursor_index--;
				oft--;
			}
			e.Event=PX_OBJECT_EVENT_VALUECHAGE;
			e.param_ptr[0]=(px_void *)(pEdit->text.buffer);
			PX_ObjectExecuteEvent(pObject,e);
		}
	}
}

px_void PX_Object_EditCursorBack(PX_Object *pObject)
{
	px_int oft;
	PX_Object_Edit *pEdit=PX_Object_GetEdit(pObject);
	if (pObject!=PX_NULL&&pEdit->onFocus)
	{
		oft=pEdit->cursor_index-1;
		if(oft>=0)
		{
			if(pEdit->text.buffer[oft]&0x80)
			{
					pEdit->cursor_index--;
					oft--;
			}
			pEdit->cursor_index--;
			oft--;
		}
	}
	PX_Object_EditUpdateCursorViewRegion(pObject);
}

px_void PX_Object_EditCursorForward(PX_Object *pObject)
{
	PX_Object_Edit *pEdit=PX_Object_GetEdit(pObject);
	
	if (pObject!=PX_NULL&&pEdit->onFocus)
	{
		if (pEdit->text.buffer[pEdit->cursor_index]==0)
		{
			return;
		}
		if(pEdit->text.buffer[pEdit->cursor_index]&0x80)
		{
			pEdit->cursor_index++;
		}
		if (pEdit->text.buffer[pEdit->cursor_index]==0)
		{
			return;
		}
		pEdit->cursor_index++;
	}
	PX_Object_EditUpdateCursorViewRegion(pObject);
}


px_void PX_Object_EditAutoNewLine(PX_Object *pObject,px_bool b)
{
	PX_Object_Edit *pEdit=PX_Object_GetEdit(pObject);
	if (pObject!=PX_NULL)
	{
		pEdit->AutoNewline=b;
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

// 
// PX_Object * PX_Object_StaticImageCreate(px_memorypool *mp,PX_Object *Parent,px_int x,px_int y,px_texture tex )
// {
// 	PX_Object *pObject;
// 	PX_Object_StaticImage *pImage=(PX_Object_StaticImage *)MP_Malloc(mp,sizeof(PX_Object_StaticImage));
// 	if (pImage==PX_NULL)
// 	{
// 		return PX_NULL;
// 	}
// 	pObject=PX_ObjectCreate(mp,Parent,(px_float)x,(px_float)y,0,0,0,0);
// 	if (pObject==PX_NULL)
// 	{
// 		MP_Free(pObject->mp,pImage);
// 		return PX_NULL;
// 	}
// 
// 	pObject->pObject=pImage;
// 	pObject->Enabled=PX_TRUE;
// 	pObject->Visible=PX_TRUE;
// 	pObject->Type=PX_OBJECT_TYPE_STATICIMAGE;
// 	pObject->ReceiveEvents=PX_FALSE;
// 	pObject->Func_ObjectFree=PX_Object_StaticImageFree;
// 	pObject->Func_ObjectRender=PX_Object_StaticImageRender;
// 	pImage->Texture=tex;
// 	pImage->Align=PX_OBJECT_ALIGN_LEFT|PX_OBJECT_ALIGN_TOP;
// 	return pObject;
// }
// 
// 
// PX_Object_StaticImage * PX_Object_GetStaticImage( PX_Object *Object )
// {
// 	if(Object->Type==PX_OBJECT_TYPE_STATICIMAGE)
// 		return (PX_Object_StaticImage *)Object->pObject;
// 	else
// 		return PX_NULL;
// }
// 
// px_void PX_Object_StaticImageSetAlign( PX_Object *pImage,px_uchar Align)
// {
// 	PX_Object_StaticImage *Bitmap=PX_Object_GetStaticImage(pImage);
// 	if (Bitmap)
// 	{
// 		Bitmap->Align=Align;
// 	}
// }
// 
// 
// px_void PX_Object_StaticImageRender(px_surface *psurface, PX_Object *im,px_uint elpased)
// {
// 	px_int x;
// 	px_int y;
// 	PX_Object_StaticImage *pImage=PX_Object_GetStaticImage(im);
// 	x=(px_int)im->x;
// 	y=(px_int)im->y;
// 
// 	if (pImage->Align&PX_OBJECT_ALIGN_BOTTOM)
// 	{
// 		y=(px_int)im->y+(px_int)im->Height-pImage->Texture.height;
// 	}
// 	if (pImage->Align&PX_OBJECT_ALIGN_TOP)
// 	{
// 		y=(px_int)im->y;
// 	}
// 	if (pImage->Align&PX_OBJECT_ALIGN_LEFT)
// 	{
// 		x=(px_int)im->x;
// 	}
// 	if (pImage->Align&PX_OBJECT_ALIGN_RIGHT)
// 	{
// 		x=(px_int)im->x+(px_int)im->Width-pImage->Texture.width;
// 	}
// 
// 	if (pImage->Align&PX_OBJECT_ALIGN_HCENTER)
// 	{
// 		x=(px_int)im->x+((px_int)im->Width-pImage->Texture.width)/2;
// 	}
// 	if (pImage->Align&PX_OBJECT_ALIGN_VCENTER)
// 	{
// 		y=(px_int)im->y+((px_int)im->Height-pImage->Texture.height)/2;
// 	}
// 
// 	if (pImage!=PX_NULL)
// 	{
// 		PX_TextureRender(psurface,&pImage->Texture,x,y,PX_TEXTURERENDER_REFPOINT_LEFTTOP,PX_NULL);
// 	}
// }
// 
// px_void PX_Object_StaticImageFree(PX_Object *pObj)
// {
// 	PX_Object_StaticImage *pImage;
// 	pImage=PX_Object_GetStaticImage(pObj);
// 	if (pImage)
// 	{
// 		PX_TextureFree(&pImage->Texture);
// 	}
// }

px_void PX_Object_ScrollArea_EventDispatcher(PX_Object *Object,PX_Object_Event e,px_void *user_ptr)
{
	PX_Object_ScrollArea *pSA=PX_Object_GetScrollArea(Object);
	if (!pSA)
	{
		return;
	}

	if (e.Event==PX_OBJECT_EVENT_CURSORCLICK||e.Event==PX_OBJECT_EVENT_CURSORDRAG||e.Event==PX_OBJECT_EVENT_CURSORDOWN||e.Event==PX_OBJECT_EVENT_CURSORUP||e.Event==PX_OBJECT_EVENT_CURSORMOVE)
	{

		if (!PX_ObjectIsPointInRegion(Object,(px_float)e.Param_uint[0],(px_float)e.Param_uint[1]))
		{
			return;
		}
		e.Param_uint[0]-=((px_int)Object->x-pSA->oftx);
		e.Param_uint[1]-=((px_int)Object->y-pSA->ofty);
	}
	PX_ObjectPostEvent(pSA->Object,e);
}



PX_Object * PX_Object_ScrollAreaCreate(px_memorypool *mp,PX_Object *Parent,int x,int y,int Width,int Height)
{
	PX_Object *pObject;
	PX_Object_ScrollArea *pSA=(PX_Object_ScrollArea *)MP_Malloc(mp,sizeof(PX_Object_ScrollArea));

	if (pSA==PX_NULL)
	{
		return PX_NULL;
	}

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
	pSA->Object=PX_ObjectCreate(pObject->mp,0,0,0,0,0,0,0);
	pSA->oftx=0;
	pSA->ofty=0;

	if(!PX_SurfaceCreate(mp,Width,Height,&pSA->surface))
	{
		MP_Free(mp,pSA);
		MP_Free(mp,pObject);
		return PX_NULL;
	}
	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_ANY,PX_Object_ScrollArea_EventDispatcher,PX_NULL);

	return pObject;
}

PX_Object * PX_Object_ScrollAreaGetIncludedObjects(PX_Object *pObj)
{
	PX_Object_ScrollArea *pSA;
	pSA=PX_Object_GetScrollArea(pObj);
	if (pSA)
	{
		return pSA->Object;
	}
	return PX_NULL;
}

px_void PX_Object_ScrollAreaMoveToBottom(PX_Object *pObject)
{
	int w,h;
	PX_Object_ScrollArea *psa=PX_Object_GetScrollArea(pObject);
	if(psa)
	{
		PX_Object_ScrollAreaGetWidthHeight(psa->Object,&w,&h);
		if (h>=pObject->Height)
		{
			psa->ofty=h-(px_int)pObject->Height+1;
		}
		else
		{
			psa->ofty=0;
		}
	}
	
}

px_void PX_Object_ScrollAreaMoveToTop(PX_Object *pObject)
{
	PX_Object_ScrollArea *psa=PX_Object_GetScrollArea(pObject);
	if(psa)
	psa->ofty=0;
}


static px_void PX_Object_ScrollAreaUpdateObjectPositionOffset(PX_Object *pObject,px_int oftX,px_int oftY)
{
	if (pObject==PX_NULL)
	{
		return;
	}
	pObject->x+=oftX;
	pObject->y+=oftY;
	PX_Object_ScrollAreaUpdateObjectPositionOffset(pObject->pNextBrother,oftX,oftY);
	PX_Object_ScrollAreaUpdateObjectPositionOffset(pObject->pChilds,oftX,oftY);
}

px_void PX_Object_ScrollAreaGetWidthHeight(PX_Object *pObject,px_int *Width,px_int *Height)
{
	if (pObject==PX_NULL)
	{
		return;
	}
	if (pObject->x+pObject->Width-1>*Width)
	{
		*Width=(px_int)pObject->x+(px_int)pObject->Width;
	}

	if (pObject->y+pObject->Height-1>*Height)
	{
		*Height=(px_int)pObject->y+(px_int)pObject->Height;
	}

	PX_Object_ScrollAreaGetWidthHeight(pObject->pNextBrother,Width,Height);
	PX_Object_ScrollAreaGetWidthHeight(pObject->pChilds,Width,Height);
}

px_void PX_Object_ScrollAreaRender(px_surface *psurface, PX_Object *pObject,px_uint elpased)
{
	PX_Object_ScrollArea *pSA;

	pSA=PX_Object_GetScrollArea(pObject);
	if (!pSA)
	{
		return;
	}

	PX_SurfaceClear(&pSA->surface,0,0,(px_int)pObject->Width-1,(px_int)pObject->Height-1,pSA->BackgroundColor);

	//draw
	PX_Object_ScrollAreaUpdateObjectPositionOffset(pSA->Object,-pSA->oftx,-pSA->ofty);
	PX_ObjectRender(&pSA->surface,pSA->Object,elpased);
	PX_Object_ScrollAreaUpdateObjectPositionOffset(pSA->Object,pSA->oftx,pSA->ofty);
	PX_SurfaceRender(psurface,&pSA->surface,(px_int)pObject->x,(px_int)pObject->y,PX_TEXTURERENDER_REFPOINT_LEFTTOP,PX_NULL);

	//Draw Border
	if(pSA->bBorder)
	PX_GeoDrawBorder(&pSA->surface,0,0,(px_int)pObject->Width-1,(px_int)pObject->Height-1,1,pSA->borderColor);
	PX_SurfaceRender(psurface,&pSA->surface,(px_int)pObject->x,(px_int)pObject->y,PX_TEXTURERENDER_REFPOINT_LEFTTOP,PX_NULL);
}



px_void PX_Object_ScrollAreaFree(PX_Object *pObj)
{
	PX_Object_ScrollArea *pSA;
	pSA=PX_Object_GetScrollArea(pObj);
	if (pSA)
	{
		PX_ObjectDelete(pSA->Object);
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


px_void PX_Object_ScrollAreaSetBorder(PX_Object *pObj,px_uchar Border)
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

PX_Object * PX_Object_AutoTextCreate(px_memorypool *mp,PX_Object *Parent,int x,int y,int width)
{
	PX_Object *pObject;

	PX_Object_AutoText *pAt=(PX_Object_AutoText *)MP_Malloc(mp,sizeof(PX_Object_AutoText));
	PX_StringInit(mp,&pAt->text);
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
	return pObject;
}

px_void PX_Object_AutoTextRender(px_surface *psurface, PX_Object *pObject,px_uint elpased)
{
	int i;
	int w=0;
	int h=0;
	PX_Object_AutoText * pAt=PX_Object_GetAutoText(pObject);
	
	if (!pAt)
	{
		return;
	}
	for (i=0;i<px_strlen(pAt->text.buffer);i++)
	{
		if (pAt->text.buffer[i]=='\n')
		{
			w=0;
			h+=PX_FontGetCharactorHeight();
			continue;
		}
		

		if (pAt->text.buffer[i]&0x80)
		{
			if(w+PX_FontGetGbkCharactorWidth()>pObject->Width)
			{
				w=0;
				h+=PX_FontGetCharactorHeight();
				PX_FontDrawGBK(psurface,(px_int)pObject->x+w,(px_int)pObject->y+h,(px_uchar *)pAt->text.buffer+i,pAt->TextColor);
				w+=PX_FontGetGbkCharactorWidth();
			}
			else
			{
				PX_FontDrawGBK(psurface,(px_int)pObject->x+w,(px_int)pObject->y+h,(px_uchar *)pAt->text.buffer+i,pAt->TextColor);
				w+=PX_FontGetGbkCharactorWidth();
			}
			i++;
		}
		else
		{
			if(w+PX_FontGetAscCharactorWidth()>pObject->Width)
			{
				w=0;
				h+=PX_FontGetCharactorHeight();
				PX_FontDrawASCII(psurface,(px_int)pObject->x+w,(px_int)pObject->y+h,pAt->text.buffer[i],pAt->TextColor);
				w+=PX_FontGetAscCharactorWidth();
			}
			else
			{
				PX_FontDrawASCII(psurface,(px_int)pObject->x+w,(px_int)pObject->y+h,pAt->text.buffer[i],pAt->TextColor);
				w+=PX_FontGetAscCharactorWidth();
			}
		}
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
	int i;
	int w=0;
	int h=0;
	PX_Object_AutoText * pAt=PX_Object_GetAutoText(pObject);

	if (!pAt)
	{
		return 0;
	}
	for (i=0;i<px_strlen(pAt->text.buffer);i++)
	{
		if (pAt->text.buffer[i]=='\n')
		{
			w=0;
			h+=PX_FontGetCharactorHeight();
			continue;
		}
		

		if (pAt->text.buffer[i]&0x80)
		{
			if(w+PX_FontGetGbkCharactorWidth()>pObject->Width)
			{
				w=0;
				h+=PX_FontGetCharactorHeight();
				w+=PX_FontGetGbkCharactorWidth();
			}
			else
			{
				w+=PX_FontGetGbkCharactorWidth();
			}
			i++;
		}
		else
		{
			if(w+PX_FontGetAscCharactorWidth()>pObject->Width)
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


px_void PX_Object_AutoTextSetText(PX_Object *Obj,px_char *Text)
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

PX_Object * PX_Object_AnimationCreate(px_memorypool *mp,PX_Object *Parent,px_int x,px_int y,px_animationlibrary *lib)
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
	pAnimation->Align=PX_OBJECT_ALIGN_HCENTER|PX_OBJECT_ALIGN_VCENTER;
	return pObject;
}

px_void PX_Object_AnimationSetLibrary(PX_Object *Object,px_animationlibrary *lib)
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

px_void PX_Object_AnimationSetAlign(PX_Object *panimation,px_uchar Align)
{
	PX_Object_Animation *pA=PX_Object_GetAnimation(panimation);
	if (pA)
	{
		pA->Align=Align;
	}
}

px_void PX_Object_AnimationRender(px_surface *psurface,PX_Object *pObj,px_uint elpased)
{
	PX_Object_Animation *pA=PX_Object_GetAnimation(pObj);
	PX_TEXTURERENDER_REFPOINT ref;
	if (pA)
	{
		if ((pA->Align&PX_OBJECT_ALIGN_LEFT)&&(pA->Align&PX_OBJECT_ALIGN_TOP))
		{
			ref=PX_TEXTURERENDER_REFPOINT_LEFTTOP;
		}
		if ((pA->Align&PX_OBJECT_ALIGN_LEFT)&&(pA->Align&PX_OBJECT_ALIGN_VCENTER))
		{
			ref=PX_TEXTURERENDER_REFPOINT_LEFTMID;
		}
		if ((pA->Align&PX_OBJECT_ALIGN_LEFT)&&(pA->Align&PX_OBJECT_ALIGN_BOTTOM))
		{
			ref=PX_TEXTURERENDER_REFPOINT_LEFTBOTTOM;
		}

		if ((pA->Align&PX_OBJECT_ALIGN_HCENTER)&&(pA->Align&PX_OBJECT_ALIGN_TOP))
		{
			ref=PX_TEXTURERENDER_REFPOINT_MIDTOP;
		}

		if ((pA->Align&PX_OBJECT_ALIGN_HCENTER)&&(pA->Align&PX_OBJECT_ALIGN_VCENTER))
		{
			ref=PX_TEXTURERENDER_REFPOINT_CENTER;
		}

		if ((pA->Align&PX_OBJECT_ALIGN_HCENTER)&&(pA->Align&PX_OBJECT_ALIGN_BOTTOM))
		{
			ref=PX_TEXTURERENDER_REFPOINT_MIDBOTTOM;
		}

		if ((pA->Align&PX_OBJECT_ALIGN_RIGHT)&&(pA->Align&PX_OBJECT_ALIGN_TOP))
		{
			ref=PX_TEXTURERENDER_REFPOINT_RIGHTTOP;
		}

		if ((pA->Align&PX_OBJECT_ALIGN_RIGHT)&&(pA->Align&PX_OBJECT_ALIGN_VCENTER))
		{
			ref=PX_TEXTURERENDER_REFPOINT_RIGHTMID;
		}

		if ((pA->Align&PX_OBJECT_ALIGN_RIGHT)&&(pA->Align&PX_OBJECT_ALIGN_BOTTOM))
		{
			ref=PX_TEXTURERENDER_REFPOINT_RIGHTBOTTOM;
		}
		PX_AnimationUpdate(&pA->animation,elpased);
		PX_AnimationRender(psurface,&pA->animation,PX_POINT((px_float)pObj->x,(px_float)pObj->y,0),ref,PX_NULL);
	}
	
}

px_void PX_Object_AnimationFree(PX_Object *pObj)
{
	PX_Object_Animation *pA=PX_Object_GetAnimation(pObj);
	if (pA)
	PX_AnimationFree(&pA->animation);
}

PX_Object * PX_Object_ParticalCreate(px_memorypool *mp,PX_Object *Parent,px_int x,px_int y,px_int z,px_texture *pTexture,PX_ScriptVM_Instance *pIns,px_char *Initfunc,px_char *_create,px_char *_update)
{
	PX_Object *pObject;
	PX_Object_Partical *pPartical=(PX_Object_Partical *)MP_Malloc(mp,sizeof(PX_Object_Partical));
	if (pPartical==PX_NULL)
	{
		return PX_NULL;
	}

	if(!PX_ParticalLauncherCreate(&pPartical->launcher,mp,pTexture,pIns,Initfunc,_create,_update))
	{
		MP_Free(mp,pPartical);
	}

	pObject=PX_ObjectCreate(mp,Parent,(px_float)x,(px_float)y,0,0,0,0);
	if (pObject==PX_NULL)
	{
		MP_Free(pObject->mp,pPartical);
		return PX_NULL;
	}

	pObject->pObject=pPartical;
	pObject->Enabled=PX_TRUE;
	pObject->Visible=PX_TRUE;
	pObject->Type=PX_OBJECT_TYPE_PARTICAL;
	pObject->ReceiveEvents=PX_FALSE;
	pObject->Func_ObjectFree=PX_Object_ParticalFree;
	pObject->Func_ObjectRender=PX_Object_ParticalRender;
	
	return pObject;
}


PX_Object * PX_Object_ParticalCreateEx(px_memorypool *mp,PX_Object *Parent,px_int x,px_int y,px_int z,PX_ParticalLauncher_InitializeInfo info)
{
	PX_Object *pObject;
	PX_Object_Partical *pPartical=(PX_Object_Partical *)MP_Malloc(mp,sizeof(PX_Object_Partical));
	if (pPartical==PX_NULL)
	{
		return PX_NULL;
	}

	if(!PX_ParticalLauncherCreateEx(&pPartical->launcher,mp,info))
	{
		MP_Free(mp,pPartical);
	}

	pObject=PX_ObjectCreate(mp,Parent,(px_float)x,(px_float)y,0,0,0,0);

	if (pObject==PX_NULL)
	{
		MP_Free(pObject->mp,pPartical);
		return PX_NULL;
	}

	pObject->pObject=pPartical;
	pObject->Enabled=PX_TRUE;
	pObject->Visible=PX_TRUE;
	pObject->Type=PX_OBJECT_TYPE_PARTICAL;
	pObject->ReceiveEvents=PX_FALSE;
	pObject->Func_ObjectFree=PX_Object_ParticalFree;
	pObject->Func_ObjectRender=PX_Object_ParticalRender;

	return pObject;
}

PX_Object_Partical * PX_Object_GetPartical(PX_Object *Object)
{
	if (Object->Type==PX_OBJECT_TYPE_PARTICAL)
	{
		return (PX_Object_Partical *)Object->pObject;
	}
	PX_ASSERT();
	return PX_NULL;
}

px_void PX_Object_ParticalRender(px_surface *psurface,PX_Object *pObject,px_uint elpased)
{
	PX_Object_Partical * pPartical=PX_Object_GetPartical(pObject);
	if (pPartical)
	{
		PX_ParticalLauncherUpdate(&pPartical->launcher,elpased);
		PX_ParticalLauncherRender(psurface,&pPartical->launcher,PX_POINT((px_float)pObject->x,(px_float)pObject->y,1));
	}
}

px_void PX_Object_ParticalFree(PX_Object *pObject)
{
	PX_Object_Partical * pPartical=PX_Object_GetPartical(pObject);
	if (pPartical)
	{
		PX_ParticalLauncherFree(&pPartical->launcher);
	}
}

px_void PX_Object_ParticalSetDirection(PX_Object *pObject,px_point direction)
{
	PX_Object_Partical * pPartical=PX_Object_GetPartical(pObject);
	if (pPartical)
	{
		pPartical->launcher.direction=direction;
	}
}

px_void PX_ObjectSetUserCode(PX_Object *pObject,px_int user)
{
	pObject->User_int=user;
}

px_void PX_ObjectSetUserPointer(PX_Object *pObject,px_void *user_ptr)
{
	pObject->User_ptr=user_ptr;
}


px_void PX_Object_RoundCursor_Mousemove(PX_Object *pobject,PX_Object_Event e,px_void *user_ptr)
{
	PX_Object_RoundCursorSetDirection(pobject,PX_POINT((px_float)e.Param_uint[0],(px_float)e.Param_uint[1],0));
}

PX_Object * PX_Object_RoundCursorCreate(px_memorypool *mp,PX_Object *Parent,px_int x,px_int y,px_shape *pShape,px_color blendColor)
{
	PX_Object *pObject;
	PX_Object_RoundCursor *pRoundCursor=(PX_Object_RoundCursor *)MP_Malloc(mp,sizeof(PX_Object_RoundCursor));
	if (pRoundCursor==PX_NULL)
	{
		return PX_NULL;
	}

	pRoundCursor->shape=pShape;
	pRoundCursor->blendColor=blendColor;
	pRoundCursor->cursorPoint=PX_POINT(0,0,0);
	pObject=PX_ObjectCreate(mp,Parent,(px_float)x,(px_float)y,0,0,0,0);

	if (pObject==PX_NULL)
	{
		MP_Free(pObject->mp,pRoundCursor);
		return PX_NULL;
	}

	pObject->pObject=pRoundCursor;
	pObject->Enabled=PX_TRUE;
	pObject->Visible=PX_TRUE;
	pObject->Type=PX_OBJECT_TYPE_ROUNDCURSOR;
	pObject->ReceiveEvents=PX_TRUE;
	pObject->Func_ObjectFree=PX_Object_RoundCursorFree;
	pObject->Func_ObjectRender=PX_Object_RoundCursorRender;
	pObject->Func_ObjectUpdate=PX_NULL;
	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_CURSORMOVE,PX_Object_RoundCursor_Mousemove,PX_NULL);
	return pObject;
}



px_void PX_Object_RoundCursorRender(px_surface *psurface,PX_Object *pObject,px_uint elpased)
{
	px_float vector_x,vector_y,sqrt_rec;
	PX_Object_RoundCursor * pRoundCursor=PX_Object_GetRoundCursor(pObject);
	if (pRoundCursor)
	{
		vector_x=pRoundCursor->cursorPoint.x-pObject->x;
		vector_y=pRoundCursor->cursorPoint.y-pObject->y;
		if (vector_x*vector_x+vector_y*vector_y)
		{
			sqrt_rec=PX_SqrtRec(vector_x*vector_x+vector_y*vector_y);
			vector_x*=sqrt_rec;
			vector_y*=sqrt_rec;
			PX_ShapeRenderEx_sincos(psurface,pRoundCursor->shape,(px_int)pObject->x,(px_int)pObject->y,PX_TEXTURERENDER_REFPOINT_CENTER,pRoundCursor->blendColor,1.0f,-vector_x,-vector_y);
		}
		
	}

	
}

px_void PX_Object_RoundCursorFree(PX_Object *pObject)
{
	
}

PX_Object_RoundCursor * PX_Object_GetRoundCursor(PX_Object *Object)
{
	if (Object->Type==PX_OBJECT_TYPE_ROUNDCURSOR)
	{
		return (PX_Object_RoundCursor *)Object->pObject;
	}
	PX_ASSERT();
	return PX_NULL;
}

px_void PX_Object_RoundCursorSetDirection(PX_Object *Object,px_point cursor_point)
{
	PX_Object_RoundCursor * pRoundCursor=PX_Object_GetRoundCursor(Object);
	if (pRoundCursor)
	{
		pRoundCursor->cursorPoint=cursor_point;
	}
}

px_void PX_Object_RoundCursorSetColor(PX_Object *Object,px_color clr)
{
	PX_Object_RoundCursor * pRoundCursor=PX_Object_GetRoundCursor(Object);
	if (pRoundCursor)
	{
		pRoundCursor->blendColor=clr;
	}
}

PX_Object * PX_Object_ShapeCreate(px_memorypool *mp, PX_Object *Parent,px_int x,px_int y,px_shape *pshape )
{
	PX_Object *pObject;
	PX_Object_Shape *pShape=(PX_Object_Shape *)MP_Malloc(mp,sizeof(PX_Object_Shape));
	if (pShape==PX_NULL)
	{
		return PX_NULL;
	}
	pObject=PX_ObjectCreate(mp,Parent,(px_float)x,(px_float)y,0,0,0,0);
	if (pObject==PX_NULL)
	{
		MP_Free(pObject->mp,pShape);
		return PX_NULL;
	}

	pObject->pObject=pShape;
	pObject->Enabled=PX_TRUE;
	pObject->Visible=PX_TRUE;
	pObject->Type=PX_OBJECT_TYPE_SHAPE;
	pObject->ReceiveEvents=PX_FALSE;
	pObject->Func_ObjectFree=PX_NULL;
	pObject->Func_ObjectRender=PX_Object_ShapeRender;
	pShape->pShape=pshape;
	pShape->Align=PX_OBJECT_ALIGN_HCENTER|PX_OBJECT_ALIGN_VCENTER;
	return pObject;
}


PX_Object_Shape * PX_Object_GetShape( PX_Object *Object )
{
	if(Object->Type==PX_OBJECT_TYPE_SHAPE)
		return (PX_Object_Shape *)Object->pObject;
	else
		return PX_NULL;
}

px_void PX_Object_ShapeSetAlign( PX_Object *pShape,px_uchar Align)
{
	PX_Object_Shape *Bitmap=PX_Object_GetShape(pShape);
	if (Bitmap)
	{
		Bitmap->Align=Align;
	}
}


px_void PX_Object_ShapeRender(px_surface *psurface, PX_Object *im,px_uint elpased)
{
	px_int x;
	px_int y;
	PX_Object_Shape *pShape=PX_Object_GetShape(im);
	x=(px_int)im->x;
	y=(px_int)im->y;

	if (pShape->Align&PX_OBJECT_ALIGN_BOTTOM)
	{
		y=(px_int)im->y+(px_int)im->Height-pShape->pShape->height;
	}
	if (pShape->Align&PX_OBJECT_ALIGN_TOP)
	{
		y=(px_int)im->y;
	}
	if (pShape->Align&PX_OBJECT_ALIGN_LEFT)
	{
		x=(px_int)im->x;
	}
	if (pShape->Align&PX_OBJECT_ALIGN_RIGHT)
	{
		x=(px_int)im->x+(px_int)im->Width-pShape->pShape->width;
	}

	if (pShape->Align&PX_OBJECT_ALIGN_HCENTER)
	{
		x=(px_int)im->x+((px_int)im->Width-pShape->pShape->width)/2;
	}
	if (pShape->Align&PX_OBJECT_ALIGN_VCENTER)
	{
		y=(px_int)im->y+((px_int)im->Height-pShape->pShape->height)/2;
	}

	if (pShape!=PX_NULL)
	{
		PX_ShapeRender(psurface,pShape->pShape,x,y,PX_TEXTURERENDER_REFPOINT_LEFTTOP,pShape->blendcolor);
	}
}

px_void PX_Object_ShapeSetBlendColor(PX_Object *pShape,px_color blendcolor)
{
	PX_Object_Shape *Bitmap=PX_Object_GetShape(pShape);
	if (Bitmap)
	{
		Bitmap->blendcolor=blendcolor;
	}
}

px_void PX_Object_CursorButtonRender(px_surface *psurface, PX_Object *pObject,px_uint elpased)
{
	px_float w,h,_x,_y;
	px_uchar alpha;
	PX_Object_CursorButton *pcb;
	w=pObject->Width/4.f;
	h=pObject->Height/4.f;
	pcb=PX_Object_GetCursorButton(pObject);
	if (!pcb)
	{
		return;
	}
	pcb->pushbutton->x=pObject->x;
	pcb->pushbutton->y=pObject->y;
	pcb->pushbutton->z=pObject->z;
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
	_x=pObject->x-pcb->c_distance;
	_y=pObject->y-pcb->c_distance;
	PX_GeoDrawRect(psurface,(px_int)_x,(px_int)_y,(px_int)(_x+w),(px_int)_y+pcb->c_width,pcb->c_color);
	PX_GeoDrawRect(psurface,(px_int)_x,(px_int)_y+pcb->c_width+1,(px_int)_x+pcb->c_width,(px_int)(_y+h),pcb->c_color);

	//left bottom
	_x=pObject->x-pcb->c_distance;
	_y=pObject->y+pObject->Height+pcb->c_distance;
	PX_GeoDrawRect(psurface,(px_int)_x,(px_int)_y,(px_int)(_x+w),(px_int)_y-pcb->c_width,pcb->c_color);
	PX_GeoDrawRect(psurface,(px_int)_x,(px_int)_y-pcb->c_width-1,(px_int)_x+pcb->c_width,(px_int)(_y-h),pcb->c_color);

	//right top
	_x=pObject->x+pObject->Width+pcb->c_distance;
	_y=pObject->y-pcb->c_distance;
	PX_GeoDrawRect(psurface,(px_int)_x,(px_int)_y,(px_int)(_x-w),(px_int)_y+pcb->c_width,pcb->c_color);
	PX_GeoDrawRect(psurface,(px_int)_x,(px_int)_y+pcb->c_width+1,(px_int)_x-pcb->c_width,(px_int)(_y+h),pcb->c_color);

	//right bottom
	_x=pObject->x+pObject->Width+pcb->c_distance;
	_y=pObject->y+pObject->Height+pcb->c_distance;
	PX_GeoDrawRect(psurface,(px_int)_x,(px_int)_y,(px_int)(_x-w),(px_int)_y-pcb->c_width,pcb->c_color);
	PX_GeoDrawRect(psurface,(px_int)_x,(px_int)_y-pcb->c_width-1,(px_int)_x-pcb->c_width,(px_int)(_y-h),pcb->c_color);
}

px_void PX_Object_CursorButtonFree( PX_Object *Obj )
{

}
px_void PX_Object_CursorButtonOnMouseMove(PX_Object *Object,PX_Object_Event e,px_void *user_ptr)
{

	PX_Object_CursorButton *pCB=PX_Object_GetCursorButton(Object);
	px_int x,y;
	x=e.Param_int[0];
	y=e.Param_int[1];

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



PX_Object *PX_Object_CursorButtonCreate(px_memorypool *mp,PX_Object *Parent,px_int x,px_int y,px_int Width,px_int Height,px_char *Text,px_color Color)
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


	pCb->pushbutton=PX_Object_PushButtonCreate(mp,pObject,x,y,Width,Height,Text,Color);
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
