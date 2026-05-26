#include "PX_World.h"

px_bool PX_World_Initialize(px_memorypool *mp,PX_World *pWorld,px_int world_width,px_int world_height,px_int surface_width,px_int surface_height)
{
	PX_memset(pWorld, 0, sizeof(PX_World));
	if(!PX_VectorInitialize(mp,&pWorld->pObjects,sizeof(PX_Object*),32)) return PX_FALSE;
	if(!PX_VectorInitialize(mp,&pWorld->pNewObjects,sizeof(PX_Object *),32)) return PX_FALSE;
	pWorld->mp=mp;
	pWorld->world_height=world_height;
	pWorld->world_width=world_width;
	pWorld->surface_height=surface_height;
	pWorld->surface_width=surface_width;
	pWorld->auxiliaryline=PX_TRUE;
	pWorld->auxiliaryline_color=PX_COLOR(64,255,192,255);
	pWorld->offsetx=0;
	pWorld->offsety=0;
	PX_srandEx(&pWorld->mt19937,0x314159);
	PX_World_SetAuxiliaryXYSpacer(pWorld,32,32);
	if (PX_WORLD_CALC_SIZE)
	{
		pWorld->calc_buffer = MP_Malloc(mp, PX_WORLD_CALC_SIZE);
		if (!pWorld->calc_buffer)
		{
			return PX_FALSE;
		}
		pWorld->calc_buffer_size = PX_WORLD_CALC_SIZE;
	}
	PX_memset(pWorld->Impact_Test_array, 0, sizeof(pWorld->Impact_Test_array));
	return PX_TRUE;
}



px_void PX_World_SetBackwardTexture(PX_World* pWorld, px_texture* ptexture)
{
	pWorld->pbackgroundtexture = ptexture;
}

px_void PX_World_SetFrontwardTexture(PX_World* pWorld, px_texture* ptexture)
{
	pWorld->pfrontwardtexture = ptexture;
}

px_void PX_World_SetAuxiliaryXYSpacer(PX_World *pw,px_int x,px_int y)
{
	pw->auxiliaryXSpacer=x;
	pw->auxiliaryYSpacer=y;
}

px_void PX_World_ShowAuxiliaryLine(PX_World *pw,px_bool bline)
{
	pw->auxiliaryline=bline;
}


px_int PX_World_GetObjectCount(PX_World *World)
{
	return World->pObjects.size;
}



PX_Object* PX_World_GetObject(PX_World* World, px_int index)
{
	if (index<0||index>= World->pObjects.size)
	{
		PX_ASSERT();
		return PX_NULL;
	}
	return *PX_VECTORAT(PX_Object*, &World->pObjects, index);
}

px_void PX_World_Clear(PX_World* world)
{
	px_int i;
	for (i = 0; i < world->pObjects.size; i++)
	{
		PX_Object* pObject = *PX_VECTORAT(PX_Object*, &world->pObjects, i);
		if (pObject)
		{
			PX_ObjectDelete(pObject);
		}
	}
	for (i = 0; i < world->pNewObjects.size; i++)
	{
		PX_Object* pObject = *PX_VECTORAT(PX_Object*, &world->pNewObjects, i);
		if (pObject)
		{
			PX_ObjectDelete(pObject);
		}
	}
	PX_VectorClear(&world->pObjects);
	PX_VectorClear(&world->pNewObjects);
}

px_void PX_World_SetSoundPlay(PX_World* pWorld, PX_SoundPlay* pSoundPlay)
{
	pWorld->psoundplay = pSoundPlay;
}

px_void PX_World_SetResourceLibrary(PX_World* pWorld, PX_ResourceLibrary* pResourceLibrary)
{
	pWorld->presourceLibrary = pResourceLibrary;
}

px_void PX_World_SoundPlay(PX_World* pWorld, const px_char key[], px_float x, px_float y)
{
	if (pWorld->presourceLibrary&&pWorld->psoundplay)
	{
		if (PX_isPointXYInRect(x, y, pWorld->offsetx-100, pWorld->offsety-100, pWorld->surface_width+200.f, pWorld->surface_height+200.f))
		{
			PX_SoundPlayPlayData(pWorld->psoundplay, PX_ResourceLibraryGetSound(pWorld->presourceLibrary, key));
		}
	}
	
}

static px_void PX_World_UpdateNewObjectList(PX_World* pworld)
{
	if (!PX_VectorMoveMemberTo(&pworld->pNewObjects, &pworld->pObjects))
	{
		PX_ERROR("PX_World_UpdateNewObjectList: PX_VectorMoveMemberTo failed.");
	}
}

