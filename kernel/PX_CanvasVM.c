#include "PX_CanvasVM.h"

px_void PX_CanvasVM_PenPaint(px_surface* psurface, px_float x, px_float y, px_float size,px_color color, px_void* ptr)
{
	PX_GeoDrawPenCircle(psurface, x, y, size, color);
}

px_void PX_CanvasVM_PaintPaint(px_surface* psurface, px_float x, px_float y, px_float size, px_color color, px_void* ptr)
{
	PX_CanvasVM* pCanvasVM = (PX_CanvasVM*)ptr;
	px_color clr;
	if (x<0||x>psurface->width-1||y<0||y>psurface->height-1)
	{
		return;
	}
	clr=PX_SURFACECOLOR(psurface, (px_int)x, (px_int)y);
	PX_TextureFill(pCanvasVM->mp, psurface, (px_int)x, (px_int)y, clr,0.25f, color);
}

px_void PX_CanvasVM_SparyPaint(px_surface* psurface, px_float x, px_float y, px_float size, px_color color, px_void* ptr)
{
	PX_GeoDrawSpray(psurface, x, y, size, color);
}

px_void PX_CanvasVM_EraserPaint(px_surface* psurface, px_float x, px_float y, px_float size, px_color color, px_void* ptr)
{
	PX_GeoDrawPenCircleEraser(psurface, x, y, size,PX_TRUE);
}

px_void PX_CanvasVM_HandPaint(px_surface* psurface, px_float x, px_float y, px_float size, px_color color, px_void* ptr)
{}

px_void PX_CanvasVMPushNode(PX_CanvasVM* pCanvasVM, PX_CanvasNode* pnode)
{
	if (pCanvasVM->currentPathCount < PX_CANVASVM_MAX_COUNT)
	{
		pCanvasVM->currentPath[pCanvasVM->currentPathCount] = *pnode;
		pCanvasVM->currentPathCount++;
	}

}

px_bool PX_CanvasVM_AddNode(PX_CanvasVM* pCanvasVM, px_float x, px_float y, px_float z)
{
	PX_CanvasNode node;
	if (pCanvasVM->currentPathCount==0)
	{
		node.x = x;
		node.y = y;
		node.z = z;
		PX_CanvasVMPushNode(pCanvasVM, &node);
		return PX_TRUE;
	}
	else
	{
		PX_CanvasNode lastNode = pCanvasVM->currentPath[pCanvasVM->currentPathCount - 1];
		px_point2D v=PX_POINT2D(x-lastNode.x,y-lastNode.y);
		px_float distance = PX_Point2DMod(v);
		v=PX_Point2DNormalization(v);
		if (distance > pCanvasVM->reg_size / PX_CANVASVM_STEP_DIV)
		{
			px_float fstep= (pCanvasVM->reg_size / PX_CANVASVM_STEP_DIV);
			px_int iStep;
			fstep=fstep < 1 ? 1 : fstep;
			iStep=(px_int)(distance/ fstep);

			node.x = lastNode.x + iStep*  v.x;
			node.y = lastNode.y + iStep * v.y;
			node.z = z;
			PX_CanvasVMPushNode(pCanvasVM, &node);
			return PX_TRUE;
		}
	}
	return PX_FALSE;
	
}

px_void PX_CanvasVM_PaintDot(PX_CanvasVM* pCanvasVM,px_float x, px_float y,px_float z)
{
	if (pCanvasVM->reg_tool >= 0 && pCanvasVM->reg_tool < pCanvasVM->reg_tools_count)
	{
		PX_CanvasVMTool* pTool = &pCanvasVM->tools[pCanvasVM->reg_tool];

		if (z<0)
		{
			z = 0;
		}
		if (z>1)
		{
			z = 1;
		}
		if (pTool->paint&&pCanvasVM->pcache_editing_surface)
		{
			pTool->paint(pCanvasVM->pcache_editing_surface, x, y, pCanvasVM->reg_size*z,pCanvasVM->reg_color ,pTool->ptr);
		}
	}
}

px_bool PX_CanvasVMInitialize(px_memorypool* mp, PX_CanvasVM* pCanvasVM, px_int width, px_int height,px_int zoom_width,px_int zoom_height)
{
	PX_memset(pCanvasVM, 0, sizeof(PX_CanvasVM));
	pCanvasVM->mp = mp;
	pCanvasVM->reg_showdot = PX_FALSE;
	pCanvasVM->reg_dotcolor = PX_COLOR_RED;
	pCanvasVM->width = width;
	pCanvasVM->height = height;
	pCanvasVM->scale = 1;
	pCanvasVM->max_scale = 10;
	pCanvasVM->reg_state=PX_CanvasVM_State_Standby;
	pCanvasVM->reg_state_id = 314159;
	PX_MemoryInitialize(mp,&pCanvasVM->shell);


	//hand--must be zero
	PX_CanvasVMRegisterTool(pCanvasVM, "hand", PX_CanvasVM_HandPaint, PX_FALSE, pCanvasVM);
	//Pen
	PX_CanvasVMRegisterTool(pCanvasVM, "pen",  PX_CanvasVM_PenPaint,PX_TRUE, pCanvasVM);
	//Paint
	PX_CanvasVMRegisterTool(pCanvasVM, "paint",  PX_CanvasVM_PaintPaint, PX_TRUE, pCanvasVM);
	//Spary
	PX_CanvasVMRegisterTool(pCanvasVM, "spary",  PX_CanvasVM_SparyPaint, PX_TRUE, pCanvasVM);
	//Eraser
	PX_CanvasVMRegisterTool(pCanvasVM, "eraser",  PX_CanvasVM_EraserPaint, PX_TRUE, pCanvasVM);
	


	if (!PX_TextureCreate(mp, &pCanvasVM->cache_backward_surface, pCanvasVM->width, pCanvasVM->height))
	{
		PX_ASSERT();
		return PX_NULL;
	}
	if (!PX_TextureCreate(mp, &pCanvasVM->cache_frontward_surface, pCanvasVM->width, pCanvasVM->height))
	{
		PX_ASSERT();
		return PX_NULL;
	}


	if (!PX_TextureCreate(mp, &pCanvasVM->cache_surface, pCanvasVM->width, pCanvasVM->height))
	{
		PX_ASSERT();
		return PX_NULL;
	}

	if (!PX_TextureCreate(mp, &pCanvasVM->view_surface, zoom_width, zoom_height))
	{
		PX_ASSERT();
		return PX_NULL;
	}


	if (pCanvasVM->cache_surface.width * 1.25f > pCanvasVM->view_surface.width)
	{
		pCanvasVM->min_scale = pCanvasVM->view_surface.width * 1.0f / (pCanvasVM->cache_surface.width * 1.25f);

		if (pCanvasVM->min_scale > pCanvasVM->view_surface.height * 1.0f / (pCanvasVM->cache_surface.height * 1.25f))
		{
			pCanvasVM->min_scale = pCanvasVM->view_surface.height * 1.0f / (pCanvasVM->cache_surface.height * 1.25f);
		}
	}
	pCanvasVM->scale = 1;

	pCanvasVM->view_x = pCanvasVM->cache_surface.width / 2.f;
	pCanvasVM->view_y = pCanvasVM->cache_surface.height / 2.f;
	return PX_TRUE;
}

