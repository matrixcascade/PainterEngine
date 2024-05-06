#include "PX_CDA.h"
#include "PX_CDA_VM.h"   
#include "PX_CDA_ObjectVM.h"          



px_bool	PX_CDA_Initialize(px_memorypool* mp, PX_CDA* pCDA, px_int grid_x_count, px_int grid_y_count, px_int view_width, px_int view_height,PX_FontModule *fm,PX_ResourceLibrary *plib)
{
	px_string *pscript;
	PX_memset(pCDA, 0, sizeof(PX_CDA));

	pscript = PX_ResourceLibraryGetString(plib, "cda_script");
	if (!pscript)
	{
		return PX_FALSE;
	}

	pCDA->mp = mp;
	pCDA->id_alloc = 0x100000;
	//Initialize camera
	pCDA->grid_size = PX_CDA_GRID_DEFAULE_SIZE;
	pCDA->camera_x = (grid_x_count *  pCDA->grid_size) / 2.f;
	pCDA->camera_y = (grid_y_count *  pCDA->grid_size) / 2.f;
	
	pCDA->view_width=view_width;
	pCDA->view_height=view_height;

	pCDA->grid_line_color = PX_CSS_COLOR_DarkGray;
	pCDA->show_grid_line=PX_TRUE;
	pCDA->show_controller_size=PX_TRUE;
	pCDA->show_id_text=PX_TRUE;

	//grid
	pCDA->grid_x_count = grid_x_count;
	pCDA->grid_y_count = grid_y_count;
	pCDA->pgrids = (PX_CDA_Grid *)MP_Malloc(mp, sizeof(PX_CDA_Grid)*grid_x_count*grid_y_count);
	if (!pCDA->pgrids)
	{
		return PX_FALSE;
	}
	pCDA->presourceLibrary=plib;
	pCDA->fontmodule = fm;
	pCDA->psource_background_texture = PX_ResourceLibraryGetTexture(plib,"cda_background");

	if (!PX_MapInitialize(mp, &pCDA->objectClasses))
	{
		MP_Free(mp, pCDA->pgrids);
		return PX_FALSE;
	}

	if (!PX_MapInitialize(mp, &pCDA->routeClasses))
	{
		MP_Free(mp, pCDA->pgrids);
		PX_MapFree(&pCDA->objectClasses);
		return PX_FALSE;
	}

	if (!PX_VectorInitialize(mp, &pCDA->pObjects, sizeof(PX_CDA_Object *), 256))
	{
		MP_Free(mp, pCDA->pgrids);
		PX_MapFree(&pCDA->objectClasses);
		PX_MapFree(&pCDA->routeClasses);
		return PX_FALSE;
	}


	if(!PX_VectorInitialize(mp, &pCDA->routes, sizeof(PX_CDA_Route), 32))
	{
		MP_Free(mp, pCDA->pgrids);
		PX_MapFree(&pCDA->objectClasses);
		PX_VectorFree(&pCDA->pObjects);
		return PX_FALSE;
	}

	PX_MemoryInitialize(mp, &pCDA->payload);

	if(!PX_CDA_LoadScript(pCDA, pscript->buffer))
	{
		MP_Free(mp, pCDA->pgrids);
		PX_MapFree(&pCDA->objectClasses);
		PX_MapFree(&pCDA->routeClasses);
		PX_VectorFree(&pCDA->pObjects);
		PX_VectorFree(&pCDA->routes);
		return PX_FALSE;
	}

	return PX_TRUE;
}

px_void PX_CDA_SetBackwardTexture(PX_CDA* pCDA, px_texture* ptexture)
{
	pCDA->psource_background_texture = ptexture;
}

px_void PX_CDA_SetViewGridSize(PX_CDA * pCDA,px_int size)
{
	pCDA->grid_size=size;
}

px_void PX_CDA_ShowGridLine(PX_CDA *pCDA,px_bool bline)
{
	pCDA->show_grid_line =bline;
}

PX_Object* PX_CDA_GetCDAObject(PX_CDA* CDA, px_int index)
{
	PX_Object* pObject;
	PX_ASSERTIF(index < 0);

	if (index >= CDA->pObjects.size)
	{
		return PX_NULL;
	}
	pObject = PX_VECTORAT(PX_Object, &CDA->pObjects, index);
	return pObject;
}

px_int PX_CDA_GetObjectCount(PX_CDA *CDA)
{
	return CDA->pObjects.size;
}

px_int PX_CDA_GetAliveCount(PX_CDA* CDA)
{
	px_int i;
	px_int count=0;
	for (i = 0; i < CDA->pObjects.size; i++)
	{
		if (PX_CDA_GetCDAObject(CDA, i)->delay_delete)
			count++;
	}
	return count;
}

px_void PX_CDA_BindResourceLibrary(PX_CDA* pCDA, PX_ResourceLibrary* pResourceLibrary)
{
	pCDA->presourceLibrary = pResourceLibrary;
}

px_bool PX_CDA_AddObject(PX_CDA* pCDA, PX_Object* pObject)
{
	return PX_VectorPushback(&pCDA->pObjects, &pObject);
}

px_void PX_CDA_DeleteObject(PX_CDA* pCDA, px_int index)
{
	if (index >= 0 && index < pCDA->pObjects.size)
	{
		PX_Object* pObject = *PX_VECTORAT(PX_Object*, &pCDA->pObjects, index);
		PX_ObjectDelete(pObject);
		PX_VectorErase(&pCDA->pObjects, index);
	}
}

px_void PX_CDA_Clear(PX_CDA* pCDA)
{
	px_int i;
	pCDA->id_alloc = 0;
	for ( i = 0; i < pCDA->grid_x_count*pCDA->grid_y_count; i++)
	{
		PX_VM_VariableFree(&pCDA->pgrids[i].var);
	}
	PX_memset(pCDA->pgrids, 0, sizeof(PX_CDA_Grid)*pCDA->grid_x_count*pCDA->grid_y_count);

	while (pCDA->pObjects.size)
	{
		PX_CDA_DeleteObject(pCDA, 0);
	}

	while (pCDA->routes.size)
	{
		PX_CDA_DeleteRoute(pCDA, 0);
	}

}

px_void PX_CDA_Free(PX_CDA* pCDA)
{
	PX_RBNode* pNode;
	PX_CDA_Clear(pCDA);
	PX_VectorFree(&pCDA->pObjects);
	PX_VectorFree(&pCDA->routes);
	if (pCDA->snapshot_texture.MP)
	{
		PX_TextureFree(&pCDA->snapshot_texture);
	}
	if (pCDA->background_texture.MP)
	{
		PX_TextureFree(&pCDA->background_texture);
	}
	MP_Free(pCDA->mp, pCDA->pgrids);

	pNode = PX_MapFirst(&pCDA->objectClasses);
	while (pNode)
	{
		MP_Free(pCDA->mp, pNode->_ptr);
		pNode->_ptr = PX_NULL;
		pNode = PX_MapNext(pNode);
	}
	PX_MapFree(&pCDA->objectClasses);

	pNode = PX_MapFirst(&pCDA->routeClasses);
	while (pNode)
	{
		MP_Free(pCDA->mp, pNode->_ptr);
		pNode->_ptr = PX_NULL;
		pNode = PX_MapNext(pNode);
	}
	PX_MapFree(&pCDA->routeClasses);

	if (pCDA->vm.mp)
	{
		PX_VMFree(&pCDA->vm);
	}

	if (pCDA->debugmap.mp)
	{
		PX_VMDebuggerMapFree(&pCDA->debugmap);
	}
	PX_MemoryFree(&pCDA->payload);
}

px_void PX_CDA_RemoveObject(PX_CDA* CDA, PX_Object* pObject)
{
	PX_Object* pEnumObject;
	if (pObject->cda_index != -1 && pObject->cda_index < CDA->pObjects.size)
	{
		pEnumObject = PX_VECTORAT(PX_Object, &CDA->pObjects, pObject->cda_index);
		if (pEnumObject == pObject)
		{
			pEnumObject->delay_delete = PX_TRUE;
			return;
		}
	}
	else
	{
		px_int i;
		for (i = 0; i < CDA->pObjects.size; i++)
		{
			pEnumObject = PX_VECTORAT(PX_Object, &CDA->pObjects, i);
			if (pEnumObject == pObject)
			{
				pEnumObject->delay_delete = PX_TRUE;
				return;
			}
		}
	}
}

px_void PX_CDA_SetGridLineColor(PX_CDA* pCDA, px_color color)
{
	pCDA->grid_line_color = color;
}

static px_void PX_CDA_UpdateCDAObjects(PX_CDA* pCDA,px_dword elapsed)
{
	px_int i;
	for (i = 0; i < pCDA->pObjects.size; i++)
	{
		PX_Object* pObject = *PX_VECTORAT(PX_Object*, &pCDA->pObjects, i);
		PX_CDA_Object *pDesc=PX_ObjectGetDesc(PX_CDA_Object,pObject);

		PX_ObjectUpdate(pObject, elapsed);

		if (pCDA->vm.binsize)
			PX_VMRun(&pDesc->vm, 65536, elapsed);
		
		pDesc->grid_x+=pObject->vx*elapsed/1000.f;
		pDesc->grid_y+=pObject->vy*elapsed/1000.f;
	}
}

static px_void PX_CDA_UpdateRemovedObjects(PX_CDA* pCDA, px_dword elapsed)
{
	px_int i;
	for (i = 0; i < pCDA->pObjects.size; i++)
	{
		PX_Object* pObject = *PX_VECTORAT(PX_Object *, &pCDA->pObjects, i);
		if (pObject&&pObject->delay_delete)
		{
			PX_VectorErase(&pCDA->pObjects, i);
			i--;
		}
	}
}

static px_void PX_CDA_UpdateRouteSignal(PX_CDA* pCDA, PX_CDA_Route* proute, px_dword elapsed)
{
	px_int i;
	for ( i = 0; i < proute->signals.size; i++)
	{
		PX_CDA_Signal *psignal=PX_VECTORAT(PX_CDA_Signal, &proute->signals, i);
		PX_ASSERTIF(psignal==PX_NULL);
		psignal->x += proute->prouteClass->busSpeed * elapsed / 1000.f;
		if (psignal->x>=proute->length)
		{
			//copy signal to end grid
			if (psignal->direction==PX_CDA_SIGNAL_DIRECTION_FORWARD)
			{
				if ((proute->start_grid_x >= 0 && proute->start_grid_y >= 0) && (proute->start_grid_x < pCDA->grid_x_count && proute->start_grid_y < pCDA->grid_y_count))
				{
					//check end point
					if ((proute->end_grid_x >= 0 && proute->end_grid_y >= 0) && (proute->end_grid_x < pCDA->grid_x_count && proute->end_grid_y < pCDA->grid_y_count))
					{
						PX_CDA_Grid* pgrid = &pCDA->pgrids[proute->end_grid_x + proute->end_grid_y * pCDA->grid_x_count];
						PX_VM_VariableFree(&pgrid->var);
						pgrid->var = PX_VM_VariableCopy(pCDA->mp, psignal->var, 0);
						pgrid->timestamp = psignal->timestamp;
					}
				}
				
			}
			else
			{
				if ((proute->start_grid_x >= 0 && proute->start_grid_y >= 0) && (proute->start_grid_x < pCDA->grid_x_count && proute->start_grid_y < pCDA->grid_y_count))
				{
					//check end point
					if ((proute->end_grid_x >= 0 && proute->end_grid_y >= 0) && (proute->end_grid_x < pCDA->grid_x_count && proute->end_grid_y < pCDA->grid_y_count))
					{
						PX_CDA_Grid* pgrid = &pCDA->pgrids[proute->start_grid_x + proute->start_grid_y * pCDA->grid_x_count];
						PX_VM_VariableFree(&pgrid->var);
						pgrid->var = PX_VM_VariableCopy(pCDA->mp, psignal->var, 0);
						pgrid->timestamp = psignal->timestamp;
					}
				}
				
			}
			//free var
			PX_VM_VariableFree(&psignal->var);
			PX_VectorErase(&proute->signals, i);
			i--;
		}
	}
}


