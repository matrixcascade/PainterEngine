#include "PX_World.h"


px_bool PX_WorldInit(px_memorypool *mp,PX_World *World,px_int world_width,px_int world_height,px_int surface_width,px_int surface_height,px_dword calcSize)
{
	px_void *ptr;
	if(!PX_VectorInitialize(mp,&World->pObjects,sizeof(PX_WorldObject),256)) return PX_FALSE;
	if(!PX_VectorInitialize(mp,&World->pNewObjects,sizeof(PX_Object *),128)) return PX_FALSE;
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
	ptr=MP_Malloc(World->mp,calcSize);
	if (!ptr)
	{
		return PX_FALSE;
	}
	World->mp_WorldCalc=MP_Create(ptr,calcSize);
	PX_memset(World->Impact_Test_array,0,sizeof(World->Impact_Test_array));
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

px_int PX_WorldAddObjectEx(PX_World *World,PX_Object *pObject)
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
			return i;
		}
	}
	if (PX_VectorPushback(&World->pObjects,&wo))
	{
		World->aliveCount++;
		return World->pObjects.size-1;
	}
	return -1;
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

px_void PX_WorldUpdate( PX_World *world,px_uint elpased )
{
	px_int updateCount;
	px_int i,b,j,k;
	px_float w_left=0,w_top=0,w_right=0,w_height=0;
	PX_Object_Event e;
	PX_WorldObject *pwo;
	
	px_int impact_count[sizeof(pwo->pObject->impact_Object_type)*8]={0};
	px_memorypool *calcmp=&world->mp_WorldCalc;

	if (world==PX_NULL)
	{
		return;
	}

	MP_Reset(calcmp);

	//Add NewObjects
	k=0;
	for (i=0;i<world->pNewObjects.size;i++)
	{
		PX_WorldObject *pwo,wo;

		wo.pObject=*PX_VECTORAT(PX_Object *,&world->pNewObjects,i);
		wo.DeleteMark=PX_FALSE;

		for (j=k;j<world->pObjects.size;j++)
		{
			pwo=PX_VECTORAT(PX_WorldObject,&world->pObjects,j);
			if (!pwo->pObject)
			{
				wo.pObject->world_index=j;
				*pwo=wo;
				world->aliveCount++;
				k=j+1;
				goto NEW_OBJECT_CONTINUE;
			}
		}

		if (PX_VectorPushback(&world->pObjects,&wo))
		{
			world->aliveCount++;
			wo.pObject->world_index=world->pObjects.size-1;
			k=wo.pObject->world_index+1;
		}
NEW_OBJECT_CONTINUE:
		continue;
	}
	PX_VectorClear(&world->pNewObjects);

	//Object Counts
	updateCount=world->pObjects.size;

	//////////////////////////////////////////////////////////////////////////
	//impact test
	//////////////////////////////////////////////////////////////////////////
	for (i=0;i<updateCount;i++)
	{
		pwo=PX_VECTORAT(PX_WorldObject,&world->pObjects,i);
		if (!pwo->pObject)
		{
			continue;
		}
		if (pwo->pObject->impact_Object_type)
		{
			j=0;
			while(!(pwo->pObject->impact_Object_type&(1<<j)))
				j++;

			impact_count[j]++;
		}
	}


	for (i=0;i<sizeof(pwo->pObject->impact_Object_type)*8;i++)
	{
		if (impact_count[i])
		{
			PX_QuadtreeCreate(calcmp,&world->Impact_Test_array[i],0,0,(px_float)world->world_width,(px_float)world->world_height,impact_count[i],2);
		}
		else
		{
			PX_QuadtreeCreate(calcmp,&world->Impact_Test_array[i],0,0,(px_float)world->world_width,(px_float)world->world_height,impact_count[i],0);
		}
	}

	for (i=0;i<updateCount;i++)
	{
		pwo=PX_VECTORAT(PX_WorldObject,&world->pObjects,i);
		if (!pwo->pObject)
		{
			continue;
		}
		if (pwo->pObject->impact_Object_type)
		{
			for (b=0;b<sizeof(pwo->pObject->impact_Object_type)*8;b++)
			{
				if ((pwo->pObject->impact_Object_type&(1<<b)))
				{
					PX_Quadtree_UserData userData;
					userData.ptr=pwo;
					PX_QuadtreeAddNode(&world->Impact_Test_array[b],(px_float)pwo->pObject->x,(px_float)pwo->pObject->y,(px_float)pwo->pObject->Width,(px_float)pwo->pObject->Height,userData);
				}
			}
		}
	}

	for (i=0;i<updateCount;i++)
	{
		pwo=PX_VECTORAT(PX_WorldObject,&world->pObjects,i);
		if (!pwo->pObject)
		{
			continue;
		}
		
		if (pwo->pObject->impact_test_type)
		{
			for (b=0;b<sizeof(pwo->pObject->impact_test_type)*8;b++)
			{
				if ((pwo->pObject->impact_test_type&(1<<b)))
				{
					px_int im_i;
					PX_Quadtree_UserData userData;
					userData.ptr=pwo;
					PX_QuadtreeResetTest(&world->Impact_Test_array[b]);
					PX_QuadtreeTestNode(&world->Impact_Test_array[b],(px_float)pwo->pObject->x,(px_float)pwo->pObject->y,(px_float)pwo->pObject->Width,(px_float)pwo->pObject->Height,userData);
					for (im_i=0;im_i<world->Impact_Test_array[b].Impacts.size;im_i++)
					{
						PX_Quadtree_UserData *puData=PX_VECTORAT(PX_Quadtree_UserData,&world->Impact_Test_array[b].Impacts,im_i);
						PX_Object *pObj1=pwo->pObject;
						PX_WorldObject *pimpactWo=(PX_WorldObject *)puData->ptr;
						PX_Object *pObj2=((PX_WorldObject *)(puData->ptr))->pObject;
						
						if (pObj1->diameter&&pObj2->diameter)
						{
							if(!PX_isCircleCrossCircle(PX_POINT(pObj1->x,pObj1->y,0),pObj1->diameter/2,PX_POINT(pObj2->x,pObj2->y,0),pObj2->diameter/2))
								continue;
						}
						else if (pObj1->diameter==0&&pObj2->diameter)
						{
							if (!PX_isRectCrossCircle(PX_RECT(pObj1->x-pObj1->Width/2,pObj1->y-pObj1->Height/2,pObj1->Width,pObj1->Height),PX_POINT(pObj2->x,pObj2->y,0),pObj2->diameter/2))
							{
								continue;
							}
						}
						else if (pObj1->diameter&&pObj2->diameter==0)
						{
							if (!PX_isRectCrossCircle(PX_RECT(pObj2->x-pObj2->Width/2,pObj2->y-pObj2->Height/2,pObj2->Width,pObj2->Height),PX_POINT(pObj1->x,pObj1->y,0),pObj1->diameter/2))
							{
								continue;
							}
						}
						if (pimpactWo->DeleteMark!=PX_TRUE)
						{
							e.Event=PX_OBJECT_EVENT_IMPACT;
							e.Param_ptr[0]=pObj2;
							PX_ObjectPostEvent(pObj1,e);
						}
					}

				}
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	//Update Objects
	

	for (i=0;i<updateCount;i++)
	{
		pwo=PX_VECTORAT(PX_WorldObject,&world->pObjects,i);
		if (!pwo->pObject)
		{
			continue;
		}
		PX_ObjectUpdate(pwo->pObject,elpased);
	}

	//////////////////////////////////////////////////////////////////////////
	//delete death objects
	for (i=0;i<updateCount;i++)
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

	pw->offsetx=(px_float)LeftTopX;
	pw->offsety=(px_float)LeftTopY;
}

static px_void PX_WorldClildRender(px_surface *pSurface, PX_Object *Object,px_uint elpased,px_float oftX,px_float oftY)
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
		px_float x=Object->x;
		px_float y=Object->y;
		Object->x-=oftX;
		Object->y-=oftY;
		Object->Func_ObjectRender(pSurface,Object,elpased);
		Object->x=x;
		Object->y=y;
	}
	if (Object->pNextBrother!=PX_NULL)
	{
		PX_WorldClildRender(pSurface,Object->pNextBrother,elpased,oftX,oftY);
	}
	if (Object->pChilds!=PX_NULL)
	{
		PX_WorldClildRender(pSurface,Object->pNextBrother,elpased,oftX,oftY);
	}
}



px_void PX_WorldRender(px_surface *psurface,PX_World *pw,px_uint elpased)
{
	px_int i,j;
	px_int surface_width,surface_height;
	PX_QuickSortAtom *ArrayIndex;
	px_memorypool *calcmp;
	px_int sx,sy;
	PX_WorldObject *pwo;

	if (pw==PX_NULL)
	{
		return;
	}

	calcmp=&pw->mp_WorldCalc;
	MP_Reset(calcmp);
	
	
	PX_WorldUpdateOffset(pw);

	surface_width=pw->surface_width;
	surface_height=pw->surface_height;


	if (pw->auxiliaryline)
	{
		for (sy=pw->auxiliaryYSpacer-((px_int)pw->offsety%pw->auxiliaryYSpacer);sy<surface_height;sy+=pw->auxiliaryYSpacer)
		{
			PX_GeoDrawLine(psurface,0,sy,surface_width-1,sy,1,pw->auxiliaryline_color);
		}

		for (sx=pw->auxiliaryXSpacer-((px_int)pw->offsetx%pw->auxiliaryXSpacer);sx<surface_width;sx+=pw->auxiliaryXSpacer)
		{
			PX_GeoDrawLine(psurface,sx,0,sx,surface_height-1,1,pw->auxiliaryline_color);
		}

	}

	ArrayIndex=(PX_QuickSortAtom *)MP_Malloc(calcmp,pw->aliveCount*sizeof(PX_QuickSortAtom));
	j=0;

	for (i=0;i<pw->pObjects.size;i++)
	{
		pwo=PX_VECTORAT(PX_WorldObject,&pw->pObjects,i);
		if (!pwo->pObject)
		{
			continue;
		}
		ArrayIndex[j].weight=pwo->pObject->z;
		ArrayIndex[j].pData=pwo->pObject;
		j++;
	}

 	PX_Quicksort_MaxToMin(ArrayIndex,0,pw->aliveCount-1);



	for (i=0;i<pw->aliveCount;i++)
	{
		PX_WorldClildRender(psurface,(PX_Object *)ArrayIndex[i].pData,elpased,pw->offsetx,pw->offsety);
	}
	MP_Reset(calcmp);
}

px_void PX_WorldSetImpact(PX_Object *pObj,px_dword type,px_dword Intersect)
{
	pObj->impact_Object_type=type;
	pObj->impact_test_type=Intersect;
}



px_bool PX_WorldAddObject(PX_World *World,PX_Object *pObject)
{
	return PX_VectorPushback(&World->pNewObjects,&pObject);
}


px_void PX_WorldSetAuxiliaryLineColor(PX_World *pw,px_color color)
{
	pw->auxiliaryline_color=color;
}



px_point PX_WolrdObjectXYtoScreenXY(PX_World *pw,px_float x,px_float y)
{
	return PX_POINT(x-pw->offsetx,y-pw->offsety,0);
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
	PX_VectorFree(&pw->pNewObjects);

	MP_Free(pw->mp,pw->mp_WorldCalc.StartAddr);

}

px_void PX_WorldSetCamera(PX_World *World,px_point camera_center_point)
{
	World->camera_offset=camera_center_point;
	PX_WorldUpdateOffset(World);
}

px_void PX_WorldRemoveObject(PX_World *world,PX_Object *pObject)
{
	PX_WorldObject *pwo;
	if (pObject->world_index!=-1&&pObject->world_index<world->pObjects.size)
	{
		pwo=PX_VECTORAT(PX_WorldObject,&world->pObjects,pObject->world_index);
		if (pwo->pObject==pObject)
		{
			pwo->DeleteMark=PX_TRUE;

			return;
		}
	}
}

px_void PX_WorldRemoveObjectByIndex(PX_World *world,px_int i_index)
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

_LIMIT px_int PX_WorldSearchRegion(PX_World *world,px_float x,px_float y,px_float raduis,PX_Object *Object[],px_int MaxSearchCount,px_dword impact_test_type)
{
	px_int b;
	px_int m=0;
	for (b=0;b<sizeof(impact_test_type)*8;b++)
	{
		if ((impact_test_type&(1<<b)))
		{
			px_int im_i;
			PX_Quadtree_UserData userData;
			userData.ptr=PX_NULL;
			PX_QuadtreeResetTest(&world->Impact_Test_array[b]);
			PX_QuadtreeTestNode(&world->Impact_Test_array[b],x,y,raduis*2,raduis*2,userData);

			for (im_i=0;im_i<world->Impact_Test_array[b].Impacts.size;im_i++)
			{
				PX_Quadtree_UserData *puData=PX_VECTORAT(PX_Quadtree_UserData,&world->Impact_Test_array[b].Impacts,im_i);
				PX_WorldObject *pimpactWo=(PX_WorldObject *)puData->ptr;
				PX_Object *pObj2=((PX_WorldObject *)(puData->ptr))->pObject;

				if (pObj2->diameter)
				{
					if(!PX_isCircleCrossCircle(PX_POINT(x,y,0),raduis,PX_POINT(pObj2->x,pObj2->y,0),pObj2->diameter/2))
						continue;
				}
				else
				{
					if (!PX_isRectCrossCircle(PX_RECT(pObj2->x-pObj2->Width/2,pObj2->y-pObj2->Height/2,pObj2->Width,pObj2->Height),PX_POINT(x,y,0),raduis))
					{
						continue;
					}
				}

				if (pimpactWo->DeleteMark!=PX_TRUE)
				{
					if (m<MaxSearchCount)
					{
						Object[m]=pObj2;
						m++;
					}
					
				}
			}

		}
	}
	return m;
}
