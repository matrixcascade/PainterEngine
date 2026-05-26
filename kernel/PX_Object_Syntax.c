#include "PX_Object_Syntax.h"


PX_OBJECT_EVENT_FUNCTION(PX_Object_Syntax_OnSyntaxRun)
{
	PX_Object_Syntax* pDesc = PX_ObjectGetDesc0(PX_Object_Syntax, (PX_Object*)ptr);
	if (pDesc->state == PX_OBJECT_SYNTAX_STATE_IDLE || pDesc->state == PX_OBJECT_SYNTAX_STATE_PAUSE || pDesc->state == PX_OBJECT_SYNTAX_STATE_ERROR)
	{
		if (pDesc->current_view_source_index == -1)
			return;

		if (!PX_Syntax_IsExecuting(pDesc->psyntax))
		{
			//push current view source index to stack
			PX_Syntax_ClearState(pDesc->psyntax);
			PX_Object_PrinterClear(pDesc->printer);
			if (!PX_Syntax_CallSourceIndex(pDesc->psyntax, pDesc->current_view_source_index,"_"))
			{
				return;
			}
		}
		pDesc->state = PX_OBJECT_SYNTAX_STATE_RUN;
	}
	else if (pDesc->state == PX_OBJECT_SYNTAX_STATE_STEP)
	{
		pDesc->state = PX_OBJECT_SYNTAX_STATE_RUN;
	}
}

PX_OBJECT_EVENT_FUNCTION(PX_Object_Syntax_OnSyntaxPause)
{
	PX_Object_Syntax* pDesc = PX_ObjectGetDesc0(PX_Object_Syntax, (PX_Object *)ptr);
	if (pDesc->state == PX_OBJECT_SYNTAX_STATE_RUN)
	{
		pDesc->state = PX_OBJECT_SYNTAX_STATE_PAUSE;
	}
	else if (pDesc->state == PX_OBJECT_SYNTAX_STATE_STEP)
	{
		pDesc->state = PX_OBJECT_SYNTAX_STATE_PAUSE;
	}
}

PX_OBJECT_EVENT_FUNCTION(PX_Object_Syntax_OnSyntaxStep)
{
	PX_Object_Syntax* pDesc = PX_ObjectGetDesc0(PX_Object_Syntax, (PX_Object *)ptr);
	if (pDesc->state == PX_OBJECT_SYNTAX_STATE_IDLE || pDesc->state == PX_OBJECT_SYNTAX_STATE_PAUSE || pDesc->state == PX_OBJECT_SYNTAX_STATE_ERROR)
	{
		if (pDesc->current_view_source_index == -1)
			return;

		if (!PX_Syntax_IsExecuting(pDesc->psyntax))
		{
			//push current view source index to stack
			PX_Syntax_ClearState(pDesc->psyntax);
			PX_Object_PrinterClear(pDesc->printer);
			if (!PX_Syntax_CallSourceIndex(pDesc->psyntax, pDesc->current_view_source_index,"_"))
			{
				return;
			}
		}
		pDesc->state = PX_OBJECT_SYNTAX_STATE_STEP;

	}
	else if (pDesc->state == PX_OBJECT_SYNTAX_STATE_RUN)
	{
		pDesc->state = PX_OBJECT_SYNTAX_STATE_STEP;
	}
}

PX_OBJECT_EVENT_FUNCTION(PX_Object_Syntax_OnSyntaxStop)
{
	PX_Object_Syntax* pDesc = PX_ObjectGetDesc0(PX_Object_Syntax,  (PX_Object*)ptr);
	if (pDesc->state == PX_OBJECT_SYNTAX_STATE_RUN || pDesc->state == PX_OBJECT_SYNTAX_STATE_STEP || pDesc->state == PX_OBJECT_SYNTAX_STATE_PAUSE)
	{
		pDesc->state = PX_OBJECT_SYNTAX_STATE_IDLE;
	}
}