static px_void PX_CDA_UpdateRoutes(PX_CDA* pCDA, px_dword elapsed)
{
	px_int i;
	for (i = 0; i < pCDA->routes.size; i++)
	{
		PX_CDA_Route* proute = PX_VECTORAT(PX_CDA_Route, &pCDA->routes, i);
		PX_ASSERTIF(proute->prouteClass!=PX_NULL);
		if (proute->prouteClass->direction== PX_CDA_ROUTE_DIRECTION_TWO_WAY|| proute->prouteClass->direction==PX_CDA_ROUTE_DIRECTION_ONE_WAY)
		{
			continue;
		}
		if (proute->prouteClass->busSpeed < 0)
		{
			continue;
		}

		//get startpoint timestamps
		if ((proute->start_grid_x >= 0 && proute->start_grid_y >=0 )&& (proute->start_grid_x < pCDA->grid_x_count && proute->start_grid_y < pCDA->grid_y_count))
		{
			//check end point
			if ((proute->end_grid_x >= 0 && proute->end_grid_y >= 0) && (proute->end_grid_x < pCDA->grid_x_count && proute->end_grid_y < pCDA->grid_y_count))
			{
				PX_CDA_Grid *pst= &pCDA->pgrids[proute->start_grid_x + proute->start_grid_y * pCDA->grid_x_count];
				PX_CDA_Grid *ped= &pCDA->pgrids[proute->end_grid_x + proute->end_grid_y * pCDA->grid_x_count];
				px_dword start_timestamp = pst->timestamp;
				px_dword end_timestamp =ped->timestamp;

				if (start_timestamp != proute->start_timestamp)
				{
					proute->start_timestamp = start_timestamp;
					if (proute->prouteClass->busSpeed == 0)
					{
						proute->end_timestamp = start_timestamp;
						PX_VM_VariableFree(&ped->var);
						ped->var= PX_VM_VariableCopy(pCDA->mp, pst->var,0);
					}
					else
					{
						//new signal
						PX_CDA_Signal signal;
						signal.direction = PX_CDA_SIGNAL_DIRECTION_FORWARD;
						signal.timestamp = start_timestamp;
						signal.var = PX_VM_VariableCopy(pCDA->mp, pst->var,0);
						signal.x = 0;
						PX_VectorPushback(&proute->signals, &signal);
					}
				}

				if (proute->prouteClass->direction== PX_CDA_ROUTE_DIRECTION_TWO_WAY)
				{
					if (end_timestamp != proute->end_timestamp)
					{
						proute->end_timestamp = end_timestamp;
						if (proute->prouteClass->busSpeed == 0)
						{
							proute->start_timestamp = end_timestamp;
							PX_VM_VariableFree(&ped->var);
							pst->var = PX_VM_VariableCopy(pCDA->mp, ped->var, 0);
						}
						else
						{
							//new signal
							PX_CDA_Signal signal;
							signal.direction = PX_CDA_SIGNAL_DIRECTION_BACKWARD;
							signal.timestamp = end_timestamp;
							signal.var = PX_VM_VariableCopy(pCDA->mp, ped->var, 0);
							signal.x = 0;
							PX_VectorPushback(&proute->signals, &signal);
						}
					}
				}
			}
		}
		
		PX_CDA_UpdateRouteSignal(pCDA, proute, elapsed);
	}

}

px_void PX_CDA_Update( PX_CDA *pCDA,px_uint elapsed )
{
	//update routes
	PX_CDA_UpdateRoutes(pCDA, elapsed);

	//transfer cda position to screen position
	PX_CDA_UpdateCDAObjects(pCDA,elapsed);

	//remove delete mark object
	PX_CDA_UpdateRemovedObjects(pCDA,elapsed);
}

static px_void PX_CDAClildRender(PX_CDA *pCDA,px_surface *pSurface, PX_Object *pObject,px_uint elapsed,px_float oftX,px_float oftY)
{
	if (pObject==PX_NULL|| pObject->Visible == PX_FALSE)
	{
		return;
	}

	if (pObject->Func_ObjectRender!=0)
	{
		px_float x=pObject->x;
		px_float y=pObject->y;
		pObject->x-=oftX;
		pObject->y-=oftY;
		pObject->Func_ObjectRender(pSurface,pObject,elapsed);

		if (pCDA->show_controller_size)
		{
			PX_GeoDrawRect(pSurface, (px_int)(pObject->x- pObject->Width/2), (px_int)(pObject->y- pObject->Height/2), (px_int)(pObject->x + pObject->Width/2), (px_int)(pObject->y + pObject->Height/2), PX_COLOR(32, 255, 64, 0));
		}


		pObject->x=x;
		pObject->y=y;
	}
	if (pObject->pNextBrother!=PX_NULL)
	{
		PX_CDAClildRender(pCDA,pSurface,pObject->pNextBrother,elapsed,oftX,oftY);
	}
	if (pObject->pChilds!=PX_NULL)
	{
		PX_CDAClildRender(pCDA,pSurface,pObject->pChilds,elapsed,oftX,oftY);
	}
}

px_void PX_CDA_RenderBackgroundTexture(px_surface* psurface, PX_CDA* pCDA)
{
	px_int offset_x, offset_y;
	px_int grid_view_size_x = pCDA->grid_x_count * pCDA->grid_size;
	px_int grid_view_size_y = pCDA->grid_y_count * pCDA->grid_size;
	if(!pCDA->psource_background_texture)
		return;

	if (!pCDA->background_texture.MP)
	{
		if (!PX_TextureCreate(pCDA->mp, &pCDA->background_texture, grid_view_size_x, grid_view_size_y))
			return;

		if(pCDA->psource_background_texture)
			PX_TextureScaleToTexture(pCDA->psource_background_texture, &pCDA->background_texture);
	}
	else
	{
		if (pCDA->background_texture.width != grid_view_size_x || pCDA->background_texture.height != grid_view_size_y)
		{
			PX_TextureFree(&pCDA->background_texture);
			PX_memset(&pCDA->background_texture, 0, sizeof(pCDA->background_texture));
			if (!PX_TextureCreate(pCDA->mp, &pCDA->background_texture, grid_view_size_x, grid_view_size_y))
				return;

			if (pCDA->psource_background_texture)
				PX_TextureScaleToTexture(pCDA->psource_background_texture, &pCDA->background_texture);
		}
	}


	offset_x = (px_int)(pCDA->camera_x - pCDA->view_width / 2);
	offset_y = (px_int)(pCDA->camera_y - pCDA->view_height / 2);
	if (pCDA->background_texture.MP)
	{
		PX_TextureRender(psurface, &pCDA->background_texture, -offset_x, -offset_y, PX_ALIGN_LEFTTOP, 0);
	}
}

px_void PX_CDA_UpdateRouteStartEndGrid(PX_CDA* pCDA, PX_CDA_Route* proute)
{
	px_int i;
	px_int x, y, ex = 0, ey = 0;
	PX_CDA_RouteLine* p = (PX_CDA_RouteLine*)proute->routes.data;
	x = proute->start_grid_x;
	y = proute->start_grid_y;

	for (i = 0; i < proute->routes.size; i++)
	{
		px_byte direction = p[i].direction;
		px_short length = p[i].length;

		switch (direction)
		{
		case 8:
			ey = y - length;
			ex = x;
			break;
		case 2:
			ey = y + length;
			ex = x;
			break;
		case 4:
			ey = y;
			ex = x - length;
			break;
		case 6:
			ey = y;
			ex = x + length;
			break;
		case 7:
			ey = y - length;
			ex = x - length;
			break;
		case 9:
			ey = y - length;
			ex = x + length;
			break;
		case 1:
			ey = y + length;
			ex = x - length;
			break;
		case 3:
			ey = y + length;
			ex = x + length;
			break;
		default:
			PX_ERROR("Route data error");
			break;
		}
		x = ex;
		y = ey;
	}
	proute->end_grid_x = x;
	proute->end_grid_y = y;
}

px_void PX_CDA_RenderRouteLines(px_surface* psurface, PX_CDA* pCDA, PX_CDA_Route* proute, px_int line_width, px_color color)
{
	px_int i;
	px_point2Di start_screen_pos, end_screen_pos;
	px_int x, y, ex = 0, ey = 0;
	PX_CDA_RouteLine* p = (PX_CDA_RouteLine*)proute->routes.data;
	x = proute->start_grid_x;
	y = proute->start_grid_y;

	for (i = 0; i < proute->routes.size; i++)
	{

		px_byte direction = p[i].direction;
		px_short length = p[i].length;

		switch (direction)
		{
		case 8:
			ey = y - length;
			ex = x;
			break;
		case 2:
			ey = y + length;
			ex = x;
			break;
		case 4:
			ey = y;
			ex = x - length;
			break;
		case 6:
			ey = y;
			ex = x + length;
			break;
		case 7:
			ey = y - length;
			ex = x - length;
			break;
		case 9:
			ey = y - length;
			ex = x + length;
			break;
		case 1:
			ey = y + length;
			ex = x - length;
			break;
		case 3:
			ey = y + length;
			ex = x + length;
			break;
		default:
			PX_ERROR("Route data error");
			break;
		}
		start_screen_pos.x = x * pCDA->grid_size + pCDA->grid_size / 2 - (px_int)(pCDA->camera_x - pCDA->view_width / 2);
		start_screen_pos.y = y * pCDA->grid_size + pCDA->grid_size / 2 - (px_int)(pCDA->camera_y - pCDA->view_height / 2);
		end_screen_pos.x = ex * pCDA->grid_size + pCDA->grid_size / 2 - (px_int)(pCDA->camera_x - pCDA->view_width / 2);
		end_screen_pos.y = ey * pCDA->grid_size + pCDA->grid_size / 2 - (px_int)(pCDA->camera_y - pCDA->view_height / 2);

		PX_GeoDrawLine(psurface, (px_int)start_screen_pos.x, (px_int)start_screen_pos.y, (px_int)end_screen_pos.x, (px_int)end_screen_pos.y, line_width * 2 / 3, color);

		x = ex;
		y = ey;
	}

}

