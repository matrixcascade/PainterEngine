#include "PX_Object_Debug.h"
#include "PX_Syntax_ir.h"


static px_int PX_Object_Debug_Request(PX_Object *pObject,px_abi *prequest_abi)
{
	
	PX_Object_Debug* pDesc = PX_ObjectGetDesc0(PX_Object_Debug, pObject);
	pDesc->alloc_request_id +=1+ PX_rand() % 0xff;
	pDesc->last_request_id = pDesc->alloc_request_id;
	pDesc->last_request_elapsed = 0;
	if (!PX_AbiSet_dword(prequest_abi,"id", pDesc->last_request_id))
	{
		return -1;
	}

	//set if
	do
	{
		const px_char* popcode = PX_AbiGet_string(prequest_abi, "opcode");
		if (popcode)
		{
			PX_printf("request---->%s:%d\n", popcode, pDesc->last_request_id);
		}
	} while (0);
	PX_ObjectExecuteEvent(pObject, PX_OBJECT_BUILD_EVENT_DATA(PX_OBJECT_DEBUG_EVENT_REQUEST, PX_AbiGet_Pointer(prequest_abi), PX_AbiGet_Size(prequest_abi)));
	return pDesc->last_request_id;
}


static px_void PX_Object_Debug_ToggleBreakpoint(PX_Object_Debug* pDesc, px_int source_index, px_int line)
{

}

PX_OBJECT_EVENT_FUNCTION(PX_Object_Debug_OnRun)
{
	PX_Object_Debug* pDesc = PX_ObjectGetDesc0(PX_Object_Debug, (PX_Object*)ptr);
	//TODO: implement run logic
}

PX_OBJECT_EVENT_FUNCTION(PX_Object_Debug_OnPause)
{
	PX_Object_Debug* pDesc = PX_ObjectGetDesc0(PX_Object_Debug, (PX_Object *)ptr);
	//TODO: implement pause logic
}

PX_OBJECT_EVENT_FUNCTION(PX_Object_Debug_OnStep)
{
	PX_Object_Debug* pDesc = PX_ObjectGetDesc0(PX_Object_Debug, (PX_Object *)ptr);
	px_int state= PX_FSM_GetCurrentState(&pDesc->fsm);
	if (state ==PX_OBJECT_DEBUG_STATE_PAUSE)
	{
		PX_Object_Code* pcode = PX_ObjectGetDesc0(PX_Object_Code, pDesc->source_code_viewer);
		if (pcode->bordercolor._argb.a)
		{
			pDesc->last_step_source_index = pDesc->current_view_source_index;
			pDesc->last_step_cursor_line = PX_Object_Code_GetCurrentCursorLine(pDesc->source_code_viewer);

			PX_FSM_SetState(&pDesc->fsm, PX_OBJECT_DEBUG_STATE_STEP_NEXT);
		}
		//pcode = PX_ObjectGetDesc0(PX_Object_Code, pDesc->ir_code_viewer);
		else //if (pcode->bordercolor._argb.a)
		{
			pDesc->last_step_ip = pDesc->reg.ip;
			PX_FSM_SetState(&pDesc->fsm, PX_OBJECT_DEBUG_STATE_STEP_IR_NEXT);
		}
	}
}

PX_OBJECT_EVENT_FUNCTION(PX_Object_Debug_OnStop)
{
	PX_Object_Debug* pDesc = PX_ObjectGetDesc0(PX_Object_Debug,  (PX_Object*)ptr);
	//TODO: implement stop logic
}

PX_OBJECT_EVENT_FUNCTION(PX_Object_Debug_OnReset)
{
	PX_Object_Debug* pDesc = PX_ObjectGetDesc0(PX_Object_Debug, (PX_Object*)ptr);
	PX_FSM_SetState(&pDesc->fsm, PX_OBJECT_DEBUG_STATE_RESET);
}

PX_OBJECT_EVENT_FUNCTION(PX_Object_Debug_OnTabButtonExecute)
{
	px_int i;
	PX_Object* pDebugObject = (PX_Object*)ptr;
	PX_Object_Debug* pDesc = PX_ObjectGetDesc0(PX_Object_Debug, pDebugObject);
	for (i = 0; i < pDesc->tab_buttons.size; i++)
	{
		PX_Object* pButton = *PX_VECTORAT(PX_Object*, &pDesc->tab_buttons, i);
		if (pButton == pObject)
		{
			PX_SyntaxLexer_Source* psrc;
			pDesc->current_view_source_index = i;
			psrc = PX_VECTORAT(PX_SyntaxLexer_Source, &pDesc->sources, i);
			PX_Object_Code_SetSource(pDesc->source_code_viewer, psrc);
			return;
		}
	}
}

PX_OBJECT_EVENT_FUNCTION(PX_Object_Debug_OnTreeExecute)
{
	PX_Object* pDebugObject = (PX_Object*)ptr;
	PX_Object_Debug* pDesc = PX_ObjectGetDesc0(PX_Object_Debug, pDebugObject);
	PX_Object_TreeNode* pNode =	PX_Object_TreeGetCurrentSelectNode(pDesc->tree_root_abi);
	if (pNode)
	{
		PX_Object_TextViewerSetText(pDesc->textviewer_out, pNode->content.buffer);
	}
	else
	{
		PX_Object_TextViewerSetText(pDesc->textviewer_out, "");
	}
}

static px_void PX_Object_Debug_ReorderTab(PX_Object* pObject)
{
	px_int i;
	PX_Object_Debug* pDesc = PX_ObjectGetDesc0(PX_Object_Debug, pObject);
	px_float x = 0;
	for (i = 0; i < pDesc->tab_buttons.size; i++)
	{
		PX_Object* pButton = *PX_VECTORAT(PX_Object*, &pDesc->tab_buttons, i);
		pButton->x = x;
		x += pButton->Width + 2;
	}
	PX_Object_ScrollAreaUpdateRange(pDesc->area_tab);
}

px_void  PX_Object_Debug_UpdateTabButtons(PX_Object* pObject)
{
	px_int i;
	PX_Object_Debug* pDesc = PX_ObjectGetDesc0(PX_Object_Debug, pObject);
	for (i = 0; i < pDesc->tab_buttons.size; i++)
	{
		PX_Object* pObject = *PX_VECTORAT(PX_Object*, &pDesc->tab_buttons, i);
		PX_ObjectDelete(pObject);
	}
	PX_VectorClear(&pDesc->tab_buttons);
	for (i = 0; i < pDesc->sources.size; i++)
	{
		px_int  w, h; PX_Object* pButtonObject;
		PX_SyntaxLexer_Source* psource = PX_VECTORAT(PX_SyntaxLexer_Source, &pDesc->sources, i);
		PX_FontModuleTextGetRenderWidthHeight(pDesc->fm, psource->name, &w, &h);
		pButtonObject = PX_Object_PushButtonCreate(pObject->mp, pDesc->area_tab, 0, 0, w + 10, 24, psource->name, pDesc->fm);
		PX_VectorPushback(&pDesc->tab_buttons, &pButtonObject);
		PX_ObjectRegisterEvent(pButtonObject, PX_OBJECT_EVENT_EXECUTE, PX_Object_Debug_OnTabButtonExecute, pObject);
	}
	PX_Object_Debug_ReorderTab(pObject);
}



PX_OBJECT_RENDER_FUNCTION(PX_Object_Debug_List_Item_Render)
{
	px_char* pcontent = (px_char*)PX_Object_ListItemGetData(pObject);
	PX_FontModuleDrawText(psurface, PX_NULL, (px_int)pObject->x+2, (px_int)pObject->y+2, PX_ALIGN_LEFTTOP, pcontent, PX_COLOR_BLACK);
}

PX_OBJECT_LIST_ITEM_CREATE_FUNCTION(PX_Object_Debug_ListCreate)
{
	PX_ObjectSetRenderFunction(ItemObject, PX_Object_Debug_List_Item_Render, 0);
	return PX_TRUE;
}




px_bool PX_Object_Debug_response_ok(PX_Object* pObject,const px_char *opcode)
{
	PX_Object_Debug* pDesc = PX_ObjectGetDesc0(PX_Object_Debug, pObject);
	PX_FSM* pfsm = &pDesc->fsm;
	if (pDesc->last_request_id && pDesc->last_request_id == pDesc->last_response_id)
	{
		const px_char* popcode, * preturn;
		pDesc->last_request_id = 0;
		pDesc->last_response_id = 0;
		popcode = PX_AbiGet_string(&pDesc->last_response_abi, "opcode");
		if (!popcode)
		{
			return PX_FALSE;
		}
		preturn = PX_AbiGet_string(&pDesc->last_response_abi, "return");
		if (!preturn)
		{
			return PX_FALSE;
		}
		if (!PX_strequ(preturn, "ok"))
		{
			return PX_FALSE;
		}
		else
		{
			return PX_TRUE;
		}
	}
	else
	{
		return PX_FALSE;
	}
}

static px_void PX_Object_Debug_UpdateSourceCodeViewer(PX_Object* pObject)
{
	PX_Object_Debug* pDesc = PX_ObjectGetDesc0(PX_Object_Debug, pObject);
	px_int found = -1;
	px_int source_index = -1;
	px_int source_line = -1;
	px_int map_count, i;
	PX_SyntaxLexer_Source* psrc;
	PX_Syntax_bin_map_to_source* pmap;
	px_dword ip;

	if (!pDesc) return;
	if (!pDesc->bin_map_to_source.buffer || pDesc->bin_map_to_source.usedsize < (px_int)sizeof(PX_Syntax_bin_map_to_source))
		return;
	pmap = (PX_Syntax_bin_map_to_source*)pDesc->bin_map_to_source.buffer;
	map_count = pDesc->bin_map_to_source.usedsize / (px_int)sizeof(PX_Syntax_bin_map_to_source);
	ip = pDesc->reg.ip;
	//find largest map entry whose ip <= reg.ip

	for (i = 0; i < map_count; i++)
	{
		if (pmap[i].ip <= ip)
		{
			found = i;
		}
		else
		{
			break;
		}
	}
	if (found == -1) return;
	source_index = (px_int)pmap[found].source_index;
	if (!PX_VectorCheckIndex(&pDesc->sources, source_index))
	{
		source_index = -1;
		return;
	}

	psrc = PX_VECTORAT(PX_SyntaxLexer_Source, &pDesc->sources, source_index);

	if ((px_int)pmap[found].source_line < psrc->line_count)
		source_line = pmap[found].source_line;
	else
		source_line = -1;

	if (source_index != pDesc->current_view_source_index || source_line != PX_Object_Code_GetCurrentCursorLine(pDesc->source_code_viewer))
	{
		if (source_index != pDesc->current_view_source_index)
		{
			pDesc->current_view_source_index = source_index;
			PX_Object_Code_SetSource(pDesc->source_code_viewer, psrc);
		}
		PX_Object_Code_SetCursorLineAndView(pDesc->source_code_viewer, source_line);
	}

}


