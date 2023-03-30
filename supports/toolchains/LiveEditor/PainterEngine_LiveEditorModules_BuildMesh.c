#include "PainterEngine_LiveEditorModules_BuildMesh.h"

px_void PX_LiveEditorModule_BuildMeshOnButtonReset(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Object *pBuildMeshObject=(PX_Object *)ptr;
	PX_LiveEditorModule_BuildMesh *pBuildMesh=(PX_LiveEditorModule_BuildMesh *)pBuildMeshObject->pObject;

	if (pBuildMesh->pLiveFramework->liveAnimations.size==0)
	{
		PX_LiveLayer *pLayer=PX_LiveFrameworkGetLayer(pBuildMesh->pLiveFramework,pBuildMesh->pLiveFramework->currentEditLayerIndex);

		PX_VectorClear(&pLayer->triangles);
		PX_VectorClear(&pLayer->vertices);

		pBuildMesh->buildpoints_count=0;
		pBuildMesh->limitpoints_count=0;
	}
	else
	{
		PX_ObjectExecuteEvent(pBuildMeshObject->pParent,PX_OBJECT_BUILD_EVENT_STRING(PX_LIVEEDITORMODULE_BUILDMESH_EVENT_MESSAGE,PX_JsonGetString(pBuildMesh->pLanguageJson,"buildmesh.vertices lock down")));
	}


	
}
px_void PX_LiveEditorModule_BuildMeshOnButtonFinish(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Object *pBuildMeshObject=(PX_Object *)ptr;
	PX_LiveEditorModule_BuildMesh *pBuildMesh=(PX_LiveEditorModule_BuildMesh *)pBuildMeshObject->pObject;

	PX_LiveFrameworkGetLayer(pBuildMesh->pLiveFramework,pBuildMesh->pLiveFramework->currentEditLayerIndex)->showMesh=PX_FALSE;
	PX_ObjectExecuteEvent(pBuildMeshObject->pParent,PX_OBJECT_BUILD_EVENT(PX_LIVEFRAMEWORKMODULES_BUILDMESH_EVENT_EXIT));
}



px_void PX_LiveEditorModule_BuildMeshRender(px_surface *pRenderSurface,PX_Object *pObject,px_dword elpased)
{
	PX_LiveEditorModule_BuildMesh *pBuildMesh=(PX_LiveEditorModule_BuildMesh *)pObject->pObject;
	PX_LiveLayer *pLayer=PX_LiveFrameworkGetLayer(pBuildMesh->pLiveFramework,pBuildMesh->pLiveFramework->currentEditLayerIndex);

	if (!pLayer->vertices.size)
	{
		px_int i;
		//drawline
		for (i=1;i<pBuildMesh->limitpoints_count;i++)
		{
			PX_GeoDrawLine(pRenderSurface,
				(px_int)(pBuildMesh->limitpoints[i-1].x+(pBuildMesh->pLiveFramework->refer_x)),\
				(px_int)(pBuildMesh->limitpoints[i-1].y+(pBuildMesh->pLiveFramework->refer_y)),\
				(px_int)(pBuildMesh->limitpoints[i].x+pBuildMesh->pLiveFramework->refer_x),\
				(px_int)(pBuildMesh->limitpoints[i].y+pBuildMesh->pLiveFramework->refer_y),
				1,
				PX_COLOR(128,0,0,255)
				);
		}
		//draw edit points
		for (i=0;i<pBuildMesh->buildpoints_count;i++)
		{
			PX_GeoDrawSolidCircle(pRenderSurface,\
				(px_int)(pBuildMesh->buildpoints[i].x+pBuildMesh->pLiveFramework->refer_x),\
				(px_int)(pBuildMesh->buildpoints[i].y+pBuildMesh->pLiveFramework->refer_y),\
				3,
				PX_COLOR(255,255,0,0)
				);

			if (PX_isPoint2DInCircle(pBuildMesh->cursor,pBuildMesh->buildpoints[i],5))
			{
				PX_GeoDrawCircle(pRenderSurface,\
					(px_int)(pBuildMesh->buildpoints[i].x+pBuildMesh->pLiveFramework->refer_x),\
					(px_int)(pBuildMesh->buildpoints[i].y+pBuildMesh->pLiveFramework->refer_y),\
					5,
					1,
					PX_COLOR(255,255,0,0)
					);
			}
		}
	}
}

