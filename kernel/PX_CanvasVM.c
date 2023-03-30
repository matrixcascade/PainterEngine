#include "PX_CanvasVM.h"

px_void PX_CanvasVM_PenDrawSamples(PX_CanvasVM* pCanvas, px_surface* psurface, px_point samples[], px_int detail)
{
	px_int i;
	px_float size = pCanvas->reg_size * pCanvas->currentPath[0].z;
	px_color clr = pCanvas->reg_color;
	if (pCanvas->currentPath[0].z <= 1)
		clr._argb.a = (px_byte)(clr._argb.a / (pCanvas->reg_step/6) * pCanvas->currentPath[0].z);
	for (i = 0; i < detail; i++)
	{
		PX_GeoDrawPenCircle(psurface, samples[i].x, samples[i].y, size, clr);
	}
}

px_void PX_CanvasVMOn_PenPaint(PX_CanvasVM* pCanvas, px_void* ptr)
{
	px_int i;
	px_int detail;
	px_point samples[PX_CANVASVM_MAX_DETAIL_SIZE] ;
	px_surface* ptarget_surface = pCanvas->ptarget_surface;

	if (ptarget_surface==PX_NULL||pCanvas->currentPathCount==0)
	{
		return;
	}
	switch (pCanvas->reg_filtermode)
	{
	case PX_CanvasVM_FilterMode_None:
		break;
	case PX_CanvasVM_FilterMode_PainterEngineFilter:
	{
		px_int count = pCanvas->currentPathCount;
		if (count > PX_CANVASVM_MAX_DETAIL_SIZE)
		{
			count = PX_CANVASVM_MAX_DETAIL_SIZE;
		}
		for (i = 0; i < count; i++)
		{
			samples[i].x = pCanvas->currentPath[i].x;
			samples[i].y = pCanvas->currentPath[i].y;
			samples[i].z = pCanvas->currentPath[i].z;
		}
		PX_GeoDrawPenSamplesLine(ptarget_surface, samples, count, pCanvas->reg_size, pCanvas->reg_color, pCanvas->reg_step, pCanvas->reg_filterfactor);
	}
	break;
	case PX_CanvasVM_FilterMode_Bezier:
	{
		px_int count = pCanvas->currentPathCount;
		px_float lenght = 0;
		if (count > PX_CANVASVM_MAX_DETAIL_SIZE)
		{
			count = PX_CANVASVM_MAX_DETAIL_SIZE;
		}
		for (i = 0; i < count; i++)
		{
			samples[i].x = pCanvas->currentPath[i].x;
			samples[i].y = pCanvas->currentPath[i].y;
			samples[i].z = pCanvas->currentPath[i].z;
			PX_GeoDrawPenCircle(ptarget_surface, pCanvas->currentPath[i].x, pCanvas->currentPath[i].y, pCanvas->reg_size, pCanvas->reg_color);
		}
		for (i = 1; i < count; i++)
		{
			lenght += PX_PointDistance(samples[i], samples[i - 1]);
			
		}
		lenght *= 2;

		for (i = 1; i < lenght; i++)
		{
			px_int j;
			px_point p1, p2;
			for (j = 0; j < count; j++)
			{
				samples[j].x = pCanvas->currentPath[j].x;
				samples[j].y = pCanvas->currentPath[j].y;
				samples[j].z = pCanvas->currentPath[j].z;
			}
			p1=PX_GeoGetBezierCurvePoint( samples, count, (i-1) * 1.0f / (lenght - 1));
			for (j = 0; j < count; j++)
			{
				samples[j].x = pCanvas->currentPath[j].x;
				samples[j].y = pCanvas->currentPath[j].y;
				samples[j].z = pCanvas->currentPath[j].z;
			}
			p2 = PX_GeoGetBezierCurvePoint( samples, count, i * 1.0f / (lenght - 1));
			PX_GeoDrawPenLine(ptarget_surface, p1.x, p1.y, p2.x, p2.y, pCanvas->reg_size, pCanvas->reg_color);
		}

		

	}
	break;
	case PX_CanvasVM_FilterMode_Breseham:
	{
		px_int count = pCanvas->currentPathCount;
		if (count > PX_CANVASVM_MAX_DETAIL_SIZE)
		{
			count = PX_CANVASVM_MAX_DETAIL_SIZE;
		}
		for (i = 1; i < count; i++)
		{
			PX_GeoDrawBresenhamLine(ptarget_surface, (px_int)pCanvas->currentPath[i - 1].x, (px_int)pCanvas->currentPath[i - 1].y, (px_int)pCanvas->currentPath[i].x, (px_int)pCanvas->currentPath[i].y, pCanvas->reg_color);
		}
	}
	break;
	default:
		if (pCanvas->currentPathCount > 1)
		{
			px_float dx, dy;
			dx = (pCanvas->currentPath[1].x - pCanvas->currentPath[0].x);
			dy = (pCanvas->currentPath[1].y - pCanvas->currentPath[0].y);
			detail = (px_int)PX_sqrt(dx * dx + dy * dy);
			detail = (detail == 0 ? 1 : detail);
			detail = (detail > PX_CANVASVM_MAX_DETAIL_SIZE ? PX_CANVASVM_MAX_DETAIL_SIZE : detail);

			PX_CanvasVMFilter(pCanvas, PX_CanvasVM_FilterMode_Liner, 1, samples, detail);
			PX_CanvasVM_PenDrawSamples(pCanvas, ptarget_surface, samples, detail);
		}

		for (i = 2; i < pCanvas->currentPathCount - 1; i++)
		{
			px_float dx, dy;
			dx = (pCanvas->currentPath[i].x - pCanvas->currentPath[i - 1].x);
			dy = (pCanvas->currentPath[i].y - pCanvas->currentPath[i - 1].y);
			detail = (px_int)PX_sqrt(dx * dx + dy * dy);
			detail = (detail == 0 ? 1 : detail);
			detail = (detail > PX_CANVASVM_MAX_DETAIL_SIZE ? PX_CANVASVM_MAX_DETAIL_SIZE : detail);

			PX_CanvasVMFilter(pCanvas, pCanvas->reg_filtermode, i, samples, detail);
			PX_CanvasVM_PenDrawSamples(pCanvas, ptarget_surface, samples, detail);
		}

		if (pCanvas->currentPathCount > 1)
		{
			px_float dx, dy;
			dx = (pCanvas->currentPath[pCanvas->currentPathCount - 1].x - pCanvas->currentPath[pCanvas->currentPathCount - 2].x);
			dy = (pCanvas->currentPath[pCanvas->currentPathCount - 1].y - pCanvas->currentPath[pCanvas->currentPathCount - 2].y);

			detail = (px_int)PX_sqrt(dx * dx + dy * dy);
			detail = (detail == 0 ? 1 : detail);
			detail = (detail > PX_CANVASVM_MAX_DETAIL_SIZE ? PX_CANVASVM_MAX_DETAIL_SIZE : detail);

			PX_CanvasVMFilter(pCanvas, PX_CanvasVM_FilterMode_Liner, pCanvas->currentPathCount - 1, samples, detail);
			PX_CanvasVM_PenDrawSamples(pCanvas, ptarget_surface, samples, detail);
		}
		break;
	}

	if (pCanvas->reg_showdot)
	{
		for (i = 0; i < pCanvas->currentPathCount; i++)
		{
			PX_GeoDrawPenCircle(ptarget_surface, pCanvas->currentPath[i].x, pCanvas->currentPath[i].y, 3, pCanvas->reg_dotcolor);
		}
	}
	
}


