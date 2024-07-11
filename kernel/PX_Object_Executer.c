#include "PX_Object_Executer.h"


px_void PX_Object_ExecuterPrintText(PX_Object *pObject,const px_char *text)
{
	
	PX_Object_Executer* pExecuter = (PX_Object_Executer *)PX_ObjectGetDescByType(pObject,PX_OBJECT_TYPE_EXECUTER);
	PX_ASSERTIF(!pExecuter);
	PX_Object_PrinterPrintText(pExecuter->printer, text);
}

PX_Object * PX_Object_ExecuterLastPrintText(PX_Object* pObject,const px_char *text)
{
	
	PX_Object_Executer* pExecuter = (PX_Object_Executer*)PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_EXECUTER);
	PX_ASSERTIF(!pExecuter);
	return PX_Object_PrinterLastPrintText(pExecuter->printer, text);
	
}

PX_Object * PX_Object_ExecuterPrintImage(PX_Object* pObject, px_texture *ptex)
{

	PX_Object_Executer* pExecuter = (PX_Object_Executer*)PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_EXECUTER);
	PX_ASSERTIF(!pExecuter);
	return PX_Object_PrinterPrintImage(pExecuter->printer, ptex);
	
}

px_void PX_Object_ExecuterClear(PX_Object* pObject)
{
	
	PX_Object_Executer* pExecuter = (PX_Object_Executer*)PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_EXECUTER);
	PX_ASSERTIF(!pExecuter);
	PX_Object_PrinterClear(pExecuter->printer);
	
}
px_bool PX_ExecuterVM_LastPrint(PX_VM *Ins,px_void *userptr)
{
	PX_Object* pObject = (PX_Object*)userptr;
	PX_Object_Executer *pExecuter= (PX_Object_Executer*)PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_EXECUTER);
	PX_ASSERTIF(!pExecuter);

	if (PX_VM_STACK(Ins,0).type!=PX_VM_VARIABLE_TYPE_STRING)
	{
		PX_VM_RET(Ins,PX_Variable_int(0));
		return PX_TRUE;
	}
	PX_Object_ExecuterLastPrintText(pObject,PX_VM_STACK(Ins,0)._string.buffer);
	PX_VM_RET(Ins,PX_Variable_int(PX_Object_PrinterGetLastCreateId(pExecuter->printer)));
	return PX_TRUE;
}

px_bool PX_ExecuterVM_Print(PX_VM *Ins,px_void *userptr)
{
	PX_Object* pObject = (PX_Object*)userptr;
	PX_Object_Executer* pExecuter = (PX_Object_Executer*)PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_EXECUTER);
	PX_ASSERTIF(!pExecuter);

	if (PX_VM_STACK(Ins,0).type!=PX_VM_VARIABLE_TYPE_STRING)
	{
		PX_VM_RET(Ins,PX_Variable_int(0));
		return PX_TRUE;
	}
	if (pExecuter->preslib)
	{
		PX_Resource* pRes = PX_ResourceLibraryGet(pExecuter->preslib, PX_VM_STACK(Ins, 0)._string.buffer);
		if (pRes)
		{
			if (pRes->Type == PX_RESOURCE_TYPE_TEXTURE)
			{
				PX_Object_ExecuterPrintImage(pObject, &pRes->texture);
				PX_VM_RET(Ins, PX_Variable_int(PX_Object_PrinterGetLastCreateId(pExecuter->printer)));
				return PX_TRUE;
			}
		}
	}

	PX_Object_ExecuterPrintText(pObject,PX_VM_STACK(Ins,0)._string.buffer);
	PX_VM_RET(Ins, PX_Variable_int(PX_Object_PrinterGetLastCreateId(pExecuter->printer)));
	return PX_TRUE;
}


px_bool PX_ExecuterVM_Sleep(PX_VM *Ins,px_void *userptr)
{
	PX_Object* pObject = (PX_Object*)userptr;
	PX_Object_Executer* pExecuter = (PX_Object_Executer*)PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_EXECUTER);
	PX_ASSERTIF(!pExecuter);

	if (PX_VM_STACK(Ins,0).type!=PX_VM_VARIABLE_TYPE_INT)
	{
		PX_VM_RET(Ins,PX_Variable_int(0));
		return PX_TRUE;
	}
	pExecuter->vm.pThread[Ins->T].sleep=PX_VM_STACK(Ins,0)._int;
	PX_VM_RET(Ins,PX_Variable_int(0));
	return PX_TRUE;
}