px_void PX_CanvasVMSetZoom(PX_CanvasVM* pCanvasVM, px_int zoom_width, px_int zoom_height)
{
	PX_TextureFree(&pCanvasVM->view_surface);
	if (!PX_TextureCreate(pCanvasVM->mp, &pCanvasVM->view_surface, zoom_width, zoom_height))
	{
		//out of memory
		PX_ASSERT();
		return;
	}
}

px_void PX_CanvasVMOnBegin(PX_CanvasVM* pCanvasVM)
{
	if (PX_CanvasVMGetEditingLayerIndex(pCanvasVM) != -1)
	{
		if (pCanvasVM->reg_state == PX_CanvasVM_State_Standby)
		{
			PX_CanvasVMUpdateMaskEditingCache(pCanvasVM);
			pCanvasVM->reg_state = PX_CanvasVM_State_Painting;
			pCanvasVM->currentPathCount = 0;
			PX_memset(pCanvasVM->currentPath, 0, sizeof(pCanvasVM->currentPath));
		}
	}
}

px_void PX_CanvasVMOnDrawLastInterpolate(PX_CanvasVM* pCanvasVM)
{
	
	if (pCanvasVM->currentPathCount > 1)
	{
		px_point3D v1 = PX_POINT(pCanvasVM->currentPath[pCanvasVM->currentPathCount - 1].x, pCanvasVM->currentPath[pCanvasVM->currentPathCount - 1].y, pCanvasVM->currentPath[pCanvasVM->currentPathCount - 1].z);
		px_point3D v2 = PX_POINT(pCanvasVM->currentPath[pCanvasVM->currentPathCount - 2].x, pCanvasVM->currentPath[pCanvasVM->currentPathCount - 2].y, pCanvasVM->currentPath[pCanvasVM->currentPathCount - 2].z);
		px_point3D dir=PX_PointSub(v1,v2);
		px_float d=PX_sqrt(dir.x*dir.x+dir.y*dir.y);
		px_float step=pCanvasVM->reg_size/PX_CANVASVM_STEP_DIV;
		px_int i,dots;

		PX_ASSERTIF(pCanvasVM->reg_size == 0)


		dir = PX_PointNormalization(dir);
		
		dots = PX_APO(d / step);

		for (i = 1; i <= dots; i++)
		{
			PX_CanvasVM_PaintDot(pCanvasVM, v2.x+dir.x*i*step, v2.y+dir.y*i * step,v2.z+ dir.z*i * step);
		}

		if (pCanvasVM->reg_showdot&& pCanvasVM->pcache_editing_surface)
		{
			PX_GeoDrawSolidCircle(pCanvasVM->pcache_editing_surface, (px_int)pCanvasVM->currentPath[pCanvasVM->currentPathCount - 1].x, (px_int)pCanvasVM->currentPath[pCanvasVM->currentPathCount - 1].y, 2, PX_COLOR_RED);
		}
	}
	else if(pCanvasVM->currentPathCount ==1)
	{
		PX_CanvasVM_PaintDot(pCanvasVM, pCanvasVM->currentPath[0].x, pCanvasVM->currentPath[0].y, pCanvasVM->currentPath[0].z);
		if (pCanvasVM->reg_showdot&& pCanvasVM->pcache_editing_surface)
		{
			PX_GeoDrawSolidCircle(pCanvasVM->pcache_editing_surface, (px_int)pCanvasVM->currentPath[0].x, (px_int)pCanvasVM->currentPath[0].y, 2, PX_COLOR_RED);
		}
	}
	
}

px_void PX_CanvasVMOnMove(PX_CanvasVM* pCanvasVM, px_float x, px_float y, px_float z)
{
	px_float step = pCanvasVM->reg_size / PX_CANVASVM_STEP_DIV;
	if (pCanvasVM->reg_state != PX_CanvasVM_State_Painting)
	{
		return;
	}

	if (step<1)
	{
		step = 1;
	}
	if (pCanvasVM->currentPathCount==0)
	{
		if (pCanvasVM->reg_tool >= 0 && pCanvasVM->reg_tool < pCanvasVM->reg_tools_count)
		{
			if (pCanvasVM->tools[pCanvasVM->reg_tool].store)
			{
				if(PX_CanvasVM_AddNode(pCanvasVM, x, y, z))
					PX_CanvasVMOnDrawLastInterpolate(pCanvasVM);
			}
			else
			{
				PX_CanvasVM_PaintDot(pCanvasVM, x, y, z);
			}
		}
		else
		{
			PX_ASSERT();
		}
		pCanvasVM->reg_lastCursorPos = PX_POINT(x, y, z);
	}
	else
	{
		px_float d;
		px_point lastPoint;

		if (pCanvasVM->reg_tool <  0 || pCanvasVM->reg_tool >= pCanvasVM->reg_tools_count)
		{
			return;
		}
		
		lastPoint.x = pCanvasVM->currentPath[pCanvasVM->currentPathCount - 1].x;
		lastPoint.y = pCanvasVM->currentPath[pCanvasVM->currentPathCount - 1].y;
		lastPoint.z = pCanvasVM->currentPath[pCanvasVM->currentPathCount - 1].z;

		do
		{
			px_float m1, m2;
			px_point2D v1, v2;
			v1 = PX_POINT2D(lastPoint.x - pCanvasVM->reg_lastCursorPos.x, lastPoint.y - pCanvasVM->reg_lastCursorPos.y);
			v2 = PX_POINT2D(x - pCanvasVM->reg_lastCursorPos.x, y - pCanvasVM->reg_lastCursorPos.y);

			m1 = PX_Point2DMod(v1);
			m2 = PX_Point2DMod(v2);

			if (m1 * m2 > 1)
			{
				px_float _cosv = PX_Point2DDot(v1, v2) / m1 / m2;
				if (_cosv > 0.5f)
				{
					if (PX_CanvasVM_AddNode(pCanvasVM, pCanvasVM->reg_lastCursorPos.x, pCanvasVM->reg_lastCursorPos.y, pCanvasVM->reg_lastCursorPos.z))
						PX_CanvasVMOnDrawLastInterpolate(pCanvasVM);

					lastPoint.x = pCanvasVM->currentPath[pCanvasVM->currentPathCount - 1].x;
					lastPoint.y = pCanvasVM->currentPath[pCanvasVM->currentPathCount - 1].y;
					lastPoint.z = pCanvasVM->currentPath[pCanvasVM->currentPathCount - 1].z;
				}
			}

		} while (0);

		pCanvasVM->reg_lastCursorPos = PX_POINT(x, y, z);

		d = PX_sqrt((x - lastPoint.x) * (x - lastPoint.x) + (y - lastPoint.y) * (y - lastPoint.y));
		if (d > pCanvasVM->reg_filter_radius)
		{
			if (PX_CanvasVM_AddNode(pCanvasVM, pCanvasVM->reg_lastCursorPos.x, pCanvasVM->reg_lastCursorPos.y, pCanvasVM->reg_lastCursorPos.z))
				PX_CanvasVMOnDrawLastInterpolate(pCanvasVM);
		}
		else
		{
			return;
		}
	}

}

