#include "PX_Object_Tree.h"

PX_OBJECT_EVENT_FUNCTION(PX_Object_TreeOnCursorMove)
{
	PX_Object_Tree* pDesc = PX_ObjectGetDescIndex(PX_Object_Tree, pObject, 0);
	px_int i; px_int cursor_index;
	px_float objx, objy, objWidth, objHeight, cursor_x, cursor_y;
	px_rect rect;
	if (!PX_ObjectIsCursorInRegion(pObject,e))
	{
		return;
	}
	rect = PX_ObjectGetRect(pObject);
	objx = rect.x;
	objy = rect.y;
	objWidth = rect.width;
	objHeight = rect.height;
	cursor_x = PX_Object_Event_GetCursorX(e) - objx;
	cursor_y = PX_Object_Event_GetCursorY(e) - objy;
	if (cursor_x >= pDesc->vslider->x)
	{
		return;
	}
	if (cursor_y >= pDesc->hslider->y)
	{
		return;
	}
	cursor_index = (px_int)((PX_Object_Event_GetCursorY(e) - objy) / pDesc->item_height + pDesc->yoffset);
	for (i = 0; i < pDesc->nodes.size; i++)
	{
		PX_Object_TreeNode* pNode = PX_VECTORAT(PX_Object_TreeNode, &pDesc->nodes, i);
		if (i == cursor_index)
		{
			pNode->bcursor = PX_TRUE;
		}
		else
		{
			pNode->bcursor = PX_FALSE;
		}
	}
}

