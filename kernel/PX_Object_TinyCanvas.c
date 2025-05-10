#include "PX_Object_TinyCanvas.h"

static px_void PX_TinyCanvasVM_PenPaint(px_surface* psurface, px_float x, px_float y, px_float size, px_color color, px_void* ptr)
{
	PX_GeoDrawPenCircle(psurface, x, y, size, color);
}

static px_void PX_TinyCanvasVM_PaintPaint(px_surface* psurface, px_float x, px_float y, px_float size, px_color color, px_void* ptr)
{
	PX_Object* pObject = (PX_Object*)ptr;
	PX_Object_TinyCanvas* pDesc = PX_ObjectGetDescIndex(PX_Object_TinyCanvas, pObject,0);
	px_color clr;
	if (x<0 || x>psurface->width - 1 || y<0 || y>psurface->height - 1)
	{
		return;
	}
	clr = PX_SURFACECOLOR(psurface, (px_int)x, (px_int)y);
	PX_TextureFill(pDesc->mp, psurface, (px_int)x, (px_int)y, clr, 0.25f, color);
}

static px_void PX_TinyCanvasVM_SparyPaint(px_surface* psurface, px_float x, px_float y, px_float size, px_color color, px_void* ptr)
{
	PX_GeoDrawSpray(psurface, x, y, size, color);
}

static px_void PX_TinyCanvasVM_EraserPaint(px_surface* psurface, px_float x, px_float y, px_float size, px_color color, px_void* ptr)
{
	PX_GeoDrawPenCircleEraser(psurface, x, y, size, PX_TRUE);
}

static px_void PX_TinyCanvasVMPushNode(PX_Object_TinyCanvas* pTinyCanvasVM, PX_TinyCanvasNode* pnode)
{
	if (pTinyCanvasVM->currentPathCount < PX_TINYCANVASVM_MAX_COUNT)
	{
		pTinyCanvasVM->currentPath[pTinyCanvasVM->currentPathCount] = *pnode;
		pTinyCanvasVM->currentPathCount++;
	}

}

static px_bool PX_TinyCanvasVM_AddNode(PX_Object_TinyCanvas* pTinyCanvasVM, px_float x, px_float y, px_float z)
{
	PX_TinyCanvasNode node;
	if (pTinyCanvasVM->currentPathCount == 0)
	{
		node.x = x;
		node.y = y;
		node.z = z;
		PX_TinyCanvasVMPushNode(pTinyCanvasVM, &node);
		return PX_TRUE;
	}
	else
	{
		PX_TinyCanvasNode lastNode = pTinyCanvasVM->currentPath[pTinyCanvasVM->currentPathCount - 1];
		px_point2D v = PX_POINT2D(x - lastNode.x, y - lastNode.y);
		px_float distance = PX_Point2DMod(v);
		v = PX_Point2DNormalization(v);
		if (distance > pTinyCanvasVM->reg_size / PX_TINYCANVASVM_STEP_DIV)
		{
			px_float fstep = (pTinyCanvasVM->reg_size / PX_TINYCANVASVM_STEP_DIV);
			px_int iStep;
			fstep = fstep < 1 ? 1 : fstep;
			iStep = (px_int)(distance / fstep);

			node.x = lastNode.x + iStep * v.x;
			node.y = lastNode.y + iStep * v.y;
			node.z = z;
			PX_TinyCanvasVMPushNode(pTinyCanvasVM, &node);
			return PX_TRUE;
		}
	}
	return PX_FALSE;

}

static px_void PX_TinyCanvasVM_PaintDot(PX_Object_TinyCanvas* pTinyCanvasVM, px_float x, px_float y, px_float z)
{
	if (pTinyCanvasVM->reg_tool >= 0 && pTinyCanvasVM->reg_tool < pTinyCanvasVM->reg_tools_count)
	{
		PX_TinyCanvasVMTool* pTool = &pTinyCanvasVM->tools[pTinyCanvasVM->reg_tool];

		if (z < 0)
		{
			z = 0;
		}
		if (z > 1)
		{
			z = 1;
		}
		if (pTool->paint)
		{
			pTool->paint(&pTinyCanvasVM->rendersurface, x, y, pTinyCanvasVM->reg_size * z, pTinyCanvasVM->reg_color, pTool->ptr);
		}
	}
}

