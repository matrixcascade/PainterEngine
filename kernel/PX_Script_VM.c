#include "PX_Script_VM.h"


static px_void PX_ScriptVM_Error(PX_ScriptVM_Instance *Ins,px_char *log)
{
	PX_ERROR(log);
}

static px_void PX_ScriptVM_LOG(px_char *log)
{
	PX_LOG(log);
}
#ifdef PX_DEBUG_MODE
#include "stdio.h"
#define PX_SCRIPT_VM_LOG  printf
#else
#define PX_SCRIPT_VM_LOG
#endif

static PX_ScriptVM_DebuggerPrint g_scriptVM_printFunc=PX_NULL;
static PX_ScriptVM_DebuggerCommand g_scriptVM_cmdFunc=PX_NULL;

px_bool PX_ScriptVM_InstanceInitialize(PX_ScriptVM_Instance *Ins,px_memorypool *mp,px_byte *code,px_int size)
{
	PX_SCRIPT_ASM_HEADER *header;
	PX_SCRIPT_EXPORT_FUNCTION *pfunc;
	PX_SCRIPT_ASM_HOST_NODE *phost;
	px_int i,j;
	if (size<sizeof(PX_SCRIPT_ASM_HEADER))
	{
		return PX_FALSE;
	}

	
	Ins->user_ptr=PX_NULL;
	Ins->mp=mp;
	header=(PX_SCRIPT_ASM_HEADER *)code;
	Ins->debug=PX_FALSE;
	Ins->VM_Globalsize=header->globalsize;
	Ins->VM_Stacksize=header->stacksize;
	Ins->VM_memsize=header->globalsize+header->stacksize*header->threadcount;
	Ins->maxThreadCount=header->threadcount;
	Ins->T=0;
	
	if (header->threadcount<1)
	{
		return PX_FALSE;
	}

	if (!PX_memequ(header,"PASM",4))
	{
		return PX_FALSE;
	}

	if (PX_crc32(code+8,size-8)!=header->CRC)
	{
		return PX_FALSE;
	}


	if ((Ins->_mem=(PX_SCRIPTVM_VARIABLE *)MP_Malloc(mp,sizeof(PX_SCRIPTVM_VARIABLE)*Ins->VM_memsize))==PX_NULL)
	{
		return PX_FALSE;
	}


	if ((Ins->pThread=(PX_ScriptVM_InstanceThread *)MP_Malloc(mp,sizeof(PX_ScriptVM_InstanceThread)*Ins->maxThreadCount))==PX_NULL)
	{
		return PX_FALSE;
	}

	for (i=0;i<Ins->maxThreadCount;i++)
	{
		PX_memset(Ins->pThread,0,sizeof(PX_ScriptVM_InstanceThread)*Ins->maxThreadCount);
		Ins->pThread[i].Activated=PX_FALSE;
		Ins->pThread[i].suspend=PX_FALSE;
		for (j=0;j<PX_SCRIPTVM_REG_COUNT;j++)
		{
			PX_memset(&Ins->pThread[i].R[j],0,sizeof(PX_SCRIPTVM_VARIABLE));
			Ins->pThread[i].R[j].type=PX_SCRIPTVM_VARIABLE_TYPE_INT;
		}
	}

	for (i=0;i<Ins->VM_memsize;i++)
	{
		PX_memset(&Ins->_mem[i],0,sizeof(PX_SCRIPTVM_VARIABLE));
		Ins->_mem[i].type=PX_SCRIPTVM_VARIABLE_TYPE_INT;
	}


	Ins->funcCount=header->funcCount;
	Ins->hostCount=header->hostCount;
	Ins->_func=PX_NULL;
	Ins->_host=PX_NULL;
	PX_memset(Ins->signal,0,sizeof(Ins->signal));

	if (Ins->funcCount!=0)
	{
		if ((Ins->_func=(PX_SCRIPT_EXPORT_FUNCTION *)MP_Malloc(mp,sizeof(PX_SCRIPT_EXPORT_FUNCTION)*Ins->funcCount))==PX_NULL)
		{
			return PX_FALSE;
		}
		//read export functions
		pfunc=(PX_SCRIPT_EXPORT_FUNCTION *)(code+header->oftfunc);
		for (i=0;i<Ins->funcCount;i++)
		{
			PX_memcpy(&Ins->_func[i],&pfunc[i],sizeof(PX_SCRIPT_EXPORT_FUNCTION));
		}
	}
	
	if(Ins->hostCount!=0)
	{
		if ((Ins->_host=(PX_SCRIPT_ASM_HOST_NODE *)MP_Malloc(mp,sizeof(PX_SCRIPT_ASM_HOST_NODE)*Ins->hostCount))==PX_NULL)
		{
			return PX_FALSE;
		}
		PX_memset(Ins->_host,0,sizeof(PX_SCRIPT_ASM_HOST_NODE)*Ins->hostCount);
		phost=(PX_SCRIPT_ASM_HOST_NODE *)(code+header->ofthost);
		for (i=0;i<Ins->hostCount;i++)
		{
			PX_memcpy(&Ins->_host[i],&phost[i],sizeof(PX_SCRIPT_ASM_HOST_NODE));
			Ins->_host[i].map=PX_NULL;
		}
	}
	

	Ins->stringsize=header->stringSize;
	Ins->memorysize=header->memsize;
	Ins->binsize=header->binsize;
	Ins->Suspend=PX_FALSE;

	

	Ins->_string=(px_char *)MP_Malloc(mp,header->stringSize);
	PX_memcpy(Ins->_string,code+header->oftString,header->stringSize);

	Ins->_memory=(px_byte *)MP_Malloc(mp,header->memsize);
	PX_memcpy(Ins->_memory,code+header->oftmem,header->memsize);

	Ins->_bin=(px_byte *)MP_Malloc(mp,header->binsize);
	PX_memcpy(Ins->_bin,code+header->oftbin,header->binsize);

	

	return PX_TRUE;
}

PX_SCRIPTVM_VARIABLE PX_SCRIPTVM_VaribaleCopy(PX_ScriptVM_Instance *Ins,PX_SCRIPTVM_VARIABLE var,px_bool *bOutOfMemory)
{
	PX_SCRIPTVM_VARIABLE cpyVar;
	cpyVar=var;
	*bOutOfMemory=PX_FALSE;

	if (var.type==PX_SCRIPTVM_VARIABLE_TYPE_STRING)
	{
		if(!PX_StringInitialize(Ins->mp,&cpyVar._string)) 
		{
			cpyVar.type=PX_SCRIPTVM_VARIABLE_TYPE_INT;
			*bOutOfMemory=PX_TRUE;
		}
		else
		{
			if(!PX_StringCopy(&cpyVar._string,&var._string))
			{
				cpyVar.type=PX_SCRIPTVM_VARIABLE_TYPE_INT;
				*bOutOfMemory=PX_TRUE;
			}
		}
	}
	else if (var.type==PX_SCRIPTVM_VARIABLE_TYPE_MEMORY)
	{
		PX_MemoryInitialize(Ins->mp,&cpyVar._memory);
		if(!PX_MemoryCat(&cpyVar._memory,var._memory.buffer,var._memory.usedsize))
		{
			cpyVar.type=PX_SCRIPTVM_VARIABLE_TYPE_INT;
			*bOutOfMemory=PX_TRUE;
		}
	}
	return cpyVar;
}

static px_void PX_SCRIPTVM_VaribaleFree(PX_ScriptVM_Instance *Ins,PX_SCRIPTVM_VARIABLE *var)
{
	if (var->type==PX_SCRIPTVM_VARIABLE_TYPE_MEMORY)
	{
		PX_MemoryFree(&var->_memory);
	}
	else if (var->type==PX_SCRIPTVM_VARIABLE_TYPE_STRING)
	{
		PX_StringFree(&var->_string);
	}
	var->type=PX_SCRIPTVM_VARIABLE_TYPE_INT;
	var->_int=0;
}


static PX_SCRIPTVM_VARIABLE  PX_SCRIPT_VM_GetParamConst(PX_ScriptVM_Instance *Ins,px_char optype,px_int param,px_bool *bOutofMemory)
{
	PX_SCRIPTVM_VARIABLE rVar,ret;
	PX_ScriptVM_InstanceThread *pT=&Ins->pThread[Ins->T];
	*bOutofMemory=PX_FALSE;
	ret.type=PX_SCRIPTVM_VARIABLE_TYPE_INT;
	ret._int=0;
	switch (optype)
	{
	case PX_SCRIPT_ASM_OPTYPE_IP:
		{
			ret.type=PX_SCRIPTVM_VARIABLE_TYPE_INT;
			ret._int=pT->IP;
			return ret;
		}
		break;

	case PX_SCRIPT_ASM_OPTYPE_SP:
		{
			ret.type=PX_SCRIPTVM_VARIABLE_TYPE_INT;
			ret._int=pT->SP;
			return ret;
		}
		break;

	case PX_SCRIPT_ASM_OPTYPE_BP:
		{
			ret.type=PX_SCRIPTVM_VARIABLE_TYPE_INT;
			ret._int=pT->BP;
			return ret;
		}
		break;
	case PX_SCRIPT_ASM_OPTYPE_HOST:
	case PX_SCRIPT_ASM_OPTYPE_INT:
		{
			ret.type=PX_SCRIPTVM_VARIABLE_TYPE_INT;
			ret._int=param;
			return ret;
		}
		break;
	case PX_SCRIPT_ASM_OPTYPE_FLOAT:
		{
			//Must be IEEE 754 format
			ret.type=PX_SCRIPTVM_VARIABLE_TYPE_FLOAT;
			ret._int=param;
			return ret;
		}
		break;
	case PX_SCRIPT_ASM_OPTYPE_STRING:
		{
			ret.type=PX_SCRIPTVM_VARIABLE_TYPE_STRING;
			ret._string.buffer=Ins->_string+param;
			ret._string.bufferlen=0; 
			ret._string.mp=PX_NULL;
			return ret;
		}
		break;

	case PX_SCRIPT_ASM_OPTYPE_MEMORY:
		{
			ret.type=PX_SCRIPTVM_VARIABLE_TYPE_MEMORY;
			ret._memory.buffer=Ins->_memory+param+4;
			ret._memory.allocsize=0;
			ret._memory.usedsize=*(px_int *)(Ins->_memory+param);
			ret._memory.mp=PX_NULL;
			return ret;
		}
		break;

	case PX_SCRIPT_ASM_OPTYPE_REG:
		{
			if (param<PX_SCRIPTVM_REG_COUNT)
			{
				return PX_SCRIPTVM_VaribaleCopy(Ins,pT->R[param],bOutofMemory);
			}
			else
			{
				PX_ScriptVM_Error(Ins,"Variable parameter error.");
			}
		}
		break;
	case PX_SCRIPT_ASM_OPTYPE_LOCAL_CONST:
		{
			if(pT->BP-param>0&&pT->BP-param<Ins->VM_memsize)
				return PX_SCRIPTVM_VaribaleCopy(Ins,Ins->_mem[pT->BP-param],bOutofMemory);
			else
				goto _ERROR;
		}
		break;
	case PX_SCRIPT_ASM_OPTYPE_LOCAL_REGREF:
		{
			if (param<PX_SCRIPTVM_REG_COUNT)
			{
				rVar=pT->R[param];
				if (rVar.type==PX_SCRIPTVM_VARIABLE_TYPE_INT)
				{
					if (pT->BP-rVar._int>0&&pT->BP-rVar._int<Ins->VM_memsize)
					{
						return PX_SCRIPTVM_VaribaleCopy(Ins,Ins->_mem[rVar._int-pT->BP],bOutofMemory);
					}
					else
					{
						goto _ERROR;
					}
				}
				else
				{
					goto _ERROR;
				}
			}
			else
			{
				goto _ERROR;
			}

		}
		break;
	case PX_SCRIPT_ASM_OPTYPE_LOCAL_GLOBALREF:
		{
			if (param<Ins->VM_memsize)
			{
				rVar=Ins->_mem[param];
				if (rVar.type==PX_SCRIPTVM_VARIABLE_TYPE_INT)
				{
					if (pT->BP-rVar._int>0&&pT->BP-rVar._int<Ins->VM_memsize)
					{
						return PX_SCRIPTVM_VaribaleCopy(Ins,Ins->_mem[pT->BP-rVar._int],bOutofMemory);
					}
					else
					{
						goto _ERROR;
					}
				}
				else
				{
					goto _ERROR;
				}
			}
			else
			{
				goto _ERROR;
			}
		}
		break;
	case PX_SCRIPT_ASM_OPTYPE_LOCAL_LOCALREF:
		{
			if (pT->BP-param>0&&pT->BP-param<Ins->VM_memsize)
			{
				rVar=Ins->_mem[pT->BP-param];
				if (rVar.type==PX_SCRIPTVM_VARIABLE_TYPE_INT)
				{
					if (pT->BP-rVar._int>0&&pT->BP-rVar._int<Ins->VM_memsize)
					{
						return PX_SCRIPTVM_VaribaleCopy(Ins,Ins->_mem[pT->BP-rVar._int],bOutofMemory);
					}
					else
					{
						goto _ERROR;
					}
				}
				else
				{
					goto _ERROR;
				}
			}
			else
			{
				goto _ERROR;
			}
		}
		break;
	case PX_SCRIPT_ASM_OPTYPE_GLOBAL_CONST:
		{
			if(param<Ins->VM_memsize)
				return PX_SCRIPTVM_VaribaleCopy(Ins,Ins->_mem[param],bOutofMemory);
			else
				goto _ERROR;
		}
		break;
	case PX_SCRIPT_ASM_OPTYPE_GLOBAL_SPREF:
		{
			if(pT->SP+param<Ins->VM_memsize)
				return PX_SCRIPTVM_VaribaleCopy(Ins,Ins->_mem[pT->SP+param],bOutofMemory);
			else
				goto _ERROR;
		}
		break;
	case PX_SCRIPT_ASM_OPTYPE_GLOBAL_REGREF:
		{
			if (param<PX_SCRIPTVM_REG_COUNT)
			{
				rVar=pT->R[param];
				if (rVar.type==PX_SCRIPTVM_VARIABLE_TYPE_INT)
				{
					if (rVar._int<Ins->VM_memsize)
					{
						return PX_SCRIPTVM_VaribaleCopy(Ins,Ins->_mem[rVar._int],bOutofMemory);
					}
					else
					{
						goto _ERROR;
					}
				}
				else
				{
					goto _ERROR;
				}
			}
			else
			{
				goto _ERROR;
			}
		}
		break;
	case PX_SCRIPT_ASM_OPTYPE_GLOBAL_GLOBALREF:
		{
			if (param<Ins->VM_memsize)
			{
				rVar=Ins->_mem[param];
				if (rVar.type==PX_SCRIPTVM_VARIABLE_TYPE_INT)
				{
					if (rVar._int<Ins->VM_memsize)
					{
						return PX_SCRIPTVM_VaribaleCopy(Ins,Ins->_mem[rVar._int],bOutofMemory);
					}
					else
					{
						goto _ERROR;
					}
				}
				else
				{
					goto _ERROR;
				}
			}
			else
			{
				goto _ERROR;
			}
		}
		break;
	case PX_SCRIPT_ASM_OPTYPE_GLOBAL_LOCALREF:
		{
			if (pT->BP-param>0&&pT->BP-param<Ins->VM_memsize)
			{
				rVar=Ins->_mem[pT->BP-param];
				if (rVar.type==PX_SCRIPTVM_VARIABLE_TYPE_INT)
				{
					if (rVar._int<Ins->VM_memsize)
					{
						return PX_SCRIPTVM_VaribaleCopy(Ins,Ins->_mem[rVar._int],bOutofMemory);
					}
					else
					{
						goto _ERROR;
					}
				}
				else
				{
					goto _ERROR;
				}
			}
			else
			{
				goto _ERROR;
			}
		}
		break;
	

	default:
		{
			goto _ERROR;
		}
	}
_ERROR:
	PX_ScriptVM_Error(Ins,"Virtual Machine Runtime error");
	return ret;

}
static PX_SCRIPTVM_VARIABLE * PX_SCRIPT_VM_GetVariablePointer(PX_ScriptVM_Instance *Ins,px_char optype,px_int param)
{
	PX_SCRIPTVM_VARIABLE rVar;
	PX_ScriptVM_InstanceThread *pT=&Ins->pThread[Ins->T];
	switch (optype)
	{
	case PX_SCRIPT_ASM_OPTYPE_REG:
		{
			if (param<PX_SCRIPTVM_REG_COUNT)
			{
				return &pT->R[param];
			}
			else
			{
				PX_ScriptVM_Error(Ins,"Variable parameter error.");
			}
		}
		break;
	case PX_SCRIPT_ASM_OPTYPE_LOCAL_CONST:
		{
			if(pT->BP-param>0&&pT->BP-param<Ins->VM_memsize)
			return &Ins->_mem[pT->BP-param];
			else
				goto _ERROR;
		}
		break;
	case PX_SCRIPT_ASM_OPTYPE_LOCAL_REGREF:
		{
			if (param<PX_SCRIPTVM_REG_COUNT)
			{
				rVar=pT->R[param];
				if (rVar.type==PX_SCRIPTVM_VARIABLE_TYPE_INT)
				{
					if (pT->BP-rVar._int>0&&pT->BP-rVar._int<Ins->VM_memsize)
					{
						return &Ins->_mem[pT->BP-rVar._int];
					}
					else
					{
						goto _ERROR;
					}
				}
				else
				{
					goto _ERROR;
				}
			}
			else
			{
				goto _ERROR;
			}
			
		}
		break;
	case PX_SCRIPT_ASM_OPTYPE_LOCAL_GLOBALREF:
		{
			if (param<Ins->VM_memsize)
			{
				rVar=Ins->_mem[param];
				if (rVar.type==PX_SCRIPTVM_VARIABLE_TYPE_INT)
				{
					if (pT->BP-rVar._int>0&&pT->BP-rVar._int<Ins->VM_memsize)
					{
						return &Ins->_mem[pT->BP-rVar._int];
					}
					else
					{
						goto _ERROR;
					}
				}
				else
				{
					goto _ERROR;
				}
			}
			else
			{
				goto _ERROR;
			}
		}
		break;
	case PX_SCRIPT_ASM_OPTYPE_LOCAL_LOCALREF:
		{
			if (pT->BP-param>0&&pT->BP-param<Ins->VM_memsize)
			{
				rVar=Ins->_mem[pT->BP-param];
				if (rVar.type==PX_SCRIPTVM_VARIABLE_TYPE_INT)
				{
					if (pT->BP-rVar._int>0&&pT->BP-rVar._int<Ins->VM_memsize)
					{
						return &Ins->_mem[pT->BP-rVar._int];
					}
					else
					{
						goto _ERROR;
					}
				}
				else
				{
					goto _ERROR;
				}
			}
			else
			{
				goto _ERROR;
			}
		}
		break;
	case PX_SCRIPT_ASM_OPTYPE_GLOBAL_CONST:
		{
			if(param<Ins->VM_memsize)
				return &Ins->_mem[param];
			else
				goto _ERROR;
		}
		break;

	case PX_SCRIPT_ASM_OPTYPE_GLOBAL_SPREF:
		{
			if(pT->SP+param<Ins->VM_memsize)
				return &Ins->_mem[pT->SP+param];
			else
				goto _ERROR;
		}
		break;

	case PX_SCRIPT_ASM_OPTYPE_GLOBAL_REGREF:
		{
			if (param<PX_SCRIPTVM_REG_COUNT)
			{
				rVar=pT->R[param];
				if (rVar.type==PX_SCRIPTVM_VARIABLE_TYPE_INT)
				{
					if (rVar._int<Ins->VM_memsize)
					{
						return &Ins->_mem[rVar._int];
					}
					else
					{
						goto _ERROR;
					}
				}
				else
				{
					goto _ERROR;
				}
			}
			else
			{
				goto _ERROR;
			}
		}
		break;
	case PX_SCRIPT_ASM_OPTYPE_GLOBAL_GLOBALREF:
		{
			if (param<Ins->VM_memsize)
			{
				rVar=Ins->_mem[param];
				if (rVar.type==PX_SCRIPTVM_VARIABLE_TYPE_INT)
				{
					if (rVar._int<Ins->VM_memsize)
					{
						return &Ins->_mem[rVar._int];
					}
					else
					{
						goto _ERROR;
					}
				}
				else
				{
					goto _ERROR;
				}
			}
			else
			{
				goto _ERROR;
			}
		}
		break;
	case PX_SCRIPT_ASM_OPTYPE_GLOBAL_LOCALREF:
		{
			if (pT->BP-param>0&&pT->BP-param<Ins->VM_memsize)
			{
				rVar=Ins->_mem[pT->BP-param];
				if (rVar.type==PX_SCRIPTVM_VARIABLE_TYPE_INT)
				{
					if (rVar._int<Ins->VM_memsize)
					{
						return &Ins->_mem[rVar._int];
					}
					else
					{
						goto _ERROR;
					}
				}
				else
				{
					goto _ERROR;
				}
			}
			else
			{
				goto _ERROR;
			}
		}
		break;

	default:
		{
			goto _ERROR;
		}
	}
	return PX_NULL;
_ERROR:
	PX_ScriptVM_Error(Ins,"Variable parameter error.");
	return PX_NULL;
	
}

#define PX_SCRIPT_VM_PARAM(i) (*(((px_int32 *)(Ins->_bin+pT->IP+4))+i))
#define PX_SCRIPT_VM_IP_PARAM(i) (*(((px_int32 *)(Ins->_bin+IP+4))+i))

px_void PX_ScriptVM_DebuggerInterrupt(PX_ScriptVM_Instance *Ins);
px_bool PX_ScriptVM_DebuggerCurrentInstruction(PX_ScriptVM_Instance *Ins,px_int IP);

