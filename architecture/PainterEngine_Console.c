#include "PainterEngine_Console.h"



static px_char const PC_ScriptPreload[]="#name \"shell\"\r\n\
							  #runtime stack 1024\r\n\
							  host int print(string s);\r\n\
							  host int printimage(string s);\r\n\
							  host int printanimation(string s);\r\n\
							  host int playanimation(int id,string s);\r\n\
							  host int runscriptfunction(string key,string func);\r\n\
							  host int close();\r\n";



px_void PX_ConsoleUpdateEx(PX_Console *pc)
{
	px_int i,y=10;
	PX_ConsoleColumn *pCc;

	while (pc->pObjects.size>pc->max_column)
	{
		pCc=(PX_VECTORAT(PX_ConsoleColumn,&pc->pObjects,0));
		PX_ObjectDelete(pCc->Object);
		PX_VectorErase(&pc->pObjects,0);
	}

	for (i=0;i<pc->pObjects.size;i++)
	{
		pCc=(PX_VECTORAT(PX_ConsoleColumn,&pc->pObjects,i));

		switch(pCc->Object->Type)
		{
		case PX_OBJECT_TYPE_AUTOTEXT:
			{
				PX_ObjectSetPosition(pCc->Object,0,(px_float)y,0);
				y+=PX_Object_AutoTextGetHeight(pCc->Object);
			}
			break;
		case PX_OBJECT_TYPE_IMAGE:
			{
				PX_ObjectSetPosition(pCc->Object,0,(px_float)y,0);
				y+=(px_int)PX_ObjectGetHeight(pCc->Object);
			}
			break;
		case PX_OBJECT_TYPE_ANIMATION:
			{
				PX_ObjectSetPosition(pCc->Object,0,(px_float)y,0);
				y+=(px_int)PX_ObjectGetHeight(pCc->Object);
			}
			break;
		default:
			{
				PX_ObjectSetPosition(pCc->Object,0,(px_float)y,0);
				y+=(px_int)PX_ObjectGetHeight(pCc->Object);
			}
			break;
		}
	}
	PX_ObjectSetPosition(pc->Input,0,(px_float)y,0);
}
PX_Object * PX_ConsolePrintText(PX_Console *pc,const px_char *text)
{
	PX_ConsoleColumn obj;
	PX_Object *pObject=PX_Object_AutoTextCreate(&pc->runtime->mp_ui,pc->Area,0,0,(px_int)pc->Area->Width-1,PX_NULL);

	if (pObject)
	{
		obj.Object=pObject;
		obj.id=pc->id++;
		PX_Object_AutoTextSetTextColor(pObject,PX_COLOR(255,0,255,0));
		PX_Object_AutoTextSetText(pObject,text);
		PX_VectorPushback(&pc->pObjects,&obj);
		PX_ConsoleUpdateEx(pc);

		PX_Object_ScrollAreaMoveToBottom(pc->Area);
	}
	return pObject;
}

PX_Object * PX_ConsolePrintImage(PX_Console *pc,px_char *res_image_key)
{
	PX_ConsoleColumn obj;
	PX_Resource *pimageRes;
	PX_Object *pObject;
	if((pimageRes=PX_ResourceLibraryGet(&pc->runtime->ResourceLibrary,res_image_key))!=PX_NULL)
	{
		if (pimageRes->Type==PX_RESOURCE_TYPE_TEXTURE)
		{
			pObject=PX_Object_ImageCreate(&pc->runtime->mp_ui,pc->Area,0,0,pimageRes->texture.width,pimageRes->texture.height,&pimageRes->texture);
			PX_Object_ImageSetAlign(pObject,PX_ALIGN_LEFTTOP);
			PX_ObjectSetSize(pObject,(px_float)pimageRes->texture.width,(px_float)pimageRes->texture.height,0);
			obj.Object=pObject;
			obj.id=pc->id++;
			PX_VectorPushback(&pc->pObjects,&obj);
			PX_ConsoleUpdateEx(pc);
			PX_Object_ScrollAreaMoveToBottom(pc->Area);
		}
		else
			return PX_NULL;
	}
	else
	{
		return PX_NULL;
	}

	return pObject;
}



