#include "PX_Object.h"


PX_Object  * PX_ObjectGetChild( PX_Object *pObject,px_int Index )
{
	PX_Object *pChildObject;
	if (!pObject)
	{
		return PX_NULL;
	}
	pChildObject =pObject->pChilds;
	while (Index>0&& pChildObject)
	{
		pChildObject = pChildObject->pNextBrother;
		Index--;
	}
	return pChildObject;

}

static PX_Object * PX_ObjectGetObjectLink(PX_Object *pObject,const px_char payload[])
{
	px_char id[PX_OBJECT_ID_MAXLEN]={0};
	px_int oft=0;
	if (!pObject)
	{
		return PX_NULL;
	}

	while (payload[oft]!='.'&&payload[oft]!='\0'&&oft<PX_OBJECT_ID_MAXLEN-1)
	{
		id[oft]=payload[oft];
		oft++;
	}



	if (payload[oft]=='.')
	{
		oft++;
	}

	if (payload[oft]=='\0')
	{
		if (PX_strequ(pObject->id,id))
		{
			return pObject;
		}
		else
		{
			return PX_ObjectGetObjectLink(pObject->pNextBrother,payload);
		}
	}
	else
	{
		if (PX_strequ(pObject->id,id))
		{
			return PX_ObjectGetObjectLink(pObject->pChilds,payload+oft);
		}
		else
		{
			return PX_ObjectGetObjectLink(pObject->pNextBrother,payload);
		}
	}
	return PX_NULL;
}

PX_Object * PX_ObjectGetObject(PX_Object *pObject,const px_char payload[])
{
	px_char id[PX_OBJECT_ID_MAXLEN]={0};

	px_int oft=0;
	if (!pObject)
	{
		return PX_NULL;
	}

	while (payload[oft]!='.'&&payload[oft]!='\0'&&oft<PX_OBJECT_ID_MAXLEN-1)
	{
		id[oft]=payload[oft];
		oft++;
	}

	if (payload[oft]=='.')
	{
		oft++;
	}

	if (payload[oft]=='\0')
	{
		if (PX_strequ(pObject->id,id))
		{
			return pObject;
		}
		else
		{
			return PX_NULL;
		}
	}
	else
	{
		if (PX_strequ(pObject->id,id))
		{
			return PX_ObjectGetObjectLink(pObject->pChilds,payload+oft);
		}
	}
	return PX_NULL;
}


static px_void PX_ObjectSetFocusEx(PX_Object *pObject)
{
	while(pObject)
	{
		pObject->OnFocus=PX_TRUE;
		pObject=pObject->pParent;
	}
}

static px_void PX_ObjectClearFocusEx(PX_Object *pObject)
{
	if (!pObject)
	{
		return;
	}
	pObject->OnFocus=PX_FALSE;
	pObject->OnFocusNode=PX_FALSE;
	PX_ObjectClearFocusEx(pObject->pChilds);
	PX_ObjectClearFocusEx(pObject->pNextBrother);
}

px_bool PX_ObjectIsOnFocus(PX_Object* pObject)
{
	return pObject->OnFocus&&pObject->OnFocusNode;
}

px_void PX_ObjectClearFocus(PX_Object *pObject)
{
	PX_Object *pClearObject=pObject;
	if (!pObject->OnFocus)
	{
		return;
	}
	while(pClearObject->pParent)
	{
		pClearObject=pClearObject->pParent;

		if (pClearObject->OnFocusNode)
		{
			pClearObject=pClearObject->pChilds;
			break;
		}
	}
	PX_ObjectClearFocusEx(pClearObject);
}



px_void PX_ObjectSetFocus(PX_Object *pObject)
{
	PX_Object *pClearObject=pObject;
	while(pClearObject->pParent)
	{
		pClearObject=pClearObject->pParent;
		if (pClearObject->OnFocusNode)
		{
			pClearObject=pClearObject->pChilds;
			break;
		}
	}
	PX_ObjectClearFocusEx(pClearObject);
	pObject->OnFocusNode=PX_TRUE;
	PX_ObjectSetFocusEx(pObject);
}


PX_Object * PX_ObjectGetRoot(PX_Object *pObject)
{
	while(pObject->pParent)
	{
		pObject=pObject->pParent;
	}
	return pObject;
}

PX_Object_Event PX_OBJECT_BUILD_EVENT(px_uint Event)
{
	PX_Object_Event e;
	PX_memset(&e,0,sizeof(e));
	e.Event=Event;
	return e;
}

PX_Object_Event PX_OBJECT_BUILD_EVENT_STRING(px_uint Event,const px_char *content)
{
	PX_Object_Event e;
	PX_memset(&e,0,sizeof(e));
	e.Event=Event;
	PX_Object_Event_SetStringPtr(&e,(px_void *)content);
	return e;
}