PX_SCRIPTVM_RUNRETURN PX_ScriptVM_InstanceRunThread(PX_ScriptVM_Instance *Ins,px_int tick)
{
	px_char opCode,opType[3],numOut[32],lastchar,*pchar;
	px_float p1=0,p2=0;
	PX_SCRIPTVM_VARIABLE *pVar,cVar,sVar,tVar;
	px_string newString;
	px_byte *newBuffer;
	px_bool bOutofMemory;
	int i,j;
	PX_ScriptVM_InstanceThread *pT=&Ins->pThread[Ins->T];
	
	while(PX_TRUE)
	{
		if(Ins->debug)
		{
			PX_ScriptVM_DebuggerInterrupt(Ins);
		}
		
		if (!pT->Activated)
		{
			return PX_SCRIPTVM_RUNRETURN_END;
		}

		if (Ins->Suspend||pT->suspend || pT->sleep)
		{
			return PX_SCRIPTVM_RUNRETURN_SUSPEND;
		}
		

		pVar=PX_NULL;
		cVar.type=PX_SCRIPTVM_VARIABLE_TYPE_INT;
		sVar.type=PX_SCRIPTVM_VARIABLE_TYPE_INT;
		tVar.type=PX_SCRIPTVM_VARIABLE_TYPE_INT;
		cVar._int=-1;
		sVar._int=-1;
		tVar._int=-1;

	
		if(tick==0)
		{
			PX_SCRIPTVM_VaribaleFree(Ins,&cVar);
			PX_SCRIPTVM_VaribaleFree(Ins,&sVar);
			PX_SCRIPTVM_VaribaleFree(Ins,&tVar);
			return PX_SCRIPTVM_RUNRETURN_TIMEOUT;
		}

		if(tick>0)
		{
			tick--;
		}


		if (pT->IP>Ins->binsize||pT->IP<0)
		{
			PX_ScriptVM_Error(Ins,"IP point to invalid address");
			goto _ERROR;
		}

		if (pT->SP>Ins->VM_memsize||pT->SP<0)
		{
			PX_ScriptVM_Error(Ins,"SP point to invalid address");
			goto _ERROR;
		}

	opCode=*(Ins->_bin+pT->IP);
	opType[0]=*(Ins->_bin+pT->IP+1);
	opType[1]=*(Ins->_bin+pT->IP+2);
	opType[2]=*(Ins->_bin+pT->IP+3);

	if (opCode!=PX_SCRIPT_ASM_INSTR_OPCODE_MOV&&opCode!=PX_SCRIPT_ASM_INSTR_OPCODE_ADD&&opCode!=PX_SCRIPT_ASM_INSTR_OPCODE_SUB&&opCode!=PX_SCRIPT_ASM_INSTR_OPCODE_PUSH&&opCode!=PX_SCRIPT_ASM_INSTR_OPCODE_POP)
	{
		if (opType[0]==PX_SCRIPT_ASM_OPTYPE_IP||opType[0]==PX_SCRIPT_ASM_OPTYPE_SP||opType[0]==PX_SCRIPT_ASM_OPTYPE_BP)
		{
			goto _ERROR;
		}
	}
	switch(opCode)
	{
	case PX_SCRIPT_ASM_INSTR_OPCODE_MOV:
		{
			cVar=PX_SCRIPT_VM_GetParamConst(Ins,opType[1],PX_SCRIPT_VM_PARAM(1),&bOutofMemory);
			if(bOutofMemory) 
			{
				PX_ScriptVM_Error(Ins,"MOV out of memory.");
				goto _ERROR;
			}
			if (opType[0]==PX_SCRIPT_ASM_OPTYPE_IP)
			{
				if (cVar.type!=PX_SCRIPTVM_VARIABLE_TYPE_INT)
				{
					PX_ScriptVM_Error(Ins,"MOV IP is not a integer.");
					goto _ERROR;
				}
				pT->IP=cVar._int;
				break;
			}

			if (opType[0]==PX_SCRIPT_ASM_OPTYPE_SP)
			{

				if (cVar.type!=PX_SCRIPTVM_VARIABLE_TYPE_INT)
				{
					PX_ScriptVM_Error(Ins,"MOV SP is not a integer.");
					goto _ERROR;
				}
				pT->SP=cVar._int;
				pT->IP+=(4+2*4);
				break;
			}

			if (opType[0]==PX_SCRIPT_ASM_OPTYPE_BP)
			{
				if (cVar.type!=PX_SCRIPTVM_VARIABLE_TYPE_INT)
				{
					PX_ScriptVM_Error(Ins,"MOV BP is not a integer.");
					goto _ERROR;
				}
				pT->BP=cVar._int;
				pT->IP+=(4+2*4);
				break;
			}

			pVar=PX_SCRIPT_VM_GetVariablePointer(Ins,opType[0],PX_SCRIPT_VM_PARAM(0));
			
			PX_SCRIPTVM_VaribaleFree(Ins,pVar);
			
			if (cVar.type==PX_SCRIPTVM_VARIABLE_TYPE_STRING)
			{
				if(!PX_StringInitialize(Ins->mp,&pVar->_string)) 
				{
					PX_ScriptVM_Error(Ins,"MOV crash.");
					goto _ERROR;
				}
				if(!PX_StringCat(&pVar->_string,cVar._string.buffer)) 
				{
					PX_ScriptVM_Error(Ins,"MOV crash.");
					goto _ERROR;
				}
				pVar->type=cVar.type;
			}
			else if (cVar.type==PX_SCRIPTVM_VARIABLE_TYPE_MEMORY)
			{
				PX_MemoryInitialize(Ins->mp,&pVar->_memory);
				if(!PX_MemoryAlloc(&pVar->_memory,cVar._memory.usedsize)) 
				{
					PX_ScriptVM_Error(Ins,"MOV crash.");
					goto _ERROR;
				}
				if(!PX_MemoryCat(&pVar->_memory,cVar._memory.buffer,cVar._memory.usedsize))
				{
					PX_ScriptVM_Error(Ins,"MOV crash.");
					goto _ERROR;
				}
				pVar->type=cVar.type;
			}
			else
			{
				*pVar=cVar;
			}
			
			pT->IP+=(4+2*4);
		}
		break;
	case PX_SCRIPT_ASM_INSTR_OPCODE_ADD:
		{
			cVar=PX_SCRIPT_VM_GetParamConst(Ins,opType[1],PX_SCRIPT_VM_PARAM(1),&bOutofMemory);
			if(bOutofMemory) 
			{
				PX_ScriptVM_Error(Ins,"Add crash.");
				goto _ERROR;
			}
			if (opType[0]==PX_SCRIPT_ASM_OPTYPE_IP)
			{
				if (cVar.type!=PX_SCRIPTVM_VARIABLE_TYPE_INT)
				{
					PX_ScriptVM_Error(Ins,"ADD IP is not a integer.");
					goto _ERROR;
				}
				pT->IP+=cVar._int;
				break;
			}

			if (opType[0]==PX_SCRIPT_ASM_OPTYPE_SP)
			{
				if (cVar.type!=PX_SCRIPTVM_VARIABLE_TYPE_INT)
				{
					PX_ScriptVM_Error(Ins,"ADD SP is not a integer.");
					goto _ERROR;
				}
				pT->SP+=cVar._int;
				pT->IP+=(4+2*4);
				break;
			}

			if (opType[0]==PX_SCRIPT_ASM_OPTYPE_BP)
			{
				if (cVar.type!=PX_SCRIPTVM_VARIABLE_TYPE_INT)
				{
					PX_ScriptVM_Error(Ins,"ADD BP is not a integer.");
					goto _ERROR;
				}
				pT->BP+=cVar._int;
				pT->IP+=(4+2*4);
				break;
			}
			pVar=PX_SCRIPT_VM_GetVariablePointer(Ins,opType[0],PX_SCRIPT_VM_PARAM(0));

			if (pVar->type==PX_SCRIPTVM_VARIABLE_TYPE_INT&&cVar.type==PX_SCRIPTVM_VARIABLE_TYPE_INT)
			{
				pVar->_int+=cVar._int;
				pT->IP+=(4+2*4);
				break;
			}
			if (pVar->type==PX_SCRIPTVM_VARIABLE_TYPE_INT&&cVar.type==PX_SCRIPTVM_VARIABLE_TYPE_FLOAT)
			{
				pVar->_float=pVar->_int+cVar._float;
				pVar->type=PX_SCRIPTVM_VARIABLE_TYPE_FLOAT;
				pT->IP+=(4+2*4);
				break;
			}
			if (pVar->type==PX_SCRIPTVM_VARIABLE_TYPE_FLOAT&&cVar.type==PX_SCRIPTVM_VARIABLE_TYPE_FLOAT)
			{
				pVar->_float=pVar->_float+cVar._float;
				pT->IP+=(4+2*4);
				break;
			}
			if (pVar->type==PX_SCRIPTVM_VARIABLE_TYPE_FLOAT&&cVar.type==PX_SCRIPTVM_VARIABLE_TYPE_INT)
			{
				pVar->_float=pVar->_float+cVar._int;
				pT->IP+=(4+2*4);
				break;
			}
			PX_ScriptVM_Error(Ins,"ADD Invalid instr");
			goto _ERROR;
		}
		break;
	case PX_SCRIPT_ASM_INSTR_OPCODE_SUB:
		{
			cVar=PX_SCRIPT_VM_GetParamConst(Ins,opType[1],PX_SCRIPT_VM_PARAM(1),&bOutofMemory);
			if(bOutofMemory) 
			{
				PX_ScriptVM_Error(Ins,"SUB out of memory.");
				goto _ERROR;
			}
			if (opType[0]==PX_SCRIPT_ASM_OPTYPE_IP)
			{
				if (cVar.type!=PX_SCRIPTVM_VARIABLE_TYPE_INT)
				{
					PX_ScriptVM_Error(Ins,"SUB IP is not a integer.");
					goto _ERROR;
				}
				pT->IP-=cVar._int;
				break;
			}

			if (opType[0]==PX_SCRIPT_ASM_OPTYPE_SP)
			{
				if (cVar.type!=PX_SCRIPTVM_VARIABLE_TYPE_INT)
				{
					PX_ScriptVM_Error(Ins,"SUB SP is not a integer.");
					goto _ERROR;
				}
				pT->SP-=cVar._int;
				if (pT->SP<0)
				{
					PX_ScriptVM_Error(Ins,"SUB SP<0.");
					goto _ERROR;
				}
				pT->IP+=(4+2*4);
				break;
			}

			if (opType[0]==PX_SCRIPT_ASM_OPTYPE_BP)
			{
				if (cVar.type!=PX_SCRIPTVM_VARIABLE_TYPE_INT)
				{
					PX_ScriptVM_Error(Ins,"SUB BP is not a integer.");
					goto _ERROR;
				}
				pT->BP-=cVar._int;
				pT->IP+=(4+2*4);
				break;
			}
			pVar=PX_SCRIPT_VM_GetVariablePointer(Ins,opType[0],PX_SCRIPT_VM_PARAM(0));
			if (pVar->type!=PX_SCRIPTVM_VARIABLE_TYPE_INT&&pVar->type!=PX_SCRIPTVM_VARIABLE_TYPE_FLOAT)
			{
				PX_ScriptVM_Error(Ins,"SUB parameters error.");
				goto _ERROR;
			}
			if (cVar.type!=PX_SCRIPTVM_VARIABLE_TYPE_INT&&cVar.type!=PX_SCRIPTVM_VARIABLE_TYPE_FLOAT)
			{
				PX_ScriptVM_Error(Ins,"SUB parameters error.");
				goto _ERROR;
			}
			if (pVar->type==PX_SCRIPTVM_VARIABLE_TYPE_INT&&cVar.type==PX_SCRIPTVM_VARIABLE_TYPE_INT)
			{
				pVar->_int-=cVar._int;
			}
			else if (pVar->type==PX_SCRIPTVM_VARIABLE_TYPE_INT&&cVar.type==PX_SCRIPTVM_VARIABLE_TYPE_FLOAT)
			{
				pVar->_float=pVar->_int-cVar._float;
				pVar->type=PX_SCRIPTVM_VARIABLE_TYPE_FLOAT;
			}
			else if (pVar->type==PX_SCRIPTVM_VARIABLE_TYPE_FLOAT&&cVar.type==PX_SCRIPTVM_VARIABLE_TYPE_FLOAT)
			{
				pVar->_float=pVar->_float-cVar._float;
			}
			else if (pVar->type==PX_SCRIPTVM_VARIABLE_TYPE_FLOAT&&cVar.type==PX_SCRIPTVM_VARIABLE_TYPE_INT)
			{
				pVar->_float=pVar->_float-cVar._int;
			}
			pT->IP+=(4+2*4);
		}
		break;
	case PX_SCRIPT_ASM_INSTR_OPCODE_DIV:
		{
			pVar=PX_SCRIPT_VM_GetVariablePointer(Ins,opType[0],PX_SCRIPT_VM_PARAM(0));
			cVar=PX_SCRIPT_VM_GetParamConst(Ins,opType[1],PX_SCRIPT_VM_PARAM(1),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			if (cVar._int==0)
			{
				PX_ScriptVM_Error(Ins,"Divide by zero error");
				goto _ERROR;
			}
			if (pVar->type!=PX_SCRIPTVM_VARIABLE_TYPE_INT&&pVar->type!=PX_SCRIPTVM_VARIABLE_TYPE_FLOAT)
			{
				PX_ScriptVM_Error(Ins,"DIV parameters error.");
				goto _ERROR;
			}
			if (cVar.type!=PX_SCRIPTVM_VARIABLE_TYPE_INT&&cVar.type!=PX_SCRIPTVM_VARIABLE_TYPE_FLOAT)
			{
				PX_ScriptVM_Error(Ins,"DIV parameters error.");
				goto _ERROR;
			}
			if (pVar->type==PX_SCRIPTVM_VARIABLE_TYPE_INT&&cVar.type==PX_SCRIPTVM_VARIABLE_TYPE_INT)
			{
				pVar->_int/=cVar._int;
			}
			else if (pVar->type==PX_SCRIPTVM_VARIABLE_TYPE_INT&&cVar.type==PX_SCRIPTVM_VARIABLE_TYPE_FLOAT)
			{
				pVar->_float=pVar->_int/cVar._float;
				pVar->type=PX_SCRIPTVM_VARIABLE_TYPE_FLOAT;
			}
			else if (pVar->type==PX_SCRIPTVM_VARIABLE_TYPE_FLOAT&&cVar.type==PX_SCRIPTVM_VARIABLE_TYPE_FLOAT)
			{
				pVar->_float=pVar->_float/cVar._float;
			}
			else if (pVar->type==PX_SCRIPTVM_VARIABLE_TYPE_FLOAT&&cVar.type==PX_SCRIPTVM_VARIABLE_TYPE_INT)
			{
				pVar->_float=pVar->_float/cVar._int;
			}
			pT->IP+=(4+2*4);
		}
		break;
	case PX_SCRIPT_ASM_INSTR_OPCODE_MUL:
		{
			pVar=PX_SCRIPT_VM_GetVariablePointer(Ins,opType[0],PX_SCRIPT_VM_PARAM(0));
			cVar=PX_SCRIPT_VM_GetParamConst(Ins,opType[1],PX_SCRIPT_VM_PARAM(1),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			if (pVar->type!=PX_SCRIPTVM_VARIABLE_TYPE_INT&&pVar->type!=PX_SCRIPTVM_VARIABLE_TYPE_FLOAT)
			{
				PX_ScriptVM_Error(Ins,"MUL parameters error.");
				goto _ERROR;
			}
			if (cVar.type!=PX_SCRIPTVM_VARIABLE_TYPE_INT&&cVar.type!=PX_SCRIPTVM_VARIABLE_TYPE_FLOAT)
			{
				PX_ScriptVM_Error(Ins,"MUL parameters error.");
				goto _ERROR;
			}
			if (pVar->type==PX_SCRIPTVM_VARIABLE_TYPE_INT&&cVar.type==PX_SCRIPTVM_VARIABLE_TYPE_INT)
			{
				pVar->_int*=cVar._int;
			}
			else if (pVar->type==PX_SCRIPTVM_VARIABLE_TYPE_INT&&cVar.type==PX_SCRIPTVM_VARIABLE_TYPE_FLOAT)
			{
				pVar->_float=pVar->_int*cVar._float;
				pVar->type=PX_SCRIPTVM_VARIABLE_TYPE_FLOAT;
			}
			else if (pVar->type==PX_SCRIPTVM_VARIABLE_TYPE_FLOAT&&cVar.type==PX_SCRIPTVM_VARIABLE_TYPE_FLOAT)
			{
				pVar->_float=pVar->_float*cVar._float;
			}
			else if (pVar->type==PX_SCRIPTVM_VARIABLE_TYPE_FLOAT&&cVar.type==PX_SCRIPTVM_VARIABLE_TYPE_INT)
			{
				pVar->_float=pVar->_float*cVar._int;
			}
			pT->IP+=(4+2*4);
		}
		break;
	case PX_SCRIPT_ASM_INSTR_OPCODE_MOD:
		{
			pVar=PX_SCRIPT_VM_GetVariablePointer(Ins,opType[0],PX_SCRIPT_VM_PARAM(0));
			cVar=PX_SCRIPT_VM_GetParamConst(Ins,opType[1],PX_SCRIPT_VM_PARAM(1),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			if (pVar->type!=PX_SCRIPTVM_VARIABLE_TYPE_INT&&cVar.type!=PX_SCRIPTVM_VARIABLE_TYPE_INT)
			{
				PX_ScriptVM_Error(Ins,"MOD parameters error.");
				goto _ERROR;
			}
			pVar->_int%=cVar._int;
			pT->IP+=(4+2*4);
		}
		break;
	case PX_SCRIPT_ASM_INSTR_OPCODE_SHL:
		{
			pVar=PX_SCRIPT_VM_GetVariablePointer(Ins,opType[0],PX_SCRIPT_VM_PARAM(0));
			cVar=PX_SCRIPT_VM_GetParamConst(Ins,opType[1],PX_SCRIPT_VM_PARAM(1),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			if (pVar->type!=PX_SCRIPTVM_VARIABLE_TYPE_INT&&cVar.type!=PX_SCRIPTVM_VARIABLE_TYPE_INT)
			{
				PX_ScriptVM_Error(Ins,"SHL parameters error.");
				goto _ERROR;
			}
			pVar->_int<<=cVar._int;
			pT->IP+=(4+2*4);
		}
		break;
	case PX_SCRIPT_ASM_INSTR_OPCODE_SHR:
		{
			pVar=PX_SCRIPT_VM_GetVariablePointer(Ins,opType[0],PX_SCRIPT_VM_PARAM(0));
			cVar=PX_SCRIPT_VM_GetParamConst(Ins,opType[1],PX_SCRIPT_VM_PARAM(1),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			if (pVar->type!=PX_SCRIPTVM_VARIABLE_TYPE_INT&&cVar.type!=PX_SCRIPTVM_VARIABLE_TYPE_INT)
			{
				PX_ScriptVM_Error(Ins,"SHR parameters error.");
				goto _ERROR;
			}
			pVar->_int>>=cVar._int;
			pT->IP+=(4+2*4);
		}
		break;
	case PX_SCRIPT_ASM_INSTR_OPCODE_AND:
		{
			pVar=PX_SCRIPT_VM_GetVariablePointer(Ins,opType[0],PX_SCRIPT_VM_PARAM(0));
			cVar=PX_SCRIPT_VM_GetParamConst(Ins,opType[1],PX_SCRIPT_VM_PARAM(1),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			if (pVar->type!=PX_SCRIPTVM_VARIABLE_TYPE_INT&&cVar.type!=PX_SCRIPTVM_VARIABLE_TYPE_INT)
			{
				PX_ScriptVM_Error(Ins,"AND parameters error.");
				goto _ERROR;
			}
			pVar->_int&=cVar._int;
			pT->IP+=(4+2*4);
		}
		break;
	case PX_SCRIPT_ASM_INSTR_OPCODE_ANDL:
		{
			pVar=PX_SCRIPT_VM_GetVariablePointer(Ins,opType[0],PX_SCRIPT_VM_PARAM(0));
			cVar=PX_SCRIPT_VM_GetParamConst(Ins,opType[1],PX_SCRIPT_VM_PARAM(1),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			if (pVar->type!=PX_SCRIPTVM_VARIABLE_TYPE_INT&&pVar->type!=PX_SCRIPTVM_VARIABLE_TYPE_FLOAT)
			{
				PX_ScriptVM_Error(Ins,"ANDL parameters error.");
				goto _ERROR;
			}
			if (cVar.type!=PX_SCRIPTVM_VARIABLE_TYPE_INT&&cVar.type!=PX_SCRIPTVM_VARIABLE_TYPE_FLOAT)
			{
				PX_ScriptVM_Error(Ins,"ANDL parameters error.");
				goto _ERROR;
			}
			pVar->type=PX_SCRIPTVM_VARIABLE_TYPE_INT;
			pVar->_int=(pVar->_int&&cVar._int);
			pT->IP+=(4+2*4);
		}
		break;
		case PX_SCRIPT_ASM_INSTR_OPCODE_ORL:
		{
			pVar=PX_SCRIPT_VM_GetVariablePointer(Ins,opType[0],PX_SCRIPT_VM_PARAM(0));
			cVar=PX_SCRIPT_VM_GetParamConst(Ins,opType[1],PX_SCRIPT_VM_PARAM(1),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			if (pVar->type!=PX_SCRIPTVM_VARIABLE_TYPE_INT&&pVar->type!=PX_SCRIPTVM_VARIABLE_TYPE_FLOAT)
			{
				PX_ScriptVM_Error(Ins,"ORL parameters error.");
				goto _ERROR;
			}
			if (cVar.type!=PX_SCRIPTVM_VARIABLE_TYPE_INT&&cVar.type!=PX_SCRIPTVM_VARIABLE_TYPE_FLOAT)
			{
				PX_ScriptVM_Error(Ins,"ORL parameters error.");
				goto _ERROR;
			}
			pVar->type=PX_SCRIPTVM_VARIABLE_TYPE_INT;
			pVar->_int=(pVar->_int||cVar._int);
			pT->IP+=(4+2*4);
		}
		break;

	case PX_SCRIPT_ASM_INSTR_OPCODE_OR:
		{
			pVar=PX_SCRIPT_VM_GetVariablePointer(Ins,opType[0],PX_SCRIPT_VM_PARAM(0));
			cVar=PX_SCRIPT_VM_GetParamConst(Ins,opType[1],PX_SCRIPT_VM_PARAM(1),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			if (pVar->type!=PX_SCRIPTVM_VARIABLE_TYPE_INT&&pVar->type!=PX_SCRIPTVM_VARIABLE_TYPE_FLOAT)
			{
				PX_ScriptVM_Error(Ins,"OR parameters error.");
				goto _ERROR;
			}
			if (cVar.type!=PX_SCRIPTVM_VARIABLE_TYPE_INT&&cVar.type!=PX_SCRIPTVM_VARIABLE_TYPE_FLOAT)
			{
				PX_ScriptVM_Error(Ins,"OR parameters error.");
				goto _ERROR;
			}
			pVar->_int|=cVar._int;
			pVar->type=PX_SCRIPTVM_VARIABLE_TYPE_INT;
			pT->IP+=(4+2*4);
		}
		break;
	case PX_SCRIPT_ASM_INSTR_OPCODE_XOR:
		{
			pVar=PX_SCRIPT_VM_GetVariablePointer(Ins,opType[0],PX_SCRIPT_VM_PARAM(0));
			cVar=PX_SCRIPT_VM_GetParamConst(Ins,opType[1],PX_SCRIPT_VM_PARAM(1),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			if (pVar->type!=PX_SCRIPTVM_VARIABLE_TYPE_INT&&pVar->type!=PX_SCRIPTVM_VARIABLE_TYPE_FLOAT)
			{
				PX_ScriptVM_Error(Ins,"XOR parameters error.");
				goto _ERROR;
			}
			if (cVar.type!=PX_SCRIPTVM_VARIABLE_TYPE_INT&&cVar.type!=PX_SCRIPTVM_VARIABLE_TYPE_FLOAT)
			{
				PX_ScriptVM_Error(Ins,"XOR parameters error.");
				goto _ERROR;
			}
			pVar->_int^=cVar._int;
			pVar->type=PX_SCRIPTVM_VARIABLE_TYPE_INT;
			pT->IP+=(4+2*4);
		}
		break;
	case PX_SCRIPT_ASM_INSTR_OPCODE_POW:
		{
			pVar=PX_SCRIPT_VM_GetVariablePointer(Ins,opType[0],PX_SCRIPT_VM_PARAM(0));
			cVar=PX_SCRIPT_VM_GetParamConst(Ins,opType[1],PX_SCRIPT_VM_PARAM(1),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			if (pVar->type!=PX_SCRIPTVM_VARIABLE_TYPE_INT&&pVar->type!=PX_SCRIPTVM_VARIABLE_TYPE_FLOAT)
			{
				PX_ScriptVM_Error(Ins,"POW parameters error.");
				goto _ERROR;
			}
			if (cVar.type!=PX_SCRIPTVM_VARIABLE_TYPE_INT&&cVar.type!=PX_SCRIPTVM_VARIABLE_TYPE_FLOAT)
			{
				PX_ScriptVM_Error(Ins,"POW parameters error.");
				goto _ERROR;
			}

			if (pVar->type==PX_SCRIPTVM_VARIABLE_TYPE_INT&&cVar.type==PX_SCRIPTVM_VARIABLE_TYPE_INT)
			{
				pVar->_int=PX_pow_ii(pVar->_int,cVar._int);
			}
			else if (pVar->type==PX_SCRIPTVM_VARIABLE_TYPE_INT&&cVar.type==PX_SCRIPTVM_VARIABLE_TYPE_FLOAT)
			{
				pVar->_float=(px_float)PX_pow(pVar->_int,cVar._float);
				pVar->type=PX_SCRIPTVM_VARIABLE_TYPE_FLOAT;
			}
			else if (pVar->type==PX_SCRIPTVM_VARIABLE_TYPE_FLOAT&&cVar.type==PX_SCRIPTVM_VARIABLE_TYPE_FLOAT)
			{
				pVar->_float=(px_float)PX_pow(pVar->_float,cVar._float);
			}
			else if (pVar->type==PX_SCRIPTVM_VARIABLE_TYPE_FLOAT&&cVar.type==PX_SCRIPTVM_VARIABLE_TYPE_INT)
			{
				pVar->_float=(px_float)PX_pow(pVar->_float,cVar._int);
			}
			pT->IP+=(4+2*4);
		}
		break;
	case PX_SCRIPT_ASM_INSTR_OPCODE_SIN:
		{
			pVar=PX_SCRIPT_VM_GetVariablePointer(Ins,opType[0],PX_SCRIPT_VM_PARAM(0));
			cVar=PX_SCRIPT_VM_GetParamConst(Ins,opType[1],PX_SCRIPT_VM_PARAM(1),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			if (cVar.type!=PX_SCRIPTVM_VARIABLE_TYPE_FLOAT&&cVar.type!=PX_SCRIPTVM_VARIABLE_TYPE_INT)
			{
				goto _ERROR;
			}
			
			if (pVar->type==PX_SCRIPTVM_VARIABLE_TYPE_STRING)
			{
				PX_StringFree(&pVar->_string);
			}

			if (pVar->type==PX_SCRIPTVM_VARIABLE_TYPE_MEMORY)
			{
				PX_MemoryFree(&pVar->_memory);
			}

			pVar->type=PX_SCRIPTVM_VARIABLE_TYPE_FLOAT;
			if (cVar.type==PX_SCRIPTVM_VARIABLE_TYPE_FLOAT)
			{
				pVar->_float=PX_sin_angle(cVar._float);
			}
			if (cVar.type==PX_SCRIPTVM_VARIABLE_TYPE_INT)
			{
				pVar->_float=PX_sin_angle((px_float)cVar._int);
			}
			pT->IP+=(4+2*4);
		}
		break;
	case PX_SCRIPT_ASM_INSTR_OPCODE_COS:
		{
			pVar=PX_SCRIPT_VM_GetVariablePointer(Ins,opType[0],PX_SCRIPT_VM_PARAM(0));
			cVar=PX_SCRIPT_VM_GetParamConst(Ins,opType[1],PX_SCRIPT_VM_PARAM(1),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			if (cVar.type!=PX_SCRIPTVM_VARIABLE_TYPE_FLOAT&&cVar.type!=PX_SCRIPTVM_VARIABLE_TYPE_INT)
			{
				goto _ERROR;
			}

			if (pVar->type==PX_SCRIPTVM_VARIABLE_TYPE_STRING)
			{
				PX_StringFree(&pVar->_string);
			}

			if (pVar->type==PX_SCRIPTVM_VARIABLE_TYPE_MEMORY)
			{
				PX_MemoryFree(&pVar->_memory);
			}
			pVar->type=PX_SCRIPTVM_VARIABLE_TYPE_FLOAT;
			if (cVar.type==PX_SCRIPTVM_VARIABLE_TYPE_FLOAT)
			{
				pVar->_float=PX_cos_angle(cVar._float);
			}
			if (cVar.type==PX_SCRIPTVM_VARIABLE_TYPE_INT)
			{
				pVar->_float=PX_cos_angle((px_float)cVar._int);
			}
			pT->IP+=(4+2*4);
		}
		break;
	case PX_SCRIPT_ASM_INSTR_OPCODE_INT:
		{
			pVar=PX_SCRIPT_VM_GetVariablePointer(Ins,opType[0],PX_SCRIPT_VM_PARAM(0));
			if (pVar->type!=PX_SCRIPTVM_VARIABLE_TYPE_FLOAT&&pVar->type!=PX_SCRIPTVM_VARIABLE_TYPE_INT)
			{
				PX_ScriptVM_Error(Ins,"INT transform error.");
				goto _ERROR;
			}
		
			if (pVar->type==PX_SCRIPTVM_VARIABLE_TYPE_FLOAT)
			{
				pVar->_int=(px_int)(pVar->_float);
				pVar->type=PX_SCRIPTVM_VARIABLE_TYPE_INT;
			}
			
			pT->IP+=(4+1*4);
		}
		break;
	case PX_SCRIPT_ASM_INSTR_OPCODE_FLT:
		{
			pVar=PX_SCRIPT_VM_GetVariablePointer(Ins,opType[0],PX_SCRIPT_VM_PARAM(0));
			if (pVar->type!=PX_SCRIPTVM_VARIABLE_TYPE_FLOAT&&pVar->type!=PX_SCRIPTVM_VARIABLE_TYPE_INT)
			{
				PX_ScriptVM_Error(Ins,"FLT transform error.");
				goto _ERROR;
			}

			if (pVar->type==PX_SCRIPTVM_VARIABLE_TYPE_INT)
			{
				pVar->_float=(px_float)(pVar->_int);
				pVar->type=PX_SCRIPTVM_VARIABLE_TYPE_FLOAT;
			}

			pT->IP+=(4+1*4);
		}
		break;
	case PX_SCRIPT_ASM_INSTR_OPCODE_STRLEN:
		{
			pVar=PX_SCRIPT_VM_GetVariablePointer(Ins,opType[0],PX_SCRIPT_VM_PARAM(0));
			cVar=PX_SCRIPT_VM_GetParamConst(Ins,opType[1],PX_SCRIPT_VM_PARAM(1),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			if (cVar.type!=PX_SCRIPTVM_VARIABLE_TYPE_STRING)
			{
				PX_ScriptVM_Error(Ins,"strlen parameters error.");
				goto _ERROR;
			}
			PX_SCRIPTVM_VaribaleFree(Ins,pVar);
			pVar->type=PX_SCRIPTVM_VARIABLE_TYPE_INT;
			pVar->_int=PX_strlen(cVar._string.buffer);
			pT->IP+=(4+2*4);
		}
		break;
	case PX_SCRIPT_ASM_INSTR_OPCODE_STRCAT:
		{
			pVar=PX_SCRIPT_VM_GetVariablePointer(Ins,opType[0],PX_SCRIPT_VM_PARAM(0));
			cVar=PX_SCRIPT_VM_GetParamConst(Ins,opType[1],PX_SCRIPT_VM_PARAM(1),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			if (pVar->type!=PX_SCRIPTVM_VARIABLE_TYPE_STRING)
			{
				PX_ScriptVM_Error(Ins,"strcat parameters error.");
				goto _ERROR;
			}
			if (cVar.type==PX_SCRIPTVM_VARIABLE_TYPE_STRING)
			{
				if(!PX_StringCat(&pVar->_string,cVar._string.buffer)) goto _ERROR;
			}
			else if (cVar.type==PX_SCRIPTVM_VARIABLE_TYPE_INT)
			{
				if(!PX_StringCatChar(&pVar->_string,(px_char)cVar._int)) goto _ERROR;
			}
			else
			{
				goto _ERROR;
			}
			pT->IP+=(4+2*4);
		}
		break;
	case PX_SCRIPT_ASM_INSTR_OPCODE_STRREP:
		{
			pVar=PX_SCRIPT_VM_GetVariablePointer(Ins,opType[0],PX_SCRIPT_VM_PARAM(0));
			cVar=PX_SCRIPT_VM_GetParamConst(Ins,opType[1],PX_SCRIPT_VM_PARAM(1),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			sVar=PX_SCRIPT_VM_GetParamConst(Ins,opType[2],PX_SCRIPT_VM_PARAM(2),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			if (pVar->type!=PX_SCRIPTVM_VARIABLE_TYPE_STRING)
			{
				PX_ScriptVM_Error(Ins,"strrep parameters error.");
				goto _ERROR;
			}
			if (cVar.type!=PX_SCRIPTVM_VARIABLE_TYPE_STRING)
			{
				PX_ScriptVM_Error(Ins,"strrep parameters error.");
				goto _ERROR;
			}
			if (sVar.type!=PX_SCRIPTVM_VARIABLE_TYPE_STRING)
			{
				PX_ScriptVM_Error(Ins,"strrep parameters error.");
				goto _ERROR;
			}
			PX_StringReplace(&pVar->_string,cVar._string.buffer,sVar._string.buffer);
			pT->IP+=(4+3*4);
		}
		break;
	case PX_SCRIPT_ASM_INSTR_OPCODE_STRCHR:
		{
			pVar=PX_SCRIPT_VM_GetVariablePointer(Ins,opType[0],PX_SCRIPT_VM_PARAM(0));
			cVar=PX_SCRIPT_VM_GetParamConst(Ins,opType[1],PX_SCRIPT_VM_PARAM(1),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			sVar=PX_SCRIPT_VM_GetParamConst(Ins,opType[2],PX_SCRIPT_VM_PARAM(2),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			PX_SCRIPTVM_VaribaleFree(Ins,pVar);

			if (cVar.type!=PX_SCRIPTVM_VARIABLE_TYPE_STRING)
			{
				PX_ScriptVM_Error(Ins,"strchr parameters error.");
				goto _ERROR;
			}
			if (sVar.type!=PX_SCRIPTVM_VARIABLE_TYPE_INT)
			{
				PX_ScriptVM_Error(Ins,"strchr parameters error.");
				goto _ERROR;
			}

			pVar->type=PX_SCRIPTVM_VARIABLE_TYPE_INT;

			if(sVar._int<PX_strlen(cVar._string.buffer))
				pVar->_int=(px_uchar)cVar._string.buffer[sVar._int];
			pT->IP+=(4+3*4);
		}
		break;

	case PX_SCRIPT_ASM_INSTR_OPCODE_STRTOI:
		{
			pVar=PX_SCRIPT_VM_GetVariablePointer(Ins,opType[0],PX_SCRIPT_VM_PARAM(0));
			cVar=PX_SCRIPT_VM_GetParamConst(Ins,opType[1],PX_SCRIPT_VM_PARAM(1),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			PX_SCRIPTVM_VaribaleFree(Ins,pVar);

			if (cVar.type!=PX_SCRIPTVM_VARIABLE_TYPE_STRING)
			{
				PX_ScriptVM_Error(Ins,"strtoi parameters error.");
				goto _ERROR;
			}

			pVar->type=PX_SCRIPTVM_VARIABLE_TYPE_INT;
			pVar->_int=PX_atoi(cVar._string.buffer);
			pT->IP+=(4+2*4);
		}
		break;
	case PX_SCRIPT_ASM_INSTR_OPCODE_STRTOF:
		{
			pVar=PX_SCRIPT_VM_GetVariablePointer(Ins,opType[0],PX_SCRIPT_VM_PARAM(0));
			cVar=PX_SCRIPT_VM_GetParamConst(Ins,opType[1],PX_SCRIPT_VM_PARAM(1),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			PX_SCRIPTVM_VaribaleFree(Ins,pVar);

			if (cVar.type!=PX_SCRIPTVM_VARIABLE_TYPE_STRING)
			{
				PX_ScriptVM_Error(Ins,"strtof parameters error.");
				goto _ERROR;
			}

			pVar->type=PX_SCRIPTVM_VARIABLE_TYPE_FLOAT;
			pVar->_int=(px_int)PX_atof(cVar._string.buffer);
			pT->IP+=(4+2*4);
		}
		break;

	case PX_SCRIPT_ASM_INSTR_OPCODE_STRFRI:
		{
			pVar=PX_SCRIPT_VM_GetVariablePointer(Ins,opType[0],PX_SCRIPT_VM_PARAM(0));
			cVar=PX_SCRIPT_VM_GetParamConst(Ins,opType[1],PX_SCRIPT_VM_PARAM(1),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			PX_SCRIPTVM_VaribaleFree(Ins,pVar);
			if (cVar.type!=PX_SCRIPTVM_VARIABLE_TYPE_INT&&cVar.type!=PX_SCRIPTVM_VARIABLE_TYPE_FLOAT)
			{
				PX_ScriptVM_Error(Ins,"strfri parameters error.");
				goto _ERROR;
			}

			pVar->type=PX_SCRIPTVM_VARIABLE_TYPE_STRING;
			PX_StringInitialize(Ins->mp,&pVar->_string);
			if(cVar.type==PX_SCRIPTVM_VARIABLE_TYPE_INT)
			PX_itoa(cVar._int,numOut,sizeof(numOut),10);
			else
			PX_itoa((px_int)cVar._float,numOut,sizeof(numOut),10);
			
			if(!PX_StringCat(&pVar->_string,numOut)) goto _ERROR;
			pT->IP+=(4+2*4);
		}
		break;

	case PX_SCRIPT_ASM_INSTR_OPCODE_STRFRF:
		{
			pVar=PX_SCRIPT_VM_GetVariablePointer(Ins,opType[0],PX_SCRIPT_VM_PARAM(0));
			cVar=PX_SCRIPT_VM_GetParamConst(Ins,opType[1],PX_SCRIPT_VM_PARAM(1),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			PX_SCRIPTVM_VaribaleFree(Ins,pVar);

			if (cVar.type!=PX_SCRIPTVM_VARIABLE_TYPE_INT&&cVar.type!=PX_SCRIPTVM_VARIABLE_TYPE_FLOAT)
			{
				PX_ScriptVM_Error(Ins,"strfrf parameters error.");
				goto _ERROR;
			}

			pVar->type=PX_SCRIPTVM_VARIABLE_TYPE_STRING;
			PX_StringInitialize(Ins->mp,&pVar->_string);
			numOut[0]=0;
			if(cVar.type==PX_SCRIPTVM_VARIABLE_TYPE_INT)
				PX_ftoa((px_float)cVar._int,numOut,sizeof(numOut),PX_SCRIPTVM_DEFALUT_FLOAT_PRECCISION);
			else
				PX_ftoa(cVar._float,numOut,sizeof(numOut),PX_SCRIPTVM_DEFALUT_FLOAT_PRECCISION);

			PX_StringCat(&pVar->_string,numOut);
			pT->IP+=(4+2*4);
		}
		break;
	case PX_SCRIPT_ASM_INSTR_OPCODE_STRTMEM:
		{
			pVar=PX_SCRIPT_VM_GetVariablePointer(Ins,opType[0],PX_SCRIPT_VM_PARAM(0));
			if (pVar->type==PX_SCRIPTVM_VARIABLE_TYPE_STRING)
			{
				PX_MemoryInitialize(Ins->mp,&cVar._memory);
				if(!PX_MemoryCat(&cVar._memory,pVar->_string.buffer,pVar->_string.bufferlen)) goto _ERROR;
				PX_StringFree(&pVar->_string);
				pVar->type=PX_SCRIPTVM_VARIABLE_TYPE_MEMORY;
				pVar->_memory=cVar._memory;
			}
			else
			{
				PX_ScriptVM_Error(Ins,"strtmem parameters error.");
				goto _ERROR;
			}
			pT->IP+=(4+1*4);
		}
		break;

	case PX_SCRIPT_ASM_INSTR_OPCODE_STRSET:
		{
			pVar=PX_SCRIPT_VM_GetVariablePointer(Ins,opType[0],PX_SCRIPT_VM_PARAM(0));
			sVar=PX_SCRIPT_VM_GetParamConst(Ins,opType[1],PX_SCRIPT_VM_PARAM(1),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			tVar=PX_SCRIPT_VM_GetParamConst(Ins,opType[2],PX_SCRIPT_VM_PARAM(2),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			if (pVar->type!=PX_SCRIPTVM_VARIABLE_TYPE_STRING)
			{
					if(pVar->type!=PX_SCRIPTVM_VARIABLE_TYPE_INT)
					{
						PX_ScriptVM_Error(Ins,"strset parameters error.");
						goto _ERROR;
					}
					else
					{
						pVar->type=PX_SCRIPTVM_VARIABLE_TYPE_STRING;
						PX_StringInitialize(Ins->mp,&pVar->_string);
					}
			}


			if (sVar.type!=PX_SCRIPTVM_VARIABLE_TYPE_INT||(tVar.type!=PX_SCRIPTVM_VARIABLE_TYPE_INT))
			{
				goto _ERROR;
			}

			if (sVar._int>=pVar->_string.bufferlen-1)
			{
				i=8;
				while(i<sVar._int+2)
				{
					i<<=1;
				}

				PX_StringInitAlloc(Ins->mp,&newString,i);
				PX_memset(newString.buffer,0,i);
				PX_strcat(newString.buffer,pVar->_string.buffer);
				PX_StringFree(&pVar->_string);
				pVar->_string=newString;
			}

			if(tVar.type==PX_SCRIPTVM_VARIABLE_TYPE_INT)
				pVar->_string.buffer[sVar._int]=(px_char)tVar._int;
			else 
				goto _ERROR;

			pT->IP+=(4+3*4);
		}
		break;

	case PX_SCRIPT_ASM_INSTR_OPCODE_STRFIND:
		{
			pVar=PX_SCRIPT_VM_GetVariablePointer(Ins,opType[0],PX_SCRIPT_VM_PARAM(0));
			sVar=PX_SCRIPT_VM_GetParamConst(Ins,opType[1],PX_SCRIPT_VM_PARAM(1),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			tVar=PX_SCRIPT_VM_GetParamConst(Ins,opType[2],PX_SCRIPT_VM_PARAM(2),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			PX_SCRIPTVM_VaribaleFree(Ins,pVar);
			if (sVar.type!=PX_SCRIPTVM_VARIABLE_TYPE_STRING||tVar.type!=PX_SCRIPTVM_VARIABLE_TYPE_STRING)
			{
				PX_ScriptVM_Error(Ins,"strfind parameters error.");
				goto _ERROR;
			}
			pVar->_int=-1;
			pchar=PX_strstr(sVar._string.buffer,tVar._string.buffer);
			if (pchar)
			{
				pVar->_int=(px_uint)(pchar-sVar._string.buffer);
			}
			pT->IP+=(4+3*4);
		}
		break;

	case PX_SCRIPT_ASM_INSTR_OPCODE_ASC:
		{
			pVar=PX_SCRIPT_VM_GetVariablePointer(Ins,opType[0],PX_SCRIPT_VM_PARAM(0));
			cVar=PX_SCRIPT_VM_GetParamConst(Ins,opType[1],PX_SCRIPT_VM_PARAM(1),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			PX_SCRIPTVM_VaribaleFree(Ins,pVar);
			if (cVar.type!=PX_SCRIPTVM_VARIABLE_TYPE_STRING)
			{
				PX_ScriptVM_Error(Ins,"asc parameters error.");
				goto _ERROR;
			}

			pVar->type=PX_SCRIPTVM_VARIABLE_TYPE_INT;
			pVar->_int=cVar._string.buffer[0];
			pT->IP+=(4+2*4);
		}
		break;

	case PX_SCRIPT_ASM_INSTR_OPCODE_MEMBYTE:
		{
			pVar=PX_SCRIPT_VM_GetVariablePointer(Ins,opType[0],PX_SCRIPT_VM_PARAM(0));
			sVar=PX_SCRIPT_VM_GetParamConst(Ins,opType[1],PX_SCRIPT_VM_PARAM(1),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			tVar=PX_SCRIPT_VM_GetParamConst(Ins,opType[2],PX_SCRIPT_VM_PARAM(2),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			PX_SCRIPTVM_VaribaleFree(Ins,pVar);

			if (sVar.type!=PX_SCRIPTVM_VARIABLE_TYPE_MEMORY||tVar.type!=PX_SCRIPTVM_VARIABLE_TYPE_INT)
			{
				PX_ScriptVM_Error(Ins,"membyte parameters error.");
				goto _ERROR;
			}
			pVar->type=PX_SCRIPTVM_VARIABLE_TYPE_INT;
			if (tVar._int<sVar._memory.usedsize)
			{
				pVar->_int=sVar._memory.buffer[tVar._int];
			}
			else
			{
				pVar->_int=0;
			}

			
			pT->IP+=(4+3*4);
		}
		break;

	case PX_SCRIPT_ASM_INSTR_OPCODE_MEMSET:
		{
			pVar=PX_SCRIPT_VM_GetVariablePointer(Ins,opType[0],PX_SCRIPT_VM_PARAM(0));
			sVar=PX_SCRIPT_VM_GetParamConst(Ins,opType[1],PX_SCRIPT_VM_PARAM(1),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			tVar=PX_SCRIPT_VM_GetParamConst(Ins,opType[2],PX_SCRIPT_VM_PARAM(2),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			
			if (pVar->type!=PX_SCRIPTVM_VARIABLE_TYPE_MEMORY)
			{
				if(pVar->type!=PX_SCRIPTVM_VARIABLE_TYPE_INT)
				{
					PX_ScriptVM_Error(Ins,"memset parameters error.");
					goto _ERROR;
				}
				else
				{
					pVar->type=PX_SCRIPTVM_VARIABLE_TYPE_MEMORY;
					PX_MemoryInitialize(Ins->mp,&pVar->_memory);
				}
			}


			if (sVar.type!=PX_SCRIPTVM_VARIABLE_TYPE_INT||tVar.type!=PX_SCRIPTVM_VARIABLE_TYPE_INT)
			{
				goto _ERROR;
			}

			if (sVar._int>=pVar->_memory.allocsize)
			{
				i=1;
				while(i<=sVar._int)
					i<<=1;
				newBuffer=(px_byte *)MP_Malloc(Ins->mp,i);
				PX_memcpy(newBuffer,pVar->_memory.buffer,pVar->_memory.usedsize);
				if (pVar->_memory.buffer)
				{
					MP_Free(Ins->mp,pVar->_memory.buffer);
				}
				pVar->_memory.buffer=newBuffer;
				pVar->_memory.allocsize=i;
			}

			pVar->_memory.buffer[sVar._int]=tVar._char;
			if(sVar._int>pVar->_memory.usedsize-1)
			pVar->_memory.usedsize=sVar._int+1;

			pT->IP+=(4+3*4);
		}
		break;

	case PX_SCRIPT_ASM_INSTR_OPCODE_MEMTRM:
		{
			pVar=PX_SCRIPT_VM_GetVariablePointer(Ins,opType[0],PX_SCRIPT_VM_PARAM(0));
			sVar=PX_SCRIPT_VM_GetParamConst(Ins,opType[1],PX_SCRIPT_VM_PARAM(1),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			tVar=PX_SCRIPT_VM_GetParamConst(Ins,opType[2],PX_SCRIPT_VM_PARAM(2),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;

			if (pVar->type!=PX_SCRIPTVM_VARIABLE_TYPE_MEMORY)
			{
				PX_ScriptVM_Error(Ins,"memtrm parameters error.");
				goto _ERROR;
			}


			if (sVar.type!=PX_SCRIPTVM_VARIABLE_TYPE_INT||tVar.type!=PX_SCRIPTVM_VARIABLE_TYPE_INT)
			{
				PX_ScriptVM_Error(Ins,"memtrm parameters error.");
				goto _ERROR;
			}

			if (sVar._int+tVar._int>pVar->_memory.usedsize)
			{
				PX_ScriptVM_Error(Ins,"memtrm out of range.");
				goto _ERROR;
			}
			j=0;
			for (i=sVar._int;i<sVar._int+tVar._int;i++)
			{
				pVar->_memory.buffer[j]=pVar->_memory.buffer[i];
				j++;
			}
			pVar->_memory.usedsize=tVar._int;

			pT->IP+=(4+3*4);
		}
		break;

	case PX_SCRIPT_ASM_INSTR_OPCODE_MEMFIND:
		{
			pVar=PX_SCRIPT_VM_GetVariablePointer(Ins,opType[0],PX_SCRIPT_VM_PARAM(0));
			sVar=PX_SCRIPT_VM_GetParamConst(Ins,opType[1],PX_SCRIPT_VM_PARAM(1),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			tVar=PX_SCRIPT_VM_GetParamConst(Ins,opType[2],PX_SCRIPT_VM_PARAM(2),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			PX_SCRIPTVM_VaribaleFree(Ins,pVar);
			if (sVar.type!=PX_SCRIPTVM_VARIABLE_TYPE_MEMORY||tVar.type!=PX_SCRIPTVM_VARIABLE_TYPE_MEMORY)
			{
				PX_ScriptVM_Error(Ins,"memfind parameters error.");
				goto _ERROR;
			}
			pVar->_int=-1;
			for(i=0;i<sVar._memory.usedsize-tVar._memory.usedsize+1;i++)
			{
				if (PX_memequ(sVar._memory.buffer+i,tVar._memory.buffer,tVar._memory.usedsize))
				{
					pVar->_int=i;
					pVar->type=PX_SCRIPTVM_VARIABLE_TYPE_INT;
					break;
				}
			}
			pT->IP+=(4+3*4);
		}
		break;

	case PX_SCRIPT_ASM_INSTR_OPCODE_MEMLEN:
		{
			pVar=PX_SCRIPT_VM_GetVariablePointer(Ins,opType[0],PX_SCRIPT_VM_PARAM(0));
			sVar=PX_SCRIPT_VM_GetParamConst(Ins,opType[1],PX_SCRIPT_VM_PARAM(1),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			PX_SCRIPTVM_VaribaleFree(Ins,pVar);
			pVar->type=PX_SCRIPTVM_VARIABLE_TYPE_INT;
			pVar->_int=sVar._memory.usedsize;
			
			pT->IP+=(4+2*4);
		}
		break;

	case PX_SCRIPT_ASM_INSTR_OPCODE_MEMCAT:
		{
			pVar=PX_SCRIPT_VM_GetVariablePointer(Ins,opType[0],PX_SCRIPT_VM_PARAM(0));
			sVar=PX_SCRIPT_VM_GetParamConst(Ins,opType[1],PX_SCRIPT_VM_PARAM(1),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			if (pVar->type!=PX_SCRIPTVM_VARIABLE_TYPE_MEMORY||sVar.type!=PX_SCRIPTVM_VARIABLE_TYPE_MEMORY)
			{
				if (pVar->type!=PX_SCRIPTVM_VARIABLE_TYPE_MEMORY||sVar.type!=PX_SCRIPTVM_VARIABLE_TYPE_INT)
				{
					PX_ScriptVM_Error(Ins,"memcat crash.");
					goto _ERROR;
				}
				else
				{
					if(!PX_MemoryCat(&pVar->_memory,&sVar._int,1)) goto _ERROR;
					pT->IP+=(4+2*4);
				}
			}
			else
			{
			if(!PX_MemoryCat(&pVar->_memory,sVar._memory.buffer,sVar._memory.usedsize)) goto _ERROR;
			pT->IP+=(4+2*4);
			}
		}
		break;
	case PX_SCRIPT_ASM_INSTR_OPCODE_MEMTSTR:
		{
			pVar=PX_SCRIPT_VM_GetVariablePointer(Ins,opType[0],PX_SCRIPT_VM_PARAM(0));
			if (pVar->type==PX_SCRIPTVM_VARIABLE_TYPE_MEMORY)
			{
				if (pVar->_memory.usedsize==0)
				{
					PX_StringInitialize(Ins->mp,&cVar._string);
					PX_MemoryFree(&pVar->_memory);
					pVar->type=PX_SCRIPTVM_VARIABLE_TYPE_STRING;
					pVar->_string=cVar._string;
				}
				else
				{
					lastchar=pVar->_memory.buffer[pVar->_memory.usedsize-1];
					
					pVar->_memory.buffer[pVar->_memory.usedsize-1]=0;
					
					PX_StringInitialize(Ins->mp,&cVar._string);
					
					if(!PX_StringCat(&cVar._string,(px_char *)pVar->_memory.buffer)) {
						PX_ScriptVM_Error(Ins,"memstr crash.");
						goto _ERROR;
					}
					if(PX_strlen((px_char *)pVar->_memory.buffer)==pVar->_memory.usedsize-1)
					PX_StringCatChar(&cVar._string,lastchar);

					PX_MemoryFree(&pVar->_memory);
					pVar->type=PX_SCRIPTVM_VARIABLE_TYPE_STRING;
					pVar->_string=cVar._string;
				}
				
			}
			else
			{
				goto _ERROR;
			}
			pT->IP+=(4+1*4);
		}
		break;
	case PX_SCRIPT_ASM_INSTR_OPCODE_DATACPY:
		{
			cVar=PX_SCRIPT_VM_GetParamConst(Ins,opType[0],PX_SCRIPT_VM_PARAM(0),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			sVar=PX_SCRIPT_VM_GetParamConst(Ins,opType[1],PX_SCRIPT_VM_PARAM(1),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			tVar=PX_SCRIPT_VM_GetParamConst(Ins,opType[2],PX_SCRIPT_VM_PARAM(2),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;

			if (cVar.type!=PX_SCRIPTVM_VARIABLE_TYPE_INT||sVar.type!=PX_SCRIPTVM_VARIABLE_TYPE_INT||tVar.type!=PX_SCRIPTVM_VARIABLE_TYPE_INT)
			{
				PX_ScriptVM_Error(Ins,"datacpy parameters error");
				goto _ERROR;
			}
			if (cVar._int+tVar._int>Ins->VM_memsize||cVar._int<0||sVar._int+tVar._int>Ins->VM_memsize||sVar._int<0)
			{
				PX_ScriptVM_Error(Ins,"Out of memory access");
				goto _ERROR;
			}
			PX_memcpy(&Ins->_mem[cVar._int],&Ins->_mem[sVar._int],sizeof(PX_SCRIPTVM_VARIABLE)*tVar._int);

			pT->IP+=(4+3*4);
		}
		break;
	case PX_SCRIPT_ASM_INSTR_OPCODE_NEG:
		{
			pVar=PX_SCRIPT_VM_GetVariablePointer(Ins,opType[0],PX_SCRIPT_VM_PARAM(0));

			if (pVar->type==PX_SCRIPTVM_VARIABLE_TYPE_INT)
			{
				pVar->_int=-pVar->_int;
			}
			else if(pVar->type==PX_SCRIPTVM_VARIABLE_TYPE_FLOAT)
			{
				pVar->_float=-pVar->_float;
			}
			else
			{
				PX_ScriptVM_Error(Ins,"neg parameters error");
				goto _ERROR;
			}

			pT->IP+=(4+1*4);
		}
		break;
	case PX_SCRIPT_ASM_INSTR_OPCODE_INV:
		{
			pVar=PX_SCRIPT_VM_GetVariablePointer(Ins,opType[0],PX_SCRIPT_VM_PARAM(0));

			if (pVar->type!=PX_SCRIPTVM_VARIABLE_TYPE_INT)
			{
				PX_ScriptVM_Error(Ins,"inv parameters error");
				goto _ERROR;
			}

			pVar->_int=~pVar->_int;

			pT->IP+=(4+1*4);
		}
		break;

	case PX_SCRIPT_ASM_INSTR_OPCODE_NOT:
		{
			pVar=PX_SCRIPT_VM_GetVariablePointer(Ins,opType[0],PX_SCRIPT_VM_PARAM(0));

			if (pVar->type!=PX_SCRIPTVM_VARIABLE_TYPE_INT)
			{
				PX_ScriptVM_Error(Ins,"not parameters error");
				goto _ERROR;
			}

			pVar->_int=!pVar->_int;

			pT->IP+=(4+1*4);
		}
		break;

	case PX_SCRIPT_ASM_INSTR_OPCODE_JMP:
		{
			cVar=PX_SCRIPT_VM_GetParamConst(Ins,opType[0],PX_SCRIPT_VM_PARAM(0),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			if (cVar.type!=PX_SCRIPTVM_VARIABLE_TYPE_INT)
			{
				PX_ScriptVM_Error(Ins,"JMP parameters error");
				goto _ERROR;
			}

			if (cVar._int<0||cVar._int>Ins->binsize)
			{
				PX_ScriptVM_Error(Ins,"JMP jump to invalid address");
				goto _ERROR;
			}

			pT->IP=cVar._int;
		}
		break;

	case PX_SCRIPT_ASM_INSTR_OPCODE_JE:
		{
			cVar=PX_SCRIPT_VM_GetParamConst(Ins,opType[0],PX_SCRIPT_VM_PARAM(0),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			sVar=PX_SCRIPT_VM_GetParamConst(Ins,opType[1],PX_SCRIPT_VM_PARAM(1),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			tVar=PX_SCRIPT_VM_GetParamConst(Ins,opType[2],PX_SCRIPT_VM_PARAM(2),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;

			if (tVar.type!=PX_SCRIPTVM_VARIABLE_TYPE_INT)
			{
				goto _ERROR;
			}

			if (cVar.type==sVar.type)
			{
				if (cVar.type!=PX_SCRIPTVM_VARIABLE_TYPE_STRING&&cVar.type!=PX_SCRIPTVM_VARIABLE_TYPE_MEMORY)
				{
					if (cVar._int==sVar._int)
					{
						if (tVar._int<0||tVar._int>Ins->binsize)
						{
							PX_ScriptVM_Error(Ins,"JE jump to invalid address");
							goto _ERROR;
						}
						pT->IP=tVar._int;
						break;
					}
				}
				else
				{
					if(cVar.type==PX_SCRIPTVM_VARIABLE_TYPE_STRING)
						if (PX_strequ(cVar._string.buffer,sVar._string.buffer))
						{
							if (tVar._int<0||tVar._int>Ins->binsize)
							{
								PX_ScriptVM_Error(Ins,"JE jump to invalid address");
								goto _ERROR;
							}
							pT->IP=tVar._int;
							break;
						}

					if(cVar.type==PX_SCRIPTVM_VARIABLE_TYPE_MEMORY)
						if (PX_memequ(cVar._memory.buffer,sVar._memory.buffer,cVar._memory.usedsize))
						{
							if (tVar._int<0||tVar._int>Ins->binsize)
							{
								PX_ScriptVM_Error(Ins,"JE jump to invalid address");
								goto _ERROR;
							}
							pT->IP=tVar._int;
							break;
						}
				}
			}
			pT->IP+=(4+3*4);
		}
		break;

	case PX_SCRIPT_ASM_INSTR_OPCODE_JNE:
		{
			cVar=PX_SCRIPT_VM_GetParamConst(Ins,opType[0],PX_SCRIPT_VM_PARAM(0),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			sVar=PX_SCRIPT_VM_GetParamConst(Ins,opType[1],PX_SCRIPT_VM_PARAM(1),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			tVar=PX_SCRIPT_VM_GetParamConst(Ins,opType[2],PX_SCRIPT_VM_PARAM(2),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			if (tVar.type!=PX_SCRIPTVM_VARIABLE_TYPE_INT)
			{
				goto _ERROR;
			}

			if (cVar.type==sVar.type)
			{
				if (cVar.type!=PX_SCRIPTVM_VARIABLE_TYPE_STRING&&cVar.type!=PX_SCRIPTVM_VARIABLE_TYPE_MEMORY)
				{
					if (cVar._int!=sVar._int)
					{
						if (tVar._int<0||tVar._int>Ins->binsize)
						{
							PX_ScriptVM_Error(Ins,"JNE jump to invalid address");
							goto _ERROR;
						}
						pT->IP=tVar._int;
						break;
					}
				}
				else
				{
						if(cVar.type==PX_SCRIPTVM_VARIABLE_TYPE_STRING)
							if (!PX_strequ(cVar._string.buffer,sVar._string.buffer))
							{
								if (tVar._int<0||tVar._int>Ins->binsize)
								{
									PX_ScriptVM_Error(Ins,"JNE jump to invalid address");
									goto _ERROR;
								}
								pT->IP=tVar._int;
								break;
							}

						if(cVar.type==PX_SCRIPTVM_VARIABLE_TYPE_MEMORY)
							if (!PX_memequ(cVar._memory.buffer,sVar._memory.buffer,cVar._memory.usedsize))
							{
								if (tVar._int<0||tVar._int>Ins->binsize)
								{
									PX_ScriptVM_Error(Ins,"JNE jump to invalid address");
									goto _ERROR;
								}
								pT->IP=tVar._int;
								break;
							}
				}
			}
			pT->IP+=(4+3*4);
			
			
		}
		break;

	case PX_SCRIPT_ASM_INSTR_OPCODE_JL:
		{
			cVar=PX_SCRIPT_VM_GetParamConst(Ins,opType[0],PX_SCRIPT_VM_PARAM(0),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			sVar=PX_SCRIPT_VM_GetParamConst(Ins,opType[1],PX_SCRIPT_VM_PARAM(1),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			tVar=PX_SCRIPT_VM_GetParamConst(Ins,opType[2],PX_SCRIPT_VM_PARAM(2),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;

			if (tVar.type!=PX_SCRIPTVM_VARIABLE_TYPE_INT)
			{
				PX_ScriptVM_Error(Ins,"JL parameters error");
				goto _ERROR;
			}
			if (cVar.type==PX_SCRIPTVM_VARIABLE_TYPE_STRING||sVar.type==PX_SCRIPTVM_VARIABLE_TYPE_STRING)
			{
				PX_ScriptVM_Error(Ins,"JL parameters error");
				goto _ERROR;
			}
			if (cVar.type==PX_SCRIPTVM_VARIABLE_TYPE_MEMORY||sVar.type==PX_SCRIPTVM_VARIABLE_TYPE_MEMORY)
			{
				PX_ScriptVM_Error(Ins,"JL parameters error");
				goto _ERROR;
			}
			if (cVar.type==PX_SCRIPTVM_VARIABLE_TYPE_INT)
			{
				p1=(px_float)cVar._int;
			}
			if (cVar.type==PX_SCRIPTVM_VARIABLE_TYPE_FLOAT)
			{
				p1=cVar._float;
			}
			if (sVar.type==PX_SCRIPTVM_VARIABLE_TYPE_INT)
			{
				p2=(px_float)sVar._int;
			}
			if (sVar.type==PX_SCRIPTVM_VARIABLE_TYPE_FLOAT)
			{
				p2=sVar._float;
			}

			if (p1<p2)
			{
				if (tVar._int<0||tVar._int>Ins->binsize)
				{
					PX_ScriptVM_Error(Ins,"JL jump to invalid address");
					goto _ERROR;
				}
				pT->IP=tVar._int;
			}
			else
			{
				pT->IP+=(4+3*4);
			}

		}
		break;

	case PX_SCRIPT_ASM_INSTR_OPCODE_JLE:
		{
			cVar=PX_SCRIPT_VM_GetParamConst(Ins,opType[0],PX_SCRIPT_VM_PARAM(0),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			sVar=PX_SCRIPT_VM_GetParamConst(Ins,opType[1],PX_SCRIPT_VM_PARAM(1),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			tVar=PX_SCRIPT_VM_GetParamConst(Ins,opType[2],PX_SCRIPT_VM_PARAM(2),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;

			if (tVar.type!=PX_SCRIPTVM_VARIABLE_TYPE_INT)
			{
				PX_ScriptVM_Error(Ins,"JLE parameters error");
				goto _ERROR;
			}
			if (cVar.type==PX_SCRIPTVM_VARIABLE_TYPE_STRING||sVar.type==PX_SCRIPTVM_VARIABLE_TYPE_STRING)
			{
				PX_ScriptVM_Error(Ins,"JLE parameters error");
				goto _ERROR;
			}
			if (cVar.type==PX_SCRIPTVM_VARIABLE_TYPE_MEMORY||sVar.type==PX_SCRIPTVM_VARIABLE_TYPE_MEMORY)
			{
				PX_ScriptVM_Error(Ins,"JLE parameters error");
				goto _ERROR;
			}
			if (cVar.type==PX_SCRIPTVM_VARIABLE_TYPE_INT)
			{
				p1=(px_float)cVar._int;
			}
			if (cVar.type==PX_SCRIPTVM_VARIABLE_TYPE_FLOAT)
			{
				p1=cVar._float;
			}
			if (sVar.type==PX_SCRIPTVM_VARIABLE_TYPE_INT)
			{
				p2=(px_float)sVar._int;
			}
			if (sVar.type==PX_SCRIPTVM_VARIABLE_TYPE_FLOAT)
			{
				p2=sVar._float;
			}

			if (p1<=p2)
			{
				if (tVar._int<0||tVar._int>Ins->binsize)
				{
					PX_ScriptVM_Error(Ins,"JLE jump to invalid address");
					goto _ERROR;
				}
				pT->IP=tVar._int;
			}
			else
			{
				pT->IP+=(4+3*4);
			}

		}
		break;

	case PX_SCRIPT_ASM_INSTR_OPCODE_JG:
		{
			cVar=PX_SCRIPT_VM_GetParamConst(Ins,opType[0],PX_SCRIPT_VM_PARAM(0),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			sVar=PX_SCRIPT_VM_GetParamConst(Ins,opType[1],PX_SCRIPT_VM_PARAM(1),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			tVar=PX_SCRIPT_VM_GetParamConst(Ins,opType[2],PX_SCRIPT_VM_PARAM(2),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;

			if (tVar.type!=PX_SCRIPTVM_VARIABLE_TYPE_INT)
			{
				PX_ScriptVM_Error(Ins,"JG parameters error");
				goto _ERROR;
			}
			if (cVar.type==PX_SCRIPTVM_VARIABLE_TYPE_STRING||sVar.type==PX_SCRIPTVM_VARIABLE_TYPE_STRING)
			{
				PX_ScriptVM_Error(Ins,"JG parameters error");
				goto _ERROR;
			}
			if (cVar.type==PX_SCRIPTVM_VARIABLE_TYPE_MEMORY||sVar.type==PX_SCRIPTVM_VARIABLE_TYPE_MEMORY)
			{
				PX_ScriptVM_Error(Ins,"JG parameters error");
				goto _ERROR;
			}
			if (cVar.type==PX_SCRIPTVM_VARIABLE_TYPE_INT)
			{
				p1=(px_float)cVar._int;
			}
			if (cVar.type==PX_SCRIPTVM_VARIABLE_TYPE_FLOAT)
			{
				p1=cVar._float;
			}
			if (sVar.type==PX_SCRIPTVM_VARIABLE_TYPE_INT)
			{
				p2=(px_float)sVar._int;
			}
			if (sVar.type==PX_SCRIPTVM_VARIABLE_TYPE_FLOAT)
			{
				p2=sVar._float;
			}

			if (p1>p2)
			{
				if (tVar._int<0||tVar._int>Ins->binsize)
				{
					PX_ScriptVM_Error(Ins,"JG jump to invalid address");
					goto _ERROR;
				}
				pT->IP=tVar._int;
			}
			else
			{
				pT->IP+=(4+3*4);
			}

		}
		break;

	case PX_SCRIPT_ASM_INSTR_OPCODE_JGE:
		{
			cVar=PX_SCRIPT_VM_GetParamConst(Ins,opType[0],PX_SCRIPT_VM_PARAM(0),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			sVar=PX_SCRIPT_VM_GetParamConst(Ins,opType[1],PX_SCRIPT_VM_PARAM(1),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			tVar=PX_SCRIPT_VM_GetParamConst(Ins,opType[2],PX_SCRIPT_VM_PARAM(2),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;

			if (tVar.type!=PX_SCRIPTVM_VARIABLE_TYPE_INT)
			{
				PX_ScriptVM_Error(Ins,"JGE parameters error");
				goto _ERROR;
			}
			if (cVar.type==PX_SCRIPTVM_VARIABLE_TYPE_STRING||sVar.type==PX_SCRIPTVM_VARIABLE_TYPE_STRING)
			{
				PX_ScriptVM_Error(Ins,"JGE parameters error");
				goto _ERROR;
			}
			if (cVar.type==PX_SCRIPTVM_VARIABLE_TYPE_MEMORY||sVar.type==PX_SCRIPTVM_VARIABLE_TYPE_MEMORY)
			{
				PX_ScriptVM_Error(Ins,"JGE parameters error");
				goto _ERROR;
			}
			if (cVar.type==PX_SCRIPTVM_VARIABLE_TYPE_INT)
			{
				p1=(px_float)cVar._int;
			}
			if (cVar.type==PX_SCRIPTVM_VARIABLE_TYPE_FLOAT)
			{
				p1=cVar._float;
			}
			if (sVar.type==PX_SCRIPTVM_VARIABLE_TYPE_INT)
			{
				p2=(px_float)sVar._int;
			}
			if (sVar.type==PX_SCRIPTVM_VARIABLE_TYPE_FLOAT)
			{
				p2=sVar._float;
			}

			if (p1>=p2)
			{
				if (tVar._int<0||tVar._int>Ins->binsize)
				{
					PX_ScriptVM_Error(Ins,"JGE jump to invalid address");
					goto _ERROR;
				}
				pT->IP=tVar._int;
			}
			else
			{
				pT->IP+=(4+3*4);
			}

		}
		break;
	case PX_SCRIPT_ASM_INSTR_OPCODE_LGE:
		{
			pVar=PX_SCRIPT_VM_GetVariablePointer(Ins,opType[0],PX_SCRIPT_VM_PARAM(0));
			sVar=PX_SCRIPT_VM_GetParamConst(Ins,opType[1],PX_SCRIPT_VM_PARAM(1),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			tVar=PX_SCRIPT_VM_GetParamConst(Ins,opType[2],PX_SCRIPT_VM_PARAM(2),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			PX_SCRIPTVM_VaribaleFree(Ins,pVar);
			pVar->type=PX_SCRIPTVM_VARIABLE_TYPE_INT;
			pVar->_int=0;
			
			if (sVar.type==tVar.type)
			{
				if (sVar.type!=PX_SCRIPTVM_VARIABLE_TYPE_STRING&&sVar.type!=PX_SCRIPTVM_VARIABLE_TYPE_MEMORY)
				{
					if (sVar._int==tVar._int)
					{
						pVar->_int=1;
						pT->IP+=(4+3*4);
						break;
					}
				}
				else
				{
					if(sVar.type==PX_SCRIPTVM_VARIABLE_TYPE_STRING)
						if (PX_strequ(sVar._string.buffer,tVar._string.buffer))
						{
							pVar->_int=1;
							pT->IP+=(4+3*4);
							break;
						}


					if(sVar.type==PX_SCRIPTVM_VARIABLE_TYPE_MEMORY)
						if ((sVar._memory.usedsize==tVar._memory.usedsize)&&PX_memequ(sVar._memory.buffer,tVar._memory.buffer,sVar._memory.usedsize))
						{
							pVar->_int=1;
							pT->IP+=(4+3*4);
							break;
						}
				}
			}
			pT->IP+=(4+3*4);
		}
		break;

	case PX_SCRIPT_ASM_INSTR_OPCODE_LGNE:
		{
			pVar=PX_SCRIPT_VM_GetVariablePointer(Ins,opType[0],PX_SCRIPT_VM_PARAM(0));
			sVar=PX_SCRIPT_VM_GetParamConst(Ins,opType[1],PX_SCRIPT_VM_PARAM(1),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			tVar=PX_SCRIPT_VM_GetParamConst(Ins,opType[2],PX_SCRIPT_VM_PARAM(2),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			PX_SCRIPTVM_VaribaleFree(Ins,pVar);
			pVar->type=PX_SCRIPTVM_VARIABLE_TYPE_INT;
			pVar->_int=1;

			if (sVar.type==tVar.type)
			{
				if (sVar.type!=PX_SCRIPTVM_VARIABLE_TYPE_STRING&&sVar.type!=PX_SCRIPTVM_VARIABLE_TYPE_MEMORY)
				{
					if (sVar._int==tVar._int)
					{
						pVar->_int=0;
						pT->IP+=(4+3*4);
						break;
					}
				}
				else
				{
					if(sVar.type==PX_SCRIPTVM_VARIABLE_TYPE_STRING)
						if (PX_strequ(sVar._string.buffer,tVar._string.buffer))
						{
							pVar->_int=0;
							pT->IP+=(4+3*4);
							break;
						}


						if(cVar.type==PX_SCRIPTVM_VARIABLE_TYPE_MEMORY)
							if ((sVar._memory.usedsize==tVar._memory.usedsize)&&PX_memequ(sVar._memory.buffer,tVar._memory.buffer,sVar._memory.usedsize))
							{
								pVar->_int=0;
								pT->IP+=(4+3*4);
								break;
							}
				}
			}
			pT->IP+=(4+3*4);
		}
		break;

	case PX_SCRIPT_ASM_INSTR_OPCODE_LGZ:
		{
			pVar=PX_SCRIPT_VM_GetVariablePointer(Ins,opType[0],PX_SCRIPT_VM_PARAM(0));

			if (pVar->type!=PX_SCRIPTVM_VARIABLE_TYPE_INT&&pVar->type!=PX_SCRIPTVM_VARIABLE_TYPE_FLOAT)
			{
				PX_ScriptVM_Error(Ins,"LGZ parameters error");
				goto _ERROR;
			}

			if (pVar->_int==0)
			{
				pVar->type=PX_SCRIPTVM_VARIABLE_TYPE_INT;
				pVar->_int=1;
			}
			else
			{
				pVar->type=PX_SCRIPTVM_VARIABLE_TYPE_INT;
				pVar->_int=0;
			}
			pT->IP+=(4+1*4);

		}
		break;

	case PX_SCRIPT_ASM_INSTR_OPCODE_LGGZ:
		{
			pVar=PX_SCRIPT_VM_GetVariablePointer(Ins,opType[0],PX_SCRIPT_VM_PARAM(0));

			if (pVar->type!=PX_SCRIPTVM_VARIABLE_TYPE_INT&&pVar->type!=PX_SCRIPTVM_VARIABLE_TYPE_FLOAT)
			{
				PX_ScriptVM_Error(Ins,"LGGZ parameters error");
				goto _ERROR;
			}

			if (pVar->_int>0)
			{
				pVar->type=PX_SCRIPTVM_VARIABLE_TYPE_INT;
				pVar->_int=1;
			}
			else
			{
				pVar->type=PX_SCRIPTVM_VARIABLE_TYPE_INT;
				pVar->_int=0;
			}
			pT->IP+=(4+1*4);

		}
		break;
	case PX_SCRIPT_ASM_INSTR_OPCODE_LGGEZ:
		{
			pVar=PX_SCRIPT_VM_GetVariablePointer(Ins,opType[0],PX_SCRIPT_VM_PARAM(0));

			if (pVar->type!=PX_SCRIPTVM_VARIABLE_TYPE_INT&&pVar->type!=PX_SCRIPTVM_VARIABLE_TYPE_FLOAT)
			{
				PX_ScriptVM_Error(Ins,"LGGEZ parameters error");
				goto _ERROR;
			}

			if (pVar->_int>=0)
			{
				pVar->type=PX_SCRIPTVM_VARIABLE_TYPE_INT;
				pVar->_int=1;
			}
			else
			{
				pVar->type=PX_SCRIPTVM_VARIABLE_TYPE_INT;
				pVar->_int=0;
			}
			pT->IP+=(4+1*4);

		}
		break;
	case PX_SCRIPT_ASM_INSTR_OPCODE_LGLZ:
		{
			pVar=PX_SCRIPT_VM_GetVariablePointer(Ins,opType[0],PX_SCRIPT_VM_PARAM(0));

			if (pVar->type!=PX_SCRIPTVM_VARIABLE_TYPE_INT&&pVar->type!=PX_SCRIPTVM_VARIABLE_TYPE_FLOAT)
			{
				PX_ScriptVM_Error(Ins,"LGLZ parameters error");
				goto _ERROR;
			}

			if (pVar->_int<0)
			{
				pVar->type=PX_SCRIPTVM_VARIABLE_TYPE_INT;
				pVar->_int=1;
			}
			else
			{
				pVar->type=PX_SCRIPTVM_VARIABLE_TYPE_INT;
				pVar->_int=0;
			}
			pT->IP+=(4+1*4);

		}
		break;
	case PX_SCRIPT_ASM_INSTR_OPCODE_LGLEZ:
		{
			pVar=PX_SCRIPT_VM_GetVariablePointer(Ins,opType[0],PX_SCRIPT_VM_PARAM(0));

			if (pVar->type!=PX_SCRIPTVM_VARIABLE_TYPE_INT&&pVar->type!=PX_SCRIPTVM_VARIABLE_TYPE_FLOAT)
			{
				PX_ScriptVM_Error(Ins,"LGLEZ parameters error");
				goto _ERROR;
			}

			if (pVar->_int<=0)
			{
				pVar->type=PX_SCRIPTVM_VARIABLE_TYPE_INT;
				pVar->_int=1;
			}
			else
			{
				pVar->type=PX_SCRIPTVM_VARIABLE_TYPE_INT;
				pVar->_int=0;
			}
			pT->IP+=(4+1*4);

		}
		break;


	case PX_SCRIPT_ASM_INSTR_OPCODE_CALL:
		{
			cVar=PX_SCRIPT_VM_GetParamConst(Ins,opType[0],PX_SCRIPT_VM_PARAM(0),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			if (cVar.type!=PX_SCRIPTVM_VARIABLE_TYPE_INT)
			{
				PX_ScriptVM_Error(Ins,"CALL parameters error");
				goto _ERROR;
			}

			if (opType[0]==PX_SCRIPT_ASM_OPTYPE_HOST)
			{
				if (Ins->_host[cVar._int].map!=PX_NULL)
				{
					if (!((PX_ScriptVM_Function_Modules)(Ins->_host[cVar._int].map))(Ins,Ins->_host[cVar._int].userptr))
					{
						PX_ScriptVM_Error(Ins,"CALL function crash");
						goto _ERROR;//runtime error
					}
				}
				else
				{
					PX_ScriptVM_Error(Ins,"CALL host call not found");
					goto _ERROR;//runtime error
				}
				pT->IP+=8;
			}
			else
			{
				//push IP+8
				if (pT->SP>0)
				{
					pT->SP-=1;
				}
				else
				{
					PX_ScriptVM_Error(Ins,"CALL error");
					goto _ERROR;
				}
				

				pVar=&Ins->_mem[pT->SP];
				if (pVar->type==PX_SCRIPTVM_VARIABLE_TYPE_STRING)
				{
					PX_StringFree(&pVar->_string);
				}

				if (pVar->type==PX_SCRIPTVM_VARIABLE_TYPE_MEMORY)
				{
					PX_MemoryFree(&pVar->_memory);
				}
				
				Ins->_mem[pT->SP].type=PX_SCRIPTVM_VARIABLE_TYPE_INT;
				Ins->_mem[pT->SP]._int=pT->IP+8;
				pT->IP=cVar._int;
			}

		}
		break;

	case PX_SCRIPT_ASM_INSTR_OPCODE_PUSH:
		{
			cVar=PX_SCRIPT_VM_GetParamConst(Ins,opType[0],PX_SCRIPT_VM_PARAM(0),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			if(pT->SP>0)
			pT->SP-=1;
			else
			{
				PX_ScriptVM_Error(Ins,"push error");
				goto _ERROR;
			}
			pVar=&Ins->_mem[pT->SP];

			if (pVar->type==PX_SCRIPTVM_VARIABLE_TYPE_STRING)
			{
				PX_StringFree(&pVar->_string);
			}

			if (pVar->type==PX_SCRIPTVM_VARIABLE_TYPE_MEMORY)
			{
				PX_MemoryFree(&pVar->_memory);
			}

			if (cVar.type==PX_SCRIPTVM_VARIABLE_TYPE_STRING)
			{
				if(!PX_StringInitialize(Ins->mp,&pVar->_string)) goto _ERROR;
				if(!PX_StringCat(&pVar->_string,cVar._string.buffer)) goto _ERROR;
				pVar->type=cVar.type;
			}
			else if (cVar.type==PX_SCRIPTVM_VARIABLE_TYPE_MEMORY)
			{
				PX_MemoryInitialize(Ins->mp,&pVar->_memory);
				if(!PX_MemoryAlloc(&pVar->_memory,cVar._memory.usedsize)) goto _ERROR;
				if(!PX_MemoryCat(&pVar->_memory,cVar._memory.buffer,cVar._memory.usedsize)) goto _ERROR;
				pVar->type=cVar.type;
			}
			else
			{
				*pVar=cVar;
			}

			pT->IP+=8;
		}
		break;

	case PX_SCRIPT_ASM_INSTR_OPCODE_POP:
		{
			if (opType[0]==PX_SCRIPT_ASM_OPTYPE_IP)
			{
				if(Ins->_mem[pT->SP].type==PX_SCRIPTVM_VARIABLE_TYPE_INT)
				{
					pT->IP=Ins->_mem[pT->SP]._int;
					pT->SP+=1;
				}
				else
				{
					PX_ScriptVM_Error(Ins,"POP error");
					goto _ERROR;
				}
			}
			else if (opType[0]==PX_SCRIPT_ASM_OPTYPE_SP)
			{
				PX_ScriptVM_Error(Ins,"POP error");
				goto _ERROR;
			}
			else if (opType[0]==PX_SCRIPT_ASM_OPTYPE_BP)
			{
				if(Ins->_mem[pT->SP].type==PX_SCRIPTVM_VARIABLE_TYPE_INT)
				{
					pT->BP=Ins->_mem[pT->SP]._int;
					pT->SP+=1;
					pT->IP+=8;
				}
				else
				{
					PX_ScriptVM_Error(Ins,"POP error");
					goto _ERROR;
				}
			}
			else
			{
				pVar=PX_SCRIPT_VM_GetVariablePointer(Ins,opType[0],PX_SCRIPT_VM_PARAM(0));

				if (pVar->type==PX_SCRIPTVM_VARIABLE_TYPE_STRING)
				{
					PX_StringFree(&pVar->_string);
				}

				if (pVar->type==PX_SCRIPTVM_VARIABLE_TYPE_MEMORY)
				{
					PX_MemoryFree(&pVar->_memory);
				}
				*pVar=Ins->_mem[pT->SP];

				Ins->_mem[pT->SP].type=PX_SCRIPTVM_VARIABLE_TYPE_INT;
				Ins->_mem[pT->SP]._int=0;

				pT->SP+=1;
				pT->IP+=8;
			}
			
		}
		break;

	case PX_SCRIPT_ASM_INSTR_OPCODE_POPN:
		{
			cVar=PX_SCRIPT_VM_GetParamConst(Ins,opType[0],PX_SCRIPT_VM_PARAM(0),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			if (cVar.type!=PX_SCRIPTVM_VARIABLE_TYPE_INT)
			{
				PX_ScriptVM_Error(Ins,"POPN error");
				goto _ERROR;
			}
			for (i=0;i<cVar._int;i++)
			{
				pVar=&Ins->_mem[pT->SP+i];

				if (pVar->type==PX_SCRIPTVM_VARIABLE_TYPE_STRING)
				{
					PX_StringFree(&pVar->_string);
				}

				if (pVar->type==PX_SCRIPTVM_VARIABLE_TYPE_MEMORY)
				{
					PX_MemoryFree(&pVar->_memory);
				}

				Ins->_mem[pT->SP+i].type=PX_SCRIPTVM_VARIABLE_TYPE_INT;
				Ins->_mem[pT->SP+i]._int=0;
			}
			pT->SP+=cVar._int;
			pT->IP+=8;
		}
		break;
	case PX_SCRIPT_ASM_INSTR_OPCODE_ADR:
		{
			pVar=PX_SCRIPT_VM_GetVariablePointer(Ins,opType[0],PX_SCRIPT_VM_PARAM(0));
			PX_SCRIPTVM_VaribaleFree(Ins,pVar);
			switch(opType[1])
			{
			case PX_SCRIPT_ASM_OPTYPE_GLOBAL:
				pVar->_int=PX_SCRIPT_VM_PARAM(1);
				break;
			case PX_SCRIPT_ASM_OPTYPE_LOCAL:
				pVar->_int=pT->BP-PX_SCRIPT_VM_PARAM(1);
				break;
			case PX_SCRIPT_ASM_OPTYPE_GLOBAL_REGREF:
				if (pT->R[PX_SCRIPT_VM_PARAM(1)].type!=PX_SCRIPTVM_VARIABLE_TYPE_INT)
				{
					PX_ScriptVM_Error(Ins,"ADR error");
					goto _ERROR;
				}
				pVar->_int=pT->R[PX_SCRIPT_VM_PARAM(1)]._int;
				break;
			case PX_SCRIPT_ASM_OPTYPE_GLOBAL_GLOBALREF:
				if (Ins->_mem[PX_SCRIPT_VM_PARAM(1)].type!=PX_SCRIPTVM_VARIABLE_TYPE_INT)
				{
					PX_ScriptVM_Error(Ins,"ADR error");
					goto _ERROR;
				}
				pVar->_int=Ins->_mem[PX_SCRIPT_VM_PARAM(1)]._int;
				break;
			case PX_SCRIPT_ASM_OPTYPE_GLOBAL_LOCALREF:
				if (Ins->_mem[pT->BP-PX_SCRIPT_VM_PARAM(1)].type!=PX_SCRIPTVM_VARIABLE_TYPE_INT)
				{
					PX_ScriptVM_Error(Ins,"ADR error");
					goto _ERROR;
				}
				pVar->_int=Ins->_mem[pT->BP-PX_SCRIPT_VM_PARAM(1)]._int;
				break;
			case PX_SCRIPT_ASM_OPTYPE_LOCAL_REGREF:
				if (Ins->_mem[PX_SCRIPT_VM_PARAM(1)].type!=PX_SCRIPTVM_VARIABLE_TYPE_INT)
				{
					PX_ScriptVM_Error(Ins,"ADR error");
					goto _ERROR;
				}
				pVar->_int=pT->BP-pT->R[PX_SCRIPT_VM_PARAM(1)]._int;
				break;
			case PX_SCRIPT_ASM_OPTYPE_LOCAL_GLOBALREF:
				if (Ins->_mem[PX_SCRIPT_VM_PARAM(1)].type!=PX_SCRIPTVM_VARIABLE_TYPE_INT)
				{
					PX_ScriptVM_Error(Ins,"ADR error");
					goto _ERROR;
				}
				pVar->_int=pT->BP-Ins->_mem[PX_SCRIPT_VM_PARAM(1)]._int;
				break;
			case PX_SCRIPT_ASM_OPTYPE_LOCAL_LOCALREF:
				if (Ins->_mem[pT->BP-PX_SCRIPT_VM_PARAM(1)].type!=PX_SCRIPTVM_VARIABLE_TYPE_INT)
				{
					PX_ScriptVM_Error(Ins,"ADR error");
					goto _ERROR;
				}
				pVar->_int=pT->BP-Ins->_mem[pT->BP-PX_SCRIPT_VM_PARAM(1)]._int;
				break;

			}
			pT->IP+=8;
		}
		break;

	case PX_SCRIPT_ASM_INSTR_OPCODE_RET:
		{
			if (pT->SP>=Ins->VM_memsize)
			{
				PX_SCRIPTVM_VaribaleFree(Ins,&cVar);
				PX_SCRIPTVM_VaribaleFree(Ins,&sVar);
				PX_SCRIPTVM_VaribaleFree(Ins,&tVar);

				PX_ScriptVM_ThreadStop(Ins,Ins->T);
				return PX_SCRIPTVM_RUNRETURN_END;
			}
			else
			{
				cVar=Ins->_mem[pT->SP];
				if (cVar.type!=PX_SCRIPTVM_VARIABLE_TYPE_INT)
				{
					PX_ScriptVM_Error(Ins,"RET error");
					goto _ERROR;
				}
				pT->SP+=1;
				if (pT->SP>=Ins->VM_memsize||cVar._int==-1)
				{
					PX_SCRIPTVM_VaribaleFree(Ins,&cVar);
					PX_SCRIPTVM_VaribaleFree(Ins,&sVar);
					PX_SCRIPTVM_VaribaleFree(Ins,&tVar);

					PX_ScriptVM_ThreadStop(Ins,Ins->T);
					return PX_SCRIPTVM_RUNRETURN_END;
				}
				else
				{
					pT->IP=cVar._int;
				}
			}
		}
		break;
	case PX_SCRIPT_ASM_INSTR_OPCODE_NOP:
		{
			pT->IP+=4;
		}
		break;
	case PX_SCRIPT_ASM_INSTR_OPCODE_BPX:
		{
			if (Ins->debug)
			{
				g_scriptVM_printFunc("Story Script Debugger Interrupt.\n");
				PX_ScriptVM_DebuggerInterrupt(Ins);

			}
			pT->IP+=4;
		}
		break;
	case PX_SCRIPT_ASM_INSTR_OPCODE_WAIT:
		{
			cVar=PX_SCRIPT_VM_GetParamConst(Ins,opType[0],PX_SCRIPT_VM_PARAM(0),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			if (cVar.type!=PX_SCRIPTVM_VARIABLE_TYPE_INT)
			{
				PX_ScriptVM_Error(Ins,"WAIT index error");
				goto _ERROR;
			}
			if (Ins->signal[cVar._int])
			{
				PX_SCRIPTVM_VaribaleFree(Ins,&cVar);
				PX_SCRIPTVM_VaribaleFree(Ins,&sVar);
				PX_SCRIPTVM_VaribaleFree(Ins,&tVar);
				return PX_SCRIPTVM_RUNRETURN_WAIT;
			}
			else
			{
				pT->IP+=8;
				if (tick==0)
				{
					tick=1;
				}
			}
		}
		break;
	case PX_SCRIPT_ASM_INSTR_OPCODE_SIGNAL:
		{
			cVar=PX_SCRIPT_VM_GetParamConst(Ins,opType[0],PX_SCRIPT_VM_PARAM(0),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			sVar=PX_SCRIPT_VM_GetParamConst(Ins,opType[1],PX_SCRIPT_VM_PARAM(1),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			if (cVar.type!=PX_SCRIPTVM_VARIABLE_TYPE_INT||sVar.type!=PX_SCRIPTVM_VARIABLE_TYPE_INT)
			{
				PX_ERROR("SIGNAL ERROR");
				goto _ERROR;
			}
			Ins->signal[cVar._int]=sVar._int;
			pT->IP+=4+4*2;
		}
		break;
	default:
		PX_ScriptVM_Error(Ins,"INVALID INSTR");
		goto _ERROR;
	}

	PX_SCRIPTVM_VaribaleFree(Ins,&cVar);
	PX_SCRIPTVM_VaribaleFree(Ins,&sVar);
	PX_SCRIPTVM_VaribaleFree(Ins,&tVar);

	if (pT->IP==-1)
		{
			break;
		}
	}
	PX_ScriptVM_ThreadStop(Ins, Ins->T);
	return PX_SCRIPTVM_RUNRETURN_END;
_ERROR:

	PX_SCRIPTVM_VaribaleFree(Ins,&cVar);
	PX_SCRIPTVM_VaribaleFree(Ins,&sVar);
	PX_SCRIPTVM_VaribaleFree(Ins,&tVar);

	PX_ScriptVM_Error(Ins,"Virtual Machine runtime error");
	return PX_SCRIPTVM_RUNRETURN_ERROR;
}

px_bool PX_ScriptVM_InstanceRunFunction(PX_ScriptVM_Instance *Ins,px_int threadID,const px_char *functionName,PX_SCRIPTVM_VARIABLE args[],px_int paramcount)
{
	int i,j,ip=-1;
	px_int old_T;
	if (paramcount>16||threadID>=Ins->maxThreadCount)
	{
		return PX_FALSE;
	}

	if (PX_strlen(functionName)>=__PX_SCRIPT_ASM_MNEMONIC_NAME_LEN)
	{
		return PX_FALSE;
	}

	for (i=0;i<Ins->funcCount;i++)
	{
		if(PX_strequ2(Ins->_func[i].name, functionName))
		{
			ip=Ins->_func[i].Addr;
			break;
		}
	}

	if (i==Ins->funcCount)
	{
		return PX_FALSE;
	}

	if (Ins->pThread[threadID].Activated==PX_FALSE)
	{
		Ins->pThread[threadID].suspend=PX_FALSE;
		Ins->pThread[threadID].Activated=PX_TRUE;
		Ins->pThread[threadID].SP=Ins->VM_memsize-Ins->VM_Stacksize*threadID;
		Ins->pThread[threadID].BP=Ins->pThread[threadID].SP;
		Ins->pThread[threadID].IP=ip;
		for(j=0;j<sizeof(Ins->pThread[threadID].R)/sizeof(Ins->pThread[threadID].R[0]);j++)
		{
			PX_SCRIPTVM_VaribaleFree(Ins,&Ins->pThread[threadID].R[j]);
		}
	}
	else
	{
		return PX_FALSE;
	}

	old_T=Ins->T;
	if(!PX_ScriptVM_InstanceThreadSwitch(Ins,threadID))
	{
		return PX_FALSE;
	}

	for (i=paramcount-1;i>=0;i--)
	{
		PX_ScriptVM_PUSH(Ins,args[i]);
	}

	PX_ScriptVM_PUSH(Ins,PX_ScriptVM_Variable_int(-1));

	PX_ScriptVM_InstanceRunThread(Ins,-1);	

	Ins->T=old_T;
	return PX_TRUE;
}

px_bool PX_ScriptVM_InstanceRunFunctionIndex(PX_ScriptVM_Instance *Ins,px_int threadID,px_int funcIndex,PX_SCRIPTVM_VARIABLE args[],px_int paramcount)
{
	int i,j,ip;
	px_int old_T;
	if (paramcount>16||threadID>=Ins->maxThreadCount)
	{
		return PX_FALSE;
	}

	ip=Ins->_func[funcIndex].Addr;

	if (Ins->pThread[threadID].Activated==PX_FALSE)
	{
		Ins->pThread[threadID].suspend=PX_FALSE;
		Ins->pThread[threadID].Activated=PX_TRUE;
		Ins->pThread[threadID].SP=Ins->VM_memsize-Ins->VM_Stacksize*threadID;
		Ins->pThread[threadID].BP=Ins->pThread[threadID].SP;
		Ins->pThread[threadID].IP=ip;
		for(j=0;j<sizeof(Ins->pThread[threadID].R)/sizeof(Ins->pThread[threadID].R[0]);j++)
		{
			PX_SCRIPTVM_VaribaleFree(Ins,&Ins->pThread[threadID].R[j]);
		}
	}
	else
	{
		return PX_FALSE;
	}

	old_T=Ins->T;
	if(!PX_ScriptVM_InstanceThreadSwitch(Ins,threadID))
	{
		return PX_FALSE;
	}

	for (i=paramcount-1;i>=0;i--)
	{
		PX_ScriptVM_PUSH(Ins,args[i]);
	}

	PX_ScriptVM_PUSH(Ins,PX_ScriptVM_Variable_int(-1));
	
	PX_ScriptVM_InstanceRunThread(Ins,-1);	
	Ins->T=old_T;
	return PX_TRUE;
}


px_bool PX_ScriptVM_InstanceBeginThreadFunction(PX_ScriptVM_Instance *Ins,px_int threadID,const px_char *func,PX_SCRIPTVM_VARIABLE args[],px_int paramcount)
{
	int i,j,ip=-1;
	px_int old_T;
	px_char	uprname[__PX_SCRIPT_ASM_MNEMONIC_NAME_LEN];

	if (paramcount>16||threadID>=Ins->maxThreadCount||threadID<0)
	{
		return PX_FALSE;
	}


	for (i=0;i<Ins->funcCount;i++)
	{
		PX_strcpy(uprname,func,sizeof(uprname));
		PX_strupr(uprname);
		if(PX_strequ(Ins->_func[i].name,uprname))
		{
			ip=Ins->_func[i].Addr;
			break;
		}
	}

	if (i==Ins->funcCount)
	{
		return PX_FALSE;
	}

	if (Ins->pThread[threadID].Activated==PX_FALSE)
	{
		Ins->pThread[threadID].suspend=PX_FALSE;
		Ins->pThread[threadID].Activated=PX_TRUE;
		Ins->pThread[threadID].SP=Ins->VM_memsize-Ins->VM_Stacksize*threadID;
		Ins->pThread[threadID].BP=Ins->pThread[threadID].SP;
		Ins->pThread[threadID].IP=ip;
		for(j=0;j<sizeof(Ins->pThread[threadID].R)/sizeof(Ins->pThread[threadID].R[0]);j++)
		{
			PX_SCRIPTVM_VaribaleFree(Ins,&Ins->pThread[threadID].R[j]);
		}
	}
	else
	{
		return PX_FALSE;
	}

	old_T=Ins->T;
	if(!PX_ScriptVM_InstanceThreadSwitch(Ins,threadID))
	{
		return PX_FALSE;
	}

	for (i=paramcount-1;i>=0;i--)
	{
		PX_ScriptVM_PUSH(Ins,args[i]);
	}

	PX_ScriptVM_PUSH(Ins,PX_ScriptVM_Variable_int(-1));

	if(!PX_ScriptVM_InstanceThreadSwitch(Ins,old_T))
	{
		return PX_FALSE;
	}
	return PX_TRUE;
}

px_bool PX_ScriptVM_InstanceBeginThreadFunctionIndex(PX_ScriptVM_Instance *Ins,px_int threadID,px_int funcIndex,PX_SCRIPTVM_VARIABLE args[],px_int paramcount)
{
	int i,j,ip;
	px_int old_T;
	if (paramcount>16||threadID>=Ins->maxThreadCount)
	{
		return PX_FALSE;
	}

	ip=Ins->_func[funcIndex].Addr;

	if (Ins->pThread[threadID].Activated==PX_FALSE)
	{
		Ins->pThread[threadID].suspend=PX_FALSE;
		Ins->pThread[threadID].Activated=PX_TRUE;
		Ins->pThread[threadID].SP=Ins->VM_memsize-Ins->VM_Stacksize*threadID;
		Ins->pThread[threadID].BP=Ins->pThread[threadID].SP;
		Ins->pThread[threadID].IP=ip;
		for(j=0;j<PX_SCRIPTVM_REG_COUNT;j++)
		{
			PX_SCRIPTVM_VaribaleFree(Ins,&Ins->pThread[threadID].R[j]);
		}
	}
	else
	{
		return PX_FALSE;
	}

	old_T=Ins->T;
	if(!PX_ScriptVM_InstanceThreadSwitch(Ins,threadID))
	{
		return PX_FALSE;
	}


	for (i=paramcount-1;i>=0;i--)
	{
		PX_ScriptVM_PUSH(Ins,args[i]);
	}
	

	PX_ScriptVM_PUSH(Ins,PX_ScriptVM_Variable_int(-1));

	if(!PX_ScriptVM_InstanceThreadSwitch(Ins,old_T))
	{
		return PX_FALSE;
	}
	return PX_TRUE;
}


px_int PX_ScriptVM_InstanceGetFreeThreadId(PX_ScriptVM_Instance *Ins)
{
	px_int i;
	for (i=0;i<Ins->maxThreadCount;i++)
	{
		if (!Ins->pThread[i].Activated)
		{
			return i;
		}
	}
	return -1;
}

px_bool PX_ScriptVM_InstanceFree(PX_ScriptVM_Instance *Ins)
{
	px_int i,j;
	for (i=0;i<Ins->VM_memsize;i++)
	{
		if (Ins->_mem[i].type==PX_SCRIPTVM_VARIABLE_TYPE_STRING)
		{
			PX_StringFree(&Ins->_mem[i]._string);
		}
		if (Ins->_mem[i].type==PX_SCRIPTVM_VARIABLE_TYPE_MEMORY)
		{
			PX_MemoryFree(&Ins->_mem[i]._memory);
		}
	}

	
	if (Ins->pThread)
	{
		//Free Thread
		for (i=0;i<Ins->maxThreadCount;i++)
		{
				for (j=0;j<PX_SCRIPTVM_REG_COUNT;j++)
				{
					if (Ins->pThread[i].R[j].type==PX_SCRIPTVM_VARIABLE_TYPE_STRING)
					{
						PX_StringFree(&Ins->pThread[i].R[j]._string);
					}
					if (Ins->pThread[i].R[j].type==PX_SCRIPTVM_VARIABLE_TYPE_MEMORY)
					{
						PX_MemoryFree(&Ins->pThread[i].R[j]._memory);
					}
				}	
		}
		MP_Free(Ins->mp,Ins->pThread);
	}


	if(Ins->_mem)
		MP_Free(Ins->mp,Ins->_mem);

	if(Ins->_func)
		MP_Free(Ins->mp,Ins->_func);

	if(Ins->_host)
		MP_Free(Ins->mp,Ins->_host);

	if(Ins->_string)
		MP_Free(Ins->mp,Ins->_string);

	if(Ins->_bin)
		MP_Free(Ins->mp,Ins->_bin);

	if(Ins->_memory)
		MP_Free(Ins->mp,Ins->_memory);

	return PX_TRUE;
}

px_bool PX_ScriptVM_RegistryHostFunction(PX_ScriptVM_Instance *Ins,const px_char *name,PX_ScriptVM_Function_Modules funcModules,px_void *userptr)
{
	px_int i;
	px_char uprname[__PX_SCRIPT_ASM_MNEMONIC_NAME_LEN];
	PX_strset(uprname,name);
	PX_strupr(uprname);
	for (i=0;i<Ins->hostCount;i++)
	{
		if (PX_strequ(uprname,Ins->_host[i].name+1))
		{
			Ins->_host[i].map=(px_void *)funcModules;
			Ins->_host[i].userptr=userptr;
			return PX_TRUE;
		}
	}
	return PX_FALSE;
}

px_void PX_ScriptVM_RET(PX_ScriptVM_Instance *Ins,PX_SCRIPTVM_VARIABLE cVar)
{
	PX_SCRIPTVM_VARIABLE *pVar;
	pVar=&Ins->pThread[Ins->T].R[1];

	PX_SCRIPTVM_VaribaleFree(Ins,pVar);
	
	if (cVar.type==PX_SCRIPTVM_VARIABLE_TYPE_STRING)
	{
		PX_StringInitialize(Ins->mp,&pVar->_string);
		PX_StringCat(&pVar->_string,cVar._string.buffer);
		pVar->type=cVar.type;
	}
	else if (cVar.type==PX_SCRIPTVM_VARIABLE_TYPE_MEMORY)
	{
		PX_MemoryInitialize(Ins->mp,&pVar->_memory);
		PX_MemoryAlloc(&pVar->_memory,cVar._memory.usedsize);
		PX_MemoryCat(&pVar->_memory,cVar._memory.buffer,cVar._memory.usedsize);
		pVar->type=cVar.type;
	}
	else
	{
		*pVar=cVar;
	}
}

px_void PX_ScriptVM_PUSH(PX_ScriptVM_Instance *Ins,PX_SCRIPTVM_VARIABLE cVar)
{
	PX_SCRIPTVM_VARIABLE *pVar;
	PX_ScriptVM_InstanceThread *pT=&Ins->pThread[Ins->T];
	if (pT->SP>0)
	{
		pT->SP-=1;
	}
	else
		return;
	
	pVar=&Ins->_mem[pT->SP];

	if (pVar->type==PX_SCRIPTVM_VARIABLE_TYPE_STRING)
	{
		PX_StringFree(&pVar->_string);
	}

	if (pVar->type==PX_SCRIPTVM_VARIABLE_TYPE_MEMORY)
	{
		PX_MemoryFree(&pVar->_memory);
	}


	if (cVar.type==PX_SCRIPTVM_VARIABLE_TYPE_STRING)
	{
		PX_StringInitialize(Ins->mp,&pVar->_string);
		PX_StringCat(&pVar->_string,cVar._string.buffer);
		pVar->type=cVar.type;
	}
	else if (cVar.type==PX_SCRIPTVM_VARIABLE_TYPE_MEMORY)
	{
		PX_MemoryInitialize(Ins->mp,&pVar->_memory);
		PX_MemoryAlloc(&pVar->_memory,cVar._memory.usedsize);
		PX_MemoryCat(&pVar->_memory,cVar._memory.buffer,cVar._memory.usedsize);
		pVar->type=cVar.type;
	}
	else
	{
		*pVar=cVar;
	}

}

px_void PX_ScriptVM_POPN(PX_ScriptVM_Instance *Ins,px_int T,px_int n)
{
	PX_SCRIPTVM_VARIABLE *pVar;
	px_int i;
	PX_ScriptVM_InstanceThread *pT=&Ins->pThread[T];

	for (i=0;i<n;i++)
	{
		pVar=&Ins->_mem[pT->SP+i];

		if (pVar->type==PX_SCRIPTVM_VARIABLE_TYPE_STRING)
		{
			PX_StringFree(&pVar->_string);
		}

		if (pVar->type==PX_SCRIPTVM_VARIABLE_TYPE_MEMORY)
		{
			PX_MemoryFree(&pVar->_memory);
		}

		Ins->_mem[pT->SP+i].type=PX_SCRIPTVM_VARIABLE_TYPE_INT;
		Ins->_mem[pT->SP+i]._int=0;
	}
	pT->SP+=n;
	if (pT->SP>Ins->VM_memsize)
	{
		PX_ScriptVM_Error(Ins,"host call Stack overflow!");
	}
}

PX_SCRIPTVM_VARIABLE PX_ScriptVM_Variable_int(px_int _int)
{
	PX_SCRIPTVM_VARIABLE var;
	var.type=PX_SCRIPTVM_VARIABLE_TYPE_INT;
	var._int=_int;
	return var;
}

PX_SCRIPTVM_VARIABLE PX_ScriptVM_Variable_float(px_float _float)
{
	PX_SCRIPTVM_VARIABLE var;
	var.type=PX_SCRIPTVM_VARIABLE_TYPE_FLOAT;
	var._float=_float;
	return var;
}

PX_SCRIPTVM_VARIABLE PX_ScriptVM_Variable_string(px_string _string)
{
	PX_SCRIPTVM_VARIABLE var;
	var.type=PX_SCRIPTVM_VARIABLE_TYPE_STRING;
	var._string=_string;
	return var;
}

PX_SCRIPTVM_VARIABLE PX_ScriptVM_Variable_memory(px_byte *buffer,px_int _size)
{
	PX_SCRIPTVM_VARIABLE var;
	var.type=PX_SCRIPTVM_VARIABLE_TYPE_MEMORY;
	var._memory.allocsize=0;
	var._memory.buffer=buffer;
	var._memory.usedsize=_size;
	return var;
}

PX_SCRIPTVM_VARIABLE PX_ScriptVM_Variable_const_string(px_char *buffer)
{
	PX_SCRIPTVM_VARIABLE var;
	var.type=PX_SCRIPTVM_VARIABLE_TYPE_STRING;
	var._string.buffer=buffer;
	var._string.bufferlen=0;
	var._string.mp=PX_NULL;
	return var;
}

PX_SCRIPTVM_VARIABLE PX_ScriptVM_Variable_const_memory(px_byte *buffer,px_int _size)
{
	PX_SCRIPTVM_VARIABLE var;
	var.type=PX_SCRIPTVM_VARIABLE_TYPE_MEMORY;
	var._memory.allocsize=0;
	var._memory.buffer=buffer;
	var._memory.usedsize=_size;
	return var;
}


px_bool PX_ScriptVM_DebuggerEnable(PX_ScriptVM_Instance *Ins,PX_ScriptVM_DebuggerPrint _printFunc,PX_ScriptVM_DebuggerCommand _cmdFunc)
{
	if (g_scriptVM_printFunc&&_printFunc!=g_scriptVM_printFunc)
	{
		return PX_FALSE;
	}
	if (g_scriptVM_cmdFunc&&_cmdFunc!=g_scriptVM_cmdFunc)
	{
		return PX_FALSE;
	}
	g_scriptVM_cmdFunc=_cmdFunc;
	g_scriptVM_printFunc=_printFunc;
	Ins->bp_IP=-1;
	Ins->bp_next=PX_TRUE;
	Ins->debug=PX_TRUE;

	g_scriptVM_printFunc("Story Script debugger enabled.\n");
	return PX_TRUE;
}


px_int PX_ScriptVM_GetFunctionIndex(PX_ScriptVM_Instance *Ins,px_char *func)
{
	int i;
	px_char cmpName[sizeof(Ins->_func[0].name)];
	if (func==PX_NULL)
	{
		return -1;
	}
	if (PX_strlen(func)>=sizeof(cmpName))
	{
		return -1;
	}
	PX_strcpy(cmpName,func,sizeof(cmpName));
	PX_strupr(cmpName);

	for (i=0;i<Ins->funcCount;i++)
	{
		if(PX_strequ(Ins->_func[i].name,cmpName))
		{
			return i;
		}
	}

	return -1;
}

px_bool PX_ScriptVM_LocalAlloc(PX_ScriptVM_Instance *Ins,int size,PX_SCRIPTVM_MEMORY_PTR *mem_ptr)
{
	px_int i;
	PX_ScriptVM_InstanceThread *pT=&Ins->pThread[Ins->T];

	if (size>pT->SP-Ins->VM_Globalsize)
	{
		return PX_FALSE;
	}
	pT->SP-=size;
	for (i=pT->SP;i<pT->SP+size;i++)
	{
		if (Ins->_mem[i].type==PX_SCRIPTVM_VARIABLE_TYPE_MEMORY)
		{
			PX_MemoryFree(&Ins->_mem[i]._memory);
		}
		if (Ins->_mem[i].type==PX_SCRIPTVM_VARIABLE_TYPE_STRING)
		{
			PX_StringFree(&Ins->_mem[i]._string);
		}
		Ins->_mem[i].type=PX_SCRIPTVM_VARIABLE_TYPE_INT;
	}
	mem_ptr->ptr= pT->SP;
	mem_ptr->size=size;
	return PX_TRUE;
}

px_bool PX_ScriptVM_LocalFree(PX_ScriptVM_Instance *Ins,PX_SCRIPTVM_MEMORY_PTR *mem_ptr)
{
	px_int i;
	PX_ScriptVM_InstanceThread *pT=&Ins->pThread[Ins->T];

	if (pT->SP!=mem_ptr->ptr)
	{
		return PX_FALSE;
	}
	for (i=pT->SP;i<pT->SP+mem_ptr->size;i++)
	{
		if (Ins->_mem[i].type==PX_SCRIPTVM_VARIABLE_TYPE_MEMORY)
		{
			PX_MemoryFree(&Ins->_mem[i]._memory);
		}
		if (Ins->_mem[i].type==PX_SCRIPTVM_VARIABLE_TYPE_STRING)
		{
			PX_StringFree(&Ins->_mem[i]._string);
		}
		Ins->_mem[i].type=PX_SCRIPTVM_VARIABLE_TYPE_INT;
	}
	pT->SP+=mem_ptr->size;
	return PX_TRUE;
}

static px_void  PX_ScriptVM_DebuggerGetParamName(PX_ScriptVM_Instance *Ins,px_char optype,px_int param)
{
	px_int i;
	PX_ScriptVM_InstanceThread *pT=&Ins->pThread[Ins->T];

	switch (optype)
	{
	case PX_SCRIPT_ASM_OPTYPE_IP:
		{
			g_scriptVM_printFunc("IP");
		}
		break;

	case PX_SCRIPT_ASM_OPTYPE_SP:
		{
			g_scriptVM_printFunc("SP");
		}
		break;

	case PX_SCRIPT_ASM_OPTYPE_BP:
		{
			g_scriptVM_printFunc("BP");
		}
		break;
	case PX_SCRIPT_ASM_OPTYPE_HOST:
	case PX_SCRIPT_ASM_OPTYPE_INT:
		{
			g_scriptVM_printFunc("%d",param);
		}
		break;
	case PX_SCRIPT_ASM_OPTYPE_FLOAT:
		{
			//Must be IEEE 754 format
			g_scriptVM_printFunc("%f",param);
		}
		break;
	case PX_SCRIPT_ASM_OPTYPE_STRING:
		{
			g_scriptVM_printFunc("%d",Ins->_string+param);
		}
		break;

	case PX_SCRIPT_ASM_OPTYPE_MEMORY:
		{
			g_scriptVM_printFunc("@");
			for (i=0;i<*(px_int *)(Ins->_memory+param);i++)
			{
				g_scriptVM_printFunc("%x",*(Ins->_memory+param+4+i));
			}
			g_scriptVM_printFunc("@");
		}
		break;

	case PX_SCRIPT_ASM_OPTYPE_REG:
		{
			g_scriptVM_printFunc("R%d",param);
		}
		break;
	case PX_SCRIPT_ASM_OPTYPE_LOCAL_CONST:
		{
			g_scriptVM_printFunc("LOCAL[%d]",param);
		}
		break;
	case PX_SCRIPT_ASM_OPTYPE_LOCAL_REGREF:
		{
			g_scriptVM_printFunc("LOCAL[R%d]",param);
		}
		break;
	case PX_SCRIPT_ASM_OPTYPE_LOCAL_GLOBALREF:
		{
			g_scriptVM_printFunc("GLOBAL[LOCAL[%d]]",param);
		}
		break;
	case PX_SCRIPT_ASM_OPTYPE_LOCAL_LOCALREF:
		{
			g_scriptVM_printFunc("LOCAL[LOCAL[%d]]",param);
		}
		break;
	case PX_SCRIPT_ASM_OPTYPE_GLOBAL_CONST:
		{
			g_scriptVM_printFunc("GLOBAL[%d]",param);
		}
		break;
	case PX_SCRIPT_ASM_OPTYPE_GLOBAL_SPREF:
		{
			g_scriptVM_printFunc("GLOBAL[%d]",pT->SP+param);
		}
		break;
	case PX_SCRIPT_ASM_OPTYPE_GLOBAL_REGREF:
		{
			g_scriptVM_printFunc("GLOBAL[R%d]",param);
		}
		break;
	case PX_SCRIPT_ASM_OPTYPE_GLOBAL_GLOBALREF:
		{
			g_scriptVM_printFunc("GLOBAL[GLOBAL[%d]]",param);
		}
		break;
	case PX_SCRIPT_ASM_OPTYPE_GLOBAL_LOCALREF:
		{
			g_scriptVM_printFunc("GLOBAL[LOCAL[%d]]",param);
		}
		break;

	default:
		{
			PX_ScriptVM_Error(Ins,"Unknow parameter");
		}
	}
}

px_void PX_ScriptVM_DebuggerDisplayBinary(PX_ScriptVM_Instance *Ins,px_int IP,px_int size)
{
	px_int i;
	px_byte *buffer=(Ins->_bin+IP);
	for (i=0;i<size;i++)
	{
		g_scriptVM_printFunc("%02X",buffer[i]);
	}
	if (size<12)
	{
		for (i=0;i<12-size;i++)
		{
			g_scriptVM_printFunc("  ");
		}
	}
}


px_int PX_ScriptVM_DebuggerCurrentInstruction(PX_ScriptVM_Instance *Ins,px_int IP)
{
	px_char opCode,opType[3];

	if (IP>=Ins->binsize)
	{
		return 0;
	}
	opCode=*(Ins->_bin+IP);
	opType[0]=*(Ins->_bin+IP+1);
	opType[1]=*(Ins->_bin+IP+2);
	opType[2]=*(Ins->_bin+IP+3);


		switch(opCode)
		{
		case PX_SCRIPT_ASM_INSTR_OPCODE_MOV:
			{
				PX_ScriptVM_DebuggerDisplayBinary(Ins,IP,4+2*4);
				g_scriptVM_printFunc(" ");
				g_scriptVM_printFunc("MOV ");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[0],PX_SCRIPT_VM_IP_PARAM(0));
				g_scriptVM_printFunc(",");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[1],PX_SCRIPT_VM_IP_PARAM(1));
				return 4+2*4;
			}
			break;
		case PX_SCRIPT_ASM_INSTR_OPCODE_ADD:
			{
				PX_ScriptVM_DebuggerDisplayBinary(Ins,IP,4+2*4);
				g_scriptVM_printFunc(" ");
				g_scriptVM_printFunc("ADD ");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[0],PX_SCRIPT_VM_IP_PARAM(0));
				g_scriptVM_printFunc(",");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[1],PX_SCRIPT_VM_IP_PARAM(1));
				return 4+2*4;
			}
			break;
		case PX_SCRIPT_ASM_INSTR_OPCODE_SUB:
			{
				PX_ScriptVM_DebuggerDisplayBinary(Ins,IP,4+2*4);
				g_scriptVM_printFunc(" ");
				g_scriptVM_printFunc("SUB ");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[0],PX_SCRIPT_VM_IP_PARAM(0));
				g_scriptVM_printFunc(",");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[1],PX_SCRIPT_VM_IP_PARAM(1));
				return 4+2*4;
			}
			break;
		case PX_SCRIPT_ASM_INSTR_OPCODE_DIV:
			{
				PX_ScriptVM_DebuggerDisplayBinary(Ins,IP,4+2*4);
				g_scriptVM_printFunc(" ");
				g_scriptVM_printFunc("DIV ");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[0],PX_SCRIPT_VM_IP_PARAM(0));
				g_scriptVM_printFunc(",");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[1],PX_SCRIPT_VM_IP_PARAM(1));
				return 4+2*4;
			}
			break;
		case PX_SCRIPT_ASM_INSTR_OPCODE_MUL:
			{
				PX_ScriptVM_DebuggerDisplayBinary(Ins,IP,4+2*4);
				g_scriptVM_printFunc(" ");
				g_scriptVM_printFunc("MUL ");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[0],PX_SCRIPT_VM_IP_PARAM(0));
				g_scriptVM_printFunc(",");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[1],PX_SCRIPT_VM_IP_PARAM(1));
				return 4+2*4;
			}
			break;
		case PX_SCRIPT_ASM_INSTR_OPCODE_MOD:
			{
				PX_ScriptVM_DebuggerDisplayBinary(Ins,IP,4+2*4);
				g_scriptVM_printFunc(" ");
				g_scriptVM_printFunc("MOD ");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[0],PX_SCRIPT_VM_IP_PARAM(0));
				g_scriptVM_printFunc(",");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[1],PX_SCRIPT_VM_IP_PARAM(1));
				return 4+2*4;
			}
			break;
		case PX_SCRIPT_ASM_INSTR_OPCODE_SHL:
			{
				PX_ScriptVM_DebuggerDisplayBinary(Ins,IP,4+2*4);
				g_scriptVM_printFunc(" ");
				g_scriptVM_printFunc("SHL ");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[0],PX_SCRIPT_VM_IP_PARAM(0));
				g_scriptVM_printFunc(",");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[1],PX_SCRIPT_VM_IP_PARAM(1));
				return 4+2*4;
			}
			break;
		case PX_SCRIPT_ASM_INSTR_OPCODE_SHR:
			{
				PX_ScriptVM_DebuggerDisplayBinary(Ins,IP,4+2*4);
				g_scriptVM_printFunc(" ");
				g_scriptVM_printFunc("SHR ");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[0],PX_SCRIPT_VM_IP_PARAM(0));
				g_scriptVM_printFunc(",");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[1],PX_SCRIPT_VM_IP_PARAM(1));
				return 4+2*4;
			}
			break;
		case PX_SCRIPT_ASM_INSTR_OPCODE_AND:
			{
				PX_ScriptVM_DebuggerDisplayBinary(Ins,IP,4+2*4);
				g_scriptVM_printFunc(" ");
				g_scriptVM_printFunc("AND ");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[0],PX_SCRIPT_VM_IP_PARAM(0));
				g_scriptVM_printFunc(",");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[1],PX_SCRIPT_VM_IP_PARAM(1));
				return 4+2*4;
			}
			break;
		case PX_SCRIPT_ASM_INSTR_OPCODE_ANDL:
			{
				PX_ScriptVM_DebuggerDisplayBinary(Ins,IP,4+2*4);
				g_scriptVM_printFunc(" ");
				g_scriptVM_printFunc("ANDL ");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[0],PX_SCRIPT_VM_IP_PARAM(0));
				g_scriptVM_printFunc(",");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[1],PX_SCRIPT_VM_IP_PARAM(1));
				return 4+2*4;
			}
			break;
		case PX_SCRIPT_ASM_INSTR_OPCODE_ORL:
			{
				PX_ScriptVM_DebuggerDisplayBinary(Ins,IP,4+2*4);
				g_scriptVM_printFunc(" ");
				g_scriptVM_printFunc("ORL ");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[0],PX_SCRIPT_VM_IP_PARAM(0));
				g_scriptVM_printFunc(",");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[1],PX_SCRIPT_VM_IP_PARAM(1));
				return 4+2*4;
			}
			break;

		case PX_SCRIPT_ASM_INSTR_OPCODE_OR:
			{
				PX_ScriptVM_DebuggerDisplayBinary(Ins,IP,4+2*4);
				g_scriptVM_printFunc(" ");
				g_scriptVM_printFunc("OR ");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[0],PX_SCRIPT_VM_IP_PARAM(0));
				g_scriptVM_printFunc(",");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[1],PX_SCRIPT_VM_IP_PARAM(1));
				return 4+2*4;
			}
			break;
		case PX_SCRIPT_ASM_INSTR_OPCODE_XOR:
			{
				PX_ScriptVM_DebuggerDisplayBinary(Ins,IP,4+2*4);
				g_scriptVM_printFunc(" ");
				g_scriptVM_printFunc("XOR ");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[0],PX_SCRIPT_VM_IP_PARAM(0));
				g_scriptVM_printFunc(",");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[1],PX_SCRIPT_VM_IP_PARAM(1));
				return 4+2*4;
			}
			break;
		case PX_SCRIPT_ASM_INSTR_OPCODE_POW:
			{
				PX_ScriptVM_DebuggerDisplayBinary(Ins,IP,4+2*4);
				g_scriptVM_printFunc(" ");
				g_scriptVM_printFunc("POW ");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[0],PX_SCRIPT_VM_IP_PARAM(0));
				g_scriptVM_printFunc(",");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[1],PX_SCRIPT_VM_IP_PARAM(1));
				return 4+2*4;
			}
			break;
		case PX_SCRIPT_ASM_INSTR_OPCODE_SIN:
			{
				PX_ScriptVM_DebuggerDisplayBinary(Ins,IP,4+2*4);
				g_scriptVM_printFunc(" ");
				g_scriptVM_printFunc("SIN ");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[0],PX_SCRIPT_VM_IP_PARAM(0));
				g_scriptVM_printFunc(",");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[1],PX_SCRIPT_VM_IP_PARAM(1));
				return 4+2*4;
			}
			break;
		case PX_SCRIPT_ASM_INSTR_OPCODE_COS:
			{
				PX_ScriptVM_DebuggerDisplayBinary(Ins,IP,4+2*4);
				g_scriptVM_printFunc(" ");
				g_scriptVM_printFunc("COS ");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[0],PX_SCRIPT_VM_IP_PARAM(0));
				g_scriptVM_printFunc(",");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[1],PX_SCRIPT_VM_IP_PARAM(1));
				return 4+2*4;
			}
			break;
		case PX_SCRIPT_ASM_INSTR_OPCODE_INT:
			{
				PX_ScriptVM_DebuggerDisplayBinary(Ins,IP,4+2*4);
				g_scriptVM_printFunc(" ");
				g_scriptVM_printFunc("INT ");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[0],PX_SCRIPT_VM_IP_PARAM(0));
				return 4+1*4;
			}
			break;
		case PX_SCRIPT_ASM_INSTR_OPCODE_FLT:
			{
				PX_ScriptVM_DebuggerDisplayBinary(Ins,IP,4+1*4);
				g_scriptVM_printFunc(" ");
				g_scriptVM_printFunc("FLT ");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[0],PX_SCRIPT_VM_IP_PARAM(0));
				return 4+1*4;
			}
			break;
		case PX_SCRIPT_ASM_INSTR_OPCODE_STRLEN:
			{
				PX_ScriptVM_DebuggerDisplayBinary(Ins,IP,4+2*4);
				g_scriptVM_printFunc(" ");
				g_scriptVM_printFunc("STRLEN ");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[0],PX_SCRIPT_VM_IP_PARAM(0));
				g_scriptVM_printFunc(",");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[1],PX_SCRIPT_VM_IP_PARAM(1));
				return 4+2*4;
			}
			break;
		case PX_SCRIPT_ASM_INSTR_OPCODE_STRCAT:
			{
				PX_ScriptVM_DebuggerDisplayBinary(Ins,IP,4+2*4);
				g_scriptVM_printFunc(" ");
				g_scriptVM_printFunc("STRCAT ");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[0],PX_SCRIPT_VM_IP_PARAM(0));
				g_scriptVM_printFunc(",");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[1],PX_SCRIPT_VM_IP_PARAM(1));
				return 4+2*4;
			}
			break;
		case PX_SCRIPT_ASM_INSTR_OPCODE_STRREP:
			{
				PX_ScriptVM_DebuggerDisplayBinary(Ins,IP,4+3*4);
				g_scriptVM_printFunc(" ");
				g_scriptVM_printFunc("STRREP ");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[0],PX_SCRIPT_VM_IP_PARAM(0));
				g_scriptVM_printFunc(",");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[1],PX_SCRIPT_VM_IP_PARAM(1));
				g_scriptVM_printFunc(",");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[2],PX_SCRIPT_VM_IP_PARAM(2));
				return 4+3*4;
			}
			break;
		case PX_SCRIPT_ASM_INSTR_OPCODE_STRCHR:
			{
				PX_ScriptVM_DebuggerDisplayBinary(Ins,IP,4+3*4);
				g_scriptVM_printFunc(" ");
				g_scriptVM_printFunc("STRCHR ");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[0],PX_SCRIPT_VM_IP_PARAM(0));
				g_scriptVM_printFunc(",");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[1],PX_SCRIPT_VM_IP_PARAM(1));
				g_scriptVM_printFunc(",");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[2],PX_SCRIPT_VM_IP_PARAM(2));
				return 4+3*4;
			}
			break;

		case PX_SCRIPT_ASM_INSTR_OPCODE_STRTOI:
			{
				PX_ScriptVM_DebuggerDisplayBinary(Ins,IP,4+2*4);
				g_scriptVM_printFunc(" ");
				g_scriptVM_printFunc("STRTOI ");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[0],PX_SCRIPT_VM_IP_PARAM(0));
				g_scriptVM_printFunc(",");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[1],PX_SCRIPT_VM_IP_PARAM(1));
				return 4+2*4;
			}
			break;
		case PX_SCRIPT_ASM_INSTR_OPCODE_STRTOF:
			{
				PX_ScriptVM_DebuggerDisplayBinary(Ins,IP,4+2*4);
				g_scriptVM_printFunc(" ");
				g_scriptVM_printFunc("STRTOF ");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[0],PX_SCRIPT_VM_IP_PARAM(0));
				g_scriptVM_printFunc(",");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[1],PX_SCRIPT_VM_IP_PARAM(1));
				return 4+2*4;
			}
			break;

		case PX_SCRIPT_ASM_INSTR_OPCODE_STRFRI:
			{
				PX_ScriptVM_DebuggerDisplayBinary(Ins,IP,4+2*4);
				g_scriptVM_printFunc(" ");
				g_scriptVM_printFunc("STRFRI ");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[0],PX_SCRIPT_VM_IP_PARAM(0));
				g_scriptVM_printFunc(",");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[1],PX_SCRIPT_VM_IP_PARAM(1));
				return 4+2*4;
			}
			break;

		case PX_SCRIPT_ASM_INSTR_OPCODE_STRFRF:
			{
				PX_ScriptVM_DebuggerDisplayBinary(Ins,IP,4+2*4);
				g_scriptVM_printFunc(" ");
				g_scriptVM_printFunc("STRFRF ");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[0],PX_SCRIPT_VM_IP_PARAM(0));
				g_scriptVM_printFunc(",");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[1],PX_SCRIPT_VM_IP_PARAM(1));
				return 4+2*4;
			}
			break;
		case PX_SCRIPT_ASM_INSTR_OPCODE_STRTMEM:
			{
				PX_ScriptVM_DebuggerDisplayBinary(Ins,IP,4+1*4);
				g_scriptVM_printFunc(" ");
				g_scriptVM_printFunc("STRMEM ");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[0],PX_SCRIPT_VM_IP_PARAM(0));
				return 4+1*4;
			}
			break;

		case PX_SCRIPT_ASM_INSTR_OPCODE_STRSET:
			{
				PX_ScriptVM_DebuggerDisplayBinary(Ins,IP,4+2*4);
				g_scriptVM_printFunc(" ");
				g_scriptVM_printFunc("STRSET ");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[0],PX_SCRIPT_VM_IP_PARAM(0));
				g_scriptVM_printFunc(",");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[1],PX_SCRIPT_VM_IP_PARAM(1));
				return 4+2*4;
			}
			break;

		case PX_SCRIPT_ASM_INSTR_OPCODE_STRFIND:
			{
				PX_ScriptVM_DebuggerDisplayBinary(Ins,IP,4+2*4);
				g_scriptVM_printFunc(" ");
				g_scriptVM_printFunc("STRFIND ");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[0],PX_SCRIPT_VM_IP_PARAM(0));
				g_scriptVM_printFunc(",");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[1],PX_SCRIPT_VM_IP_PARAM(1));
				g_scriptVM_printFunc(",");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[2],PX_SCRIPT_VM_IP_PARAM(2));
				return 4+3*4;
			}
			break;

		case PX_SCRIPT_ASM_INSTR_OPCODE_ASC:
			{
				PX_ScriptVM_DebuggerDisplayBinary(Ins,IP,4+2*4);
				g_scriptVM_printFunc(" ");
				g_scriptVM_printFunc("ASC ");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[0],PX_SCRIPT_VM_IP_PARAM(0));
				g_scriptVM_printFunc(",");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[1],PX_SCRIPT_VM_IP_PARAM(1));
				return 4+2*4;
			}
			break;

		case PX_SCRIPT_ASM_INSTR_OPCODE_MEMBYTE:
			{
				PX_ScriptVM_DebuggerDisplayBinary(Ins,IP,4+3*4);
				g_scriptVM_printFunc(" ");
				g_scriptVM_printFunc("MEMBYTE ");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[0],PX_SCRIPT_VM_IP_PARAM(0));
				g_scriptVM_printFunc(",");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[1],PX_SCRIPT_VM_IP_PARAM(1));
				g_scriptVM_printFunc(",");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[2],PX_SCRIPT_VM_IP_PARAM(2));
				return 4+2*4;
			}
			break;

		case PX_SCRIPT_ASM_INSTR_OPCODE_MEMSET:
			{
				PX_ScriptVM_DebuggerDisplayBinary(Ins,IP,4+3*4);
				g_scriptVM_printFunc(" ");
				g_scriptVM_printFunc("MEMSET ");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[0],PX_SCRIPT_VM_IP_PARAM(0));
				g_scriptVM_printFunc(",");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[1],PX_SCRIPT_VM_IP_PARAM(1));
				g_scriptVM_printFunc(",");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[2],PX_SCRIPT_VM_IP_PARAM(2));
				return 4+3*4;
			}
			break;

		case PX_SCRIPT_ASM_INSTR_OPCODE_MEMTRM:
			{
				PX_ScriptVM_DebuggerDisplayBinary(Ins,IP,4+3*4);
				g_scriptVM_printFunc(" ");
				g_scriptVM_printFunc("MEMCPY ");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[0],PX_SCRIPT_VM_IP_PARAM(0));
				g_scriptVM_printFunc(",");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[1],PX_SCRIPT_VM_IP_PARAM(1));
				g_scriptVM_printFunc(",");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[2],PX_SCRIPT_VM_IP_PARAM(2));
				return 4+3*4;
			}
			break;

		case PX_SCRIPT_ASM_INSTR_OPCODE_MEMFIND:
			{
				PX_ScriptVM_DebuggerDisplayBinary(Ins,IP,4+3*4);
				g_scriptVM_printFunc(" ");
				g_scriptVM_printFunc("MEMFIND ");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[0],PX_SCRIPT_VM_IP_PARAM(0));
				g_scriptVM_printFunc(",");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[1],PX_SCRIPT_VM_IP_PARAM(1));
				g_scriptVM_printFunc(",");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[2],PX_SCRIPT_VM_IP_PARAM(2));
				return 4+3*4;
			}
			break;

		case PX_SCRIPT_ASM_INSTR_OPCODE_MEMLEN:
			{
				PX_ScriptVM_DebuggerDisplayBinary(Ins,IP,4+2*4);
				g_scriptVM_printFunc(" ");
				g_scriptVM_printFunc("MEMLEN ");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[0],PX_SCRIPT_VM_IP_PARAM(0));
				g_scriptVM_printFunc(",");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[1],PX_SCRIPT_VM_IP_PARAM(1));
				return 4+2*4;
			}
			break;

		case PX_SCRIPT_ASM_INSTR_OPCODE_MEMCAT:
			{
				PX_ScriptVM_DebuggerDisplayBinary(Ins,IP,4+2*4);
				g_scriptVM_printFunc(" ");
				g_scriptVM_printFunc("MEMCAT ");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[0],PX_SCRIPT_VM_IP_PARAM(0));
				g_scriptVM_printFunc(",");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[1],PX_SCRIPT_VM_IP_PARAM(1));
				return 4+2*4;
			}
			break;
		case PX_SCRIPT_ASM_INSTR_OPCODE_MEMTSTR:
			{
				PX_ScriptVM_DebuggerDisplayBinary(Ins,IP,4+1*4);
				g_scriptVM_printFunc(" ");
				g_scriptVM_printFunc("MEMSTR ");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[0],PX_SCRIPT_VM_IP_PARAM(0));
				return 4+1*4;
			}
			break;
		case PX_SCRIPT_ASM_INSTR_OPCODE_DATACPY:
			{
				PX_ScriptVM_DebuggerDisplayBinary(Ins,IP,4+3*4);
				g_scriptVM_printFunc(" ");
				g_scriptVM_printFunc("DATACPY ");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[0],PX_SCRIPT_VM_IP_PARAM(0));
				g_scriptVM_printFunc(",");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[1],PX_SCRIPT_VM_IP_PARAM(1));
				g_scriptVM_printFunc(",");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[2],PX_SCRIPT_VM_IP_PARAM(2));
				return 4+3*4;
			}
			break;
		case PX_SCRIPT_ASM_INSTR_OPCODE_NEG:
			{
				PX_ScriptVM_DebuggerDisplayBinary(Ins,IP,4+1*4);
				g_scriptVM_printFunc(" ");
				g_scriptVM_printFunc("NEG ");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[0],PX_SCRIPT_VM_IP_PARAM(0));
				return 4+1*4;
			}
			break;
		case PX_SCRIPT_ASM_INSTR_OPCODE_INV:
			{
				PX_ScriptVM_DebuggerDisplayBinary(Ins,IP,4+1*4);
				g_scriptVM_printFunc(" ");
				g_scriptVM_printFunc("INV ");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[0],PX_SCRIPT_VM_IP_PARAM(0));
				return 4+1*4;
			}
			break;

		case PX_SCRIPT_ASM_INSTR_OPCODE_NOT:
			{
				PX_ScriptVM_DebuggerDisplayBinary(Ins,IP,4+1*4);
				g_scriptVM_printFunc(" ");
				g_scriptVM_printFunc("NOT ");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[0],PX_SCRIPT_VM_IP_PARAM(0));
				return 4+1*4;
			}
			break;

		case PX_SCRIPT_ASM_INSTR_OPCODE_JMP:
			{
				PX_ScriptVM_DebuggerDisplayBinary(Ins,IP,4+1*4);
				g_scriptVM_printFunc(" ");
				g_scriptVM_printFunc("JMP ");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[0],PX_SCRIPT_VM_IP_PARAM(0));
				return 4+1*4;
			}
			break;

		case PX_SCRIPT_ASM_INSTR_OPCODE_JE:
			{
				PX_ScriptVM_DebuggerDisplayBinary(Ins,IP,4+3*4);
				g_scriptVM_printFunc(" ");
				g_scriptVM_printFunc("JE ");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[0],PX_SCRIPT_VM_IP_PARAM(0));
				g_scriptVM_printFunc(",");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[1],PX_SCRIPT_VM_IP_PARAM(1));
				g_scriptVM_printFunc(",");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[2],PX_SCRIPT_VM_IP_PARAM(2));
				return 4+3*4;
			}
			break;

		case PX_SCRIPT_ASM_INSTR_OPCODE_JNE:
			{
				PX_ScriptVM_DebuggerDisplayBinary(Ins,IP,4+3*4);
				g_scriptVM_printFunc(" ");
				g_scriptVM_printFunc("JNE ");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[0],PX_SCRIPT_VM_IP_PARAM(0));
				g_scriptVM_printFunc(",");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[1],PX_SCRIPT_VM_IP_PARAM(1));
				g_scriptVM_printFunc(",");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[2],PX_SCRIPT_VM_IP_PARAM(2));
				return 4+3*4;
			}
			break;

		case PX_SCRIPT_ASM_INSTR_OPCODE_JL:
			{
				PX_ScriptVM_DebuggerDisplayBinary(Ins,IP,4+3*4);
				g_scriptVM_printFunc(" ");
				g_scriptVM_printFunc("JL ");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[0],PX_SCRIPT_VM_IP_PARAM(0));
				g_scriptVM_printFunc(",");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[1],PX_SCRIPT_VM_IP_PARAM(1));
				g_scriptVM_printFunc(",");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[2],PX_SCRIPT_VM_IP_PARAM(2));
				return 4+3*4;

			}
			break;

		case PX_SCRIPT_ASM_INSTR_OPCODE_JLE:
			{
				PX_ScriptVM_DebuggerDisplayBinary(Ins,IP,4+3*4);
				g_scriptVM_printFunc(" ");
				g_scriptVM_printFunc("JLE ");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[0],PX_SCRIPT_VM_IP_PARAM(0));
				g_scriptVM_printFunc(",");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[1],PX_SCRIPT_VM_IP_PARAM(1));
				g_scriptVM_printFunc(",");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[2],PX_SCRIPT_VM_IP_PARAM(2));
				return 4+3*4;

			}
			break;

		case PX_SCRIPT_ASM_INSTR_OPCODE_JG:
			{
				PX_ScriptVM_DebuggerDisplayBinary(Ins,IP,4+3*4);
				g_scriptVM_printFunc(" ");
				g_scriptVM_printFunc("JG ");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[0],PX_SCRIPT_VM_IP_PARAM(0));
				g_scriptVM_printFunc(",");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[1],PX_SCRIPT_VM_IP_PARAM(1));
				g_scriptVM_printFunc(",");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[2],PX_SCRIPT_VM_IP_PARAM(2));
				return 4+3*4;

			}
			break;

		case PX_SCRIPT_ASM_INSTR_OPCODE_JGE:
			{
				PX_ScriptVM_DebuggerDisplayBinary(Ins,IP,4+3*4);
				g_scriptVM_printFunc(" ");
				g_scriptVM_printFunc("JGE ");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[0],PX_SCRIPT_VM_IP_PARAM(0));
				g_scriptVM_printFunc(",");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[1],PX_SCRIPT_VM_IP_PARAM(1));
				g_scriptVM_printFunc(",");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[2],PX_SCRIPT_VM_IP_PARAM(2));
				return 4+3*4;
			}
			break;
		case PX_SCRIPT_ASM_INSTR_OPCODE_LGE:
			{
				PX_ScriptVM_DebuggerDisplayBinary(Ins,IP,4+3*4);
				g_scriptVM_printFunc(" ");
				g_scriptVM_printFunc("LGE ");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[0],PX_SCRIPT_VM_IP_PARAM(0));
				g_scriptVM_printFunc(",");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[1],PX_SCRIPT_VM_IP_PARAM(1));
				g_scriptVM_printFunc(",");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[2],PX_SCRIPT_VM_IP_PARAM(2));
				return 4+3*4;
			}
			break;

		case PX_SCRIPT_ASM_INSTR_OPCODE_LGNE:
			{
				PX_ScriptVM_DebuggerDisplayBinary(Ins,IP,4+3*4);
				g_scriptVM_printFunc(" ");
				g_scriptVM_printFunc("LGNE ");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[0],PX_SCRIPT_VM_IP_PARAM(0));
				g_scriptVM_printFunc(",");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[1],PX_SCRIPT_VM_IP_PARAM(1));
				g_scriptVM_printFunc(",");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[2],PX_SCRIPT_VM_IP_PARAM(2));
				return 4+3*4;
			}
			break;

		case PX_SCRIPT_ASM_INSTR_OPCODE_LGZ:
			{
				PX_ScriptVM_DebuggerDisplayBinary(Ins,IP,4+1*4);
				g_scriptVM_printFunc(" ");
				g_scriptVM_printFunc("LGZ ");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[0],PX_SCRIPT_VM_IP_PARAM(0));
				return 4+1*4;
			}
			break;

		case PX_SCRIPT_ASM_INSTR_OPCODE_LGGZ:
			{
				PX_ScriptVM_DebuggerDisplayBinary(Ins,IP,4+1*4);
				g_scriptVM_printFunc(" ");
				g_scriptVM_printFunc("LGGZ ");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[0],PX_SCRIPT_VM_IP_PARAM(0));
				return 4+1*4;
			}
			break;
		case PX_SCRIPT_ASM_INSTR_OPCODE_LGGEZ:
			{
				PX_ScriptVM_DebuggerDisplayBinary(Ins,IP,4+1*4);
				g_scriptVM_printFunc(" ");
				g_scriptVM_printFunc("LGGEZ ");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[0],PX_SCRIPT_VM_IP_PARAM(0));
				return 4+1*4;
			}
			break;
		case PX_SCRIPT_ASM_INSTR_OPCODE_LGLZ:
			{
				PX_ScriptVM_DebuggerDisplayBinary(Ins,IP,4+1*4);
				g_scriptVM_printFunc(" ");
				g_scriptVM_printFunc("LGLZ ");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[0],PX_SCRIPT_VM_IP_PARAM(0));
				return 4+1*4;
			}
			break;
		case PX_SCRIPT_ASM_INSTR_OPCODE_LGLEZ:
			{
				PX_ScriptVM_DebuggerDisplayBinary(Ins,IP,4+1*4);
				g_scriptVM_printFunc(" ");
				g_scriptVM_printFunc("LGLZE ");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[0],PX_SCRIPT_VM_IP_PARAM(0));
				return 4+1*4;
			}
			break;


		case PX_SCRIPT_ASM_INSTR_OPCODE_CALL:
			{
				PX_ScriptVM_DebuggerDisplayBinary(Ins,IP,4+1*4);
				g_scriptVM_printFunc(" ");
				g_scriptVM_printFunc("CALL ");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[0],PX_SCRIPT_VM_IP_PARAM(0));
				return 4+1*4;
			}
			break;

		case PX_SCRIPT_ASM_INSTR_OPCODE_PUSH:
			{
				PX_ScriptVM_DebuggerDisplayBinary(Ins,IP,4+1*4);
				g_scriptVM_printFunc(" ");
				g_scriptVM_printFunc("PUSH ");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[0],PX_SCRIPT_VM_IP_PARAM(0));
				return 4+1*4;
			}
			break;

		case PX_SCRIPT_ASM_INSTR_OPCODE_POP:
			{
				PX_ScriptVM_DebuggerDisplayBinary(Ins,IP,4+1*4);
				g_scriptVM_printFunc(" ");
				g_scriptVM_printFunc("POP ");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[0],PX_SCRIPT_VM_IP_PARAM(0));
				return 4+1*4;
			}
			break;

		case PX_SCRIPT_ASM_INSTR_OPCODE_POPN:
			{
				PX_ScriptVM_DebuggerDisplayBinary(Ins,IP,4+1*4);
				g_scriptVM_printFunc(" ");
				g_scriptVM_printFunc("POPN ");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[0],PX_SCRIPT_VM_IP_PARAM(0));
				return 4+1*4;
			}
			break;
		case PX_SCRIPT_ASM_INSTR_OPCODE_ADR:
			{
				PX_ScriptVM_DebuggerDisplayBinary(Ins,IP,4+1*4);
				g_scriptVM_printFunc(" ");
				g_scriptVM_printFunc("ADR ");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[0],PX_SCRIPT_VM_IP_PARAM(0));
				return 4+1*4;
			}
			break;

		case PX_SCRIPT_ASM_INSTR_OPCODE_RET:
			{
				PX_ScriptVM_DebuggerDisplayBinary(Ins,IP,4);
				g_scriptVM_printFunc(" ");
				g_scriptVM_printFunc("RET");
				return 4;
			}
			break;
		case PX_SCRIPT_ASM_INSTR_OPCODE_NOP:
			{
				PX_ScriptVM_DebuggerDisplayBinary(Ins,IP,4);
				g_scriptVM_printFunc(" ");
				g_scriptVM_printFunc("NOP");
				return 4;
			}
			break;
		case PX_SCRIPT_ASM_INSTR_OPCODE_BPX:
			{
				PX_ScriptVM_DebuggerDisplayBinary(Ins,IP,4);
				g_scriptVM_printFunc(" ");
				g_scriptVM_printFunc("BPX");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[0],PX_SCRIPT_VM_IP_PARAM(0));
				return 8;
			}
			break;
		case PX_SCRIPT_ASM_INSTR_OPCODE_WAIT:
			{
				PX_ScriptVM_DebuggerDisplayBinary(Ins,IP,4);
				g_scriptVM_printFunc(" ");
				g_scriptVM_printFunc("WAIT");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[0],PX_SCRIPT_VM_IP_PARAM(0));
				return 8;
			}
			break;
		case PX_SCRIPT_ASM_INSTR_OPCODE_SIGNAL:
			{
				PX_ScriptVM_DebuggerDisplayBinary(Ins,IP,4+2*4);
				g_scriptVM_printFunc(" ");
				g_scriptVM_printFunc("SIGNAL ");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[0],PX_SCRIPT_VM_IP_PARAM(0));
				g_scriptVM_printFunc(",");
				PX_ScriptVM_DebuggerGetParamName(Ins,opType[1],PX_SCRIPT_VM_IP_PARAM(1));
				return 4+2*4;
			}
			break;
		default:
			PX_ScriptVM_Error(Ins,"unknow instruction");
		}

	PX_ScriptVM_Error(Ins,"unknow instruction");
	return 0;
}
px_void PX_ScriptVM_DebuggerDisplayVariable(PX_ScriptVM_Instance *Ins,px_int index)
{
	PX_SCRIPTVM_VARIABLE var;
	if (index>=Ins->VM_memsize)
	{
		g_scriptVM_printFunc("Variable illegal.\n");
		return;
	}
	var=Ins->_mem[index];

	switch(var.type)
	{
	case PX_SCRIPTVM_VARIABLE_TYPE_INT:
		g_scriptVM_printFunc("memptr %d [INT] %d\n",index,var._int);
		break;
	case PX_SCRIPTVM_VARIABLE_TYPE_FLOAT:
		g_scriptVM_printFunc("memptr %d [FLOAT] %f\n",index,var._float);
		break;
	case PX_SCRIPTVM_VARIABLE_TYPE_MEMORY:
		g_scriptVM_printFunc("memptr %d [MEMORY]\n",index);
		break;
	case PX_SCRIPTVM_VARIABLE_TYPE_STRING:
		g_scriptVM_printFunc("memptr %d [STRING] %s\n",index,var._string.buffer);
		break;
	}
}

