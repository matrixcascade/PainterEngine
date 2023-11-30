#include "PX_Object_Executer.h"


px_void PX_Object_ExecuterPrintText(PX_Object *pObject,const px_char *text)
{
	if (pObject->Type==PX_OBJECT_TYPE_EXECUTER)
	{
		PX_Object_Executer* pExecuter = PX_ObjectGetDesc(PX_Object_Executer, pObject);
		PX_Object_PrinterPrintText(pExecuter->printer, text);
	}
	
}

PX_Object * PX_Object_ExecuterLastPrintText(PX_Object* pObject,const px_char *text)
{
	if (pObject->Type == PX_OBJECT_TYPE_EXECUTER)
	{
		PX_Object_Executer* pExecuter = PX_ObjectGetDesc(PX_Object_Executer, pObject);
		return PX_Object_PrinterLastPrintText(pExecuter->printer, text);
	}
	return PX_NULL;
}

PX_Object * PX_Object_ExecuterPrintImage(PX_Object* pObject, px_texture *ptex)
{
	if (pObject->Type == PX_OBJECT_TYPE_EXECUTER)
	{
		PX_Object_Executer* pExecuter = PX_ObjectGetDesc(PX_Object_Executer, pObject);
		return PX_Object_PrinterPrintImage(pExecuter->printer, ptex);
	}
	return PX_NULL;
}

px_void PX_Object_ExecuterClear(PX_Object* pObject)
{
	if (pObject->Type == PX_OBJECT_TYPE_EXECUTER)
	{
		PX_Object_Executer* pExecuter = PX_ObjectGetDesc(PX_Object_Executer, pObject);
		PX_Object_PrinterClear(pExecuter->printer);
	}
}
px_bool PX_ExecuterVM_LastPrint(PX_VM *Ins,px_void *userptr)
{
	PX_Object* pObject = (PX_Object*)userptr;
	PX_Object_Executer *pExecute=PX_ObjectGetDesc(PX_Object_Executer,pObject);

	if (PX_VM_STACK(Ins,0).type!=PX_VM_VARIABLE_TYPE_STRING)
	{
		PX_VM_RET(Ins,PX_VM_Variable_int(0));
		return PX_TRUE;
	}
	PX_Object_ExecuterLastPrintText(pObject,PX_VM_STACK(Ins,0)._string.buffer);
	PX_VM_RET(Ins,PX_VM_Variable_int(PX_Object_PrinterGetLastCreateId(pExecute->printer)));
	return PX_TRUE;
}

px_bool PX_ExecuterVM_Print(PX_VM *Ins,px_void *userptr)
{
	PX_Object* pObject = (PX_Object*)userptr;
	PX_Object_Executer* pExecute = PX_ObjectGetDesc(PX_Object_Executer, pObject);

	if (PX_VM_STACK(Ins,0).type!=PX_VM_VARIABLE_TYPE_STRING)
	{
		PX_VM_RET(Ins,PX_VM_Variable_int(0));
		return PX_TRUE;
	}
	PX_Object_ExecuterPrintText(pObject,PX_VM_STACK(Ins,0)._string.buffer);
	PX_VM_RET(Ins, PX_VM_Variable_int(PX_Object_PrinterGetLastCreateId(pExecute->printer)));
	return PX_TRUE;
}


px_bool PX_ExecuterVM_Sleep(PX_VM *Ins,px_void *userptr)
{
	PX_Object* pObject = (PX_Object*)userptr;
	PX_Object_Executer* pExecute = PX_ObjectGetDesc(PX_Object_Executer, pObject);

	if (PX_VM_STACK(Ins,0).type!=PX_VM_VARIABLE_TYPE_INT)
	{
		PX_VM_RET(Ins,PX_VM_Variable_int(0));
		return PX_TRUE;
	}
	pExecute->vm.pThread[Ins->T].sleep=PX_VM_STACK(Ins,0)._int;
	PX_VM_RET(Ins,PX_VM_Variable_int(0));
	return PX_TRUE;
}