px_int PX_Object_TreeForward(PX_Object* pObject, px_abi* prootabi, const px_char root_payload[], px_int y, px_int deep)
{
	
	PX_Object_Tree* pDesc = PX_ObjectGetDescIndex(PX_Object_Tree, pObject, 0);
	px_byte* pabi_byte_pointer;
	px_abi current_abi;
	px_dword datasize;
	px_bool* pexpand;
	if (root_payload[0] == '\0')
	{
		current_abi = *prootabi;
	}
	else
	{
		if (!PX_AbiGet_AbiReadOnly(prootabi, &current_abi, root_payload))
		{
			PX_ASSERTX("PX_Object_TreeForward: PX_AbiGet_Abi failed, payload not found");
			return y;
		}
	}
	pabi_byte_pointer = PX_AbiGet_First(&current_abi);
	datasize = PX_AbiGet_Size(&current_abi);

	while (pabi_byte_pointer)
	{
		if (y < pDesc->yoffset)
		{
			pabi_byte_pointer = PX_AbiGet_Next(&current_abi, pabi_byte_pointer);
			y++;
		}
		else
		{
			
			const px_char* pname;
			PX_ABI_TYPE type = PX_AbiPointer_GetType(pabi_byte_pointer);
			pname = PX_AbiPointer_GetName(pabi_byte_pointer);
			if (type == PX_ABI_TYPE_BOOL && PX_strequ(pname, "+-"))
			{
				//do nothing, this is a expand flag
			}
			else if (type == PX_ABI_TYPE_ABI)
			{
				PX_Object_TreeNode newnode = { 0 };
				px_char expand_payload[260] = { 0 };
				PX_strcat_s(expand_payload, sizeof(expand_payload), root_payload);
				PX_strcat_s(expand_payload, sizeof(expand_payload), ".");
				PX_strcat_s(expand_payload, sizeof(expand_payload), pname);
				PX_strcat_s(expand_payload, sizeof(expand_payload), ".+-");
				pexpand = PX_AbiGet_bool(prootabi, expand_payload);
				if (!PX_StringInitialize(pObject->mp, &newnode.content))
				{
					PX_ASSERTX("PX_Object_TreeForward: StringInitialize failed, out of memory?");
					return y;
				}
				if (!PX_StringInitialize(pObject->mp, &newnode.payload))
				{
					PX_ASSERTX("PX_Object_TreeForward: StringInitialize failed, out of memory?");
					PX_StringFree(&newnode.content);
					return y;
				}
				newnode.babi = PX_TRUE;
				newnode.deep = deep;
				if (root_payload[0])
				{
					PX_StringSet(&newnode.payload, root_payload);
					PX_StringCat(&newnode.payload, ".");
				}
				PX_StringCat(&newnode.payload, pname);
				if (pexpand && *pexpand)
				{
					px_char current_payload[260] = { 0 };
					PX_StringCat(&newnode.content, pname);
					if (root_payload[0])
					{
						PX_strcpy(current_payload, root_payload, sizeof(current_payload));
						PX_strcat_s(current_payload, sizeof(current_payload), ".");
					}
					PX_strcat_s(current_payload, sizeof(current_payload), pname);

					newnode.width = PX_FontModuleGetWidth(pDesc->fm, newnode.content.buffer);
					PX_VectorPushback(&pDesc->nodes, &newnode);
					y++;
					y = PX_Object_TreeForward(pObject, prootabi, current_payload, y, deep + 1);
				}
				else
				{
					PX_StringCat(&newnode.content, pname);
					newnode.width = PX_FontModuleGetWidth(pDesc->fm, newnode.content.buffer);
					y++;
					PX_VectorPushback(&pDesc->nodes, &newnode);
				}
			}
			else
			{
				PX_Object_TreeNode newnode = { 0 };
				newnode.babi = PX_FALSE;
				newnode.deep = deep;
				if(!PX_StringInitialize(pObject->mp, &newnode.content))
				{
					PX_ASSERTX("PX_Object_TreeForward: StringInitialize failed, out of memory?");
					return y;
				}
				if(!PX_StringInitialize(pObject->mp, &newnode.payload))
				{
					PX_ASSERTX("PX_Object_TreeForward: StringInitialize failed, out of memory?");
					PX_StringFree(&newnode.content);
					return y;
				}
				if (type == PX_ABI_TYPE_BOOL)
				{
					px_bool value = *(px_bool*)PX_AbiPointer_GetDataPointer(pabi_byte_pointer);
					if (pDesc->display_name)
					{
						PX_StringFormat2(&newnode.content, "%1:%2", PX_STRINGFORMAT_STRING(pname), PX_STRINGFORMAT_STRING(value ? "TRUE" : "FALSE"));
					}
					else
					{
						PX_StringFormat1(&newnode.content, "%1", PX_STRINGFORMAT_STRING(value ? "TRUE" : "FALSE"));
					}
				}
				else if (type == PX_ABI_TYPE_STRING)
				{
					const px_char* value = (px_char*)PX_AbiPointer_GetDataPointer(pabi_byte_pointer);
					if (pDesc->display_name)
					{
						PX_StringFormat2(&newnode.content, "%1:\"%2\"", PX_STRINGFORMAT_STRING(pname), PX_STRINGFORMAT_STRING(value));
					}
					else
					{
						PX_StringFormat1(&newnode.content, "%1", PX_STRINGFORMAT_STRING(value));
					}
				}
				else if (type == PX_ABI_TYPE_INT)
				{
					px_int value = *(px_int*)PX_AbiPointer_GetDataPointer(pabi_byte_pointer);
					if (pDesc->display_name)
					{
						PX_StringFormat2(&newnode.content, "%1:%2", PX_STRINGFORMAT_STRING(pname), PX_STRINGFORMAT_INT(value));
					}
					else
					{
						PX_StringFormat1(&newnode.content, "%1", PX_STRINGFORMAT_INT(value));
					}
				}
				else if (type == PX_ABI_TYPE_DWORD)
				{
					px_dword value = *(px_dword*)PX_AbiPointer_GetDataPointer(pabi_byte_pointer);
					if (pDesc->display_name)
					{
						PX_StringFormat2(&newnode.content, "%1:%2", PX_STRINGFORMAT_STRING(pname), PX_STRINGFORMAT_INT(value));
					}
					else
					{
						PX_StringFormat1(&newnode.content, "%1", PX_STRINGFORMAT_INT(value));
					}
				}
				else if (type == PX_ABI_TYPE_WORD)
				{
					px_word value = *(px_word*)PX_AbiPointer_GetDataPointer(pabi_byte_pointer);
					if (pDesc->display_name)
					{
						PX_StringFormat2(&newnode.content, "%1:%2", PX_STRINGFORMAT_STRING(pname), PX_STRINGFORMAT_INT(value));
					}
					else
					{
						PX_StringFormat1(&newnode.content, "%1", PX_STRINGFORMAT_INT(value));
					}
				}
				else if (type == PX_ABI_TYPE_BYTE)
				{
					px_byte value = *(px_byte*)PX_AbiPointer_GetDataPointer(pabi_byte_pointer);
					if (pDesc->display_name)
					{
						PX_StringFormat2(&newnode.content, "%1:%2", PX_STRINGFORMAT_STRING(pname), PX_STRINGFORMAT_INT(value));
					}
					else
					{
						PX_StringFormat1(&newnode.content, "%1", PX_STRINGFORMAT_INT(value));
					}
				}
				else if (type == PX_ABI_TYPE_DATA)
				{
					px_int data_size = PX_AbiPointer_GetDataSize(pabi_byte_pointer);
					if (pDesc->display_name)
					{
						PX_StringFormat2(&newnode.content, "%1:DATA:%2 bytes", PX_STRINGFORMAT_STRING(pname), PX_STRINGFORMAT_INT(data_size));
					}
					else
					{
						PX_StringFormat1(&newnode.content, "DATA:%1 bytes", PX_STRINGFORMAT_INT(data_size));
					}
				}
				else
				{
					if (pDesc->display_name)
					{
						PX_StringFormat2(&newnode.content, "%1:%2", PX_STRINGFORMAT_STRING(pname), PX_STRINGFORMAT_STRING("Unknown Type"));
					}
					else
					{
						PX_StringFormat1(&newnode.content, "%1", PX_STRINGFORMAT_STRING("undefined type"));
					}
				}
				if (root_payload[0])
				{
					PX_StringSet(&newnode.payload, root_payload);
					PX_StringCat(&newnode.payload, ".");
				}
				PX_StringCat(&newnode.payload, pname);
				newnode.width = PX_FontModuleGetWidth(pDesc->fm, newnode.content.buffer);
				PX_VectorPushback(&pDesc->nodes, &newnode);
				y++;
			}
			pabi_byte_pointer = PX_AbiGet_Next(&current_abi, pabi_byte_pointer);
		}
	}
	return y;
}