px_bool PX_ExecuterVM_Rand(PX_VM *Ins,px_void *userptr)
{
	PX_VM_RET(Ins,PX_Variable_int(PX_rand()%0x7fffffff));
	return PX_TRUE;
}

px_bool PX_ExecuterVM_Sin(PX_VM *Ins,px_void *userptr)
{
	if (PX_VM_STACK(Ins,0).type!=PX_VM_VARIABLE_TYPE_FLOAT)
	{
		PX_VM_RET(Ins,PX_Variable_int(0));
		return PX_TRUE;
	}
	PX_VM_RET(Ins,PX_Variable_float((px_float)PX_sind(PX_VM_STACK(Ins,0)._float)));
	return PX_TRUE;
}

px_bool PX_ExecuterVM_Cos(PX_VM *Ins,px_void *userptr)
{
	if (PX_VM_STACK(Ins,0).type!=PX_VM_VARIABLE_TYPE_FLOAT)
	{
		PX_VM_RET(Ins,PX_Variable_int(0));
		return PX_TRUE;
	}
	PX_VM_RET(Ins,PX_Variable_float((px_float)PX_cosd(PX_VM_STACK(Ins,0)._float)));
	return PX_TRUE;
}

px_bool PX_ExecuterVM_Gets(PX_VM *Ins,px_void *userptr)
{
	PX_Object* pObject = (PX_Object*)userptr;
	PX_Object_Executer* pExecuter = (PX_Object_Executer*)PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_EXECUTER);
	PX_ASSERTIF(!pExecuter);

	Ins->pThread[Ins->T].suspend=PX_TRUE;
	PX_Object_PrinterGets(pExecuter->printer);
	return PX_TRUE;
}



px_bool PX_ExecuterVM_Clear(PX_VM *Ins,px_void *userptr)
{
	PX_Object* pObject = (PX_Object*)userptr;
	PX_Object_Executer* pExecuter = (PX_Object_Executer*)PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_EXECUTER);
	PX_ASSERTIF(!pExecuter);
	PX_Object_ExecuterClear(pObject);
	return PX_TRUE;
}


px_bool PX_ExecuterVM_CreateThread(PX_VM *Ins,px_void *userptr)
{
	PX_Object* pObject = (PX_Object*)userptr;
	PX_Object_Executer* pExecute = (PX_Object_Executer*)PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_EXECUTER);

	if (PX_VM_STACK(Ins,0).type!=PX_VM_VARIABLE_TYPE_STRING)
	{
		PX_VM_RET(Ins,PX_Variable_int(0));
		return PX_TRUE;
	}

	if(!PX_VMBeginThreadFunction(&pExecute->vm,PX_VMGetFreeThreadId(Ins),PX_VM_STACK(Ins,0)._string.buffer,PX_NULL,0))
	{
		PX_VM_RET(Ins,PX_Variable_int(0));
	}
	return PX_TRUE;
}

px_bool PX_Object_ExecuterRunPayload(PX_Object* pObject, const px_byte bin[],px_int binsize)
{
	PX_Object_Executer* pExecute = (PX_Object_Executer*)PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_EXECUTER);

	if (!PX_VMInitialize(&pExecute->vm, pObject->mp, bin, binsize))
	{
		return PX_FALSE;
	}

	PX_VMRegistHostFunction(&pExecute->vm, "print", PX_ExecuterVM_Print, pObject);//Print
	PX_VMRegistHostFunction(&pExecute->vm, "printf", PX_ExecuterVM_Print, pObject);//Print
	PX_VMRegistHostFunction(&pExecute->vm, "gets", PX_ExecuterVM_Gets, pObject);//Gets
	PX_VMRegistHostFunction(&pExecute->vm, "clear", PX_ExecuterVM_Clear, pObject);//Clear
	PX_VMRegistHostFunction(&pExecute->vm, "sleep", PX_ExecuterVM_Sleep, pObject);//Sleep
	PX_VMRegistHostFunction(&pExecute->vm, "rand", PX_ExecuterVM_Rand, pObject);//Rand
	PX_VMRegistHostFunction(&pExecute->vm, "sin", PX_ExecuterVM_Sin, pObject);//Sin
	PX_VMRegistHostFunction(&pExecute->vm, "cos", PX_ExecuterVM_Cos, pObject);//Cos
	PX_VMRegistHostFunction(&pExecute->vm, "lastprint", PX_ExecuterVM_LastPrint, pObject);//lastprint
	PX_VMRegistHostFunction(&pExecute->vm, "createthread", PX_ExecuterVM_CreateThread, pObject);//createthread

	if (!PX_VMBeginThreadFunction(&pExecute->vm, 0, "main", PX_NULL, 0))
	{
		PX_VMFree(&pExecute->vm);
		return PX_FALSE;
	}
	return PX_TRUE;
}