px_void PX_World_Update( PX_World *pworld,px_uint elapsed )
{
	px_int i,b,j;
	px_float w_left=0,w_top=0,w_right=0,w_height=0;
	PX_Object_Event e;
	PX_Object *pWolrdObject;
	px_int impact_count[sizeof(((PX_Object*)0)->impact_object_type)*8]={0};
	px_memorypool calcmp;

	if (pworld==PX_NULL)
	{
		return;
	}

	calcmp = MP_Create(pworld->calc_buffer, pworld->calc_buffer_size);

	//Add NewObjects
	PX_World_UpdateNewObjectList(pworld);
	//////////////////////////////////////////////////////////////////////////
	//impact test
	//////////////////////////////////////////////////////////////////////////
	if (pworld->surface_height&&pworld->surface_width)
	{
		for (i = 0; i < pworld->pObjects.size; i++)
		{
			pWolrdObject = *PX_VECTORAT(PX_Object*, &pworld->pObjects, i);
			if (!pWolrdObject)
			{
				continue;
			}
			if (!pWolrdObject->Enabled)
			{
				continue;
			}
			if (pWolrdObject->delay_delete)
			{
				continue;
			}
			if (pWolrdObject->impact_object_type)
			{
				for (j = 0; j < (px_int)(sizeof(pWolrdObject->impact_object_type) * 8); j++)
				{
					if (pWolrdObject->impact_object_type & (1 << j))
						impact_count[j]++;
				}
			}
		}


		for (i = 0; i < sizeof(pWolrdObject->impact_object_type) * 8; i++)
		{
			if (impact_count[i])
			{
				PX_QuadtreeCreate(&calcmp, &pworld->Impact_Test_array[i], 0, 0, (px_float)pworld->world_width, (px_float)pworld->world_height, impact_count[i], 2);
			}
			else
			{
				PX_QuadtreeCreate(&calcmp, &pworld->Impact_Test_array[i], 0, 0, (px_float)pworld->world_width, (px_float)pworld->world_height, impact_count[i], 0);
			}
		}

		for (i = 0; i < pworld->pObjects.size; i++)
		{
			pWolrdObject = *PX_VECTORAT(PX_Object*, &pworld->pObjects, i);
			if (!pWolrdObject)
			{
				continue;
			}
			if (!pWolrdObject->Enabled)
			{
				continue;
			}
			if (pWolrdObject->delay_delete)
			{
				continue;
			}
			if (pWolrdObject->impact_object_type)
			{
				for (b = 0; b < sizeof(pWolrdObject->impact_object_type) * 8; b++)
				{
					if ((pWolrdObject->impact_object_type & (1 << b)))
					{
						PX_Quadtree_UserData userData;
						userData.ptr = pWolrdObject;
						if ((px_float)pWolrdObject->diameter)
						{
							PX_QuadtreeAddNode(&pworld->Impact_Test_array[b], (px_float)pWolrdObject->x, (px_float)pWolrdObject->y, (px_float)pWolrdObject->diameter, (px_float)pWolrdObject->diameter, userData);
						}
						else
						{
							PX_QuadtreeAddNode(&pworld->Impact_Test_array[b], (px_float)pWolrdObject->x, (px_float)pWolrdObject->y, (px_float)pWolrdObject->Width, (px_float)pWolrdObject->Height, userData);
						}

					}
				}
			}
		}

		for (i = 0; i < pworld->pObjects.size; i++)
		{
			pWolrdObject = *PX_VECTORAT(PX_Object*, &pworld->pObjects, i);
			if (!pWolrdObject)
			{
				continue;
			}

			if (pWolrdObject->impact_target_type)
			{
				for (b = 0; b < sizeof(pWolrdObject->impact_target_type) * 8; b++)
				{
					if ((pWolrdObject->impact_target_type & (1 << b)))
					{
						px_int im_i;
						PX_Quadtree_UserData userData;
						userData.ptr = pWolrdObject;
						PX_QuadtreeResetTest(&pworld->Impact_Test_array[b]);
						if (pWolrdObject->diameter)
						{
							PX_QuadtreeTestNode(&pworld->Impact_Test_array[b], (px_float)pWolrdObject->x, (px_float)pWolrdObject->y, (px_float)pWolrdObject->diameter, (px_float)pWolrdObject->diameter, userData);
						}
						else
						{
							PX_QuadtreeTestNode(&pworld->Impact_Test_array[b], (px_float)pWolrdObject->x, (px_float)pWolrdObject->y, (px_float)pWolrdObject->Width, (px_float)pWolrdObject->Height, userData);
						}

						for (im_i = 0; im_i < pworld->Impact_Test_array[b].Impacts.size; im_i++)
						{
							PX_Quadtree_UserData* puData = PX_VECTORAT(PX_Quadtree_UserData, &pworld->Impact_Test_array[b].Impacts, im_i);
							PX_Object* pObj1 = pWolrdObject;
							PX_Object* pObj2 = (PX_Object*)(puData->ptr);

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
							if (pObj1->delay_delete != PX_TRUE && pObj2->delay_delete != PX_TRUE)
							{
								e.Event = PX_OBJECT_EVENT_IMPACT;
								PX_Object_Event_SetImpactTargetObject(&e, pObj2);
								PX_ObjectExecuteEvent(pObj1, e);
							}
						}

					}
				}
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	//Update Objects
	for (i=0;i< pworld->pObjects.size;i++)
	{
		pWolrdObject=*PX_VECTORAT(PX_Object *,&pworld->pObjects,i);
		PX_ASSERTIFX(!pWolrdObject, "PX_WorldUpdate: pWolrdObject is NULL");
		if(pWolrdObject->Enabled&& !pWolrdObject->delay_delete)
		{
			px_int i;
			for (i = 0; i < PX_OBJECT_MAX_DESC_COUNT; i++)
			{
				if (pWolrdObject->Func_ObjectUpdate[i] != 0)
				{
					pWolrdObject->Func_ObjectUpdate[i](pWolrdObject, i, elapsed);
				}
			}

			if (pWolrdObject->pChildren != PX_NULL)
			{
				PX_Object_ObjectLinkerUpdate(pWolrdObject->pChildren, elapsed);
			}

		} 
		PX_ObjectUpdateDelayDelete(pWolrdObject->pChildren);
	}

	//////////////////////////////////////////////////////////////////////////
	//delete death objects
	i = 0;
	while(i < pworld->pObjects.size)
	{
		pWolrdObject=*PX_VECTORAT(PX_Object*,&pworld->pObjects,i);
		PX_ASSERTIFX(!pWolrdObject, "PX_WorldUpdate: pWolrdObject is NULL");
		if (pWolrdObject->delay_delete)
		{
			PX_ObjectDelete(pWolrdObject);
			PX_VectorErase(&pworld->pObjects, i);
		}
		else
		{
			i++;
		}
	}

}

px_void PX_World_SetCamera(PX_World *pw, px_float camera_center_x, px_float camera_center_y)
{
	px_int surface_width,surface_height;
	px_int LeftTopX,LeftTopY;

	surface_width=pw->surface_width;
	surface_height=pw->surface_height;
	LeftTopX=(px_int)(camera_center_x -surface_width/2),LeftTopY=(px_int)(camera_center_y -surface_height/2);

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


px_void PX_World_Render(px_surface *psurface,PX_World *pw,px_dword elapsed)
{
	px_int i,j;
	px_int surface_width,surface_height;
	PX_QuickSortAtom *ArrayIndex;
	px_memorypool calcmp;
	px_int sx,sy;
	PX_Object *pWolrdObject;

	if (pw==PX_NULL)
	{
		return;
	}

	calcmp = MP_Create(pw->calc_buffer, pw->calc_buffer_size);

	surface_width=pw->surface_width;
	surface_height=pw->surface_height;

	if (pw->pbackgroundtexture)
	{
		px_float xfactor;
		px_float yfactor;
		px_int x;
		px_int y;
		if (pw->world_width != pw->surface_width)
		{
			xfactor = pw->offsetx * 1.0f / (pw->world_width - pw->surface_width);
		}
		else
		{
			xfactor = 0;
		}

		if (pw->world_height != pw->surface_height)
		{
			yfactor = pw->offsety * 1.0f / (pw->world_height - pw->surface_height);
		}
		else
		{
			yfactor = 0;
		}

		x = (px_int)((pw->pbackgroundtexture->width - pw->surface_width) * xfactor);
		y = (px_int)((pw->pbackgroundtexture->height - pw->surface_height) * yfactor);
		PX_TextureRender(psurface, pw->pbackgroundtexture, -x, -y, PX_ALIGN_LEFTTOP, 0);
	}

	if (pw->auxiliaryline&& pw->auxiliaryline_color._argb.a)
	{
		if (pw->auxiliaryYSpacer > 0)
		{
			for (sy=pw->auxiliaryYSpacer-((px_int)pw->offsety%pw->auxiliaryYSpacer);sy<surface_height;sy+=pw->auxiliaryYSpacer)
			{
				PX_GeoDrawLine(psurface,0,sy,surface_width-1,sy,1,pw->auxiliaryline_color);
			}
		}

		if (pw->auxiliaryXSpacer > 0)
		{
			for (sx=pw->auxiliaryXSpacer-((px_int)pw->offsetx%pw->auxiliaryXSpacer);sx<surface_width;sx+=pw->auxiliaryXSpacer)
			{
				PX_GeoDrawLine(psurface,sx,0,sx,surface_height-1,1,pw->auxiliaryline_color);
			}
		}

	}


	if (pw->pObjects.size)
	{
		ArrayIndex = (PX_QuickSortAtom*)MP_Malloc(&calcmp, pw->pObjects.size * sizeof(PX_QuickSortAtom));
		if (ArrayIndex)
		{
			j = 0;

			for (i = 0; i < pw->pObjects.size; i++)
			{
				pWolrdObject = *PX_VECTORAT(PX_Object*, &pw->pObjects, i);
				if (!pWolrdObject)
				{
					PX_CRASH("px_world memory crashed");return;
				}
				ArrayIndex[j].weight = pWolrdObject->z;
				ArrayIndex[j].pData = pWolrdObject;
				j++;
			}

			PX_Quicksort_ArrayMaxToMin(ArrayIndex, 0, pw->pObjects.size - 1);

			for (i = 0; i < pw->pObjects.size; i++)
			{
				PX_Object* pObject = (PX_Object*)ArrayIndex[i].pData;
				px_float x = pObject->x;
				px_float y = pObject->y;
				pObject->x -= pw->offsetx;
				pObject->y -= pw->offsety;
				PX_ObjectRender(psurface, pObject, elapsed);
				pObject->x = x;
				pObject->y = y;
			}
		}
	}
	

	if (pw->pfrontwardtexture)
	{
		px_float xfactor;
		px_float yfactor;
		px_int x;
		px_int y;
		if (pw->world_width != pw->surface_width)
		{
			xfactor = pw->offsetx * 1.0f / (pw->world_width - pw->surface_width);
		}
		else
		{
			xfactor = 0;
		}

		if (pw->world_height != pw->surface_height)
		{
			yfactor = pw->offsety * 1.0f / (pw->world_height - pw->surface_height);
		}
		else
		{
			yfactor = 0;
		}

		x = (px_int)((pw->pfrontwardtexture->width - pw->surface_width) * xfactor);
		y = (px_int)((pw->pfrontwardtexture->height - pw->surface_height) * yfactor);
		PX_TextureRender(psurface, pw->pfrontwardtexture, -x, -y, PX_ALIGN_LEFTTOP, 0);
	}

}




px_bool PX_World_AddObject(PX_World *World,PX_Object *pObject)
{
	return PX_VectorPushback(&World->pNewObjects,&pObject);
}




px_void PX_World_SetAuxiliaryLineColor(PX_World *pw,px_color color)
{
	pw->auxiliaryline_color=color;
}



px_point2D PX_World_ObjectXYtoScreenXY(PX_World *pw,px_float x,px_float y)
{
	return PX_POINT2D(x-pw->offsetx,y-pw->offsety);
}

px_void PX_World_PostEvent(PX_World* pw, PX_Object_Event e)
{
	PX_Object* pWolrdObject;
	px_int i;
	for (i = 0; i < pw->pObjects.size; i++)
	{
		pWolrdObject = *PX_VECTORAT(PX_Object*, &pw->pObjects, i);
		PX_ASSERTIFX(!pWolrdObject, "PX_WorldPostEvent: pWolrdObject is NULL");
		PX_ObjectPostEvent(pWolrdObject, e);
	}
}


px_void PX_World_Free(PX_World *pw)
{
	PX_Object *pWolrdObject;
	px_int i;
	for (i=0;i<pw->pObjects.size;i++)
	{
		pWolrdObject= *PX_VECTORAT(PX_Object*, &pw->pObjects, i);
		if (pWolrdObject)
		{
			PX_ObjectDelete(pWolrdObject);
		}
	}
	
	for (i = 0; i < pw->pNewObjects.size; i++)
	{
		PX_Object *pObject = *PX_VECTORAT(PX_Object *, &pw->pNewObjects, i);
		if (pObject)
		{
			PX_ObjectDelete(pObject);
		}
	}

	PX_VectorFree(&pw->pObjects);
	PX_VectorFree(&pw->pNewObjects);

	if (pw->calc_buffer)
	{
		MP_Free(pw->mp, pw->calc_buffer);
		pw->calc_buffer = PX_NULL;
		pw->calc_buffer_size = 0;
	}
}


px_void PX_World_RemoveObjectByIndex(PX_World *world,px_int i_index)
{
	PX_Object* pObject;
	PX_ASSERTIFX(i_index < 0 || i_index >= world->pObjects.size, "PX_World_RemoveObjectByIndex: index out of range");
	pObject = *PX_VECTORAT(PX_Object*, &world->pObjects, i_index);

	pObject->delay_delete = PX_TRUE;
	
}

px_void PX_World_RemoveObject(PX_World* pWorld, PX_Object* pObject)
{
	pObject->delay_delete = PX_TRUE;
}

px_void PX_World_SetSize(PX_World* pWorld, px_int world_width, px_int world_height)
{
	pWorld->world_width = world_width;
	pWorld->world_height = world_height;
}