px_void PX_CanvasVMOnEndEx(PX_CanvasVM* pCanvasVM,px_bool store)
{
	PX_ASSERTIF(pCanvasVM->reg_tool == -1);

	if (pCanvasVM->reg_state != PX_CanvasVM_State_Painting)
	{
		return;
	}

	if (pCanvasVM->reg_lastCursorPos.x != pCanvasVM->currentPath[pCanvasVM->currentPathCount - 1].x)
	{
		if (pCanvasVM->reg_lastCursorPos.y != pCanvasVM->currentPath[pCanvasVM->currentPathCount - 1].y)
		{
			if (pCanvasVM->reg_tool >= 0 && pCanvasVM->reg_tool < pCanvasVM->reg_tools_count)
			{
				if (PX_CanvasVM_AddNode(pCanvasVM, pCanvasVM->reg_lastCursorPos.x, pCanvasVM->reg_lastCursorPos.y, pCanvasVM->reg_lastCursorPos.z))
					PX_CanvasVMOnDrawLastInterpolate(pCanvasVM);

			}
			else
			{
				PX_ASSERT();
			}

		}
	}
	if (store)
	{
		if (pCanvasVM->reg_tool >= 0 && pCanvasVM->reg_tool < pCanvasVM->reg_tools_count)
		{
			if (pCanvasVM->tools[pCanvasVM->reg_tool].store)
				PX_CanvasVMStorePaintShell(pCanvasVM);
		}
		PX_CanvasVMRepaintEditingLayerMiniPreview(pCanvasVM);
	}

	PX_memset(pCanvasVM->currentPath, 0, sizeof(pCanvasVM->currentPath));
	pCanvasVM->currentPathCount = 0;
	pCanvasVM->reg_state = PX_CanvasVM_State_Standby;
	pCanvasVM->reg_state_id++;
	return;
}
px_void PX_CanvasVMOnEnd(PX_CanvasVM* pCanvasVM)
{
	PX_CanvasVMOnEndEx(pCanvasVM, PX_TRUE);
}


px_bool PX_CanvasVMSelectToolByName(PX_CanvasVM* pCanvasVM, const px_char name[])
{
	px_int i;
	for (i=0;i<PX_COUNTOF(pCanvasVM->tools);i++)
	{
		if(!pCanvasVM->tools[i].name[0])
			break;
		if (PX_strequ2(pCanvasVM->tools[i].name,name))
		{
			pCanvasVM->reg_tool = i;
			return PX_TRUE;
		}
	}
	pCanvasVM->reg_tool = -1;
	return PX_FALSE;
}


px_void PX_CanvasVMSetSize(PX_CanvasVM* pCanvasVM, px_float size)
{
	pCanvasVM->reg_size = size;
}


px_void PX_CanvasVMSetFilter(PX_CanvasVM* pCanvasVM, px_float filter)
{
	pCanvasVM->reg_filter_radius = filter;
}

px_void PX_CanvasVMSetViewPosition(PX_CanvasVM* pCanvasVM, px_float x, px_float y)
{
	pCanvasVM->view_x = x;
	pCanvasVM->view_y = y;

	if (pCanvasVM->view_x < 0)
	{
		pCanvasVM->view_x = 0;
	};

	if (pCanvasVM->view_x > pCanvasVM->cache_surface.width)
	{
		pCanvasVM->view_x = pCanvasVM->cache_surface.width * 1.f;
	};

	if (pCanvasVM->view_y < 0)
	{
		pCanvasVM->view_y = 0;
	};
	if (pCanvasVM->view_y > pCanvasVM->cache_surface.height)
	{
		pCanvasVM->view_y =pCanvasVM->cache_surface.height * 1.f;
	};

	PX_CanvasVMRepaintCacheToView(pCanvasVM);
	pCanvasVM->reg_state_id++;
}

px_void PX_CanvasVMSetViewScale(PX_CanvasVM* pCanvasVM, px_float scale)
{
	pCanvasVM->scale = scale;
	pCanvasVM->reg_state_id++;
	if (pCanvasVM->scale < pCanvasVM->min_scale)
	{
		pCanvasVM->scale = pCanvasVM->min_scale;
	}
	if (pCanvasVM->scale > pCanvasVM->max_scale)
	{
		pCanvasVM->scale = pCanvasVM->max_scale;
	}
	PX_CanvasVMRepaintCacheToView(pCanvasVM);
	pCanvasVM->reg_state_id++;
	
}

px_void PX_CanvasVMResetView(PX_CanvasVM* pCanvasVM)
{
	pCanvasVM->scale = 1;
	pCanvasVM->view_x=pCanvasVM->cache_surface.width/2.f;
	pCanvasVM->view_y=pCanvasVM->cache_surface.height/2.f;
	pCanvasVM->reg_state_id++;
	PX_CanvasVMRepaintCacheToView(pCanvasVM);
}

px_void PX_CanvasVMSetColor(PX_CanvasVM* pCanvasVM, px_color color)
{
	pCanvasVM->reg_color = color;
}

px_void PX_CanvasVMLayerSetEditing(PX_CanvasVM* pCanvasVM, px_int index)
{
	
	px_int lastip;
	PX_CanvasVMShell_Header* pHeader;
	lastip= PX_CanvasVMGetLastShellIP(pCanvasVM);
	PX_ASSERTIF(lastip == -1);
	pHeader=(PX_CanvasVMShell_Header*)(pCanvasVM->shell.buffer+lastip);
	if (pHeader->opcode==PX_CANVASVM_OPCODE_LAYEREDIT)
	{
		pCanvasVM->reg_ip = lastip;
		pHeader->payload=(px_dword)index;
	}
	else
	{
		PX_CanvasVMShell_Header header = { 0 };
		header.opcode = PX_CANVASVM_OPCODE_LAYEREDIT;
		header.color = PX_COLOR_NONE;
		header.psize = 0;
		header.payload = (px_dword)index;
		PX_MemoryLeft(&pCanvasVM->shell, pCanvasVM->reg_ip);
		if (!PX_MemoryCat(&pCanvasVM->shell, &header, sizeof(header)))return;
		
	}

	PX_CanvasVMExecuteShell(pCanvasVM);
	PX_CanvasVMRepaintAllCache(pCanvasVM);
}

px_int PX_CanvasVMGetEditingLayerIndex(PX_CanvasVM* pCanvasVM)
{
	px_int i;
	for (i = 0; i < PX_COUNTOF(pCanvasVM->layers); i++)
	{
		if (pCanvasVM->layers[i].editing)
		{
			return i;
		}
	}
	return -1;
}