PX_Object * PX_ConsolePrintAnimation(PX_Console *pc,px_char *res_animation_key)
{
	PX_ConsoleColumn obj;
	PX_Resource *pAnimationRes;
	PX_Object *pObject;
	px_rect rect;
	if((pAnimationRes=PX_ResourceLibraryGet(&pc->runtime->ResourceLibrary,res_animation_key))!=PX_NULL)
	{
		if (pAnimationRes->Type==PX_RESOURCE_TYPE_ANIMATIONLIBRARY)
		{
			pObject=PX_Object_AnimationCreate(&pc->runtime->mp_ui,pc->Area,0,0,&pAnimationRes->animationlibrary);
			PX_Object_AnimationSetAlign(pObject,PX_ALIGN_LEFTTOP);
			rect=PX_AnimationGetSize(&PX_Object_GetAnimation(pObject)->animation);
			PX_ObjectSetSize(pObject,(px_float)rect.width,(px_float)rect.height,0);
			obj.Object=pObject;
			obj.id=pc->id++;
			PX_VectorPushback(&pc->pObjects,&obj);
			PX_ConsoleUpdateEx(pc);
			PX_Object_ScrollAreaMoveToBottom(pc->Area);
		}
		else
			return PX_NULL;
	}
	else
	{
		return PX_NULL;
	}

	return pObject;
}




PX_Object * PX_ConsoleShowImage(PX_Console *pc,px_char *res_image_key)
{
	PX_ConsoleColumn obj;
	PX_Resource *pimageRes;
	PX_Object *pObject;
	if((pimageRes=PX_ResourceLibraryGet(&pc->runtime->ResourceLibrary,res_image_key))!=PX_NULL)
	{
		if (pimageRes->Type==PX_RESOURCE_TYPE_TEXTURE)
		{
			pObject=PX_Object_ImageCreate(&pc->runtime->mp_ui,pc->Area,0,0,pimageRes->texture.width,pimageRes->texture.height,&pimageRes->texture);
			PX_Object_ImageSetAlign(pObject,PX_ALIGN_LEFTTOP);
			obj.Object=pObject;
			obj.id=pc->id++;
			PX_VectorPushback(&pc->pObjects,&obj);
			PX_ConsoleUpdateEx(pc);
			PX_Object_ScrollAreaMoveToBottom(pc->Area);
		}
		else
			return PX_NULL;
	}
	else
	{
		return PX_NULL;
	}

	return pObject;
}

px_void  PX_ConsolePlayAnimation(PX_Console *pc,px_int id,px_char *animation_name)
{
	px_int i;
	PX_ConsoleColumn *pCc;

	for (i=0;i<pc->pObjects.size;i++)
	{
		pCc=PX_VECTORAT(PX_ConsoleColumn,&pc->pObjects,i);
		if(pCc->id==id)
		{
			if (pCc->Object->Type==PX_OBJECT_TYPE_ANIMATION)
			{
				PX_AnimationSetCurrentPlayAnimationByName(&PX_Object_GetAnimation(pCc->Object)->animation,animation_name);
				return;
			}
		}
	}
}

px_void  PX_ConsoleSetImageMask(PX_Console *pc,px_int id,px_char *mask_key)
{
	px_int i;
	PX_ConsoleColumn *pCc;

	for (i=0;i<pc->pObjects.size;i++)
	{
		pCc=PX_VECTORAT(PX_ConsoleColumn,&pc->pObjects,i);
		if(pCc->id==id)
		{
			if (pCc->Object->Type==PX_OBJECT_TYPE_IMAGE)
			{
				PX_Resource *pimageRes;
				if((pimageRes=PX_ResourceLibraryGet(&pc->runtime->ResourceLibrary,mask_key))!=PX_NULL)
				{
					if (pimageRes->Type==PX_RESOURCE_TYPE_TEXTURE)
					{
						PX_Object_ImageSetMask(pCc->Object,&pimageRes->texture);
					}
				}
				
			}
			return;
		}
	}
}