px_void PX_ScriptVM_DebuggerDisplayReg(PX_ScriptVM_Instance *Ins,px_int index)
{
	PX_SCRIPTVM_VARIABLE var;
	if (index>=PX_SCRIPTVM_REG_COUNT)
	{
		g_scriptVM_printFunc("Register illegal.\n");
		return;
	}
	var=Ins->pThread[Ins->T].R[index];

	switch(var.type)
	{
	case PX_SCRIPTVM_VARIABLE_TYPE_INT:
		g_scriptVM_printFunc("Thread %d R%d [INT] %d\n",Ins->T,index,var._int);
		break;
	case PX_SCRIPTVM_VARIABLE_TYPE_FLOAT:
		g_scriptVM_printFunc("Thread %d R%d [FLOAT] %f\n",Ins->T,index,var._float);
		break;
	case PX_SCRIPTVM_VARIABLE_TYPE_MEMORY:
		g_scriptVM_printFunc("Thread %d R%d [MEMORY]\n",Ins->T,index);
		break;
	case PX_SCRIPTVM_VARIABLE_TYPE_STRING:
		g_scriptVM_printFunc("Thread %d R%d [STRING] %s\n",Ins->T,index,var._string.buffer);
		break;
	}
}

static px_bool PX_ScriptVM_DebuggerSolveCmd(PX_ScriptVM_Instance *Ins,px_char *cmd)
{

	PX_LEXER_LEXEME_TYPE type;
	px_lexer lexer;
	px_int u,i,oft,instrLen;
	px_bool bContinue=PX_FALSE;

	PX_LexerInitialize(&lexer,Ins->mp);
	PX_LexerRegisterSpacer(&lexer,' ');
	PX_LexerRegisterSpacer(&lexer,'\t');

	PX_LexerLoadSourceFromMemory(&lexer,cmd);

	///////////////////////////////////////////////////////////////////////////
	//
	//b break Point
	//n next instruction
	//r run
	//u disassembly
	//global global[]
	//local local[]

	while (PX_TRUE)
	{
		type=PX_LexerGetNextLexeme(&lexer);
		if (type==PX_LEXER_LEXEME_TYPE_END)
		{
			break;
		}
		if (PX_strequ(lexer.CurLexeme.buffer,"B"))
		{
			type=PX_LexerGetNextLexeme(&lexer);
			if (type!=PX_LEXER_LEXEME_TYPE_SPACER)
			{
				break;
			}
			type=PX_LexerGetNextLexeme(&lexer);
			if(PX_strIsInt(lexer.CurLexeme.buffer))
				Ins->bp_IP=PX_atoi(lexer.CurLexeme.buffer);
			else
				break;

			type=PX_LexerGetNextLexeme(&lexer);
			if (type!=PX_LEXER_LEXEME_TYPE_END)
			{
				Ins->bp_IP=-1;
				break;
			}
		}
		else if(PX_strequ(lexer.CurLexeme.buffer,"N"))
		{
			type=PX_LexerGetNextLexeme(&lexer);
			if (type!=PX_LEXER_LEXEME_TYPE_END)
			{
				break;
			}
			Ins->bp_next=PX_TRUE;
			bContinue=PX_TRUE;
		}
		else if(PX_strequ(lexer.CurLexeme.buffer,"C"))
		{
			type=PX_LexerGetNextLexeme(&lexer);
			if (type!=PX_LEXER_LEXEME_TYPE_END)
			{
				break;
			}
			Ins->bp_next=PX_FALSE;
			bContinue=PX_TRUE;
		}
		else if(PX_strequ(lexer.CurLexeme.buffer,"U"))
		{
			type=PX_LexerGetNextLexeme(&lexer);
			if (type!=PX_LEXER_LEXEME_TYPE_SPACER)
			{
				break;
			}

			type=PX_LexerGetNextLexeme(&lexer);
			if(PX_strIsInt(lexer.CurLexeme.buffer))
				u=PX_atoi(lexer.CurLexeme.buffer);
			else
				break;

			type=PX_LexerGetNextLexeme(&lexer);
			if (type!=PX_LEXER_LEXEME_TYPE_END)
			{
				break;
			}

			oft=0;
			for (i=0;i<u;i++)
			{
				instrLen=PX_ScriptVM_DebuggerCurrentInstruction(Ins,Ins->pThread[Ins->T].IP+oft);
				oft+=instrLen;
				if(instrLen)
				g_scriptVM_printFunc("\n");
			}
			break;
		}
		else if(PX_strequ(lexer.CurLexeme.buffer,"R"))
		{
			type=PX_LexerGetNextLexeme(&lexer);
			if (type!=PX_LEXER_LEXEME_TYPE_SPACER)
			{
				break;
			}

			type=PX_LexerGetNextLexeme(&lexer);
			if(PX_strIsInt(lexer.CurLexeme.buffer))
				u=PX_atoi(lexer.CurLexeme.buffer);
			else
				break;

			type=PX_LexerGetNextLexeme(&lexer);
			if (type!=PX_LEXER_LEXEME_TYPE_END)
			{
				break;
			}

			PX_ScriptVM_DebuggerDisplayReg(Ins,u);

			break;
		}
		else if(PX_strequ(lexer.CurLexeme.buffer,"G"))
		{
			type=PX_LexerGetNextLexeme(&lexer);
			if (type!=PX_LEXER_LEXEME_TYPE_SPACER)
			{
				break;
			}

			type=PX_LexerGetNextLexeme(&lexer);
			if(PX_strIsInt(lexer.CurLexeme.buffer))
				u=PX_atoi(lexer.CurLexeme.buffer);
			else
				break;

			type=PX_LexerGetNextLexeme(&lexer);
			if (type!=PX_LEXER_LEXEME_TYPE_END)
			{
				break;
			}

			PX_ScriptVM_DebuggerDisplayVariable(Ins,u);

			break;
		}
		else if(PX_strequ(lexer.CurLexeme.buffer,"L"))
		{
			type=PX_LexerGetNextLexeme(&lexer);
			if (type!=PX_LEXER_LEXEME_TYPE_SPACER)
			{
				break;
			}

			type=PX_LexerGetNextLexeme(&lexer);
			if(PX_strIsInt(lexer.CurLexeme.buffer))
				u=PX_atoi(lexer.CurLexeme.buffer);
			else
				break;

			PX_ScriptVM_DebuggerDisplayVariable(Ins,Ins->pThread[Ins->T].BP-u);


			type=PX_LexerGetNextLexeme(&lexer);
			if (type!=PX_LEXER_LEXEME_TYPE_END)
			{
				break;
			}
			break;
		}
		else if(PX_strequ(lexer.CurLexeme.buffer,"D"))
		{
			g_scriptVM_printFunc("IP:%d  SP:%d  BP:%d\n",Ins->pThread[Ins->T].IP,Ins->pThread[Ins->T].SP,Ins->pThread[Ins->T].BP);
		}
		
	}

	PX_LexerFree(&lexer);

	return bContinue;
}
px_void PX_ScriptVM_DebuggerInterrupt(PX_ScriptVM_Instance *Ins)
{
	
	px_char cmd[PX_SCRIPTVM_DEBUGGER_CMD_LEN];
	PX_ScriptVM_InstanceThread *pT=&Ins->pThread[Ins->T];

	px_byte *pInstrs=Ins->_bin+pT->IP;

	if (!Ins->debug)
	{
		return;
	}
	if ((Ins->bp_IP==pT->IP)||Ins->bp_next)
	{
		if(Ins->bp_next)
		Ins->bp_IP=PX_FALSE;
	}
	else
	{
		return;
	}
	PX_ScriptVM_DebuggerCurrentInstruction(Ins,pT->IP);
	g_scriptVM_printFunc("\n");
	do 
	{
		g_scriptVM_cmdFunc(cmd,PX_SCRIPTVM_DEBUGGER_CMD_LEN);
		PX_strupr(cmd);
	} while (!PX_ScriptVM_DebuggerSolveCmd(Ins,cmd));
	
	

}