px_void PX_CanvasVMStorePaintShell(PX_CanvasVM* pCanvasVM)
{
	PX_CanvasVMShell_Header header = { 0 };
	header.opcode= PX_CANVASVM_OPCODE_PAINT;
	header.payload = pCanvasVM->reg_tool;
	header.color = pCanvasVM->reg_color;
	header.psize=pCanvasVM->reg_size;
	header.filter = pCanvasVM->reg_filter_radius;
	header.size = pCanvasVM->currentPathCount* sizeof(PX_CanvasNode);
	PX_MemoryLeft(&pCanvasVM->shell, pCanvasVM->reg_ip);
	if (!PX_MemoryCat(&pCanvasVM->shell, &header, sizeof(header)))return;
	if (!PX_MemoryCat(&pCanvasVM->shell, pCanvasVM->currentPath, sizeof(PX_CanvasNode) * pCanvasVM->currentPathCount))return;

	pCanvasVM->reg_ip = pCanvasVM->shell.usedsize;

}

px_void PX_CanvasVMRegisterTool(PX_CanvasVM* pCanvasVM,const px_char name[PX_CANVASVM_MAX_TOOL_NAME], pfun_PX_CanvasVMOnPaint paint,px_bool store, px_void* ptr)
{
	px_int i;
	for (i=0;i<PX_COUNTOF(pCanvasVM->tools);i++)
	{
		if (pCanvasVM->tools[i].name[0]==0)
		{
			PX_strset(pCanvasVM->tools[i].name, name);
			pCanvasVM->tools[i].paint = paint;
			pCanvasVM->tools[i].ptr = ptr;
			pCanvasVM->tools[i].store = store;
			pCanvasVM->reg_tools_count++;

			return;
		}
	}
}

px_void PX_CanvasVMFree(PX_CanvasVM* pCanvasVM)
{
	px_int i;
	PX_MemoryFree(&pCanvasVM->shell);


	for (i = 0; i < PX_COUNTOF(pCanvasVM->layers); i++)
	{
		if (pCanvasVM->layers[i].activating)
		{
			PX_TextureFree(&pCanvasVM->layers[i].surface_layer);
			PX_TextureFree(&pCanvasVM->layers[i].surface_preview_mini);
		}
		if (pCanvasVM->snapshot_layers[i].activating)
		{
			PX_TextureFree(&pCanvasVM->snapshot_layers[i].surface_layer);
			PX_TextureFree(&pCanvasVM->snapshot_layers[i].surface_preview_mini);
		}
	}

	PX_TextureFree(&pCanvasVM->cache_backward_surface);
	PX_TextureFree(&pCanvasVM->cache_frontward_surface);

	PX_TextureFree(&pCanvasVM->cache_surface);
	PX_TextureFree(&pCanvasVM->view_surface);
}

px_void PX_CanvasVMReset(PX_CanvasVM* pCanvasVM)
{
	px_int i;
	PX_MemoryClear(&pCanvasVM->shell);
	for (i = 0; i < PX_COUNTOF(pCanvasVM->layers); i++)
	{
		if (pCanvasVM->layers[i].activating)
		{
			PX_TextureFree(&pCanvasVM->layers[i].surface_layer);
			PX_TextureFree(&pCanvasVM->layers[i].surface_preview_mini);
		}
		if (pCanvasVM->snapshot_layers[i].activating)
		{
			PX_TextureFree(&pCanvasVM->snapshot_layers[i].surface_layer);
			PX_TextureFree(&pCanvasVM->snapshot_layers[i].surface_preview_mini);
		}
	}
	PX_memset(pCanvasVM->layers, 0, sizeof(pCanvasVM->layers));
	PX_memset(pCanvasVM->snapshot_layers, 0, sizeof(pCanvasVM->snapshot_layers));
	pCanvasVM->reg_ip = 0;
	pCanvasVM->reg_snapshot_ip = 0;

	PX_CanvasVMRepaintAllCache(pCanvasVM);
	PX_CanvasVMRepaintAllLayersPreview(pCanvasVM);
}

px_bool PX_CanvasVMExport(PX_CanvasVM* pCanvasVM, px_memory* bin)
{
	typedef struct
	{
		px_dword pi2;
		px_dword size;
	}header;
	header hd;
	hd.pi2 = 0x6282;
	hd.size = pCanvasVM->shell.usedsize;

	if (!PX_MemoryCat(bin, &hd, sizeof(hd)))return PX_FALSE;
	if (!PX_MemoryCat(bin, pCanvasVM->shell.buffer, pCanvasVM->shell.usedsize))return PX_FALSE;
	return PX_TRUE;
}

px_bool PX_CanvasVMExportAsPng(PX_CanvasVM* pCanvasVM, px_memory* bin)
{
	PX_PngSurfaceToBuffer(&pCanvasVM->cache_surface, bin);
	return PX_TRUE;
}

px_bool PX_CanvasVMImport(PX_CanvasVM* pCanvasVM, const px_byte data[], px_int size)
{
	typedef struct
	{
		px_dword pi2;
		px_dword size;
	}header;
	header* hd;
	PX_CanvasVMReset(pCanvasVM);
	//check header and size
	if ((px_uint)size < sizeof(header))return PX_FALSE;
	hd = (header*)data;
	if (hd->pi2 != 0x6282)return PX_FALSE;
	if ((px_uint)size < hd->size + sizeof(header))return PX_FALSE;
	//import
	if (!PX_MemoryCat(&pCanvasVM->shell, data + sizeof(header), hd->size))return PX_FALSE;
	pCanvasVM->reg_ip = 0;
	PX_CanvasVMPushad(pCanvasVM);
	while (pCanvasVM->reg_ip<pCanvasVM->shell.usedsize)
	{
		PX_CanvasVMExecuteShell(pCanvasVM);
	}
	PX_CanvasVMRepaintAllCache(pCanvasVM);
	PX_CanvasVMRepaintAllLayersPreview(pCanvasVM);
	PX_CanvasVMPopad(pCanvasVM);
	return PX_TRUE;
}

px_void PX_CanvasVMPushad(PX_CanvasVM* pCanvasVM)
{
	pCanvasVM->reg_tool2= pCanvasVM->reg_tool;
	pCanvasVM->reg_size2 = pCanvasVM->reg_size;
	pCanvasVM->reg_color2 = pCanvasVM->reg_color;
	pCanvasVM->reg_filter_radius2 = pCanvasVM->reg_filter_radius;

}

px_void PX_CanvasVMPopad(PX_CanvasVM* pCanvasVM)
{
	pCanvasVM->reg_tool = pCanvasVM->reg_tool2;
	pCanvasVM->reg_size = pCanvasVM->reg_size2;
	pCanvasVM->reg_color = pCanvasVM->reg_color2;
	pCanvasVM->reg_filter_radius = pCanvasVM->reg_filter_radius2;
}