px_bool PX_CanvasVMInitialize(px_memorypool* mp, PX_CanvasVM* pCanvas)
{
	PX_memset(pCanvas, 0, sizeof(PX_CanvasVM));
	pCanvas->mp = mp;
	if (!PX_VectorInitialize(mp,&pCanvas->payload_offsets,sizeof(px_int),256))
	{
		return PX_FALSE;
	}
	PX_MemoryInitialize(mp,&pCanvas->payload);

	//Pen
	PX_CanvasVMRegisterTool(pCanvas, "pen", 1, PX_CanvasVMOn_PenPaint, 0);

	pCanvas->reg_showdot = PX_FALSE;
	pCanvas->reg_dotcolor = PX_COLOR_RED;
	return PX_TRUE;
}



px_void PX_CanvasVMOnBegin(PX_CanvasVM* pCanvas, PX_CANVASVM_OPCODE opcode, px_float size, px_color color, px_float step,  px_float reg_factor)
{
	if (pCanvas->reg_state==PX_CanvasVM_State_Painting)
	{
		PX_CanvasVMOnEnd(pCanvas);
	}

	if (pCanvas->reg_state == PX_CanvasVM_State_Standby)
	{
		px_int i;
		for (i=0;i<PX_COUNTOF(pCanvas->tools);i++)
		{
			if (pCanvas->tools[i].opcode==opcode)
			{
				break;
			}
		}
		if (i== PX_COUNTOF(pCanvas->tools))
		{
			//not found
			return;
		}
		pCanvas->reg_size=size;
		pCanvas->reg_color = color;
		pCanvas->reg_step = step;
		pCanvas->reg_filterfactor = reg_factor;
		pCanvas->reg_state = PX_CanvasVM_State_Painting;
		pCanvas->currentPathCount = 0;
		return;
	}
	PX_ASSERT();
}