px_void PX_LiveEditorModule_BuildMeshPostEvent_CursorLDown(PX_LiveEditorModule_BuildMesh *pBuildMesh,px_float x,px_float y)
{
	PX_LiveLayer *pLayer=PX_LiveFrameworkGetLayer(pBuildMesh->pLiveFramework,pBuildMesh->pLiveFramework->currentEditLayerIndex);
	if (pLayer->vertices.size)
	{
		//edit mode
		px_int i;
		pBuildMesh->pLiveFramework->currentEditVertexIndex=-1;
		pBuildMesh->widget_coefficient->Visible=PX_FALSE;
		for (i=0;i<pLayer->vertices.size;i++)
		{
			PX_LiveVertex *pVertex=PX_VECTORAT(PX_LiveVertex,&pLayer->vertices,i);
			if (PX_isPoint2DInCircle(PX_POINT2D(x,y),PX_POINT2D(pVertex->currentPosition.x,pVertex->currentPosition.y),5))
			{
				px_char fcontent[32]={0};
				pBuildMesh->pLiveFramework->currentEditVertexIndex=i;
				pBuildMesh->lastdragcursor=PX_POINT2D(x,y);
				pBuildMesh->widget_coefficient->Visible=PX_TRUE;
				if (pVertex->k<0)
				{
					pVertex->k=0;
				}
				if (pVertex->k>100)
				{
					pVertex->k=100;
				}
				PX_itoa(pVertex->k,fcontent,sizeof(fcontent),10);
				PX_Object_EditSetText(pBuildMesh->edit_coefficient,fcontent);
				return;
			}
		}

	}
	else
	{
		//add mode
		if (pBuildMesh->buildpoints_count<PX_COUNTOF(pBuildMesh->buildpoints))
		{
			px_int i=-1;
			
			for (i=0;i<pBuildMesh->buildpoints_count;i++)
			{
				if (PX_isPoint2DInCircle(PX_POINT2D(x,y),pBuildMesh->buildpoints[i],5))
				{
					break;
				}
			}
			if (i==pBuildMesh->buildpoints_count)
			{
				pBuildMesh->limitpoints[pBuildMesh->limitpoints_count]=PX_POINT2D(x,y);
				pBuildMesh->limitpoints_count++;

				pBuildMesh->buildpoints[pBuildMesh->buildpoints_count]=PX_POINT2D(x,y);
				pBuildMesh->buildpoints_count++;
			}
			else
			{
				pBuildMesh->limitpoints[pBuildMesh->limitpoints_count]=pBuildMesh->buildpoints[i];
				pBuildMesh->limitpoints_count++;
			}
		}
	}
}