static px_void PX_Object_Debug_UpdateIRCodeViewer(PX_Object* pObject)
{
	PX_Object_Debug* pDesc = PX_ObjectGetDesc0(PX_Object_Debug, pObject);
	px_int found = -1;
	px_int source_index = -1;
	px_int source_line = -1;
	px_int map_count, i;
	PX_SyntaxLexer_Source* psrc;
	PX_Syntax_bin_map_to_ir* pmap;
	px_dword ip;

	if (!pDesc) return;
	if (!pDesc->bin_map_to_ir.buffer || pDesc->bin_map_to_ir.usedsize < (px_int)sizeof(PX_Syntax_bin_map_to_ir))
		return;
	pmap = (PX_Syntax_bin_map_to_ir*)pDesc->bin_map_to_ir.buffer;
	map_count = pDesc->bin_map_to_ir.usedsize / (px_int)sizeof(PX_Syntax_bin_map_to_ir);
	ip = pDesc->reg.ip;
	//find largest map entry whose ip <= reg.ip

	for (i = 0; i < map_count; i++)
	{
		if (pmap[i].ip <= ip)
		{
			found = i;
		}
		else
		{
			break;
		}
	}
	if (found == -1) return;
	source_index = (px_int)pmap[found].source_index;
	if (!PX_VectorCheckIndex(&pDesc->ir_sources, source_index))
	{
		source_index = -1;
		return;
	}

	psrc = PX_VECTORAT(PX_SyntaxLexer_Source, &pDesc->ir_sources, source_index);

	if ((px_int)pmap[found].source_line < psrc->line_count)
		source_line = pmap[found].source_line;
	else
		source_line = -1;

	if (source_index != pDesc->current_view_source_index || source_line != PX_Object_Code_GetCurrentCursorLine(pDesc->ir_code_viewer))
	{
		if (source_index != pDesc->current_view_source_index)
		{
			pDesc->current_view_source_index = source_index;
			PX_Object_Code_SetSource(pDesc->ir_code_viewer, psrc);
		}
		PX_Object_Code_SetCursorLineAndView(pDesc->ir_code_viewer, source_line);
	}
}


static px_void PX_Object_Debug_UpdateRegisters(PX_Object* pObject)
{
	PX_Object_Debug* pDesc = PX_ObjectGetDesc0(PX_Object_Debug, pObject);
	px_int ri;
	px_char hexbuf[16];
	px_dword val;
	const px_char* names[] = { "R0","R1","R2","R3","R4(IP)","R5(SP)","R6(BP)","R7(FLAG)" };
	PX_Object_Debug_UpdateSourceCodeViewer(pObject);
	PX_Object_Debug_UpdateIRCodeViewer(pObject);
	for (ri = 0; ri < 8; ri++)
	{
		px_int hi;
		val = pDesc->reg.r[ri];
		PX_strcpy(hexbuf, "00000000", 9);
		for (hi = 7; hi >= 0; hi--)
		{
			px_dword nibble = val & 0xF;
			hexbuf[hi] = (px_char)(nibble < 10 ? '0' + nibble : 'a' + nibble - 10);
			val >>= 4;
		}
		if (ri == 7)
		{
			PX_sprintf3(pDesc->list_info_content[ri], 64, "%1 0x%2 %3",
				PX_STRINGFORMAT_STRING(names[ri]),
				PX_STRINGFORMAT_STRING(hexbuf),
				PX_STRINGFORMAT_STRING(
					(pDesc->reg.Z ? "Z1:" : "Z0:")
				));
			PX_strcat(pDesc->list_info_content[ri], pDesc->reg.N ? "N1:" : "N0:");
			PX_strcat(pDesc->list_info_content[ri], pDesc->reg.C ? "C1:" : "C0:");
			PX_strcat(pDesc->list_info_content[ri], pDesc->reg.V ? "V1" : "V0");
		}
		else
		{
			PX_sprintf3(pDesc->list_info_content[ri], 64, "%1 0x%2 %3",
				PX_STRINGFORMAT_STRING(names[ri]),
				PX_STRINGFORMAT_STRING(hexbuf),
				PX_STRINGFORMAT_INT((px_int)pDesc->reg.r[ri]));
		}
	}
	for (ri = 0; ri < 4; ri++)
	{
		px_int hi;
		px_dword fraw;
		PX_memcpy(&fraw, &pDesc->reg.f[ri], sizeof(px_dword));
		val = fraw;
		PX_strcpy(hexbuf, "00000000", 9);
		for (hi = 7; hi >= 0; hi--)
		{
			px_dword nibble = val & 0xF;
			hexbuf[hi] = (px_char)(nibble < 10 ? '0' + nibble : 'a' + nibble - 10);
			val >>= 4;
		}
		PX_sprintf3(pDesc->list_info_content[8 + ri], 64, "F%1 0x%2 %3",
			PX_STRINGFORMAT_INT(ri),
			PX_STRINGFORMAT_STRING(hexbuf),
			PX_STRINGFORMAT_FLOAT(pDesc->reg.f[ri]));
	}
}
static px_void PX_Object_Debug_HandleGetStateResponse(PX_Object* pObject)
{
	px_dword* pr, * pf, * pbp;
	px_dword size;
	PX_Object_Debug* pDesc = PX_ObjectGetDesc0(PX_Object_Debug, pObject);
	PX_FSM* pfsm = &pDesc->fsm;
	const px_char* pstate = PX_AbiGet_string(&pDesc->last_response_abi, "state");
	PX_strset(pDesc->vm_state, "Machine:");
	PX_strcat_s(pDesc->vm_state,sizeof(pDesc->vm_state), pstate);
	//update registers
	pr = PX_AbiGet_data(&pDesc->last_response_abi, "r", &size);
	if (pr && size == sizeof(px_dword) * 12)
	{
		PX_memcpy(pDesc->reg.r, pr, sizeof(px_dword) * 12);
	}
	else
	{
		PX_printf("Failed to get registers from response ABI.\n");
		PX_FSM_SetState(pfsm, PX_OBJECT_DEBUG_STATE_ERROR);
	}

	pf = PX_AbiGet_data(&pDesc->last_response_abi, "f", &size);
	if (pf && size == sizeof(px_float32) * 4)
	{
		PX_memcpy(pDesc->reg.f, pf, sizeof(px_float32) * 4);
	}
	else
	{
		PX_printf("Failed to get floating-point registers from response ABI.\n");
		PX_FSM_SetState(pfsm, PX_OBJECT_DEBUG_STATE_ERROR);
	}

	pbp = PX_AbiGet_data(&pDesc->last_response_abi, "bp", &size);
	if (pbp && size == sizeof(px_dword) * 32)
	{
		PX_memcpy(pDesc->ip_breakpoint, pbp, sizeof(px_dword) * 32);
	}
	else
	{
		PX_printf("Failed to get breakpoints from response ABI.\n");
		PX_FSM_SetState(pfsm, PX_OBJECT_DEBUG_STATE_ERROR);
	}

	PX_Object_Debug_UpdateRegisters(pObject);
}

PX_FSM_UPDATE_FUNCTION(PX_Object_Debug_STATE_DISCONNECT)
{
	//do nothing
}




PX_FSM_UPDATE_FUNCTION(PX_Object_Debug_STATE_RESET)
{
	//query statepx_abi load_abi;
	px_abi request_abi;
	PX_Object* pObject = (PX_Object*)userptr;
	PX_Object_Debug* pDesc = PX_ObjectGetDesc0(PX_Object_Debug, pObject);
	PX_AbiCreate_DynamicWriter(&request_abi, pDesc->mp);
	pDesc->last_request_id++;
	if (!PX_AbiSet_string(&request_abi, "opcode", "reset"))
	{
		PX_AbiFree(&request_abi);
		return;
	}
	if (PX_Object_Debug_Request(pObject, &request_abi) == -1)
	{
		PX_FSM_SetState(pfsm, PX_OBJECT_DEBUG_STATE_ERROR);
		PX_AbiFree(&request_abi);
		return;
	}
	PX_AbiFree(&request_abi);
	PX_FSM_SetState(pfsm, PX_OBJECT_DEBUG_STATE_RESET_WAIT);
}

PX_FSM_UPDATE_FUNCTION(PX_Object_Debug_STATE_RESET_WAIT)
{
	PX_Object* pObject = (PX_Object*)userptr;
	PX_Object_Debug* pDesc = PX_ObjectGetDesc0(PX_Object_Debug, pObject);
	if (PX_Object_Debug_response_ok(pObject, "reset"))
	{
		PX_FSM_SetState(pfsm, PX_OBJECT_DEBUG_STATE_UPLOADING);
		return;
	}
	else
	{
		if (pDesc->last_request_elapsed >= PX_OBJECT_DEBUG_REQUEST_TIMEOUT)
		{
			PX_FSM_SetState(pfsm, PX_OBJECT_DEBUG_STATE_ERROR);
			return;
		}
	}
}