px_void PX_CDA_RenderPreviewRouteLines(px_surface* psurface, PX_CDA* pCDA, PX_CDA_Route* proute, px_int line_width, px_color color)
{
	px_int i;
	px_point2Di start_screen_pos, end_screen_pos;
	px_int x, y, ex = 0, ey = 0;
	PX_CDA_RouteLine* p = (PX_CDA_RouteLine*)proute->routes.data;
	x = proute->start_grid_x;
	y = proute->start_grid_y;

	for (i = 0; i < proute->routes.size; i++)
	{

		px_byte direction = p[i].direction;
		px_short length = p[i].length;

		switch (direction)
		{
		case 8:
			ey = y - length;
			ex = x;
			break;
		case 2:
			ey = y + length;
			ex = x;
			break;
		case 4:
			ey = y;
			ex = x - length;
			break;
		case 6:
			ey = y;
			ex = x + length;
			break;
		case 7:
			ey = y - length;
			ex = x - length;
			break;
		case 9:
			ey = y - length;
			ex = x + length;
			break;
		case 1:
			ey = y + length;
			ex = x - length;
			break;
		case 3:
			ey = y + length;
			ex = x + length;
			break;
		default:
			PX_ERROR("Route data error");
			break;
		}
		start_screen_pos.x = x * pCDA->grid_size + pCDA->grid_size / 2 - (px_int)(pCDA->camera_x - pCDA->view_width / 2);
		start_screen_pos.y = y * pCDA->grid_size + pCDA->grid_size / 2 - (px_int)(pCDA->camera_y - pCDA->view_height / 2);
		end_screen_pos.x = ex * pCDA->grid_size + pCDA->grid_size / 2 - (px_int)(pCDA->camera_x - pCDA->view_width / 2);
		end_screen_pos.y = ey * pCDA->grid_size + pCDA->grid_size / 2 - (px_int)(pCDA->camera_y - pCDA->view_height / 2);

		if (i < pCDA->editing_route_pin)
		{
			PX_GeoDrawLine(psurface, (px_int)start_screen_pos.x, (px_int)start_screen_pos.y, (px_int)end_screen_pos.x, (px_int)end_screen_pos.y, line_width * 2 / 3, color);
		}
		else
		{
			PX_GeoDrawDashLine(psurface, (px_int)start_screen_pos.x, (px_int)start_screen_pos.y, (px_int)end_screen_pos.x, (px_int)end_screen_pos.y, line_width * 2 / 3,8, color);
		}


		x = ex;
		y = ey;
	}

}

px_void PX_CDA_RenderRoute(px_surface* psurface, PX_CDA* pCDA, PX_CDA_Route* proute,px_bool preview)
{
	px_point2Di render_pos;
	px_float scale = pCDA->grid_size * 1.0f / PX_CDA_GRID_DEFAULE_SIZE;
	px_int line_width = (px_int)(proute->prouteClass->linewidth* scale);
	PX_CDA_RouteLine* plastline;
	if (line_width < 1)line_width = 1;

	
	if (!preview)
		PX_CDA_RenderRouteLines(psurface, pCDA, proute, line_width, proute->prouteClass->color);
	else
		PX_CDA_RenderPreviewRouteLines(psurface, pCDA, proute, line_width, proute->prouteClass->color);



	render_pos.x=proute->start_grid_x*pCDA->grid_size+ pCDA->grid_size / 2- (px_int)(pCDA->camera_x - pCDA->view_width / 2);
	render_pos.y=proute->start_grid_y*pCDA->grid_size+ pCDA->grid_size / 2 - (px_int)(pCDA->camera_y - pCDA->view_height / 2);

	PX_GeoDrawSolidCircle(psurface, (px_int)render_pos.x, (px_int)render_pos.y, line_width*2, proute->prouteClass->color);
	PX_GeoDrawSolidCircle(psurface, (px_int)render_pos.x, (px_int)render_pos.y, line_width, PX_COLOR_WHITE);

	render_pos.x = proute->end_grid_x * pCDA->grid_size + pCDA->grid_size / 2 - (px_int)(pCDA->camera_x - pCDA->view_width / 2);
	render_pos.y = proute->end_grid_y * pCDA->grid_size + pCDA->grid_size / 2 - (px_int)(pCDA->camera_y - pCDA->view_height / 2);

	
	
	if (proute->prouteClass->direction==PX_CDA_ROUTE_DIRECTION_ONE_WAY)
	{
		if (proute->routes.size)
		{
			px_point2Di p0 = { 0 }, p1 = { 0 }, p2 = {0};
			plastline = PX_VECTORLAST(PX_CDA_RouteLine, &proute->routes);
			if (plastline)
			{
				switch (plastline->direction)
				{
				default:
				case 8:
					//draw triangle up
					p0.x = render_pos.x;
					p0.y = render_pos.y - line_width * 2;
					p1.x = render_pos.x - line_width * 2;
					p1.y = render_pos.y + line_width * 2;
					p2.x = render_pos.x + line_width * 2;
					p2.y = render_pos.y + line_width * 2;
					break;
				case 2:
					//draw triangle down
					p0.x = render_pos.x;
					p0.y = render_pos.y + line_width * 2;
					p1.x = render_pos.x - line_width * 2;
					p1.y = render_pos.y - line_width * 2;
					p2.x = render_pos.x + line_width * 2;
					p2.y = render_pos.y - line_width * 2;
					break;
				case 4:
					//draw triangle left
					p0.x = render_pos.x - line_width * 2;
					p0.y = render_pos.y;
					p1.x = render_pos.x + line_width * 2;
					p1.y = render_pos.y - line_width * 2;
					p2.x = render_pos.x + line_width * 2;
					p2.y = render_pos.y + line_width * 2;
					break;
				case 6:
					//draw triangle right
					p0.x = render_pos.x + line_width * 2;
					p0.y = render_pos.y;
					p1.x = render_pos.x - line_width * 2;
					p1.y = render_pos.y - line_width * 2;
					p2.x = render_pos.x - line_width * 2;
					p2.y = render_pos.y + line_width * 2;
					break;
				case 9:
					//draw triangle right up
					p0.x = render_pos.x - line_width * 2;
					p0.y = render_pos.y - line_width * 2;
					p1.x = render_pos.x + line_width * 2;
					p1.y = render_pos.y - line_width * 2;
					p2.x = render_pos.x + line_width * 2;
					p2.y = render_pos.y + line_width * 2;
					break;
				case 7:
					//draw triangle left up
					p0.x = render_pos.x + line_width * 2;
					p0.y = render_pos.y - line_width * 2;
					p1.x = render_pos.x - line_width * 2;
					p1.y = render_pos.y - line_width * 2;
					p2.x = render_pos.x - line_width * 2;
					p2.y = render_pos.y + line_width * 2;
					break;
				case 3:
					//draw triangle right down
					p0.x = render_pos.x - line_width * 2;
					p0.y = render_pos.y + line_width * 2;
					p1.x = render_pos.x + line_width * 2;
					p1.y = render_pos.y + line_width * 2;
					p2.x = render_pos.x + line_width * 2;
					p2.y = render_pos.y - line_width * 2;
					break;
				case 1:
					//draw triangle left down
					p0.x = render_pos.x + line_width * 2;
					p0.y = render_pos.y + line_width * 2;
					p1.x = render_pos.x - line_width * 2;
					p1.y = render_pos.y + line_width * 2;
					p2.x = render_pos.x - line_width * 2;
					p2.y = render_pos.y - line_width * 2;
					break;
				}
				PX_GeoDrawTriangle(psurface, PX_POINT2D(p0.x*1.f,p0.y * 1.f), PX_POINT2D(p1.x * 1.f, p1.y * 1.f), PX_POINT2D(p2.x * 1.f, p2.y * 1.f), proute->prouteClass->color);
			}
		}

	}
	else
	{
		PX_GeoDrawSolidCircle(psurface, (px_int)render_pos.x, (px_int)render_pos.y, line_width * 2, proute->prouteClass->color);
		PX_GeoDrawSolidCircle(psurface, (px_int)render_pos.x, (px_int)render_pos.y, line_width, PX_COLOR_WHITE);
	}
	


	switch (proute->mode)
	{
	case PX_CDA_ROUTE_MODE_NORMAL:
		break;
	case PX_CDA_ROUTE_MODE_CURSOR:
		if(!preview)
			PX_CDA_RenderRouteLines(psurface, pCDA, proute, line_width + 2, proute->prouteClass->color);
		else
			PX_CDA_RenderPreviewRouteLines(psurface, pCDA, proute, line_width + 2, proute->prouteClass->color);
		break;
	case PX_CDA_ROUTE_MODE_SELECT:
		if (!preview)
			PX_CDA_RenderRouteLines(psurface, pCDA, proute, line_width + 2, proute->prouteClass->color);
		else
			PX_CDA_RenderPreviewRouteLines(psurface, pCDA, proute, line_width + 2, proute->prouteClass->color);
		break;
	default:
		break;
	}


}

px_void PX_CDA_RenderRoutes(px_surface* psurface, PX_CDA* pCDA)
{
	px_int i;
	for ( i = 0; i < pCDA->routes.size; i++)
	{
		PX_CDA_Route* proute = PX_VECTORAT(PX_CDA_Route, &pCDA->routes, i);
		PX_ASSERTIF(proute == PX_NULL);
		PX_CDA_RenderRoute(psurface, pCDA, proute,PX_FALSE);
	}
	if (pCDA->editing_route.routes.mp)
	{
		PX_CDA_RenderRoute(psurface, pCDA, &pCDA->editing_route,PX_TRUE);
	}
}

px_void PX_CDA_RenderGrid(px_surface* psurface, PX_CDA* pCDA,px_int x,px_int y,px_int size,px_int index)
{
	
	//PX_GeoDrawRect(psurface, x, y, x + size-1, y + size-1, PX_COLOR(128, 255, 0, 0));
	//PX_itoa(index, index_str,32, 10);
	//PX_FontModuleDrawText(psurface,pCDA->fontmodule, x+pCDA->grid_size/2, y + pCDA->grid_size / 2,PX_ALIGN_CENTER, index_str, PX_COLOR(255, 255, 255, 255));
}

px_void PX_CDA_GetGridRenderXY(PX_CDA* pCDA,px_int index,px_int *x,px_int *y)
{
	px_int grid_view_size=pCDA->grid_size;
	px_int grid_x = index % pCDA->grid_x_count;
	px_int grid_y = index / pCDA->grid_x_count;

	*x = grid_x*grid_view_size - (px_int)pCDA->camera_x + pCDA->view_width / 2;
	*y = grid_y*grid_view_size - (px_int)pCDA->camera_y + pCDA->view_height / 2;
}