px_bool PX_Object_ExecuterRunScript(PX_Object *pObject,const px_char pshellstr[])
{
	PX_Compiler compiler = {0};
	px_memory bin = {0};
	const px_char stdlib[]="#name \"stdlib.h\"\n\
						 host int print(string s);\n\
						 host int printf(string s);\n\
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
	const px_char stdio[] = "#name \"stdio.h\"\n\
						 #include \"stdlib.h\"\n";

	PX_Object_Executer* pExecute = (PX_Object_Executer*)PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_EXECUTER);

	
	PX_VMDebuggerMapFree(&pExecute->debugmap);
	PX_VMDebuggerMapInitialize(pObject->mp, &pExecute->debugmap);
	PX_MemoryInitialize(pObject->mp, &bin);
	if (!PX_CompilerInitialize(pObject->mp, &compiler))	goto _ERROR;
	if (!PX_CompilerAddSource(&compiler, stdlib))	goto _ERROR;
	if (!PX_CompilerAddSource(&compiler, stdio))	goto _ERROR;
	if (!PX_CompilerAddSource(&compiler, pshellstr))	goto _ERROR;
	if (!PX_CompilerCompile(&compiler, &bin, &pExecute->debugmap, "main"))goto _ERROR;

	PX_CompilerFree(&compiler);
	PX_memset(&compiler, 0, sizeof(compiler));
	//Load
	if (!PX_Object_ExecuterRunPayload(pObject, bin.buffer, bin.usedsize))
	{
		goto _ERROR;
	}
	PX_MemoryFree(&bin);
	return PX_TRUE;
_ERROR:
	PX_CompilerFree(&compiler);
	PX_MemoryFree(&bin);
	PX_VMDebuggerMapFree(&pExecute->debugmap);
	PX_memset(&pExecute->debugmap, 0, sizeof(pExecute->debugmap));
	return PX_FALSE;
}

px_void PX_Object_ExecuterSetBackgroundColor(PX_Object* pObject, px_color color)
{
	PX_Object_Executer* pExecuter = (PX_Object_Executer*)PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_EXECUTER);
	PX_ASSERTIF(!pExecuter);
	PX_Object_PrinterSetBackgroundColor(pExecuter->printer, color);
	
}

px_void PX_Object_ExecuterSetFontColor(PX_Object* pObject, px_color color)
{
	PX_Object_Executer* pExecuter = (PX_Object_Executer*)PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_EXECUTER);
	PX_ASSERTIF(!pExecuter);
	PX_Object_PrinterSetFontColor(pExecuter->printer, color);

}

px_void PX_Object_ExecuterSetRunTick(PX_Object* pObject, px_int tick)
{
	PX_Object_Executer* pExecuter = (PX_Object_Executer*)PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_EXECUTER);
	PX_ASSERTIF(!pExecuter);
	pExecuter->runtick=tick;

}

PX_VM_DebuggerMap* PX_Object_ExecuterGetDebugMap(PX_Object* pObject)
{
	PX_Object_Executer* pExecuter = (PX_Object_Executer*)PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_EXECUTER);
	return &pExecuter->debugmap;

}

PX_VM* PX_Object_ExecuterGetVM(PX_Object* pObject)
{
	PX_Object_Executer* pExecuter = (PX_Object_Executer*)PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_EXECUTER);
	return &pExecuter->vm;
}

px_bool PX_ExecuterRegistryHostFunction(PX_Object *pObject,const px_char Name[],PX_VM_Host_Function_Modules function,px_void *userptr)
{
	
	PX_Object_Executer* pExecuter=(PX_Object_Executer*)PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_EXECUTER);
	return PX_VMRegistHostFunction(&pExecuter->vm, Name, function, userptr);
	
}

PX_OBJECT_UPDATE_FUNCTION(PX_Object_ExecuterUpdate)
{
	PX_Object_Executer *pdesc =PX_ObjectGetDesc(PX_Object_Executer,pObject);
	PX_VMRun(&pdesc->vm,pdesc->runtick, elapsed);
}