px_void PX_Object_TreeClearNodes(PX_Object* pObject)
{
	px_int i;
	PX_Object_Tree* pDesc = PX_ObjectGetDescIndex(PX_Object_Tree, pObject, 0);
	for (i = 0; i < pDesc->nodes.size; i++)
	{
		PX_Object_TreeNode* pNode = PX_VECTORAT(PX_Object_TreeNode, &pDesc->nodes, i);
		PX_StringFree(&pNode->content);
		PX_StringFree(&pNode->payload);
	}
	PX_VectorClear(&pDesc->nodes);
	pDesc->pcurrent_select_node = PX_NULL;
}

px_bool PX_Object_TreeReRender(PX_Object* pObject)
{
	PX_Object_Tree* pDesc = PX_ObjectGetDescIndex(PX_Object_Tree, pObject, 0);
	pDesc->ycount = (px_int)((pObject->Height + pDesc->item_height - 21) / pDesc->item_height);
	if (pDesc->render_target.width != pObject->Width-20 || pDesc->render_target.height != pObject->Height-20)
	{
		if (pDesc->render_target.mp)
		{
			PX_TextureFree(&pDesc->render_target);
			pDesc->render_target.mp = PX_NULL;
		}
		if (!PX_TextureCreate(pObject->mp, &pDesc->render_target, (px_int)pObject->Width-20, (px_int)pObject->Height-20))
		{
			return PX_FALSE;
		}
	}
	PX_Object_TreeClearNodes(pObject);
	PX_Object_TreeForward(pObject, &pDesc->tree_root_abi, "", pDesc->yoffset, 0);
	if (pDesc->nodes.size<=pDesc->ycount)
	{
		PX_ObjectSetEnabled(pDesc->vslider, PX_FALSE);
		PX_Object_SliderBarSetRange(pDesc->vslider, 0, 0);
	}
	else
	{
		px_int buttonlength = (px_int)(pDesc->vslider->Height * pDesc->ycount / pDesc->nodes.size);
		if (buttonlength<32)
		{
			buttonlength = 32;
		}
		PX_ObjectSetEnabled(pDesc->vslider, PX_TRUE);
		PX_Object_SliderBarSetRange(pDesc->vslider, 0, pDesc->nodes.size - pDesc->ycount+2);
		PX_Object_SliderBarSetSliderButtonLength(pDesc->vslider, buttonlength);
	}

	return PX_TRUE;
}