PX_Object_Event PX_OBJECT_BUILD_EVENT_INT(px_uint Event, px_int i)
{
	PX_Object_Event e;
	PX_memset(&e, 0, sizeof(e));
	e.Event = Event;
	PX_Object_Event_SetIndex(&e, i);
	return e;
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

px_float PX_Object_Event_GetCursorPressure(PX_Object_Event e)
{
	return e.Param_float[3];
}

px_int PX_Object_Event_GetCursorIndex(PX_Object_Event e)
{
	return e.Param_int[2];
}

px_float PX_Object_Event_GetDamage(PX_Object_Event e)
{
	return e.Param_float[0];
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

px_void PX_Object_Event_SetImpactTargetObject(PX_Object_Event* e, PX_Object* pObject)
{
	e->Param_ptr[0] = (px_void *)pObject;
}

PX_Object* PX_Object_Event_GetImpactTargetObject(PX_Object_Event e)
{
	return (PX_Object *)e.Param_ptr[0];
}

px_int PX_Object_Event_GetInt(PX_Object_Event e)
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

px_void PX_Object_Event_SetDamage(PX_Object_Event* e, px_float damage)
{
	e->Param_float[0] = damage;
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

px_void PX_Object_Event_SetKeyUp(PX_Object_Event *e,px_uint key)
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

px_int PX_ObjectGetFreeDescIndex(PX_Object* pObject)
{
	px_int i;
	for (i = 0; i < PX_OBJECT_MAX_DESC_COUNT; i++)
	{
		if (pObject->pObjectDesc[i] == PX_NULL)
		{
			return i;
		}
	}
	return -1;
}

px_bool PX_ObjectCheckType(PX_Object* pObject, px_int type)
{
	px_int i;
	for (i = 0; i < PX_OBJECT_MAX_DESC_COUNT; i++)
	{
		if (pObject->Type[i] == type)
		{
			return PX_TRUE;
		}
	}
	return PX_FALSE;
	
}

px_int PX_ObjectGetTypeIndex(PX_Object* pObject, px_int type)
{
	px_int i;
	for (i = 0; i < PX_OBJECT_MAX_DESC_COUNT; i++)
	{
		if (pObject->Type[i] == type)
		{
			return i;
		}
	}
	return -1;
	
}

px_int PX_ObjectGetDescIndexByType(PX_Object* pObject, px_int type)
{
	px_int i;
	PX_ASSERTIF(pObject == PX_NULL);
	for (i = 0; i < PX_OBJECT_MAX_DESC_COUNT; i++)
	{
		if (pObject->Type[i] == type)
		{
			return i;
		}
	}
	return -1;
}

px_void* PX_ObjectGetDescByType(PX_Object* pObject, px_int type)
{
	px_int i=PX_ObjectGetDescIndexByType(pObject, type);
	if(i!=-1)
		return pObject->pObjectDesc[i];
	return PX_NULL;
	
}

PX_Object * PX_ObjectCreate(px_memorypool *mp,PX_Object *Parent,px_float x,px_float y,px_float z,px_float Width,px_float Height,px_float Lenght)
{
	PX_Object *pObject=(PX_Object *)MP_Malloc(mp,sizeof(PX_Object));
	if (pObject==PX_NULL)
	{
		return PX_NULL;
	}

	PX_ObjectInitialize(mp,pObject,Parent,x,y,z,Width,Height,Lenght);

	return pObject;
}

PX_Object* PX_ObjectCreateRoot(px_memorypool* mp)
{
	return PX_ObjectCreate(mp, PX_NULL, 0, 0, 0, 0, 0, 0);
}

PX_Object* PX_ObjectCreateEx(px_memorypool* mp, PX_Object* Parent, px_float x, px_float y, px_float z, px_float Width, px_float Height, px_float Lenght, px_int type, Function_ObjectUpdate Func_ObjectUpdate, Function_ObjectRender Func_ObjectRender, Function_ObjectFree Func_ObjectFree, px_void* desc, px_int size)
{
	PX_Object* pObject = PX_ObjectCreate(mp, Parent, x, y, z, Width, Height, Lenght);
	if (pObject)
	{
		if (size)
		{
			if(PX_ObjectCreateDesc(pObject, 0, type, Func_ObjectUpdate, Func_ObjectRender, Func_ObjectFree, desc, size))
				return pObject;
			else
			{
				PX_ObjectDelete(pObject);
				return PX_NULL;
			}
		}
	}
	return PX_NULL;
}

px_void PX_ObjectDetach(PX_Object* pObject, px_int type)
{
	px_int i;
	PX_OBJECT_EventAction* pPre, * pCur;
	for (i = 0; i < PX_OBJECT_MAX_DESC_COUNT; i++)
	{
		if (pObject->Type[i] == type)
		{
			if (pObject->Func_ObjectFree[i])
			{
				pObject->Func_ObjectFree[i](pObject, i);
			}
			pObject->Type[i] = 0;
			if (pObject->pObjectDesc[i])
			{
				MP_Free(pObject->mp, pObject->pObjectDesc[i]);
				pObject->pObjectDesc[i] = PX_NULL;
			}
			pObject->Func_ObjectUpdate[i] = PX_NULL;
			pObject->Func_ObjectRender[i] = PX_NULL;
			pObject->Func_ObjectFree[i] = PX_NULL;
		}
	}
	//release event
	
	//PX_Object_Free Events linker
	pPre = PX_NULL;
	pCur = pObject->pEventActions;
	while (pCur)
	{
		if (pCur->owner == type)
		{
			if (pPre == PX_NULL)
			{
				pObject->pEventActions = pCur->pNext;
				MP_Free(pObject->mp, pCur);
				pCur = pObject->pEventActions;
				pPre = PX_NULL;
			}
			else
			{
				pPre->pNext = pCur->pNext;
				MP_Free(pObject->mp, pCur);
				pCur = pPre->pNext;
			}
		}
		else
		{
			pPre = pCur;
			pCur = pCur->pNext;
		}
	}
}


px_int PX_ObjectSetRenderFunction(PX_Object* pObject, Function_ObjectRender Func_ObjectRender, px_int index)
{
	pObject->Func_ObjectRender[index] = Func_ObjectRender;
	return index;
}


px_int PX_ObjectSetUpdateFunction(PX_Object* pObject, Function_ObjectUpdate Func_ObjectUpdate, px_int index)
{
	pObject->Func_ObjectUpdate[index] = Func_ObjectUpdate;
	return index;
}


px_int PX_ObjectSetFreeFunction(PX_Object* pObject, Function_ObjectFree Func_ObjectFree, px_int index)
{
	pObject->Func_ObjectFree[index] = Func_ObjectFree;
	return index;
}


px_void* PX_ObjectCreateDesc(PX_Object* pObject, px_int idesc, px_int type, Function_ObjectUpdate Func_ObjectUpdate, Function_ObjectRender Func_ObjectRender, Function_ObjectFree Func_ObjectFree, px_void* pDesc, px_int descSize)
{
	PX_ASSERTIF(idesc < 0 || idesc >= PX_OBJECT_MAX_DESC_COUNT || pObject->pObjectDesc[idesc] != PX_NULL);
	if (idesc >= 0 && idesc < PX_OBJECT_MAX_DESC_COUNT && pObject->pObjectDesc[idesc] == PX_NULL)
	{
		if (descSize)
		{
			pObject->pObjectDesc[idesc] = MP_Malloc(pObject->mp, descSize);
			if (!pObject->pObjectDesc[idesc])
			{
				return PX_NULL;
			}
			if (pDesc)
				PX_memcpy(pObject->pObjectDesc[idesc], pDesc, descSize);
			else
				PX_memset(pObject->pObjectDesc[idesc], 0, descSize);
		}
		else
		{
			pObject->pObjectDesc[idesc] = PX_NULL;
		}
		
		PX_ObjectSetRenderFunction(pObject, Func_ObjectRender, idesc);
		PX_ObjectSetUpdateFunction(pObject, Func_ObjectUpdate, idesc);
		PX_ObjectSetFreeFunction(pObject, Func_ObjectFree, idesc);
		pObject->Type[idesc]=type;
		return (px_void *)pObject->pObjectDesc[idesc];
	}
	return PX_NULL;
}


PX_Object* PX_ObjectCreateFunction(px_memorypool* mp, PX_Object* Parent,Function_ObjectUpdate Func_ObjectUpdate, Function_ObjectRender Func_ObjectRender, Function_ObjectFree Func_ObjectFree)
{
	PX_Object *pObject= PX_ObjectCreate(mp,Parent,0,0,0,0,0,0);
	if (pObject)
	{
		PX_ObjectSetRenderFunction(pObject, Func_ObjectRender, 0);
		PX_ObjectSetUpdateFunction(pObject, Func_ObjectUpdate, 0);
		PX_ObjectSetFreeFunction(pObject, Func_ObjectFree, 0);
	}
	return pObject;
}


px_void PX_ObjectGetInheritXY(PX_Object *pObject,px_float *x,px_float *y)
{
	*x=0;
	*y=0;
	pObject=pObject->pParent;
	while (pObject)
	{
		*x+=pObject->x;
		*y+=pObject->y;
		pObject=pObject->pParent;
	}
}

static px_void PX_Object_ObjectEventFree( PX_Object **pObject )
{
	PX_OBJECT_EventAction *pNext,*pCur;;
	//PX_Object_Free Events linker
	pCur=(*pObject)->pEventActions;
	while (pCur)
	{
		pNext=pCur->pNext;
		MP_Free((*pObject)->mp,pCur);
		pCur=pNext;
	}

	(*pObject)->pEventActions=PX_NULL;
}



static px_void PX_Object_ObjectFree( PX_Object *pObject )
{
	px_int i;
	PX_Object_ObjectEventFree(&pObject);
	
	for (i = 0; i < PX_OBJECT_MAX_DESC_COUNT; i++)
	{
		if (pObject->Func_ObjectFree[i])
		{
			pObject->Func_ObjectFree[i](pObject,i);
		}
	}

	for (i = 0; i < PX_OBJECT_MAX_DESC_COUNT; i++)
	{
		if (pObject->pObjectDesc[i])
		{
			MP_Free(pObject->mp, pObject->pObjectDesc[i]);
		}
	}

	MP_Free(pObject->mp,pObject);
}


static px_void PX_Object_DeleteLinkerObject( PX_Object **ppObject )
{   
	PX_Object *pObject=(*ppObject);

	if (pObject==PX_NULL)
	{
		return;
	}
	PX_Object_DeleteLinkerObject(&pObject->pNextBrother);
	PX_Object_DeleteLinkerObject(&pObject->pChilds);

	PX_Object_ObjectFree(pObject);

	(*ppObject)=PX_NULL;
}

px_void PX_ObjectDelete( PX_Object *pObject )
{
	if (pObject==PX_NULL) 
	{
		return;
	}
	if (pObject->OnFocus)
	{
		PX_ObjectClearFocus(pObject);
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
				PX_ERROR("Invalid pObject struct");
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


px_void PX_ObjectDelayDelete(PX_Object* pObject)
{
	pObject->delay_delete = PX_TRUE;
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

px_void PX_Object_ObjectLinkerUpdate( PX_Object *pObject,px_uint elapsed)
{
	if (pObject==PX_NULL)
	{
		return;
	}
	if (pObject->Enabled)
	{
		px_int i;
		for (i = 0; i < PX_OBJECT_MAX_DESC_COUNT; i++)
		{
			if (pObject->Func_ObjectUpdate[i])
			{
				pObject->Func_ObjectUpdate[i](pObject,i, elapsed);
			}
		}
		if (!pObject->delay_delete)
		{
			PX_Object_ObjectLinkerUpdate(pObject->pChilds, elapsed);
		}
		
	}
	PX_Object_ObjectLinkerUpdate(pObject->pNextBrother,elapsed);	
}


static px_void PX_ObjectUpdateDelayDelete(PX_Object* pObject)
{
	if (pObject==PX_NULL)
	{
		return;
	}
	PX_ObjectUpdateDelayDelete(pObject->pNextBrother);
	if (pObject->delay_delete)
	{
		PX_ObjectDelete(pObject);
	}
	else
	{
		PX_ObjectUpdateDelayDelete(pObject->pChilds);
	}
}

px_void PX_ObjectUpdatePhysics(PX_Object* pObject, px_uint elapsed)
{
	px_float vfx, vfy, vfz;
	if (pObject == PX_NULL)
	{
		PX_ASSERT();
		return;
	}
	if (pObject->Enabled == PX_FALSE)
	{
		return;
	}

	vfx = pObject->vx * pObject->vx * pObject->ak;
	vfy = pObject->vy * pObject->vy * pObject->ak;
	vfz = pObject->vz * pObject->vz * pObject->ak;

	if (PX_ABS(vfx * elapsed / 1000.f) > PX_ABS(pObject->vx))
	{
		pObject->vx = 0;
	}
	else
	{
		if (pObject->vx > 0)
			pObject->vx -= vfx * elapsed / 1000.f;
		else
			pObject->vx += vfx * elapsed / 1000.f;
	}

	if (PX_ABS(vfy * elapsed / 1000.f) > PX_ABS(pObject->vy))
	{
		pObject->vy = 0;
	}
	else
	{
		if (pObject->vy > 0)
			pObject->vy -= vfy * elapsed / 1000.f;
		else
			pObject->vy += vfy * elapsed / 1000.f;
	}

	if (PX_ABS(vfz * elapsed / 1000.f) > PX_ABS(pObject->vz))
	{
		pObject->vz = 0;
	}
	else
	{
		if (pObject->vz > 0)
			pObject->vz -= vfz * elapsed / 1000.f;
		else
			pObject->vz += vfz * elapsed / 1000.f;
	}


	pObject->vx += pObject->fx * elapsed / 1000.f;
	pObject->vy += pObject->fy * elapsed / 1000.f;
	pObject->vz += pObject->fz * elapsed / 1000.f;

	pObject->x += pObject->vx * elapsed / 1000.f;
	pObject->y += pObject->vy * elapsed / 1000.f;
	pObject->z += pObject->vz * elapsed / 1000.f;
}


px_void PX_ObjectUpdate(PX_Object *pObject,px_uint elapsed )
{
	px_int i;
	if (pObject==PX_NULL)
	{
		PX_ASSERT();
		return;
	}
	if (pObject->Enabled==PX_FALSE)
	{
		return;
	}

	for (i=0;i<PX_OBJECT_MAX_DESC_COUNT;i++)
	{
		if (pObject->Func_ObjectUpdate[i] != 0)
		{
			pObject->Func_ObjectUpdate[i](pObject,i, elapsed);
		}
	}
	
	if (pObject->pChilds!=PX_NULL&&!pObject->delay_delete)
	{
		PX_Object_ObjectLinkerUpdate(pObject->pChilds,elapsed);
	}

	if (pObject->delay_delete)
	{
		PX_ObjectDelete(pObject);
	}
	else
	{
		PX_ObjectUpdateDelayDelete(pObject->pChilds);
	}

}


static px_void PX_ObjectRenderEx(px_surface *pSurface, PX_Object *pObject,px_uint elapsed )
{
	if (pObject==PX_NULL)
	{
		return;
	}

	if (pObject->OnFocus)
	{
		PX_ObjectRenderEx(pSurface,pObject->pNextBrother,elapsed);	

		if (pObject->Visible!=PX_FALSE)
		{
			if (pObject->Width>=0&&pObject->Height>=0)
			{
				px_int i;
				for ( i = 0; i < PX_OBJECT_MAX_DESC_COUNT; i++)
				{
					if (pObject->Func_ObjectRender[i] != 0)
					{
						pObject->Func_ObjectRender[i](pSurface, pObject,i, elapsed);
					}
				}
			}
			
			PX_ObjectRenderEx(pSurface,pObject->pChilds,elapsed);
			
		}
	}
	else
	{
		if (pObject->Visible!=PX_FALSE)
		{
			if (pObject->Width >= 0 && pObject->Height >= 0)
			{
				px_int i;
				for (i = 0; i < PX_OBJECT_MAX_DESC_COUNT; i++)
				{
					if (pObject->Func_ObjectRender[i] != 0)
					{
						pObject->Func_ObjectRender[i](pSurface, pObject,i, elapsed);
					}
				}
			}
			PX_ObjectRenderEx(pSurface,pObject->pChilds,elapsed);		
		}
		PX_ObjectRenderEx(pSurface,pObject->pNextBrother,elapsed);	
	}
}

px_void PX_ObjectRender(px_surface *pSurface, PX_Object *pObject,px_uint elapsed )
{
	if (pObject==PX_NULL)
	{
		return;
	}

	if (pObject->OnFocus)
	{
		
		if (pObject->Visible!=PX_FALSE)
		{
			px_int i;
			for (i = 0; i < PX_OBJECT_MAX_DESC_COUNT; i++)
			{
				if (pObject->Func_ObjectRender[i] != 0)
				{
					pObject->Func_ObjectRender[i](pSurface, pObject,i, elapsed);
				}
			}
			if (!pObject->delay_delete)
			{
				PX_ObjectRenderEx(pSurface, pObject->pChilds, elapsed);
			}
		}
	}
	else
	{
		if (pObject->Visible!=PX_FALSE)
		{
			px_int i;
			for (i = 0; i < PX_OBJECT_MAX_DESC_COUNT; i++)
			{
				if (pObject->Func_ObjectRender[i] != 0)
				{
					pObject->Func_ObjectRender[i](pSurface, pObject,i, elapsed);
				}
			}
			if (!pObject->delay_delete)
			{
				PX_ObjectRenderEx(pSurface, pObject->pChilds, elapsed);
			}
		}
	}

	if (pObject->delay_delete)
	{
		PX_ObjectDelete(pObject);
	}
	else
	{
		PX_ObjectUpdateDelayDelete(pObject->pChilds);
	}
}

px_bool PX_ObjectIsPointInRegion( PX_Object *pObject,px_float x,px_float y )
{
	px_float objx,objy,objw,objh,r;
	px_float inheritX,inheritY;

	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objw=pObject->Width;
	objh=pObject->Height;
	r = pObject->diameter/2;
	if (r)
		return PX_isPoint2DInCircle(PX_POINT2D(x,y),PX_POINT2D(objx,objy),r);
	else
		return PX_isXYInRegion(x,y,objx,objy,objw,objh);
}

px_bool PX_ObjectIsCursorInRegion(PX_Object *pObject,PX_Object_Event e)
{
	return PX_ObjectIsPointInRegion(pObject,PX_Object_Event_GetCursorX(e),PX_Object_Event_GetCursorY(e));
}

px_float PX_ObjectGetHeight(PX_Object *pObject)
{
	return pObject->Height;
}

px_float PX_ObjectGetWidth(PX_Object *pObject)
{
	return pObject->Width;
}

px_void PX_ObjectAddChild(PX_Object *Parent,PX_Object *child)
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

px_void PX_ObjectInitialize(px_memorypool *mp,PX_Object *pObject,PX_Object *Parent,px_float x,px_float y,px_float z,px_float Width,px_float Height,px_float Lenght )
{
	PX_memset(pObject,0,sizeof(PX_Object));

	pObject->x=x;
	pObject->y=y;
	pObject->z=z;
	pObject->Width=Width;
	pObject->Height=Height;

	pObject->Enabled=PX_TRUE;
	pObject->Visible=PX_TRUE;
	pObject->ReceiveEvents=PX_TRUE;
	pObject->mp=mp;

	if (Parent!=PX_NULL)
	{
		if (Parent->Func_ObjectLinkChild!=PX_NULL)
		{
			Parent->Func_ObjectLinkChild(Parent,pObject);
		}
		else
		{
			PX_ObjectAddChild(Parent,pObject);
		}
		
	}
}


px_void PX_ObjectSetId(PX_Object *pObject,const px_char id[])
{
	PX_strcpy(pObject->id,id,sizeof(pObject->id)); 
}

px_void PX_ObjectSetVisible( PX_Object *pObject,px_bool visible )
{
	if(pObject!=PX_NULL)
	{
		if (visible==PX_FALSE&&pObject->OnFocus)
		{
			PX_ObjectClearFocus(pObject);
		}
		pObject->Visible=visible;
	}
}


px_void PX_ObjectSetEnabled(PX_Object *pObject,px_bool enabled)
{
	pObject->Enabled=enabled;
}

px_void PX_ObjectEnable(PX_Object *pObject)
{
	pObject->Enabled=PX_TRUE;
}

px_void PX_ObjectDisable(PX_Object *pObject)
{
	pObject->Enabled=PX_FALSE;
}

px_void PX_ObjectEnableEventAction(PX_Object* pObject, px_int ownerType)
{
		PX_OBJECT_EventAction* pPoint;
		pPoint = pObject->pEventActions;
		while (pPoint)
		{
			if (pPoint->owner == ownerType)
			{
				pPoint->enable = PX_TRUE;
			}
			pPoint = pPoint->pNext;
		}
}
px_void PX_ObjectDisableEventAction(PX_Object* pObject, px_int ownerType)
{
	PX_OBJECT_EventAction* pPoint;
	pPoint = pObject->pEventActions;
	while (pPoint)
	{
		if (pPoint->owner == ownerType)
		{
			pPoint->enable = PX_FALSE;
		}
		pPoint = pPoint->pNext;
	}
}
px_void PX_ObjectEnableNoTheEventAction(PX_Object* pObject, px_int ownerType)
{
	PX_OBJECT_EventAction* pPoint;
	pPoint = pObject->pEventActions;
	while (pPoint)
	{
		if (pPoint->owner != ownerType)
		{
			pPoint->enable = PX_TRUE;
		}
		pPoint = pPoint->pNext;
	}
}
px_void PX_ObjectDisableNoTheEventAction(PX_Object* pObject, px_int ownerType)
{
	PX_OBJECT_EventAction* pPoint;
	pPoint = pObject->pEventActions;
	while (pPoint)
	{
		if (pPoint->owner != ownerType)
		{
			pPoint->enable = PX_FALSE;
		}
		pPoint = pPoint->pNext;
	}
}

px_bool PX_ObjectRegisterEvent( PX_Object *pObject,px_uint Event,px_void (*ProcessFunc)(PX_Object *,PX_Object_Event e,px_void *user_ptr),px_void *user)
{
	return PX_ObjectRegisterEventEx(pObject, Event, pObject->Type[0], ProcessFunc, user);
}

px_bool PX_ObjectRegisterEventEx(PX_Object* pObject, px_uint Event,px_int owner, px_void(*ProcessFunc)(PX_Object*, PX_Object_Event e, px_void* user_ptr), px_void* user)
{
	PX_OBJECT_EventAction* pPoint;
	PX_OBJECT_EventAction* pAction = (PX_OBJECT_EventAction*)MP_Malloc(pObject->mp, sizeof(PX_OBJECT_EventAction));
	if (pAction == PX_NULL)
	{
		return PX_FALSE;
	}
	pAction->enable = PX_TRUE;
	pAction->owner= owner;
	pAction->pNext = PX_NULL;
	pAction->pPre = PX_NULL;
	pAction->EventAction = Event;
	pAction->EventActionFunc = ProcessFunc;

	pAction->user_ptr = user;

	pPoint = pObject->pEventActions;
	if (pPoint == PX_NULL)
	{
		pObject->pEventActions = pAction;
		return PX_TRUE;
	}

	while (pPoint->pNext)
	{
		pPoint = pPoint->pNext;
	}
	pAction->pPre = pPoint;
	pPoint->pNext = pAction;

	return PX_TRUE;
}

px_void PX_ObjectRemoveEvent(PX_Object* pObject, px_uint Event)
{
	PX_OBJECT_EventAction *pPoint;
	PX_OBJECT_EventAction *pAction;

	pPoint = pObject->pEventActions;
	while (pPoint)
	{
		pAction = pPoint;
		pPoint = pPoint->pNext;
		if (pAction->EventAction == Event)
		{
			if (pAction->pPre)
			{
				pAction->pPre->pNext = pAction->pNext;
			}
			else
			{
				pObject->pEventActions = pAction->pNext;
			}

			if (pAction->pNext)
			{
				pAction->pNext->pPre = pAction->pPre;
			}
			MP_Free(pObject->mp, pAction);
		}
	}
	
}


px_void PX_ObjectExecuteEvent(PX_Object *pPost,PX_Object_Event Event)
{
	PX_OBJECT_EventAction *EventAction;

	if (pPost->Visible==PX_FALSE||pPost->Enabled==PX_FALSE||pPost->ReceiveEvents==PX_FALSE)
	{
		return;
	}

	EventAction=pPost->pEventActions;
	while(EventAction)
	{
		if (EventAction->EventAction==PX_OBJECT_EVENT_ANY||EventAction->EventAction==Event.Event)
		{
			if(EventAction->enable)
				EventAction->EventActionFunc(pPost,Event,EventAction->user_ptr);
		}
		EventAction=EventAction->pNext;
	}
}

px_int PX_ObjectSearchRegion(PX_Object_CollisionTest* ptest, px_float x, px_float y, px_float raduis, PX_Object* pObject[], px_int MaxSearchCount, px_dword impact_test_type)
{
	px_int b;
	px_int m = 0;
	for (b = 0; b < sizeof(impact_test_type) * 8; b++)
	{
		if ((impact_test_type & (1 << b)))
		{
			px_int im_i;
			PX_Quadtree_UserData userData;
			userData.ptr = PX_NULL;
			PX_QuadtreeResetTest(&ptest->Impact_Test_array[b]);
			PX_QuadtreeTestNode(&ptest->Impact_Test_array[b], x, y, raduis * 2, raduis * 2, userData);

			for (im_i = 0; im_i < ptest->Impact_Test_array[b].Impacts.size; im_i++)
			{
				PX_Quadtree_UserData* puData = PX_VECTORAT(PX_Quadtree_UserData, &ptest->Impact_Test_array[b].Impacts, im_i);
				PX_Object* pimpactWo = (PX_Object*)puData->ptr;
				PX_Object* pObj2 = ((PX_Object*)(puData->ptr));

				if (pObj2->diameter)
				{
					if (!PX_isCircleCrossCircle(PX_POINT(x, y, 0), raduis, PX_POINT(pObj2->x, pObj2->y, 0), pObj2->diameter / 2))
						continue;
				}
				else
				{
					if (!PX_isRectCrossCircle(PX_RECT(pObj2->x - pObj2->Width / 2, pObj2->y - pObj2->Height / 2, pObj2->Width, pObj2->Height), PX_POINT(x, y, 0), raduis))
					{
						continue;
					}
				}

				
				if (m < MaxSearchCount)
				{
					pObject[m] = pObj2;
					m++;
				}
			}

		}
	}
	return m;
}

px_void PX_ObjectCollisionTestFree(PX_Object_CollisionTest* ptest)
{
	MP_Free(ptest->mp, ptest->calc_mp.StartAddr);
}

px_bool PX_ObjectCollisionTestCreate(px_memorypool* mp, PX_Object_CollisionTest* ptest,px_int width,px_int height, px_vector* pObjects)
{
	px_int allocSize = 1024 * 1024;
	px_byte* cache = MP_Malloc(mp, allocSize);
	px_memorypool testcalc;
	px_int i;
	if (cache)
	{
		px_int impact_count[32] = { 0 };
		PX_memset(&testcalc, 0, sizeof(testcalc));
		ptest->mp = mp;
		ptest->calc_mp = MP_Create(cache, allocSize);

		for (i = 0; i < pObjects->size; i++)
		{
			PX_Object *pTestObject = *PX_VECTORAT(PX_Object*, pObjects, i);
			if (!pTestObject->delay_delete)
			{
				continue;
			}
			if (pTestObject->impact_object_type)
			{
				px_dword j = 0;
				while (!(pTestObject->impact_object_type & (1 << j)))
					j++;
				impact_count[j]++;
			}
		}

		for (i = 0; i < sizeof(px_dword) * 8; i++)
		{
			if (impact_count[i])
			{
				if(impact_count[i]<=64)
					PX_QuadtreeCreate(&ptest->calc_mp, &ptest->Impact_Test_array[i], 0, 0, (px_float)width, (px_float)height, impact_count[i], 1);
				else if(impact_count[i] <= 256)
					PX_QuadtreeCreate(&ptest->calc_mp, &ptest->Impact_Test_array[i], 0, 0, (px_float)width, (px_float)height, impact_count[i], 2);
				else
					PX_QuadtreeCreate(&ptest->calc_mp, &ptest->Impact_Test_array[i], 0, 0, (px_float)width, (px_float)height, impact_count[i], 3);
			}
			else
			{
				PX_QuadtreeCreate(&ptest->calc_mp, &ptest->Impact_Test_array[i], 0, 0, (px_float)width, (px_float)height, impact_count[i], 0);
			}
		}

		for (i = 0; i < pObjects->size; i++)
		{
			PX_Object* pTestObject = *PX_VECTORAT(PX_Object*, pObjects, i);

			if (!pTestObject->delay_delete)
			{
				continue;
			}
			if (pTestObject->impact_object_type)
			{
				px_int b;
				for (b = 0; b < sizeof(px_dword) * 8; b++)
				{
					if ((pTestObject->impact_object_type & (1 << b)))
					{
						PX_Quadtree_UserData userData;
						userData.ptr = pTestObject;
						if ((px_float)pTestObject->diameter)
						{
							PX_QuadtreeAddNode(&ptest->Impact_Test_array[b], (px_float)pTestObject->x, (px_float)pTestObject->y, (px_float)pTestObject->diameter, (px_float)pTestObject->diameter, userData);
						}
						else
						{
							PX_QuadtreeAddNode(&ptest->Impact_Test_array[b], (px_float)pTestObject->x, (px_float)pTestObject->y, (px_float)pTestObject->Width, (px_float)pTestObject->Height, userData);
						}

					}
				}
			}
		}

		for (i = 0; i < pObjects->size; i++)
		{
			PX_Object* pTestObject = *PX_VECTORAT(PX_Object*, pObjects, i);
			if (!pTestObject->delay_delete)
			{
				continue;
			}

			if (pTestObject->impact_target_type)
			{
				px_int b;
				for (b = 0; b < sizeof(pTestObject->impact_target_type) * 8; b++)
				{
					if ((pTestObject->impact_target_type & (1 << b)))
					{
						px_int im_i;
						PX_Quadtree_UserData userData;
						userData.ptr = pTestObject;
						PX_QuadtreeResetTest(&ptest->Impact_Test_array[b]);
						if (pTestObject->diameter)
						{
							PX_QuadtreeTestNode(&ptest->Impact_Test_array[b], (px_float)pTestObject->x, (px_float)pTestObject->y, (px_float)pTestObject->diameter, (px_float)pTestObject->diameter, userData);
						}
						else
						{
							PX_QuadtreeTestNode(&ptest->Impact_Test_array[b], (px_float)pTestObject->x, (px_float)pTestObject->y, (px_float)pTestObject->Width, (px_float)pTestObject->Height, userData);
						}

						for (im_i = 0; im_i < ptest->Impact_Test_array[b].Impacts.size; im_i++)
						{
							PX_Quadtree_UserData* puData = PX_VECTORAT(PX_Quadtree_UserData, &ptest->Impact_Test_array[b].Impacts, im_i);
							PX_Object* pObj1 = pTestObject;
							PX_Object* pObj2 = (PX_Object*)puData->ptr;

							if (pObj1->diameter && pObj2->diameter)
							{
								if (!PX_isCircleCrossCircle(PX_POINT(pObj1->x, pObj1->y, 0), pObj1->diameter / 2, PX_POINT(pObj2->x, pObj2->y, 0), pObj2->diameter / 2))
									continue;
							}
							else if (pObj1->diameter == 0 && pObj2->diameter)
							{
								if (!PX_isRectCrossCircle(PX_RECT(pObj1->x - pObj1->Width / 2, pObj1->y - pObj1->Height / 2, pObj1->Width, pObj1->Height), PX_POINT(pObj2->x, pObj2->y, 0), pObj2->diameter / 2))
								{
									continue;
								}
							}
							else if (pObj1->diameter && pObj2->diameter == 0)
							{
								if (!PX_isRectCrossCircle(PX_RECT(pObj2->x - pObj2->Width / 2, pObj2->y - pObj2->Height / 2, pObj2->Width, pObj2->Height), PX_POINT(pObj1->x, pObj1->y, 0), pObj1->diameter / 2))
								{
									continue;
								}
							}
							do
							{
								PX_Object_Event e = {0};
								e.Event = PX_OBJECT_EVENT_IMPACT;
								PX_Object_Event_SetImpactTargetObject(&e, pObj2);
								PX_ObjectExecuteEvent(pObj1, e);
							} while (0);
						}

					}
				}
			}
		}

		return PX_TRUE;
	}
	return PX_FALSE;
}


px_bool PX_ObjectPostEventLink( PX_Object *pPost,PX_Object_Event Event )
{
	if (pPost==PX_NULL)
	{
		return PX_TRUE;
	}

	if (pPost->Visible==PX_FALSE||pPost->Enabled==PX_FALSE||pPost->ReceiveEvents==PX_FALSE)
	{
		if(PX_ObjectPostEventLink(pPost->pNextBrother,Event)==PX_FALSE) return PX_FALSE;
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
		if (pPost->OnFocus)
		{
			return PX_FALSE;
		}
		return PX_TRUE;
	}
	
}

px_void PX_ObjectPostEvent( PX_Object *pPost,PX_Object_Event Event )
{

	if (pPost==PX_NULL)
	{
		return;
	}

	if (pPost->Visible==PX_FALSE||pPost->Enabled==PX_FALSE||pPost->ReceiveEvents==PX_FALSE)
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


px_void PX_ObjectSetPosition( PX_Object *pObject,px_float x,px_float y,px_float z)
{
	PX_ASSERTIF(pObject == PX_NULL);

	pObject->x=x;
	pObject->y=y;
	pObject->z=z;
}

px_void	  PX_ObjectSetForce(PX_Object* pObject, px_float x, px_float y, px_float z)
{
	PX_ASSERTIF(pObject == PX_NULL);
	pObject->fx = x;
	pObject->fy = y;
	pObject->fz = z;
}
px_void	  PX_ObjectSetVelocity(PX_Object* pObject, px_float x, px_float y, px_float z)
{
	PX_ASSERTIF(pObject == PX_NULL);
	pObject->vx = x;
	pObject->vy = y;
	pObject->vz = z;
}
px_void	  PX_ObjectSetResistance(PX_Object* pObject, px_float ak)
{
	pObject->ak = ak;
}

px_void PX_ObjectSetSize( PX_Object *pObject,px_float Width,px_float Height,px_float length)
{
	if (pObject!=PX_NULL)
	{
		pObject->Width=Width;
		pObject->Height=Height;

	}
	else
	{
		PX_ASSERT();
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

px_void PX_ObjectSetParent(PX_Object* pObject, PX_Object* pParent)
{
	if (pObject == PX_NULL) return;
	if (pObject->pParent == pParent) return;
	if (pObject->OnFocus) PX_ObjectClearFocus(pObject);

	// detach from parent
	if (pObject->pParent != PX_NULL)
	{
		if (pObject->pParent->pChilds == pObject)
		{
			pObject->pParent->pChilds = pObject->pNextBrother;
			if (pObject->pNextBrother)
			{
				pObject->pNextBrother->pParent = pObject->pParent;
				pObject->pNextBrother->pPreBrother = PX_NULL;
			}
		}
		else
		{
			if (pObject->pPreBrother != PX_NULL)
			{
				pObject->pPreBrother->pNextBrother = pObject->pNextBrother;
				if (pObject->pNextBrother)
					pObject->pNextBrother->pPreBrother = pObject->pPreBrother;
			}
			else
			{
				PX_ERROR("Invalid pObject struct");
			}
		}
	}
	else
	{
		if (pObject->pPreBrother != PX_NULL)
		{
			pObject->pPreBrother = pObject->pNextBrother;
		}
	}

	// if the parent is null, then the object is a root object
	if (pParent == PX_NULL) {
		pObject->pParent = PX_NULL;
		pObject->pNextBrother = PX_NULL;
		pObject->pPreBrother = PX_NULL;
	}
	else {
		PX_ObjectAddChild(pParent, pObject);
	}
}