px_void PX_CanvasVMFilter(PX_CanvasVM* pCanvas,PX_CanvasVM_FilterMode mode,px_int index, px_point samples[PX_CANVASVM_MAX_DETAIL_SIZE],px_int detail)
{
	if (detail<=0)
	{
		PX_ASSERT();
		return;
	}

	if (index<0||index>=pCanvas->currentPathCount)
	{
		PX_ASSERT();
		return;
	}
	else
	{
		switch(mode)
		{
		case PX_CanvasVM_FilterMode_None:
		{
			px_float x, y,z;
			x = pCanvas->currentPath[index].x;
			y = pCanvas->currentPath[index].y;
			z = pCanvas->currentPath[index].z;
			
			samples[0].x = x;
			samples[0].y = y;
			samples[0].z = z;

		}
		break;
		case PX_CanvasVM_FilterMode_Liner:
		{
			px_int i;
			px_point v;
			px_float x0, y0, z0;
			px_float x1, y1, z1;
			px_float step;
			if (index ==0)
			{
				px_float x, y, z;
				x = pCanvas->currentPath[index].x;
				y = pCanvas->currentPath[index].y;
				z = pCanvas->currentPath[index].z;

				samples[0].x = x;
				samples[0].y = y;
				samples[0].z = z;
				return;
			}

			x0 = pCanvas->currentPath[index-1].x;
			y0 = pCanvas->currentPath[index-1].y;
			z0 = pCanvas->currentPath[index-1].z;
			x1 = pCanvas->currentPath[index].x;
			y1 = pCanvas->currentPath[index].y;
			z1 = pCanvas->currentPath[index].z;
			v = PX_POINT(x1 - x0, y1 - y0, z1 - z0);
			v = PX_PointNormalization(v);
			step = PX_sqrt((x1 - x0) * (x1 - x0)+ (y1 - y0) *( y1 - y0))/detail;
			for (i=0;i<detail;i++)
			{
				samples[i] = PX_PointAdd(PX_POINT(x0, y0, z0), PX_PointMul(v, step * i));
			}
		}
		break;
		case PX_CanvasVM_FilterMode_BSpline:
		{
			if (index == 0)
			{
				px_float x, y, z;
				x = pCanvas->currentPath[index].x;
				y = pCanvas->currentPath[index].y;
				z = pCanvas->currentPath[index].z;

				samples[0].x = x;
				samples[0].y = y;
				samples[0].z = z;
				return;
			}
			else if (index==1)
			{
				px_int i;
				px_point v;
				px_float x0, y0, z0;
				px_float x1, y1, z1;
				px_float step;
				x0 = pCanvas->currentPath[index - 1].x;
				y0 = pCanvas->currentPath[index - 1].y;
				z0 = pCanvas->currentPath[index - 1].z;
				x1 = pCanvas->currentPath[index].x;
				y1 = pCanvas->currentPath[index].y;
				z1 = pCanvas->currentPath[index].z;
				v = PX_POINT(x1 - x0, y1 - y0, z1 - z0);
				v = PX_PointNormalization(v);
				step = PX_sqrt((x1 - x0) * (x1 - x0) + (y1 - y0) * (y1 - y0)) / detail;
				for (i = 0; i < detail; i++)
				{
					samples[i] = PX_PointAdd(PX_POINT(x0, y0, z0), PX_PointMul(v, step * i));
				}
			}
			else
			{
				if (index<pCanvas->currentPathCount-1)
				{
					px_point2D samples2D[PX_CANVASVM_MAX_DETAIL_SIZE];
					px_point2D bspline_order3[4],v,v1,v2,v3;
					px_float x0, y0, z0,x1, y1, z1, x2, y2, z2, x3, y3, z3,d1,d2,d3;
					px_int i;
					x0 = pCanvas->currentPath[index - 2].x;
					y0 = pCanvas->currentPath[index - 2].y;
					z0 = pCanvas->currentPath[index - 2].z;
					
					x1 = pCanvas->currentPath[index-1].x;
					y1 = pCanvas->currentPath[index-1].y;
					z1 = pCanvas->currentPath[index-1].z;
					
					x2 = pCanvas->currentPath[index].x;
					y2 = pCanvas->currentPath[index].y;
					z2 = pCanvas->currentPath[index].z;

					x3 = pCanvas->currentPath[index + 1].x;
					y3 = pCanvas->currentPath[index + 1].y;
					z3 = pCanvas->currentPath[index + 1].z;
					
					bspline_order3[0].x = x1;
					bspline_order3[0].y = y1;

					v1 = (PX_POINT2D((x1 - x0), (y1 - y0)));
					v2= (PX_POINT2D((x2 - x1), (y2 - y1)));
					v3 = (PX_POINT2D((x3 - x2), (y3 - y2)));
					d1 = PX_Point2DMod(v1);
					d2 = PX_Point2DMod(v2);
					d3 = PX_Point2DMod(v3);
					v1 = PX_Point2DMul(v1, d2/d1);
					v = PX_Point2DAdd(v1,v2);
					v = PX_Point2DMul(v, 0.25f * pCanvas->reg_filterfactor);

					bspline_order3[1].x =x1+ v.x;
					bspline_order3[1].y =y1+ v.y;

					v3 = PX_Point2DMul(v3, d2 / d3);
					v = PX_Point2DAdd(v2, v3);
					v = PX_Point2DMul(v, -0.25f * pCanvas->reg_filterfactor);

					bspline_order3[2].x = x2 + v.x;
					bspline_order3[2].y = y2 + v.y;

					bspline_order3[3].x = x2;
					bspline_order3[3].y = y2;

					PX_GeoBSpline3(bspline_order3, samples2D, detail);

					for (i=0;i<detail;i++)
					{
						samples[i].x = samples2D[i].x;
						samples[i].y = samples2D[i].y;
						samples[i].z = z1 + (z2 - z1) * i / detail;
					}
				}
				else
				{
					PX_ASSERT();
				}
			}
		}
		break;
		case PX_CanvasVM_FilterMode_PainterEngineFilter:
		{
			PX_ASSERT();
			return;
		}
		break;
		default:
			return;
		}
	}
	
}