px_void PX_CanvasVMRepaintLayerMiniPreview(PX_CanvasVM* pCanvasVM, px_int layerindex)
{
	if (layerindex < 0 || layerindex >= PX_COUNTOF(pCanvasVM->layers) || !pCanvasVM->layers[layerindex].activating)
	{
		PX_ASSERT();
		return;
	}
	PX_TextureScaleToTexture(&pCanvasVM->layers[layerindex].surface_layer, &pCanvasVM->layers[layerindex].surface_preview_mini);
}

px_void PX_CanvasVMRepaintEditingLayerMiniPreview(PX_CanvasVM* pCanvasVM)
{
	px_int i = 0;
	for (i = 0; i < PX_COUNTOF(pCanvasVM->layers); i++)
	{
		if (!pCanvasVM->layers[i].activating)
		{
			break;
		}
		if (pCanvasVM->layers[i].editing)
		{
			PX_CanvasVMRepaintLayerMiniPreview(pCanvasVM, i);
			return;
		}
	}
}

px_void PX_CanvasVMLayerCreate(PX_CanvasVM* pCanvasVM, const px_char name[])
{
	
   PX_CanvasVMShell_Header header = { 0 };
	//add shell
	header.opcode = PX_CANVASVM_OPCODE_LAYERCREATE;
	header.color = PX_COLOR_NONE;
	header.psize = 0;
	header.size = PX_strlen(name)+1;
	PX_MemoryLeft(&pCanvasVM->shell, pCanvasVM->reg_ip);
	if (!PX_MemoryCat(&pCanvasVM->shell, &header, sizeof(header)))return;
	if (!PX_MemoryCat(&pCanvasVM->shell, name, PX_strlen(name) + 1))return;

	PX_CanvasVMExecuteShell(pCanvasVM);
	PX_CanvasVMRepaintAllCache(pCanvasVM);
	return;
}

px_void PX_CanvasVMLayerDelete(PX_CanvasVM* pCanvasVM)
{
	PX_CanvasVMShell_Header header = { 0 };
	if (PX_CanvasVMGetEditingLayerIndex(pCanvasVM)==-1)
	{
		return;
	}
	//add shell
	header.opcode = PX_CANVASVM_OPCODE_LAYERDELETE;
	header.color = PX_COLOR_NONE;
	header.psize = 0;
	header.size = 0;
	PX_MemoryLeft(&pCanvasVM->shell, pCanvasVM->reg_ip);
	if (!PX_MemoryCat(&pCanvasVM->shell, &header, sizeof(header)))return;
	PX_CanvasVMExecuteShell(pCanvasVM);
	PX_CanvasVMRepaintAllCache(pCanvasVM);

}

px_void PX_CanvasVMLayerMoveUp(PX_CanvasVM* pCanvasVM)
{
	PX_CanvasVMShell_Header header = { 0 };

	if (PX_CanvasVMGetEditingLayerIndex(pCanvasVM) == -1)
	{
		return;
	}

	//add shell
	header.opcode = PX_CANVASVM_OPCODE_LAYERMOVEUP;
	header.color = PX_COLOR_NONE;
	header.psize = 0;
	header.size = 0;
	PX_MemoryLeft(&pCanvasVM->shell, pCanvasVM->reg_ip);
	if (!PX_MemoryCat(&pCanvasVM->shell, &header, sizeof(header)))return;

	PX_CanvasVMExecuteShell(pCanvasVM);

	PX_CanvasVMRepaintAllCache(pCanvasVM);
}

px_void PX_CanvasVMLayerMoveDown(PX_CanvasVM* pCanvasVM)
{
	PX_CanvasVMShell_Header header = { 0 };

	if (PX_CanvasVMGetEditingLayerIndex(pCanvasVM) == -1)
	{
		return;
	}
	//add shell
	header.opcode = PX_CANVASVM_OPCODE_LAYERMOVEDOWN;
	header.color = PX_COLOR_NONE;
	header.psize = 0;
	header.size = 0;
	PX_MemoryLeft(&pCanvasVM->shell, pCanvasVM->reg_ip);
	if (!PX_MemoryCat(&pCanvasVM->shell, &header, sizeof(header)))return;

	PX_CanvasVMExecuteShell(pCanvasVM);

	PX_CanvasVMRepaintAllCache(pCanvasVM);
}

px_void PX_CanvasVMLayerVisible(PX_CanvasVM* pCanvasVM)
{
	PX_CanvasVMShell_Header header = { 0 };

	if (PX_CanvasVMGetEditingLayerIndex(pCanvasVM) == -1)
	{
		return;
	}
	//add shell
	header.opcode = PX_CANVASVM_OPCODE_LAYERVISIBLE;
	header.color = PX_COLOR_NONE;
	header.psize = 0;
	header.size = 0;
	PX_MemoryLeft(&pCanvasVM->shell, pCanvasVM->reg_ip);
	if (!PX_MemoryCat(&pCanvasVM->shell, &header, sizeof(header)))return;

	PX_CanvasVMExecuteShell(pCanvasVM);

	PX_CanvasVMRepaintAllCache(pCanvasVM);


}

px_void PX_CanvasVMLayerClip(PX_CanvasVM* pCanvasVM)
{
	
	PX_CanvasVMShell_Header header = { 0 };

	if (PX_CanvasVMGetEditingLayerIndex(pCanvasVM) == -1)
	{
		return;
	}

	//add shell
	header.opcode = PX_CANVASVM_OPCODE_LAYERCLIP;
	header.color = PX_COLOR_NONE;
	header.psize = 0;
	header.size = 0;
	PX_MemoryLeft(&pCanvasVM->shell, pCanvasVM->reg_ip);
	if (!PX_MemoryCat(&pCanvasVM->shell, &header, sizeof(header)))return;

	PX_CanvasVMExecuteShell(pCanvasVM);

	PX_CanvasVMRepaintAllCache(pCanvasVM);
}

static px_void PX_CanvasVMRenderLayerToSurface(PX_CanvasVM* pCanvasVM, px_int index, px_texture* prendertarget)
{
	if (pCanvasVM->layers[index].activating == PX_FALSE)
	{
		return;
	}

	if (pCanvasVM->layers[index].attribute == PX_OBJECT_LAYER_ATTRIBUTE_CLIP)
	{
		//find normal
		px_int j;
		for (j = index + 1; j < PX_COUNTOF(pCanvasVM->layers); j++)
		{
			if (pCanvasVM->layers[j].activating == PX_FALSE)
			{
				break;
			}
			//find mask
			if (pCanvasVM->layers[j].attribute == PX_OBJECT_LAYER_ATTRIBUTE_NORMAL)
			{
				if (pCanvasVM->layers[j].visible)
				{
					PX_TextureRenderMask(prendertarget, &pCanvasVM->layers[j].surface_layer, &pCanvasVM->layers[index].surface_layer, 0, 0, PX_ALIGN_LEFTTOP, 0);
					return;
				}
			}
		}
	}

	if (pCanvasVM->layers[index].visible)
		PX_TextureRender(prendertarget, &pCanvasVM->layers[index].surface_layer, 0, 0, PX_ALIGN_LEFTTOP, 0);

}

