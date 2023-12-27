#include "PX_Object_AsmDebugger.h"

px_void PX_Object_AsmDebuggerMonitorOnRender(px_surface* psurface, PX_Object* pObject, px_dword elapsed)
{
	PX_Object_AsmDebugger_VarMonitor *pvar = (PX_Object_AsmDebugger_VarMonitor *)PX_Object_ListItemGetData(pObject);
	PX_Object* pDescObject = (PX_Object*)pObject->User_ptr;
	PX_Object_AsmDebugger* pDesc = PX_ObjectGetDesc(PX_Object_AsmDebugger, pDescObject);

	PX_FontModuleDrawText(psurface, pDesc->fm, (px_int)pObject->x + 8, (px_int)pObject->y + (px_int)(pObject->Height / 2), PX_ALIGN_LEFTMID, pvar->name, PX_COLOR(255,255,255,0));
}

px_bool PX_Object_AsmDebuggerMonitorOnCreate(px_memorypool* mp, PX_Object* ItemObject, px_void* userptr)
{
	ItemObject->Func_ObjectRender = PX_Object_AsmDebuggerMonitorOnRender;
	ItemObject->User_ptr = userptr;
	return PX_TRUE;
}

px_void PX_Object_AsmDebuggerPrintVar(PX_Object_AsmDebugger* pAsm, PX_VM_VARIABLE* pvar, px_char name[])
{
	px_char content[64];
	switch (pvar->type)
	{
	case PX_VM_VARIABLE_TYPE_INT:
		PX_sprintf1(content, sizeof(content), "%1(int)", PX_STRINGFORMAT_STRING(name));
		PX_Object_LabelSetText(pAsm->label_name, content);
		PX_sprintf1(content, sizeof(content), "%1", PX_STRINGFORMAT_INT(pvar->_int));
		PX_Object_AutoTextSetText(pAsm->autotext_data, content);
		break;
	case PX_VM_VARIABLE_TYPE_FLOAT:
		PX_sprintf1(content, sizeof(content), "%1:float", PX_STRINGFORMAT_STRING(name));
		PX_Object_LabelSetText(pAsm->label_name, content);
		PX_sprintf1(content, sizeof(content), "%1", PX_STRINGFORMAT_FLOAT(pvar->_float));
		PX_Object_AutoTextSetText(pAsm->autotext_data, content);
		break;
	case PX_VM_VARIABLE_TYPE_MEMORY:
		PX_sprintf1(content, sizeof(content), "%1:memory", PX_STRINGFORMAT_STRING(name));
		PX_Object_LabelSetText(pAsm->label_name, content);
		PX_Object_AutoTextSetText(pAsm->autotext_data, "");
		break;
	case PX_VM_VARIABLE_TYPE_STRING:
		PX_sprintf1(content, sizeof(content), "%1:string", PX_STRINGFORMAT_STRING(name));
		PX_Object_LabelSetText(pAsm->label_name, content);
		PX_Object_AutoTextSetText(pAsm->autotext_data, pvar->_string.buffer);
		break;
	case PX_VM_VARIABLE_TYPE_HANDLE:
		PX_sprintf1(content, sizeof(content), "%1(handle)", PX_STRINGFORMAT_STRING(name));
		PX_Object_LabelSetText(pAsm->label_name, content);
		PX_sprintf1(content, sizeof(content), "%1", PX_STRINGFORMAT_INT((px_int)(pvar->_uint)));
		PX_Object_AutoTextSetText(pAsm->autotext_data, content);
		break;
	default:
		break;
	}
}