px_void PX_LiveEditorModule_BuildMeshPostEvent_CursorRDown(PX_LiveEditorModule_BuildMesh *pBuildMesh,px_float x,px_float y)
{
	PX_LiveLayer *pLayer=PX_LiveFrameworkGetLayer(pBuildMesh->pLiveFramework,pBuildMesh->pLiveFramework->currentEditLayerIndex);
	PX_LiveTexture *pLiveTexture;
	px_int i;
	if (pLayer->triangles.size)
	{
		return;
	}
	pLiveTexture=PX_LiveFrameworkGetLiveTexture(pBuildMesh->pLiveFramework,pLayer->LinkTextureIndex);
	if (!pLiveTexture)
	{
		PX_ASSERT();
		return;
	}
	PX_VectorClear(&pLayer->triangles);
	PX_VectorClear(&pLayer->vertices);
	PX_PointsMeshBuild(&pBuildMesh->pruntime->mp,pBuildMesh->limitpoints,pBuildMesh->limitpoints_count,pBuildMesh->buildpoints,pBuildMesh->buildpoints_count,&pLayer->triangles,PX_DELAUNAY_RETURN_TYPE_TRIANGLE_INDEX);
	
	if (pLayer->triangles.size==0)
	{
		PX_VectorClear(&pLayer->triangles);
		PX_Object_MessageBoxAlertOk(pBuildMesh->messagebox,PX_JsonGetString(pBuildMesh->pLanguageJson,"buildmesh.points not closed"),0,0);
	}
	else
	{
		for (i=0;i<pBuildMesh->buildpoints_count;i++)
		{
			px_int width,height;
			PX_LiveVertex vertex;
			PX_memset(&vertex,0,sizeof(vertex));
			vertex.k=0;
			vertex.normal=PX_POINT(0,0,-1);
			vertex.sourcePosition=PX_POINT(pBuildMesh->buildpoints[i].x,pBuildMesh->buildpoints[i].y,pLayer->keyPoint.z);
			vertex.currentPosition=vertex.sourcePosition;
			width=pLiveTexture->Texture.width;
			height=pLiveTexture->Texture.height;
			vertex.u=(vertex.sourcePosition.x-pLiveTexture->textureOffsetX)*1.0f/width;
			vertex.v=(vertex.sourcePosition.y-pLiveTexture->textureOffsetY)*1.0f/height;
			PX_VectorPushback(&pLayer->vertices,&vertex);
		}
		pBuildMesh->buildpoints_count=0;
		pBuildMesh->limitpoints_count=0;
	}
}

px_void PX_LiveEditorModule_BuildMeshPostEvent_CursorDrag(PX_LiveEditorModule_BuildMesh *pBuildMesh,px_float x,px_float y)
{
	PX_LiveLayer *pLayer=PX_LiveFrameworkGetLayer(pBuildMesh->pLiveFramework,pBuildMesh->pLiveFramework->currentEditLayerIndex);
	PX_LiveTexture *pLiveTexture;
	if (pLayer->vertices.size)
	{
		if (pBuildMesh->pLiveFramework->currentEditVertexIndex!=-1)
		{
			PX_LiveVertex *pVertex=PX_VECTORAT(PX_LiveVertex,&pLayer->vertices,pBuildMesh->pLiveFramework->currentEditVertexIndex);
			pLiveTexture=PX_LiveFrameworkGetLiveTexture(pBuildMesh->pLiveFramework,pLayer->LinkTextureIndex);
			if (pLiveTexture)
			{
				pVertex->sourcePosition.x+=x-pBuildMesh->lastdragcursor.x;
				pVertex->sourcePosition.y+=y-pBuildMesh->lastdragcursor.y;
				pVertex->u=(pVertex->sourcePosition.x-pLiveTexture->textureOffsetX)*1.0f/pLiveTexture->Texture.width;
				pVertex->v=(pVertex->sourcePosition.y-pLiveTexture->textureOffsetY)*1.0f/pLiveTexture->Texture.height;
				pBuildMesh->lastdragcursor.x=x;
				pBuildMesh->lastdragcursor.y=y;
				pVertex->currentPosition=pVertex->sourcePosition;
				pVertex->velocity=PX_POINT(0,0,0);
			}
		
		}
	}
}