px_bool PX_ExecuterVM_Rand(PX_VM *Ins,px_void *userptr)
{
	PX_VM_RET(Ins,PX_VM_Variable_int(PX_rand()%0x7fffffff));
	return PX_TRUE;
}

px_bool PX_ExecuterVM_Sin(PX_VM *Ins,px_void *userptr)
{
	if (PX_VM_STACK(Ins,0).type!=PX_VM_VARIABLE_TYPE_FLOAT)
	{
		PX_VM_RET(Ins,PX_VM_Variable_int(0));
		return PX_TRUE;
	}
	PX_VM_RET(Ins,PX_VM_Variable_float((px_float)PX_sind(PX_VM_STACK(Ins,0)._float)));
	return PX_TRUE;
}

px_bool PX_ExecuterVM_Cos(PX_VM *Ins,px_void *userptr)
{
	if (PX_VM_STACK(Ins,0).type!=PX_VM_VARIABLE_TYPE_FLOAT)
	{
		PX_VM_RET(Ins,PX_VM_Variable_int(0));
		return PX_TRUE;
	}
	PX_VM_RET(Ins,PX_VM_Variable_float((px_float)PX_cosd(PX_VM_STACK(Ins,0)._float)));
	return PX_TRUE;
}

px_bool PX_ExecuterVM_Gets(PX_VM *Ins,px_void *userptr)
{
	PX_Object* pObject = (PX_Object*)userptr;
	PX_Object_Executer* pExecute = PX_ObjectGetDesc(PX_Object_Executer, pObject);

	Ins->pThread[Ins->T].suspend=PX_TRUE;
	PX_Object_PrinterGets(pExecute->printer);
	return PX_TRUE;
}



px_bool PX_ExecuterVM_Clear(PX_VM *Ins,px_void *userptr)
{
	PX_Object* pObject = (PX_Object*)userptr;
	PX_Object_Executer* pExecute = PX_ObjectGetDesc(PX_Object_Executer, pObject);
	PX_Object_ExecuterClear(pObject);
	return PX_TRUE;
}


px_bool PX_ExecuterVM_CreateThread(PX_VM *Ins,px_void *userptr)
{
	PX_Object* pObject = (PX_Object*)userptr;
	PX_Object_Executer* pExecute = PX_ObjectGetDesc(PX_Object_Executer, pObject);

	if (PX_VM_STACK(Ins,0).type!=PX_VM_VARIABLE_TYPE_STRING)
	{
		PX_VM_RET(Ins,PX_VM_Variable_int(0));
		return PX_TRUE;
	}

	if(!PX_VMBeginThreadFunction(&pExecute->vm,PX_VMGetFreeThreadId(Ins),PX_VM_STACK(Ins,0)._string.buffer,PX_NULL,0))
	{
		PX_VM_RET(Ins,PX_VM_Variable_int(0));
	}
	return PX_TRUE;
}