px_bool PX_ConsoleVM_Print(PX_ScriptVM_Instance *Ins,px_void *userptr)
{
	PX_Console *pc=(PX_Console *)userptr;

	if (PX_ScriptVM_STACK(Ins,0).type!=PX_SCRIPTVM_VARIABLE_TYPE_STRING)
	{
		PX_ScriptVM_RET(Ins,PX_ScriptVM_Variable_int(0));
		return PX_TRUE;
	}
	PX_ConsolePrintText(pc,PX_ScriptVM_STACK(Ins,0)._string.buffer);
	PX_ScriptVM_RET(Ins,PX_ScriptVM_Variable_int(PX_VECTORAT(PX_ConsoleColumn,&pc->pObjects,pc->pObjects.size-1)->id));
	return PX_TRUE;
}

px_bool PX_ConsoleVM_PrintImage(PX_ScriptVM_Instance *Ins,px_void *userptr)
{
	PX_Console *pc=(PX_Console *)userptr;

	if (PX_ScriptVM_STACK(Ins,0).type!=PX_SCRIPTVM_VARIABLE_TYPE_STRING)
	{
		PX_ScriptVM_RET(Ins,PX_ScriptVM_Variable_int(0));
		return PX_TRUE;
	}
	PX_ConsolePrintImage(pc,PX_ScriptVM_STACK(Ins,0)._string.buffer);
	PX_ScriptVM_RET(Ins,PX_ScriptVM_Variable_int(PX_VECTORAT(PX_ConsoleColumn,&pc->pObjects,pc->pObjects.size-1)->id));
	return PX_TRUE;
}


px_bool PX_ConsoleVM_PrintAnimation(PX_ScriptVM_Instance *Ins,px_void *userptr)
{
	PX_Console *pc=(PX_Console *)userptr;
	if (PX_ScriptVM_STACK(Ins,0).type!=PX_SCRIPTVM_VARIABLE_TYPE_STRING)
	{
		PX_ScriptVM_RET(Ins,PX_ScriptVM_Variable_int(0));
		return PX_TRUE;
	}
	PX_ConsolePrintAnimation(pc,PX_ScriptVM_STACK(Ins,0)._string.buffer);
	PX_ScriptVM_RET(Ins,PX_ScriptVM_Variable_int(PX_VECTORAT(PX_ConsoleColumn,&pc->pObjects,pc->pObjects.size-1)->id));
	return PX_TRUE;
}

px_bool PX_ConsoleVM_PlayAnimation(PX_ScriptVM_Instance *Ins,px_void *userptr)
{
	PX_Console *pc=(PX_Console *)userptr;

	if (PX_ScriptVM_STACK(Ins,0).type!=PX_SCRIPTVM_VARIABLE_TYPE_INT)
	{
		PX_ScriptVM_RET(Ins,PX_ScriptVM_Variable_int(0));
		return PX_TRUE;
	}

	if (PX_ScriptVM_STACK(Ins,1).type!=PX_SCRIPTVM_VARIABLE_TYPE_STRING)
	{
		PX_ScriptVM_RET(Ins,PX_ScriptVM_Variable_int(0));
		return PX_TRUE;
	}

	PX_ConsolePlayAnimation(pc,PX_ScriptVM_STACK(Ins,0)._int,PX_ScriptVM_STACK(Ins,1)._string.buffer);
	
	return PX_TRUE;
}


px_bool PX_ConsoleVM_Close(PX_ScriptVM_Instance *Ins,px_void *userptr)
{
	PX_Console *pc=(PX_Console *)userptr;
	PX_ConsoleShow(pc,PX_FALSE);
	return PX_TRUE;
}




px_void  PX_ConsoleRunScriptFunction(PX_Console *pc,px_char *script_key,px_char *func_name)
{
	PX_Resource *pScriptRes;
	if((pScriptRes=PX_ResourceLibraryGet(&pc->runtime->ResourceLibrary,script_key))!=PX_NULL)
	{
		if (pScriptRes->Type==PX_RESOURCE_TYPE_SCRIPT)
		{
			PX_ScriptVM_RegistryHostFunction(&pScriptRes->Script,"PRINT",PX_ConsoleVM_Print,pc);//Print
			PX_ScriptVM_RegistryHostFunction(&pScriptRes->Script,"PRINTIMAGE",PX_ConsoleVM_PrintImage,pc);//Print Image
			PX_ScriptVM_RegistryHostFunction(&pScriptRes->Script,"PRINTANIMATION",PX_ConsoleVM_PrintAnimation,pc);//Print Animation
			if(!PX_ScriptVM_InstanceRunFunction(&pScriptRes->Script,0,"_BOOT",PX_NULL,0))
			{
				return;
			}
			if(!PX_ScriptVM_InstanceRunFunction(&pScriptRes->Script,0,func_name,PX_NULL,0))
			{
				return ;
			}
		}
		else
			return;
	}
	else
	{
		return;
	}

}