px_bool PX_ScriptVM_InstanceThreadSwitch(PX_ScriptVM_Instance *Ins,px_int T)
{
	if (T<Ins->maxThreadCount&&Ins->pThread[T].Activated)
	{
		Ins->T=T;
		return PX_TRUE;
	}
	return PX_FALSE;
}

px_void PX_ScriptVM_ThreadStop(PX_ScriptVM_Instance *Ins,px_int ThreadId)
{
	px_int popn;
	if (ThreadId<Ins->maxThreadCount)
	{
		if (Ins->pThread[ThreadId].Activated)
		{
			popn=Ins->VM_memsize-Ins->VM_Stacksize*ThreadId-Ins->pThread[ThreadId].SP;
			PX_ScriptVM_POPN(Ins,ThreadId,popn);
			Ins->pThread[ThreadId].Activated=PX_FALSE;
		}
	}
}


px_void PX_ScriptVM_ThreadClear(PX_ScriptVM_Instance *Ins,px_int ThreadId)
{
	px_int i,popn;
	if (ThreadId<Ins->maxThreadCount)
	{
		    //clear stack
			popn=Ins->VM_memsize-Ins->VM_Stacksize*ThreadId-Ins->pThread[ThreadId].SP;
			PX_ScriptVM_POPN(Ins,ThreadId,popn);
			//Clear registers
			for (i=0;i<PX_SCRIPTVM_REG_COUNT;i++)
			 {
			 	PX_SCRIPTVM_VaribaleFree(Ins,&Ins->pThread[ThreadId].R[i]);
			 }
			Ins->pThread[ThreadId].Activated=PX_FALSE;
	}
}