static px_bool PX_TinyCanvasVMOnBegin(PX_Object_TinyCanvas* pTinyCanvasVM)
{
	if (pTinyCanvasVM->reg_state == PX_TinyCanvasVM_State_Standby)
	{
		pTinyCanvasVM->reg_state = PX_TinyCanvasVM_State_Painting;
		pTinyCanvasVM->currentPathCount = 0;
		PX_memset(pTinyCanvasVM->currentPath, 0, sizeof(pTinyCanvasVM->currentPath));
		return PX_TRUE;
	}
	return PX_FALSE;
}

static px_void PX_TinyCanvasVMOnDrawLastInterpolate(PX_Object_TinyCanvas* pTinyCanvasVM)
{

	if (pTinyCanvasVM->currentPathCount > 1)
	{
		px_point3D v1 = PX_POINT(pTinyCanvasVM->currentPath[pTinyCanvasVM->currentPathCount - 1].x, pTinyCanvasVM->currentPath[pTinyCanvasVM->currentPathCount - 1].y, pTinyCanvasVM->currentPath[pTinyCanvasVM->currentPathCount - 1].z);
		px_point3D v2 = PX_POINT(pTinyCanvasVM->currentPath[pTinyCanvasVM->currentPathCount - 2].x, pTinyCanvasVM->currentPath[pTinyCanvasVM->currentPathCount - 2].y, pTinyCanvasVM->currentPath[pTinyCanvasVM->currentPathCount - 2].z);
		px_point3D dir = PX_PointSub(v1, v2);
		px_float d = PX_sqrt(dir.x * dir.x + dir.y * dir.y);
		px_float step = pTinyCanvasVM->reg_size / PX_TINYCANVASVM_STEP_DIV;
		px_int i, dots;

		PX_ASSERTIF(pTinyCanvasVM->reg_size == 0)


		dir = PX_PointNormalization(dir);

		dots = PX_APO(d / step);

		for (i = 1; i <= dots; i++)
		{
			PX_TinyCanvasVM_PaintDot(pTinyCanvasVM, v2.x + dir.x * i * step, v2.y + dir.y * i * step, v2.z + dir.z * i * step);
		}
	}
	else if (pTinyCanvasVM->currentPathCount == 1)
	{
		PX_TinyCanvasVM_PaintDot(pTinyCanvasVM, pTinyCanvasVM->currentPath[0].x, pTinyCanvasVM->currentPath[0].y, pTinyCanvasVM->currentPath[0].z);
	}

}