PX_FSM_UPDATE_FUNCTION(PX_Object_Debug_STATE_UPLOADING)
{
	px_abi request_abi;
	PX_Object* pObject = (PX_Object*)userptr;
	PX_Object_Debug* pDesc = PX_ObjectGetDesc0(PX_Object_Debug, pObject);
	PX_AbiCreate_DynamicWriter(&request_abi, pDesc->mp);
	pDesc->last_request_id++;
	if (!PX_AbiSet_string(&request_abi, "opcode", "load"))
	{
		PX_AbiFree(&request_abi);
		return;
	}
	if (!PX_AbiSet_Abi(&request_abi, "bin", &pDesc->bin_packet_abi))
	{
		PX_AbiFree(&request_abi);
		return;
	}
	PX_Object_Debug_Request(pObject, &request_abi);
	PX_AbiFree(&request_abi);
	PX_FSM_SetState(pfsm, PX_OBJECT_DEBUG_STATE_UPLOADING_WAIT);
}

PX_FSM_UPDATE_FUNCTION(PX_Object_Debug_STATE_UPLOADING_WAIT)
{
	PX_Object* pObject = (PX_Object*)userptr;
	PX_Object_Debug* pDesc = PX_ObjectGetDesc0(PX_Object_Debug, pObject);
	if (PX_Object_Debug_response_ok(pObject, "load"))
	{
		PX_FSM_SetState(pfsm, PX_OBJECT_DEBUG_STATE_QUERY_STATE);
		return;
	}
	else
	{
		if (pDesc->last_request_elapsed >= PX_OBJECT_DEBUG_REQUEST_TIMEOUT)
		{
			PX_FSM_SetState(pfsm, PX_OBJECT_DEBUG_STATE_ERROR);
			return;
		}
	}
}

PX_FSM_UPDATE_FUNCTION(PX_Object_Debug_STATE_QUERY_STATE)
{
	//query state px_abi;
	px_abi request_abi;
	PX_Object* pObject = (PX_Object*)userptr;
	PX_Object_Debug* pDesc = PX_ObjectGetDesc0(PX_Object_Debug, pObject);
	PX_AbiCreate_DynamicWriter(&request_abi, pDesc->mp);
	pDesc->last_request_id++;
	if (!PX_AbiSet_string(&request_abi, "opcode", "get_state"))
	{
		PX_AbiFree(&request_abi);
		return;
	}
	if (PX_Object_Debug_Request(pObject, &request_abi) == -1)
	{
		PX_FSM_SetState(pfsm, PX_OBJECT_DEBUG_STATE_ERROR);
		PX_AbiFree(&request_abi);
		return;
	}
	PX_AbiFree(&request_abi);
	PX_FSM_SetState(pfsm, PX_OBJECT_DEBUG_STATE_QUERY_STATE_WAIT);
}

PX_FSM_UPDATE_FUNCTION(PX_Object_Debug_STATE_QUERY_STATE_WAIT)
{
	PX_Object* pObject = (PX_Object*)userptr;
	PX_Object_Debug* pDesc = PX_ObjectGetDesc0(PX_Object_Debug, pObject);
	if (PX_Object_Debug_response_ok(pObject, "get_state"))
	{
		const px_char *pstate = PX_AbiGet_string(&pDesc->last_response_abi, "state");
		if (PX_strequ(pstate, "idle"))
			PX_FSM_SetState(pfsm, PX_OBJECT_DEBUG_STATE_UPLOADING);
		else if (PX_strequ(pstate, "pause"))
			PX_FSM_SetState(pfsm, PX_OBJECT_DEBUG_STATE_PAUSE);
		else if (PX_strequ(pstate, "running"))
			PX_FSM_SetState(pfsm, PX_OBJECT_DEBUG_STATE_RUNNING);
		else if (PX_strequ(pstate, "error"))
			PX_FSM_SetState(pfsm, PX_OBJECT_DEBUG_STATE_ERROR);
		else
			PX_FSM_SetState(pfsm, PX_OBJECT_DEBUG_STATE_ERROR);

		PX_Object_Debug_HandleGetStateResponse(pObject);
	}
	else
	{
		if (pDesc->last_request_elapsed >= PX_OBJECT_DEBUG_REQUEST_TIMEOUT)
		{
			PX_FSM_SetState(pfsm, PX_OBJECT_DEBUG_STATE_ERROR);
			return;
		}
	}
}

PX_FSM_UPDATE_FUNCTION(PX_Object_Debug_STATE_PAUSE)
{
	return;
}

PX_FSM_UPDATE_FUNCTION(PX_Object_Debug_STATE_STEP_NEXT)
{
	PX_Object* pObject = (PX_Object*)userptr;
	PX_Object_Debug* pDesc = PX_ObjectGetDesc0(PX_Object_Debug, pObject);
	//query state px_abi;
	px_abi request_abi;
	PX_AbiCreate_DynamicWriter(&request_abi, pDesc->mp);
	pDesc->last_request_id++;
	if (!PX_AbiSet_string(&request_abi, "opcode", "step"))
	{
		PX_AbiFree(&request_abi);
		return;
	}
	if (!PX_AbiSet_dword(&request_abi, "tick", 1))
	{
		PX_AbiFree(&request_abi);
		return;
	}
	if (PX_Object_Debug_Request(pObject, &request_abi) == -1)
	{
		PX_FSM_SetState(pfsm, PX_OBJECT_DEBUG_STATE_ERROR);
		PX_AbiFree(&request_abi);
		return;
	}
	PX_AbiFree(&request_abi);
	PX_FSM_SetState(pfsm, PX_OBJECT_DEBUG_STATE_STEP_NEXT_WAIT);
}

PX_FSM_UPDATE_FUNCTION(PX_Object_Debug_STATE_STEP_NEXT_WAIT)
{
	PX_Object* pObject = (PX_Object*)userptr;
	PX_Object_Debug* pDesc = PX_ObjectGetDesc0(PX_Object_Debug, pObject);
	if (PX_Object_Debug_response_ok(pObject, "step"))
	{
		PX_FSM_SetState(pfsm, PX_OBJECT_DEBUG_STATE_STEP_QUERY_STATE);
		return;
	}
	else
	{
		if (pDesc->last_request_elapsed >= PX_OBJECT_DEBUG_REQUEST_TIMEOUT)
		{
			PX_FSM_SetState(pfsm, PX_OBJECT_DEBUG_STATE_ERROR);
			return;
		}
	}
}

PX_FSM_UPDATE_FUNCTION(PX_Object_Debug_STATE_STEP_NEXT_QUERY_STATE)
{
	//query state px_abi;
	px_abi request_abi;
	PX_Object* pObject = (PX_Object*)userptr;
	PX_Object_Debug* pDesc = PX_ObjectGetDesc0(PX_Object_Debug, pObject);
	PX_AbiCreate_DynamicWriter(&request_abi, pDesc->mp);
	pDesc->last_request_id++;
	if (!PX_AbiSet_string(&request_abi, "opcode", "get_state"))
	{
		PX_AbiFree(&request_abi);
		return;
	}
	if (PX_Object_Debug_Request(pObject, &request_abi) == -1)
	{
		PX_FSM_SetState(pfsm, PX_OBJECT_DEBUG_STATE_ERROR);
		PX_AbiFree(&request_abi);
		return;
	}
	PX_AbiFree(&request_abi);
	PX_FSM_SetState(pfsm, PX_OBJECT_DEBUG_STATE_STEP_QUERY_STATE_WAIT);
}

PX_FSM_UPDATE_FUNCTION(PX_Object_Debug_STATE_STEP_NEXT_QUERY_STATE_WAIT)
{
	PX_Object* pObject = (PX_Object*)userptr;
	PX_Object_Debug* pDesc = PX_ObjectGetDesc0(PX_Object_Debug, pObject);
	if (PX_Object_Debug_response_ok(pObject, "get_state"))
	{
		PX_Object_Debug_HandleGetStateResponse(pObject);
		if (pDesc->last_step_cursor_line!=PX_Object_Code_GetCurrentCursorLine(pDesc->source_code_viewer))
		{
			PX_FSM_SetState(pfsm, PX_OBJECT_DEBUG_STATE_PAUSE);
		}
		else
		{
			PX_FSM_SetState(pfsm, PX_OBJECT_DEBUG_STATE_STEP_NEXT);
		}
		return;
	}
	else
	{
		if (pDesc->last_request_elapsed >= PX_OBJECT_DEBUG_REQUEST_TIMEOUT)
		{
			PX_FSM_SetState(pfsm, PX_OBJECT_DEBUG_STATE_ERROR);
			return;
		}
	}
}

//
PX_FSM_UPDATE_FUNCTION(PX_Object_Debug_STATE_STEP_IR_NEXT)
{
	PX_Object* pObject = (PX_Object*)userptr;
	PX_Object_Debug* pDesc = PX_ObjectGetDesc0(PX_Object_Debug, pObject);
	//query state px_abi;
	px_abi request_abi;
	PX_AbiCreate_DynamicWriter(&request_abi, pDesc->mp);
	pDesc->last_request_id++;
	if (!PX_AbiSet_string(&request_abi, "opcode", "step"))
	{
		PX_AbiFree(&request_abi);
		return;
	}
	if (!PX_AbiSet_dword(&request_abi, "tick", 1))
	{
		PX_AbiFree(&request_abi);
		return;
	}
	if (PX_Object_Debug_Request(pObject, &request_abi) == -1)
	{
		PX_FSM_SetState(pfsm, PX_OBJECT_DEBUG_STATE_ERROR);
		PX_AbiFree(&request_abi);
		return;
	}
	PX_AbiFree(&request_abi);
	PX_FSM_SetState(pfsm, PX_OBJECT_DEBUG_STATE_STEP_IR_NEXT_WAIT);
}

PX_FSM_UPDATE_FUNCTION(PX_Object_Debug_STATE_STEP_IR_NEXT_WAIT)
{
	PX_Object* pObject = (PX_Object*)userptr;
	PX_Object_Debug* pDesc = PX_ObjectGetDesc0(PX_Object_Debug, pObject);
	if (PX_Object_Debug_response_ok(pObject, "step"))
	{
		PX_FSM_SetState(pfsm, PX_OBJECT_DEBUG_STATE_STEP_IR_QUERY_STATE);
		return;
	}
	else
	{
		if (pDesc->last_request_elapsed >= PX_OBJECT_DEBUG_REQUEST_TIMEOUT)
		{
			PX_FSM_SetState(pfsm, PX_OBJECT_DEBUG_STATE_ERROR);
			return;
		}
	}
}