px_void PX_CDA_RenderGrids(px_surface* psurface, PX_CDA* pCDA)
{
	px_int draw_x, draw_y,start_draw_x, start_draw_y;
	px_int index_x, index_y;
	px_int grid_view_size=pCDA->grid_size;

	px_int start_x_index = ((px_int)pCDA->camera_x - pCDA->view_width / 2) / grid_view_size;
	px_int start_y_index = ((px_int)pCDA->camera_y - pCDA->view_height / 2) / grid_view_size;

	start_draw_x = start_x_index*grid_view_size - (px_int)pCDA->camera_x + pCDA->view_width / 2;
	start_draw_y = start_y_index*grid_view_size - (px_int)pCDA->camera_y + pCDA->view_height / 2;

	if (start_x_index < 0)
	{
		start_draw_x -= start_x_index * grid_view_size;
		start_x_index = 0;
	}
	else if (start_x_index>=pCDA->grid_x_count)
	{
		return;
	}
		
	if (start_y_index < 0)
	{
		start_draw_y -= start_y_index * grid_view_size;
		start_y_index = 0;
	}
	else if (start_y_index>=pCDA->grid_y_count)
	{
		return;
	}

		
	draw_y = start_draw_y;
	index_y=start_y_index;
	while (draw_y<pCDA->view_height)
	{
		draw_x=start_draw_x;
		index_x=start_x_index;

		if (index_y >= pCDA->grid_y_count)
		{
			break;
		}
		while (draw_x<pCDA->view_width)
		{
			if (index_x >= pCDA->grid_x_count)
			{
				break;
			}
			PX_CDA_RenderGrid(psurface, pCDA, draw_x, draw_y, grid_view_size, index_x + index_y *pCDA->grid_x_count);
			index_x++;
			draw_x+=grid_view_size;
		}
		index_y++;
		draw_y+= grid_view_size;
	}

	//draw grid line
	//draw horizontal lines
	draw_x = start_draw_x;
	draw_y = start_draw_y;
	px_int max_h_length = (pCDA->grid_x_count - start_x_index) * pCDA->grid_size;
	px_int max_v_length = (pCDA->grid_y_count - start_y_index) * pCDA->grid_size;
	while (draw_y < pCDA->view_height&& max_v_length>=0)
	{
		PX_GeoDrawLine(psurface, draw_x, draw_y, draw_x + max_h_length, draw_y, 1, pCDA->grid_line_color);
		draw_y += grid_view_size;
		max_v_length -= grid_view_size;
	}

	
	//draw vertical lines
	draw_x = start_draw_x;
	draw_y = start_draw_y;
	max_h_length = (pCDA->grid_x_count - start_x_index) * pCDA->grid_size;
	max_v_length = (pCDA->grid_y_count - start_y_index) * pCDA->grid_size;
	while (draw_x < pCDA->view_width&&max_h_length>=0)
	{
		PX_GeoDrawLine(psurface, draw_x, draw_y, draw_x, draw_y + max_v_length, 1, pCDA->grid_line_color);
		draw_x += grid_view_size;
		max_h_length -= grid_view_size;
	}
}

px_void PX_CDA_RenderObjects(px_surface* psurface, PX_CDA* pCDA,px_dword elapsed)
{
	px_int i;
	px_int offset_x, offset_y;
	offset_x = (px_int)(pCDA->camera_x - pCDA->view_width / 2);
	offset_y = (px_int)(pCDA->camera_y - pCDA->view_height / 2);

	for (i = 0; i < pCDA->pObjects.size; i++)
	{
		PX_Object* pObject = *PX_VECTORAT(PX_Object*, &pCDA->pObjects, i);
		PX_CDA_Object* pDesc = PX_ObjectGetDesc(PX_CDA_Object, pObject);

		pObject->x = pDesc->grid_x * pCDA->grid_size- offset_x*1.f;
		pObject->y = pDesc->grid_y * pCDA->grid_size- offset_y*1.f;

		pObject->Width = pDesc->grid_width * pCDA->grid_size * 1.f;
		pObject->Height = pDesc->grid_height * pCDA->grid_size * 1.f;

		if (pObject->Visible == PX_FALSE)
		{
			continue;
		}
		PX_ObjectRender(psurface, pObject, elapsed);
		switch (pDesc->mode)
		{
			case PX_CDA_OBJECT_MODE_NORMAL:
			//PX_CDAClildRender(pCDA, psurface, pObject, elapsed, offset_x, offset_y);
			break;
			case PX_CDA_OBJECT_MODE_CURSOR:
			//render a border
			PX_GeoDrawBorder(psurface, (px_int)(pObject->x-pObject->Width / 2), (px_int)(pObject->y-pObject->Height/2), (px_int)(pObject->x + pObject->Width/2), (px_int)(pObject->y + pObject->Height/2), 2, PX_COLOR(255, 255, 0, 0));
			break;
			case PX_CDA_OBJECT_MODE_SELECT:
			//render a border
				PX_GeoDrawBorder(psurface, (px_int)(pObject->x - pObject->Width / 2), (px_int)(pObject->y - pObject->Height / 2), (px_int)(pObject->x + pObject->Width/2), (px_int)(pObject->y + pObject->Height/2), 2, PX_COLOR(255, 0, 0, 255));
			break;
			default:
				break;
		}
	}
}

px_void PX_CDA_Render(px_surface *psurface,PX_CDA *pCDA,px_dword elapsed)
{
	//redner background
	PX_CDA_RenderBackgroundTexture(psurface, pCDA);
	
	//render grid
	PX_CDA_RenderGrids(psurface, pCDA);

	//render routes
	PX_CDA_RenderRoutes(psurface, pCDA);

	//render objects
	PX_CDA_RenderObjects(psurface, pCDA, elapsed);
}

px_bool PX_CDA_BeginRouteEdit(PX_CDA* pCDA, px_int start_grid_x, px_int start_grid_y, PX_CDA_RouteClass* pRouteClass)
{
	if (pCDA->editing_route.routes.mp)
	{
		PX_ERROR("logic failed:Route is editing");
		return PX_FALSE;
	}
	PX_memset(&pCDA->editing_route, 0, sizeof(pCDA->editing_route));
	pCDA->editing_route.start_grid_x = start_grid_x;
	pCDA->editing_route.start_grid_y = start_grid_y;
	pCDA->editing_route.end_grid_x = start_grid_x;
	pCDA->editing_route.end_grid_y = start_grid_y;
	pCDA->editing_route.prouteClass= pRouteClass;
	if (!PX_VectorInitialize(pCDA->mp, &pCDA->editing_route.routes, sizeof(px_dword), 32))
	{
		return PX_FALSE;
	}

	pCDA->editing_route_pin = 0;
	return PX_TRUE;
}

px_void PX_CDA_Optimize(PX_CDA* pCDA)
{
	if (pCDA->editing_route.routes.mp)
	{
		px_int i;
		
		for (i = 1; i < pCDA->editing_route.routes.size; i++)
		{
			PX_CDA_RouteLine* pCurrent=PX_VECTORAT(PX_CDA_RouteLine, &pCDA->editing_route.routes, i);
			PX_CDA_RouteLine* pLast = PX_VECTORAT(PX_CDA_RouteLine, &pCDA->editing_route.routes, i-1);
			switch (pCurrent->direction)
			{
				case 8:
				{
				    if (pLast->direction == 8)
					{
						pLast->length += pCurrent->length;
						PX_VectorErase(&pCDA->editing_route.routes, i);
						i--;
					}
					else if (pLast->direction==2)
					{
						if (pLast->length>pCurrent->length)
						{
							pLast->length -= pCurrent->length;
							PX_VectorErase(&pCDA->editing_route.routes, i);
							i--;
						}
						else if (pLast->length == pCurrent->length)
						{
							PX_VectorErase(&pCDA->editing_route.routes, i);
							PX_VectorErase(&pCDA->editing_route.routes, i-1);
							i-=2;
						}
						else
						{
							pCurrent->length -= pLast->length;
							PX_VectorErase(&pCDA->editing_route.routes, i-1);
							i--;
						}
					}
				}
				break;
				case 2:
				{
					if (pLast->direction == 2)
					{
						pLast->length += pCurrent->length;
						PX_VectorErase(&pCDA->editing_route.routes, i);
						i--;
					}
					else if (pLast->direction == 8)
					{
						if (pLast->length>pCurrent->length)
						{
							pLast->length -= pCurrent->length;
							PX_VectorErase(&pCDA->editing_route.routes, i);
							i--;
						}
						else if (pLast->length == pCurrent->length)
						{
							PX_VectorErase(&pCDA->editing_route.routes, i);
							PX_VectorErase(&pCDA->editing_route.routes, i - 1);
							i -= 2;
						}
						else
						{
							pCurrent->length -= pLast->length;
							PX_VectorErase(&pCDA->editing_route.routes, i - 1);
							i--;
						}
					}
				}
				break;
				case 4:
				{
					if (pLast->direction == 4)
					{
						pLast->length += pCurrent->length;
						PX_VectorErase(&pCDA->editing_route.routes, i);
						i--;
					}
					else if (pLast->direction == 6)
					{
						if (pLast->length>pCurrent->length)
						{
							pLast->length -= pCurrent->length;
							PX_VectorErase(&pCDA->editing_route.routes, i);
							i--;
						}
						else if (pLast->length == pCurrent->length)
						{
							PX_VectorErase(&pCDA->editing_route.routes, i);
							PX_VectorErase(&pCDA->editing_route.routes, i - 1);
							i -= 2;
						}
						else
						{
							pCurrent->length -= pLast->length;
							PX_VectorErase(&pCDA->editing_route.routes, i - 1);
							i--;
						}
					}
				}
				break;
				case 6:
				{
					if (pLast->direction == 6)
					{
						pLast->length += pCurrent->length;
						PX_VectorErase(&pCDA->editing_route.routes, i);
						i--;
					}
					else if (pLast->direction == 4)
					{
						if (pLast->length>pCurrent->length)
						{
							pLast->length -= pCurrent->length;
							PX_VectorErase(&pCDA->editing_route.routes, i);
							i--;
						}
						else if (pLast->length == pCurrent->length)
						{
							PX_VectorErase(&pCDA->editing_route.routes, i);
							PX_VectorErase(&pCDA->editing_route.routes, i - 1);
							i -= 2;
						}
						else
						{
							pCurrent->length -= pLast->length;
							PX_VectorErase(&pCDA->editing_route.routes, i - 1);
							i--;
						}
					}
				}
				break;
				case 7:
				{
					if (pLast->direction == 7)
					{
						pLast->length += pCurrent->length;
						PX_VectorErase(&pCDA->editing_route.routes, i);
						i--;
					}
					else if (pLast->direction == 3)
					{
						if (pLast->length>pCurrent->length)
						{
							pLast->length -= pCurrent->length;
							PX_VectorErase(&pCDA->editing_route.routes, i);
							i--;
						}
						else if (pLast->length == pCurrent->length)
						{
							PX_VectorErase(&pCDA->editing_route.routes, i);
							PX_VectorErase(&pCDA->editing_route.routes, i - 1);
							i -= 2;
						}
						else
						{
							pCurrent->length -= pLast->length;
							PX_VectorErase(&pCDA->editing_route.routes, i - 1);
							i--;
						}
					}
				}
				break;
				case 9:
				{
					if (pLast->direction == 9)
					{
						pLast->length += pCurrent->length;
						PX_VectorErase(&pCDA->editing_route.routes, i);
						i--;
					}
					else if (pLast->direction == 1)
					{
						if (pLast->length>pCurrent->length)
						{
							pLast->length -= pCurrent->length;
							PX_VectorErase(&pCDA->editing_route.routes, i);
							i--;
						}
						else if (pLast->length == pCurrent->length)
						{
							PX_VectorErase(&pCDA->editing_route.routes, i);
							PX_VectorErase(&pCDA->editing_route.routes, i - 1);
							i -= 2;
						}
						else
						{
							pCurrent->length -= pLast->length;
							PX_VectorErase(&pCDA->editing_route.routes, i - 1);
							i--;
						}
					}
				}
				break;
				case 1:
				{
					if (pLast->direction == 1)
					{
						pLast->length += pCurrent->length;
						PX_VectorErase(&pCDA->editing_route.routes, i);
						i--;
					}
					else if (pLast->direction == 9)
					{
						if (pLast->length>pCurrent->length)
						{
							pLast->length -= pCurrent->length;
							PX_VectorErase(&pCDA->editing_route.routes, i);
							i--;
						}
						else if (pLast->length == pCurrent->length)
						{
							PX_VectorErase(&pCDA->editing_route.routes, i);
							PX_VectorErase(&pCDA->editing_route.routes, i - 1);
							i -= 2;
						}
						else
						{
							pCurrent->length -= pLast->length;
							PX_VectorErase(&pCDA->editing_route.routes, i - 1);
							i--;
						}
					}
				}
				break;
				case 3:
				{
					if (pLast->direction == 3)
					{
						pLast->length += pCurrent->length;
						PX_VectorErase(&pCDA->editing_route.routes, i);
						i--;
					}
					else if (pLast->direction == 7)
					{
						if (pLast->length>pCurrent->length)
						{
							pLast->length -= pCurrent->length;
							PX_VectorErase(&pCDA->editing_route.routes, i);
							i--;
						}
						else if (pLast->length == pCurrent->length)
						{
							PX_VectorErase(&pCDA->editing_route.routes, i);
							PX_VectorErase(&pCDA->editing_route.routes, i - 1);
							i -= 2;
						}
						else
						{
							pCurrent->length -= pLast->length;
							PX_VectorErase(&pCDA->editing_route.routes, i - 1);
							i--;
						}
					}
				}
				break;
			default:
				break;
			}
		}
	}
	PX_CDA_UpdateRouteStartEndGrid(pCDA, &pCDA->editing_route);
}