px_void PX_CanvasVMOnMove(PX_CanvasVM* pCanvas, px_float x, px_float y, px_float z)
{
	if (pCanvas->reg_state != PX_CanvasVM_State_Painting)
	{
		return;
	}
	pCanvas->reg_lastpoint.x = x;
	pCanvas->reg_lastpoint.y = y;
	pCanvas->reg_lastpoint.z = z;

	if (pCanvas->reg_current_tool_index >= 0 && pCanvas->reg_current_tool_index < pCanvas->reg_tools_count)
	{
		PX_CanvasNode node = { 0 };
		px_float distance,px,py,dx,dy;

			if (pCanvas->currentPathCount==0)
			{
				node.x = x;
				node.y = y;
				node.z = z;
				PX_CanvasVMPushNode(pCanvas, &node);
				PX_CanvasVMPushNode(pCanvas, &node);
			}
			else
			{

				if (pCanvas->currentPathCount>2)
				{
					px_float m1, m2;
					px_point2D v1, v2;
					v1 = PX_POINT2D(\
						pCanvas->currentPath[pCanvas->currentPathCount - 1].x - pCanvas->currentPath[pCanvas->currentPathCount - 2].x,
						pCanvas->currentPath[pCanvas->currentPathCount - 1].y - pCanvas->currentPath[pCanvas->currentPathCount - 2].y);

					v2 = PX_POINT2D(\
						x - pCanvas->currentPath[pCanvas->currentPathCount - 1].x,
						y - pCanvas->currentPath[pCanvas->currentPathCount - 1].y);

					m1 = PX_Point2DMod(v1);
					m2 = PX_Point2DMod(v2);
					if (m1*m2!=0)
					{
						if (PX_Point2DDot(v1,v2)/m1/m2<0.7f)
						{
							node.x = x;
							node.y = y;
							node.z = z;
							PX_CanvasVMPushNode(pCanvas, &node);
						}
					}
				}
				px = pCanvas->currentPath[pCanvas->currentPathCount - 1].x;
				py = pCanvas->currentPath[pCanvas->currentPathCount - 1].y;

				dx= px - pCanvas->currentPath[pCanvas->currentPathCount - 2].x;
				dy = py - pCanvas->currentPath[pCanvas->currentPathCount - 2].y;
				distance = PX_sqrt(dx* dx + dy * dy);
				if (distance< pCanvas->reg_step)
				{
					pCanvas->currentPath[pCanvas->currentPathCount - 1].x = x;
					pCanvas->currentPath[pCanvas->currentPathCount - 1].y = y;
				}
				else
				{
					node.x = x;
					node.y = y;
					node.z = z;
					PX_CanvasVMPushNode(pCanvas, &node);
				}
		}
		return;
	}
	PX_ASSERT();
	return;
}




