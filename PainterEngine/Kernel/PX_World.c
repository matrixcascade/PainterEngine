#include "PX_World.h"


px_bool PX_WorldInit(px_memorypool *mp,PX_World *World,px_int world_width,px_int world_height,px_int surface_width,px_int surface_height)
{
	if(!PX_VectorInit(mp,&World->pObjects,sizeof(PX_WorldObject),128)) return PX_FALSE;
	if(!PX_MapInit(mp,&World->ObjectLibraryMap)) return PX_FALSE;
	PX_ListInit(mp,&World->ObjectLibrary);
	World->mp=mp;
	World->world_height=world_height;
	World->world_width=world_width;
	World->surface_height=surface_height;
	World->surface_width=surface_width;
	World->auxiliaryline=PX_TRUE;
	World->auxiliaryline_color=PX_COLOR(255,255,192,255);
	World->camera_offset=PX_POINT(0,0,0);
	World->aliveCount=0;
	World->offsetx=0;
	World->offsety=0;
	PX_WorldSetAuxiliaryXYSpacer(World,32,32);
	return PX_TRUE;
}

px_void PX_WorldSetAuxiliaryXYSpacer(PX_World *pw,px_int x,px_int y)
{
	pw->auxiliaryXSpacer=x;
	pw->auxiliaryYSpacer=y;
}

px_void PX_WorldEnableAuxiliaryLine(PX_World *pw,px_bool bline)
{
	pw->auxiliaryline=bline;
}


px_int PX_WorldGetCount(PX_World *World)
{
	return World->pObjects.size;
}