px_void PX_Object_Syntax_Refresh_Message(PX_Object* pObject)
{
	PX_Object_Syntax* pDesc = PX_ObjectGetDesc0(PX_Object_Syntax, pObject);
	px_string* pstr = PX_Syntax_GetMessage(pDesc->psyntax);
	if (PX_StringLen(pstr) != pDesc->reg_target_syntax_message_offset)
	{
		px_char content[128] = { 0 };
		px_int index = pDesc->reg_target_syntax_message_offset;
		px_int content_i = 0;
		if (pDesc->reg_target_syntax_message_offset>PX_StringLen(pstr))
		{
			PX_Object_PrinterClear(pDesc->printer);
		}
		while (PX_TRUE)
		{
			if (index >= PX_StringLen(pstr))
			{
				break;
			}
			if (pstr->buffer[index] == '\n')
			{
				pDesc->reg_target_syntax_message_offset = index + 1;
				index = pDesc->reg_target_syntax_message_offset;
				content_i = 0;
				PX_Object_PrinterPrintText(pDesc->printer, content);
			}
			else if (content_i >= sizeof(content) - 1)
			{
				pDesc->reg_target_syntax_message_offset = index;
				index = pDesc->reg_target_syntax_message_offset;
				content_i = 0;
				PX_Object_PrinterPrintText(pDesc->printer, content);
			}
			else
			{
				content[content_i++] = pstr->buffer[index++];
				content[content_i] = '\0';
			}
		}
	}
}

PX_OBJECT_EVENT_FUNCTION(PX_Object_Syntax_OnTabButtonExecute)
{
	px_int i;
	PX_Object* pIDEObject = (PX_Object*)ptr;
	PX_Object_Syntax* pDesc = PX_ObjectGetDesc0(PX_Object_Syntax, pIDEObject);
	for (i = 0; i < pDesc->tab_buttons.size; i++)
	{
		PX_Object* pButton = *PX_VECTORAT(PX_Object*, &pDesc->tab_buttons, i);
		if (pButton == pObject)
		{
			PX_Object_Syntax_pagememory* pagememory;
			PX_SyntaxLexer_Source* psrc;

			// save current page scroll state
			if (pDesc->current_view_source_index >= 0 && PX_VectorCheckIndex(&pDesc->pages_memory, pDesc->current_view_source_index))
			{
				PX_Object_Syntax_pagememory* pcurpage = PX_VECTORAT(PX_Object_Syntax_pagememory, &pDesc->pages_memory, pDesc->current_view_source_index);
				PX_ASSERTIFX(!pcurpage, "current_view_source_index is valid but pagememory is null");
				pcurpage->row_offset = PX_Object_Code_GetScrollOffsetYRow(pDesc->source_code_viewer);
				pcurpage->x_offset = PX_Object_Code_GetScrollOffsetX(pDesc->source_code_viewer);
			}

			pDesc->current_view_source_index = i;

			// ensure pages_memory is large enough
			if (pDesc->current_view_source_index >= pDesc->pages_memory.size)
				PX_VectorResize(&pDesc->pages_memory, pDesc->current_view_source_index + 1);

			psrc = PX_Syntax_GetSourceByIndex(pDesc->psyntax, pDesc->current_view_source_index);
			PX_Object_Code_SetSource(pDesc->source_code_viewer, psrc);

			// restore scroll state for new page
			pagememory = PX_VECTORAT(PX_Object_Syntax_pagememory, &pDesc->pages_memory, pDesc->current_view_source_index);
			PX_Object_Code_SetScrollOffset(pDesc->source_code_viewer, pagememory->row_offset, pagememory->x_offset);
			return;
		}
	}
}