px_bool PC_ConsoleVM_RunScriptFunction(PX_ScriptVM_Instance *Ins,px_void *userptr)
{
	PX_Console *pc=(PX_Console *)userptr;

	if (PX_ScriptVM_STACK(Ins,0).type!=PX_SCRIPTVM_VARIABLE_TYPE_STRING)
	{
		PX_ScriptVM_RET(Ins,PX_ScriptVM_Variable_int(0));
		return PX_TRUE;
	}

	if (PX_ScriptVM_STACK(Ins,1).type!=PX_SCRIPTVM_VARIABLE_TYPE_STRING)
	{
		PX_ScriptVM_RET(Ins,PX_ScriptVM_Variable_int(0));
		return PX_TRUE;
	}

	PX_ConsoleRunScriptFunction(pc,PX_ScriptVM_STACK(Ins,0)._string.buffer,PX_ScriptVM_STACK(Ins,1)._string.buffer);

	return PX_TRUE;
}

px_bool PX_ConsoleExecute(PX_Console *pc,px_char *pshellstr)
{
	px_memory bin;
	px_string shell;
	PX_ScriptVM_Instance Ins;
	PX_SCRIPT_LIBRARY lib;
	px_string asmcodeString;
	px_memorypool mp_calc;

	mp_calc=MP_Create(MP_Malloc(&pc->runtime->mp,PE_MEMORY_CALC_SIZE),PE_MEMORY_CALC_SIZE);
	if(mp_calc.StartAddr==PX_NULL) return PX_FALSE;
	PX_ConsolePrintText(pc,pshellstr);
	MP_Reset(&mp_calc);
	PX_MemoryInitialize(&mp_calc,&bin);
	PX_StringInitialize(&mp_calc,&shell);
	PX_StringCat(&shell,(px_char *)PC_ScriptPreload);
	if(pc->script_header_append)
	PX_StringCat(&shell,(px_char *)pc->script_header_append);
	PX_StringCat(&shell,"export void main(){\r\n");
	PX_StringCat(&shell,pshellstr);
	PX_StringCat(&shell,"\r\n}");

	if(!PX_ScriptCompilerInitialize(&lib,&mp_calc))
	{
		MP_Free(&pc->runtime->mp,mp_calc.StartAddr);
		return PX_FALSE;
	}

	if(!PX_ScriptCompilerLoad(&lib,(px_char *)shell.buffer))
	{
		MP_Free(&pc->runtime->mp,mp_calc.StartAddr);
		return PX_FALSE;
	}

	PX_StringFree(&shell);

	PX_StringInitialize(&mp_calc,&asmcodeString);
	if(PX_ScriptCompilerCompile(&lib,"shell",&asmcodeString,32))
	{
		PX_ScriptAsmOptimization(&asmcodeString);

		if(!PX_ScriptAsmCompile(&mp_calc,asmcodeString.buffer,&bin))
		{
			MP_Free(&pc->runtime->mp,mp_calc.StartAddr);
			return PX_FALSE;
		}
	}
	else
	{
		MP_Free(&pc->runtime->mp,mp_calc.StartAddr);
		return PX_FALSE;
	}

	PX_StringFree(&asmcodeString);
	PX_ScriptCompilerFree(&lib);

	if(!PX_ScriptVM_InstanceInitialize(&Ins,&mp_calc,bin.buffer,bin.usedsize))
	{
		MP_Free(&pc->runtime->mp,mp_calc.StartAddr);
		return PX_FALSE;
	}
	PX_MemoryFree(&bin);
	
	PX_ScriptVM_RegistryHostFunction(&Ins,"PRINT",PX_ConsoleVM_Print,pc);//Print
	PX_ScriptVM_RegistryHostFunction(&Ins,"PRINTIMAGE",PX_ConsoleVM_PrintImage,pc);//Print Image
	PX_ScriptVM_RegistryHostFunction(&Ins,"PRINTANIMATION",PX_ConsoleVM_PrintAnimation,pc);//Print Animation
	PX_ScriptVM_RegistryHostFunction(&Ins,"PLAYANIMATION",PX_ConsoleVM_PlayAnimation,pc);//play Animation
	PX_ScriptVM_RegistryHostFunction(&Ins,"RUNSCRIPTFUNCTION",PC_ConsoleVM_RunScriptFunction,pc);//Load PatricalScript
	PX_ScriptVM_RegistryHostFunction(&Ins,"CLOSE",PX_ConsoleVM_Close,pc);//close
	if (pc->registry_call)
	{
		pc->registry_call(&Ins);
	}
	if(!PX_ScriptVM_InstanceRunFunction(&Ins,0,"_BOOT",PX_NULL,0))
	{
		MP_Free(&pc->runtime->mp,mp_calc.StartAddr);
		return PX_FALSE;
	}
	if(!PX_ScriptVM_InstanceRunFunction(&Ins,0,"MAIN",PX_NULL,0))
	{
		MP_Free(&pc->runtime->mp,mp_calc.StartAddr);
		return PX_FALSE;
	}


	PX_ScriptVM_InstanceFree(&Ins);
#if defined(PX_DEBUG_MODE) && defined(PX_MEMORYPOOL_DEBUG_CHECK)
	MP_UnreleaseInfo(&mp_calc);
#endif
	
	MP_Free(&pc->runtime->mp,mp_calc.StartAddr);

	return PX_TRUE;

}