PX_OBJECT_EVENT_FUNCTION(PX_Object_TreeOnVSliderChanged)
{
	PX_Object_Tree* pDesc = PX_ObjectGetDescIndex(PX_Object_Tree, (PX_Object *)ptr, 0);
	px_int yoffset;
	yoffset = PX_Object_SliderBarGetValue(pDesc->vslider);
	if (yoffset != pDesc->yoffset)
	{
		pDesc->yoffset = yoffset;
	}
}
PX_OBJECT_EVENT_FUNCTION(PX_Object_TreeOnHSliderChanged)
{
	PX_Object_Tree* pDesc = PX_ObjectGetDescIndex(PX_Object_Tree, (PX_Object*)ptr, 0);
	px_int xoffset;
	xoffset = PX_Object_SliderBarGetValue(pDesc->hslider);
	if (xoffset != pDesc->xoffset)
	{
		pDesc->xoffset = xoffset;
	}
}

PX_OBJECT_EVENT_FUNCTION(PX_Object_TreeOnCursorWhell)
{
	PX_Object_Tree* pDesc = PX_ObjectGetDescIndex(PX_Object_Tree, (PX_Object*)ptr, 0);
	if (!PX_ObjectIsCursorInRegion(pObject,e))
	{
		return;
	}
	if (PX_Object_Event_GetCursorZ(e) > 0)
	{
		PX_Object_SliderBarSetValue(pDesc->vslider, PX_Object_SliderBarGetValue(pDesc->vslider) - 1);
	}
	else
	{
		PX_Object_SliderBarSetValue(pDesc->vslider, PX_Object_SliderBarGetValue(pDesc->vslider) + 1);
	}
	PX_ObjectExecuteEvent(pDesc->vslider, PX_OBJECT_BUILD_EVENT_INT(PX_OBJECT_EVENT_VALUECHANGED, PX_Object_SliderBarGetValue(pDesc->vslider)));
	
}

PX_OBJECT_EVENT_FUNCTION(PX_Object_TreeOnCursorDown)
{
	PX_Object_Tree* pDesc = PX_ObjectGetDescIndex(PX_Object_Tree, pObject, 0);
	px_int i; px_int cursor_index;
	px_float objx, objy, objWidth, objHeight;
	px_float cursor_x, cursor_y;
	px_rect rect;
	if (!PX_ObjectIsCursorInRegion(pObject, e))
	{
		return;
	}
	
	rect = PX_ObjectGetRect(pObject);
	objx = rect.x;
	objy = rect.y;
	objWidth = rect.width;
	objHeight = rect.height;
	cursor_x = PX_Object_Event_GetCursorX(e)- objx;
	cursor_y = PX_Object_Event_GetCursorY(e) - objy;
	if (cursor_x>=pDesc->vslider->x)
	{
		return;
	}
	if (cursor_y>=pDesc->hslider->y)
	{
		return;
	}
	cursor_index = (px_int)(cursor_y) / pDesc->item_height + pDesc->yoffset;
	pDesc->pcurrent_select_node = PX_NULL;
	for (i = 0; i < pDesc->nodes.size; i++)
	{
		
		if (i == cursor_index)
		{
			px_char payload[260] = {0};
			PX_Object_TreeNode* pNode = PX_VECTORAT(PX_Object_TreeNode, &pDesc->nodes, i);
			pDesc->pcurrent_select_node = pNode;
			if (pNode->babi)
			{
				PX_sprintf1(payload, sizeof(payload), "%1.+-", PX_STRINGFORMAT_STRING(pNode->payload.buffer));
				px_bool* pexpand = PX_AbiGet_bool(&pDesc->tree_root_abi, payload);
				if (pexpand)
				{
					if (!PX_AbiSet_bool(&pDesc->tree_root_abi, payload, !(*pexpand)))
					{
						return;
					}
				}
				else
				{
					if (!PX_AbiSet_bool(&pDesc->tree_root_abi, payload, PX_TRUE))
					{
						return;
					}
				}
				pDesc->need_rerender = PX_TRUE;
			}
			
			do
			{
				PX_Object_Event e = { 0 };
				e.Event = PX_OBJECT_EVENT_EXECUTE;
				PX_Object_Event_SetStringPtr(&e, pNode->payload.buffer);
				PX_ObjectExecuteEvent(pObject, e);
			} while (0);
			
			return;
		}
	}
}