px_bool PX_CDA_RouteMoving(PX_CDA* pCDA, px_int grid_x, px_int grid_y)
{
	if (pCDA->editing_route.routes.mp)
	{
			px_int i;
			PX_CDA_RouteLine* p = (PX_CDA_RouteLine*)pCDA->editing_route.routes.data;
			px_int x, y, dx, dy;
			px_float scale = pCDA->grid_size * 1.0f / PX_CDA_GRID_DEFAULE_SIZE;
			px_int line_width = (px_int)(pCDA->editing_route.prouteClass->linewidth * scale);
			if (line_width < 1)line_width = 1;

			PX_VectorPopN(&pCDA->editing_route.routes, pCDA->editing_route.routes.size - pCDA->editing_route_pin);

			x = pCDA->editing_route.start_grid_x;
			y = pCDA->editing_route.start_grid_y;

			for (i = 0; i < pCDA->editing_route.routes.size; i++)
			{
				px_byte direction = p[i].direction;
				px_short length = p[i].length;
				switch (direction)
				{
				case 8:
					y = y - length;
					//x = x;
					break;
				case 2:
					y = y + length;
					//x = x;
					break;
				case 4:
					//y = y;
					x = x - length;
					break;
				case 6:
					//y = y;
					x = x + length;
					break;
				case 7:
					y = y - length;
					x = x - length;
					break;
				case 9:
					y = y - length;
					x = x + length;
					break;
				case 1:
					y = y + length;
					x = x - length;
					break;
				case 3:
					y = y + length;
					x = x + length;
					break;
				default:
					PX_ERROR("Route data error");
					break;
				}

			}

			dx = grid_x-x;
			dy = grid_y-y;
			if (dx != 0 || dy != 0)
			{
				PX_CDA_RouteLine line;
				if (PX_ABS(dx) == PX_ABS(dy))
				{
					if (dx < 0 && dy < 0)	line.direction = 7;
					if (dx > 0 && dy < 0)	line.direction = 9;
					if (dx < 0 && dy > 0)	line.direction = 1;
					if (dx > 0 && dy > 0)	line.direction = 3;
					line.length = PX_ABS(dx);
					PX_VectorPushback(&pCDA->editing_route.routes, &line);
				}
				else if (PX_ABS(dx) > PX_ABS(dy))
				{
					if (dx < 0) line.direction = 4;
					else line.direction = 6;
					line.length = PX_ABS(dx);
					PX_VectorPushback(&pCDA->editing_route.routes, &line);
					if (dy)
					{
						if (dy < 0) line.direction = 8;
						else line.direction = 2;
						line.length = PX_ABS(dy);
						PX_VectorPushback(&pCDA->editing_route.routes, &line);
					}
					
				}
				else
				{
					if (dy < 0) line.direction = 8;
					else line.direction = 2;
					line.length = PX_ABS(dy);
					PX_VectorPushback(&pCDA->editing_route.routes, &line);
					if (dx)
					{
						if (dx < 0) line.direction = 4;
						else line.direction = 6;
						line.length = PX_ABS(dx);
						PX_VectorPushback(&pCDA->editing_route.routes, &line);
					}
					
				}
			}
	}
	PX_CDA_UpdateRouteStartEndGrid(pCDA, &pCDA->editing_route);
	return PX_TRUE;
}

px_void PX_CDA_MoveObject(PX_CDA* pCDA, px_int index, px_float grid_x, px_float grid_y)
{
	if (index>=0&&index<pCDA->pObjects.size)
	{
		PX_Object* pObject = *PX_VECTORAT(PX_Object*, &pCDA->pObjects, index);
		PX_CDA_Object* pDesc = PX_ObjectGetDesc(PX_CDA_Object, pObject);
		pDesc->grid_x = grid_x;
		pDesc->grid_y = grid_y;
	}
}

px_void PX_CDA_MoveSelectObject(PX_CDA* pCDA, px_float grid_x, px_float grid_y)
{
	px_int i;
	for (i = 0; i < pCDA->pObjects.size; i++)
	{
		PX_Object* pObject = *PX_VECTORAT(PX_Object*, &pCDA->pObjects, i);
		PX_CDA_Object* pDesc = PX_ObjectGetDesc(PX_CDA_Object, pObject);
		if (pDesc->mode == PX_CDA_OBJECT_MODE_SELECT)
		{
			PX_CDA_MoveObject(pCDA, i, grid_x, grid_y);
			return;
		}
	}
}

px_bool PX_CDA_RoutePin(PX_CDA* pCDA)
{
	if (pCDA->editing_route.routes.mp)
	{
		PX_CDA_Optimize(pCDA);
		pCDA->editing_route_pin = pCDA->editing_route.routes.size;
		return PX_TRUE;
	}
	return PX_FALSE;
}

px_bool PX_CDA_CancelRouteEdit(PX_CDA* pCDA)
{
	PX_ASSERTIF (pCDA->editing_route.routes.mp==0)
	PX_VectorFree(&pCDA->editing_route.routes);
	PX_memset(&pCDA->editing_route, 0, sizeof(pCDA->editing_route));
	pCDA->editing_route_pin = 0;
	return PX_TRUE;
}

px_texture * PX_CDA_TakeSnapShot(PX_CDA* pCDA)
{
	px_int snapshot_width = pCDA->grid_x_count * pCDA->grid_size;
	px_int snapshot_height = pCDA->grid_y_count * pCDA->grid_size;
	px_int old_view_width = pCDA->view_width;
	px_int old_view_height = pCDA->view_height;
	px_float old_camera_x = pCDA->camera_x;
	px_float old_camera_y = pCDA->camera_y;
	
	//refresh snapshot texture
	if (!pCDA->snapshot_texture.MP)
	{
		px_int snapshot_width = pCDA->grid_x_count * pCDA->grid_size;
		px_int snapshot_height = pCDA->grid_y_count * pCDA->grid_size;
		if (!PX_TextureCreate(pCDA->mp, &pCDA->snapshot_texture, snapshot_width, snapshot_height))
			return PX_NULL;
		PX_SurfaceClearAll(&pCDA->snapshot_texture, PX_COLOR_WHITE);
	}
	else
	{
		PX_SurfaceClearAll(&pCDA->snapshot_texture, PX_COLOR_WHITE);
	}

	pCDA->view_width = snapshot_width;
	pCDA->view_height = snapshot_height;
	pCDA->camera_x = snapshot_width / 2.f;
	pCDA->camera_y = snapshot_height / 2.f;

	PX_CDA_Render(&pCDA->snapshot_texture, pCDA, 0);

	pCDA->view_width = old_view_width;
	pCDA->view_height = old_view_height;
	pCDA->camera_x = old_camera_x;
	pCDA->camera_y = old_camera_y;
	return &pCDA->snapshot_texture;
}

px_void PX_CDA_PostEvent(PX_CDA* pCDA, PX_Object_Event e)
{
	PX_Object* pObject;
	px_int i;
	for (i = 0; i < pCDA->pObjects.size; i++)
	{
		pObject = PX_VECTORAT(PX_Object, &pCDA->pObjects, i);
		PX_ObjectPostEvent(pObject, e);
	}
}

px_bool PX_CDA_AddObjectClass(PX_CDA* pCDA, PX_CDA_ObjectClass *pclass)
{
	PX_CDA_ObjectClass *pNewClass=(PX_CDA_ObjectClass *)MP_Malloc(pCDA->mp,sizeof(PX_CDA_ObjectClass));
	if (pNewClass==PX_NULL)
	{
		return PX_FALSE;
	}
	*pNewClass= *pclass;
	if (PX_MapPut(&pCDA->objectClasses,(const px_byte *)pclass->name,PX_strlen(pclass->name),pNewClass)!=PX_HASHMAP_RETURN_OK)
	{
		MP_Free(pCDA->mp,pNewClass);
		return PX_FALSE;
	}
	return PX_TRUE;
}

px_bool PX_CDA_AddRouteClass(PX_CDA* pCDA, PX_CDA_RouteClass* pclass)
{
	PX_CDA_RouteClass* pNewClass = (PX_CDA_RouteClass*)MP_Malloc(pCDA->mp, sizeof(PX_CDA_RouteClass));
	if (pNewClass == PX_NULL)
	{
		return PX_FALSE;
	}
	*pNewClass = *pclass;
	if (PX_MapPut(&pCDA->routeClasses, (const px_byte*)pclass->name, PX_strlen(pclass->name), pNewClass) != PX_HASHMAP_RETURN_OK)
	{
		MP_Free(pCDA->mp, pNewClass);
		return PX_FALSE;
	}
	return PX_TRUE;
}