px_void PX_Object_AsmDebuggerSourceOnRender(px_surface *psurface,PX_Object *pObject,px_dword elapsed)
{
	PX_Object_AsmDebugger_Line* pline = (PX_Object_AsmDebugger_Line *)PX_Object_ListItemGetData(pObject);
	PX_Object* pDescObject = (PX_Object*)pObject->User_ptr;
	PX_Object_AsmDebugger* pDesc = PX_ObjectGetDesc(PX_Object_AsmDebugger, pDescObject);
	px_color clr=PX_COLOR(255,0,255,0);
	px_char num[16];
	
	if (pline->bbreak)
	{
		PX_GeoDrawRect(psurface, (px_int)pObject->x, (px_int)pObject->y, (px_int)(pObject->x + pObject->Width), (px_int)(pObject->y + pObject->Height), PX_COLOR(64, 255, 255, 0));
		PX_GeoDrawSolidCircle(psurface, (px_int)(pObject->x + pObject->Height / 2), (px_int)(pObject->y + pObject->Height / 2), 6, PX_COLOR(192, 255, 0, 0));
	}

	if (pline->brun)
	{
		PX_GeoDrawRect(psurface, (px_int)pObject->x, (px_int)pObject->y, (px_int)(pObject->x + pObject->Width), (px_int)(pObject->y + pObject->Height), PX_COLOR(64, 0, 255, 0));
		PX_GeoDrawTriangle(psurface, PX_POINT2D(pObject->x, pObject->y), PX_POINT2D(pObject->x, pObject->y + pObject->Height), PX_POINT2D(pObject->x + pObject->Height / 2, pObject->y + pObject->Height / 2), PX_COLOR(255, 0, 255, 0));
	}


	if (pline->source.buffer[0]==';')
	{
		clr = PX_COLOR(255, 192, 192, 192);
	}
	else if (PX_memequ(pline->source.buffer, "_", sizeof("_") - 1))
	{
		clr = PX_COLOR(255, 64, 230, 180);
	}
	else if (PX_memequ(pline->source.buffer, "EXPORT FUNCTION", sizeof("EXPORT FUNCTION") - 1) || PX_memequ(pline->source.buffer, "FUNCTION", sizeof("FUNCTION") - 1))
	{
		clr = PX_COLOR(255, 255, 64, 96);
	}
	else if(PX_memequ(pline->source.buffer, "RET", sizeof("RET") - 1))
	{
		clr = PX_COLOR(255, 255, 192, 96);
	}
	else if (PX_memequ(pline->source.buffer, "CALL", sizeof("CALL") - 1))
	{
		clr = PX_COLOR(255, 64, 128, 255);
	}
	PX_sprintf1(num, sizeof(num), "%1:",PX_STRINGFORMAT_INT(PX_Object_ListItemGetIndex(pObject)+1));
	PX_FontModuleDrawText(psurface, pDesc->fm, (px_int)pObject->x + 50, (px_int)pObject->y + (px_int)(pObject->Height / 2), PX_ALIGN_RIGHTMID, num, PX_COLOR(255,192,128,255));
	PX_FontModuleDrawText(psurface, pDesc->fm, (px_int)pObject->x + 64, (px_int)pObject->y + (px_int)(pObject->Height / 2), PX_ALIGN_LEFTMID, pline->source.buffer, clr);
}