px_void PX_CanvasVMPaint(PX_CanvasVM* pCanvas, px_surface* prender, px_surface* pmask)
{
	if (pCanvas->reg_current_tool_index >= 0 && pCanvas->reg_current_tool_index < pCanvas->reg_tools_count)
	{
		pCanvas->ptarget_surface = prender;
		pCanvas->pmask_surface = pmask;
		pCanvas->tools[pCanvas->reg_current_tool_index].paint(pCanvas, pCanvas->tools[pCanvas->reg_current_tool_index].ptr);
		return;
	}
	PX_ASSERT();

}

px_void PX_CanvasVMOnEnd(PX_CanvasVM* pCanvas)
{
	if (pCanvas->reg_current_tool_index >= 0 && pCanvas->reg_current_tool_index < pCanvas->reg_tools_count)
	{
		PX_CanvasNode node = { 0 };
		px_float  lx, ly;
		lx = pCanvas->currentPath[pCanvas->currentPathCount - 1].x;
		ly = pCanvas->currentPath[pCanvas->currentPathCount - 1].y;
		if (lx!=pCanvas->reg_lastpoint.x|| ly != pCanvas->reg_lastpoint.y)
		{
			node.x = pCanvas->reg_lastpoint.x;
			node.y = pCanvas->reg_lastpoint.y;
			node.z = pCanvas->reg_lastpoint.z;
			PX_CanvasVMPushNode(pCanvas, &node);
		}
		
		pCanvas->reg_state = PX_CanvasVM_State_Standby;
		return;
	}
	PX_ASSERT();
}