px_void PX_CanvasVMRepaintAllLayersPreview(PX_CanvasVM* pCanvasVM)
{
	px_int i;
	for (i = 0; i < PX_COUNTOF(pCanvasVM->layers); i++)
	{
		if (pCanvasVM->layers[i].activating)
		{
			PX_CanvasVMRepaintLayerMiniPreview(pCanvasVM, i);
		}
	}
}

px_void PX_CanvasVMRepaintBackFrontCache(PX_CanvasVM* pdesc)
{
	px_texture* prender;
	px_int index = 0;

	if (pdesc->layers[0].activating==PX_FALSE)
	{
		return;
	}

	pdesc->reg_r_drawback = PX_FALSE;
	pdesc->reg_r_drawfront = PX_FALSE;

	PX_SurfaceClearAll(&pdesc->cache_frontward_surface, PX_COLOR(0, 255, 255, 255));
	PX_SurfaceClearAll(&pdesc->cache_backward_surface, PX_COLOR(0, 255, 255, 255));

	while (pdesc->layers[index].activating)
	{
		index++;
	}
	if (!index)
	{
		return;
	}
	index--;
	prender = &pdesc->cache_backward_surface;
	while (index >= 0)
	{

		if (pdesc->layers[index].editing)
		{
			prender = &pdesc->cache_frontward_surface;
		}
		else
		{
			if (prender == &pdesc->cache_backward_surface)
			{
				if (pdesc->layers[index].visible)
				{
					pdesc->reg_r_drawback = PX_TRUE;
				}
			}
			else
			{
				if (pdesc->layers[index].visible)
				{
					pdesc->reg_r_drawfront = PX_TRUE;
				}
			}
			PX_CanvasVMRenderLayerToSurface(pdesc, index, prender);
		}

		index--;
	}

}

px_void PX_CanvasVMUpdateMaskEditingCache(PX_CanvasVM* pdesc)
{
	px_int index = 0;
	px_int activatingIndex = 0;
	
	if (pdesc->layers[0].activating == PX_FALSE)
	{
		return;
	}

	pdesc->pcache_editing_surface = PX_NULL;
	pdesc->pcache_mask_surface = PX_NULL;

	for (activatingIndex = 0; activatingIndex < PX_COUNTOF(pdesc->layers); activatingIndex++)
	{
		if (pdesc->layers[activatingIndex].activating == PX_FALSE)
		{
			break;
		}
		if (pdesc->layers[activatingIndex].editing == PX_TRUE)
		{
			if (pdesc->layers[activatingIndex].visible)
			{
				pdesc->pcache_editing_surface = &pdesc->layers[activatingIndex].surface_layer;
			}

			if (pdesc->layers[activatingIndex].attribute == PX_OBJECT_LAYER_ATTRIBUTE_CLIP)
			{
				index = activatingIndex + 1;

				while (index < PX_COUNTOF(pdesc->layers))
				{
					if (!pdesc->layers[index].activating)
					{
						break;
					}
					if (pdesc->layers[index].attribute == PX_OBJECT_LAYER_ATTRIBUTE_NORMAL)
					{
						pdesc->pcache_mask_surface = &pdesc->layers[index].surface_layer;
						break;
					}
					index++;
				}
			}
			break;
		}
	}
}

px_void PX_CanvasVMRepaintCache(PX_CanvasVM* pdesc)
{

	if (pdesc->layers[0].activating == PX_FALSE)
	{
		PX_SurfaceClearAll(&pdesc->cache_surface, PX_COLOR(0, 255, 255, 255));
		return;
	}

	PX_SurfaceClearAll(&pdesc->cache_surface, PX_COLOR(255, 255, 255, 255));

	if (pdesc->reg_r_drawback)
	{
		PX_TextureRender(&pdesc->cache_surface, &pdesc->cache_backward_surface, 0, 0, PX_ALIGN_LEFTTOP, 0);
	}

	if (pdesc->pcache_editing_surface)
	{
		if (pdesc->pcache_mask_surface)
			PX_TextureRenderMask(&pdesc->cache_surface, pdesc->pcache_mask_surface, pdesc->pcache_editing_surface, 0, 0, PX_ALIGN_LEFTTOP, 0);
		else
			PX_TextureRender(&pdesc->cache_surface, pdesc->pcache_editing_surface, 0, 0, PX_ALIGN_LEFTTOP, 0);
	}

	if (pdesc->reg_r_drawfront)
	{
		PX_TextureRender(&pdesc->cache_surface, &pdesc->cache_frontward_surface, 0, 0, PX_ALIGN_LEFTTOP, 0);
	}

}

px_void PX_CanvasVMRepaintCacheToView(PX_CanvasVM* pdesc)
{
	px_int ltx, lty, rbx, rby;
	px_int v_width = pdesc->view_surface.width;
	px_int v_height = pdesc->view_surface.height;
	px_int clipw, cliph;
	px_texture clip_tex;


	if (pdesc->layers[0].activating == PX_FALSE)
	{
		PX_SurfaceClearAll(&pdesc->view_surface, PX_COLOR(0, 255, 255, 255));
		return;
	}

	clipw = (px_int)(v_width / pdesc->scale);
	cliph = (px_int)(v_height / pdesc->scale);

	ltx = (px_int)(pdesc->view_x - clipw / 2);
	rbx = (px_int)(pdesc->view_x + clipw / 2);
	lty = (px_int)(pdesc->view_y - cliph / 2);
	rby = (px_int)(pdesc->view_y + cliph / 2);

	if (pdesc->scale == 1)
	{
		PX_SurfaceClearAll(&pdesc->view_surface, PX_COLOR(0, 255, 255, 255));
		PX_TextureCover(&pdesc->view_surface, &pdesc->cache_surface, -ltx, -lty, PX_ALIGN_LEFTTOP);
	}
	else
	{
		if (!PX_TextureCreate(pdesc->mp, &clip_tex, clipw, cliph))
		{
			return;
		}

		PX_SurfaceClearAll(&clip_tex, PX_COLOR(0, 255, 255, 255));

		PX_TextureRender(&clip_tex, &pdesc->cache_surface, -ltx, -lty, PX_ALIGN_LEFTTOP, 0);

		if (!PX_TextureScaleToTexture(&clip_tex, &pdesc->view_surface))
		{
			PX_TextureFree(&clip_tex);
			return;
		}
		PX_TextureFree(&clip_tex);
	}


}

px_void PX_CanvasVMRepaintAllCache(PX_CanvasVM* pCanvasVM)
{
	PX_CanvasVMRepaintBackFrontCache(pCanvasVM);
	PX_CanvasVMUpdateMaskEditingCache(pCanvasVM);
	PX_CanvasVMRepaintCache(pCanvasVM);
	PX_CanvasVMRepaintCacheToView(pCanvasVM);
}