px_bool PX_Object_AsmDebuggerSourceOnCreate(px_memorypool* mp, PX_Object* ItemObject, px_void* userptr)
{
	ItemObject->Func_ObjectRender = PX_Object_AsmDebuggerSourceOnRender;
	ItemObject->User_ptr = userptr;
	return PX_TRUE;
}
px_void PX_Object_AsmDebuggerRender(px_surface* prendersurface, PX_Object* pObject, px_dword elapsed)
{
	PX_Object_AsmDebugger* pDesc = PX_ObjectGetDesc(PX_Object_AsmDebugger, pObject);
	if (pObject->Width<30||pObject->Height<30)
	{
		return;
	}
	do
	{
		pDesc->button_run->x = 0;
		pDesc->button_run->y = 0;

		

		pDesc->button_step->x = pDesc->button_run->x + pDesc->button_run->Width ;
		pDesc->button_step->y = 0;

		pDesc->button_pause->x = pDesc->button_step->x + pDesc->button_step->Width;
		pDesc->button_pause->y = 0;

		pDesc->button_break->x = pDesc->button_pause->x + pDesc->button_pause->Width ;
		pDesc->button_break->y = 0;

		pDesc->button_reset->x = pDesc->button_break->x + pDesc->button_break->Width;
		pDesc->button_reset->y = 0;

		pDesc->label_register->x = pDesc->button_reset->x + pDesc->button_reset->Width;
		pDesc->label_register->y = 0;



		pDesc->list_source->x = 0;
		pDesc->list_source->y = pDesc->button_run->Height;
		pDesc->list_source->Width = pObject->Width * 2 / 3;
		pDesc->list_source->Height = pObject->Height - pDesc->button_run->Height;

		pDesc->list_monitor->x = pDesc->list_source->Width;
		pDesc->list_monitor->y = pDesc->button_run->Height;
		pDesc->list_monitor->Width = pObject->Width / 3;
		pDesc->list_monitor->Height = (pObject->Height - pDesc->button_run->Height) / 2;

		pDesc->label_name->x = pDesc->list_source->Width;
		pDesc->label_name->y = pDesc->button_run->Height + pDesc->list_monitor->Height;
		pDesc->label_name->Width=pObject->Width / 3;

		pDesc->autotext_data->x = pDesc->list_source->Width;
		pDesc->autotext_data->y = pDesc->label_name->y+ pDesc->label_name->Height;
		pDesc->autotext_data->Width = pObject->Width / 3;



	} while (0);

	if (pDesc->vm && pDesc->map)
	{
		px_int i;
		px_int T = pDesc->vm->T;
		px_int ip = pDesc->vm->pThread[pDesc->vm->T].IP;
		px_int line = PX_VMDebuggerMapIpToLine(pDesc->map, ip);
		px_int sp= pDesc->vm->pThread[pDesc->vm->T].SP;
		px_int bp = pDesc->vm->pThread[pDesc->vm->T].BP;
		px_char content[64];
		PX_sprintf4(content, sizeof(content), " Thread:%1 IP:%2 SP:%3 BP:%4", PX_STRINGFORMAT_INT(T), PX_STRINGFORMAT_INT(ip), PX_STRINGFORMAT_INT(sp), PX_STRINGFORMAT_INT(bp));
		PX_Object_LabelSetText(pDesc->label_register, content);



		if (ip != pDesc->lastip&&ip>=0)
		{
			px_char opcode,opcode_name[16];
			px_int paramcount;
			px_char optype[3];
			px_int param[3];
			pDesc->lastip = ip;

			for (i = 0; i < pDesc->lines.size; i++)
			{
				PX_Object_AsmDebugger_Line* pline = PX_VECTORAT(PX_Object_AsmDebugger_Line, &pDesc->lines, i);
				pline->brun = PX_FALSE;
			}


			if ( line >= 0 && line < pDesc->lines.size)
			{
				PX_Object_ListViewFocus(pDesc->list_source, line);
				PX_VECTORAT(PX_Object_AsmDebugger_Line, &pDesc->lines, line)->brun = PX_TRUE;
			}
	
			if (/*pDesc->vm->Suspend &&*/PX_VMDebuggerInstruction(pDesc->vm, ip, &opcode, opcode_name, &paramcount, optype, param))
			{
				PX_Object_AsmDebugger_VarMonitor pvarMonitor;

				for (i = 0; i < paramcount; i++)
				{
					PX_VM_VARIABLE* pvar = PX_VMGetVariablePointer(pDesc->vm, optype[i], param[i]);
					px_char name[32];

					if (pvar)
					{
						pvarMonitor.pvar = pvar;
						if (PX_VMDebuggerGetVarParamName(pDesc->vm, optype[i], param[i], name, sizeof(name)))
						{
							PX_sprintf2(pvarMonitor.name, sizeof(pvarMonitor.name), "%1:%2", PX_STRINGFORMAT_INT(line + 1), PX_STRINGFORMAT_STRING(name));
							if (pDesc->monitor.size >= PX_OBJECT_ASMDEBUGGER_VARMONITOR_MAX_COUNT)
							{
								PX_VectorErase(&pDesc->monitor, 0);
							}
							PX_VectorPushback(&pDesc->monitor, &pvarMonitor);
							PX_Object_AsmDebuggerPrintVar(pDesc, pvar, name);
						}
					}

				}
				PX_Object_ListClear(pDesc->list_monitor);
				for (i = 0; i < pDesc->monitor.size; i++)
				{
					PX_Object_ListAdd(pDesc->list_monitor, PX_VECTORAT(PX_Object_AsmDebugger_VarMonitor, &pDesc->monitor, i));
				}
				PX_Object_ListMoveToBottom(pDesc->list_monitor);
			}




		}
		
		if (pDesc->vm->debug)
		{
			for (i = 0; i < pDesc->lines.size; i++)
			{
				PX_Object_AsmDebugger_Line* pline = PX_VECTORAT(PX_Object_AsmDebugger_Line, &pDesc->lines, i);
				pline->bbreak = PX_FALSE;
			}

			for (i = 0; i < PX_COUNTOF(pDesc->vm->breakpoints); i++)
			{
				if (pDesc->vm->breakpoints[i] != -1)
				{
					line = PX_VMDebuggerMapIpToLine(pDesc->map, pDesc->vm->breakpoints[i]);
					if (line && line >= 0 && line < pDesc->lines.size)
					{
						PX_VECTORAT(PX_Object_AsmDebugger_Line, &pDesc->lines, line)->bbreak = PX_TRUE;
					}
				}
			}
		}
	} 
}