PX_OBJECT_RENDER_FUNCTION(PX_Object_TreeRender)
{
	px_int i;
	PX_Object_Tree* pDesc = PX_ObjectGetDesc(PX_Object_Tree, pObject);
	px_float objx, objy, objWidth, objHeight;
	px_rect rect;
	px_int max_font_width=0;
	px_int current_font_width;
	if (pDesc->need_rerender)
	{
		PX_Object_TreeReRender(pObject);
		pDesc->need_rerender = PX_FALSE;
	}

	rect = PX_ObjectGetRect(pObject);
	objx = rect.x;
	objy = rect.y;
	objWidth = rect.width;
	objHeight = rect.height;

	pDesc->hslider->x = 0;
	pDesc->hslider->y =  objHeight - pDesc->hslider->Height;
	pDesc->hslider->Width = objWidth;

	pDesc->vslider->x = objWidth - pDesc->vslider->Width;
	pDesc->vslider->y = 0;
	pDesc->vslider->Height = objHeight-20;

	if (pDesc->render_target.width != (px_int)objWidth-20 || pDesc->render_target.height != (px_int)objHeight-20)
	{
		if (!PX_Object_TreeReRender(pObject))
		{
			return;
		}
	}
	PX_SurfaceClearAll(&pDesc->render_target, pDesc->background_color);
	//render items
	for (i = pDesc->yoffset; i < pDesc->nodes.size; i++)
	{
		if (i>pDesc->yoffset+pDesc->ycount)
		{
			break;
		}
		else
		{
			PX_Object_TreeNode* pNode = PX_VECTORAT(PX_Object_TreeNode, &pDesc->nodes, i);
			px_float x = pNode->deep * 20.f + 10-pDesc->xoffset;
			px_float y = (i- pDesc->yoffset) * pDesc->item_height * 1.f;
			px_int len, woffset = 0;
			px_char build_content[96] = { 0 };
			const px_char* pcontent;
			if (pNode->bcursor)
			{
				px_color color = pDesc->select_color;
				color._argb.a /= 2;
				PX_GeoDrawRect(&pDesc->render_target, 0, (px_int)y, (px_int)pObject->Width, (px_int)y + pDesc->item_height, color);
			}
			if (pNode==pDesc->pcurrent_select_node)
			{
				PX_GeoDrawRect(&pDesc->render_target, 0, (px_int)y, (px_int)pObject->Width, (px_int)y + pDesc->item_height, pDesc->select_color);

			}

			pcontent = pNode->content.buffer;
			for (len = 0; len < 32; len++)
			{
				if (pcontent[len] == '\0')
				{
					break;
				}
				if (pcontent[len] == '\r')
				{
					build_content[woffset++] = '.';
					build_content[woffset++] = '.';
					build_content[woffset++] = '.';
					break;
				}
				if (pcontent[len] == '\n')
				{
					build_content[woffset++] = '.';
					build_content[woffset++] = '.';
					build_content[woffset++] = '.';
					break;
				}
				build_content[woffset++] = pcontent[len];
			}

			if (pNode->babi)
			{
				px_bool* pexpand;
				px_char payload[260] = { 0 };
				


				PX_strcat_s(payload, sizeof(payload), pNode->payload.buffer);
				PX_strcat_s(payload, sizeof(payload), ".+-");
				pexpand = PX_AbiGet_bool(&pDesc->tree_root_abi, payload);
				if (pexpand && *pexpand)
					PX_GeoDrawDownTriangle(&pDesc->render_target, x + 4, y + pDesc->item_height / 2 - 4, \
						x + 12, y + pDesc->item_height / 2 + 4,
						pDesc->text_color);
				else
					PX_GeoDrawRightTriangle(&pDesc->render_target, x + 4, y + pDesc->item_height / 2 - 4, \
						x + 12, y + pDesc->item_height / 2 + 4,
						pDesc->text_color);

				

				current_font_width=PX_FontModuleDrawText(&pDesc->render_target,
					pDesc->fm,
					(px_int)x + 18, (px_int)y + (px_int)pDesc->item_height / 2,
					PX_ALIGN_LEFTMID,
					build_content,
					pDesc->text_color
				);
				if (current_font_width+ pNode->deep * 20.f  + 64 > max_font_width)
				{
					max_font_width = current_font_width+ pNode->deep * 20  + 64;
				}
			}
			else
			{
				current_font_width=PX_FontModuleDrawText(&pDesc->render_target,
					pDesc->fm,
					(px_int)x + 18, (px_int)y + (px_int)pDesc->item_height / 2,
					PX_ALIGN_LEFTMID,
					build_content,
					pDesc->text_color
				);
				if (current_font_width + pNode->deep * 20.f  + 64 > max_font_width)
				{
					max_font_width = current_font_width + pNode->deep * 20  + 64;
				}
			}
		}
	}

	if (max_font_width<objWidth)
	{
		PX_ObjectSetEnabled(pDesc->hslider, PX_FALSE);
		PX_Object_SliderBarSetRange(pDesc->hslider, 0, 0);

	}else if (PX_Object_SliderBarGetMax(pDesc->hslider)!= max_font_width- (px_int)objWidth)
	{
		px_int buttonLenght = (px_int)(pDesc->hslider->Width * objWidth / (max_font_width));
		PX_ObjectSetEnabled(pDesc->hslider, PX_TRUE);
		PX_Object_SliderBarSetRange(pDesc->hslider, 0, max_font_width - (px_int)objWidth);
		if(buttonLenght<32)
		{
			buttonLenght = 32;
		}
		PX_Object_SliderBarSetSliderButtonLength(pDesc->hslider, buttonLenght);
	}
	//draw border
	PX_GeoDrawBorder(&pDesc->render_target, 0, 0, pDesc->render_target.width - 1, pDesc->render_target.height - 1, 1, pDesc->border_color);
	PX_TextureRender(psurface, &pDesc->render_target, (px_int)objx, (px_int)objy, PX_ALIGN_LEFTTOP, PX_NULL);
}