px_point2D PX_CanvasVMCanvasPostionToViewPosition(PX_CanvasVM* pdesc, px_point2D canvasPos)
{
	px_float sw, sh;
	px_point2D p;
	//distance
	sw = pdesc->view_surface.width * 1.0f / pdesc->scale;
	sh = pdesc->view_surface.height * 1.0f / pdesc->scale;
	p = PX_Point2DSub(canvasPos, PX_POINT2D(pdesc->view_x, pdesc->view_y));
	p.x = (p.x / sw * 2) * pdesc->view_surface.width / 2 + pdesc->view_surface.width / 2;
	p.y = (p.y / sh * 2) * pdesc->view_surface.height / 2 + pdesc->view_surface.height / 2;
	return p;
}

px_point2D PX_CanvasVMViewPositionToCanvasPostion(PX_CanvasVM* pdesc, px_point2D ViewPos)
{
	px_float left, right, top, bottom;
	px_float canvas_x_rate, canvas_y_rate;
	px_point2D p;
	//left
	left = pdesc->view_x - pdesc->view_surface.width / 2 / pdesc->scale;
	right = pdesc->view_x + pdesc->view_surface.width / 2 / pdesc->scale;
	top = pdesc->view_y - pdesc->view_surface.height / 2 / pdesc->scale;
	bottom = pdesc->view_y + pdesc->view_surface.height / 2 / pdesc->scale;

	canvas_x_rate = ViewPos.x / pdesc->view_surface.width;
	canvas_y_rate = ViewPos.y / pdesc->view_surface.height;

	p.x = canvas_x_rate * (right - left) + left;
	p.y = canvas_y_rate * (bottom - top) + top;
	return p;
}

px_bool PX_CanvasVMExecuteShell(PX_CanvasVM* pCanvasVM)
{
	PX_CanvasVMShell_Header* pHeader;
	if ((px_uint)pCanvasVM->reg_ip>pCanvasVM->shell.usedsize-sizeof(PX_CanvasVMShell_Header))
	{
		return PX_FALSE;
	}
	
	pHeader= (PX_CanvasVMShell_Header *)(pCanvasVM->shell.buffer+pCanvasVM->reg_ip);

	if ((px_uint)pCanvasVM->reg_ip + pHeader->size+ sizeof(PX_CanvasVMShell_Header)> (px_uint)pCanvasVM->shell.usedsize)
	{
		return PX_FALSE;
	}

	switch (pHeader->opcode)
	{
	case PX_CANVASVM_OPCODE_NONE:
		pCanvasVM->reg_ip+=sizeof(PX_CanvasVMShell_Header);
		break;
	case PX_CANVASVM_OPCODE_LAYEREDIT:
		{
			px_int index;
			index = pHeader->payload;

			PX_ASSERTIF(index < 0 || index >= PX_COUNTOF(pCanvasVM->layers));
			if (pCanvasVM->layers[index].activating)
			{
				px_int i;
				for (i = 0; i < PX_COUNTOF(pCanvasVM->layers); i++)
				{
					pCanvasVM->layers[i].editing = PX_FALSE;
				}
				pCanvasVM->layers[index].editing = PX_TRUE;
			}

			pCanvasVM->reg_ip += sizeof(PX_CanvasVMShell_Header);
		}
		break;
	case PX_CANVASVM_OPCODE_LAYERCREATE:
	{
		px_int i;

		if (pCanvasVM->shell.buffer[pCanvasVM->reg_ip+sizeof(PX_CanvasVMShell_Header)+pHeader->size-1]!='\0')
		{
			PX_ASSERT();
			return PX_FALSE;
		}

		for (i = 0; i < PX_COUNTOF(pCanvasVM->layers); i++)
		{
			if (pCanvasVM->layers[i].activating == PX_FALSE)
			{
				break;
			}
		}
		if (i == PX_COUNTOF(pCanvasVM->layers))
		{
			return PX_FALSE;
		}

		for (i = PX_COUNTOF(pCanvasVM->layers) - 1; i > 0; i--)
		{
			pCanvasVM->layers[i] = pCanvasVM->layers[i - 1];
			pCanvasVM->layers[i].editing = PX_FALSE;
		}

		if (!PX_TextureCreate(pCanvasVM->mp, &pCanvasVM->layers[i].surface_layer, pCanvasVM->width, pCanvasVM->height))
		{
			//out of memory
			PX_ASSERTIF(1);
			return PX_FALSE;
		}

		if (!PX_TextureCreate(pCanvasVM->mp, &pCanvasVM->layers[i].surface_preview_mini, 48, 48))
		{
			//out of memory
			PX_TextureFree(&pCanvasVM->layers[i].surface_layer);
			PX_ASSERTIF(1);
			return PX_FALSE;
		}

		PX_TextureClearAll(&pCanvasVM->layers[i].surface_layer, PX_COLOR(0, 255, 255, 255));

		pCanvasVM->layers[i].attribute = PX_OBJECT_LAYER_ATTRIBUTE_NORMAL;
		pCanvasVM->layers[i].activating = PX_TRUE;
		pCanvasVM->layers[i].editing = PX_TRUE;
		PX_strcpy(pCanvasVM->layers[i].name, (const px_char *)pCanvasVM->shell.buffer+pCanvasVM->reg_ip + sizeof(PX_CanvasVMShell_Header), sizeof(pCanvasVM->layers[i].name));
		pCanvasVM->layers[i].visible = PX_TRUE;

		pCanvasVM->reg_ip += sizeof(PX_CanvasVMShell_Header);
		pCanvasVM->reg_ip += pHeader->size;
	}
	break;
	case PX_CANVASVM_OPCODE_PAINT:
		{
			px_int i;
			px_int ptcount=pHeader->size/sizeof(PX_CanvasNode);
			PX_CanvasNode* pNode;
			PX_ASSERTIF((px_uint)pCanvasVM->shell.usedsize - (px_uint)pCanvasVM->reg_ip < sizeof(PX_CanvasVMShell_Header) + pHeader->size);
			
			pCanvasVM->reg_color = pHeader->color;
			pCanvasVM->reg_filter_radius = pHeader->filter;
			pCanvasVM->reg_tool= pHeader->payload;
			pCanvasVM->reg_size= pHeader->psize;
			PX_CanvasVMOnBegin(pCanvasVM);
			pNode=(PX_CanvasNode *)(pCanvasVM->shell.buffer+pCanvasVM->reg_ip+ sizeof(PX_CanvasVMShell_Header));
			for ( i = 0; i < ptcount; i++)
			{
				PX_CanvasVMOnMove(pCanvasVM, pNode[i].x, pNode[i].y, pNode[i].z);
			}
			PX_CanvasVMOnEndEx(pCanvasVM,PX_FALSE);
			pCanvasVM->reg_ip += sizeof(PX_CanvasVMShell_Header);
			pCanvasVM->reg_ip += pHeader->size;
		}
		break;
	case PX_CANVASVM_OPCODE_LAYERDELETE:
	{
		px_int select_index = PX_CanvasVMGetEditingLayerIndex(pCanvasVM);
		if (select_index != -1)
		{
			px_int i;
			PX_TextureFree(&pCanvasVM->layers[select_index].surface_preview_mini);
			PX_TextureFree(&pCanvasVM->layers[select_index].surface_layer);
			for (i = select_index; i < PX_COUNTOF(pCanvasVM->layers) - 1; i++)
			{
				pCanvasVM->layers[i] = pCanvasVM->layers[i + 1];
				if (!pCanvasVM->layers[i].activating)
				{
					break;
				}
			}
			PX_memset(&pCanvasVM->layers[i], 0, sizeof(pCanvasVM->layers[i]));
		}

		pCanvasVM->reg_ip += sizeof(PX_CanvasVMShell_Header);
	}
	break;
	case PX_CANVASVM_OPCODE_LAYERMOVEUP:
	{
		px_int select_index = PX_CanvasVMGetEditingLayerIndex(pCanvasVM);
		if (select_index > 0)
		{
			PX_Object_Layer layer = pCanvasVM->layers[select_index];
			pCanvasVM->layers[select_index] = pCanvasVM->layers[select_index - 1];
			pCanvasVM->layers[select_index - 1] = layer;
		}

		pCanvasVM->reg_ip += sizeof(PX_CanvasVMShell_Header);
	}
	break;
	case PX_CANVASVM_OPCODE_LAYERMOVEDOWN:
	{
		px_int select_index = PX_CanvasVMGetEditingLayerIndex(pCanvasVM);

		if (pCanvasVM->layers[select_index + 1].activating)
		{
			PX_Object_Layer layer = pCanvasVM->layers[select_index];
			pCanvasVM->layers[select_index] = pCanvasVM->layers[select_index + 1];
			pCanvasVM->layers[select_index + 1] = layer;
		}

		pCanvasVM->reg_ip += sizeof(PX_CanvasVMShell_Header);
	}
	break;
	case PX_CANVASVM_OPCODE_LAYERVISIBLE:
	{
		px_int select_index = PX_CanvasVMGetEditingLayerIndex(pCanvasVM);
		PX_ASSERTIF(select_index == -1);
		
		pCanvasVM->layers[select_index].visible = !pCanvasVM->layers[select_index].visible;

		pCanvasVM->reg_ip += sizeof(PX_CanvasVMShell_Header);
	}
	break;
	case PX_CANVASVM_OPCODE_LAYERCLIP:
	{
		px_int select_index = PX_CanvasVMGetEditingLayerIndex(pCanvasVM);
		PX_ASSERTIF(select_index ==-1)
		if (pCanvasVM->layers[select_index].attribute == PX_OBJECT_LAYER_ATTRIBUTE_CLIP)
		{
			pCanvasVM->layers[select_index].attribute = PX_OBJECT_LAYER_ATTRIBUTE_NORMAL;
		}
		else
		{
			pCanvasVM->layers[select_index].attribute = PX_OBJECT_LAYER_ATTRIBUTE_CLIP;
		}

		pCanvasVM->reg_ip += sizeof(PX_CanvasVMShell_Header);
	}
	break;
	default:
		break;
	}
	PX_CanvasVMUpdateMaskEditingCache(pCanvasVM);
	pCanvasVM->reg_state_id++;
	return PX_TRUE;
}