static px_void PX_TinyCanvasVMOnMove(PX_Object_TinyCanvas* pTinyCanvasVM, px_float x, px_float y, px_float z)
{
	px_float step = pTinyCanvasVM->reg_size / PX_TINYCANVASVM_STEP_DIV;
	if (pTinyCanvasVM->reg_state != PX_TinyCanvasVM_State_Painting)
	{
		return;
	}

	if (step < 1)
	{
		step = 1;
	}
	if (pTinyCanvasVM->currentPathCount == 0)
	{
		if (pTinyCanvasVM->reg_tool >= 0 && pTinyCanvasVM->reg_tool < pTinyCanvasVM->reg_tools_count)
		{
			if (PX_TinyCanvasVM_AddNode(pTinyCanvasVM, x, y, z))
				PX_TinyCanvasVMOnDrawLastInterpolate(pTinyCanvasVM);
		}
		else
		{
			PX_ASSERT();
		}
		pTinyCanvasVM->reg_lastCursorPos = PX_POINT(x, y, z);
	}
	else
	{
		px_float d;
		px_point lastPoint;

		if (pTinyCanvasVM->reg_tool < 0 || pTinyCanvasVM->reg_tool >= pTinyCanvasVM->reg_tools_count)
		{
			return;
		}

		lastPoint.x = pTinyCanvasVM->currentPath[pTinyCanvasVM->currentPathCount - 1].x;
		lastPoint.y = pTinyCanvasVM->currentPath[pTinyCanvasVM->currentPathCount - 1].y;
		lastPoint.z = pTinyCanvasVM->currentPath[pTinyCanvasVM->currentPathCount - 1].z;

		do
		{
			px_float m1, m2;
			px_point2D v1, v2;
			v1 = PX_POINT2D(lastPoint.x - pTinyCanvasVM->reg_lastCursorPos.x, lastPoint.y - pTinyCanvasVM->reg_lastCursorPos.y);
			v2 = PX_POINT2D(x - pTinyCanvasVM->reg_lastCursorPos.x, y - pTinyCanvasVM->reg_lastCursorPos.y);

			m1 = PX_Point2DMod(v1);
			m2 = PX_Point2DMod(v2);

			if (m1 * m2 > 1)
			{
				px_float _cosv = PX_Point2DDot(v1, v2) / m1 / m2;
				if (_cosv > 0.5f)
				{
					if (PX_TinyCanvasVM_AddNode(pTinyCanvasVM, pTinyCanvasVM->reg_lastCursorPos.x, pTinyCanvasVM->reg_lastCursorPos.y, pTinyCanvasVM->reg_lastCursorPos.z))
						PX_TinyCanvasVMOnDrawLastInterpolate(pTinyCanvasVM);

					lastPoint.x = pTinyCanvasVM->currentPath[pTinyCanvasVM->currentPathCount - 1].x;
					lastPoint.y = pTinyCanvasVM->currentPath[pTinyCanvasVM->currentPathCount - 1].y;
					lastPoint.z = pTinyCanvasVM->currentPath[pTinyCanvasVM->currentPathCount - 1].z;
				}
			}

		} while (0);

		pTinyCanvasVM->reg_lastCursorPos = PX_POINT(x, y, z);

		d = PX_sqrt((x - lastPoint.x) * (x - lastPoint.x) + (y - lastPoint.y) * (y - lastPoint.y));
		if (d > pTinyCanvasVM->reg_filter_radius)
		{
			if (PX_TinyCanvasVM_AddNode(pTinyCanvasVM, pTinyCanvasVM->reg_lastCursorPos.x, pTinyCanvasVM->reg_lastCursorPos.y, pTinyCanvasVM->reg_lastCursorPos.z))
				PX_TinyCanvasVMOnDrawLastInterpolate(pTinyCanvasVM);
		}
		else
		{
			return;
		}
	}

}

static px_void PX_TinyCanvasVMOnEnd(PX_Object_TinyCanvas* pTinyCanvasVM)
{
	PX_ASSERTIF(pTinyCanvasVM->reg_tool == -1);

	if (pTinyCanvasVM->reg_state != PX_TinyCanvasVM_State_Painting)
	{
		return;
	}

	if (pTinyCanvasVM->reg_lastCursorPos.x != pTinyCanvasVM->currentPath[pTinyCanvasVM->currentPathCount - 1].x)
	{
		if (pTinyCanvasVM->reg_lastCursorPos.y != pTinyCanvasVM->currentPath[pTinyCanvasVM->currentPathCount - 1].y)
		{
			if (pTinyCanvasVM->reg_tool >= 0 && pTinyCanvasVM->reg_tool < pTinyCanvasVM->reg_tools_count)
			{
				if (PX_TinyCanvasVM_AddNode(pTinyCanvasVM, pTinyCanvasVM->reg_lastCursorPos.x, pTinyCanvasVM->reg_lastCursorPos.y, pTinyCanvasVM->reg_lastCursorPos.z))
					PX_TinyCanvasVMOnDrawLastInterpolate(pTinyCanvasVM);

			}
			else
			{
				PX_ASSERT();
			}

		}
	}

	PX_memset(pTinyCanvasVM->currentPath, 0, sizeof(pTinyCanvasVM->currentPath));
	pTinyCanvasVM->currentPathCount = 0;
	pTinyCanvasVM->reg_state = PX_TinyCanvasVM_State_Standby;
	pTinyCanvasVM->reg_state_id++;
	return;
}

static px_bool PX_TinyCanvasVMSelectToolByName(PX_Object_TinyCanvas* pTinyCanvasVM, const px_char name[])
{
	px_int i;
	for (i = 0; i < PX_COUNTOF(pTinyCanvasVM->tools); i++)
	{
		if (!pTinyCanvasVM->tools[i].name[0])
			break;
		if (PX_strequ2(pTinyCanvasVM->tools[i].name, name))
		{
			pTinyCanvasVM->reg_tool = i;
			return PX_TRUE;
		}
	}
	pTinyCanvasVM->reg_tool = -1;
	return PX_FALSE;
}