PX_FSM_UPDATE_FUNCTION(PX_Object_Debug_STATE_STEP_IR_QUERY_STATE)
{
	//query state px_abi;
	px_abi request_abi;
	PX_Object* pObject = (PX_Object*)userptr;
	PX_Object_Debug* pDesc = PX_ObjectGetDesc0(PX_Object_Debug, pObject);
	PX_AbiCreate_DynamicWriter(&request_abi, pDesc->mp);
	pDesc->last_request_id++;
	if (!PX_AbiSet_string(&request_abi, "opcode", "get_state"))
	{
		PX_AbiFree(&request_abi);
		return;
	}
	if (PX_Object_Debug_Request(pObject, &request_abi) == -1)
	{
		PX_FSM_SetState(pfsm, PX_OBJECT_DEBUG_STATE_ERROR);
		PX_AbiFree(&request_abi);
		return;
	}
	PX_AbiFree(&request_abi);
	PX_FSM_SetState(pfsm, PX_OBJECT_DEBUG_STATE_STEP_IR_QUERY_STATE_WAIT);
}

PX_FSM_UPDATE_FUNCTION(PX_Object_Debug_STATE_STEP_IR_QUERY_STATE_WAIT)
{
	PX_Object* pObject = (PX_Object*)userptr;
	PX_Object_Debug* pDesc = PX_ObjectGetDesc0(PX_Object_Debug, pObject);
	if (PX_Object_Debug_response_ok(pObject, "get_state"))
	{
		PX_Object_Debug_HandleGetStateResponse(pObject);
		PX_FSM_SetState(pfsm, PX_OBJECT_DEBUG_STATE_PAUSE);
		return;
	}
	else
	{
		if (pDesc->last_request_elapsed >= PX_OBJECT_DEBUG_REQUEST_TIMEOUT)
		{
			PX_FSM_SetState(pfsm, PX_OBJECT_DEBUG_STATE_ERROR);
			return;
		}
	}
}
//

PX_FSM_UPDATE_FUNCTION(PX_Object_Debug_STATE_RUNNING)
{
	PX_Object* pObject = (PX_Object*)userptr;
	PX_Object_Debug* pDesc = PX_ObjectGetDesc0(PX_Object_Debug, pObject);
	pDesc->fsm_running_update_elapsed += elapsed;
	if (pDesc->fsm_running_update_elapsed >= 500)
	{
		pDesc->fsm_running_update_elapsed = 0;
		PX_FSM_SetState(pfsm, PX_OBJECT_DEBUG_STATE_QUERY_STATE);
	}
	return;
}

PX_FSM_UPDATE_FUNCTION(PX_Object_Debug_STATE_ERROR)
{
	//do nothing, wait for user command or state change
	return;
}

PX_OBJECT_UPDATE_FUNCTION(PX_Object_Debug_Update)
{
	PX_Object_Debug* pDesc = PX_ObjectGetDesc0(PX_Object_Debug, pObject);
	if (!pObject->Visible)
	{
		return;
	}
	pDesc->last_request_elapsed += elapsed;
	PX_FSM_Update(&pDesc->fsm, elapsed);
}




px_void PX_Object_Debug_Response(PX_Object* pObject, const px_byte payload[], px_int size)
{
	PX_Object_Debug* pDesc = PX_ObjectGetDesc0(PX_Object_Debug, pObject);
	if (size>0)
	{
		px_abi resp_abi;
		const px_char* popcode;
		px_dword* pid;
		if (!pDesc) return;
		PX_AbiCreate_StaticReader(&resp_abi, payload, size);
		if (!PX_AbiCheck(&resp_abi)) return;
		popcode = PX_AbiGet_string(&resp_abi, "opcode");
		if (!popcode) return;
		pid = PX_AbiGet_dword(&resp_abi, "id");
		if (!pid) return;

		PX_printf("response<----%s:%d\n", popcode,*pid);
		if (pDesc->last_request_id != *pid) return;
		PX_AbiClear(&pDesc->last_response_abi);
		if (PX_AbiCopy_FromBuffer(&pDesc->last_response_abi, payload, size))
		{
			pDesc->last_response_id = pDesc->last_request_id;
		}
		pDesc->last_request_elapsed = 0;
		PX_FSM_ExecuteEvent(&pDesc->fsm, PX_FSM_BUILD_EVENT(PX_OBJECT_DEBUG_FSM_EVENT_RESPONSE));
	}
	else
	{
		PX_FSM_ExecuteEvent(&pDesc->fsm, PX_FSM_BUILD_EVENT(PX_OBJECT_DEBUG_FSM_EVENT_DISCONNECT));
	}

}

PX_OBJECT_RENDER_FUNCTION(PX_Object_Debug_Render)
{
	px_rect region = PX_ObjectGetRect(pObject);
	PX_Object_Debug* pDesc = PX_ObjectGetDesc(PX_Object_Debug, pObject);
	px_int content_ir_panel_width= (px_int)region.width * 3 / 4 - 20;
	px_int content_panel_width, content_panel_height;

	content_panel_width = content_ir_panel_width * 5 / 8;
	content_panel_height = ((px_int)region.height - 32) * 3 / 4 - 20;
	if (content_panel_height<100)
	{
		content_panel_height = 100;
	}

	if (pObject->Width<100)
	{
		pObject->Width = 100;
	}
	if (pObject->Height < 100)
	{
		pObject->Height = 100;
	}

	pDesc->area_tab->x = 0;
	pDesc->area_tab->y = 0;
	pDesc->area_tab->Width = region.width/2;
	pDesc->area_tab->Height = 32;
	pDesc->controller_panel->x = pDesc->area_tab->Width+32;
	pDesc->controller_panel->y = 0;

	pDesc->list_info->x = 0;
	pDesc->list_info->y = pDesc->area_tab->Height;
	pDesc->list_info->Width = region.width / 4;
	pDesc->list_info->Height = (region.height - (px_int)pDesc->area_tab->Height)/2;

	pDesc->tree_root_abi->x = 0;
	pDesc->tree_root_abi->y = pDesc->area_tab->Height + pDesc->list_info->Height;
	pDesc->tree_root_abi->Width = region.width / 4;
	pDesc->tree_root_abi->Height = (region.height - (px_int)pDesc->area_tab->Height) / 2;

	pDesc->source_code_viewer->x = pDesc->tree_root_abi->Width;
	pDesc->source_code_viewer->y = pDesc->area_tab->Height;
	pDesc->source_code_viewer->Width = (px_float)content_panel_width + 20;
	pDesc->source_code_viewer->Height = (px_float)content_panel_height + 20;

	pDesc->textviewer_out->x = pDesc->tree_root_abi->Width + content_panel_width + 20.f;
	pDesc->textviewer_out->y = pDesc->list_info->y;
	pDesc->textviewer_out->Width = region.width - content_panel_width - pDesc->tree_root_abi->Width - 20;
	pDesc->textviewer_out->Height = content_panel_height+20.f;


	pDesc->ir_code_viewer->x = pDesc->textviewer_out->x;
	pDesc->ir_code_viewer->y = pDesc->textviewer_out->y;
	pDesc->ir_code_viewer->Width = pDesc->textviewer_out->Width;
	pDesc->ir_code_viewer->Height = pDesc->textviewer_out->Height;

	pDesc->printer->Width = region.width - pDesc->tree_root_abi->Width;
	pDesc->printer->Height = (region.height - 32) / 4;
	pDesc->printer->x = pDesc->tree_root_abi->Width;
	pDesc->printer->y = pDesc->area_tab->Height + content_panel_height + 40;

	switch (PX_FSM_GetCurrentState(&pDesc->fsm))
	{
	case PX_OBJECT_DEBUG_STATE_DISCONNECT:
		PX_Object_LabelSetText(pDesc->debugger_state_label, "State: disconnect");
		break;
	case PX_OBJECT_DEBUG_STATE_RESET:
		PX_Object_LabelSetText(pDesc->debugger_state_label, "State: reset");
		break;
	case PX_OBJECT_DEBUG_STATE_RESET_WAIT:
		PX_Object_LabelSetText(pDesc->debugger_state_label, "State: reset wait");
		break;
	case PX_OBJECT_DEBUG_STATE_UPLOADING:
		PX_Object_LabelSetText(pDesc->debugger_state_label, "State: uploading");
		break;
	case PX_OBJECT_DEBUG_STATE_UPLOADING_WAIT:
		PX_Object_LabelSetText(pDesc->debugger_state_label, "State: uploading wait");
		break;
	case PX_OBJECT_DEBUG_STATE_QUERY_STATE:
		PX_Object_LabelSetText(pDesc->debugger_state_label, "State: query state");
		break;
	case PX_OBJECT_DEBUG_STATE_QUERY_STATE_WAIT:
		PX_Object_LabelSetText(pDesc->debugger_state_label, "State: query state wait");
		break;
	case PX_OBJECT_DEBUG_STATE_PAUSE:
		PX_Object_LabelSetText(pDesc->debugger_state_label, "State: pause");
		break;
	case PX_OBJECT_DEBUG_STATE_STEP_NEXT:
		PX_Object_LabelSetText(pDesc->debugger_state_label, "State: step next");
		break;
	case PX_OBJECT_DEBUG_STATE_STEP_NEXT_WAIT:
		PX_Object_LabelSetText(pDesc->debugger_state_label, "State: step next wait");
		break;
	case PX_OBJECT_DEBUG_STATE_STEP_QUERY_STATE:
		PX_Object_LabelSetText(pDesc->debugger_state_label, "State: step query state");
		break;
	case PX_OBJECT_DEBUG_STATE_STEP_QUERY_STATE_WAIT:
		PX_Object_LabelSetText(pDesc->debugger_state_label, "State: step query state wait");
		break;
	case PX_OBJECT_DEBUG_STATE_STEP_IR_NEXT:
		PX_Object_LabelSetText(pDesc->debugger_state_label, "State: step IR next");
		break;
	case PX_OBJECT_DEBUG_STATE_STEP_IR_NEXT_WAIT:
		PX_Object_LabelSetText(pDesc->debugger_state_label, "State: step IR next wait");
		break;
	case PX_OBJECT_DEBUG_STATE_STEP_IR_QUERY_STATE:
		PX_Object_LabelSetText(pDesc->debugger_state_label, "State: step IR query state");
		break;
	case PX_OBJECT_DEBUG_STATE_STEP_IR_QUERY_STATE_WAIT:
		PX_Object_LabelSetText(pDesc->debugger_state_label, "State: step IR query state wait");
		break;
	case PX_OBJECT_DEBUG_STATE_RUNNING:
		PX_Object_LabelSetText(pDesc->debugger_state_label, "State: running");
		break;
	case PX_OBJECT_DEBUG_STATE_ERROR:
		PX_Object_LabelSetText(pDesc->debugger_state_label, "State: error");
		break;
	default:
		PX_Object_LabelSetText(pDesc->debugger_state_label, "State: unknown");
		break;
	}

	PX_Object_LabelSetText(pDesc->vm_state_label, pDesc->vm_state);
}