px_void PX_Object_AsmDebuggerFree(PX_Object* pObject)
{
	px_int i;
	PX_Object_AsmDebugger *pDesc = PX_ObjectGetDesc(PX_Object_AsmDebugger, pObject);
	if (!pDesc->vm || !pDesc->map)
	{
		return;
	}
	for (i = 0; i < pDesc->lines.size; i++)
	{
		PX_StringFree(&PX_VECTORAT(PX_Object_AsmDebugger_Line,&pDesc->lines,i)->source);
	}
	PX_VectorFree(&pDesc->lines);
}


px_void PX_Object_AsmDebuggerOnMonitorChanged(PX_Object* pObject,PX_Object_Event e,px_void *ptr)
{
	px_int index = PX_Object_ListGetCurrentSelectIndex(pObject);
	PX_Object* pAsmObject=(PX_Object *)ptr;
	PX_Object_AsmDebugger* pAsm = PX_ObjectGetDesc(PX_Object_AsmDebugger, pAsmObject);
	if (!pAsm->vm || !pAsm->map)
	{
		return;
	}
	if (index!=-1)
	{
		PX_Object_AsmDebugger_VarMonitor* pm = PX_VECTORAT(PX_Object_AsmDebugger_VarMonitor, &pAsm->monitor, index);
		PX_Object_AsmDebuggerPrintVar(pAsm, pm->pvar, pm->name);
	}
}