PX_OBJECT_RENDER_FUNCTION(PX_CDA_Object_Render)
{
	PX_CDA_Object *pCDADesc=PX_ObjectGetDesc(PX_CDA_Object,pObject);
	PX_CDA* pCDA = (PX_CDA*)pCDADesc->pCDA;
	px_float scale = 1;
	px_float angle = 0;

	if (!pCDADesc->pObjectClass->static_scale&&pCDADesc->grid_width* PX_CDA_GRID_DEFAULE_SIZE!=(px_int)pObject->Width)
	{
		scale = pObject->Width / (pCDADesc->grid_width * PX_CDA_GRID_DEFAULE_SIZE);
	}

	switch (pCDADesc->pObjectClass->display_type)
	{
		case PX_OBJECT_CDA_DISPLAY_TYPE_TEXTURE:
		{
				PX_TextureRenderEx(psurface,pCDADesc->pTexture,(px_int)pObject->x,(px_int)pObject->y,pCDADesc->pObjectClass->render_align,0,scale,angle);
		}
		break;
		case PX_OBJECT_CDA_DISPLAY_TYPE_STRING:
		{
			PX_FontModuleDrawText(psurface,pCDADesc->pObjectClass->fontmodule,(px_int)pObject->x,(px_int)pObject->y,pCDADesc->pObjectClass->render_align,pCDADesc->text.buffer,pCDADesc->pObjectClass->fontcolor);
		}
		break;
		case PX_OBJECT_CDA_DISPLAY_TYPE_GIF:
		{
			px_texture* ptexture;
			PX_GifUpdate(&pCDADesc->gif,elapsed);
			ptexture=PX_GifGetTexture(&pCDADesc->gif);
			if (ptexture)
			{
				PX_TextureRenderEx(psurface,ptexture,(px_int)pObject->x,(px_int)pObject->y,pCDADesc->pObjectClass->render_align,0, scale, angle);
			}
		}
		break;
		case PX_OBJECT_CDA_DISPLAY_TYPE_2DX:
		{
			
			if (pCDADesc->animation.linker)
			{
				px_texture* pTexture;
				PX_AnimationUpdate(&pCDADesc->animation, elapsed);
				pTexture= PX_AnimationGetCurrentTexture(&pCDADesc->animation);
				if (pTexture)
				{
					PX_TextureRenderEx(psurface, pTexture, (px_int)pObject->x, (px_int)pObject->y, pCDADesc->pObjectClass->render_align, 0, scale, angle);
				}
			}
		}
		break;
	default:
		break;
	}

	if (pCDA->show_controller_size)
	{
		PX_GeoDrawRect(psurface, (px_int)(pObject->x - pObject->Width / 2), (px_int)(pObject->y - pObject->Height / 2), (px_int)(pObject->x + pObject->Width / 2), (px_int)(pObject->y + pObject->Height / 2), PX_COLOR(32, 0, 255, 0));
		//border
		PX_GeoDrawBorder(psurface, (px_int)(pObject->x - pObject->Width / 2), (px_int)(pObject->y - pObject->Height / 2), (px_int)(pObject->x + pObject->Width / 2), (px_int)(pObject->y + pObject->Height / 2), 1, PX_COLOR(255, 255, 0, 0));
	}

	if (pCDA->show_id_text)
	{
		PX_FontModuleDrawText(psurface, 0, (px_int)pObject->x, (px_int)pObject->y, PX_ALIGN_CENTER, pObject->id, PX_COLOR(255, 255, 0, 0));
	}
}

PX_OBJECT_UPDATE_FUNCTION(PX_CDA_Object_Update)
{
	PX_CDA_Object* pCDADesc = PX_ObjectGetDesc(PX_CDA_Object, pObject);
	if(pCDADesc->vm.mp)
		PX_VMRun(&pCDADesc->vm,0xffff,elapsed);
}

PX_OBJECT_FREE_FUNCTION(PX_CDA_Object_Free)
{
	PX_CDA_Object* pCDADesc = PX_ObjectGetDesc(PX_CDA_Object, pObject);
	
	switch (pCDADesc->pObjectClass->display_type)
	{
	case PX_OBJECT_CDA_DISPLAY_TYPE_TEXTURE:
	break;
	case PX_OBJECT_CDA_DISPLAY_TYPE_STRING:
	{
		PX_StringFree(&pCDADesc->text);
	}
	break;
	case PX_OBJECT_CDA_DISPLAY_TYPE_GIF:
	{
		PX_GifFree(&pCDADesc->gif);
	}
	break;
	case PX_OBJECT_CDA_DISPLAY_TYPE_2DX:
	{
		PX_AnimationFree(&pCDADesc->animation);
	}
	break;
	default:
		break;
	}

	if (pCDADesc->vm.mp)
	{
		PX_VMFree(&pCDADesc->vm);
	}
}

PX_Object* PX_CDA_CreateClassObjectEx(PX_CDA* pCDA, px_char classname[], px_float grid_x, px_float grid_y)
{
	PX_CDA_ObjectClass* pObjectClass = (PX_CDA_ObjectClass*)PX_MapGet(&pCDA->objectClasses, (const px_byte*)classname, PX_strlen(classname));
	if (pObjectClass)
	{
		px_char reskey[32] = { 0 };
		PX_CDA_Object* pDesc;
		PX_Object* pObject = PX_ObjectCreateEx(pCDA->mp, 0, 0, 0, 0, 0, 0, 0, 0, PX_CDA_Object_Update, PX_CDA_Object_Render, PX_CDA_Object_Free, 0, sizeof(PX_CDA_Object));
		if (!pObject)return PX_NULL;
		PX_BufferToHexString((px_byte*)&pCDA->id_alloc, 4, pObject->id);
		pCDA->id_alloc++;
		pDesc = PX_ObjectGetDesc(PX_CDA_Object, pObject);
		pDesc->pCDA = pCDA;
		pDesc->grid_x = grid_x;
		pDesc->grid_y = grid_y;
		pDesc->grid_width = pObjectClass->grid_width;
		pDesc->grid_height = pObjectClass->grid_height;
		pDesc->pObjectClass = pObjectClass;

		PX_sprintf1(reskey, sizeof(reskey), "%1_display", PX_STRINGFORMAT_STRING(pObjectClass->name));
		switch (pDesc->pObjectClass->display_type)
		{
		case PX_OBJECT_CDA_DISPLAY_TYPE_TEXTURE:
		{
			pDesc->pTexture = PX_ResourceLibraryGetTexture(pCDA->presourceLibrary, reskey);
			if (!pDesc->pTexture)
			{
				PX_LOG("invalid texture data");
				return PX_NULL;
			}
		}
		break;
		case PX_OBJECT_CDA_DISPLAY_TYPE_STRING:
		{
			if (!PX_StringInitialize(pCDA->mp, &pDesc->text))
				return PX_NULL;
		}
		break;
		case PX_OBJECT_CDA_DISPLAY_TYPE_GIF:
		{
			px_memory* pdata = PX_ResourceLibraryGetData(pCDA->presourceLibrary, reskey);
			if (!PX_GifCreate(pCDA->mp, &pDesc->gif, pdata->buffer, pdata->usedsize))
			{
				PX_LOG("invalid gif data");
				return PX_NULL;
			}
		}
		break;
		case PX_OBJECT_CDA_DISPLAY_TYPE_2DX:
		{
			PX_AnimationLibrary* pdata = PX_ResourceLibraryGetAnimationLibrary(pCDA->presourceLibrary, reskey);
			if (!pdata)
			{
				PX_LOG("invalid 2dx data");
				return PX_NULL;
			}
			if (!PX_AnimationCreate(&pDesc->animation, pdata))
			{
				PX_LOG("invalid 2dx data");
			}
		}
		break;
		default:
			break;
		}


		if (pObjectClass->vmbin.usedsize)
		{
			if (PX_VMInitialize(&pDesc->vm, pCDA->mp, pObjectClass->vmbin.buffer, pObjectClass->vmbin.usedsize))
			{
				PX_VMRegistHostFunction(&pDesc->vm, "sleep", PX_CDA_ObjectVM_Sleep, pObject);//Sleep
				PX_VMRegistHostFunction(&pDesc->vm, "rand", PX_CDA_ObjectVM_Rand, pObject);//Rand
				PX_VMRegistHostFunction(&pDesc->vm, "sin", PX_CDA_ObjectVM_Sin, pObject);//Sin
				PX_VMRegistHostFunction(&pDesc->vm, "cos", PX_CDA_ObjectVM_Cos, pObject);//Cos
				PX_VMRegistHostFunction(&pDesc->vm, "sqrt", PX_CDA_ObjectVM_Sqrt, pObject);//sqrt
				PX_VMRegistHostFunction(&pDesc->vm, "getCDAsize", PX_CDA_ObjectVM_GetCDASize, pObject);//getCDAsize
				PX_VMRegistHostFunction(&pDesc->vm, "getobjectname", PX_CDA_ObjectVM_GetObjectName, pObject);//getobjectname
				PX_VMRegistHostFunction(&pDesc->vm, "setobjectname", PX_CDA_ObjectVM_SetObjectName, pObject);//setobjectname
				PX_VMRegistHostFunction(&pDesc->vm, "setobjectposition", PX_CDA_ObjectVM_SetObjectPosition, pObject);//setobjectvelocity
				PX_VMRegistHostFunction(&pDesc->vm, "getobjectposition", PX_CDA_ObjectVM_GetObjectPosition, pObject);//setobjectvelocity
				PX_VMRegistHostFunction(&pDesc->vm, "setobjectvelocity", PX_CDA_ObjectVM_SetObjectVelocity, pObject);//setobjectvelocity
				PX_VMRegistHostFunction(&pDesc->vm, "setobjectforce", PX_CDA_ObjectVM_SetObjectForce, pObject);//setobjectforce
				PX_VMRegistHostFunction(&pDesc->vm, "setobjectresistance", PX_CDA_ObjectVM_SetObjectResistance, pObject);//setobjectresistance
				PX_VMRegistHostFunction(&pDesc->vm, "abi_int", PX_CDA_ObjectVM_Abi_int, pObject);//abi_int
				PX_VMRegistHostFunction(&pDesc->vm, "abi_float", PX_CDA_ObjectVM_Abi_float, pObject);//abi_float
				PX_VMRegistHostFunction(&pDesc->vm, "abi_string", PX_CDA_ObjectVM_Abi_string, pObject);//abi_string
				PX_VMRegistHostFunction(&pDesc->vm, "abi_point", PX_CDA_ObjectVM_Abi_point, pObject);//abi_point
				PX_VMRegistHostFunction(&pDesc->vm, "abi_color", PX_CDA_ObjectVM_Abi_color, pObject);//abi_color
				PX_VMRegistHostFunction(&pDesc->vm, "createthread", PX_CDA_ObjectVM_CreateThread, pObject);//createthread
				PX_VMBeginThreadFunction(&pDesc->vm, 0, "main", 0, 0);
			}
		}
		PX_CDA_AddObject(pCDA, pObject);
		return pObject;
	}
	return PX_NULL;

}



px_bool PX_CDA_ExecutePayload_CreateObject(PX_CDA* pcda,PX_Json *pjson)
{
	const px_char* content;
	px_dword grid_x_dw, grid_y_dw;
	px_float grid_x, grid_y;
	px_char classname[32] = { 0 };
	content =PX_JsonGetString(pjson, "classname");
	if (!content)
	{
		PX_ASSERT();
		return PX_FALSE;
	}
	PX_strcpy(classname, content, sizeof(classname));
	content=PX_JsonGetString(pjson, "grid_x");
	if (!content)
	{
		PX_ASSERT();
		return PX_FALSE;
	}
	PX_HexStringToBuffer(content, (px_byte *) & grid_x_dw);
	grid_x = *((px_float*)&grid_x_dw);
	content = PX_JsonGetString(pjson, "grid_y");
	if (!content)
	{
		PX_ASSERT();
		return PX_FALSE;
	}
	PX_HexStringToBuffer(content, (px_byte*)&grid_y_dw);
	grid_y = *((px_float*)&grid_y_dw);
	PX_CDA_CreateClassObjectEx(pcda, classname, grid_x, grid_y);
	return PX_TRUE;
}

px_bool PX_CDA_ExecutePayload_DeleteObject(PX_CDA* pcda, PX_Json* pjson)
{
	px_int index;
	const px_char* content;
	content = PX_JsonGetString(pjson, "index");
	if (!content)
	{
		PX_ASSERT();
		return PX_FALSE;
	}
	index = PX_atoi(content);
	PX_CDA_DeleteObject(pcda, index);
	return PX_TRUE;
}