static px_void PX_Object_Debug_FreeSource(px_memorypool* mp, PX_SyntaxLexer_Source* psrc)
{
	px_int j;
	if (psrc->name) MP_Free(mp, psrc->name);
	if (psrc->source) MP_Free(mp, psrc->source);
	if (psrc->source_index_map_to_line_index) MP_Free(mp, psrc->source_index_map_to_line_index);
	if (psrc->cells) MP_Free(mp, psrc->cells);
	if (psrc->source_index_map_to_cell_index) MP_Free(mp, psrc->source_index_map_to_cell_index);
	if (psrc->line_begin_cell_index_map) MP_Free(mp, psrc->line_begin_cell_index_map);
	for (j = 0; j < PX_VectorSize(&psrc->descriptor); j++)
	{
		PX_AbiFree(PX_VECTORAT(px_abi, &psrc->descriptor, j));
	}
	PX_VectorFree(&psrc->descriptor);
}

px_void PX_Object_Debug_Clear(PX_Object* pObject)
{
	PX_Object_Debug* pDesc = PX_ObjectGetDesc0(PX_Object_Debug, pObject);
	px_int i;
	if (!pDesc)
	{
		return;
	}
	pDesc->current_view_source_index = -1;
	PX_Object_Code_SetSource(pDesc->source_code_viewer, PX_NULL);

	PX_Object_TreeClear(pDesc->tree_root_abi);
	PX_Object_ListClear(pDesc->list_info);
	for (i = 0; i < pDesc->list_contents.size; i++)
	{
		px_char* p = *PX_VECTORAT(px_char*, &pDesc->list_contents, i);
		MP_Free(pDesc->mp, p);
	}
	PX_VectorClear(&pDesc->list_contents);

	for (i = 0; i < PX_VectorSize(&pDesc->sources); i++)
	{
		PX_Object_Debug_FreeSource(pDesc->mp, PX_VECTORAT(PX_SyntaxLexer_Source, &pDesc->sources, i));
	}
	PX_VectorClear(&pDesc->sources);
	for (i = 0; i < PX_VectorSize(&pDesc->ir_sources); i++)
	{
		PX_Object_Debug_FreeSource(pDesc->mp, PX_VECTORAT(PX_SyntaxLexer_Source, &pDesc->ir_sources, i));
	}
	PX_VectorClear(&pDesc->ir_sources);
	PX_MemoryFree(&pDesc->bin_map_to_source);
	PX_MemoryFree(&pDesc->bin_map_to_ir);
	PX_AbiFree(&pDesc->bin_packet_abi);
	PX_memset(pDesc->ip_breakpoint, 0xFF, sizeof(pDesc->ip_breakpoint));
	PX_memset(pDesc->breakpoints, 0, sizeof(pDesc->breakpoints));
	PX_VectorClear(&pDesc->pages_memory);
}

PX_OBJECT_FREE_FUNCTION(PX_Object_Debug_Free)
{
	PX_Object_Debug* pDesc = PX_ObjectGetDesc0(PX_Object_Debug, pObject);
	if (pDesc)
	{
		PX_Object_Debug_Clear(pObject);
		PX_VectorFree(&pDesc->tab_buttons);
		PX_VectorFree(&pDesc->list_contents);
		PX_VectorFree(&pDesc->pages_memory);
		PX_VectorFree(&pDesc->sources);
		PX_VectorFree(&pDesc->ir_sources);
		PX_TextureFree(&pDesc->texture_pause);
		PX_TextureFree(&pDesc->texture_run);
		PX_TextureFree(&pDesc->texture_step);
		PX_TextureFree(&pDesc->texture_stop);
		PX_TextureFree(&pDesc->texture_reset);
		PX_MemoryFree(&pDesc->bin_map_to_source);
		PX_MemoryFree(&pDesc->bin_map_to_ir);
		PX_AbiFree(&pDesc->bin_packet_abi);
		PX_AbiFree(&pDesc->last_response_abi);
	}
}



PX_OBJECT_EVENT_FUNCTION(PX_OBJECT_OnSourceCodeViewerCursorDown)
{
	PX_Object_Debug* pDesc = PX_ObjectGetDesc0(PX_Object_Debug, (PX_Object*)ptr);
	if (PX_ObjectIsCursorInRegion(pObject, e))
	{
		PX_Object_Code_SetBorderColor(pObject, PX_COLOR_RED);
		PX_Object_Code_SetBorderColor(pDesc->ir_code_viewer, PX_COLOR_NONE);
	}
}

PX_OBJECT_EVENT_FUNCTION(PX_OBJECT_OnIRCodeViewerCursorDown)
{
	PX_Object_Debug* pDesc = PX_ObjectGetDesc0(PX_Object_Debug, (PX_Object*)ptr);
	if (PX_ObjectIsCursorInRegion(pObject, e))
	{
		PX_Object_Code_SetBorderColor(pObject, PX_COLOR_RED);
		PX_Object_Code_SetBorderColor(pDesc->source_code_viewer, PX_COLOR_NONE);
	}
}