PX_OBJECT_EVENT_FUNCTION(PX_Object_Syntax_OnTreeExecute)
{
	PX_Object* pIDEObject = (PX_Object*)ptr;
	PX_Object_Syntax* pDesc = PX_ObjectGetDesc0(PX_Object_Syntax, pIDEObject);
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

static px_void PX_Object_Syntax_ReorderTab(PX_Object* pObject)
{
	px_int i;
	PX_Object_Syntax* pDesc = PX_ObjectGetDesc0(PX_Object_Syntax, pObject);
	px_float x = 0;
	for (i = 0; i < pDesc->tab_buttons.size; i++)
	{
		PX_Object* pButton = *PX_VECTORAT(PX_Object*, &pDesc->tab_buttons, i);
		pButton->x = x;
		x += pButton->Width + 2;
	}
	PX_Object_ScrollAreaUpdateRange(pDesc->area_tab);
}

px_void  PX_Object_Syntax_UpdateTabButtons(PX_Object* pObject)
{
	px_int i;
	PX_Object_Syntax* pDesc = PX_ObjectGetDesc0(PX_Object_Syntax, pObject);
	for (i = 0; i < pDesc->tab_buttons.size; i++)
	{
		PX_Object* pObject = *PX_VECTORAT(PX_Object*, &pDesc->tab_buttons, i);
		PX_ObjectDelete(pObject);
	}
	PX_VectorClear(&pDesc->tab_buttons);
	for (i = 0; i < PX_Syntax_GetSourceCount(pDesc->psyntax); i++)
	{
		px_int  w, h; PX_Object* pButtonObject;
		PX_SyntaxLexer_Source* psource = PX_Syntax_GetSourceByIndex(pDesc->psyntax, i);
		PX_FontModuleTextGetRenderWidthHeight(pDesc->fm, psource->name, &w, &h);
		pButtonObject = PX_Object_PushButtonCreate(pObject->mp, pDesc->area_tab, 0, 0, w + 10, 24, psource->name, pDesc->fm);
		PX_VectorPushback(&pDesc->tab_buttons, &pButtonObject);
		PX_ObjectRegisterEvent(pButtonObject, PX_OBJECT_EVENT_EXECUTE, PX_Object_Syntax_OnTabButtonExecute, pObject);
	}
	PX_Object_Syntax_ReorderTab(pObject);
}


static px_bool PX_Object_Syntax_Refresh_SyntaxAst(PX_Object* pObject)
{
	PX_Object_Syntax* pDesc = PX_ObjectGetDesc0(PX_Object_Syntax, pObject);
	
	if (pDesc->psyntax)
	{
		px_int i;
		PX_Object_ListClear(pDesc->list_info);
		for (i = 0; i < pDesc->list_contents.size; i++)
		{
			px_char* p = *PX_VECTORAT(px_char*, &pDesc->list_contents, i);
			MP_Free(pDesc->mp, p);
		}
		PX_VectorClear(&pDesc->list_contents);

		for (i = 0; i < pDesc->psyntax->reg_ast_stack.size; i++)
		{
			px_char content[64] = { 0 },*pnewcontent;
			PX_Syntax_ast* past = PX_VECTORAT(PX_Syntax_ast, &pDesc->psyntax->reg_ast_stack, i);
			PX_Syntax_pebnf* ppebnf = PX_Syntax_GetPebnfByIndex(pDesc->psyntax, past->pebnf_index);
			if (ppebnf)
			{
				PX_strcat_s(content, sizeof(content), "*");
			}

			if (past->pbnfnode)
			{
				switch (past->pbnfnode->type)
				{
				case PX_SYNTAX_AST_TYPE_CONSTANT:
					PX_strcat_s(content, sizeof(content), "const:");
					break;
				case PX_SYNTAX_AST_TYPE_CONTINUOUS:
					PX_strcat_s(content, sizeof(content), "_:");
					break;
				case PX_SYNTAX_AST_TYPE_FUNCTION:
					PX_strcat_s(content, sizeof(content), "func:");
					break;
				case PX_SYNTAX_AST_TYPE_LINKER:
					PX_strcat_s(content, sizeof(content), "linker:");
					break;
				case PX_SYNTAX_AST_TYPE_RECURSION:
					PX_strcat_s(content, sizeof(content), "...:");
					break;
				default:
					PX_strcat_s(content, sizeof(content), "unknow:");
					break;
				}

				PX_strcat_s(content, sizeof(content), "[");
				PX_strcat_s(content, sizeof(content), past->pbnfnode->constant.buffer);
				PX_strcat_s(content, sizeof(content), "] ");
			}

			PX_strcat_s(content, sizeof(content), "ret:");
			PX_strcat_s(content, sizeof(content), PX_itos(past->call_abistack_count, 10).data);
			PX_strcat_s(content, sizeof(content), " ");
			
			pnewcontent = MP_Malloc(pDesc->mp, sizeof(content));
			PX_memcpy(pnewcontent, content, sizeof(content));
			PX_Object_ListAdd(pDesc->list_info, pnewcontent);
		}
		PX_Object_ListMoveToBottom(pDesc->list_info);
	}

	return PX_TRUE;

}

static px_bool PX_Object_Syntax_Refresh_SyntaxAbi(PX_Object* pObject)
{
	px_int i;
	px_abi* pabis;
	px_int count;
	PX_Object_Syntax* pDesc = PX_ObjectGetDesc0(PX_Object_Syntax, pObject);
	if (!pDesc)
	{
		return PX_FALSE;
	}
	if (!pDesc->psyntax)
	{
		return PX_FALSE;
	}
	pabis = (px_abi*)pDesc->psyntax->reg_abi_stack.data;
	count = pDesc->psyntax->reg_abi_stack.size;
	PX_Object_TreeClear(pDesc->tree_root_abi);
	if (count <= 0)
	{
		return PX_TRUE;
	}
	for (i = count - 1; i >= 0; i--)
	{
		px_char content[32] = { 0 };
		const px_char* pname = PX_AbiGet_string(&pabis[i], "name");
		if (!pname)
		{
			pname = "UNKNOW";
		}
		PX_strcat(content, PX_itos(i, 10).data);
		PX_strcat(content, ":");
		PX_strcat(content, "[");
		PX_strcat(content, pname);
		PX_strcat(content, "]");

		PX_Object_TreeAddAbi(pDesc->tree_root_abi, content, &pabis[i]);
	}

	return PX_TRUE;

}

px_void PX_Object_Syntax_OutContent_Clear(PX_Object* pObject)
{
	PX_Object_Syntax* pDesc = PX_ObjectGetDesc0(PX_Object_Syntax, pObject);
	PX_Object_TextViewerSetText(pDesc->textviewer_out, "");
}



PX_OBJECT_UPDATE_FUNCTION(PX_Object_Syntax_Update)
{
	PX_Object_Syntax* pDesc = PX_ObjectGetDesc0(PX_Object_Syntax, pObject);
	px_int run_circles = 1;
	if (pDesc->psyntax==PX_NULL)
	{
		return;
	}
	if (pDesc->struct_update_delay)
	{
		if (pDesc->struct_update_delay > elapsed)
		{
			pDesc->struct_update_delay -= elapsed;
		}
		else
		{
			pDesc->struct_update_delay = 0;
			PX_Object_Syntax_Refresh_Message(pObject);
			PX_Object_Syntax_Refresh_SyntaxAst(pObject);
			PX_Object_Syntax_Refresh_SyntaxAbi(pObject);
		}
	}



	switch (pDesc->state)
	{
	case PX_OBJECT_SYNTAX_STATE_IDLE:
	case PX_OBJECT_SYNTAX_STATE_PAUSE:
	case PX_OBJECT_SYNTAX_STATE_ERROR:
		break;
	case PX_OBJECT_SYNTAX_STATE_RUN:
	{
		if (elapsed >= 50)
		{
			pDesc->last_run_circles -= 5;
		}
		else
		{
			pDesc->last_run_circles += 5;
		}
		if (pDesc->last_run_circles < 5)
		{
			pDesc->last_run_circles = 5;
		}
		run_circles = pDesc->last_run_circles;
	}
	case PX_OBJECT_SYNTAX_STATE_STEP:
	{
		while (run_circles--)
		{
			PX_SYNTAX_AST_RETURN ast_return = PX_Syntax_ExecuteNext(pDesc->psyntax);
			px_int current_source_index = PX_Syntax_GetCurrentSourceIndex(pDesc->psyntax);

			if (current_source_index != -1)
			{
				if (pDesc->current_view_source_index != current_source_index)
				{
					// save current page scroll state
					if (pDesc->current_view_source_index >= 0 && PX_VectorCheckIndex(&pDesc->pages_memory, pDesc->current_view_source_index))
					{
						PX_Object_Syntax_pagememory* pcurpage = PX_VECTORAT(PX_Object_Syntax_pagememory, &pDesc->pages_memory, pDesc->current_view_source_index);
						pcurpage->row_offset = PX_Object_Code_GetScrollOffsetYRow(pDesc->source_code_viewer);
						pcurpage->x_offset = PX_Object_Code_GetScrollOffsetX(pDesc->source_code_viewer);
					}

					pDesc->current_view_source_index = current_source_index;

					// ensure pages_memory is large enough
					if (pDesc->current_view_source_index >= pDesc->pages_memory.size)
					{
						PX_Object_Syntax_pagememory* pnewpage;
						PX_VectorResize(&pDesc->pages_memory, pDesc->current_view_source_index + 1);
						pnewpage = PX_VECTORLAST(PX_Object_Syntax_pagememory, &pDesc->pages_memory);
						pnewpage->row_offset = 0;
						pnewpage->x_offset = 0;
					}
				}
			}

			if (ast_return == PX_SYNTAX_AST_RETURN_ERROR)
			{
				pDesc->state = PX_OBJECT_SYNTAX_STATE_ERROR;
				break;
			}
			else if (ast_return == PX_SYNTAX_AST_RETURN_END)
			{
				pDesc->state = PX_OBJECT_SYNTAX_STATE_IDLE;
				break;
			}
			else if (ast_return == PX_SYNTAX_AST_RETURN_CONTINUE)
			{
				if (pDesc->state == PX_OBJECT_SYNTAX_STATE_STEP)
				{
					pDesc->state = PX_OBJECT_SYNTAX_STATE_PAUSE;
					break;
				}
			}
			else
			{
				pDesc->state = PX_OBJECT_SYNTAX_STATE_ERROR;
				break;
			}

			if (current_source_index != -1)
			{
				px_int current_row = PX_Syntax_GetCurrentLine(pDesc->psyntax);

				if (pDesc->reg_last_break_row_index != current_row && current_row != -1)
				{
					PX_SyntaxLexer_Source* pSource = PX_Syntax_GetSourceByIndex(pDesc->psyntax, current_source_index);
					PX_SyntaxLexer_LineMap* pRow = &pSource->line_begin_cell_index_map[current_row];
					if (pRow->bdebugbreak)
					{
						pDesc->reg_last_break_row_index = current_row;
						pDesc->state = PX_OBJECT_SYNTAX_STATE_PAUSE;
						run_circles = 0;
						break;
					}
				}

			}
		}
		if(pDesc->struct_update_delay==0)
			pDesc->struct_update_delay = 100;

	
		if (PX_Syntax_GetCurrentSourceIndex(pDesc->psyntax) == pDesc->current_view_source_index)
			PX_Object_Code_SetCursorLineAndView(pDesc->source_code_viewer, PX_Syntax_GetCurrentLine(pDesc->psyntax));
		
	}
	break;
  }

}


PX_OBJECT_RENDER_FUNCTION(PX_Object_Syntax_Render)
{
	px_rect region = PX_ObjectGetRect(pObject);
	PX_Object_Syntax* pDesc = PX_ObjectGetDesc(PX_Object_Syntax, pObject);
	px_int content_ir_panel_width= (px_int)region.width * 3 / 4 - 20;
	px_int content_panel_width, content_panel_height;

	if (content_ir_panel_width < 120)
		content_ir_panel_width = 120;

	content_panel_width = content_ir_panel_width * 3 / 4;
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

	pDesc->textviewer_out->x = pDesc->source_code_viewer->x + pDesc->source_code_viewer->Width;
	pDesc->textviewer_out->y = pDesc->list_info->y;
	pDesc->textviewer_out->Width = region.width - pDesc->source_code_viewer->Width - pDesc->tree_root_abi->Width;
	pDesc->textviewer_out->Height = (px_float)content_panel_height + 20;

	if (pDesc->psyntax)
	{
		if (pDesc->last_tab_syntax_source_count != PX_Syntax_GetSourceCount(pDesc->psyntax))
		{
			pDesc->last_tab_syntax_source_count = PX_Syntax_GetSourceCount(pDesc->psyntax);
			PX_Object_Syntax_UpdateTabButtons(pObject);
		}
	}

	pDesc->printer->Width = region.width - pDesc->tree_root_abi->Width;
	pDesc->printer->Height = (region.height - 32) / 4;
	pDesc->printer->x = pDesc->tree_root_abi->Width;
	pDesc->printer->y = pDesc->source_code_viewer->y + pDesc->source_code_viewer->Height;
}

PX_SYNTAX_FUNCTION(PX_Syntax_ide_error)
{
	//px_syntaxlexer* pLexer = PX_Syntax_GetSyntaxLexer(pSyntax);
	PX_ASSERTX("unknown symbols syntax error");
	PX_Syntax_Terminate(pSyntax, "ast:error:Symbols Syntax Error.");
	return PX_FALSE;
}

PX_SYNTAX_FUNCTION(PX_Syntax_ide_no_symbol)
{
	px_char ch;
	PX_SYNTAXLEXER_STATE lexerState = PX_Syntax_GetLexerState(pSyntax);
	ch = PX_Syntax_GetNextChar(pSyntax);
	if (ch==';')
	{
		ch = PX_Syntax_GetNextChar(pSyntax);
		if (ch=='@')
		{
			PX_Syntax_SetLexerState(pSyntax, &lexerState);
			return PX_FALSE;
		}
	}
	while (PX_TRUE)
	{
		ch = PX_Syntax_GetNextChar(pSyntax);
		if (ch=='\0'||ch=='\n')
		{
			break;
		}
	}
	return PX_TRUE;
}



PX_SYNTAX_FUNCTION(PX_Syntax_ide_local)
{
	PX_Object* pObject = (PX_Object*)userptr;
	PX_Object_Syntax* pDesc = PX_ObjectGetDesc0(PX_Object_Syntax, pObject);
	px_abi* plastsecond = PX_Syntax_GetAbiSecondLast(pSyntax);
	px_abi* plast = PX_Syntax_GetAbiLast(pSyntax);
	PX_Syntax_PopAbi(pSyntax);
	PX_Syntax_PopAbi(pSyntax);
	return PX_TRUE;
}








PX_OBJECT_RENDER_FUNCTION(PX_Object_Syntax_List_Item_Render)
{
	px_char* pcontent = (px_char*)PX_Object_ListItemGetData(pObject);
	PX_FontModuleDrawText(psurface, PX_NULL, (px_int)pObject->x+2, (px_int)pObject->y+2, PX_ALIGN_LEFTTOP, pcontent, PX_COLOR_BLACK);
}

PX_OBJECT_LIST_ITEM_CREATE_FUNCTION(PX_Object_Syntax_ListCreate)
{
	PX_ObjectSetRenderFunction(ItemObject, PX_Object_Syntax_List_Item_Render, 0);
	return PX_TRUE;
}



PX_OBJECT_EVENT_FUNCTION(PX_Object_Syntax_OnBreakpointTrigger)
{
	PX_Object* pSyntaxObject = (PX_Object*)ptr;
	PX_Object_Syntax* pDesc = PX_ObjectGetDesc0(PX_Object_Syntax, pSyntaxObject);
	px_int abs_line = PX_Object_Event_GetInt(e);
	// toggle breakpoint (here do nothing but record the breakpoint line, the breakpoint will be effective after execution reaches this line)
	return;
}


px_void PX_Object_Syntax_Clear(PX_Object* pObject)
{
	PX_Object_Syntax* pDesc = PX_ObjectGetDesc0(PX_Object_Syntax, pObject);
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
}

PX_OBJECT_FREE_FUNCTION(PX_Object_Syntax_Free)
{
	PX_Object_Syntax* pDesc = PX_ObjectGetDesc0(PX_Object_Syntax, pObject);
	if (pDesc)
	{
		PX_Object_Syntax_Clear(pObject);
		PX_VectorFree(&pDesc->tab_buttons);
		PX_VectorFree(&pDesc->list_contents);
		PX_VectorFree(&pDesc->pages_memory);
		PX_TextureFree(&pDesc->texture_pause);
		PX_TextureFree(&pDesc->texture_run);
		PX_TextureFree(&pDesc->texture_step);
		PX_TextureFree(&pDesc->texture_stop);
		PX_TextureFree(&pDesc->texture_reset);

	}
}
PX_Object* PX_Object_Syntax_Create(px_memorypool* mp, PX_Object* Parent, px_int x, px_int y, px_int Width, px_int Height, PX_FontModule* fm)
{
	PX_Object* pObject;
	PX_Object_Syntax* pDesc;
	pObject = PX_ObjectCreateEx(mp, Parent, (px_float)x, (px_float)y,0, (px_float)Width, (px_float)Height,0, PX_OBJECT_TYPE_SYNTAX, PX_Object_Syntax_Update, PX_Object_Syntax_Render, PX_Object_Syntax_Free,0, sizeof(PX_Object_Syntax));
	pDesc = PX_ObjectGetDesc0(PX_Object_Syntax, pObject);
	if (!pDesc)
	{
		return PX_NULL;
	}
	pDesc->fm = fm;
	pDesc->mp = mp;
	pDesc->current_view_source_index = -1;
	pDesc->reg_last_break_row_index = -1;
	pDesc->area_tab = PX_Object_ScrollAreaCreate(mp, pObject, 0, 0, Width/2, 32);
	PX_Object_ScrollAreaSetHSliderBarHeight(pDesc->area_tab, 8);
	PX_VectorInitialize(mp, &pDesc->tab_buttons, sizeof(PX_Object*), 0);
	PX_VectorInitialize(mp, &pDesc->pages_memory, sizeof(PX_Object_Syntax_pagememory), 32);
	PX_VectorInitialize(mp, &pDesc->list_contents, sizeof(px_char *), 32);

	pDesc->list_info = PX_Object_ListCreate(mp, pObject, 0, (px_int)pDesc->area_tab->Height, Width / 4, (Height - (px_int)pDesc->area_tab->Height)/2, 20, PX_Object_Syntax_ListCreate, pObject);
	pDesc->tree_root_abi = PX_Object_TreeCreate(mp, pObject, 0, (px_int)(pDesc->area_tab->Height+ pDesc->list_info->Height), Width / 4, (Height - (px_int)pDesc->area_tab->Height) / 2, 20 ,fm);
	PX_ObjectRegisterEvent(pDesc->tree_root_abi, PX_OBJECT_EVENT_EXECUTE, PX_Object_Syntax_OnTreeExecute, pObject);

	pDesc->printer = PX_Object_PrinterCreate(mp, pObject, 0, 0, Width / 4 * 3, Height, fm);
	pDesc->source_code_viewer = PX_Object_Code_Create(mp, pObject, 0, 0, Width * 3 / 4, Height * 3 / 4, fm);
	PX_ObjectRegisterEvent(pDesc->source_code_viewer, PX_OBJECT_CODE_EVENT_TRIGGER_BREAKPOINT, PX_Object_Syntax_OnBreakpointTrigger, pObject);

	pDesc->textviewer_out=PX_Object_TextViewerCreate(mp, pObject, 0, 0, 200, Height, fm);

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
	PX_ObjectRegisterEvent(pDesc->button_run, PX_OBJECT_EVENT_EXECUTE, PX_Object_Syntax_OnSyntaxRun, pObject);

	pDesc->button_pause = PX_Object_PushButtonCreate(mp, pDesc->controller_panel, 28, 3, 26, 26, "", 0);
	PX_Object_PushButtonSetTexture(pDesc->button_pause, &pDesc->texture_pause);
	PX_ObjectRegisterEvent(pDesc->button_pause, PX_OBJECT_EVENT_EXECUTE, PX_Object_Syntax_OnSyntaxPause, pObject);

	pDesc->button_step = PX_Object_PushButtonCreate(mp, pDesc->controller_panel,  28*2, 3, 26, 26, "", 0);
	PX_Object_PushButtonSetTexture(pDesc->button_step, &pDesc->texture_step);
	PX_ObjectRegisterEvent(pDesc->button_step, PX_OBJECT_EVENT_EXECUTE, PX_Object_Syntax_OnSyntaxStep, pObject);

	pDesc->button_stop = PX_Object_PushButtonCreate(mp, pDesc->controller_panel, 28 * 3, 3, 26, 26, "", 0);
	PX_Object_PushButtonSetTexture(pDesc->button_stop, &pDesc->texture_stop);
	PX_ObjectRegisterEvent(pDesc->button_stop, PX_OBJECT_EVENT_EXECUTE, PX_Object_Syntax_OnSyntaxStop, pObject);

	pDesc->button_reset = PX_Object_PushButtonCreate(mp, pDesc->controller_panel, 28 * 4, 3, 26, 26, "", 0);
	PX_Object_PushButtonSetTexture(pDesc->button_reset, &pDesc->texture_reset);

	return pObject;
}

px_void PX_Object_Syntax_SetSyntax(PX_Object* pObject, PX_Syntax* psyntax)
{
	PX_Object_Syntax* pDesc = PX_ObjectGetDesc0(PX_Object_Syntax, pObject);
	if (!pDesc)
	{
		return;
	}
	pDesc->psyntax = psyntax;
	PX_Object_Code_SetSource(pDesc->source_code_viewer, PX_NULL);
	PX_Object_Syntax_UpdateTabButtons(pObject);
	
}

px_void PX_Object_Syntax_SetCurrentViewSourceIndex(PX_Object* pObject, px_int source_index)
{
	PX_Object_Syntax* pDesc = PX_ObjectGetDesc0(PX_Object_Syntax, pObject);
	if (!pDesc)
	{
		return;
	}
	if (!pDesc->psyntax)
	{
		return;
	}
	if (source_index>=0&&source_index<PX_Syntax_GetSourceCount(pDesc->psyntax))
	{
		pDesc->current_view_source_index = source_index;
		PX_Object_Code_SetSource(pDesc->source_code_viewer, PX_Syntax_GetSourceByIndex(pDesc->psyntax, source_index));
	}

}

