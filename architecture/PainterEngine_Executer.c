#include "PainterEngine_Executer.h"



px_void PX_ExecuterUpdateEx(PX_Executer *pExecute)
{
	px_int i,y=10;
	PX_ExecuterColumn *pCc;

	while (pExecute->pObjects.size>pExecute->max_column)
	{
		pCc=(PX_VECTORAT(PX_ExecuterColumn,&pExecute->pObjects,0));
		PX_ObjectDelete(pCc->Object);
		PX_VectorErase(&pExecute->pObjects,0);
	}

	for (i=0;i<pExecute->pObjects.size;i++)
	{
		pCc=(PX_VECTORAT(PX_ExecuterColumn,&pExecute->pObjects,i));

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
	PX_ObjectSetPosition(pExecute->Input,0,(px_float)y,0);
}
PX_Object * PX_ExecuterPrintText(PX_Executer *pExecute,const px_char *text)
{
	PX_ExecuterColumn obj;
	PX_Object *pObject=PX_Object_AutoTextCreate(&pExecute->runtime->mp_ui,(pExecute->Area),0,0,pExecute->runtime->surface_width-1,PX_NULL);

	if (pObject)
	{
		obj.Object=pObject;
		obj.id=pExecute->id++;
		PX_Object_AutoTextSetTextColor(pObject,PX_COLOR(255,0,255,0));
		PX_Object_AutoTextSetText(pObject,text);
		PX_VectorPushback(&pExecute->pObjects,&obj);
		PX_ExecuterUpdateEx(pExecute);
		PX_Object_ScrollAreaMoveToBottom(pExecute->Area);
	}
	return pObject;
}

PX_Object * PX_ExecuterLastPrintText(PX_Executer *pExecute,const px_char *text)
{
	PX_ExecuterColumn *pobjColumn;
	PX_Object *pObject;
	if (pExecute->pObjects.size==0)
	{
		return PX_NULL;
	}
	pobjColumn=PX_VECTORLAST(PX_ExecuterColumn,&pExecute->pObjects);
	pObject=pobjColumn->Object;
	if (pObject->Type==PX_OBJECT_TYPE_AUTOTEXT)
	{
		PX_Object_AutoTextSetTextColor(pObject,PX_COLOR(255,0,255,0));
		PX_Object_AutoTextSetText(pObject,text);
		return pObject;
	}
	else
		return PX_NULL;
	
}

PX_Object * PX_ExecuterPrintImage(PX_Executer *pExecute,const px_char *res_image_key)
{
	PX_ExecuterColumn obj;
	PX_Resource *pimageRes;
	PX_Object *pObject;
	if((pimageRes=PX_ResourceLibraryGet(&pExecute->runtime->ResourceLibrary,res_image_key))!=PX_NULL)
	{
		if (pimageRes->Type==PX_RESOURCE_TYPE_TEXTURE)
		{
			pObject=PX_Object_ImageCreate(&pExecute->runtime->mp_ui,pExecute->Area,0,0,pimageRes->texture.width,pimageRes->texture.height,&pimageRes->texture);
			PX_Object_ImageSetAlign(pObject,PX_ALIGN_LEFTTOP);
			PX_ObjectSetSize(pObject,(px_float)pimageRes->texture.width,(px_float)pimageRes->texture.height,0);
			obj.Object=pObject;
			obj.id=pExecute->id++;
			PX_VectorPushback(&pExecute->pObjects,&obj);
			PX_ExecuterUpdateEx(pExecute);
			PX_Object_ScrollAreaMoveToBottom(pExecute->Area);
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
px_bool PX_ExecuterVM_LastPrint(PX_ScriptVM_Instance *Ins,px_void *userptr)
{
	PX_Executer *pExecute=(PX_Executer *)userptr;

	if (PX_ScriptVM_STACK(Ins,0).type!=PX_SCRIPTVM_VARIABLE_TYPE_STRING)
	{
		PX_ScriptVM_RET(Ins,PX_ScriptVM_Variable_int(0));
		return PX_TRUE;
	}
	PX_ExecuterLastPrintText(pExecute,PX_ScriptVM_STACK(Ins,0)._string.buffer);
	PX_ScriptVM_RET(Ins,PX_ScriptVM_Variable_int(PX_VECTORAT(PX_ExecuterColumn,&pExecute->pObjects,pExecute->pObjects.size-1)->id));
	return PX_TRUE;
}

px_bool PX_ExecuterVM_Print(PX_ScriptVM_Instance *Ins,px_void *userptr)
{
	PX_Executer *pExecute=(PX_Executer *)userptr;

	if (PX_ScriptVM_STACK(Ins,0).type!=PX_SCRIPTVM_VARIABLE_TYPE_STRING)
	{
		PX_ScriptVM_RET(Ins,PX_ScriptVM_Variable_int(0));
		return PX_TRUE;
	}
	PX_ExecuterPrintText(pExecute,PX_ScriptVM_STACK(Ins,0)._string.buffer);
	PX_ScriptVM_RET(Ins,PX_ScriptVM_Variable_int(PX_VECTORAT(PX_ExecuterColumn,&pExecute->pObjects,pExecute->pObjects.size-1)->id));
	return PX_TRUE;
}
px_bool PX_ExecuterVM_PrintImage(PX_ScriptVM_Instance *Ins,px_void *userptr)
{
	PX_Executer *pExecute=(PX_Executer *)userptr;

	if (PX_ScriptVM_STACK(Ins,0).type!=PX_SCRIPTVM_VARIABLE_TYPE_STRING)
	{
		PX_ScriptVM_RET(Ins,PX_ScriptVM_Variable_int(0));
		return PX_TRUE;
	}
	PX_ExecuterPrintImage(pExecute,PX_ScriptVM_STACK(Ins,0)._string.buffer);
	PX_ScriptVM_RET(Ins,PX_ScriptVM_Variable_int(PX_VECTORAT(PX_ExecuterColumn,&pExecute->pObjects,pExecute->pObjects.size-1)->id));
	return PX_TRUE;
}


px_bool PX_ExecuterVM_Sleep(PX_ScriptVM_Instance *Ins,px_void *userptr)
{
	PX_Executer *pExecute=(PX_Executer *)userptr;

	if (PX_ScriptVM_STACK(Ins,0).type!=PX_SCRIPTVM_VARIABLE_TYPE_INT)
	{
		PX_ScriptVM_RET(Ins,PX_ScriptVM_Variable_int(0));
		return PX_TRUE;
	}
	pExecute->Sleep[Ins->T]=PX_ScriptVM_STACK(Ins,0)._int;
	PX_ScriptVM_ThreadSuspend(Ins,Ins->T);
	PX_ScriptVM_RET(Ins,PX_ScriptVM_Variable_int(0));
	return PX_TRUE;
}

px_bool PX_ExecuterVM_Rand(PX_ScriptVM_Instance *Ins,px_void *userptr)
{
	PX_ScriptVM_RET(Ins,PX_ScriptVM_Variable_int(PX_rand()%0x7fffffff));
	return PX_TRUE;
}

px_bool PX_ExecuterVM_Sin(PX_ScriptVM_Instance *Ins,px_void *userptr)
{
	if (PX_ScriptVM_STACK(Ins,0).type!=PX_SCRIPTVM_VARIABLE_TYPE_FLOAT)
	{
		PX_ScriptVM_RET(Ins,PX_ScriptVM_Variable_int(0));
		return PX_TRUE;
	}
	PX_ScriptVM_RET(Ins,PX_ScriptVM_Variable_float((px_float)PX_sind(PX_ScriptVM_STACK(Ins,0)._float)));
	return PX_TRUE;
}

px_bool PX_ExecuterVM_Cos(PX_ScriptVM_Instance *Ins,px_void *userptr)
{
	if (PX_ScriptVM_STACK(Ins,0).type!=PX_SCRIPTVM_VARIABLE_TYPE_FLOAT)
	{
		PX_ScriptVM_RET(Ins,PX_ScriptVM_Variable_int(0));
		return PX_TRUE;
	}
	PX_ScriptVM_RET(Ins,PX_ScriptVM_Variable_float((px_float)PX_cosd(PX_ScriptVM_STACK(Ins,0)._float)));
	return PX_TRUE;
}

px_bool PX_ExecuterVM_Gets(PX_ScriptVM_Instance *Ins,px_void *userptr)
{
	PX_Executer *pExecute=(PX_Executer *)userptr;
	Ins->pThread[Ins->T].suspend=PX_TRUE;
	pExecute->bInput=PX_TRUE;
	return PX_TRUE;
}

px_void PX_ExecuterClear(PX_Executer *pExecute)
{
	PX_ExecuterColumn *pCc;
	px_int i;
	for (i=0;i<pExecute->pObjects.size;i++)
	{
		pCc=(PX_VECTORAT(PX_ExecuterColumn,&pExecute->pObjects,i));
		PX_ObjectDelete(pCc->Object);
	}
	PX_VectorClear(&pExecute->pObjects);
}

px_bool PX_ExecuterVM_Clear(PX_ScriptVM_Instance *Ins,px_void *userptr)
{
	PX_Executer *pExecute=(PX_Executer *)userptr;
	PX_ExecuterClear(pExecute);
	return PX_TRUE;
}


px_bool PX_ExecuterVM_CreateThread(PX_ScriptVM_Instance *Ins,px_void *userptr)
{
	PX_Executer *pExecute=(PX_Executer *)userptr;

	if (PX_ScriptVM_STACK(Ins,0).type!=PX_SCRIPTVM_VARIABLE_TYPE_STRING)
	{
		PX_ScriptVM_RET(Ins,PX_ScriptVM_Variable_int(0));
		return PX_TRUE;
	}

	if(!PX_ScriptVM_InstanceBeginThreadFunction(&pExecute->VM_Instance,PX_ScriptVM_InstanceGetFreeThreadId(Ins),PX_ScriptVM_STACK(Ins,0)._string.buffer,PX_NULL,0))
	{
		PX_ScriptVM_RET(Ins,PX_ScriptVM_Variable_int(0));
	}
	return PX_TRUE;
}




px_bool PX_ExecuterRunScipt(PX_Executer *pExecute,const px_char *pshellstr)
{
	px_memory bin;
	PX_SCRIPT_LIBRARY lib;
	px_string asmcodeString;
	px_memorypool mp_calc;
	const px_char stdio[]="#name \"stdlib.h\"\n\
						 host int print(string s);\n\
						 host int printImage(string key);\n\
						 host string gets();\n\
						 host void clear();\n\
						 host void sleep(int millionsecond);\n\
						 host int rand();\n\
						 host float sin(float x);\n\
						 host float cos(float x);\n\
						 host int lastprint(string s);\n\
						 host int createthread(string callname);\n\
						  ";
	mp_calc=MP_Create(MP_Malloc(&pExecute->runtime->mp,PE_MEMORY_CALC_SIZE),PE_MEMORY_CALC_SIZE);
	if(mp_calc.StartAddr==PX_NULL) return PX_FALSE;
	MP_Reset(&mp_calc);
	PX_MemoryInitialize(&mp_calc,&bin);

	if(!PX_ScriptCompilerInitialize(&lib,&mp_calc))
	{
		MP_Free(&pExecute->runtime->mp,mp_calc.StartAddr);
		return PX_FALSE;
	}

	if(!PX_ScriptCompilerLoad(&lib,stdio))
	{
		MP_Free(&pExecute->runtime->mp,mp_calc.StartAddr);
		return PX_FALSE;
	}

	if(!PX_ScriptCompilerLoad(&lib,pshellstr))
	{
		MP_Free(&pExecute->runtime->mp,mp_calc.StartAddr);
		return PX_FALSE;
	}


	PX_StringInitialize(&mp_calc,&asmcodeString);
	if(PX_ScriptCompilerCompile(&lib,"main",&asmcodeString,128))
	{
		PX_ScriptAsmOptimization(&asmcodeString);

		if(!PX_ScriptAsmCompile(&mp_calc,asmcodeString.buffer,&bin))
		{
			MP_Free(&pExecute->runtime->mp,mp_calc.StartAddr);
			return PX_FALSE;
		}
	}
	else
	{
		MP_Free(&pExecute->runtime->mp,mp_calc.StartAddr);
		return PX_FALSE;
	}

	PX_StringFree(&asmcodeString);
	PX_ScriptCompilerFree(&lib);

	//Load
	if(!PX_ScriptVM_InstanceInitialize(&pExecute->VM_Instance,&pExecute->runtime->mp_game,bin.buffer,bin.usedsize))
	{
		MP_Free(&pExecute->runtime->mp,mp_calc.StartAddr);
		return PX_FALSE;
	}
	PX_MemoryFree(&bin);
	MP_Free(&pExecute->runtime->mp,mp_calc.StartAddr);
	//RegistryFunctions
	
	PX_ScriptVM_RegistryHostFunction(&pExecute->VM_Instance,"print",PX_ExecuterVM_Print,pExecute);//Print
	PX_ScriptVM_RegistryHostFunction(&pExecute->VM_Instance,"printImage",PX_ExecuterVM_PrintImage,pExecute);//Print Image
	PX_ScriptVM_RegistryHostFunction(&pExecute->VM_Instance,"gets",PX_ExecuterVM_Gets,pExecute);//Gets
	PX_ScriptVM_RegistryHostFunction(&pExecute->VM_Instance,"clear",PX_ExecuterVM_Clear,pExecute);//Clear
	PX_ScriptVM_RegistryHostFunction(&pExecute->VM_Instance,"sleep",PX_ExecuterVM_Sleep,pExecute);//Sleep
	PX_ScriptVM_RegistryHostFunction(&pExecute->VM_Instance,"rand",PX_ExecuterVM_Rand,pExecute);//Rand
	PX_ScriptVM_RegistryHostFunction(&pExecute->VM_Instance,"sin",PX_ExecuterVM_Sin,pExecute);//Rand
	PX_ScriptVM_RegistryHostFunction(&pExecute->VM_Instance,"cos",PX_ExecuterVM_Cos,pExecute);//Rand
	PX_ScriptVM_RegistryHostFunction(&pExecute->VM_Instance,"lastprint",PX_ExecuterVM_LastPrint,pExecute);//lastprint
	PX_ScriptVM_RegistryHostFunction(&pExecute->VM_Instance,"createthread",PX_ExecuterVM_CreateThread,pExecute);//lastprint
	//////////////////////////////////////////////////////////////////////////
	//sleep
	if (pExecute->Sleep)
	{
		MP_Free(&pExecute->runtime->mp_game,pExecute->Sleep);
	}
	pExecute->Sleep=(px_dword *)MP_Malloc(&pExecute->runtime->mp_game,pExecute->VM_Instance.maxThreadCount*sizeof(px_dword));
	
	
	//////////////////////////////////////////////////////////////////////////

	if(!PX_ScriptVM_InstanceRunFunction(&pExecute->VM_Instance,0,"_BOOT",PX_NULL,0))
	{
		
		return PX_FALSE;
	}

	if(!PX_ScriptVM_InstanceBeginThreadFunction(&pExecute->VM_Instance,0,"MAIN",PX_NULL,0))
	{
		return PX_FALSE;
	}


	return PX_TRUE;

}


px_bool PX_ExecuterLoadScipt(PX_Executer* pExecuter, const px_char* pshellstr)
{
	return PX_ExecuterRunScipt(pExecuter, pshellstr);
}

px_bool PX_ExecuterRegistryHostFunction(PX_Executer *pExecuter,const px_char Name[],PX_ScriptVM_Function_Modules function,px_void *userptr)
{
	return PX_ScriptVM_RegistryHostFunction(&pExecuter->VM_Instance,Name,function,userptr);
}

px_void PX_ExecuterOnEnter(PX_Object *Obj,PX_Object_Event e,px_void *user_ptr)
{
	PX_Executer *pExecute=(PX_Executer *)Obj->User_ptr;
	PX_Object_Edit *pEdit=PX_Object_GetEdit(pExecute->Input);

	if (!pExecute->Input->Visible)
	{
		return;
	}

	if (e.Event==PX_OBJECT_EVENT_KEYDOWN)
	{
		if (PX_Object_Event_GetKeyDown(e)=='\r')
		{
			PX_ScriptVM_RET(&pExecute->VM_Instance,PX_ScriptVM_Variable_const_string(pEdit->text.buffer));
			PX_ExecuterPrintText(pExecute,pEdit->text.buffer);
			PX_Object_EditSetText(pExecute->Input,"");
			pExecute->bInput=PX_FALSE;
			pExecute->VM_Instance.pThread[0].suspend=PX_FALSE;
			
		}
	}

	if (e.Event==PX_OBJECT_EVENT_CURSORRDOWN)
	{
		PX_Object_EditSetFocus(pExecute->Input,PX_TRUE);
	}
}

px_void PX_ExecuterOnMouseDown(PX_Object *Obj,PX_Object_Event e,px_void *user_ptr)
{
	PX_Executer *pExecute=(PX_Executer *)Obj->User_ptr;
	if (e.Event==PX_OBJECT_EVENT_CURSORDOWN)
	{
		PX_Object_EditSetFocus(pExecute->Input,PX_TRUE);
	}
}


PX_Object * PX_ExecuterShowImage(PX_Executer *pExecute,const px_char *res_image_key)
{
	PX_ExecuterColumn obj;
	PX_Resource *pimageRes;
	PX_Object *pObject;
	if((pimageRes=PX_ResourceLibraryGet(&pExecute->runtime->ResourceLibrary,res_image_key))!=PX_NULL)
	{
		if (pimageRes->Type==PX_RESOURCE_TYPE_TEXTURE)
		{
			pObject=PX_Object_ImageCreate(&pExecute->runtime->mp_ui,PX_Object_ScrollAreaGetIncludedObjects(pExecute->Area),0,0,pimageRes->texture.width,pimageRes->texture.height,&pimageRes->texture);
			PX_Object_ImageSetAlign(pObject,PX_ALIGN_LEFTTOP);
			obj.Object=pObject;
			obj.id=pExecute->id++;
			PX_VectorPushback(&pExecute->pObjects,&obj);
			PX_ExecuterUpdateEx(pExecute);
			PX_Object_ScrollAreaMoveToBottom(pExecute->Area);
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



px_bool PX_ExecuterInitialize(PX_Runtime *runtime,PX_Executer *pExecute)
{
	px_char const fox_executer_logo[] =
	{
	#include "PainterEngine_FoxLogo.h"
	};
	//console initialize
	PX_memset(pExecute,0,sizeof(PX_Executer));

	pExecute->runtime=runtime;
	pExecute->show=PX_TRUE;
	pExecute->max_column=PX_EXECUTER_DEFAULT_MAX_COLUMN;
	pExecute->column=0;
	if(!(pExecute->Root=PX_ObjectCreate(&pExecute->runtime->mp_ui,0,0,0,0,0,0,0))) return PX_FALSE;
	if(!(pExecute->Area=PX_Object_ScrollAreaCreate(&pExecute->runtime->mp_ui,pExecute->Root,0,0,pExecute->runtime->surface_width,pExecute->runtime->surface_height))) return PX_FALSE;


	pExecute->Area->User_ptr=pExecute;
	PX_ObjectRegisterEvent(pExecute->Area,PX_OBJECT_EVENT_KEYDOWN,PX_ExecuterOnEnter,PX_NULL);
	PX_ObjectRegisterEvent(pExecute->Area,PX_OBJECT_EVENT_CURSORDOWN,PX_ExecuterOnMouseDown,PX_NULL);
	PX_Object_ScrollAreaSetBorder(pExecute->Area,PX_FALSE);
	if(!(pExecute->Input=PX_Object_EditCreate(&pExecute->runtime->mp_ui,(pExecute->Area),0,0,pExecute->runtime->surface_width-1,PX_FontGetCharactorHeight()+4,PX_NULL))) return PX_FALSE;
	PX_Object_EditSetTextColor(pExecute->Input,PX_COLOR(255,0,255,0));
	PX_Object_EditSetCursorColor(pExecute->Input,PX_COLOR(255,0,255,0));
	PX_Object_EditSetTextColor(pExecute->Input,PX_COLOR(255,0,255,0));
	PX_Object_EditSetBorderColor(pExecute->Input,PX_COLOR(255,0,255,0));
	PX_Object_EditSetOffset(pExecute->Input,2,3);
	PX_Object_EditSetLimit(pExecute->Input,"zxcvbnm,./asdfghjkl;'qwertyyuiop[]\\`1234567890-=ZXCVBNM<>?ASDFGHJKL:\"QWERTYUIOP{}|~!@#$%^&*()_+");
	PX_Object_ScrollAreaSetBkColor(pExecute->Area,PX_COLOR(255,0,0,0));
	pExecute->id=1;

	PX_VectorInitialize(&pExecute->runtime->mp_ui,&pExecute->pObjects,sizeof(PX_ExecuterColumn),PX_EXECUTER_DEFAULT_MAX_COLUMN);


	//////////////////////////////////////////////////////////////////////////
	//logo

	if(!PX_ResourceLibraryLoad(&pExecute->runtime->ResourceLibrary,PX_RESOURCE_TYPE_TEXTURE,(px_byte *)fox_executer_logo,sizeof(fox_executer_logo),"fox_executer_logo"))return PX_FALSE;

	PX_ExecuterShowImage(pExecute,"fox_executer_logo");
	PX_ExecuterShowImage(pExecute,"console_logo");
	PX_ExecuterPrintText(pExecute,"----------------------------------------");
	PX_ExecuterPrintText(pExecute,"-PainterEngine Script Executer         -\n-Code By DBinary Build on v2021         -\n-Refer To:www.GitHub.com/matrixcascade -");
	PX_ExecuterPrintText(pExecute,"----------------------------------------");


	return PX_TRUE;
}
px_void PX_ExecuterPostEvent(PX_Executer *pExecute,PX_Object_Event e)
{
	if(pExecute->show)
	{
		if (e.Event==PX_OBJECT_EVENT_KEYDOWN)
		{
			if (PX_Object_Event_GetKeyDown(e)==36)
			{
				PX_ExecuterShow(pExecute,PX_FALSE);
				return;
			}
		}

		if (e.Event!=PX_OBJECT_EVENT_CURSORMOVE)
		{
			PX_ObjectPostEvent(pExecute->Root,e);
		}
	}
	else
	{
		if (e.Event==PX_OBJECT_EVENT_KEYDOWN)
		{
			if (PX_Object_Event_GetKeyDown(e)==36)
			{
				PX_ExecuterShow(pExecute,PX_TRUE);
			}
		}
	}
}
px_void PX_ExecuterUpdate(PX_Executer *pExecute,px_dword elapsed)
{
	px_int i;

	pExecute->Area->Width=(px_float)pExecute->runtime->surface_width;
	pExecute->Area->Height=(px_float)pExecute->runtime->surface_height;
	pExecute->Input->Width=(px_float)pExecute->runtime->surface_width-1;
	PX_ObjectUpdate(pExecute->Area,elapsed);

	if (pExecute->bInput)
	{
		pExecute->Input->Visible=PX_TRUE;
	}
	else
	{
		pExecute->Input->Visible=PX_FALSE;
	}

	if (pExecute->VM_Instance.binsize)
	{
		for (i=0;i<pExecute->VM_Instance.maxThreadCount;i++)
		{
			if (pExecute->VM_Instance.pThread[i].Activated)
			{
				if (pExecute->Sleep[i]==0)
				{
					PX_ScriptVM_InstanceRun(&pExecute->VM_Instance,0xffff);
				}
				else
				{
					if (pExecute->Sleep[i]<=elapsed)
					{
						pExecute->VM_Instance.pThread[i].suspend=PX_FALSE;
						pExecute->Sleep[i]=0;
					}
					else
					{
						pExecute->VM_Instance.pThread[i].suspend=PX_TRUE;
						pExecute->Sleep[i]-=elapsed;
					}
				}
				
			}
		}
		
	}

}


px_void PX_ExecuterShow(PX_Executer *pExecute,px_bool b)
{
	pExecute->show=b;
}

px_void PX_ExecuterRender(PX_Executer *pExecute,px_dword elapsed)
{
	if(pExecute->show)
	{
		PX_ObjectRender(&pExecute->runtime->RenderSurface,pExecute->Area,elapsed);
	}
}