PX_Object* PX_Object_Debug_Create(px_memorypool* mp, PX_Object* Parent, px_int x, px_int y, px_int Width, px_int Height, PX_FontModule* fm)
{
	PX_Object* pObject;
	PX_Object_Debug* pDesc;
	px_int i;
	pObject = PX_ObjectCreateEx(mp, Parent, (px_float)x, (px_float)y,0, (px_float)Width, (px_float)Height,0, PX_OBJECT_TYPE_DEBUG, PX_Object_Debug_Update, PX_Object_Debug_Render, PX_Object_Debug_Free,0, sizeof(PX_Object_Debug));
	pDesc = PX_ObjectGetDesc0(PX_Object_Debug, pObject);
	if (!pDesc)
	{
		return PX_NULL;
	}
	pDesc->fm = fm;
	pDesc->mp = mp;
	pDesc->area_tab = PX_Object_ScrollAreaCreate(mp, pObject, 0, 0, Width/2, 32);
	PX_Object_ScrollAreaSetHSliderBarHeight(pDesc->area_tab, 8);
	PX_VectorInitialize(mp, &pDesc->tab_buttons, sizeof(PX_Object*), 0);
	PX_VectorInitialize(mp, &pDesc->pages_memory, sizeof(PX_Object_Debug_pagememory), 32);
	PX_VectorInitialize(mp, &pDesc->list_contents, sizeof(px_char *), 32);
	PX_VectorInitialize(mp, &pDesc->sources, sizeof(PX_SyntaxLexer_Source), 0);
	PX_VectorInitialize(mp, &pDesc->ir_sources, sizeof(PX_SyntaxLexer_Source), 0);
	PX_AbiCreate_DynamicWriter(&pDesc->last_response_abi, mp);
	PX_memset(pDesc->ip_breakpoint, 0xFF, sizeof(pDesc->ip_breakpoint));
	PX_memset(pDesc->breakpoints, 0, sizeof(pDesc->breakpoints));
	PX_MemoryInitialize(mp, &pDesc->bin_map_to_source);
	PX_MemoryInitialize(mp, &pDesc->bin_map_to_ir);
	PX_AbiCreate_DynamicWriter(&pDesc->bin_packet_abi, mp);
	PX_memset(&pDesc->reg, 0, sizeof(pDesc->reg));
	pDesc->current_view_source_index = -1;
	pDesc->last_request_id = 1;
	pDesc->messagebox = PX_Object_MessageBoxCreate(mp, pObject, fm);
	if (pDesc->messagebox)
		PX_ObjectSetVisible(pDesc->messagebox, PX_FALSE);

	pDesc->list_info = PX_Object_ListCreate(mp, pObject, 0, (px_int)pDesc->area_tab->Height, Width / 4, (Height - (px_int)pDesc->area_tab->Height)/2, 20, PX_Object_Debug_ListCreate, pObject);
	//add content
	for(i=0;i<16; i++)
	{
		//PX_strset(pDesc->list_info_content[i], "N/A");
		PX_Object_ListAdd(pDesc->list_info,pDesc->list_info_content[i]);
	}

	pDesc->tree_root_abi = PX_Object_TreeCreate(mp, pObject, 0, (px_int)(pDesc->area_tab->Height+ pDesc->list_info->Height), Width / 4, (Height - (px_int)pDesc->area_tab->Height) / 2, 20 ,fm);
	PX_ObjectRegisterEvent(pDesc->tree_root_abi, PX_OBJECT_EVENT_EXECUTE, PX_Object_Debug_OnTreeExecute, pObject);

	pDesc->printer = PX_Object_PrinterCreate(mp, pObject, 0, 0, Width / 4 * 3, Height, fm);
	pDesc->source_code_viewer = PX_Object_Code_Create(mp, pObject, 0, 0, Width * 5 / 8, Height * 3 / 4, fm);


	pDesc->textviewer_out=PX_Object_TextViewerCreate(mp, pObject, 0, 0, 200, Height, fm);
	PX_ObjectSetVisible(pDesc->textviewer_out, PX_FALSE);
	pDesc->ir_code_viewer= PX_Object_Code_Create(mp, pObject, 0, 0, 200, Height, fm);
	PX_ObjectSetVisible(pDesc->ir_code_viewer, PX_TRUE);

	PX_ObjectRegisterEvent(pDesc->source_code_viewer, PX_OBJECT_EVENT_CURSORDOWN, PX_OBJECT_OnSourceCodeViewerCursorDown, pObject);
	PX_ObjectRegisterEvent(pDesc->ir_code_viewer, PX_OBJECT_EVENT_CURSORDOWN, PX_OBJECT_OnIRCodeViewerCursorDown, pObject);

	if(!PX_TextureCreate(mp,&pDesc->texture_run,20,20))
	{
		PX_ObjectDelete(pObject);
		return PX_NULL;
	}

	if (!PX_TextureCreate(mp, &pDesc->texture_pause, 20, 20))
	{
		PX_ObjectDelete(pObject);
		return PX_NULL;
	}

	if (!PX_TextureCreate(mp, &pDesc->texture_stop, 20, 20))
	{
		PX_ObjectDelete(pObject);
		return PX_NULL;
	}
	if (!PX_TextureCreate(mp, &pDesc->texture_step, 20, 20))
	{
		PX_ObjectDelete(pObject);
		return PX_NULL;
	}

	if (!PX_TextureCreate(mp, &pDesc->texture_reset, 20, 20))
	{
		PX_ObjectDelete(pObject);
		return PX_NULL;
	}

	PX_GeoDrawRightTriangle(&pDesc->texture_run, 0, 0, 19, 19, PX_COLOR(255, 128, 128, 128));
	PX_GeoDrawRect(&pDesc->texture_stop, 0, 0, 19, 19, PX_COLOR(255, 128, 128, 128));
	PX_GeoDrawArrow(&pDesc->texture_step, PX_POINT2D(0, 10), PX_POINT2D(19, 10), 1, PX_COLOR(255, 128, 128, 128));
	PX_GeoDrawRect(&pDesc->texture_pause, 0, 0, 8, 19, PX_COLOR(255, 128, 128, 128));
	PX_GeoDrawRect(&pDesc->texture_pause, 11, 0, 19, 19, PX_COLOR(255, 128, 128, 128));
	PX_GeoDrawRing(&pDesc->texture_reset, 10, 10, 5, 2, PX_COLOR(255, 128, 128, 128),0,270);
	PX_GeoDrawUpTriangle(&pDesc->texture_reset, 12, 5, 19, 10, PX_COLOR(255, 128, 128, 128));

	pDesc->controller_panel = PX_ObjectCreate(mp, pObject, 0, 0, 0, 0, 0,0 );
	pDesc->button_run= PX_Object_PushButtonCreate(mp, pDesc->controller_panel, 0, 3, 26, 26, "", 0);
	PX_Object_PushButtonSetTexture(pDesc->button_run, &pDesc->texture_run);
	PX_ObjectRegisterEvent(pDesc->button_run, PX_OBJECT_EVENT_EXECUTE, PX_Object_Debug_OnRun, pObject);

	pDesc->button_pause = PX_Object_PushButtonCreate(mp, pDesc->controller_panel, 28, 3, 26, 26, "", 0);
	PX_Object_PushButtonSetTexture(pDesc->button_pause, &pDesc->texture_pause);
	PX_ObjectRegisterEvent(pDesc->button_pause, PX_OBJECT_EVENT_EXECUTE, PX_Object_Debug_OnPause, pObject);

	pDesc->button_step = PX_Object_PushButtonCreate(mp, pDesc->controller_panel,  28*2, 3, 26, 26, "", 0);
	PX_Object_PushButtonSetTexture(pDesc->button_step, &pDesc->texture_step);
	PX_ObjectRegisterEvent(pDesc->button_step, PX_OBJECT_EVENT_EXECUTE, PX_Object_Debug_OnStep, pObject);

	pDesc->button_stop = PX_Object_PushButtonCreate(mp, pDesc->controller_panel, 28 * 3, 3, 26, 26, "", 0);
	PX_Object_PushButtonSetTexture(pDesc->button_stop, &pDesc->texture_stop);
	PX_ObjectRegisterEvent(pDesc->button_stop, PX_OBJECT_EVENT_EXECUTE, PX_Object_Debug_OnStop, pObject);

	pDesc->button_reset = PX_Object_PushButtonCreate(mp, pDesc->controller_panel, 28 * 4, 3, 26, 26, "", 0);
	PX_Object_PushButtonSetTexture(pDesc->button_reset, &pDesc->texture_reset);
	PX_ObjectRegisterEvent(pDesc->button_reset, PX_OBJECT_EVENT_EXECUTE, PX_Object_Debug_OnReset, pObject);


	pDesc->debugger_state_label = PX_Object_LabelCreate(mp, pDesc->controller_panel, 28 * 6, 3, 180, 20, "", fm, PX_COLOR_BLACK);
	PX_Object_LabelSetAlign(pDesc->debugger_state_label, PX_ALIGN_LEFTMID);

	pDesc->vm_state_label = PX_Object_LabelCreate(mp, pDesc->controller_panel, (px_int)pDesc->debugger_state_label->x+ (px_int)pDesc->debugger_state_label->Width+5, 3, 180, 20, "", fm, PX_COLOR_BLACK);
	PX_Object_LabelSetAlign(pDesc->vm_state_label, PX_ALIGN_LEFTMID);
	/*
	PX_OBJECT_DEBUG_STATE_DISCONNECT,
	PX_OBJECT_DEBUG_STATE_IDLE,
	PX_OBJECT_DEBUG_STATE_IDLE_WAIT,
	PX_OBJECT_DEBUG_STATE_RESET,
	PX_OBJECT_DEBUG_STATE_RESET_WAIT,
	PX_OBJECT_DEBUG_STATE_UPLOADING,
	PX_OBJECT_DEBUG_STATE_UPLOADING_WAIT,
	PX_OBJECT_DEBUG_STATE_QUERY_STATE,
	PX_OBJECT_DEBUG_STATE_QUERY_STATE_WAIT,
	PX_OBJECT_DEBUG_STATE_PAUSE,
	PX_OBJECT_DEBUG_STATE_PAUSE_WAIT,
	PX_OBJECT_DEBUG_STATE_STEP_NEXT,
	PX_OBJECT_DEBUG_STATE_STEP_NEXT_WAIT,
	PX_OBJECT_DEBUG_STATE_STEP_QUERY_STATE,
	PX_OBJECT_DEBUG_STATE_STEP_QUERY_STATE_WAIT,
	PX_OBJECT_DEBUG_STATE_RUNNING,
	PX_OBJECT_DEBUG_STATE_ERROR
	*/
	//FSM
	if (!PX_FSM_Initialize(mp, &pDesc->fsm))return PX_NULL;
	if (!PX_FSM_NewState(&pDesc->fsm, PX_OBJECT_DEBUG_STATE_DISCONNECT, PX_Object_Debug_STATE_DISCONNECT, pObject))return PX_NULL;
	if (!PX_FSM_NewState(&pDesc->fsm, PX_OBJECT_DEBUG_STATE_RESET, PX_Object_Debug_STATE_RESET, pObject))return PX_NULL;
	if (!PX_FSM_NewState(&pDesc->fsm, PX_OBJECT_DEBUG_STATE_RESET_WAIT, PX_Object_Debug_STATE_RESET_WAIT, pObject))return PX_NULL;
	if (!PX_FSM_NewState(&pDesc->fsm, PX_OBJECT_DEBUG_STATE_UPLOADING, PX_Object_Debug_STATE_UPLOADING, pObject))return PX_NULL;
	if (!PX_FSM_NewState(&pDesc->fsm, PX_OBJECT_DEBUG_STATE_UPLOADING_WAIT, PX_Object_Debug_STATE_UPLOADING_WAIT, pObject))return PX_NULL;
	if (!PX_FSM_NewState(&pDesc->fsm, PX_OBJECT_DEBUG_STATE_QUERY_STATE, PX_Object_Debug_STATE_QUERY_STATE, pObject))return PX_NULL;
	if (!PX_FSM_NewState(&pDesc->fsm, PX_OBJECT_DEBUG_STATE_QUERY_STATE_WAIT, PX_Object_Debug_STATE_QUERY_STATE_WAIT, pObject))return PX_NULL;
	if (!PX_FSM_NewState(&pDesc->fsm, PX_OBJECT_DEBUG_STATE_PAUSE, PX_Object_Debug_STATE_PAUSE, pObject))return PX_NULL;
	if (!PX_FSM_NewState(&pDesc->fsm, PX_OBJECT_DEBUG_STATE_PAUSE_WAIT, PX_Object_Debug_STATE_PAUSE, pObject))return PX_NULL;
	if (!PX_FSM_NewState(&pDesc->fsm, PX_OBJECT_DEBUG_STATE_STEP_NEXT, PX_Object_Debug_STATE_STEP_NEXT, pObject))return PX_NULL;
	if (!PX_FSM_NewState(&pDesc->fsm, PX_OBJECT_DEBUG_STATE_STEP_NEXT_WAIT, PX_Object_Debug_STATE_STEP_NEXT_WAIT, pObject))return PX_NULL;
	if (!PX_FSM_NewState(&pDesc->fsm, PX_OBJECT_DEBUG_STATE_STEP_QUERY_STATE, PX_Object_Debug_STATE_STEP_NEXT_QUERY_STATE, pObject))return PX_NULL;
	if (!PX_FSM_NewState(&pDesc->fsm, PX_OBJECT_DEBUG_STATE_STEP_QUERY_STATE_WAIT, PX_Object_Debug_STATE_STEP_NEXT_QUERY_STATE_WAIT, pObject))return PX_NULL;
	if (!PX_FSM_NewState(&pDesc->fsm, PX_OBJECT_DEBUG_STATE_STEP_IR_NEXT, PX_Object_Debug_STATE_STEP_IR_NEXT, pObject))return PX_NULL;
	if (!PX_FSM_NewState(&pDesc->fsm, PX_OBJECT_DEBUG_STATE_STEP_IR_NEXT_WAIT, PX_Object_Debug_STATE_STEP_IR_NEXT_WAIT, pObject))return PX_NULL;
	if (!PX_FSM_NewState(&pDesc->fsm, PX_OBJECT_DEBUG_STATE_STEP_IR_QUERY_STATE, PX_Object_Debug_STATE_STEP_IR_QUERY_STATE, pObject))return PX_NULL;
	if (!PX_FSM_NewState(&pDesc->fsm, PX_OBJECT_DEBUG_STATE_STEP_IR_QUERY_STATE_WAIT, PX_Object_Debug_STATE_STEP_IR_QUERY_STATE_WAIT, pObject))return PX_NULL;
	if (!PX_FSM_NewState(&pDesc->fsm, PX_OBJECT_DEBUG_STATE_RUNNING, PX_Object_Debug_STATE_RUNNING, pObject))return PX_NULL;
	if (!PX_FSM_NewState(&pDesc->fsm, PX_OBJECT_DEBUG_STATE_ERROR, PX_Object_Debug_STATE_ERROR, pObject))return PX_NULL;

	PX_FSM_SetState(&pDesc->fsm, PX_OBJECT_DEBUG_STATE_DISCONNECT);


	return pObject;
}