static px_void PX_TinyCanvasVMSetSize(PX_Object_TinyCanvas* pTinyCanvasVM, px_float size)
{
	pTinyCanvasVM->reg_size = size;
}

static px_void PX_TinyCanvasVMSetFilter(PX_Object_TinyCanvas* pTinyCanvasVM, px_float filter)
{
	pTinyCanvasVM->reg_filter_radius = filter;
}

PX_OBJECT_EVENT_FUNCTION(PX_Object_TinyCanvasOnCursorDown)
{
	px_rect rect;
	px_float x, y;
	PX_Object_TinyCanvas* pdesc = (PX_Object_TinyCanvas*)PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_TINYCANVAS);
	PX_ASSERTIF(!pdesc);

	rect = PX_ObjectGetRect(pObject);
	x = rect.x;
	y = rect.y;
	if (PX_ObjectIsCursorInRegion(pObject, e))
	{
		pdesc->reg_x = PX_Object_Event_GetCursorX(e) - x;
		pdesc->reg_y = PX_Object_Event_GetCursorY(e) - y;


		if (PX_TinyCanvasVMOnBegin(pdesc))
		{
			PX_ObjectExecuteEvent(pObject, PX_OBJECT_BUILD_EVENT(PX_OBJECT_EVENT_TINYCANVAS_PAINTBEGIN));
		}
		if (pdesc->reg_state == PX_CanvasVM_State_Painting)
		{
			PX_TinyCanvasVMOnMove(pdesc, pdesc->reg_x, pdesc->reg_y, 1);
		}
	}
}

PX_OBJECT_EVENT_FUNCTION(PX_Object_TinyCanvasOnCursorMove)
{
	px_rect region = PX_ObjectGetRect(pObject);
	PX_Object_TinyCanvas* pdesc = (PX_Object_TinyCanvas*)PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_TINYCANVAS);
	if (PX_ObjectIsCursorInRegion(pObject, e))
	{
		pdesc->reg_x = PX_Object_Event_GetCursorX(e) - region.x;
		pdesc->reg_y = PX_Object_Event_GetCursorY(e) - region.y;
	}

	if (pdesc->reg_state == PX_CanvasVM_State_Painting)
	{
		PX_TinyCanvasVMOnEnd(pdesc);
	}

}

PX_OBJECT_EVENT_FUNCTION(PX_Object_TinyCanvasOnCursorDrag)
{
	px_float x, y, w, h;
	px_rect rect;
	PX_Object_TinyCanvas* pdesc = (PX_Object_TinyCanvas*)PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_TINYCANVAS);
	PX_ASSERTIF(!pdesc);
	rect = PX_ObjectGetRect(pObject);
	x = rect.x;
	y = rect.y;
	//px_surface* prender = PX_Object_CanvasGetCurrentEditingSurface(pObject);

	PX_OBJECT_INHERIT_CODE(pObject, x, y, w, h);
	if (PX_ObjectIsCursorInRegion(pObject, e))
	{

		pdesc->reg_x = PX_Object_Event_GetCursorX(e) - x;
		pdesc->reg_y = PX_Object_Event_GetCursorY(e) - y;

		if (pdesc->reg_state == PX_CanvasVM_State_Painting)
		{
			PX_TinyCanvasVMOnMove(pdesc, pdesc->reg_x, pdesc->reg_y, 1);
		}
	}
	else
	{
		return;
	}
}

PX_OBJECT_EVENT_FUNCTION(PX_Object_TinyCanvasOnCursorUp)
{
	PX_Object_TinyCanvas* pdesc = (PX_Object_TinyCanvas*)PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_TINYCANVAS);
	PX_ASSERTIF(!pdesc);
	if (pdesc->reg_state == PX_CanvasVM_State_Painting)
	{
		PX_ObjectExecuteEvent(pObject, PX_OBJECT_BUILD_EVENT(PX_OBJECT_EVENT_TINYCANVAS_PAINTEND));
		PX_TinyCanvasVMOnEnd(pdesc);
	}
	

}