px_bool PX_CDA_ExecutePayload_MoveObject(PX_CDA* pcda, PX_Json* pjson)
{
	const px_char* content;
	px_dword grid_x_dw, grid_y_dw;
	px_float grid_x, grid_y;
	px_int index;

	content = PX_JsonGetString(pjson, "index");
	if (!content)
	{
		PX_ASSERT();
		return PX_FALSE;
	}
	index = PX_atoi(content);
	
	content = PX_JsonGetString(pjson, "grid_x");
	if (!content)
	{
		PX_ASSERT();
		return PX_FALSE;
	}
	PX_HexStringToBuffer(content, (px_byte*)&grid_x_dw);
	grid_x = *((px_float*)&grid_x_dw);
	content = PX_JsonGetString(pjson, "grid_y");
	if (!content)
	{
		PX_ASSERT();
		return PX_FALSE;
	}
	PX_HexStringToBuffer(content, (px_byte*)&grid_y_dw);
	grid_y = *((px_float*)&grid_y_dw);
	PX_CDA_MoveObject(pcda, index, grid_x, grid_y);
	return PX_TRUE;
}

px_bool PX_CDA_ExecutePayload_CreateRoute(PX_CDA* pcda, PX_Json* pjson)
{
	PX_CDA_Route route = {0};
	PX_CDA_RouteClass *pRouteClass;
	const px_char* content;
	px_int grid_x, grid_y;
	px_char classname[32] = { 0 };
	PX_Json_Value* pvalue;
	px_list_node *pNode;
	content = PX_JsonGetString(pjson, "classname");
	if (!content)
	{
		PX_ASSERT();
		return PX_FALSE;
	}
	PX_strcpy(classname, content, sizeof(classname));

	pRouteClass= (PX_CDA_RouteClass*)PX_MapGet(&pcda->routeClasses, (const px_byte*)classname, PX_strlen(classname));
	if (!pRouteClass)
	{
		PX_ASSERT();
		return PX_FALSE;
	}
	route.prouteClass = pRouteClass;

	content = PX_JsonGetString(pjson, "start_grid_x");
	if (!content)
	{
		PX_ASSERT();
		return PX_FALSE;
	}
	grid_x =PX_atoi(content);
	route.start_grid_x=grid_x;
	content = PX_JsonGetString(pjson, "start_grid_y");
	if (!content)
	{
		PX_ASSERT();
		return PX_FALSE;
	}
	grid_y = PX_atoi(content);
	route.start_grid_y = grid_y;

	pvalue = PX_JsonGetValue(pjson, "payload");
	if (!pvalue||pvalue->type!=PX_JSON_VALUE_TYPE_ARRAY)
	{
		return PX_FALSE;
	}
	PX_VectorInitialize(pcda->mp, &route.routes, sizeof(PX_CDA_RouteLine), pvalue->_array.size/2);
	PX_VectorInitialize(pcda->mp, &route.signals, sizeof(PX_CDA_Signal), 0);
	if (pvalue->_array.size)
	{
		pNode = pvalue->_array.head;
		while (pNode)
		{
			PX_CDA_RouteLine line;
			PX_Json_Value* pValue = PX_LIST_NODEDATA(pNode);
			if (pValue->type!=PX_JSON_VALUE_TYPE_NUMBER)
			{
				return PX_FALSE;
			}
			line.direction=(px_byte)pValue->_number;
			pNode = pNode->pnext;
			if (!pNode)
			{
				return PX_FALSE;
			}
			pValue = PX_LIST_NODEDATA(pNode);
			if (pValue->type != PX_JSON_VALUE_TYPE_NUMBER)
			{
				return PX_FALSE;
			}
			line.length = (px_word)pValue->_number;
			line.type = 0;
			route.length += line.length;
			PX_VectorPushback(&route.routes, &line);
			pNode = pNode->pnext;
		}
	}
	PX_CDA_UpdateRouteStartEndGrid(pcda, &route);
	if (!PX_VectorPushback(&pcda->routes, &route))
		return PX_FALSE;
	return PX_TRUE;
}

px_bool PX_CDA_ExecutePayload_DeleteRoute(PX_CDA* pcda, PX_Json* pjson)
{
	px_int index;
	const px_char* content;
	content = PX_JsonGetString(pjson, "index");
	if (!content)
	{
		PX_ASSERT();
		return PX_FALSE;
	}
	index = PX_atoi(content);
	PX_CDA_DeleteRoute(pcda, index);
	return PX_TRUE;
}

px_bool PX_CDA_ExecutePayload(PX_CDA* pcda,px_bool ClearForward)
{
	px_byte cache[8 * 1024] = { 0 };
	px_memorypool mp;
	PX_Json json;
	const px_char* opcode;
	if (pcda->ip< pcda->payload.usedsize)
	{
		mp = MP_Create(cache, sizeof(cache));
		PX_JsonInitialize( &mp, &json);

		if (!PX_JsonParse(&json, pcda->payload.buffer + pcda->ip))
		{
			PX_ASSERT();
		}
		pcda->ip += PX_strlen(pcda->payload.buffer + pcda->ip) + 1;
		if (ClearForward)
		{
			pcda->payload.usedsize = pcda->ip;
		}

		
		opcode = PX_JsonGetString(&json, "opcode");
		if (PX_strequ(opcode, "createobject"))
		{
			return PX_CDA_ExecutePayload_CreateObject(pcda,&json);
		}
		else if (PX_strequ(opcode, "deleteobject"))
		{
			return PX_CDA_ExecutePayload_DeleteObject(pcda, &json);
		}
		else if (PX_strequ(opcode, "moveobject"))
		{
			return PX_CDA_ExecutePayload_MoveObject(pcda, &json);
		}
		else if (PX_strequ(opcode, "createroute"))
		{
			return PX_CDA_ExecutePayload_CreateRoute(pcda, &json);
		}
		else if (PX_strequ(opcode, "deleteroute"))
		{
			return PX_CDA_ExecutePayload_DeleteRoute(pcda, &json);
		}
		else
		{
			PX_ASSERT();
		}
		PX_JsonFree(&json);
		
	}
	else
	{
		return PX_FALSE;
	}
	return PX_TRUE;
}

PX_Object* PX_CDA_CreateClassObject(PX_CDA* pCDA, px_char classname[], px_float grid_x, px_float grid_y)
{
	px_char content[32] = {0};
	//build payload
	PX_MemoryCatString(&pCDA->payload, "{\n");
	PX_MemoryCatString(&pCDA->payload, "\"opcode\":\"createobject\",");
	PX_MemoryCatString(&pCDA->payload, "\"classname\":\"");
	PX_MemoryCatString(&pCDA->payload, classname);
	PX_MemoryCatString(&pCDA->payload, "\",\n");
	PX_MemoryCatString(&pCDA->payload, "\"grid_x\":\"");

	PX_BufferToHexString((px_byte*)&grid_x, 4, content);
	PX_MemoryCatString(&pCDA->payload, content);
	PX_MemoryCatString(&pCDA->payload, "\",\n");
	PX_MemoryCatString(&pCDA->payload, "\"grid_y\":\"");
	PX_BufferToHexString((px_byte*)&grid_y, 4, content);
	PX_MemoryCatString(&pCDA->payload, content);
	PX_MemoryCatString(&pCDA->payload, "\"\n}\n");
	PX_MemoryCat(&pCDA->payload, "\0", 1);
	if(PX_CDA_ExecutePayload(pCDA,PX_TRUE))
		return *PX_VECTORLAST(PX_Object *, &pCDA->pObjects);
	else
		return PX_NULL;
}

px_void  PX_CDA_DeleteSelectObject(PX_CDA* pCDA)
{
	px_char content[32] = { 0 };
	//get index
	px_int i;
	for (i = 0; i < pCDA->pObjects.size; i++)
	{
		PX_Object** ppObject = PX_VECTORAT(PX_Object*, &pCDA->pObjects, i);
		PX_ASSERTIF(ppObject == PX_NULL);
		PX_Object* pCDAObject = *ppObject;
		PX_CDA_Object* pDesc = PX_ObjectGetDesc(PX_CDA_Object, pCDAObject);
		if (pDesc->mode == PX_CDA_OBJECT_MODE_SELECT)
		{
			break;
		}
	}
	if (i== pCDA->pObjects.size)
	{
		return;
	}
	//build payload
	PX_MemoryCatString(&pCDA->payload, "{\n");
	PX_MemoryCatString(&pCDA->payload, "\"opcode\":\"deleteobject\",");
	PX_MemoryCatString(&pCDA->payload, "\"index\":\"");
	PX_itoa(i, content, sizeof(content), 10);
	PX_MemoryCatString(&pCDA->payload, content);
	PX_MemoryCatString(&pCDA->payload, "\"\n}\n");
	PX_MemoryCat(&pCDA->payload, "\0", 1);
	PX_CDA_ExecutePayload(pCDA, PX_TRUE);
}

px_void PX_CDA_MoveSelectObjectLast(PX_CDA* pCDA, px_float grid_x, px_float grid_y)
{
	px_int i;
	px_char content[32] = { 0 };
	for (i = 0; i < pCDA->pObjects.size; i++)
	{
		PX_Object* pObject = *PX_VECTORAT(PX_Object*, &pCDA->pObjects, i);
		PX_CDA_Object* pDesc = PX_ObjectGetDesc(PX_CDA_Object, pObject);
		if (pDesc->mode == PX_CDA_OBJECT_MODE_SELECT)
		{
			break;
		}
	}
	if (i== pCDA->pObjects.size)
	{
		return;
	}
	PX_MemoryCatString(&pCDA->payload, "{\n");
	PX_MemoryCatString(&pCDA->payload, "\"opcode\":\"moveobject\",");
	PX_MemoryCatString(&pCDA->payload, "\"index\":\"");
	PX_itoa(i, content, sizeof(content), 10);
	PX_MemoryCatString(&pCDA->payload, content);
	PX_MemoryCatString(&pCDA->payload, "\",\n");
	PX_MemoryCatString(&pCDA->payload, "\"grid_x\":\"");
	PX_BufferToHexString((px_byte*)&grid_x, 4, content);
	PX_MemoryCatString(&pCDA->payload, content);
	PX_MemoryCatString(&pCDA->payload, "\",\n");
	PX_MemoryCatString(&pCDA->payload, "\"grid_y\":\"");
	PX_BufferToHexString((px_byte*)&grid_y, 4, content);
	PX_MemoryCatString(&pCDA->payload, content);
	PX_MemoryCatString(&pCDA->payload, "\"\n}\n");
	PX_MemoryCat(&pCDA->payload, "\0", 1);
	PX_CDA_ExecutePayload(pCDA, PX_TRUE);
}

px_void PX_CDA_DeleteRouteSignal(PX_CDA* pCDA, px_int index)
{
	PX_CDA_Route* proute = PX_VECTORAT(PX_CDA_Route, &pCDA->routes, index);
	px_int i;
	PX_ASSERTIF(index < 0 || index >= pCDA->routes.size);
	for (i = 0; i < proute->signals.size; i++)
	{
		PX_CDA_Signal* psignal = PX_VECTORAT(PX_CDA_Signal, &proute->signals, i);
		PX_VM_VariableFree(&psignal->var);
	}
	PX_VectorFree(&proute->signals);
}