PX_OBJECT_FREE_FUNCTION(PX_Object_TreeFree)
{
	PX_Object_Tree* pDesc = PX_ObjectGetDesc(PX_Object_Tree, pObject);
	if (pDesc->render_target.mp)
	{
		PX_TextureFree(&pDesc->render_target);
		pDesc->render_target.mp = PX_NULL;
	}
	if(pDesc->tree_root_abi.dynamic.mp)
		PX_AbiFree(&pDesc->tree_root_abi);
	PX_Object_TreeClearNodes(pObject);
	PX_VectorFree(&pDesc->nodes);


}

PX_Object* PX_Object_TreeCreate(px_memorypool* mp, PX_Object* Parent, px_int x, px_int y, px_int Width, px_int Height, px_int item_height, PX_FontModule* fm)
{
	PX_Object_Tree* pDesc;
	PX_Object* pObject = PX_ObjectCreateEx(mp, Parent, (px_float)x, (px_float)y,0, (px_float)Width, (px_float)Height,0, PX_OBJECT_TYPE_TREE,0, PX_Object_TreeRender, PX_Object_TreeFree, 0,sizeof(PX_Object_Tree));
	if (!pObject)
	{
		return PX_NULL;
	}
	pDesc = PX_ObjectGetDescIndex(PX_Object_Tree, pObject,0);
	pDesc->fm = fm;
	pDesc->yoffset = 0;
	pDesc->text_color = PX_COLOR_FONTCOLOR;
	pDesc->select_color = PX_COLOR_CURSORCOLOR;
	pDesc->border_color = PX_COLOR_BORDERCOLOR;
	pDesc->background_color = PX_COLOR_BACKGROUNDCOLOR;
	pDesc->display_name = PX_TRUE;
	pDesc->hslider = PX_Object_SliderBarCreate(mp, pObject, 0, 0, Width, 20,PX_OBJECT_SLIDERBAR_TYPE_HORIZONTAL,PX_OBJECT_SLIDERBAR_STYLE_BOX);
	pDesc->vslider = PX_Object_SliderBarCreate(mp, pObject, 0, 0, 20, Height, PX_OBJECT_SLIDERBAR_TYPE_VERTICAL, PX_OBJECT_SLIDERBAR_STYLE_BOX);
	pDesc->item_height = item_height;
	PX_VectorInitialize(mp, &pDesc->nodes, sizeof(PX_Object_TreeNode), 16);
	if (!PX_TextureCreate(pObject->mp, &pDesc->render_target, (px_int)pObject->Width-20, (px_int)pObject->Height-20))
	{
		return PX_NULL;
	}
	PX_AbiCreate_DynamicWriter(&pDesc->tree_root_abi, mp);
	PX_ObjectRegisterEvent(pObject, PX_OBJECT_EVENT_CURSORMOVE, PX_Object_TreeOnCursorMove, pObject);
	PX_ObjectRegisterEvent(pObject, PX_OBJECT_EVENT_CURSORDOWN, PX_Object_TreeOnCursorDown, pObject);
	PX_ObjectRegisterEvent(pObject, PX_OBJECT_EVENT_CURSORWHEEL, PX_Object_TreeOnCursorWhell, pObject);
	PX_ObjectRegisterEvent(pDesc->vslider, PX_OBJECT_EVENT_VALUECHANGED, PX_Object_TreeOnVSliderChanged, pObject);
	PX_ObjectRegisterEvent(pDesc->hslider, PX_OBJECT_EVENT_VALUECHANGED, PX_Object_TreeOnHSliderChanged, pObject);
	PX_Object_TreeReRender(pObject);
	return pObject;
}