PX_OBJECT_RENDER_FUNCTION(PX_Object_TinyCanvasRender)
{
	PX_Object_TinyCanvas* pDesc = PX_ObjectGetDesc(PX_Object_TinyCanvas, pObject);
	px_int x, y, w, h;
	px_rect rect;

	rect = PX_ObjectGetRect(pObject);
	x = (px_int)rect.x;
	y = (px_int)rect.y;
	w = (px_int)rect.width;
	h = (px_int)rect.height;


	if (w != pDesc->rendersurface.width || h != pDesc->rendersurface.height)
	{
		px_texture newtex;
		if (w > 0 && h > 0)
		{
			if (!PX_TextureCreate(pObject->mp, &newtex, w, h))
			{
				return;
			}
			PX_TextureCover(&newtex, &pDesc->rendersurface, 0, 0, PX_ALIGN_LEFTTOP);
			PX_TextureFree(&pDesc->rendersurface);
			pDesc->rendersurface = newtex;
		}
	}

	if (w > 0 && h > 0)
	{
		
		PX_TextureRender(psurface, &pDesc->rendersurface, x, y, PX_ALIGN_LEFTTOP, PX_NULL);
	}
	PX_GeoDrawCircle(psurface,(px_int)(x + pDesc->reg_x),(px_int)(y + pDesc->reg_y), (px_int)pDesc->reg_size, 1, PX_COLOR_BLACK);


}

PX_OBJECT_FREE_FUNCTION(PX_Object_TinyCanvasFree)
{
	PX_Object_TinyCanvas* pDesc = PX_ObjectGetDesc(PX_Object_TinyCanvas, pObject);
	PX_TextureFree(&pDesc->rendersurface);
}


px_void PX_Object_TinyCanvasRegisterTool(PX_Object*pObject, const px_char name[PX_TINYCANVASVM_MAX_TOOL_NAME], pfun_PX_TinyCanvasVMOnPaint paint, px_bool store, px_void* ptr)
{
	px_int i;
	PX_Object_TinyCanvas* pDesc = PX_ObjectGetDescIndex(PX_Object_TinyCanvas, pObject, 0);
	for (i = 0; i < PX_COUNTOF(pDesc->tools); i++)
	{
		if (pDesc->tools[i].name[0] == 0)
		{
			PX_strset(pDesc->tools[i].name, name);
			pDesc->tools[i].paint = paint;
			pDesc->tools[i].ptr = ptr;
			pDesc->reg_tools_count++;
			return;
		}
	}
}

PX_Object* PX_Object_TinyCanvasAttachObject(PX_Object* pObject, px_int attachIndex)
{
	px_memorypool* mp = pObject->mp;

	PX_Object_TinyCanvas* pdesc;

	PX_ASSERTIF(pObject == PX_NULL);
	PX_ASSERTIF(attachIndex < 0 || attachIndex >= PX_COUNTOF(pObject->pObjectDesc));
	PX_ASSERTIF(pObject->pObjectDesc[attachIndex] != PX_NULL);
	pdesc = (PX_Object_TinyCanvas*)PX_ObjectCreateDesc(pObject, attachIndex, PX_OBJECT_TYPE_TINYCANVAS, 0, PX_Object_TinyCanvasRender, PX_Object_TinyCanvasFree, 0, sizeof(PX_Object_TinyCanvas));
	PX_ASSERTIF(pdesc == PX_NULL);

	PX_ASSERTIF(pObject->Width < 0 || pObject->Height < 0);

	if (!PX_TextureCreate(pObject->mp, &pdesc->rendersurface, (px_int)pObject->Width, (px_int)pObject->Height))
	{
		PX_ObjectDelete(pObject);
		return PX_NULL;
	}
	//hand--must be zero
	PX_Object_TinyCanvasRegisterTool(pObject, "hand", PX_TinyCanvasVM_PenPaint, PX_FALSE, pObject);
	//Pen
	PX_Object_TinyCanvasRegisterTool(pObject, "pen", PX_TinyCanvasVM_PenPaint, PX_TRUE, pObject);
	//Paint
	PX_Object_TinyCanvasRegisterTool(pObject, "paint", PX_TinyCanvasVM_PaintPaint, PX_TRUE, pObject);
	//Spary
	PX_Object_TinyCanvasRegisterTool(pObject, "spary", PX_TinyCanvasVM_SparyPaint, PX_TRUE, pObject);
	//Eraser
	PX_Object_TinyCanvasRegisterTool(pObject, "eraser", PX_TinyCanvasVM_EraserPaint, PX_TRUE, pObject);

	pdesc->mp = mp;
	pdesc->reg_state = PX_TinyCanvasVM_State_Standby;
	pdesc->reg_state_id = 314159;

	PX_Object_TinyCanvasSetTool(pObject, "pen");
	PX_Object_TinyCanvasSetSize(pObject, 5);
	PX_Object_TinyCanvasSetFilter(pObject, 1);
	PX_Object_TinyCanvasSetColor(pObject, PX_COLOR_BLACK);

	PX_ObjectRegisterEvent(pObject, PX_OBJECT_EVENT_CURSORDOWN, PX_Object_TinyCanvasOnCursorDown, pObject);
	PX_ObjectRegisterEvent(pObject, PX_OBJECT_EVENT_CURSORUP, PX_Object_TinyCanvasOnCursorUp, pObject);
	PX_ObjectRegisterEvent(pObject, PX_OBJECT_EVENT_CURSORMOVE, PX_Object_TinyCanvasOnCursorMove, pObject);
	PX_ObjectRegisterEvent(pObject, PX_OBJECT_EVENT_CURSORDRAG, PX_Object_TinyCanvasOnCursorDrag, pObject);
	PX_SurfaceClearAll(&pdesc->rendersurface, PX_COLOR_WHITE);
	return pObject;
}