px_void PX_ScriptVM_ThreadSuspend(PX_ScriptVM_Instance *Ins,px_int ThreadId)
{
	if (ThreadId<Ins->maxThreadCount)
	{
		if (Ins->pThread[ThreadId].Activated)
		{
			Ins->pThread[ThreadId].suspend=PX_TRUE;
		}
	}
}

px_void PX_ScriptVM_ThreadResume(PX_ScriptVM_Instance *Ins,px_int ThreadId)
{
	if (ThreadId<Ins->maxThreadCount)
	{
		if (Ins->pThread[ThreadId].Activated)
		{
			Ins->pThread[ThreadId].suspend=PX_FALSE;
		}
	}
}

px_bool PX_ScriptVM_InstanceIsRuning(PX_ScriptVM_Instance* Ins)
{
	px_int i;
	for (i=0;i< Ins->maxThreadCount;i++)
	{
		if (Ins->pThread[i].Activated)
		{
			return PX_TRUE;
		}
	}
	return PX_FALSE;
}



px_void PX_ScriptVM_InstanceRun(PX_ScriptVM_Instance *Ins,px_int tick)
{
	PX_ScriptVM_InstanceRunTime(Ins, tick, 0);
}

px_void PX_ScriptVM_InstanceRunTime(PX_ScriptVM_Instance* Ins, px_int tick, px_dword elapsed)
{
	px_int i, activating = 0;
	for (i = 0; i < Ins->maxThreadCount; i++)
	{
		if (Ins->pThread[i].Activated)
		{
			activating++;
		}
	}
	if (!activating)
	{
		return;
	}

	if (tick < activating * PX_SCRIPTVM_ATOM_INSTRUCTMENTS)
	{
		tick = activating * PX_SCRIPTVM_ATOM_INSTRUCTMENTS;
	}

	while (1)
	{
		for (i = 0; i < Ins->maxThreadCount; i++)
			if (Ins->pThread[i].Activated)
			{
				if (Ins->pThread[i].sleep>elapsed)
				{
					Ins->pThread[i].sleep -= elapsed;
				}
				else
				{
					Ins->pThread[i].sleep = 0;
				}

				PX_ScriptVM_InstanceThreadSwitch(Ins, i);
				PX_ScriptVM_InstanceRunThread(Ins, PX_SCRIPTVM_ATOM_INSTRUCTMENTS);
				tick -= PX_SCRIPTVM_ATOM_INSTRUCTMENTS;
				if (tick <= 0)
				{
					return;
				}
			}
	}
}