px_void PX_ConsoleOnEnter(PX_Object *Obj,PX_Object_Event e,px_void *user_ptr)
{
	PX_Console *pc=(PX_Console *)Obj->User_ptr;

	PX_Object_Edit *pEdit=PX_Object_GetEdit(pc->Input);

	if (e.Event==PX_OBJECT_EVENT_KEYDOWN)
	{
		if (PX_Object_Event_GetKeyDown(e)=='\r')
		{
			if(!PX_ConsoleExecute(pc,pEdit->text.buffer))
			{
				PX_Object_AutoTextSetTextColor(PX_ConsolePrintText(pc,"Invalid shell code."),PX_COLOR(255,255,0,0));
			}
			PX_Object_EditSetText(pc->Input,"");
		}
	}

	if (e.Event==PX_OBJECT_EVENT_CURSORRDOWN)
	{
		PX_Object_EditSetFocus(pc->Input,PX_TRUE);
	}
}

px_void PX_ConsoleOnMouseDown(PX_Object *Obj,PX_Object_Event e,px_void *user_ptr)
{
	PX_Console *pc=(PX_Console *)Obj->User_ptr;
	if (e.Event==PX_OBJECT_EVENT_CURSORDOWN)
	{
		PX_Object_EditSetFocus(pc->Input,PX_TRUE);
	}
}