px_void PX_CDA_DeleteRoute(PX_CDA* pCDA, px_int index)
{
	PX_CDA_Route* proute = PX_VECTORAT(PX_CDA_Route, &pCDA->routes, index);
	PX_ASSERTIF(index < 0 || index >= pCDA->routes.size);
	PX_VectorFree(&proute->routes);
	PX_VectorFree(&proute->signals);
	PX_VectorErase(&pCDA->routes, index);

}

px_void PX_CDA_DeleteSelectRoute(PX_CDA* pCDA)
{
	px_int i;
	px_char content[32] = { 0 };
	for (i = 0; i < pCDA->routes.size; i++)
	{
		PX_CDA_Route* proute = PX_VECTORAT(PX_CDA_Route, &pCDA->routes, i);
		if (proute->mode == PX_CDA_OBJECT_MODE_SELECT)
		{
			break;
		}
	}
	if (i == pCDA->routes.size)
	{
		return;
	}
	
	//build payload
	PX_MemoryCatString(&pCDA->payload, "{\n");
	PX_MemoryCatString(&pCDA->payload, "\"opcode\":\"deleteroute\",");
	PX_MemoryCatString(&pCDA->payload, "\"index\":\"");
	PX_itoa(i, content, sizeof(content), 10);
	PX_MemoryCatString(&pCDA->payload, content);
	PX_MemoryCatString(&pCDA->payload, "\"\n}\n");
	PX_MemoryCat(&pCDA->payload, "\0", 1);
	PX_CDA_ExecutePayload(pCDA, PX_TRUE);
}

px_bool PX_CDA_isInRouteEditing(PX_CDA* pCDA)
{
	return pCDA->editing_route.routes.mp?PX_TRUE:PX_FALSE;
}

px_bool PX_CDA_EndRouteEdit(PX_CDA* pCDA, px_bool save)
{
	if (pCDA->editing_route.routes.mp)
	{
		if (save)
		{
			PX_VectorPopN(&pCDA->editing_route.routes, pCDA->editing_route.routes.size - pCDA->editing_route_pin);
			if (pCDA->editing_route.routes.size)
			{
				PX_CDA_UpdateRouteStartEndGrid(pCDA, &pCDA->editing_route);
				//generate payload
				px_char content[32] = { 0 };
				px_char reskey[32] = { 0 };
				px_int i;
				PX_MemoryCatString(&pCDA->payload, "{\n");
				PX_MemoryCatString(&pCDA->payload, "\"opcode\":\"createroute\",\n");
				PX_MemoryCatString(&pCDA->payload, "\"classname\":\"");
				PX_MemoryCatString(&pCDA->payload, pCDA->editing_route.prouteClass->name);
				PX_MemoryCatString(&pCDA->payload, "\",\n");
				PX_MemoryCatString(&pCDA->payload, "\"start_grid_x\":\"");
				PX_itoa(pCDA->editing_route.start_grid_x, content, sizeof(content), 10);
				PX_MemoryCatString(&pCDA->payload, content);
				PX_MemoryCatString(&pCDA->payload, "\",\n");
				PX_MemoryCatString(&pCDA->payload, "\"start_grid_y\":\"");
				PX_itoa(pCDA->editing_route.start_grid_y, content, sizeof(content), 10);
				PX_MemoryCatString(&pCDA->payload, content);
				PX_MemoryCatString(&pCDA->payload, "\",\n");
				if(pCDA->editing_route.routes.size)
					PX_MemoryCatString(&pCDA->payload, "\"payload\":[");
				for (i = 0; i < pCDA->editing_route.routes.size; i++)
				{
					PX_CDA_RouteLine* p = PX_VECTORAT(PX_CDA_RouteLine, &pCDA->editing_route.routes, i);
					PX_itoa(p->direction, content, sizeof(content), 10);
					PX_MemoryCatString(&pCDA->payload, content);
					PX_MemoryCatString(&pCDA->payload, ",");
					PX_itoa(p->length, content, sizeof(content), 10);
					PX_MemoryCatString(&pCDA->payload, content);
					if (i != pCDA->editing_route.routes.size - 1)
					{
						PX_MemoryCatString(&pCDA->payload, ",");
					}
				}
				if (pCDA->editing_route.routes.size)
				{
					PX_MemoryCatString(&pCDA->payload, "]\n}\n");
				}
				else
				{
					PX_MemoryCatString(&pCDA->payload, "\n}\n");
				}
				PX_MemoryCat(&pCDA->payload, "\0", 1);
				//exec
				PX_CDA_ExecutePayload(pCDA, PX_TRUE);
			}
		}

		PX_VectorFree(&pCDA->editing_route.routes);
		PX_memset(&pCDA->editing_route, 0, sizeof(pCDA->editing_route));
		pCDA->editing_route_pin = 0;
		return PX_TRUE;
	}
	return PX_FALSE;

}

px_void PX_CDA_SelectClear(PX_CDA* pCDA)
{
	px_int i;
	//clear objects
	for (i = 0; i < pCDA->pObjects.size; i++)
	{
		PX_Object** ppObject = PX_VECTORAT(PX_Object*, &pCDA->pObjects, i);
		PX_ASSERTIF(ppObject == PX_NULL);
		PX_Object* pCDAObject = *ppObject;
		PX_CDA_Object* pDesc = PX_ObjectGetDesc(PX_CDA_Object, pCDAObject);
		pDesc->mode = PX_CDA_OBJECT_MODE_NORMAL;
	}
	//clear routes
	for (i = 0; i < pCDA->routes.size; i++)
	{
		PX_CDA_Route* proute = PX_VECTORAT(PX_CDA_Route, &pCDA->routes, i);
		proute->mode = PX_CDA_OBJECT_MODE_NORMAL;
	}
}

px_bool PX_CDA_SelectObject(PX_CDA* pCDA, px_float grid_x, px_float grid_y)
{
	px_int i;

	for (i = 0; i < pCDA->pObjects.size; i++)
	{
		PX_Object* pObject = *PX_VECTORAT(PX_Object*, &pCDA->pObjects, i);
		PX_CDA_Object* pDesc = PX_ObjectGetDesc(PX_CDA_Object, pObject);
		if (grid_x > pDesc->grid_x- pDesc->grid_width/2.f && grid_x < pDesc->grid_x + pDesc->grid_width/2.f&&
			grid_y > pDesc->grid_y- pDesc->grid_height/2.f && grid_y < pDesc->grid_y + pDesc->grid_height/2.f)
		{
			pDesc->mode = PX_CDA_OBJECT_MODE_SELECT;
			return PX_TRUE;
		}
	}
	return PX_FALSE;
	
}

px_bool PX_CDA_isPositionSelectRoute(PX_CDA* pCDA, PX_CDA_Route* proute, px_float grid_x, px_float grid_y)
{
	px_int i;
	px_float x, y, ex = 0, ey = 0;
	PX_CDA_RouteLine* p = (PX_CDA_RouteLine*)proute->routes.data;
	x = proute->start_grid_x+0.5f;
	y = proute->start_grid_y+0.5f;

	for (i = 0; i < proute->routes.size; i++)
	{

		px_byte direction = p[i].direction;
		px_short length = p[i].length;

		switch (direction)
		{
		case 8:
			ey = y - length;
			ex = x;
			break;
		case 2:
			ey = y + length;
			ex = x;
			break;
		case 4:
			ey = y;
			ex = x - length;
			break;
		case 6:
			ey = y;
			ex = x + length;
			break;
		case 7:
			ey = y - length;
			ex = x - length;
			break;
		case 9:
			ey = y - length;
			ex = x + length;
			break;
		case 1:
			ey = y + length;
			ex = x - length;
			break;
		case 3:
			ey = y + length;
			ex = x + length;
			break;
		default:
			PX_ERROR("Route data error");
			break;
		}
	

		if (PX_isLineCrossCircle(PX_POINT2D(1.0f*x*pCDA->grid_size, 1.0f * y * pCDA->grid_size),\
			PX_POINT2D(1.0f * ex * pCDA->grid_size, 1.0f * ey * pCDA->grid_size),\
			PX_POINT2D(1.0f * grid_x * pCDA->grid_size, 1.0f * grid_y*pCDA->grid_size),\
			proute->prouteClass->linewidth+2))
		{
			return PX_TRUE;
		}

		x = ex;
		y = ey;
	}
	return PX_FALSE;
}

px_bool PX_CDA_SelectRoute(PX_CDA* pCDA, px_float grid_x, px_float grid_y)
{
	px_int i;
	for (i = 0; i < pCDA->routes.size; i++)
	{
		PX_CDA_Route* proute = PX_VECTORAT(PX_CDA_Route, &pCDA->routes, i);
		PX_ASSERTIF(proute == PX_NULL);
		if (PX_CDA_isPositionSelectRoute(pCDA, proute, grid_x, grid_y))
		{
			proute->mode = PX_CDA_OBJECT_MODE_SELECT;
			return PX_TRUE;
		}
	}
	return PX_FALSE;
}

px_bool PX_CDA_HasSelectingRoute(PX_CDA* pCDA)
{
	px_int i;
	for (i = 0; i < pCDA->routes.size; i++)
	{
		PX_CDA_Route* proute = PX_VECTORAT(PX_CDA_Route, &pCDA->routes, i);
		if (proute->mode == PX_CDA_OBJECT_MODE_SELECT)
		{
			return PX_TRUE;
		}
	}
	return PX_FALSE;
}

px_bool PX_CDA_HasSelectingObject(PX_CDA* pCDA)
{
	px_int i;
	for (i = 0; i < pCDA->pObjects.size; i++)
	{
		PX_Object* pObject = *PX_VECTORAT(PX_Object*, &pCDA->pObjects, i);
		PX_CDA_Object* pDesc = PX_ObjectGetDesc(PX_CDA_Object, pObject);
		if (pDesc->mode == PX_CDA_OBJECT_MODE_SELECT)
		{
			return PX_TRUE;
		}
	}
	return PX_FALSE;
}

PX_Object* PX_CDA_GetSelectObject(PX_CDA* pCDA)
{
	px_int i;
	for (i = 0; i < pCDA->pObjects.size; i++)
	{
		PX_Object* pObject = *PX_VECTORAT(PX_Object*, &pCDA->pObjects, i);
		PX_CDA_Object* pDesc = PX_ObjectGetDesc(PX_CDA_Object, pObject);
		if (pDesc->mode == PX_CDA_OBJECT_MODE_SELECT)
		{
			return pObject;
		}
	}
	return PX_NULL;
}

px_void PX_CDA_Rollback(PX_CDA* pCDA)
{
	px_int ip;
	if (pCDA->ip==0)
	{
		return;
	}
	ip = pCDA->ip-2;
	while (ip)
	{
		ip--;
		if (pCDA->payload.buffer[ip] == '\0')
		{
			ip++;
			break;
		}
	}
	
	PX_CDA_Clear(pCDA);
	pCDA->ip = 0;
	while(pCDA->ip<ip)
	{
		PX_CDA_ExecutePayload(pCDA,PX_FALSE);
	}
}

px_void PX_CDA_Forward(PX_CDA* pCDA)
{
	if(pCDA->ip<pCDA->payload.usedsize)
		PX_CDA_ExecutePayload(pCDA,PX_FALSE);
}