PX_OBJECT_RENDER_FUNCTION(PX_Object_ExecuterRender)
{
	PX_Object_Executer* pdesc = PX_ObjectGetDesc(PX_Object_Executer, pObject);
	pdesc->printer->Width = pObject->Width;
	pdesc->printer->Height = pObject->Height;


}
PX_OBJECT_FREE_FUNCTION(PX_Object_ExecuterFree)
{
	PX_Object_Executer* pdesc = PX_ObjectGetDesc(PX_Object_Executer, pObject);
	if (pdesc->vm._bin)
	PX_VMFree(&pdesc->vm);
	if(pdesc->debugmap.mp)
	PX_VMDebuggerMapFree(&pdesc->debugmap);
}

PX_Object* PX_Object_ExecuterAttachObject(PX_Object* pObject,px_int attachIndex, px_int x, px_int y, px_int width, px_int height, PX_FontModule* fm, const px_char shellcode[])
{
	px_memorypool* mp=pObject->mp;
	PX_Object_Executer * pdesc;
	PX_ASSERTIF(pObject == PX_NULL);
	PX_ASSERTIF(attachIndex < 0 || attachIndex >= PX_COUNTOF(pObject->pObjectDesc));
	PX_ASSERTIF(pObject->pObjectDesc[attachIndex] != PX_NULL);
	pdesc = (PX_Object_Executer*)PX_ObjectCreateDesc(pObject, attachIndex, PX_OBJECT_TYPE_EXECUTER, PX_Object_ExecuterUpdate, 0, PX_Object_ExecuterFree, 0, sizeof(PX_Object_Executer));
	PX_ASSERTIF(pdesc == PX_NULL);

	pdesc->fm = fm;
	pdesc->runtick = -1;
	pdesc = (PX_Object_Executer*)PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_EXECUTER);
	pdesc->printer = PX_Object_PrinterCreate(mp, pObject, x, y, width, height, fm);

	if (shellcode)
	{
		if (!PX_Object_ExecuterRunScript(pObject, shellcode))
		{
			PX_ObjectDelete(pObject);
			return PX_NULL;
		}
		PX_Object_ExecuterPrintText(pObject, "----------------------------------------");
		PX_Object_ExecuterPrintText(pObject, "PainterEngine Console");
		PX_Object_ExecuterPrintText(pObject, "----------------------------------------");
	}
	return pObject;

}

PX_Object* PX_Object_ExecuterCreate(px_memorypool* mp, PX_Object* Parent, px_int x, px_int y, px_int width, px_int height, PX_FontModule* fm, const px_char shellcode[])
{
	PX_Object* pObject;
	pObject = PX_ObjectCreate(mp, Parent, 0, 0, 0, (px_float)width, (px_float)height, 0);
	if (!pObject)
	{
		return PX_NULL;
	}
	if (!PX_Object_ExecuterAttachObject(pObject, 0, x, y, width, height, fm, shellcode))
	{
		PX_ObjectDelete(pObject);
		return PX_NULL;
	}
	
	return pObject;
}

px_void PX_Object_ExecuterSetResourceLibrary(PX_Object* pObject, PX_ResourceLibrary* preslib)
{
	PX_Object_Executer* pdesc = (PX_Object_Executer*)PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_EXECUTER);
	pdesc->preslib = preslib;
}

PX_Object* PX_Object_ExecuterCreatePayload(px_memorypool* mp, PX_Object* Parent, px_int x, px_int y, px_int width, px_int height, PX_FontModule* fm, const px_byte bin[],px_int binsize)
{
	PX_Object* pObject;
	PX_Object_Executer desc, * pdesc;
	//console initialize
	PX_memset(&desc, 0, sizeof(desc));
	desc.fm = fm;

	pObject = PX_ObjectCreateEx(mp, Parent, 0, 0, 0, (px_float)width, (px_float)height, 0, PX_OBJECT_TYPE_EXECUTER, PX_Object_ExecuterUpdate, 0, PX_Object_ExecuterFree, &desc, sizeof(desc));
	pdesc = (PX_Object_Executer*)PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_EXECUTER);
	pdesc->printer = PX_Object_PrinterCreate(mp, pObject, x, y, width, height, fm);


	if (!PX_Object_ExecuterRunPayload(pObject, bin,binsize))
	{
		PX_ObjectDelete(pObject);
		return PX_NULL;
	}

	PX_Object_ExecuterPrintText(pObject, "----------------------------------------");
	PX_Object_ExecuterPrintText(pObject, "PainterEngine Script Executer");
	PX_Object_ExecuterPrintText(pObject, "----------------------------------------");

	return pObject;
}