static px_bool PX_Object_Debug_PackDebugSource(PX_Syntax* psource, px_abi* packabi,const px_char name[])
{
	px_int i, j;
	px_string key;
	if (!PX_StringInitialize(packabi->dynamic.mp, &key))
		return PX_FALSE;

	//packet px_syntax->px_syntaxlexer->sources
	if (!PX_StringFormat1(&key, "%1_count", PX_STRINGFORMAT_STRING(name)))
	{
		PX_StringFree(&key);
		return PX_FALSE;
	}
	if (!PX_AbiSet_int(packabi, key.buffer, PX_VectorSize(&psource->reg_syntaxlexer.sources)))
	{
		PX_StringFree(&key);
		return PX_FALSE;
	}
	for (i = 0; i < PX_VectorSize(&psource->reg_syntaxlexer.sources); i++)
	{
		PX_SyntaxLexer_Source* src = PX_VECTORAT(PX_SyntaxLexer_Source, &psource->reg_syntaxlexer.sources, i);
		px_abi source_abi;
		if (!src)
		{
			PX_ASSERT();
			goto _ERROR;
		}
		PX_AbiCreate_DynamicWriter(&source_abi, packabi->dynamic.mp);

		//name
		if (!PX_AbiSet_string(&source_abi, "name", src->name ? src->name : (px_char*)""))
		{
			PX_AbiFree(&source_abi);
			goto _ERROR;
		}
		//source
		if (!PX_AbiSet_int(&source_abi, "source_length", src->source_length))
		{
			PX_AbiFree(&source_abi);
			goto _ERROR;
		}
		if (!PX_AbiSet_data(&source_abi, "source", src->source, src->source_length))
		{
			PX_AbiFree(&source_abi);
			goto _ERROR;
		}
		//line info
		if (!PX_AbiSet_int(&source_abi, "line_count", src->line_count))
		{
			PX_AbiFree(&source_abi);
			goto _ERROR;
		}
		if (!PX_AbiSet_int(&source_abi, "max_line_char_width", src->max_line_char_width))
		{
			PX_AbiFree(&source_abi);
			goto _ERROR;
		}
		if (!PX_AbiSet_data(&source_abi, "source_index_map_to_line_index", src->source_index_map_to_line_index, src->source_length * (px_int)sizeof(px_int)))
		{
			PX_AbiFree(&source_abi);
			goto _ERROR;
		}
		//cells
		if (!PX_AbiSet_int(&source_abi, "cells_count", src->cells_count))
		{
			PX_AbiFree(&source_abi);
			goto _ERROR;
		}
		if (!PX_AbiSet_data(&source_abi, "cells", src->cells, src->cells_count * (px_int)sizeof(PX_SyntaxLexer_Cell)))
		{
			PX_AbiFree(&source_abi);
			goto _ERROR;
		}
		if (!PX_AbiSet_data(&source_abi, "source_index_map_to_cell_index", src->source_index_map_to_cell_index, src->source_length * (px_int)sizeof(px_int)))
		{
			PX_AbiFree(&source_abi);
			goto _ERROR;
		}
		//line map
		if (!PX_AbiSet_data(&source_abi, "line_begin_cell_index_map", src->line_begin_cell_index_map, src->line_count * (px_int)sizeof(PX_SyntaxLexer_LineMap)))
		{
			PX_AbiFree(&source_abi);
			goto _ERROR;
		}
		//descriptor (vector of px_abi)
		if (!PX_AbiSet_int(&source_abi, "last_descriptor_index", src->last_descriptor_index))
		{
			PX_AbiFree(&source_abi);
			goto _ERROR;
		}
		if (!PX_AbiSet_int(&source_abi, "descriptor_count", PX_VectorSize(&src->descriptor)))
		{
			PX_AbiFree(&source_abi);
			goto _ERROR;
		}
		for (j = 0; j < PX_VectorSize(&src->descriptor); j++)
		{
			px_abi* pdesc = PX_VECTORAT(px_abi, &src->descriptor, j);
			if (!PX_StringFormat1(&key, "descriptor[%1]", PX_STRINGFORMAT_INT(j)))
			{
				PX_AbiFree(&source_abi);
				goto _ERROR;
			}
			if (!PX_AbiSet_Abi(&source_abi, key.buffer, pdesc))
			{
				PX_AbiFree(&source_abi);
				goto _ERROR;
			}
		}

		if (!PX_StringFormat2(&key, "%1[%2]",PX_STRINGFORMAT_STRING(name),PX_STRINGFORMAT_INT(i)))
		{
			PX_AbiFree(&source_abi);
			goto _ERROR;
		}
		if (!PX_AbiSet_Abi(packabi, key.buffer, &source_abi))
		{
			PX_AbiFree(&source_abi);
			goto _ERROR;
		}
		PX_AbiFree(&source_abi);
	}
	PX_StringFree(&key);
	return PX_TRUE;
_ERROR:
	PX_StringFree(&key);
	return PX_FALSE;
}

px_bool PX_Object_Debug_PackDebugInfo(PX_Syntax *psource, PX_Syntax *pir, px_abi *packabi)
{
	//check packabi is dynamic or not
	PX_ASSERTIFX(!packabi->dynamic.mp, "Pack abi must be dynamic memory");
	if (!PX_Object_Debug_PackDebugSource(psource, packabi, "source"))
		return PX_FALSE;
	if (!PX_Object_Debug_PackDebugSource(pir, packabi, "ir"))
		return PX_FALSE;
	
	do{
		px_abi* pscope = PX_Syntax_GetAbiFromForward(pir, "scope");
		if (pscope)
		{
			px_dword map_size = 0, ir_map_size = 0, bin_size = 0, rdata_size=0;
			const px_char* pimport;
			px_void* psrc_map;
			px_void* pir_map;
			px_void* pbin;
			px_void* prdata;
			px_abi export_abi;

			if (!PX_AbiExist_Type(pscope, "text", PX_ABI_TYPE_DATA))
				goto _ERROR;
			psrc_map = PX_AbiExist_Type(pscope, "bin_map_to_source", PX_ABI_TYPE_DATA) ? PX_AbiGet_data(pscope, "bin_map_to_source", &map_size) : PX_NULL;
			pir_map = PX_AbiExist_Type(pscope, "bin_map_to_ir", PX_ABI_TYPE_DATA) ? PX_AbiGet_data(pscope, "bin_map_to_ir", &ir_map_size) : PX_NULL;
			prdata = PX_AbiGet_data(pscope, "rdata", &rdata_size);
			pbin = PX_AbiGet_data(pscope, "text", &bin_size);
			pimport = PX_AbiGet_string(pscope, "import");
			if ( !pbin )
				goto _ERROR;
			if (!PX_AbiSet_string(packabi, "bin.module_name", PX_Syntax_GetCurrentLexerEntrySourceName(pir)))
				goto _ERROR;

			if (pimport)
			{
				if (!PX_AbiSet_string(packabi, "bin.import", pimport))
					goto _ERROR;
			}

			if (PX_AbiGet_AbiReadOnly(pscope, &export_abi, "export"))
			{
				if (!PX_AbiSet_Abi(packabi, "bin.export", &export_abi))
					goto _ERROR;
			}
			if (psrc_map)
			{
				if (!PX_AbiSet_data(packabi, "bin_map_to_source", psrc_map, (px_int)map_size))
					goto _ERROR;
			}
			if (pir_map)
			{
				if (!PX_AbiSet_data(packabi, "bin_map_to_ir", pir_map, (px_int)ir_map_size))
					goto _ERROR;
			}
			
			if (prdata)
			{
				if (!PX_AbiSet_data(packabi, "bin.rdata", prdata, (px_int)rdata_size))
					goto _ERROR;
			}

			if (!PX_AbiSet_data(packabi, "bin.text", pbin, (px_int)bin_size))
				goto _ERROR;
			
		}
		else
		{
			goto _ERROR;
		}
	}while(0);

	return PX_TRUE;
_ERROR:
	return PX_FALSE;
}