px_void PX_Object_AsmDebuggerOnSourceChanged(PX_Object* pObject, PX_Object_Event e, px_void* ptr)
{
	px_int index = PX_Object_ListGetCurrentSelectIndex(pObject);
	PX_Object* pAsmObject = (PX_Object*)ptr;
	PX_Object_AsmDebugger* pAsm = PX_ObjectGetDesc(PX_Object_AsmDebugger, pAsmObject);
	px_char opcode, opcode_name[16];
	px_int paramcount;
	px_char optype[3];
	px_int param[3];
	px_int i,ip;
	if (!pAsm->vm || !pAsm->map)
	{
		return;
	}
	if ((ip = PX_VMDebuggerMapLineToIp(pAsm->map, index)) != -1)
	{
		if (PX_VMDebuggerInstruction(pAsm->vm, ip, &opcode, opcode_name, &paramcount, optype, param))
		{
			PX_Object_AsmDebugger_VarMonitor pvarMonitor;

			for (i = 0; i < paramcount; i++)
			{
				PX_VM_VARIABLE* pvar = PX_VMGetVariablePointer(pAsm->vm, optype[i], param[i]);
				px_char name[32];

				if (pvar)
				{
					pvarMonitor.pvar = pvar;
					if (PX_VMDebuggerGetVarParamName(pAsm->vm, optype[i], param[i], name, sizeof(name)))
					{
						PX_sprintf2(pvarMonitor.name, sizeof(pvarMonitor.name), "%1:%2", PX_STRINGFORMAT_INT(index + 1), PX_STRINGFORMAT_STRING(name));
						if (pAsm->monitor.size >= PX_OBJECT_ASMDEBUGGER_VARMONITOR_MAX_COUNT)
						{
							PX_VectorErase(&pAsm->monitor, 0);
						}
						PX_VectorPushback(&pAsm->monitor, &pvarMonitor);
						PX_Object_AsmDebuggerPrintVar(pAsm, pvar, name);
						
					}
				}

			}
			PX_Object_ListClear(pAsm->list_monitor);
			for (i = 0; i < pAsm->monitor.size; i++)
			{
				PX_Object_ListAdd(pAsm->list_monitor, PX_VECTORAT(PX_Object_AsmDebugger_VarMonitor, &pAsm->monitor, i));
			}
			PX_Object_ListMoveToBottom(pAsm->list_monitor);
		}
	}
	
}


px_void PX_Object_AsmDebuggerOnButtonRun(PX_Object* pObject, PX_Object_Event e, px_void* ptr)
{
	PX_Object* pAsmObject = (PX_Object*)ptr;
	PX_Object_AsmDebugger* pAsm = PX_ObjectGetDesc(PX_Object_AsmDebugger, pAsmObject);
	if (pAsm->vm && pAsm->map)
	{
		PX_VMDebugContinue(pAsm->vm);
	}
}

px_void PX_Object_AsmDebuggerOnButtonPause(PX_Object* pObject, PX_Object_Event e, px_void* ptr)
{
	PX_Object* pAsmObject = (PX_Object*)ptr;
	PX_Object_AsmDebugger* pAsm = PX_ObjectGetDesc(PX_Object_AsmDebugger, pAsmObject);
	if (pAsm->vm && pAsm->map)
	{
		PX_VMSuspend(pAsm->vm);
	}
}

px_void PX_Object_AsmDebuggerOnButtonBreak(PX_Object* pObject, PX_Object_Event e, px_void* ptr)
{
	PX_Object* pAsmObject = (PX_Object*)ptr;
	PX_Object_AsmDebugger* pAsm = PX_ObjectGetDesc(PX_Object_AsmDebugger, pAsmObject);
	px_int index = PX_Object_ListGetCurrentSelectIndex(pAsm->list_source);
	px_int ip;
	if (pAsm->vm && pAsm->map)
	{
		if ((ip = PX_VMDebuggerMapLineToIp(pAsm->map, index)) != -1)
		{
			PX_VMTriggerBreakPoint(pAsm->vm, ip);
		}
	}
}

px_void PX_Object_AsmDebuggerOnButtonStep(PX_Object* pObject, PX_Object_Event e, px_void* ptr)
{
	PX_Object* pAsmObject = (PX_Object*)ptr;
	PX_Object_AsmDebugger* pAsm = PX_ObjectGetDesc(PX_Object_AsmDebugger, pAsmObject);
	if (pAsm->vm && pAsm->map)
	{
		PX_VMDebugContinue(pAsm->vm);
		PX_VMRun(pAsm->vm, 1, 0);
		PX_VMSuspend(pAsm->vm);
	}
}