px_int PX_CanvasVMGetLastShellIP(PX_CanvasVM* pCanvasVM)
{
	px_int ip;
	PX_CanvasVMShell_Header* pheader;
	if (pCanvasVM->shell.usedsize == 0)
	{
		return -1;
	}
	ip = 0;
	while (PX_TRUE)
	{
		pheader = (PX_CanvasVMShell_Header*)(pCanvasVM->shell.buffer + ip);
		ip += sizeof(PX_CanvasVMShell_Header);
		ip += pheader->size;

		if (ip >= pCanvasVM->shell.usedsize)
		{
			return ip - sizeof(PX_CanvasVMShell_Header) - pheader->size;
		}
	}

}

static px_void PX_CanvasVMTakeSnapshot(PX_CanvasVM* pCanvasVM)
{
	px_int i;

	for (i = 0; i < PX_COUNTOF(pCanvasVM->layers); i++)
	{
		if (pCanvasVM->layers[i].activating)
		{
			PX_TextureFree(&pCanvasVM->layers[i].surface_layer);
			PX_TextureFree(&pCanvasVM->layers[i].surface_preview_mini);
		}
	}

	for (i = 0; i < PX_COUNTOF(pCanvasVM->snapshot_layers); i++)
	{
		pCanvasVM->layers[i] = pCanvasVM->snapshot_layers[i];
		if (pCanvasVM->snapshot_layers[i].activating)
		{
			PX_TextureCopy(pCanvasVM->mp, &pCanvasVM->snapshot_layers[i].surface_layer, &pCanvasVM->layers[i].surface_layer);
			PX_TextureCopy(pCanvasVM->mp, &pCanvasVM->snapshot_layers[i].surface_preview_mini, &pCanvasVM->layers[i].surface_preview_mini);
		}
	}

	pCanvasVM->reg_ip = pCanvasVM->reg_snapshot_ip;
}

px_void PX_CanvasVMMoveBack(PX_CanvasVM* pCanvasVM)
{
	PX_CanvasVMShell_Header* pHeader;
	px_int lastip = pCanvasVM->reg_ip;
	if (pCanvasVM->shell.usedsize==0)
	{
		return;
	}
	PX_CanvasVMPushad(pCanvasVM);
	PX_CanvasVMTakeSnapshot(pCanvasVM);

	while(PX_TRUE)
	{
		pHeader=(PX_CanvasVMShell_Header*)(pCanvasVM->shell.buffer+pCanvasVM->reg_ip);
		if ((px_uint)pCanvasVM->reg_ip+sizeof(PX_CanvasVMShell_Header)+ (px_uint)pHeader->size>= (px_uint)lastip)
		{
			break;
		}
		PX_CanvasVMExecuteShell(pCanvasVM);
	}
	pCanvasVM->reg_state_id++;
	PX_CanvasVMRepaintAllCache(pCanvasVM);
	PX_CanvasVMRepaintAllLayersPreview(pCanvasVM);
	PX_CanvasVMPopad(pCanvasVM);
}

px_void PX_CanvasVMMoveForward(PX_CanvasVM* pCanvasVM)
{
	if (pCanvasVM->reg_ip< pCanvasVM->shell.usedsize)
	{
		PX_CanvasVMPushad(pCanvasVM);
		PX_CanvasVMExecuteShell(pCanvasVM);
		PX_CanvasVMRepaintAllCache(pCanvasVM);
		PX_CanvasVMRepaintAllLayersPreview(pCanvasVM);
		PX_CanvasVMPopad(pCanvasVM);
	}
}