px_void PX_LiveEditorModule_BuildMeshHandleEvent(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	px_float x,y;
	PX_LiveEditorModule_BuildMesh *pBuildMesh;

	pBuildMesh=(PX_LiveEditorModule_BuildMesh *)pObject->pObject;

	switch(e.Event)
	{
	case PX_OBJECT_EVENT_CURSORMOVE:
		{
			x=PX_Object_Event_GetCursorX(e);
			y=PX_Object_Event_GetCursorY(e);
			x-=(pBuildMesh->pLiveFramework->refer_x);
			y-=(pBuildMesh->pLiveFramework->refer_y);
			pBuildMesh->cursor.x=x;
			pBuildMesh->cursor.y=y;
		}
		break;
	case PX_OBJECT_EVENT_CURSORDOWN:
		{
			x=PX_Object_Event_GetCursorX(e);
			y=PX_Object_Event_GetCursorY(e);
			x-=(pBuildMesh->pLiveFramework->refer_x);
			y-=(pBuildMesh->pLiveFramework->refer_y);
			PX_LiveEditorModule_BuildMeshPostEvent_CursorLDown(pBuildMesh,x,y);
		}
		break;
	case PX_OBJECT_EVENT_CURSORRDOWN:
		{
			x=PX_Object_Event_GetCursorX(e);
			y=PX_Object_Event_GetCursorY(e);
			x-=(pBuildMesh->pLiveFramework->refer_x);
			y-=(pBuildMesh->pLiveFramework->refer_y);
			PX_LiveEditorModule_BuildMeshPostEvent_CursorRDown(pBuildMesh,x,y);
		}
		break;
	case PX_OBJECT_EVENT_CURSORDRAG:
		{
			x=PX_Object_Event_GetCursorX(e);
			y=PX_Object_Event_GetCursorY(e);
			x-=(pBuildMesh->pLiveFramework->refer_x);
			y-=(pBuildMesh->pLiveFramework->refer_y);
			pBuildMesh->cursor.x=x;
			pBuildMesh->cursor.y=y;
			PX_LiveEditorModule_BuildMeshPostEvent_CursorDrag(pBuildMesh,x,y);
		}
		break;
	default:
		break;
	}
}

px_void PX_LiveEditorModule_BuildMeshOnEditKChanged(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Object *pBuildMeshObject=(PX_Object *)ptr;
	PX_LiveEditorModule_BuildMesh *pBuildMesh=(PX_LiveEditorModule_BuildMesh *)pBuildMeshObject->pObject;
	PX_LiveVertex *pVertex=PX_LiveFrameworkGetCurrentEditLiveVertex(pBuildMesh->pLiveFramework);
	if (pVertex)
	{
		pVertex->k=PX_atoi(PX_Object_EditGetText(pBuildMesh->edit_coefficient));
		if (pVertex->k<0)
		{
			pVertex->k=0;
		}
		if (pVertex->k>100)
		{
			pVertex->k=100;
		}
	}
}


px_void PX_LiveEditorModule_BuildMeshEnable(PX_Object *pObject)
{
	PX_LiveEditorModule_BuildMesh *pBuildMesh=(PX_LiveEditorModule_BuildMesh *)pObject->pObject;
	if (pBuildMesh->pLiveFramework->currentEditLayerIndex>=0&&pBuildMesh->pLiveFramework->currentEditLayerIndex<pBuildMesh->pLiveFramework->layers.size)
	{
		PX_LiveFrameworkGetLayer(pBuildMesh->pLiveFramework,pBuildMesh->pLiveFramework->currentEditLayerIndex)->showMesh=PX_TRUE;
		PX_ObjectSetFocus(pObject);
		pObject->Enabled=PX_TRUE;
		pObject->Visible=PX_TRUE;
		pBuildMesh->widget_coefficient->Visible=PX_FALSE;
		pBuildMesh->pLiveFramework->currentEditVertexIndex=-1;

		pBuildMesh->pLiveFramework->showFocusLayer=PX_TRUE;
		pBuildMesh->pLiveFramework->showKeypoint=PX_FALSE;
		pBuildMesh->pLiveFramework->showlinker=PX_FALSE;
		pBuildMesh->pLiveFramework->showRootHelperLine=PX_FALSE;

		PX_LiveFrameworkStop(pBuildMesh->pLiveFramework);
	}
}

px_void PX_LiveEditorModule_BuildMeshDisable(PX_Object *pObject)
{
	PX_LiveEditorModule_BuildMesh *pBuildMesh=(PX_LiveEditorModule_BuildMesh *)pObject->pObject;
	pObject->Enabled=PX_FALSE;
	pObject->Visible=PX_FALSE;
	pBuildMesh->widget_coefficient->Visible=PX_FALSE;
	pBuildMesh->pLiveFramework->currentEditVertexIndex=-1;
}