px_bool PX_ConsoleInitializeEx(PX_Runtime *runtime,PX_Console *pc,px_int x,px_int y,px_int width,px_int height)
{
	px_char const fox_console_logo[] = \
	{
#include "PainterEngine_FoxLogo.h"
	};
	//console initialize
	pc->runtime=runtime;
	pc->registry_call=PX_NULL;
	pc->script_header_append=PX_NULL;

	pc->show=PX_TRUE;
	pc->max_column=PC_CONSOLE_DEFAULT_MAX_COLUMN;
	pc->column=0;
	if(!(pc->Root=PX_ObjectCreate(&pc->runtime->mp_ui,0,0,0,0,0,0,0))) return PX_FALSE;
	if(!(pc->Area=PX_Object_ScrollAreaCreate(&pc->runtime->mp_ui,pc->Root,x,y,width,height))) return PX_FALSE;


	pc->Area->User_ptr=pc;
	PX_ObjectRegisterEvent(pc->Area,PX_OBJECT_EVENT_KEYDOWN,PX_ConsoleOnEnter,PX_NULL);
	PX_ObjectRegisterEvent(pc->Area,PX_OBJECT_EVENT_CURSORDOWN,PX_ConsoleOnMouseDown,PX_NULL);
	PX_Object_ScrollAreaSetBorder(pc->Area,PX_FALSE);
	if(!(pc->Input=PX_Object_EditCreate(&pc->runtime->mp_ui,PX_Object_ScrollAreaGetIncludedObjects(pc->Area),0,0,width-1,PX_FontGetCharactorHeight()+4,PX_NULL))) return PX_FALSE;
	PX_Object_EditSetTextColor(pc->Input,PX_COLOR(255,0,255,0));
	PX_Object_EditSetCursorColor(pc->Input,PX_COLOR(255,0,255,0));
	PX_Object_EditSetTextColor(pc->Input,PX_COLOR(255,0,255,0));
	PX_Object_EditSetBorderColor(pc->Input,PX_COLOR(255,0,255,0));
	PX_Object_EditSetOffset(pc->Input,2,3);
	PX_Object_ScrollAreaSetBkColor(pc->Area,PX_COLOR(255,0,0,0));
	pc->id=1;

	PX_VectorInitialize(&pc->runtime->mp_ui,&pc->pObjects,sizeof(PX_ConsoleColumn),PC_CONSOLE_DEFAULT_MAX_COLUMN);


	//////////////////////////////////////////////////////////////////////////
	//logo

	if(!PX_ResourceLibraryLoad(&pc->runtime->ResourceLibrary,PX_RESOURCE_TYPE_TEXTURE,(px_byte *)fox_console_logo,sizeof(fox_console_logo),"console_logo"))return PX_FALSE;

	PX_ConsoleShowImage(pc,"console_logo");
	PX_ConsolePrintText(pc,"----------------------------------------");
	PX_ConsolePrintText(pc,"-PainterEngine JIT Compilation Console -\n-Code By DBinary Build on 2019         -\n-Refer To:www.GitHub.com/matrixcascade -");
	PX_ConsolePrintText(pc,"----------------------------------------");
	return PX_TRUE;
}


px_bool PX_ConsoleInitialize(PX_Runtime *runtime,PX_Console *pc)
{
	return PX_ConsoleInitializeEx(runtime,pc,0,0,runtime->surface_width,runtime->surface_height);
}

px_void PX_ConsolePostEvent(PX_Console *pc,PX_Object_Event e)
{
	if(pc->show)
	{
		if (e.Event==PX_OBJECT_EVENT_KEYDOWN)
		{
			if (PX_Object_Event_GetKeyDown(e)==36)
			{
				PX_ConsoleShow(pc,PX_FALSE);
				return;
			}
		}

		if (e.Event!=PX_OBJECT_EVENT_CURSORMOVE)
		{
			PX_ObjectPostEvent(pc->Root,e);
		}
	}
	else
	{
		if (e.Event==PX_OBJECT_EVENT_KEYDOWN)
		{
			if (PX_Object_Event_GetKeyDown(e)==36)
			{
				PX_ConsoleShow(pc,PX_TRUE);
			}
		}
	}
}
px_void PX_ConsoleUpdate(PX_Console *pc,px_dword elapsed)
{
	PX_ObjectUpdate(pc->Area,elapsed);
}


px_void PX_ConsoleShow(PX_Console *pc,px_bool b)
{
	pc->show=b;
}

px_void PX_ConsoleRender(px_surface *psurface,PX_Console *pc,px_dword elapsed)
{
	if(pc->show)
	{
		PX_ObjectRender(psurface,pc->Area,elapsed);
	}
}

px_void PX_ConsoleRegistryHostCall(PX_Console *pc,console_registry_hostcall call_func)
{
	pc->registry_call=call_func;
}

px_void PX_ConsoleRegistryScriptHeader(PX_Console *pc,const px_char *header)
{
	pc->script_header_append=header;
}

px_void PX_ConsoleClear(PX_Console *pc)
{
	PX_ConsoleColumn *pCc;
	px_int i;
	for (i=0;i<pc->pObjects.size;i++)
	{
		pCc=(PX_VECTORAT(PX_ConsoleColumn,&pc->pObjects,i));
		PX_ObjectDelete(pCc->Object);
	}
	PX_VectorClear(&pc->pObjects);
}