px_void PX_Object_AsmDebuggerOnButtonReset(PX_Object* pObject, PX_Object_Event e, px_void* ptr)
{
	
	PX_Object* pAsmObject = (PX_Object*)ptr;
	PX_Object_AsmDebugger* pAsm = PX_ObjectGetDesc(PX_Object_AsmDebugger, pAsmObject);
	if (pAsm->vm&&pAsm->map)
	{
		PX_VMReset(pAsm->vm);
		PX_VMBeginThreadFunction(pAsm->vm, 0, "main", 0, 0);
		PX_VMSuspend(pAsm->vm);
	}
	
}

px_void PX_Object_AsmDebuggerDetach(PX_Object* pObject)
{
	px_int i;
	if (pObject->Type==PX_OBJECT_TYPE_ASMDEBUGGER)
	{
		PX_Object_AsmDebugger* pDesc = PX_ObjectGetDesc(PX_Object_AsmDebugger, pObject);
		for (i=0;i<pDesc->lines.size;i++)
		{
			PX_Object_AsmDebugger_Line* pLine = PX_VECTORAT(PX_Object_AsmDebugger_Line,&pDesc->lines, i);
			PX_StringFree(&pLine->source);
		}
		PX_VectorClear(&pDesc->lines);
		PX_Object_ListClear(pDesc->list_source);
		pDesc->vm = 0;
		pDesc->map = 0;
		pDesc->lastip = -1;
	}
}

px_bool PX_Object_AsmDebuggerAttach(PX_Object *pObject,PX_VM_DebuggerMap* map,PX_VM* vm)
{
	PX_Object_AsmDebugger* pDesc = PX_ObjectGetDesc(PX_Object_AsmDebugger, pObject);
	px_int i, offset=0;

	if (pDesc->map)
	{
		PX_Object_AsmDebuggerDetach(pObject);
	}

	pDesc->vm = vm;
	pDesc->map = map;

	while (map->source.buffer[offset])
	{
		PX_Object_AsmDebugger_Line line;
		PX_memset(&line, 0, sizeof(line));

		PX_StringInitialize(pObject->mp, &line.source);
		while (PX_TRUE)
		{
			if (map->source.buffer[offset] == '\n' || map->source.buffer[offset] == '\0')
			{
				offset++;
				break;
			}
			else
			{
				PX_StringCatCharEx(&line.source, map->source.buffer[offset]);
			}
			offset++;
		}
		PX_VectorPushback(&pDesc->lines, &line);
	}



	for (i = 0; i < pDesc->lines.size; i++)
	{
		PX_Object_ListAdd(pDesc->list_source, PX_VECTORAT(PX_Object_AsmDebugger_Line, &pDesc->lines, i));
	}

	PX_VMSuspend(vm);

	return PX_TRUE;

}