PX_Object* PX_Object_TinyCanvasCreate(px_memorypool* mp, PX_Object* Parent, px_int x, px_int y, px_int width, px_int height)
{
	PX_Object* pObject;

	pObject = PX_ObjectCreate(mp, Parent, (px_float)x, (px_float)y, 0, (px_float)width, (px_float)height, 0);
	if (pObject == PX_NULL)
	{
		return PX_NULL;
	}

	if (!PX_Object_TinyCanvasAttachObject(pObject, 0))
	{
		PX_ObjectDelete(pObject);
		return PX_NULL;
	}

	return pObject;
}

px_surface* PX_Object_TinyCanvasGetSurface(PX_Object* pObject)
{
	PX_Object_TinyCanvas* pDesc = (PX_Object_TinyCanvas*)PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_TINYCANVAS);
	PX_ASSERTIF(pDesc == PX_NULL);
	return &pDesc->rendersurface;
}

px_void PX_Object_TinyCanvasSetColor(PX_Object* pObject, px_color color)
{
	PX_Object_TinyCanvas* pDesc = (PX_Object_TinyCanvas*)PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_TINYCANVAS);
	PX_ASSERTIF(pDesc == PX_NULL);
	pDesc->reg_color = color;
}

px_void PX_Object_TinyCanvasSetTool(PX_Object* pObject, const px_char name[PX_TINYCANVASVM_MAX_TOOL_NAME])
{
	PX_Object_TinyCanvas* pDesc = (PX_Object_TinyCanvas*)PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_TINYCANVAS);
	PX_ASSERTIF(pDesc == PX_NULL);
	if (!PX_TinyCanvasVMSelectToolByName(pDesc, name))
	{
		PX_ASSERT();
	}
}

px_void PX_Object_TinyCanvasSetSize(PX_Object* pObject, px_float size)
{
	PX_Object_TinyCanvas* pDesc = (PX_Object_TinyCanvas*)PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_TINYCANVAS);
	PX_ASSERTIF(pDesc == PX_NULL);
	PX_TinyCanvasVMSetSize(pDesc, size);
}

px_void PX_Object_TinyCanvasSetFilter(PX_Object* pObject, px_float filter)
{
	PX_Object_TinyCanvas* pDesc = (PX_Object_TinyCanvas*)PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_TINYCANVAS);
	PX_ASSERTIF(pDesc == PX_NULL);
	PX_TinyCanvasVMSetFilter(pDesc, filter);
}

px_void PX_Object_TinyCanvasClear(PX_Object* pObject)
{
	PX_Object_TinyCanvas* pDesc = (PX_Object_TinyCanvas*)PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_TINYCANVAS);
	PX_ASSERTIF(pDesc == PX_NULL);
	PX_SurfaceClearAll(&pDesc->rendersurface, PX_COLOR_WHITE);
	PX_memset(pDesc->currentPath, 0, sizeof(pDesc->currentPath));
	pDesc->currentPathCount = 0;
	pDesc->reg_state = PX_TinyCanvasVM_State_Standby;
}