PX_Object * PX_LiveEditorModule_BuildMeshInstall(PX_Object *pparent,PX_Runtime *pruntime,PX_FontModule *fm,PX_LiveFramework *pLiveFramework,PX_Json *pLanguageJson)
{
	PX_Object *pObject;
	PX_LiveEditorModule_BuildMesh BuildMesh,*pBuildMesh;
	PX_memset(&BuildMesh,0,sizeof(PX_LiveEditorModule_BuildMesh));
	
	pObject=PX_ObjectCreateEx(&pruntime->mp_ui,pparent,0,0,0,0,0,0,0,0,PX_LiveEditorModule_BuildMeshRender,0,&BuildMesh,sizeof(BuildMesh));
	pBuildMesh=(PX_LiveEditorModule_BuildMesh *)pObject->pObject;
	
	pBuildMesh->pruntime=pruntime;
	pBuildMesh->fontmodule=fm;
	pBuildMesh->pLanguageJson=pLanguageJson;
	pBuildMesh->pLiveFramework=pLiveFramework;
    pBuildMesh->messagebox= PX_Object_MessageBoxCreate(&pruntime->mp_ui,pObject,fm);

	pBuildMesh->widget_controller=PX_Object_WidgetCreate(&pruntime->mp_ui,pObject,pruntime->surface_width-280,64,176,62,"",fm);
	PX_Object_WidgetShowHideCloseButton(pBuildMesh->widget_controller,PX_FALSE);
	pBuildMesh->button_reset=PX_Object_PushButtonCreate(&pruntime->mp_ui,pBuildMesh->widget_controller,3+85*0,3,84,32,PX_JsonGetString(pLanguageJson,"buildmesh.reset"),fm);
	pBuildMesh->button_finish=PX_Object_PushButtonCreate(&pruntime->mp_ui,pBuildMesh->widget_controller,3+85*1,3,84,32,PX_JsonGetString(pLanguageJson,"buildmesh.finish"),fm);

	pBuildMesh->widget_coefficient=PX_Object_WidgetCreate(&pruntime->mp_ui,pObject,pruntime->surface_width-280,128,256,80,"",PX_NULL);
	if(!pBuildMesh->widget_coefficient)return PX_FALSE;
	PX_Object_WidgetShowHideCloseButton(pBuildMesh->widget_coefficient,PX_FALSE);
	PX_Object_LabelCreate(&pruntime->mp_ui,pBuildMesh->widget_coefficient,16,16,128,24,PX_JsonGetString(pLanguageJson,"buildmesh.coefficient"),fm,PX_OBJECT_UI_DEFAULT_FONTCOLOR);
	pBuildMesh->edit_coefficient=PX_Object_EditCreate(&pruntime->mp_ui,pBuildMesh->widget_coefficient,150,16,80,24,fm);
	PX_Object_EditSetLimit(pBuildMesh->edit_coefficient,"0123456789");
	PX_Object_EditSetMaxTextLength(pBuildMesh->edit_coefficient,3);
	PX_ObjectRegisterEvent(pBuildMesh->edit_coefficient,PX_OBJECT_EVENT_VALUECHANGED,PX_LiveEditorModule_BuildMeshOnEditKChanged,pObject);

	PX_ObjectRegisterEvent(pBuildMesh->button_reset,PX_OBJECT_EVENT_EXECUTE,PX_LiveEditorModule_BuildMeshOnButtonReset,pObject);
	PX_ObjectRegisterEvent(pBuildMesh->button_finish,PX_OBJECT_EVENT_EXECUTE,PX_LiveEditorModule_BuildMeshOnButtonFinish,pObject);
	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_ANY,PX_LiveEditorModule_BuildMeshHandleEvent,pObject);
	return pObject;
}