px_void PX_WorldUpdate( PX_World *world,px_memorypool *calcmp,px_uint elpased )
{
	px_int i,b,j;
	PX_Object_Event e;
	PX_WorldObject *pwo;
	PX_Quadtree Impact_Test_array[sizeof(pwo->pObject->impact_type)*8];
	px_int impact_count[sizeof(pwo->pObject->impact_type)*8]={0};
	PX_Quadtree_AABB_ImpactInfo *pImpact;
	if (world==PX_NULL)
	{
		return;
	}
	for (i=0;i<world->pObjects.size;i++)
	{
		pwo=PX_VECTORAT(PX_WorldObject,&world->pObjects,i);
		if (!pwo->pObject)
		{
			continue;
		}
		PX_ObjectUpdate(pwo->pObject,elpased);
	}

	//////////////////////////////////////////////////////////////////////////
	//impact test
	//////////////////////////////////////////////////////////////////////////
	for (i=0;i<world->pObjects.size;i++)
	{
		pwo=PX_VECTORAT(PX_WorldObject,&world->pObjects,i);
		if (!pwo->pObject)
		{
			continue;
		}
		if ((pwo->pObject->impact_type&(1<<i)))
		{
			impact_count[i]++;
		}
	}


	for (i=0;i<sizeof(pwo->pObject->impact_type)*8;i++)
		PX_QuadtreeCreate(calcmp,&Impact_Test_array[i],0,0,(px_float)world->world_width,(px_float)world->world_height,impact_count[i],2);

	for (i=0;i<world->pObjects.size;i++)
	{
		pwo=PX_VECTORAT(PX_WorldObject,&world->pObjects,i);
		if (!pwo->pObject)
		{
			continue;
		}

		for (b=0;b<sizeof(pwo->pObject->impact_type)*8;b++)
		{
			if ((pwo->pObject->impact_type&(1<<b))&&((pwo->pObject->impact_test_type&(1<<b))))
			{
				PX_QuadtreeAddAndTestNode(&Impact_Test_array[b],(px_float)pwo->pObject->x,(px_float)pwo->pObject->y,(px_float)pwo->pObject->z,(px_float)pwo->pObject->Width,(px_float)pwo->pObject->Height,(px_float)pwo->pObject->Length,i);
			}
			else if ((pwo->pObject->impact_type&(1<<b)))
			{
				PX_QuadtreeAddNode(&Impact_Test_array[b],(px_float)pwo->pObject->x,(px_float)pwo->pObject->y,(px_float)pwo->pObject->z,(px_float)pwo->pObject->Width,(px_float)pwo->pObject->Height,(px_float)pwo->pObject->Length,i);
			}
			else if((pwo->pObject->impact_test_type&(1<<b)))
			{
				PX_QuadtreeTestNode(&Impact_Test_array[b],(px_float)pwo->pObject->x,(px_float)pwo->pObject->y,(px_float)pwo->pObject->z,(px_float)pwo->pObject->Width,(px_float)pwo->pObject->Height,(px_float)pwo->pObject->Length,i);
			}
		}
	}

	for (b=0;b<sizeof(pwo->pObject->impact_type)*8;b++)
	{
		for (j=0;j<Impact_Test_array[b].Impacts.size;j++)
		{
			pImpact=PX_VECTORAT(PX_Quadtree_AABB_ImpactInfo,&Impact_Test_array[b].Impacts,j);
			e.Event=PX_OBJECT_EVENT_IMPACT;
			e.Param_int[0]=pImpact->box1Index;
			e.Param_int[1]=pImpact->box2Index;
			pwo=PX_VECTORAT(PX_WorldObject,&world->pObjects,pImpact->box1Index);
			PX_ObjectPostEvent(pwo->pObject,e);

			e.Param_int[0]=pImpact->box2Index;
			e.Param_int[1]=pImpact->box1Index;
			pwo=PX_VECTORAT(PX_WorldObject,&world->pObjects,pImpact->box2Index);
			PX_ObjectPostEvent(pwo->pObject,e);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	//delete death objects
	for (i=0;i<world->pObjects.size;i++)
	{
		pwo=PX_VECTORAT(PX_WorldObject,&world->pObjects,i);
		if (pwo->pObject&&pwo->DeleteMark)
		{
			PX_WorldRemoveObjectEx(world,i);
		}
	}

}

px_void PX_WorldUpdateOffset(PX_World *pw)
{
	px_int surface_width,surface_height;
	px_int LeftTopX,LeftTopY;

	surface_width=pw->surface_width;
	surface_height=pw->surface_height;
	LeftTopX=(px_int)(pw->camera_offset.x-surface_width/2),LeftTopY=(px_int)(pw->camera_offset.y-surface_height/2);

	if (LeftTopX+surface_width>pw->world_width)
	{
		LeftTopX=pw->world_width-surface_width;
	}

	if (LeftTopY+surface_height>pw->world_height)
	{
		LeftTopY=pw->world_height-surface_height;
	}

	if (LeftTopX<0)
	{
		LeftTopX=0;
	}
	if (LeftTopY<0)
	{
		LeftTopY=0;
	}

	pw->offsetx=LeftTopX;
	pw->offsety=LeftTopY;
}

px_void PX_WorldRender(px_surface *psurface,px_memorypool *calcmp,PX_World *pw,px_uint elpased)
{
	px_int i;
	px_int surface_width,surface_height;
	PX_QuickSortAtom *ArrayIndex;
	
	px_int sx,sy;
	PX_WorldObject *pwo;

	if (pw==PX_NULL)
	{
		return;
	}

	PX_WorldUpdateOffset(pw);

	surface_width=pw->surface_width;
	surface_height=pw->surface_height;


	if (pw->auxiliaryline)
	{
		for (sy=pw->auxiliaryYSpacer-(pw->offsety%pw->auxiliaryYSpacer);sy<surface_height;sy+=pw->auxiliaryYSpacer)
		{
			PX_GeoDrawLine(psurface,0,sy,surface_width-1,sy,1,pw->auxiliaryline_color);
		}

		for (sx=pw->auxiliaryXSpacer-(pw->offsetx%pw->auxiliaryXSpacer);sx<surface_width;sx+=pw->auxiliaryXSpacer)
		{
			PX_GeoDrawLine(psurface,sx,0,sx,surface_height-1,1,pw->auxiliaryline_color);
		}

	}


	MP_Reset(calcmp);
	ArrayIndex=(PX_QuickSortAtom *)MP_Malloc(calcmp,pw->aliveCount*sizeof(PX_QuickSortAtom));


	for (i=0;i<pw->aliveCount;i++)
	{
		pwo=PX_VECTORAT(PX_WorldObject,&pw->pObjects,i);
		if (!pwo->pObject)
		{
			continue;
		}
		ArrayIndex[i].power=pwo->pObject->z;
		ArrayIndex[i].pData=pwo->pObject;
	}

	PX_Quicksort_MaxToMin(ArrayIndex,0,pw->aliveCount-1);



	for (i=0;i<pw->aliveCount;i++)
	{
		((PX_Object *)(ArrayIndex[i].pData))->x-=(px_float)pw->offsetx;
		((PX_Object *)(ArrayIndex[i].pData))->y-=(px_float)pw->offsety;
		PX_ObjectRender(psurface,(PX_Object *)ArrayIndex[i].pData,elpased);
		((PX_Object *)(ArrayIndex[i].pData))->x+=(px_float)pw->offsetx;
		((PX_Object *)(ArrayIndex[i].pData))->y+=(px_float)pw->offsety;
	}
	MP_Reset(calcmp);
}

px_void PX_WorldSetImpact(PX_Object *pObj,px_dword type,px_dword Intersect)
{
	pObj->impact_type=type;
	pObj->impact_test_type=Intersect;
}

px_bool PX_WorldAddObject(PX_World *World,PX_Object *pObject)
{
	PX_WorldObject *pwo,wo;
	int i;
	wo.pObject=pObject;
	wo.DeleteMark=PX_FALSE;

	for (i=0;i<World->pObjects.size;i++)
	{
		pwo=PX_VECTORAT(PX_WorldObject,&World->pObjects,i);
		if (!pwo->pObject)
		{
			*pwo=wo;
			World->aliveCount++;
			return PX_TRUE;
		}
	}
	if (PX_VectorPushback(&World->pObjects,&wo))
	{
		World->aliveCount++;
		return PX_TRUE;
	}
	return PX_FALSE;
}

px_void PX_WorldRemoveObjectEx(PX_World *world,px_int i_index)
{
	PX_WorldObject *pwo=PX_VECTORAT(PX_WorldObject,&world->pObjects,i_index);
	if (pwo->pObject)
	{
		pwo->DeleteMark=PX_FALSE;
		world->aliveCount--;
		PX_ObjectDelete(pwo->pObject);
		pwo->pObject=PX_NULL;
	}
}

px_void PX_WorldSetAuxiliaryLineColor(PX_World *pw,px_color color)
{
	pw->auxiliaryline_color=color;
}

PX_Object* PX_WorldCreateObject(PX_World *world,px_char key[],px_float x,px_float y,px_float z,px_float width,px_float height,px_float length,px_void *user)
{
	PX_WorldObjectType *pt;
	PX_Object *pObject;
	pt=(PX_WorldObjectType *)PX_MapGet(&world->ObjectLibraryMap,key);
	if (pt)
	{
		pObject=pt->func(world,x,y,z,width,height,length,user);
		if (pObject)
		{
		   if(PX_WorldAddObject(world,pObject))
		   {
			   return pObject;
		   }
		   else
		   {
			   PX_ObjectDelete(pObject);
			   return PX_NULL;
		   }
		}
	}
	return PX_NULL;
}

px_bool PX_WorldRegistryObjectType(PX_World *pw,const px_char name[],px_world_func_CreateObject pcreate_func)
{
	PX_WorldObjectType t;
	px_void *p;
	t.name=name;
	t.func=pcreate_func;
	p=PX_ListPush(&pw->ObjectLibrary,&t,sizeof(t));
	if (!p)
	{
		return PX_FALSE;
	}
	if(PX_MapPut(&pw->ObjectLibraryMap,(px_char *)name,pw->ObjectLibrary.end->pdata)==PX_HASHMAP_RETURN_OK)
		return PX_TRUE;
	return PX_FALSE;
}

px_void PX_WorldFree(PX_World *pw)
{
	PX_WorldObject *pwo;
	int i;
	for (i=0;i<pw->pObjects.size;i++)
	{
		pwo=PX_VECTORAT(PX_WorldObject,&pw->pObjects,i);
		if (pwo->pObject)
		{
			PX_ObjectDelete(pwo->pObject);
		}
	}
	PX_VectorFree(&pw->pObjects);
	PX_MapFree(&pw->ObjectLibraryMap);
}

px_void PX_WorldSetCamera(PX_World *World,px_point camera_center_point)
{
	World->camera_offset=camera_center_point;
	PX_WorldUpdateOffset(World);
}

px_void PX_WorldRemoveObject(PX_World *world,px_int i_index)
{
	PX_WorldObject *pwo=PX_VECTORAT(PX_WorldObject,&world->pObjects,i_index);

#ifdef PX_DEBUG_MODE
	if (i_index>world->pObjects.size)
	{
		PX_ASSERT();
	}
#endif
	
	if (pwo->pObject)
	{
		pwo->DeleteMark=PX_TRUE;
	}
}