px_bool PX_Object_ExecuterRunPayload(PX_Object* pObject, const px_byte bin[],px_int binsize)
{
	PX_Object_Executer* pExecute = PX_ObjectGetDesc(PX_Object_Executer, pObject);

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

static px_bool PX_Object_ExecuterRunScript(PX_Object *pObject,const px_char *pshellstr)
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

	PX_Object_Executer* pExecute = PX_ObjectGetDesc(PX_Object_Executer, pObject);
	if (pObject->Type!=PX_OBJECT_TYPE_EXECUTER)
	{
		PX_ASSERT();
		return PX_FALSE;
	}
	
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



PX_VM_DebuggerMap* PX_Object_ExecuterGetDebugMap(PX_Object* pObject)
{
	if (pObject->Type != PX_OBJECT_TYPE_EXECUTER)
	{
		PX_ASSERT();
		return 0;
	}
	else
	{
		PX_Object_Executer* pExecuter = PX_ObjectGetDesc(PX_Object_Executer, pObject);
		return &pExecuter->debugmap;
	}
}

PX_VM* PX_Object_ExecuterGetVM(PX_Object* pObject)
{
	if (pObject->Type != PX_OBJECT_TYPE_EXECUTER)
	{
		PX_ASSERT();
		return 0;
	}
	else
	{
		PX_Object_Executer* pExecuter = PX_ObjectGetDesc(PX_Object_Executer, pObject);
		return &pExecuter->vm;
	}
}

px_bool PX_ExecuterRegistryHostFunction(PX_Object *pObject,const px_char Name[],PX_VM_Function_Modules function,px_void *userptr)
{
	if (pObject->Type!=PX_OBJECT_TYPE_EXECUTER)
	{
		PX_ASSERT();
		return PX_FALSE;
	}
	else
	{
		PX_Object_Executer* pExecuter=PX_ObjectGetDesc(PX_Object_Executer, pObject);
		return PX_VMRegistHostFunction(&pExecuter->vm, Name, function, userptr);
	}
}

px_void PX_Object_ExecuterUpdate(PX_Object* pObject, px_dword elapsed)
{
	PX_Object_Executer *pdesc = PX_ObjectGetDesc(PX_Object_Executer, pObject);
	PX_VMRun(&pdesc->vm, -1, elapsed);
}

px_void PX_Object_ExecuterRender(px_surface *psurface,PX_Object* pObject, px_dword elapsed)
{
	PX_Object_Executer* pdesc = PX_ObjectGetDesc(PX_Object_Executer, pObject);
	pdesc->printer->Width = pObject->Width;
	pdesc->printer->Height = pObject->Height;


}
px_void PX_Object_ExecuterFree(PX_Object* pObject)
{
	PX_Object_Executer* pdesc = PX_ObjectGetDesc(PX_Object_Executer, pObject);
	if (pdesc->vm._bin)
	PX_VMFree(&pdesc->vm);
	if(pdesc->debugmap.mp)
	PX_VMDebuggerMapFree(&pdesc->debugmap);
}

PX_Object* PX_Object_ExecuterCreate(px_memorypool* mp, PX_Object* Parent, px_int x, px_int y, px_int width, px_int height, PX_FontModule* fm, const px_char shellcode[])
{
	PX_Object* pObject;
	PX_Object_Executer desc, * pdesc;
	//console initialize
	PX_memset(&desc, 0, sizeof(desc));
	desc.fm = fm;

	pObject = PX_ObjectCreateEx(mp, Parent, 0, 0, 0, (px_float)width, (px_float)height, 0, PX_OBJECT_TYPE_EXECUTER, PX_Object_ExecuterUpdate, 0, PX_Object_ExecuterFree, &desc, sizeof(desc));
	pdesc = PX_ObjectGetDesc(PX_Object_Executer, pObject);
	pdesc->printer = PX_Object_PrinterCreate(mp, pObject, x, y, width, height, fm);
	

	if (!PX_Object_ExecuterRunScript(pObject,shellcode))
	{
		PX_ObjectDelete(pObject);
		return PX_NULL;
	}
	PX_Object_ExecuterPrintText(pObject, "----------------------------------------");
	PX_Object_ExecuterPrintText(pObject, "PainterEngine Console");
	PX_Object_ExecuterPrintText(pObject, "----------------------------------------");

	return pObject;
}

PX_Object* PX_Object_ExecuterCreatePayload(px_memorypool* mp, PX_Object* Parent, px_int x, px_int y, px_int width, px_int height, PX_FontModule* fm, const px_byte bin[],px_int binsize)
{
	PX_Object* pObject;
	PX_Object_Executer desc, * pdesc;
	//console initialize
	PX_memset(&desc, 0, sizeof(desc));
	desc.fm = fm;

	pObject = PX_ObjectCreateEx(mp, Parent, 0, 0, 0, (px_float)width, (px_float)height, 0, PX_OBJECT_TYPE_EXECUTER, PX_Object_ExecuterUpdate, 0, PX_Object_ExecuterFree, &desc, sizeof(desc));
	pdesc = PX_ObjectGetDesc(PX_Object_Executer, pObject);
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