PX_Object* PX_Object_AsmDebuggerCreate(px_memorypool* mp, PX_Object* Parent, px_int x, px_int y, px_int width, px_int height, PX_FontModule* fm)
{
	PX_Object_AsmDebugger Desc, * pDesc;
	PX_Object* pObject;
	PX_memset(&Desc, 0, sizeof(Desc));
	Desc.vm = 0;
	Desc.fm = fm;
	Desc.map = 0;
	Desc.lastip = -1;
	pObject = PX_ObjectCreateEx(mp, Parent,(px_float) x, (px_float)y, 0, (px_float)width, (px_float)height, 0, PX_OBJECT_TYPE_ASMDEBUGGER, 0, PX_Object_AsmDebuggerRender, PX_Object_AsmDebuggerFree, &Desc, sizeof(Desc));
	pDesc = PX_ObjectGetDesc(PX_Object_AsmDebugger, pObject);
	pDesc->button_break = PX_Object_PushButtonCreate(mp, pObject, 0, 0, 48, 24, "break", PX_NULL);
	PX_Object_PushButtonSetTextColor(pDesc->button_break, PX_COLOR(255, 255, 255, 0));
	pDesc->button_step = PX_Object_PushButtonCreate(mp, pObject, 0, 0, 48, 24, "step", PX_NULL);
	PX_Object_PushButtonSetTextColor(pDesc->button_step, PX_COLOR(255, 255, 0, 255));
	pDesc->button_run = PX_Object_PushButtonCreate(mp, pObject, 0, 0, 48, 24, "run", PX_NULL);
	PX_Object_PushButtonSetTextColor(pDesc->button_run, PX_COLOR(255, 0, 255, 0));
	pDesc->button_pause = PX_Object_PushButtonCreate(mp, pObject, 0, 0, 48, 24, "pause", PX_NULL);
	PX_Object_PushButtonSetTextColor(pDesc->button_pause, PX_COLOR(255, 0, 255, 255));
	pDesc->button_reset = PX_Object_PushButtonCreate(mp, pObject, 0, 0, 48, 24, "reset", PX_NULL);
	PX_Object_PushButtonSetTextColor(pDesc->button_reset, PX_COLOR(255, 255, 0, 0));
	pDesc->label_register = PX_Object_LabelCreate(mp, pObject, 0, 0, 256, 24, "", PX_NULL, PX_COLOR(255, 0, 255, 0));
	PX_ObjectRegisterEvent(pDesc->button_break, PX_OBJECT_EVENT_EXECUTE, PX_Object_AsmDebuggerOnButtonBreak, pObject);
	PX_ObjectRegisterEvent(pDesc->button_step, PX_OBJECT_EVENT_EXECUTE, PX_Object_AsmDebuggerOnButtonStep, pObject);
	PX_ObjectRegisterEvent(pDesc->button_run, PX_OBJECT_EVENT_EXECUTE, PX_Object_AsmDebuggerOnButtonRun, pObject);
	PX_ObjectRegisterEvent(pDesc->button_pause, PX_OBJECT_EVENT_EXECUTE, PX_Object_AsmDebuggerOnButtonPause, pObject);
	PX_ObjectRegisterEvent(pDesc->button_reset, PX_OBJECT_EVENT_EXECUTE, PX_Object_AsmDebuggerOnButtonReset, pObject);
	pDesc->list_source = PX_Object_ListCreate(mp, pObject, 0, 0, 10, 10, 18, PX_Object_AsmDebuggerSourceOnCreate, pObject);
	pDesc->autotext_data = PX_Object_AutoTextCreate(mp, pObject, 0, 0, 10, fm);
	pDesc->list_monitor= PX_Object_ListCreate(mp, pObject, 0, 0, 10, 10, 18, PX_Object_AsmDebuggerMonitorOnCreate, pObject);
	PX_ObjectRegisterEvent(pDesc->list_monitor, PX_OBJECT_EVENT_VALUECHANGED, PX_Object_AsmDebuggerOnMonitorChanged, pObject);
	PX_ObjectRegisterEvent(pDesc->list_source, PX_OBJECT_EVENT_VALUECHANGED, PX_Object_AsmDebuggerOnSourceChanged, pObject);
	pDesc->label_name = PX_Object_LabelCreate(mp, pObject, 0, 0, 128, 24, "", PX_NULL, PX_COLOR(255, 64, 255, 64));
	pDesc->autotext_data = PX_Object_AutoTextCreate(mp, pObject, 0, 0, 128,fm);

	PX_VectorInitialize(mp, &pDesc->lines, sizeof(PX_Object_AsmDebugger_Line), 1024);
	PX_VectorInitialize(mp, &pDesc->monitor, sizeof(PX_Object_AsmDebugger_VarMonitor), PX_OBJECT_ASMDEBUGGER_VARMONITOR_MAX_COUNT);

	pObject->Visible = PX_FALSE;
	return pObject;
}