static px_bool PX_Object_Debug_UnpackDebugSource(PX_Object_Debug* pDesc, px_abi* pack_abi, const px_char name[], px_vector* target, px_string* key)
{
	px_int i, j, source_count;

	if (!PX_StringFormat1(key, "%1_count", PX_STRINGFORMAT_STRING(name)))
		return PX_FALSE;
	if (!PX_AbiExist_Type(pack_abi, key->buffer, PX_ABI_TYPE_INT))
		return PX_FALSE;
	source_count = PX_AbiGetValue_int(pack_abi, key->buffer);

	for (i = 0; i < source_count; i++)
	{
		PX_SyntaxLexer_Source new_source;
		px_abi source_abi;
		const px_char* pname;
		px_void* pdata;
		px_dword datasize;
		px_int descriptor_count;

		PX_memset(&new_source, 0, sizeof(new_source));

		if (!PX_StringFormat2(key, "%1[%2]", PX_STRINGFORMAT_STRING(name), PX_STRINGFORMAT_INT(i)))
			return PX_FALSE;
		if (!PX_AbiGet_AbiReadOnly(pack_abi, &source_abi, key->buffer))
			return PX_FALSE;

		//name
		pname = PX_AbiGet_string(&source_abi, "name");
		if (!pname) return PX_FALSE;
		new_source.name = (px_char*)MP_Malloc(pDesc->mp, PX_strlen(pname) + 1);
		if (!new_source.name) goto _ERROR_ITEM;
		PX_strcpy(new_source.name, pname, PX_strlen(pname) + 1);

		//source length & data
		new_source.source_length = PX_AbiGetValue_int(&source_abi, "source_length");
		pdata = PX_AbiGet_data(&source_abi, "source", &datasize);
		if (!pdata || (px_int)datasize != new_source.source_length) goto _ERROR_ITEM;
		new_source.source = (px_char*)MP_Malloc(pDesc->mp, new_source.source_length + 1);
		if (!new_source.source) goto _ERROR_ITEM;
		PX_memcpy(new_source.source, pdata, new_source.source_length);
		new_source.source[new_source.source_length] = 0;

		//line info
		new_source.line_count = PX_AbiGetValue_int(&source_abi, "line_count");
		new_source.max_line_char_width = PX_AbiGetValue_int(&source_abi, "max_line_char_width");
		pdata = PX_AbiGet_data(&source_abi, "source_index_map_to_line_index", &datasize);
		if (!pdata || (px_int)datasize != new_source.source_length * (px_int)sizeof(px_int)) goto _ERROR_ITEM;
		new_source.source_index_map_to_line_index = (px_int*)MP_Malloc(pDesc->mp, datasize);
		if (!new_source.source_index_map_to_line_index) goto _ERROR_ITEM;
		PX_memcpy(new_source.source_index_map_to_line_index, pdata, datasize);

		//cells
		new_source.cells_count = PX_AbiGetValue_int(&source_abi, "cells_count");
		pdata = PX_AbiGet_data(&source_abi, "cells", &datasize);
		if (!pdata || (px_int)datasize != new_source.cells_count * (px_int)sizeof(PX_SyntaxLexer_Cell)) goto _ERROR_ITEM;
		new_source.cells = (PX_SyntaxLexer_Cell*)MP_Malloc(pDesc->mp, datasize);
		if (!new_source.cells) goto _ERROR_ITEM;
		PX_memcpy(new_source.cells, pdata, datasize);

		pdata = PX_AbiGet_data(&source_abi, "source_index_map_to_cell_index", &datasize);
		if (!pdata || (px_int)datasize != new_source.source_length * (px_int)sizeof(px_int)) goto _ERROR_ITEM;
		new_source.source_index_map_to_cell_index = (px_int*)MP_Malloc(pDesc->mp, datasize);
		if (!new_source.source_index_map_to_cell_index) goto _ERROR_ITEM;
		PX_memcpy(new_source.source_index_map_to_cell_index, pdata, datasize);

		//line map
		pdata = PX_AbiGet_data(&source_abi, "line_begin_cell_index_map", &datasize);
		if (!pdata || (px_int)datasize != new_source.line_count * (px_int)sizeof(PX_SyntaxLexer_LineMap)) goto _ERROR_ITEM;
		new_source.line_begin_cell_index_map = (PX_SyntaxLexer_LineMap*)MP_Malloc(pDesc->mp, datasize);
		if (!new_source.line_begin_cell_index_map) goto _ERROR_ITEM;
		PX_memcpy(new_source.line_begin_cell_index_map, pdata, datasize);

		//descriptor
		new_source.last_descriptor_index = PX_AbiGetValue_int(&source_abi, "last_descriptor_index");
		descriptor_count = PX_AbiGetValue_int(&source_abi, "descriptor_count");
		if (!PX_VectorInitialize(pDesc->mp, &new_source.descriptor, sizeof(px_abi), descriptor_count > 0 ? descriptor_count : 1))
			goto _ERROR_ITEM;
		for (j = 0; j < descriptor_count; j++)
		{
			px_abi desc_readonly;
			px_abi desc_dynamic;
			if (!PX_StringFormat1(key, "descriptor[%1]", PX_STRINGFORMAT_INT(j)))
				goto _ERROR_ITEM_DESC;
			if (!PX_AbiGet_AbiReadOnly(&source_abi, &desc_readonly, key->buffer))
				goto _ERROR_ITEM_DESC;
			PX_AbiCreate_DynamicWriter(&desc_dynamic, pDesc->mp);
			if (!PX_AbiCopy_FromAbi(&desc_dynamic, &desc_readonly))
			{
				PX_AbiFree(&desc_dynamic);
				goto _ERROR_ITEM_DESC;
			}
			if (!PX_VectorPushback(&new_source.descriptor, &desc_dynamic))
			{
				PX_AbiFree(&desc_dynamic);
				goto _ERROR_ITEM_DESC;
			}
		}

		if (!PX_VectorPushback(target, &new_source))
			goto _ERROR_ITEM_DESC;

		continue;
	_ERROR_ITEM_DESC:
		for (j = 0; j < PX_VectorSize(&new_source.descriptor); j++)
			PX_AbiFree(PX_VECTORAT(px_abi, &new_source.descriptor, j));
		PX_VectorFree(&new_source.descriptor);
	_ERROR_ITEM:
		if (new_source.name) MP_Free(pDesc->mp, new_source.name);
		if (new_source.source) MP_Free(pDesc->mp, new_source.source);
		if (new_source.source_index_map_to_line_index) MP_Free(pDesc->mp, new_source.source_index_map_to_line_index);
		if (new_source.cells) MP_Free(pDesc->mp, new_source.cells);
		if (new_source.source_index_map_to_cell_index) MP_Free(pDesc->mp, new_source.source_index_map_to_cell_index);
		if (new_source.line_begin_cell_index_map) MP_Free(pDesc->mp, new_source.line_begin_cell_index_map);
		return PX_FALSE;
	}
	return PX_TRUE;
}

px_bool PX_Object_Debug_UnPackDebugInfo(PX_Object *pObject, const px_byte packed_debug_info_abi[], px_int size)
{
	PX_Object_Debug* pDesc = PX_ObjectGetDesc0(PX_Object_Debug, pObject);
	px_abi pack_abi;
	px_int i;
	px_string key;
	if (!pDesc)
	{
		return PX_FALSE;
	}

	//clear previous unpacked content
	for (i = 0; i < PX_VectorSize(&pDesc->sources); i++)
	{
		PX_Object_Debug_FreeSource(pDesc->mp, PX_VECTORAT(PX_SyntaxLexer_Source, &pDesc->sources, i));
	}
	PX_VectorClear(&pDesc->sources);
	for (i = 0; i < PX_VectorSize(&pDesc->ir_sources); i++)
	{
		PX_Object_Debug_FreeSource(pDesc->mp, PX_VECTORAT(PX_SyntaxLexer_Source, &pDesc->ir_sources, i));
	}
	PX_VectorClear(&pDesc->ir_sources);
	PX_MemoryClear(&pDesc->bin_map_to_source);
	PX_MemoryClear(&pDesc->bin_map_to_ir);
	PX_AbiClear(&pDesc->bin_packet_abi);

	if (!PX_AbiCheckBufferReady(packed_debug_info_abi, size))
		return PX_FALSE;

	PX_AbiCreate_StaticReader(&pack_abi, packed_debug_info_abi, size);

	if (!PX_StringInitialize(pDesc->mp, &key))
		return PX_FALSE;

	if (!PX_Object_Debug_UnpackDebugSource(pDesc, &pack_abi, "source", &pDesc->sources, &key))
		goto _ERROR;
	if (!PX_Object_Debug_UnpackDebugSource(pDesc, &pack_abi, "ir", &pDesc->ir_sources, &key))
		goto _ERROR;

	if (PX_AbiExist_Type(&pack_abi, "bin_map_to_source", PX_ABI_TYPE_DATA))
	{
		px_dword map_size = 0;
		px_void* pmap = PX_AbiGet_data(&pack_abi, "bin_map_to_source", &map_size);
		if (pmap && map_size)
		{
			if (!PX_MemoryCat(&pDesc->bin_map_to_source, pmap, (px_int)map_size))
				goto _ERROR;
		}
	}
	if (PX_AbiExist_Type(&pack_abi, "bin_map_to_ir", PX_ABI_TYPE_DATA))
	{
		px_dword map_size = 0;
		px_void* pmap = PX_AbiGet_data(&pack_abi, "bin_map_to_ir", &map_size);
		if (pmap && map_size)
		{
			if (!PX_MemoryCat(&pDesc->bin_map_to_ir, pmap, (px_int)map_size))
				goto _ERROR;
		}
	}
	if (PX_AbiExist_Type(&pack_abi, "bin", PX_ABI_TYPE_ABI))
	{
		px_abi bin_readonly;
		if (!PX_AbiGet_AbiReadOnly(&pack_abi, &bin_readonly, "bin"))
			goto _ERROR;
		
		//check text
		if (!PX_AbiExist_Type(&bin_readonly, "text", PX_ABI_TYPE_DATA))
		{
			goto _ERROR;
		}
		
		if(!PX_AbiCopy_FromBuffer(&pDesc->bin_packet_abi, PX_AbiGet_Pointer(&bin_readonly), PX_AbiGet_Size(&bin_readonly)))
		{
			goto _ERROR;
		}
	}

	PX_StringFree(&key);
	PX_Object_Debug_UpdateTabButtons(pObject);
	if (PX_VectorCheckIndex(&pDesc->sources, 0))
	{
		pDesc->current_view_source_index = 0;
		PX_Object_Code_SetSource(pDesc->source_code_viewer, PX_VECTORAT(PX_SyntaxLexer_Source, &pDesc->sources, 0));
	}
	if (PX_VectorCheckIndex(&pDesc->ir_sources, 0))
	{
		pDesc->current_view_source_index = 0;
		PX_Object_Code_SetSource(pDesc->ir_code_viewer, PX_VECTORAT(PX_SyntaxLexer_Source, &pDesc->ir_sources, 0));
	}
	return PX_TRUE;
_ERROR:
	PX_StringFree(&key);
	return PX_FALSE;
}