px_bool PX_CanvasVMSelectToolByIndex(PX_CanvasVM* pCanvas, px_int index)
{
	if (index >= 0 && index < pCanvas->reg_tools_count)
	{
		pCanvas->reg_current_tool_index = index;
		return PX_TRUE;
	}
	return PX_FALSE;
}

px_bool PX_CanvasVMSelectToolByName(PX_CanvasVM* pCanvas, const px_char name[])
{
	px_int i;
	for (i=0;i<PX_COUNTOF(pCanvas->tools);i++)
	{
		if(!pCanvas->tools[i].name[0])
			break;
		if (PX_strequ2(pCanvas->tools[i].name,name))
		{
			pCanvas->reg_current_tool_index = i;
			return PX_TRUE;
		}
	}
	return PX_FALSE;
}

px_void PX_CanvasVMSetTargetTexture(PX_CanvasVM* pCanvas, px_texture* ptex)
{
	pCanvas->ptarget_surface = ptex;
}

px_void PX_CanvasVMSetMaskTexture(PX_CanvasVM* pCanvas, px_texture* ptex)
{
	pCanvas->pmask_surface = ptex;
}

px_void PX_CanvasVMSetSize(PX_CanvasVM* pCanvas, px_float size)
{
	pCanvas->reg_size = size;
}

px_void PX_CanvasVMPushNode(PX_CanvasVM* pCanvas, PX_CanvasNode* pnode)
{
	if (pCanvas->currentPathCount< PX_CANVASVM_MAX_COUNT)
	{
		pCanvas->currentPath[pCanvas->currentPathCount] = *pnode;
		pCanvas->currentPathCount++;
	}
	
}

px_void PX_CanvasVMStorePayload(PX_CanvasVM* pCanvas)
{
	PX_CanvasNode_Header header = { 0 };
	header.opcode = pCanvas->tools[pCanvas->reg_current_tool_index].opcode;
	header.size = pCanvas->currentPathCount;
	if (!PX_MemoryCat(&pCanvas->payload, &header, sizeof(header)))return;
	if (!PX_MemoryCat(&pCanvas->payload, pCanvas->currentPath, sizeof(PX_CanvasNode) * pCanvas->currentPathCount))return;
	pCanvas->currentPathCount = 0;
}

px_void PX_CanvasVMRegisterTool(PX_CanvasVM* pCanvas, px_char name[PX_CANVASVM_MAX_TOOL_NAME], px_dword opcode, pfun_PX_CanvasVMOnPaint paint, px_void* ptr)
{
	px_int i;
	for (i=0;i<PX_COUNTOF(pCanvas->tools);i++)
	{
		if (pCanvas->tools[i].name[0]==0)
		{
			PX_strset(pCanvas->tools[i].name, name);
			pCanvas->tools[i].opcode = opcode;
			pCanvas->tools[i].paint = paint;
			pCanvas->tools[i].ptr = ptr;
			pCanvas->reg_tools_count++;
			return;
		}
	}
}

px_void PX_CanvasVMFree(PX_CanvasVM* pCanvas)
{
	PX_VectorFree( &pCanvas->payload_offsets);
	PX_MemoryFree(&pCanvas->payload);

}

px_void PX_CanvasVMSetMode(PX_CanvasVM* pCanvas, PX_CanvasVM_FilterMode fmode)
{
	pCanvas->reg_filtermode = fmode;
}