px_bool PX_Object_TreeSetAbi(PX_Object* pObject, px_abi* pAbi)
{
	PX_Object_Tree* pDesc = PX_ObjectGetDescIndex(PX_Object_Tree, pObject, 0);
	if (PX_AbiCopy_FromAbi(&pDesc->tree_root_abi,pAbi))
	{
		PX_Object_TreeReRender(pObject);
		return PX_TRUE;
	}
	else
	{
		PX_ASSERTX("PX_Object_TreeSetAbi: Abi copy failed, out of memory?");
		return PX_FALSE;
	}
}

px_void PX_Object_TreeClear(PX_Object* pObject)
{
	PX_Object_Tree* pDesc = PX_ObjectGetDescIndex(PX_Object_Tree, pObject, 0);
	PX_Object_TreeClearNodes(pObject);
	PX_AbiClear(&pDesc->tree_root_abi);
	pDesc->yoffset = 0;
	pDesc->xoffset = 0;
	pDesc->display_name = PX_TRUE;
	pDesc->need_rerender = PX_TRUE;
	PX_Object_TreeReRender(pObject);
	
}

PX_Object_TreeNode* PX_Object_TreeGetCurrentSelectNode(PX_Object* pObject)
{
	PX_Object_Tree* pDesc = PX_ObjectGetDescIndex(PX_Object_Tree, pObject, 0);
	return pDesc->pcurrent_select_node;
	
}

px_bool PX_Object_TreeAddAbi(PX_Object* pObject,const px_char name[], px_abi* pAbi)
{
	PX_Object_Tree* pDesc = PX_ObjectGetDescIndex(PX_Object_Tree, pObject, 0);
	if (PX_AbiSet_Abi(&pDesc->tree_root_abi, name, pAbi))
	{
		pDesc->need_rerender = PX_TRUE;
		return PX_TRUE;
	}
	else
	{
		PX_ASSERTX("PX_Object_TreeSetAbi: Abi copy failed, out of memory?");
		return PX_FALSE;
	}
}