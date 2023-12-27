#include "PX_Script_VM.h"


static px_void PX_VM_Error(PX_VM *Ins, const px_char *log)
{
	PX_LOG(log);
}

static px_void PX_VM_LOG(const px_char *log)
{
	PX_LOG(log);
}

#define PX_SCRIPT_VM_LOG


px_bool PX_VMInitialize(PX_VM *Ins,px_memorypool *mp,const px_byte *code,px_int size)
{
	PX_SCRIPT_ASM_HEADER *header;
	PX_VM_EXPORT_FUNCTION *pfunc;
	PX_ASM_HOST_NODE *phost;
	px_int i,j;
	PX_memset(Ins, 0, sizeof(PX_VM));

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


	if ((Ins->_mem=(PX_VM_VARIABLE *)MP_Malloc(mp,sizeof(PX_VM_VARIABLE)*Ins->VM_memsize))==PX_NULL)
	{
		goto _ERROR;
	}


	if ((Ins->pThread=(PX_VM_Thread *)MP_Malloc(mp,sizeof(PX_VM_Thread)*Ins->maxThreadCount))==PX_NULL)
	{
		goto _ERROR;
	}
	PX_memset(Ins->pThread, 0, sizeof(PX_VM_Thread) * Ins->maxThreadCount);
	for (i=0;i<Ins->maxThreadCount;i++)
	{
		Ins->pThread[i].Activated=PX_FALSE;
		Ins->pThread[i].suspend=PX_FALSE;
		for (j=0;j<PX_VM_REG_COUNT;j++)
		{
			PX_memset(&Ins->pThread[i].R[j],0,sizeof(PX_VM_VARIABLE));
			Ins->pThread[i].R[j].type=PX_VM_VARIABLE_TYPE_INT;
		}
	}

	for (i=0;i<Ins->VM_memsize;i++)
	{
		PX_memset(&Ins->_mem[i],0,sizeof(PX_VM_VARIABLE));
		Ins->_mem[i].type=PX_VM_VARIABLE_TYPE_INT;
	}


	Ins->funcCount=header->funcCount;
	Ins->hostCount=header->hostCount;
	Ins->_func=PX_NULL;
	Ins->_host=PX_NULL;
	PX_memset(Ins->signal,0,sizeof(Ins->signal));

	if (Ins->funcCount!=0)
	{
		if ((Ins->_func=(PX_VM_EXPORT_FUNCTION *)MP_Malloc(mp,sizeof(PX_VM_EXPORT_FUNCTION)*Ins->funcCount))==PX_NULL)
		{
			goto _ERROR;
		}
		//read export functions
		pfunc=(PX_VM_EXPORT_FUNCTION *)(code+header->oftfunc);
		for (i=0;i<Ins->funcCount;i++)
		{
			PX_memcpy(&Ins->_func[i],&pfunc[i],sizeof(PX_VM_EXPORT_FUNCTION));
		}
	}
	
	if(Ins->hostCount!=0)
	{
		if ((Ins->_host=(PX_ASM_HOST_NODE *)MP_Malloc(mp,sizeof(PX_ASM_HOST_NODE)*Ins->hostCount))==PX_NULL)
		{
			goto _ERROR;
		}
		PX_memset(Ins->_host,0,sizeof(PX_ASM_HOST_NODE)*Ins->hostCount);
		phost=(PX_ASM_HOST_NODE *)(code+header->ofthost);
		for (i=0;i<Ins->hostCount;i++)
		{
			PX_memcpy(&Ins->_host[i],&phost[i],sizeof(PX_ASM_HOST_NODE));
			Ins->_host[i].map=PX_NULL;
		}
	}
	

	Ins->stringsize=header->stringSize;
	Ins->memorysize=header->memsize;
	Ins->binsize=header->binsize;
	Ins->Suspend=PX_FALSE;


	

	Ins->_string=(px_char *)MP_Malloc(mp,header->stringSize);
	if (header->stringSize&&!Ins->_string)
	{
		goto _ERROR;
	}
	PX_memcpy(Ins->_string,code+header->oftString,header->stringSize);

	Ins->_memory=(px_byte *)MP_Malloc(mp,header->memsize);
	if (header->memsize&&!Ins->_memory)
	{
		goto _ERROR;
	}
	PX_memcpy(Ins->_memory,code+header->oftmem,header->memsize);

	Ins->_bin=(px_byte *)MP_Malloc(mp,header->binsize);
	if (header->binsize&&!Ins->_bin)
	{
		goto _ERROR;
	}
	PX_memcpy(Ins->_bin,code+header->oftbin,header->binsize);

	
	for (i=0;i<PX_COUNTOF(Ins->breakpoints);i++)
	{
		Ins->breakpoints[i] = -1;
	}
	PX_VMRunFunction(Ins, 0, "_BOOT", 0, 0);
	return PX_TRUE;
_ERROR:

	if (Ins->_mem)
		MP_Free(mp, Ins->_mem);

	if (Ins->pThread)
		MP_Free(mp, Ins->pThread);


	if (Ins->_func)
		MP_Free(mp, Ins->_func);

	if (Ins->_host)
		MP_Free(mp, Ins->_host);

	if (Ins->_string)
		MP_Free(mp, Ins->_string);

	if (Ins->_memory)
		MP_Free(mp, Ins->_memory);

	if (Ins->_bin)
		MP_Free(mp, Ins->_bin);

	return PX_FALSE;
}

PX_VM_VARIABLE PX_VM_VaribaleCopy(PX_VM *Ins,PX_VM_VARIABLE var,px_bool *bOutOfMemory)
{
	PX_VM_VARIABLE cpyVar;
	cpyVar=var;
	*bOutOfMemory=PX_FALSE;

	if (var.type==PX_VM_VARIABLE_TYPE_STRING)
	{
		if(!PX_StringInitialize(Ins->mp,&cpyVar._string)) 
		{
			cpyVar.type=PX_VM_VARIABLE_TYPE_INT;
			*bOutOfMemory=PX_TRUE;
		}
		else
		{
			if(!PX_StringCopy(&cpyVar._string,&var._string))
			{
				cpyVar.type=PX_VM_VARIABLE_TYPE_INT;
				*bOutOfMemory=PX_TRUE;
			}
		}
	}
	else if (var.type==PX_VM_VARIABLE_TYPE_MEMORY)
	{
		PX_MemoryInitialize(Ins->mp,&cpyVar._memory);
		if(!PX_MemoryCat(&cpyVar._memory,var._memory.buffer,var._memory.usedsize))
		{
			cpyVar.type=PX_VM_VARIABLE_TYPE_INT;
			*bOutOfMemory=PX_TRUE;
		}
	}
	return cpyVar;
}

px_void PX_VM_VariableFree(PX_VM *Ins,PX_VM_VARIABLE *var)
{
	if (var->type==PX_VM_VARIABLE_TYPE_MEMORY)
	{
		PX_MemoryFree(&var->_memory);
	}
	else if (var->type==PX_VM_VARIABLE_TYPE_STRING)
	{
		PX_StringFree(&var->_string);
	}
	var->type=PX_VM_VARIABLE_TYPE_INT;
	var->_int=0;
}


static PX_VM_VARIABLE  PX_VM_GetParamConst(PX_VM *Ins,px_char optype,px_int param,px_bool *bOutofMemory)
{
	PX_VM_VARIABLE rVar,ret;
	PX_VM_Thread *pT=&Ins->pThread[Ins->T];
	*bOutofMemory=PX_FALSE;
	ret.type=PX_VM_VARIABLE_TYPE_INT;
	ret._int=0;
	switch (optype)
	{
	case PX_SCRIPT_ASM_OPTYPE_IP:
		{
			ret.type=PX_VM_VARIABLE_TYPE_INT;
			ret._int=pT->IP;
			return ret;
		}
		break;

	case PX_SCRIPT_ASM_OPTYPE_SP:
		{
			ret.type=PX_VM_VARIABLE_TYPE_INT;
			ret._int=pT->SP;
			return ret;
		}
		break;

	case PX_SCRIPT_ASM_OPTYPE_BP:
		{
			ret.type=PX_VM_VARIABLE_TYPE_INT;
			ret._int=pT->BP;
			return ret;
		}
		break;
	case PX_SCRIPT_ASM_OPTYPE_HOST:
	case PX_SCRIPT_ASM_OPTYPE_INT:
		{
			ret.type=PX_VM_VARIABLE_TYPE_INT;
			ret._int=param;
			return ret;
		}
		break;
	case PX_SCRIPT_ASM_OPTYPE_HANDLE:
		{
			ret.type = PX_VM_VARIABLE_TYPE_HANDLE;
			ret._int = param;
			return ret;
		}
	break;
	case PX_SCRIPT_ASM_OPTYPE_FLOAT:
		{
			//Must be IEEE 754 format
			ret.type=PX_VM_VARIABLE_TYPE_FLOAT;
			ret._int=param;
			return ret;
		}
		break;
	case PX_SCRIPT_ASM_OPTYPE_STRING:
		{
			ret.type=PX_VM_VARIABLE_TYPE_STRING;
			ret._string.buffer=Ins->_string+param;
			ret._string.bufferlen=0; 
			ret._string.mp=PX_NULL;
			return ret;
		}
		break;

	case PX_SCRIPT_ASM_OPTYPE_MEMORY:
		{
			ret.type=PX_VM_VARIABLE_TYPE_MEMORY;
			ret._memory.buffer=Ins->_memory+param+4;
			ret._memory.allocsize=0;
			ret._memory.usedsize=*(px_int *)(Ins->_memory+param);
			ret._memory.mp=PX_NULL;
			return ret;
		}
		break;

	case PX_SCRIPT_ASM_OPTYPE_REG:
		{
			if (param<PX_VM_REG_COUNT)
			{
				return PX_VM_VaribaleCopy(Ins,pT->R[param],bOutofMemory);
			}
			else
			{
				PX_VM_Error(Ins,"Variable parameter error.");
			}
		}
		break;
	case PX_SCRIPT_ASM_OPTYPE_LOCAL_CONST:
		{
			if(pT->BP-param>=0&&pT->BP-param<Ins->VM_memsize)
				return PX_VM_VaribaleCopy(Ins,Ins->_mem[pT->BP-param],bOutofMemory);
			else
				goto _ERROR;
		}
		break;
	case PX_SCRIPT_ASM_OPTYPE_LOCAL_REGREF:
		{
			if (param<PX_VM_REG_COUNT)
			{
				rVar=pT->R[param];
				if (rVar.type==PX_VM_VARIABLE_TYPE_INT)
				{
					if (pT->BP-rVar._int>=0&&pT->BP-rVar._int<Ins->VM_memsize)
					{
						return PX_VM_VaribaleCopy(Ins,Ins->_mem[rVar._int-pT->BP],bOutofMemory);
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
			if (param>=0&&param<Ins->VM_memsize)
			{
				rVar=Ins->_mem[param];
				if (rVar.type==PX_VM_VARIABLE_TYPE_INT)
				{
					if (pT->BP-rVar._int>=0&&pT->BP-rVar._int<Ins->VM_memsize)
					{
						return PX_VM_VaribaleCopy(Ins,Ins->_mem[pT->BP-rVar._int],bOutofMemory);
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
			if (pT->BP-param>=0&&pT->BP-param<Ins->VM_memsize)
			{
				rVar=Ins->_mem[pT->BP-param];
				if (rVar.type==PX_VM_VARIABLE_TYPE_INT)
				{
					if (pT->BP-rVar._int>=0&&pT->BP-rVar._int<Ins->VM_memsize)
					{
						return PX_VM_VaribaleCopy(Ins,Ins->_mem[pT->BP-rVar._int],bOutofMemory);
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
			if(param>=0&&param<Ins->VM_memsize)
				return PX_VM_VaribaleCopy(Ins,Ins->_mem[param],bOutofMemory);
			else
				goto _ERROR;
		}
		break;
	case PX_SCRIPT_ASM_OPTYPE_GLOBAL_SPREF:
		{
			if(pT->SP + param>=0&&pT->SP+param<Ins->VM_memsize)
				return PX_VM_VaribaleCopy(Ins,Ins->_mem[pT->SP+param],bOutofMemory);
			else
				goto _ERROR;
		}
		break;
	case PX_SCRIPT_ASM_OPTYPE_GLOBAL_REGREF:
		{
			if (param<PX_VM_REG_COUNT)
			{
				rVar=pT->R[param];
				if (rVar.type==PX_VM_VARIABLE_TYPE_INT)
				{
					if (rVar._int>=0&&rVar._int<Ins->VM_memsize)
					{
						return PX_VM_VaribaleCopy(Ins,Ins->_mem[rVar._int],bOutofMemory);
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
			if (param>=0&&param<Ins->VM_memsize)
			{
				rVar=Ins->_mem[param];
				if (rVar.type==PX_VM_VARIABLE_TYPE_INT)
				{
					if (rVar._int>=0&&rVar._int<Ins->VM_memsize)
					{
						return PX_VM_VaribaleCopy(Ins,Ins->_mem[rVar._int],bOutofMemory);
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
				if (rVar.type==PX_VM_VARIABLE_TYPE_INT)
				{
					if (rVar._int>=0&&rVar._int<Ins->VM_memsize)
					{
						return PX_VM_VaribaleCopy(Ins,Ins->_mem[rVar._int],bOutofMemory);
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
	PX_VM_Error(Ins,"Virtual Machine Runtime error");
	return ret;

}
PX_VM_VARIABLE * PX_VMGetVariablePointer(PX_VM *Ins,px_char optype,px_int param)
{
	PX_VM_VARIABLE rVar;
	PX_VM_Thread *pT=&Ins->pThread[Ins->T];
	switch (optype)
	{
	case PX_SCRIPT_ASM_OPTYPE_REG:
		{
			if (param<PX_VM_REG_COUNT)
			{
				return &pT->R[param];
			}
			else
			{
				PX_VM_Error(Ins,"Variable parameter error.");
			}
		}
		break;
	case PX_SCRIPT_ASM_OPTYPE_LOCAL_CONST:
		{
			if(pT->BP-param>=0&&pT->BP-param<Ins->VM_memsize)
			return &Ins->_mem[pT->BP-param];
			else
				goto _ERROR;
		}
		break;
	case PX_SCRIPT_ASM_OPTYPE_LOCAL_REGREF:
		{
			if (param<PX_VM_REG_COUNT)
			{
				rVar=pT->R[param];
				if (rVar.type==PX_VM_VARIABLE_TYPE_INT)
				{
					if (pT->BP-rVar._int>=0&&pT->BP-rVar._int<Ins->VM_memsize)
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
			if (param>=0&&param<Ins->VM_memsize)
			{
				rVar=Ins->_mem[param];
				if (rVar.type==PX_VM_VARIABLE_TYPE_INT)
				{
					if (pT->BP-rVar._int>=0&&pT->BP-rVar._int<Ins->VM_memsize)
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
			if (pT->BP-param>=0&&pT->BP-param<Ins->VM_memsize)
			{
				rVar=Ins->_mem[pT->BP-param];
				if (rVar.type==PX_VM_VARIABLE_TYPE_INT)
				{
					if (pT->BP-rVar._int>=0&&pT->BP-rVar._int<Ins->VM_memsize)
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
			if(param>=0&&param<Ins->VM_memsize)
				return &Ins->_mem[param];
			else
				goto _ERROR;
		}
		break;

	case PX_SCRIPT_ASM_OPTYPE_GLOBAL_SPREF:
		{
			if(pT->SP + param>=0&&pT->SP+param<Ins->VM_memsize)
				return &Ins->_mem[pT->SP+param];
			else
				goto _ERROR;
		}
		break;

	case PX_SCRIPT_ASM_OPTYPE_GLOBAL_REGREF:
		{
			if (param<PX_VM_REG_COUNT)
			{
				rVar=pT->R[param];
				if (rVar.type==PX_VM_VARIABLE_TYPE_INT)
				{
					if (rVar._int>=0&&rVar._int<Ins->VM_memsize)
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
			if (param>=0&&param<Ins->VM_memsize)
			{
				rVar=Ins->_mem[param];
				if (rVar.type==PX_VM_VARIABLE_TYPE_INT)
				{
					if (rVar._int>=0&&rVar._int<Ins->VM_memsize)
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
			if (pT->BP-param>=0&&pT->BP-param<Ins->VM_memsize)
			{
				rVar=Ins->_mem[pT->BP-param];
				if (rVar.type==PX_VM_VARIABLE_TYPE_INT)
				{
					if (rVar._int>=0&&rVar._int<Ins->VM_memsize)
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
	return PX_NULL;
	
}

#define PX_SCRIPT_VM_PARAM(i) (*(((px_int32 *)(Ins->_bin+pT->IP+4))+i))
#define PX_SCRIPT_VM_IP_PARAM(i) (*(((px_int32 *)(Ins->_bin+IP+4))+i))




PX_VM_RUNRETURN PX_VMRunThread(PX_VM *Ins,px_int tick)
{
	px_char opCode,opType[3],numOut[32],lastchar,*pchar;
	px_float p1=0,p2=0;
	PX_VM_VARIABLE *pVar,cVar,sVar,tVar;
	px_string newString;
	px_byte *newBuffer;
	px_bool bOutofMemory;
	px_int i,j;
	PX_VM_Thread *pT=&Ins->pThread[Ins->T];
	
	while(PX_TRUE)
	{

		if (pT->IP > Ins->binsize || pT->IP < 0)
		{
			PX_VM_Error(Ins, "IP point to invalid address");
			goto _ERROR;
		}

		if (pT->SP > Ins->VM_memsize || pT->SP < 0)
		{
			PX_VM_Error(Ins, "SP point to invalid address");
			goto _ERROR;
		}

		if (!pT->Activated)
		{
			return PX_VM_RUNRETURN_END;
		}

		if (Ins->Suspend||pT->suspend )
		{
			return PX_VM_RUNRETURN_SUSPEND;
		}

		if (pT->sleep)
		{
			return PX_VM_RUNRETURN_WAIT;
		}
		
		if (Ins->debug)
		{
			for (i = 0; i < PX_COUNTOF(Ins->breakpoints); i++)
			{
				if (Ins->breakpoints[i]==-1)
				{
					continue;
				}
				
				if (Ins->pThread[Ins->T].IP == Ins->breakpoints[i])
				{
					if (Ins->breakpoint_next)
					{
						Ins->breakpoint_next = PX_FALSE;
						break;
					}
					else
					{
						PX_VMSuspend(Ins);
						return PX_VM_RUNRETURN_SUSPEND;
					}
					
				}
			}
		}
		


		pVar=PX_NULL;
		cVar.type=PX_VM_VARIABLE_TYPE_INT;
		sVar.type=PX_VM_VARIABLE_TYPE_INT;
		tVar.type=PX_VM_VARIABLE_TYPE_INT;
		cVar._int=-1;
		sVar._int=-1;
		tVar._int=-1;

	
		if(tick==0)
		{
			PX_VM_VariableFree(Ins,&cVar);
			PX_VM_VariableFree(Ins,&sVar);
			PX_VM_VariableFree(Ins,&tVar);
			return PX_VM_RUNRETURN_TIMEOUT;
		}

		if(tick>0)
		{
			tick--;
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
			cVar=PX_VM_GetParamConst(Ins,opType[1],PX_SCRIPT_VM_PARAM(1),&bOutofMemory);
			if(bOutofMemory) 
			{
				PX_VM_Error(Ins,"MOV out of memory.");
				goto _ERROR;
			}
			if (opType[0]==PX_SCRIPT_ASM_OPTYPE_IP)
			{
				if (cVar.type!=PX_VM_VARIABLE_TYPE_INT)
				{
					PX_VM_Error(Ins,"MOV IP is not a integer.");
					goto _ERROR;
				}
				pT->IP=cVar._int;
				break;
			}

			if (opType[0]==PX_SCRIPT_ASM_OPTYPE_SP)
			{

				if (cVar.type!=PX_VM_VARIABLE_TYPE_INT)
				{
					PX_VM_Error(Ins,"MOV SP is not a integer.");
					goto _ERROR;
				}
				pT->SP=cVar._int;
				pT->IP+=(4+2*4);
				break;
			}

			if (opType[0]==PX_SCRIPT_ASM_OPTYPE_BP)
			{
				if (cVar.type!=PX_VM_VARIABLE_TYPE_INT)
				{
					PX_VM_Error(Ins,"MOV BP is not a integer.");
					goto _ERROR;
				}
				pT->BP=cVar._int;
				pT->IP+=(4+2*4);
				break;
			}

			pVar=PX_VMGetVariablePointer(Ins,opType[0],PX_SCRIPT_VM_PARAM(0));
			if (pVar==PX_NULL)
			{
				PX_VM_Error(Ins, "MOV crash.");
				goto _ERROR;
			}
			PX_VM_VariableFree(Ins,pVar);
			
			if (cVar.type==PX_VM_VARIABLE_TYPE_STRING)
			{
				if(!PX_StringInitialize(Ins->mp,&pVar->_string)) 
				{
					PX_VM_Error(Ins,"MOV crash.");
					goto _ERROR;
				}
				if(!PX_StringCat(&pVar->_string,cVar._string.buffer)) 
				{
					PX_VM_Error(Ins,"MOV crash.");
					goto _ERROR;
				}
				pVar->type=cVar.type;
			}
			else if (cVar.type==PX_VM_VARIABLE_TYPE_MEMORY)
			{
				PX_MemoryInitialize(Ins->mp,&pVar->_memory);
				if(!PX_MemoryAlloc(&pVar->_memory,cVar._memory.usedsize)) 
				{
					PX_VM_Error(Ins,"MOV crash.");
					goto _ERROR;
				}
				if(!PX_MemoryCat(&pVar->_memory,cVar._memory.buffer,cVar._memory.usedsize))
				{
					PX_VM_Error(Ins,"MOV crash.");
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
			cVar=PX_VM_GetParamConst(Ins,opType[1],PX_SCRIPT_VM_PARAM(1),&bOutofMemory);
			if(bOutofMemory) 
			{
				PX_VM_Error(Ins,"Add crash.");
				goto _ERROR;
			}
			if (opType[0]==PX_SCRIPT_ASM_OPTYPE_IP)
			{
				if (cVar.type!=PX_VM_VARIABLE_TYPE_INT)
				{
					PX_VM_Error(Ins,"ADD IP is not a integer.");
					goto _ERROR;
				}
				pT->IP+=cVar._int;
				break;
			}

			if (opType[0]==PX_SCRIPT_ASM_OPTYPE_SP)
			{
				if (cVar.type!=PX_VM_VARIABLE_TYPE_INT)
				{
					PX_VM_Error(Ins,"ADD SP is not a integer.");
					goto _ERROR;
				}
				pT->SP+=cVar._int;
				pT->IP+=(4+2*4);
				break;
			}

			if (opType[0]==PX_SCRIPT_ASM_OPTYPE_BP)
			{
				if (cVar.type!=PX_VM_VARIABLE_TYPE_INT)
				{
					PX_VM_Error(Ins,"ADD BP is not a integer.");
					goto _ERROR;
				}
				pT->BP+=cVar._int;
				pT->IP+=(4+2*4);
				break;
			}
			pVar=PX_VMGetVariablePointer(Ins,opType[0],PX_SCRIPT_VM_PARAM(0));

			if (pVar->type==PX_VM_VARIABLE_TYPE_INT&&cVar.type==PX_VM_VARIABLE_TYPE_INT)
			{
				pVar->_int+=cVar._int;
				pT->IP+=(4+2*4);
				break;
			}
			if (pVar->type==PX_VM_VARIABLE_TYPE_INT&&cVar.type==PX_VM_VARIABLE_TYPE_FLOAT)
			{
				pVar->_float=pVar->_int+cVar._float;
				pVar->type=PX_VM_VARIABLE_TYPE_FLOAT;
				pT->IP+=(4+2*4);
				break;
			}
			if (pVar->type==PX_VM_VARIABLE_TYPE_FLOAT&&cVar.type==PX_VM_VARIABLE_TYPE_FLOAT)
			{
				pVar->_float=pVar->_float+cVar._float;
				pT->IP+=(4+2*4);
				break;
			}
			if (pVar->type==PX_VM_VARIABLE_TYPE_FLOAT&&cVar.type==PX_VM_VARIABLE_TYPE_INT)
			{
				pVar->_float=pVar->_float+cVar._int;
				pT->IP+=(4+2*4);
				break;
			}
			PX_VM_Error(Ins,"ADD Invalid instr");
			goto _ERROR;
		}
		break;
	case PX_SCRIPT_ASM_INSTR_OPCODE_SUB:
		{
			cVar=PX_VM_GetParamConst(Ins,opType[1],PX_SCRIPT_VM_PARAM(1),&bOutofMemory);
			if(bOutofMemory) 
			{
				PX_VM_Error(Ins,"SUB out of memory.");
				goto _ERROR;
			}
			if (opType[0]==PX_SCRIPT_ASM_OPTYPE_IP)
			{
				if (cVar.type!=PX_VM_VARIABLE_TYPE_INT)
				{
					PX_VM_Error(Ins,"SUB IP is not a integer.");
					goto _ERROR;
				}
				pT->IP-=cVar._int;
				break;
			}

			if (opType[0]==PX_SCRIPT_ASM_OPTYPE_SP)
			{
				if (cVar.type!=PX_VM_VARIABLE_TYPE_INT)
				{
					PX_VM_Error(Ins,"SUB SP is not a integer.");
					goto _ERROR;
				}
				pT->SP-=cVar._int;
				if (pT->SP<0)
				{
					PX_VM_Error(Ins,"SUB SP<0.");
					goto _ERROR;
				}
				pT->IP+=(4+2*4);
				break;
			}

			if (opType[0]==PX_SCRIPT_ASM_OPTYPE_BP)
			{
				if (cVar.type!=PX_VM_VARIABLE_TYPE_INT)
				{
					PX_VM_Error(Ins,"SUB BP is not a integer.");
					goto _ERROR;
				}
				pT->BP-=cVar._int;
				pT->IP+=(4+2*4);
				break;
			}
			pVar=PX_VMGetVariablePointer(Ins,opType[0],PX_SCRIPT_VM_PARAM(0));
			if (pVar->type!=PX_VM_VARIABLE_TYPE_INT&&pVar->type!=PX_VM_VARIABLE_TYPE_FLOAT)
			{
				PX_VM_Error(Ins,"SUB parameters error.");
				goto _ERROR;
			}
			if (cVar.type!=PX_VM_VARIABLE_TYPE_INT&&cVar.type!=PX_VM_VARIABLE_TYPE_FLOAT)
			{
				PX_VM_Error(Ins,"SUB parameters error.");
				goto _ERROR;
			}
			if (pVar->type==PX_VM_VARIABLE_TYPE_INT&&cVar.type==PX_VM_VARIABLE_TYPE_INT)
			{
				pVar->_int-=cVar._int;
			}
			else if (pVar->type==PX_VM_VARIABLE_TYPE_INT&&cVar.type==PX_VM_VARIABLE_TYPE_FLOAT)
			{
				pVar->_float=pVar->_int-cVar._float;
				pVar->type=PX_VM_VARIABLE_TYPE_FLOAT;
			}
			else if (pVar->type==PX_VM_VARIABLE_TYPE_FLOAT&&cVar.type==PX_VM_VARIABLE_TYPE_FLOAT)
			{
				pVar->_float=pVar->_float-cVar._float;
			}
			else if (pVar->type==PX_VM_VARIABLE_TYPE_FLOAT&&cVar.type==PX_VM_VARIABLE_TYPE_INT)
			{
				pVar->_float=pVar->_float-cVar._int;
			}
			pT->IP+=(4+2*4);
		}
		break;
	case PX_SCRIPT_ASM_INSTR_OPCODE_DIV:
		{
			pVar=PX_VMGetVariablePointer(Ins,opType[0],PX_SCRIPT_VM_PARAM(0));
			cVar=PX_VM_GetParamConst(Ins,opType[1],PX_SCRIPT_VM_PARAM(1),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			if (cVar._int==0)
			{
				PX_VM_Error(Ins,"Divide by zero error");
				goto _ERROR;
			}
			if (pVar->type!=PX_VM_VARIABLE_TYPE_INT&&pVar->type!=PX_VM_VARIABLE_TYPE_FLOAT)
			{
				PX_VM_Error(Ins,"DIV parameters error.");
				goto _ERROR;
			}
			if (cVar.type!=PX_VM_VARIABLE_TYPE_INT&&cVar.type!=PX_VM_VARIABLE_TYPE_FLOAT)
			{
				PX_VM_Error(Ins,"DIV parameters error.");
				goto _ERROR;
			}
			if (pVar->type==PX_VM_VARIABLE_TYPE_INT&&cVar.type==PX_VM_VARIABLE_TYPE_INT)
			{
				pVar->_int/=cVar._int;
			}
			else if (pVar->type==PX_VM_VARIABLE_TYPE_INT&&cVar.type==PX_VM_VARIABLE_TYPE_FLOAT)
			{
				pVar->_float=pVar->_int/cVar._float;
				pVar->type=PX_VM_VARIABLE_TYPE_FLOAT;
			}
			else if (pVar->type==PX_VM_VARIABLE_TYPE_FLOAT&&cVar.type==PX_VM_VARIABLE_TYPE_FLOAT)
			{
				pVar->_float=pVar->_float/cVar._float;
			}
			else if (pVar->type==PX_VM_VARIABLE_TYPE_FLOAT&&cVar.type==PX_VM_VARIABLE_TYPE_INT)
			{
				pVar->_float=pVar->_float/cVar._int;
			}
			pT->IP+=(4+2*4);
		}
		break;
	case PX_SCRIPT_ASM_INSTR_OPCODE_MUL:
		{
			pVar=PX_VMGetVariablePointer(Ins,opType[0],PX_SCRIPT_VM_PARAM(0));
			cVar=PX_VM_GetParamConst(Ins,opType[1],PX_SCRIPT_VM_PARAM(1),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			if (pVar->type!=PX_VM_VARIABLE_TYPE_INT&&pVar->type!=PX_VM_VARIABLE_TYPE_FLOAT)
			{
				PX_VM_Error(Ins,"MUL parameters error.");
				goto _ERROR;
			}
			if (cVar.type!=PX_VM_VARIABLE_TYPE_INT&&cVar.type!=PX_VM_VARIABLE_TYPE_FLOAT)
			{
				PX_VM_Error(Ins,"MUL parameters error.");
				goto _ERROR;
			}
			if (pVar->type==PX_VM_VARIABLE_TYPE_INT&&cVar.type==PX_VM_VARIABLE_TYPE_INT)
			{
				pVar->_int*=cVar._int;
			}
			else if (pVar->type==PX_VM_VARIABLE_TYPE_INT&&cVar.type==PX_VM_VARIABLE_TYPE_FLOAT)
			{
				pVar->_float=pVar->_int*cVar._float;
				pVar->type=PX_VM_VARIABLE_TYPE_FLOAT;
			}
			else if (pVar->type==PX_VM_VARIABLE_TYPE_FLOAT&&cVar.type==PX_VM_VARIABLE_TYPE_FLOAT)
			{
				pVar->_float=pVar->_float*cVar._float;
			}
			else if (pVar->type==PX_VM_VARIABLE_TYPE_FLOAT&&cVar.type==PX_VM_VARIABLE_TYPE_INT)
			{
				pVar->_float=pVar->_float*cVar._int;
			}
			pT->IP+=(4+2*4);
		}
		break;
	case PX_SCRIPT_ASM_INSTR_OPCODE_MOD:
		{
			pVar=PX_VMGetVariablePointer(Ins,opType[0],PX_SCRIPT_VM_PARAM(0));
			cVar=PX_VM_GetParamConst(Ins,opType[1],PX_SCRIPT_VM_PARAM(1),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			if (pVar->type!=PX_VM_VARIABLE_TYPE_INT&&cVar.type!=PX_VM_VARIABLE_TYPE_INT)
			{
				PX_VM_Error(Ins,"MOD parameters error.");
				goto _ERROR;
			}
			if (cVar._int!=0)
			{
				pVar->_int %= cVar._int;
			}
			else
			{
				goto _ERROR;
			}
			
			pT->IP+=(4+2*4);
		}
		break;
	case PX_SCRIPT_ASM_INSTR_OPCODE_SHL:
		{
			pVar=PX_VMGetVariablePointer(Ins,opType[0],PX_SCRIPT_VM_PARAM(0));
			cVar=PX_VM_GetParamConst(Ins,opType[1],PX_SCRIPT_VM_PARAM(1),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			if (pVar->type!=PX_VM_VARIABLE_TYPE_INT&&cVar.type!=PX_VM_VARIABLE_TYPE_INT)
			{
				PX_VM_Error(Ins,"SHL parameters error.");
				goto _ERROR;
			}
			pVar->_int<<=cVar._int;
			pT->IP+=(4+2*4);
		}
		break;
	case PX_SCRIPT_ASM_INSTR_OPCODE_SHR:
		{
			pVar=PX_VMGetVariablePointer(Ins,opType[0],PX_SCRIPT_VM_PARAM(0));
			cVar=PX_VM_GetParamConst(Ins,opType[1],PX_SCRIPT_VM_PARAM(1),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			if (pVar->type!=PX_VM_VARIABLE_TYPE_INT&&cVar.type!=PX_VM_VARIABLE_TYPE_INT)
			{
				PX_VM_Error(Ins,"SHR parameters error.");
				goto _ERROR;
			}
			pVar->_int>>=cVar._int;
			pT->IP+=(4+2*4);
		}
		break;
	case PX_SCRIPT_ASM_INSTR_OPCODE_AND:
		{
			pVar=PX_VMGetVariablePointer(Ins,opType[0],PX_SCRIPT_VM_PARAM(0));
			cVar=PX_VM_GetParamConst(Ins,opType[1],PX_SCRIPT_VM_PARAM(1),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			if (pVar->type!=PX_VM_VARIABLE_TYPE_INT&&cVar.type!=PX_VM_VARIABLE_TYPE_INT)
			{
				PX_VM_Error(Ins,"AND parameters error.");
				goto _ERROR;
			}
			pVar->_int&=cVar._int;
			pT->IP+=(4+2*4);
		}
		break;
	case PX_SCRIPT_ASM_INSTR_OPCODE_ANDL:
		{
			pVar=PX_VMGetVariablePointer(Ins,opType[0],PX_SCRIPT_VM_PARAM(0));
			cVar=PX_VM_GetParamConst(Ins,opType[1],PX_SCRIPT_VM_PARAM(1),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			if (pVar->type!=PX_VM_VARIABLE_TYPE_INT&&pVar->type!=PX_VM_VARIABLE_TYPE_FLOAT)
			{
				PX_VM_Error(Ins,"ANDL parameters error.");
				goto _ERROR;
			}
			if (cVar.type!=PX_VM_VARIABLE_TYPE_INT&&cVar.type!=PX_VM_VARIABLE_TYPE_FLOAT)
			{
				PX_VM_Error(Ins,"ANDL parameters error.");
				goto _ERROR;
			}
			pVar->type=PX_VM_VARIABLE_TYPE_INT;
			pVar->_int=(pVar->_int&&cVar._int);
			pT->IP+=(4+2*4);
		}
		break;
		case PX_SCRIPT_ASM_INSTR_OPCODE_ORL:
		{
			pVar=PX_VMGetVariablePointer(Ins,opType[0],PX_SCRIPT_VM_PARAM(0));
			cVar=PX_VM_GetParamConst(Ins,opType[1],PX_SCRIPT_VM_PARAM(1),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			if (pVar->type!=PX_VM_VARIABLE_TYPE_INT&&pVar->type!=PX_VM_VARIABLE_TYPE_FLOAT)
			{
				PX_VM_Error(Ins,"ORL parameters error.");
				goto _ERROR;
			}
			if (cVar.type!=PX_VM_VARIABLE_TYPE_INT&&cVar.type!=PX_VM_VARIABLE_TYPE_FLOAT)
			{
				PX_VM_Error(Ins,"ORL parameters error.");
				goto _ERROR;
			}
			pVar->type=PX_VM_VARIABLE_TYPE_INT;
			pVar->_int=(pVar->_int||cVar._int);
			pT->IP+=(4+2*4);
		}
		break;

	case PX_SCRIPT_ASM_INSTR_OPCODE_OR:
		{
			pVar=PX_VMGetVariablePointer(Ins,opType[0],PX_SCRIPT_VM_PARAM(0));
			cVar=PX_VM_GetParamConst(Ins,opType[1],PX_SCRIPT_VM_PARAM(1),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			if (pVar->type!=PX_VM_VARIABLE_TYPE_INT&&pVar->type!=PX_VM_VARIABLE_TYPE_FLOAT)
			{
				PX_VM_Error(Ins,"OR parameters error.");
				goto _ERROR;
			}
			if (cVar.type!=PX_VM_VARIABLE_TYPE_INT&&cVar.type!=PX_VM_VARIABLE_TYPE_FLOAT)
			{
				PX_VM_Error(Ins,"OR parameters error.");
				goto _ERROR;
			}
			pVar->_int|=cVar._int;
			pVar->type=PX_VM_VARIABLE_TYPE_INT;
			pT->IP+=(4+2*4);
		}
		break;
	case PX_SCRIPT_ASM_INSTR_OPCODE_XOR:
		{
			pVar=PX_VMGetVariablePointer(Ins,opType[0],PX_SCRIPT_VM_PARAM(0));
			cVar=PX_VM_GetParamConst(Ins,opType[1],PX_SCRIPT_VM_PARAM(1),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			if (pVar->type!=PX_VM_VARIABLE_TYPE_INT&&pVar->type!=PX_VM_VARIABLE_TYPE_FLOAT)
			{
				PX_VM_Error(Ins,"XOR parameters error.");
				goto _ERROR;
			}
			if (cVar.type!=PX_VM_VARIABLE_TYPE_INT&&cVar.type!=PX_VM_VARIABLE_TYPE_FLOAT)
			{
				PX_VM_Error(Ins,"XOR parameters error.");
				goto _ERROR;
			}
			pVar->_int^=cVar._int;
			pVar->type=PX_VM_VARIABLE_TYPE_INT;
			pT->IP+=(4+2*4);
		}
		break;
	case PX_SCRIPT_ASM_INSTR_OPCODE_POW:
		{
			pVar=PX_VMGetVariablePointer(Ins,opType[0],PX_SCRIPT_VM_PARAM(0));
			cVar=PX_VM_GetParamConst(Ins,opType[1],PX_SCRIPT_VM_PARAM(1),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			if (pVar->type!=PX_VM_VARIABLE_TYPE_INT&&pVar->type!=PX_VM_VARIABLE_TYPE_FLOAT)
			{
				PX_VM_Error(Ins,"POW parameters error.");
				goto _ERROR;
			}
			if (cVar.type!=PX_VM_VARIABLE_TYPE_INT&&cVar.type!=PX_VM_VARIABLE_TYPE_FLOAT)
			{
				PX_VM_Error(Ins,"POW parameters error.");
				goto _ERROR;
			}

			if (pVar->type==PX_VM_VARIABLE_TYPE_INT&&cVar.type==PX_VM_VARIABLE_TYPE_INT)
			{
				pVar->_int=PX_pow_ii(pVar->_int,cVar._int);
			}
			else if (pVar->type==PX_VM_VARIABLE_TYPE_INT&&cVar.type==PX_VM_VARIABLE_TYPE_FLOAT)
			{
				pVar->_float=(px_float)PX_pow(pVar->_int,cVar._float);
				pVar->type=PX_VM_VARIABLE_TYPE_FLOAT;
			}
			else if (pVar->type==PX_VM_VARIABLE_TYPE_FLOAT&&cVar.type==PX_VM_VARIABLE_TYPE_FLOAT)
			{
				pVar->_float=(px_float)PX_pow(pVar->_float,cVar._float);
			}
			else if (pVar->type==PX_VM_VARIABLE_TYPE_FLOAT&&cVar.type==PX_VM_VARIABLE_TYPE_INT)
			{
				pVar->_float=(px_float)PX_pow(pVar->_float,cVar._int);
			}
			pT->IP+=(4+2*4);
		}
		break;
	case PX_SCRIPT_ASM_INSTR_OPCODE_SIN:
		{
			pVar=PX_VMGetVariablePointer(Ins,opType[0],PX_SCRIPT_VM_PARAM(0));
			cVar=PX_VM_GetParamConst(Ins,opType[1],PX_SCRIPT_VM_PARAM(1),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			if (cVar.type!=PX_VM_VARIABLE_TYPE_FLOAT&&cVar.type!=PX_VM_VARIABLE_TYPE_INT)
			{
				goto _ERROR;
			}
			
			if (pVar->type==PX_VM_VARIABLE_TYPE_STRING)
			{
				PX_StringFree(&pVar->_string);
			}

			if (pVar->type==PX_VM_VARIABLE_TYPE_MEMORY)
			{
				PX_MemoryFree(&pVar->_memory);
			}

			pVar->type=PX_VM_VARIABLE_TYPE_FLOAT;
			if (cVar.type==PX_VM_VARIABLE_TYPE_FLOAT)
			{
				pVar->_float=PX_sin_angle(cVar._float);
			}
			if (cVar.type==PX_VM_VARIABLE_TYPE_INT)
			{
				pVar->_float=PX_sin_angle((px_float)cVar._int);
			}
			pT->IP+=(4+2*4);
		}
		break;
	case PX_SCRIPT_ASM_INSTR_OPCODE_COS:
		{
			pVar=PX_VMGetVariablePointer(Ins,opType[0],PX_SCRIPT_VM_PARAM(0));
			cVar=PX_VM_GetParamConst(Ins,opType[1],PX_SCRIPT_VM_PARAM(1),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			if (cVar.type!=PX_VM_VARIABLE_TYPE_FLOAT&&cVar.type!=PX_VM_VARIABLE_TYPE_INT)
			{
				goto _ERROR;
			}

			if (pVar->type==PX_VM_VARIABLE_TYPE_STRING)
			{
				PX_StringFree(&pVar->_string);
			}

			if (pVar->type==PX_VM_VARIABLE_TYPE_MEMORY)
			{
				PX_MemoryFree(&pVar->_memory);
			}
			pVar->type=PX_VM_VARIABLE_TYPE_FLOAT;
			if (cVar.type==PX_VM_VARIABLE_TYPE_FLOAT)
			{
				pVar->_float=PX_cos_angle(cVar._float);
			}
			if (cVar.type==PX_VM_VARIABLE_TYPE_INT)
			{
				pVar->_float=PX_cos_angle((px_float)cVar._int);
			}
			pT->IP+=(4+2*4);
		}
		break;
	case PX_SCRIPT_ASM_INSTR_OPCODE_INT:
		{
			pVar=PX_VMGetVariablePointer(Ins,opType[0],PX_SCRIPT_VM_PARAM(0));
			if (pVar->type!=PX_VM_VARIABLE_TYPE_FLOAT&&pVar->type!=PX_VM_VARIABLE_TYPE_INT)
			{
				PX_VM_Error(Ins,"INT transform error.");
				goto _ERROR;
			}
		
			if (pVar->type==PX_VM_VARIABLE_TYPE_FLOAT)
			{
				pVar->_int=(px_int)(pVar->_float);
				pVar->type=PX_VM_VARIABLE_TYPE_INT;
			}
			
			pT->IP+=(4+1*4);
		}
		break;
	case PX_SCRIPT_ASM_INSTR_OPCODE_FLT:
		{
			pVar=PX_VMGetVariablePointer(Ins,opType[0],PX_SCRIPT_VM_PARAM(0));
			if (pVar->type!=PX_VM_VARIABLE_TYPE_FLOAT&&pVar->type!=PX_VM_VARIABLE_TYPE_INT)
			{
				PX_VM_Error(Ins,"FLT transform error.");
				goto _ERROR;
			}

			if (pVar->type==PX_VM_VARIABLE_TYPE_INT)
			{
				pVar->_float=(px_float)(pVar->_int);
				pVar->type=PX_VM_VARIABLE_TYPE_FLOAT;
			}

			pT->IP+=(4+1*4);
		}
		break;
	case PX_SCRIPT_ASM_INSTR_OPCODE_STRLEN:
		{
			pVar=PX_VMGetVariablePointer(Ins,opType[0],PX_SCRIPT_VM_PARAM(0));
			cVar=PX_VM_GetParamConst(Ins,opType[1],PX_SCRIPT_VM_PARAM(1),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			if (cVar.type!=PX_VM_VARIABLE_TYPE_STRING)
			{
				PX_VM_Error(Ins,"strlen parameters error.");
				goto _ERROR;
			}
			PX_VM_VariableFree(Ins,pVar);
			pVar->type=PX_VM_VARIABLE_TYPE_INT;
			pVar->_int=PX_strlen(cVar._string.buffer);
			pT->IP+=(4+2*4);
		}
		break;
	case PX_SCRIPT_ASM_INSTR_OPCODE_STRCAT:
		{
			pVar=PX_VMGetVariablePointer(Ins,opType[0],PX_SCRIPT_VM_PARAM(0));
			cVar=PX_VM_GetParamConst(Ins,opType[1],PX_SCRIPT_VM_PARAM(1),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			if (pVar->type!=PX_VM_VARIABLE_TYPE_STRING)
			{
				PX_VM_Error(Ins,"strcat parameters error.");
				goto _ERROR;
			}
			if (cVar.type==PX_VM_VARIABLE_TYPE_STRING)
			{
				if(!PX_StringCat(&pVar->_string,cVar._string.buffer)) goto _ERROR;
			}
			else if (cVar.type==PX_VM_VARIABLE_TYPE_INT)
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
			pVar=PX_VMGetVariablePointer(Ins,opType[0],PX_SCRIPT_VM_PARAM(0));
			cVar=PX_VM_GetParamConst(Ins,opType[1],PX_SCRIPT_VM_PARAM(1),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			sVar=PX_VM_GetParamConst(Ins,opType[2],PX_SCRIPT_VM_PARAM(2),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			if (pVar->type!=PX_VM_VARIABLE_TYPE_STRING)
			{
				PX_VM_Error(Ins,"strrep parameters error.");
				goto _ERROR;
			}
			if (cVar.type!=PX_VM_VARIABLE_TYPE_STRING)
			{
				PX_VM_Error(Ins,"strrep parameters error.");
				goto _ERROR;
			}
			if (sVar.type!=PX_VM_VARIABLE_TYPE_STRING)
			{
				PX_VM_Error(Ins,"strrep parameters error.");
				goto _ERROR;
			}
			PX_StringReplace(&pVar->_string,cVar._string.buffer,sVar._string.buffer);
			pT->IP+=(4+3*4);
		}
		break;
	case PX_SCRIPT_ASM_INSTR_OPCODE_STRCHR:
		{
			pVar=PX_VMGetVariablePointer(Ins,opType[0],PX_SCRIPT_VM_PARAM(0));
			cVar=PX_VM_GetParamConst(Ins,opType[1],PX_SCRIPT_VM_PARAM(1),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			sVar=PX_VM_GetParamConst(Ins,opType[2],PX_SCRIPT_VM_PARAM(2),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			PX_VM_VariableFree(Ins,pVar);

			if (cVar.type!=PX_VM_VARIABLE_TYPE_STRING)
			{
				PX_VM_Error(Ins,"strchr parameters error.");
				goto _ERROR;
			}
			if (sVar.type!=PX_VM_VARIABLE_TYPE_INT)
			{
				PX_VM_Error(Ins,"strchr parameters error.");
				goto _ERROR;
			}

			pVar->type=PX_VM_VARIABLE_TYPE_INT;

			if(sVar._int<PX_strlen(cVar._string.buffer))
				pVar->_int=(px_uchar)cVar._string.buffer[sVar._int];
			pT->IP+=(4+3*4);
		}
		break;

	case PX_SCRIPT_ASM_INSTR_OPCODE_STRTOI:
		{
			pVar=PX_VMGetVariablePointer(Ins,opType[0],PX_SCRIPT_VM_PARAM(0));
			cVar=PX_VM_GetParamConst(Ins,opType[1],PX_SCRIPT_VM_PARAM(1),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			PX_VM_VariableFree(Ins,pVar);

			if (cVar.type!=PX_VM_VARIABLE_TYPE_STRING)
			{
				PX_VM_Error(Ins,"strtoi parameters error.");
				goto _ERROR;
			}

			pVar->type=PX_VM_VARIABLE_TYPE_INT;
			pVar->_int=PX_atoi(cVar._string.buffer);
			pT->IP+=(4+2*4);
		}
		break;
	case PX_SCRIPT_ASM_INSTR_OPCODE_STRTOF:
		{
			pVar=PX_VMGetVariablePointer(Ins,opType[0],PX_SCRIPT_VM_PARAM(0));
			cVar=PX_VM_GetParamConst(Ins,opType[1],PX_SCRIPT_VM_PARAM(1),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			PX_VM_VariableFree(Ins,pVar);

			if (cVar.type!=PX_VM_VARIABLE_TYPE_STRING)
			{
				PX_VM_Error(Ins,"strtof parameters error.");
				goto _ERROR;
			}

			pVar->type=PX_VM_VARIABLE_TYPE_FLOAT;
			pVar->_int=(px_int)PX_atof(cVar._string.buffer);
			pT->IP+=(4+2*4);
		}
		break;

	case PX_SCRIPT_ASM_INSTR_OPCODE_STRFRI:
		{
			pVar=PX_VMGetVariablePointer(Ins,opType[0],PX_SCRIPT_VM_PARAM(0));
			cVar=PX_VM_GetParamConst(Ins,opType[1],PX_SCRIPT_VM_PARAM(1),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			PX_VM_VariableFree(Ins,pVar);
			if (cVar.type!=PX_VM_VARIABLE_TYPE_INT&&cVar.type!=PX_VM_VARIABLE_TYPE_FLOAT)
			{
				PX_VM_Error(Ins,"strfri parameters error.");
				goto _ERROR;
			}

			pVar->type=PX_VM_VARIABLE_TYPE_STRING;
			PX_StringInitialize(Ins->mp,&pVar->_string);
			if(cVar.type==PX_VM_VARIABLE_TYPE_INT)
			PX_itoa(cVar._int,numOut,sizeof(numOut),10);
			else
			PX_itoa((px_int)cVar._float,numOut,sizeof(numOut),10);
			
			if(!PX_StringCat(&pVar->_string,numOut)) goto _ERROR;
			pT->IP+=(4+2*4);
		}
		break;

	case PX_SCRIPT_ASM_INSTR_OPCODE_STRFRF:
		{
			pVar=PX_VMGetVariablePointer(Ins,opType[0],PX_SCRIPT_VM_PARAM(0));
			cVar=PX_VM_GetParamConst(Ins,opType[1],PX_SCRIPT_VM_PARAM(1),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			PX_VM_VariableFree(Ins,pVar);

			if (cVar.type!=PX_VM_VARIABLE_TYPE_INT&&cVar.type!=PX_VM_VARIABLE_TYPE_FLOAT)
			{
				PX_VM_Error(Ins,"strfrf parameters error.");
				goto _ERROR;
			}

			pVar->type=PX_VM_VARIABLE_TYPE_STRING;
			PX_StringInitialize(Ins->mp,&pVar->_string);
			numOut[0]=0;
			if(cVar.type==PX_VM_VARIABLE_TYPE_INT)
				PX_ftoa((px_float)cVar._int,numOut,sizeof(numOut),PX_VM_DEFALUT_FLOAT_PRECCISION);
			else
				PX_ftoa(cVar._float,numOut,sizeof(numOut),PX_VM_DEFALUT_FLOAT_PRECCISION);

			PX_StringCat(&pVar->_string,numOut);
			pT->IP+=(4+2*4);
		}
		break;
	case PX_SCRIPT_ASM_INSTR_OPCODE_STRTMEM:
		{
			pVar=PX_VMGetVariablePointer(Ins,opType[0],PX_SCRIPT_VM_PARAM(0));
			if (pVar->type==PX_VM_VARIABLE_TYPE_STRING)
			{
				PX_MemoryInitialize(Ins->mp,&cVar._memory);
				if(!PX_MemoryCat(&cVar._memory,pVar->_string.buffer,pVar->_string.bufferlen)) goto _ERROR;
				PX_StringFree(&pVar->_string);
				pVar->type=PX_VM_VARIABLE_TYPE_MEMORY;
				pVar->_memory=cVar._memory;
			}
			else
			{
				PX_VM_Error(Ins,"strtmem parameters error.");
				goto _ERROR;
			}
			pT->IP+=(4+1*4);
		}
		break;

	case PX_SCRIPT_ASM_INSTR_OPCODE_STRSET:
		{
			pVar=PX_VMGetVariablePointer(Ins,opType[0],PX_SCRIPT_VM_PARAM(0));
			sVar=PX_VM_GetParamConst(Ins,opType[1],PX_SCRIPT_VM_PARAM(1),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			tVar=PX_VM_GetParamConst(Ins,opType[2],PX_SCRIPT_VM_PARAM(2),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			if (pVar->type!=PX_VM_VARIABLE_TYPE_STRING)
			{
					if(pVar->type!=PX_VM_VARIABLE_TYPE_INT)
					{
						PX_VM_Error(Ins,"strset parameters error.");
						goto _ERROR;
					}
					else
					{
						pVar->type=PX_VM_VARIABLE_TYPE_STRING;
						PX_StringInitialize(Ins->mp,&pVar->_string);
					}
			}


			if (sVar.type!=PX_VM_VARIABLE_TYPE_INT||(tVar.type!=PX_VM_VARIABLE_TYPE_INT))
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

			if(tVar.type==PX_VM_VARIABLE_TYPE_INT)
				pVar->_string.buffer[sVar._int]=(px_char)tVar._int;
			else 
				goto _ERROR;

			pT->IP+=(4+3*4);
		}
		break;

	case PX_SCRIPT_ASM_INSTR_OPCODE_STRFIND:
		{
			pVar=PX_VMGetVariablePointer(Ins,opType[0],PX_SCRIPT_VM_PARAM(0));
			sVar=PX_VM_GetParamConst(Ins,opType[1],PX_SCRIPT_VM_PARAM(1),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			tVar=PX_VM_GetParamConst(Ins,opType[2],PX_SCRIPT_VM_PARAM(2),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			PX_VM_VariableFree(Ins,pVar);
			if (sVar.type!=PX_VM_VARIABLE_TYPE_STRING||tVar.type!=PX_VM_VARIABLE_TYPE_STRING)
			{
				PX_VM_Error(Ins,"strfind parameters error.");
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

	case PX_SCRIPT_ASM_INSTR_OPCODE_STRCUT:
	{
		pVar = PX_VMGetVariablePointer(Ins, opType[0], PX_SCRIPT_VM_PARAM(0));
		sVar = PX_VM_GetParamConst(Ins, opType[1], PX_SCRIPT_VM_PARAM(1), &bOutofMemory);
		if (bOutofMemory) goto _ERROR;
		tVar = PX_VM_GetParamConst(Ins, opType[2], PX_SCRIPT_VM_PARAM(2), &bOutofMemory);
		if (bOutofMemory) goto _ERROR;
	
		if (pVar->type!= PX_VM_VARIABLE_TYPE_STRING||sVar.type != PX_VM_VARIABLE_TYPE_INT || tVar.type != PX_VM_VARIABLE_TYPE_INT)
		{
			PX_VM_Error(Ins, "strcut parameters error.");
			goto _ERROR;
		}
		
		PX_strcut(pVar->_string.buffer,sVar._int, tVar._int);
		PX_StringUpdateExReg(&pVar->_string);
		pT->IP += (4 + 3 * 4);
	}
	break;

	case PX_SCRIPT_ASM_INSTR_OPCODE_ASC:
		{
			pVar=PX_VMGetVariablePointer(Ins,opType[0],PX_SCRIPT_VM_PARAM(0));
			cVar=PX_VM_GetParamConst(Ins,opType[1],PX_SCRIPT_VM_PARAM(1),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			PX_VM_VariableFree(Ins,pVar);
			if (cVar.type!=PX_VM_VARIABLE_TYPE_STRING)
			{
				PX_VM_Error(Ins,"asc parameters error.");
				goto _ERROR;
			}

			pVar->type=PX_VM_VARIABLE_TYPE_INT;
			pVar->_int=cVar._string.buffer[0];
			pT->IP+=(4+2*4);
		}
		break;

	case PX_SCRIPT_ASM_INSTR_OPCODE_MEMBYTE:
		{
			pVar=PX_VMGetVariablePointer(Ins,opType[0],PX_SCRIPT_VM_PARAM(0));
			sVar=PX_VM_GetParamConst(Ins,opType[1],PX_SCRIPT_VM_PARAM(1),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			tVar=PX_VM_GetParamConst(Ins,opType[2],PX_SCRIPT_VM_PARAM(2),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			PX_VM_VariableFree(Ins,pVar);

			if (sVar.type!=PX_VM_VARIABLE_TYPE_MEMORY||tVar.type!=PX_VM_VARIABLE_TYPE_INT)
			{
				PX_VM_Error(Ins,"membyte parameters error.");
				goto _ERROR;
			}
			pVar->type=PX_VM_VARIABLE_TYPE_INT;
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
			pVar=PX_VMGetVariablePointer(Ins,opType[0],PX_SCRIPT_VM_PARAM(0));
			sVar=PX_VM_GetParamConst(Ins,opType[1],PX_SCRIPT_VM_PARAM(1),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			tVar=PX_VM_GetParamConst(Ins,opType[2],PX_SCRIPT_VM_PARAM(2),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			
			if (pVar->type!=PX_VM_VARIABLE_TYPE_MEMORY)
			{
				if(pVar->type!=PX_VM_VARIABLE_TYPE_INT)
				{
					PX_VM_Error(Ins,"memset parameters error.");
					goto _ERROR;
				}
				else
				{
					pVar->type=PX_VM_VARIABLE_TYPE_MEMORY;
					PX_MemoryInitialize(Ins->mp,&pVar->_memory);
				}
			}


			if (sVar.type!=PX_VM_VARIABLE_TYPE_INT||tVar.type!=PX_VM_VARIABLE_TYPE_INT)
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
			pVar=PX_VMGetVariablePointer(Ins,opType[0],PX_SCRIPT_VM_PARAM(0));
			sVar=PX_VM_GetParamConst(Ins,opType[1],PX_SCRIPT_VM_PARAM(1),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			tVar=PX_VM_GetParamConst(Ins,opType[2],PX_SCRIPT_VM_PARAM(2),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;

			if (pVar->type!=PX_VM_VARIABLE_TYPE_MEMORY)
			{
				PX_VM_Error(Ins,"memtrm parameters error.");
				goto _ERROR;
			}


			if (sVar.type!=PX_VM_VARIABLE_TYPE_INT||tVar.type!=PX_VM_VARIABLE_TYPE_INT)
			{
				PX_VM_Error(Ins,"memtrm parameters error.");
				goto _ERROR;
			}

			if (sVar._int+tVar._int>pVar->_memory.usedsize)
			{
				PX_VM_Error(Ins,"memtrm out of range.");
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
			pVar=PX_VMGetVariablePointer(Ins,opType[0],PX_SCRIPT_VM_PARAM(0));
			sVar=PX_VM_GetParamConst(Ins,opType[1],PX_SCRIPT_VM_PARAM(1),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			tVar=PX_VM_GetParamConst(Ins,opType[2],PX_SCRIPT_VM_PARAM(2),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			PX_VM_VariableFree(Ins,pVar);
			if (sVar.type!=PX_VM_VARIABLE_TYPE_MEMORY||tVar.type!=PX_VM_VARIABLE_TYPE_MEMORY)
			{
				PX_VM_Error(Ins,"memfind parameters error.");
				goto _ERROR;
			}
			pVar->_int=-1;
			for(i=0;i<sVar._memory.usedsize-tVar._memory.usedsize+1;i++)
			{
				if (PX_memequ(sVar._memory.buffer+i,tVar._memory.buffer,tVar._memory.usedsize))
				{
					pVar->_int=i;
					pVar->type=PX_VM_VARIABLE_TYPE_INT;
					break;
				}
			}
			pT->IP+=(4+3*4);
		}
		break;

	case PX_SCRIPT_ASM_INSTR_OPCODE_MEMLEN:
		{
			pVar=PX_VMGetVariablePointer(Ins,opType[0],PX_SCRIPT_VM_PARAM(0));
			sVar=PX_VM_GetParamConst(Ins,opType[1],PX_SCRIPT_VM_PARAM(1),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			PX_VM_VariableFree(Ins,pVar);
			pVar->type=PX_VM_VARIABLE_TYPE_INT;
			pVar->_int=sVar._memory.usedsize;
			
			pT->IP+=(4+2*4);
		}
		break;

	case PX_SCRIPT_ASM_INSTR_OPCODE_MEMCAT:
		{
			pVar=PX_VMGetVariablePointer(Ins,opType[0],PX_SCRIPT_VM_PARAM(0));
			sVar=PX_VM_GetParamConst(Ins,opType[1],PX_SCRIPT_VM_PARAM(1),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			if (pVar->type!=PX_VM_VARIABLE_TYPE_MEMORY||sVar.type!=PX_VM_VARIABLE_TYPE_MEMORY)
			{
				if (pVar->type!=PX_VM_VARIABLE_TYPE_MEMORY||sVar.type!=PX_VM_VARIABLE_TYPE_INT)
				{
					PX_VM_Error(Ins,"memcat crash.");
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
			pVar=PX_VMGetVariablePointer(Ins,opType[0],PX_SCRIPT_VM_PARAM(0));
			if (pVar->type==PX_VM_VARIABLE_TYPE_MEMORY)
			{
				if (pVar->_memory.usedsize==0)
				{
					PX_StringInitialize(Ins->mp,&cVar._string);
					PX_MemoryFree(&pVar->_memory);
					pVar->type=PX_VM_VARIABLE_TYPE_STRING;
					pVar->_string=cVar._string;
				}
				else
				{
					lastchar=pVar->_memory.buffer[pVar->_memory.usedsize-1];
					
					pVar->_memory.buffer[pVar->_memory.usedsize-1]=0;
					
					PX_StringInitialize(Ins->mp,&cVar._string);
					
					if(!PX_StringCat(&cVar._string,(px_char *)pVar->_memory.buffer)) {
						PX_VM_Error(Ins,"memstr crash.");
						goto _ERROR;
					}
					if(PX_strlen((px_char *)pVar->_memory.buffer)==pVar->_memory.usedsize-1)
					PX_StringCatChar(&cVar._string,lastchar);

					PX_MemoryFree(&pVar->_memory);
					pVar->type=PX_VM_VARIABLE_TYPE_STRING;
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
			cVar=PX_VM_GetParamConst(Ins,opType[0],PX_SCRIPT_VM_PARAM(0),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			sVar=PX_VM_GetParamConst(Ins,opType[1],PX_SCRIPT_VM_PARAM(1),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			tVar=PX_VM_GetParamConst(Ins,opType[2],PX_SCRIPT_VM_PARAM(2),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;

			if (cVar.type!=PX_VM_VARIABLE_TYPE_INT||sVar.type!=PX_VM_VARIABLE_TYPE_INT||tVar.type!=PX_VM_VARIABLE_TYPE_INT)
			{
				PX_VM_Error(Ins,"datacpy parameters error");
				goto _ERROR;
			}
			if (cVar._int+tVar._int>Ins->VM_memsize||cVar._int<0||sVar._int+tVar._int>Ins->VM_memsize||sVar._int<0)
			{
				PX_VM_Error(Ins,"Out of memory access");
				goto _ERROR;
			}
			PX_memcpy(&Ins->_mem[cVar._int],&Ins->_mem[sVar._int],sizeof(PX_VM_VARIABLE)*tVar._int);

			pT->IP+=(4+3*4);
		}
		break;
	case PX_SCRIPT_ASM_INSTR_OPCODE_NEG:
		{
			pVar=PX_VMGetVariablePointer(Ins,opType[0],PX_SCRIPT_VM_PARAM(0));

			if (pVar->type==PX_VM_VARIABLE_TYPE_INT)
			{
				pVar->_int=-pVar->_int;
			}
			else if(pVar->type==PX_VM_VARIABLE_TYPE_FLOAT)
			{
				pVar->_float=-pVar->_float;
			}
			else
			{
				PX_VM_Error(Ins,"neg parameters error");
				goto _ERROR;
			}

			pT->IP+=(4+1*4);
		}
		break;
	case PX_SCRIPT_ASM_INSTR_OPCODE_INV:
		{
			pVar=PX_VMGetVariablePointer(Ins,opType[0],PX_SCRIPT_VM_PARAM(0));

			if (pVar->type!=PX_VM_VARIABLE_TYPE_INT)
			{
				PX_VM_Error(Ins,"inv parameters error");
				goto _ERROR;
			}

			pVar->_int=~pVar->_int;

			pT->IP+=(4+1*4);
		}
		break;

	case PX_SCRIPT_ASM_INSTR_OPCODE_NOT:
		{
			pVar=PX_VMGetVariablePointer(Ins,opType[0],PX_SCRIPT_VM_PARAM(0));

			if (pVar->type!=PX_VM_VARIABLE_TYPE_INT)
			{
				PX_VM_Error(Ins,"not parameters error");
				goto _ERROR;
			}

			pVar->_int=!pVar->_int;

			pT->IP+=(4+1*4);
		}
		break;

	case PX_SCRIPT_ASM_INSTR_OPCODE_JMP:
		{
			cVar=PX_VM_GetParamConst(Ins,opType[0],PX_SCRIPT_VM_PARAM(0),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			if (cVar.type!=PX_VM_VARIABLE_TYPE_INT)
			{
				PX_VM_Error(Ins,"JMP parameters error");
				goto _ERROR;
			}

			if (cVar._int<0||cVar._int>Ins->binsize)
			{
				PX_VM_Error(Ins,"JMP jump to invalid address");
				goto _ERROR;
			}

			pT->IP=cVar._int;
		}
		break;

	case PX_SCRIPT_ASM_INSTR_OPCODE_JE:
		{
			cVar=PX_VM_GetParamConst(Ins,opType[0],PX_SCRIPT_VM_PARAM(0),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			sVar=PX_VM_GetParamConst(Ins,opType[1],PX_SCRIPT_VM_PARAM(1),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			tVar=PX_VM_GetParamConst(Ins,opType[2],PX_SCRIPT_VM_PARAM(2),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;

			if (tVar.type!=PX_VM_VARIABLE_TYPE_INT)
			{
				goto _ERROR;
			}

			if (cVar.type==sVar.type)
			{
				if (cVar.type!=PX_VM_VARIABLE_TYPE_STRING&&cVar.type!=PX_VM_VARIABLE_TYPE_MEMORY)
				{
					if (cVar._int==sVar._int)
					{
						if (tVar._int<0||tVar._int>Ins->binsize)
						{
							PX_VM_Error(Ins,"JE jump to invalid address");
							goto _ERROR;
						}
						pT->IP=tVar._int;
						break;
					}
				}
				else
				{
					if(cVar.type==PX_VM_VARIABLE_TYPE_STRING)
						if (PX_strequ(cVar._string.buffer,sVar._string.buffer))
						{
							if (tVar._int<0||tVar._int>Ins->binsize)
							{
								PX_VM_Error(Ins,"JE jump to invalid address");
								goto _ERROR;
							}
							pT->IP=tVar._int;
							break;
						}

					if(cVar.type==PX_VM_VARIABLE_TYPE_MEMORY)
						if (PX_memequ(cVar._memory.buffer,sVar._memory.buffer,cVar._memory.usedsize))
						{
							if (tVar._int<0||tVar._int>Ins->binsize)
							{
								PX_VM_Error(Ins,"JE jump to invalid address");
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
			cVar=PX_VM_GetParamConst(Ins,opType[0],PX_SCRIPT_VM_PARAM(0),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			sVar=PX_VM_GetParamConst(Ins,opType[1],PX_SCRIPT_VM_PARAM(1),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			tVar=PX_VM_GetParamConst(Ins,opType[2],PX_SCRIPT_VM_PARAM(2),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			if (tVar.type!=PX_VM_VARIABLE_TYPE_INT)
			{
				goto _ERROR;
			}

			if (cVar.type==sVar.type)
			{
				if (cVar.type!=PX_VM_VARIABLE_TYPE_STRING&&cVar.type!=PX_VM_VARIABLE_TYPE_MEMORY)
				{
					if (cVar._int!=sVar._int)
					{
						if (tVar._int<0||tVar._int>Ins->binsize)
						{
							PX_VM_Error(Ins,"JNE jump to invalid address");
							goto _ERROR;
						}
						pT->IP=tVar._int;
						break;
					}
				}
				else
				{
						if(cVar.type==PX_VM_VARIABLE_TYPE_STRING)
							if (!PX_strequ(cVar._string.buffer,sVar._string.buffer))
							{
								if (tVar._int<0||tVar._int>Ins->binsize)
								{
									PX_VM_Error(Ins,"JNE jump to invalid address");
									goto _ERROR;
								}
								pT->IP=tVar._int;
								break;
							}

						if(cVar.type==PX_VM_VARIABLE_TYPE_MEMORY)
							if (!PX_memequ(cVar._memory.buffer,sVar._memory.buffer,cVar._memory.usedsize))
							{
								if (tVar._int<0||tVar._int>Ins->binsize)
								{
									PX_VM_Error(Ins,"JNE jump to invalid address");
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
			cVar=PX_VM_GetParamConst(Ins,opType[0],PX_SCRIPT_VM_PARAM(0),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			sVar=PX_VM_GetParamConst(Ins,opType[1],PX_SCRIPT_VM_PARAM(1),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			tVar=PX_VM_GetParamConst(Ins,opType[2],PX_SCRIPT_VM_PARAM(2),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;

			if (tVar.type!=PX_VM_VARIABLE_TYPE_INT)
			{
				PX_VM_Error(Ins,"JL parameters error");
				goto _ERROR;
			}
			if (cVar.type==PX_VM_VARIABLE_TYPE_STRING||sVar.type==PX_VM_VARIABLE_TYPE_STRING)
			{
				PX_VM_Error(Ins,"JL parameters error");
				goto _ERROR;
			}
			if (cVar.type==PX_VM_VARIABLE_TYPE_MEMORY||sVar.type==PX_VM_VARIABLE_TYPE_MEMORY)
			{
				PX_VM_Error(Ins,"JL parameters error");
				goto _ERROR;
			}
			if (cVar.type==PX_VM_VARIABLE_TYPE_INT)
			{
				p1=(px_float)cVar._int;
			}
			if (cVar.type==PX_VM_VARIABLE_TYPE_FLOAT)
			{
				p1=cVar._float;
			}
			if (sVar.type==PX_VM_VARIABLE_TYPE_INT)
			{
				p2=(px_float)sVar._int;
			}
			if (sVar.type==PX_VM_VARIABLE_TYPE_FLOAT)
			{
				p2=sVar._float;
			}

			if (p1<p2)
			{
				if (tVar._int<0||tVar._int>Ins->binsize)
				{
					PX_VM_Error(Ins,"JL jump to invalid address");
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
			cVar=PX_VM_GetParamConst(Ins,opType[0],PX_SCRIPT_VM_PARAM(0),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			sVar=PX_VM_GetParamConst(Ins,opType[1],PX_SCRIPT_VM_PARAM(1),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			tVar=PX_VM_GetParamConst(Ins,opType[2],PX_SCRIPT_VM_PARAM(2),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;

			if (tVar.type!=PX_VM_VARIABLE_TYPE_INT)
			{
				PX_VM_Error(Ins,"JLE parameters error");
				goto _ERROR;
			}
			if (cVar.type==PX_VM_VARIABLE_TYPE_STRING||sVar.type==PX_VM_VARIABLE_TYPE_STRING)
			{
				PX_VM_Error(Ins,"JLE parameters error");
				goto _ERROR;
			}
			if (cVar.type==PX_VM_VARIABLE_TYPE_MEMORY||sVar.type==PX_VM_VARIABLE_TYPE_MEMORY)
			{
				PX_VM_Error(Ins,"JLE parameters error");
				goto _ERROR;
			}
			if (cVar.type==PX_VM_VARIABLE_TYPE_INT)
			{
				p1=(px_float)cVar._int;
			}
			if (cVar.type==PX_VM_VARIABLE_TYPE_FLOAT)
			{
				p1=cVar._float;
			}
			if (sVar.type==PX_VM_VARIABLE_TYPE_INT)
			{
				p2=(px_float)sVar._int;
			}
			if (sVar.type==PX_VM_VARIABLE_TYPE_FLOAT)
			{
				p2=sVar._float;
			}

			if (p1<=p2)
			{
				if (tVar._int<0||tVar._int>Ins->binsize)
				{
					PX_VM_Error(Ins,"JLE jump to invalid address");
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
			cVar=PX_VM_GetParamConst(Ins,opType[0],PX_SCRIPT_VM_PARAM(0),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			sVar=PX_VM_GetParamConst(Ins,opType[1],PX_SCRIPT_VM_PARAM(1),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			tVar=PX_VM_GetParamConst(Ins,opType[2],PX_SCRIPT_VM_PARAM(2),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;

			if (tVar.type!=PX_VM_VARIABLE_TYPE_INT)
			{
				PX_VM_Error(Ins,"JG parameters error");
				goto _ERROR;
			}
			if (cVar.type==PX_VM_VARIABLE_TYPE_STRING||sVar.type==PX_VM_VARIABLE_TYPE_STRING)
			{
				PX_VM_Error(Ins,"JG parameters error");
				goto _ERROR;
			}
			if (cVar.type==PX_VM_VARIABLE_TYPE_MEMORY||sVar.type==PX_VM_VARIABLE_TYPE_MEMORY)
			{
				PX_VM_Error(Ins,"JG parameters error");
				goto _ERROR;
			}
			if (cVar.type==PX_VM_VARIABLE_TYPE_INT)
			{
				p1=(px_float)cVar._int;
			}
			if (cVar.type==PX_VM_VARIABLE_TYPE_FLOAT)
			{
				p1=cVar._float;
			}
			if (sVar.type==PX_VM_VARIABLE_TYPE_INT)
			{
				p2=(px_float)sVar._int;
			}
			if (sVar.type==PX_VM_VARIABLE_TYPE_FLOAT)
			{
				p2=sVar._float;
			}

			if (p1>p2)
			{
				if (tVar._int<0||tVar._int>Ins->binsize)
				{
					PX_VM_Error(Ins,"JG jump to invalid address");
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
			cVar=PX_VM_GetParamConst(Ins,opType[0],PX_SCRIPT_VM_PARAM(0),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			sVar=PX_VM_GetParamConst(Ins,opType[1],PX_SCRIPT_VM_PARAM(1),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			tVar=PX_VM_GetParamConst(Ins,opType[2],PX_SCRIPT_VM_PARAM(2),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;

			if (tVar.type!=PX_VM_VARIABLE_TYPE_INT)
			{
				PX_VM_Error(Ins,"JGE parameters error");
				goto _ERROR;
			}
			if (cVar.type==PX_VM_VARIABLE_TYPE_STRING||sVar.type==PX_VM_VARIABLE_TYPE_STRING)
			{
				PX_VM_Error(Ins,"JGE parameters error");
				goto _ERROR;
			}
			if (cVar.type==PX_VM_VARIABLE_TYPE_MEMORY||sVar.type==PX_VM_VARIABLE_TYPE_MEMORY)
			{
				PX_VM_Error(Ins,"JGE parameters error");
				goto _ERROR;
			}
			if (cVar.type==PX_VM_VARIABLE_TYPE_INT)
			{
				p1=(px_float)cVar._int;
			}
			if (cVar.type==PX_VM_VARIABLE_TYPE_FLOAT)
			{
				p1=cVar._float;
			}
			if (sVar.type==PX_VM_VARIABLE_TYPE_INT)
			{
				p2=(px_float)sVar._int;
			}
			if (sVar.type==PX_VM_VARIABLE_TYPE_FLOAT)
			{
				p2=sVar._float;
			}

			if (p1>=p2)
			{
				if (tVar._int<0||tVar._int>Ins->binsize)
				{
					PX_VM_Error(Ins,"JGE jump to invalid address");
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
			pVar=PX_VMGetVariablePointer(Ins,opType[0],PX_SCRIPT_VM_PARAM(0));
			sVar=PX_VM_GetParamConst(Ins,opType[1],PX_SCRIPT_VM_PARAM(1),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			tVar=PX_VM_GetParamConst(Ins,opType[2],PX_SCRIPT_VM_PARAM(2),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			PX_VM_VariableFree(Ins,pVar);
			pVar->type=PX_VM_VARIABLE_TYPE_INT;
			pVar->_int=0;
			
			if (sVar.type==tVar.type)
			{
				if (sVar.type!=PX_VM_VARIABLE_TYPE_STRING&&sVar.type!=PX_VM_VARIABLE_TYPE_MEMORY)
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
					if(sVar.type==PX_VM_VARIABLE_TYPE_STRING)
						if (PX_strequ(sVar._string.buffer,tVar._string.buffer))
						{
							pVar->_int=1;
							pT->IP+=(4+3*4);
							break;
						}


					if(sVar.type==PX_VM_VARIABLE_TYPE_MEMORY)
						if ((sVar._memory.usedsize==tVar._memory.usedsize)&&PX_memequ(sVar._memory.buffer,tVar._memory.buffer,sVar._memory.usedsize))
						{
							pVar->_int=1;
							pT->IP+=(4+3*4);
							break;
						}
				}
			}
			else
			{
				if (sVar.type == PX_VM_VARIABLE_TYPE_INT && tVar.type == PX_VM_VARIABLE_TYPE_FLOAT)
				{
					if (sVar._int == tVar._float)
					{
						pVar->_int = 1;
						pT->IP += (4 + 3 * 4);
						break;
					}
				}

				if (sVar.type == PX_VM_VARIABLE_TYPE_FLOAT && tVar.type == PX_VM_VARIABLE_TYPE_INT)
				{
					if (sVar._float == tVar._int)
					{
						pVar->_int = 1;
						pT->IP += (4 + 3 * 4);
						break;
					}
				}
			}
			pT->IP+=(4+3*4);
		}
		break;

	case PX_SCRIPT_ASM_INSTR_OPCODE_LGNE:
		{
			pVar=PX_VMGetVariablePointer(Ins,opType[0],PX_SCRIPT_VM_PARAM(0));
			sVar=PX_VM_GetParamConst(Ins,opType[1],PX_SCRIPT_VM_PARAM(1),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			tVar=PX_VM_GetParamConst(Ins,opType[2],PX_SCRIPT_VM_PARAM(2),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			PX_VM_VariableFree(Ins,pVar);
			pVar->type=PX_VM_VARIABLE_TYPE_INT;
			pVar->_int=1;

			if (sVar.type==tVar.type)
			{
				if (sVar.type!=PX_VM_VARIABLE_TYPE_STRING&&sVar.type!=PX_VM_VARIABLE_TYPE_MEMORY)
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
					if(sVar.type==PX_VM_VARIABLE_TYPE_STRING)
						if (PX_strequ(sVar._string.buffer,tVar._string.buffer))
						{
							pVar->_int=0;
							pT->IP+=(4+3*4);
							break;
						}


						if(cVar.type==PX_VM_VARIABLE_TYPE_MEMORY)
							if ((sVar._memory.usedsize==tVar._memory.usedsize)&&PX_memequ(sVar._memory.buffer,tVar._memory.buffer,sVar._memory.usedsize))
							{
								pVar->_int=0;
								pT->IP+=(4+3*4);
								break;
							}
				}
			}
			else
			{
				if (sVar.type== PX_VM_VARIABLE_TYPE_INT&&tVar.type== PX_VM_VARIABLE_TYPE_FLOAT)
				{
					if (sVar._int == tVar._float)
					{
						pVar->_int = 0;
						pT->IP += (4 + 3 * 4);
						break;
					}
				}

				if (sVar.type == PX_VM_VARIABLE_TYPE_FLOAT && tVar.type == PX_VM_VARIABLE_TYPE_INT)
				{
					if (sVar._float == tVar._int)
					{
						pVar->_int = 0;
						pT->IP += (4 + 3 * 4);
						break;
					}
				}
			}
			pT->IP+=(4+3*4);
		}
		break;

	case PX_SCRIPT_ASM_INSTR_OPCODE_LGZ:
		{
			pVar=PX_VMGetVariablePointer(Ins,opType[0],PX_SCRIPT_VM_PARAM(0));

			if (pVar->type!=PX_VM_VARIABLE_TYPE_INT&&pVar->type!=PX_VM_VARIABLE_TYPE_FLOAT)
			{
				PX_VM_Error(Ins,"LGZ parameters error");
				goto _ERROR;
			}

			if (pVar->_int==0)
			{
				pVar->type=PX_VM_VARIABLE_TYPE_INT;
				pVar->_int=1;
			}
			else
			{
				pVar->type=PX_VM_VARIABLE_TYPE_INT;
				pVar->_int=0;
			}
			pT->IP+=(4+1*4);

		}
		break;

	case PX_SCRIPT_ASM_INSTR_OPCODE_LGGZ:
		{
			pVar=PX_VMGetVariablePointer(Ins,opType[0],PX_SCRIPT_VM_PARAM(0));

			if (pVar->type!=PX_VM_VARIABLE_TYPE_INT&&pVar->type!=PX_VM_VARIABLE_TYPE_FLOAT)
			{
				PX_VM_Error(Ins,"LGGZ parameters error");
				goto _ERROR;
			}

			if (pVar->_int>0)
			{
				pVar->type=PX_VM_VARIABLE_TYPE_INT;
				pVar->_int=1;
			}
			else
			{
				pVar->type=PX_VM_VARIABLE_TYPE_INT;
				pVar->_int=0;
			}
			pT->IP+=(4+1*4);

		}
		break;
	case PX_SCRIPT_ASM_INSTR_OPCODE_LGGEZ:
		{
			pVar=PX_VMGetVariablePointer(Ins,opType[0],PX_SCRIPT_VM_PARAM(0));

			if (pVar->type!=PX_VM_VARIABLE_TYPE_INT&&pVar->type!=PX_VM_VARIABLE_TYPE_FLOAT)
			{
				PX_VM_Error(Ins,"LGGEZ parameters error");
				goto _ERROR;
			}

			if (pVar->_int>=0)
			{
				pVar->type=PX_VM_VARIABLE_TYPE_INT;
				pVar->_int=1;
			}
			else
			{
				pVar->type=PX_VM_VARIABLE_TYPE_INT;
				pVar->_int=0;
			}
			pT->IP+=(4+1*4);

		}
		break;
	case PX_SCRIPT_ASM_INSTR_OPCODE_LGLZ:
		{
			pVar=PX_VMGetVariablePointer(Ins,opType[0],PX_SCRIPT_VM_PARAM(0));

			if (pVar->type!=PX_VM_VARIABLE_TYPE_INT&&pVar->type!=PX_VM_VARIABLE_TYPE_FLOAT)
			{
				PX_VM_Error(Ins,"LGLZ parameters error");
				goto _ERROR;
			}

			if (pVar->_int<0)
			{
				pVar->type=PX_VM_VARIABLE_TYPE_INT;
				pVar->_int=1;
			}
			else
			{
				pVar->type=PX_VM_VARIABLE_TYPE_INT;
				pVar->_int=0;
			}
			pT->IP+=(4+1*4);

		}
		break;
	case PX_SCRIPT_ASM_INSTR_OPCODE_LGLEZ:
		{
			pVar=PX_VMGetVariablePointer(Ins,opType[0],PX_SCRIPT_VM_PARAM(0));

			if (pVar->type!=PX_VM_VARIABLE_TYPE_INT&&pVar->type!=PX_VM_VARIABLE_TYPE_FLOAT)
			{
				PX_VM_Error(Ins,"LGLEZ parameters error");
				goto _ERROR;
			}

			if (pVar->_int<=0)
			{
				pVar->type=PX_VM_VARIABLE_TYPE_INT;
				pVar->_int=1;
			}
			else
			{
				pVar->type=PX_VM_VARIABLE_TYPE_INT;
				pVar->_int=0;
			}
			pT->IP+=(4+1*4);

		}
		break;


	case PX_SCRIPT_ASM_INSTR_OPCODE_CALL:
		{
			cVar=PX_VM_GetParamConst(Ins,opType[0],PX_SCRIPT_VM_PARAM(0),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			if (cVar.type!=PX_VM_VARIABLE_TYPE_INT)
			{
				PX_VM_Error(Ins,"CALL parameters error");
				goto _ERROR;
			}

			if (opType[0]==PX_SCRIPT_ASM_OPTYPE_HOST)
			{
				if (Ins->_host[cVar._int].map!=PX_NULL)
				{
					px_int runtick;
					runtick = ((PX_VM_Function_Modules)(Ins->_host[cVar._int].map))(Ins, Ins->_host[cVar._int].userptr);
					if (runtick==0)
					{
						PX_VM_Error(Ins,"CALL function crash");
						goto _ERROR;//runtime error
					}
					else
					{
						if (tick<runtick)
						{
							if(tick > 0)
								tick = 0;
						}
						else
						{
							if(runtick!=1)
								tick -= runtick;
						}
					}
				}
				else
				{
					PX_VM_Error(Ins,"CALL host call not found");
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
					PX_VM_Error(Ins,"CALL error");
					goto _ERROR;
				}
				

				pVar=&Ins->_mem[pT->SP];
				if (pVar->type==PX_VM_VARIABLE_TYPE_STRING)
				{
					PX_StringFree(&pVar->_string);
				}

				if (pVar->type==PX_VM_VARIABLE_TYPE_MEMORY)
				{
					PX_MemoryFree(&pVar->_memory);
				}
				
				Ins->_mem[pT->SP].type=PX_VM_VARIABLE_TYPE_INT;
				Ins->_mem[pT->SP]._int=pT->IP+8;
				pT->IP=cVar._int;
			}

		}
		break;

	case PX_SCRIPT_ASM_INSTR_OPCODE_PUSH:
		{
			cVar=PX_VM_GetParamConst(Ins,opType[0],PX_SCRIPT_VM_PARAM(0),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			if(pT->SP>0)
			pT->SP-=1;
			else
			{
				PX_VM_Error(Ins,"push error");
				goto _ERROR;
			}
			pVar=&Ins->_mem[pT->SP];

			if (pVar->type==PX_VM_VARIABLE_TYPE_STRING)
			{
				PX_StringFree(&pVar->_string);
			}

			if (pVar->type==PX_VM_VARIABLE_TYPE_MEMORY)
			{
				PX_MemoryFree(&pVar->_memory);
			}

			if (cVar.type==PX_VM_VARIABLE_TYPE_STRING)
			{
				if(!PX_StringInitialize(Ins->mp,&pVar->_string)) goto _ERROR;
				if(!PX_StringCat(&pVar->_string,cVar._string.buffer)) goto _ERROR;
				pVar->type=cVar.type;
			}
			else if (cVar.type==PX_VM_VARIABLE_TYPE_MEMORY)
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
				if(Ins->_mem[pT->SP].type==PX_VM_VARIABLE_TYPE_INT)
				{
					pT->IP=Ins->_mem[pT->SP]._int;
					pT->SP+=1;
				}
				else
				{
					PX_VM_Error(Ins,"POP error");
					goto _ERROR;
				}
			}
			else if (opType[0]==PX_SCRIPT_ASM_OPTYPE_SP)
			{
				PX_VM_Error(Ins,"POP error");
				goto _ERROR;
			}
			else if (opType[0]==PX_SCRIPT_ASM_OPTYPE_BP)
			{
				if(Ins->_mem[pT->SP].type==PX_VM_VARIABLE_TYPE_INT)
				{
					pT->BP=Ins->_mem[pT->SP]._int;
					pT->SP+=1;
					pT->IP+=8;
				}
				else
				{
					PX_VM_Error(Ins,"POP error");
					goto _ERROR;
				}
			}
			else
			{
				pVar=PX_VMGetVariablePointer(Ins,opType[0],PX_SCRIPT_VM_PARAM(0));

				if (pVar->type==PX_VM_VARIABLE_TYPE_STRING)
				{
					PX_StringFree(&pVar->_string);
				}

				if (pVar->type==PX_VM_VARIABLE_TYPE_MEMORY)
				{
					PX_MemoryFree(&pVar->_memory);
				}
				*pVar=Ins->_mem[pT->SP];

				Ins->_mem[pT->SP].type=PX_VM_VARIABLE_TYPE_INT;
				Ins->_mem[pT->SP]._int=0;

				pT->SP+=1;
				pT->IP+=8;
			}
			
		}
		break;

	case PX_SCRIPT_ASM_INSTR_OPCODE_POPN:
		{
			cVar=PX_VM_GetParamConst(Ins,opType[0],PX_SCRIPT_VM_PARAM(0),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			if (cVar.type!=PX_VM_VARIABLE_TYPE_INT)
			{
				PX_VM_Error(Ins,"POPN error");
				goto _ERROR;
			}
			for (i=0;i<cVar._int;i++)
			{
				pVar=&Ins->_mem[pT->SP+i];

				if (pVar->type==PX_VM_VARIABLE_TYPE_STRING)
				{
					PX_StringFree(&pVar->_string);
				}

				if (pVar->type==PX_VM_VARIABLE_TYPE_MEMORY)
				{
					PX_MemoryFree(&pVar->_memory);
				}

				Ins->_mem[pT->SP+i].type=PX_VM_VARIABLE_TYPE_INT;
				Ins->_mem[pT->SP+i]._int=0;
			}
			pT->SP+=cVar._int;
			pT->IP+=8;
		}
		break;
	case PX_SCRIPT_ASM_INSTR_OPCODE_ADR:
		{
			pVar=PX_VMGetVariablePointer(Ins,opType[0],PX_SCRIPT_VM_PARAM(0));
			PX_VM_VariableFree(Ins,pVar);
			switch(opType[1])
			{
			case PX_SCRIPT_ASM_OPTYPE_GLOBAL:
				pVar->_int=PX_SCRIPT_VM_PARAM(1);
				break;
			case PX_SCRIPT_ASM_OPTYPE_LOCAL:
				pVar->_int=pT->BP-PX_SCRIPT_VM_PARAM(1);
				break;
			case PX_SCRIPT_ASM_OPTYPE_GLOBAL_REGREF:
				if (pT->R[PX_SCRIPT_VM_PARAM(1)].type!=PX_VM_VARIABLE_TYPE_INT)
				{
					PX_VM_Error(Ins,"ADR error");
					goto _ERROR;
				}
				pVar->_int=pT->R[PX_SCRIPT_VM_PARAM(1)]._int;
				break;
			case PX_SCRIPT_ASM_OPTYPE_GLOBAL_GLOBALREF:
				if (Ins->_mem[PX_SCRIPT_VM_PARAM(1)].type!=PX_VM_VARIABLE_TYPE_INT)
				{
					PX_VM_Error(Ins,"ADR error");
					goto _ERROR;
				}
				pVar->_int=Ins->_mem[PX_SCRIPT_VM_PARAM(1)]._int;
				break;
			case PX_SCRIPT_ASM_OPTYPE_GLOBAL_LOCALREF:
				if (Ins->_mem[pT->BP-PX_SCRIPT_VM_PARAM(1)].type!=PX_VM_VARIABLE_TYPE_INT)
				{
					PX_VM_Error(Ins,"ADR error");
					goto _ERROR;
				}
				pVar->_int=Ins->_mem[pT->BP-PX_SCRIPT_VM_PARAM(1)]._int;
				break;
			case PX_SCRIPT_ASM_OPTYPE_LOCAL_REGREF:
				if (Ins->_mem[PX_SCRIPT_VM_PARAM(1)].type!=PX_VM_VARIABLE_TYPE_INT)
				{
					PX_VM_Error(Ins,"ADR error");
					goto _ERROR;
				}
				pVar->_int=pT->BP-pT->R[PX_SCRIPT_VM_PARAM(1)]._int;
				break;
			case PX_SCRIPT_ASM_OPTYPE_LOCAL_GLOBALREF:
				if (Ins->_mem[PX_SCRIPT_VM_PARAM(1)].type!=PX_VM_VARIABLE_TYPE_INT)
				{
					PX_VM_Error(Ins,"ADR error");
					goto _ERROR;
				}
				pVar->_int=pT->BP-Ins->_mem[PX_SCRIPT_VM_PARAM(1)]._int;
				break;
			case PX_SCRIPT_ASM_OPTYPE_LOCAL_LOCALREF:
				if (Ins->_mem[pT->BP-PX_SCRIPT_VM_PARAM(1)].type!=PX_VM_VARIABLE_TYPE_INT)
				{
					PX_VM_Error(Ins,"ADR error");
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
				PX_VM_VariableFree(Ins,&cVar);
				PX_VM_VariableFree(Ins,&sVar);
				PX_VM_VariableFree(Ins,&tVar);

				PX_VMThreadStop(Ins,Ins->T);
				return PX_VM_RUNRETURN_END;
			}
			else
			{
				cVar=Ins->_mem[pT->SP];
				if (cVar.type!=PX_VM_VARIABLE_TYPE_INT)
				{
					PX_VM_Error(Ins,"RET error");
					goto _ERROR;
				}
				pT->SP+=1;
				if (pT->SP>=Ins->VM_memsize||cVar._int==-1)
				{
					PX_VM_VariableFree(Ins,&cVar);
					PX_VM_VariableFree(Ins,&sVar);
					PX_VM_VariableFree(Ins,&tVar);

					PX_VMThreadStop(Ins,Ins->T);
					return PX_VM_RUNRETURN_END;
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
				//"Story Script Debugger Interrupt.";
				PX_VMSuspend(Ins);
			}
			pT->IP+=4;
		}
		break;
	case PX_SCRIPT_ASM_INSTR_OPCODE_WAIT:
		{
			cVar=PX_VM_GetParamConst(Ins,opType[0],PX_SCRIPT_VM_PARAM(0),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			if (cVar.type!=PX_VM_VARIABLE_TYPE_INT)
			{
				PX_VM_Error(Ins,"WAIT index error");
				goto _ERROR;
			}
			if (Ins->signal[cVar._int])
			{
				PX_VM_VariableFree(Ins,&cVar);
				PX_VM_VariableFree(Ins,&sVar);
				PX_VM_VariableFree(Ins,&tVar);
				return PX_VM_RUNRETURN_WAIT;
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
			cVar=PX_VM_GetParamConst(Ins,opType[0],PX_SCRIPT_VM_PARAM(0),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			sVar=PX_VM_GetParamConst(Ins,opType[1],PX_SCRIPT_VM_PARAM(1),&bOutofMemory);
			if(bOutofMemory) goto _ERROR;
			if (cVar.type!=PX_VM_VARIABLE_TYPE_INT||sVar.type!=PX_VM_VARIABLE_TYPE_INT)
			{
				PX_ERROR("SIGNAL ERROR");
				goto _ERROR;
			}
			Ins->signal[cVar._int]=sVar._int;
			pT->IP+=4+4*2;
		}
		break;
	default:
		PX_VM_Error(Ins,"INVALID INSTR");
		goto _ERROR;
	}

	PX_VM_VariableFree(Ins,&cVar);
	PX_VM_VariableFree(Ins,&sVar);
	PX_VM_VariableFree(Ins,&tVar);

	if (pT->IP==-1)
		{
			break;
		}
	}
	PX_VMThreadStop(Ins, Ins->T);
	return PX_VM_RUNRETURN_END;
_ERROR:

	PX_VM_VariableFree(Ins,&cVar);
	PX_VM_VariableFree(Ins,&sVar);
	PX_VM_VariableFree(Ins,&tVar);
	PX_VMSuspend(Ins);
	PX_VM_Error(Ins,"Virtual Machine runtime error");
	return PX_VM_RUNRETURN_ERROR;
}

px_bool PX_VMCallFunctionEx(PX_VM* Ins, px_int threadID, const px_char* functionName, PX_VM_VARIABLE args[], px_int paramcount,px_int returnIP)
{
	px_int i, j, ip = -1;
	px_int old_T;
	if (paramcount > 16 || threadID >= Ins->maxThreadCount)
	{
		return PX_FALSE;
	}

	if (PX_strlen(functionName) >= __PX_SCRIPT_ASM_MNEMONIC_NAME_LEN)
	{
		return PX_FALSE;
	}

	for (i = 0; i < Ins->funcCount; i++)
	{
		if (PX_strequ2(Ins->_func[i].name, functionName))
		{
			ip = Ins->_func[i].Addr;
			break;
		}
	}

	if (i == Ins->funcCount)
	{
		return PX_FALSE;
	}

	if (Ins->pThread[threadID].Activated == PX_FALSE)
	{
		Ins->pThread[threadID].suspend = PX_FALSE;
		Ins->pThread[threadID].Activated = PX_TRUE;
		Ins->pThread[threadID].SP = Ins->VM_memsize - Ins->VM_Stacksize * threadID;
		Ins->pThread[threadID].BP = Ins->pThread[threadID].SP;
		Ins->pThread[threadID].IP = ip;
		for (j = 0; j < sizeof(Ins->pThread[threadID].R) / sizeof(Ins->pThread[threadID].R[0]); j++)
		{
			PX_VM_VariableFree(Ins, &Ins->pThread[threadID].R[j]);
		}
	}
	else
	{
		return PX_FALSE;
	}

	old_T = Ins->T;
	if (!PX_VMThreadSwitch(Ins, threadID))
	{
		return PX_FALSE;
	}

	for (i = paramcount - 1; i >= 0; i--)
	{
		PX_VM_PUSH(Ins, args[i]);
	}

	PX_VM_PUSH(Ins, PX_VM_Variable_int(returnIP));

	PX_VMRunThread(Ins, -1);

	Ins->T = old_T;
	return PX_TRUE;
}

px_bool PX_VMRunFunction(PX_VM *Ins,px_int threadID,const px_char *functionName,PX_VM_VARIABLE args[],px_int paramcount)
{
	return PX_VMCallFunctionEx(Ins, threadID, functionName, args, paramcount, -1);
}



px_bool PX_VMCallFunction(PX_VM* Ins, px_int threadID, const px_char* functionName, PX_VM_VARIABLE args[], px_int paramcount)
{
	return PX_VMCallFunctionEx(Ins, threadID, functionName, args, paramcount, Ins->pThread[threadID].IP);
}

px_bool PX_VMRunFunctionIndex(PX_VM *Ins,px_int threadID,px_int funcIndex,PX_VM_VARIABLE args[],px_int paramcount)
{
	px_int i,j,ip;
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
			PX_VM_VariableFree(Ins,&Ins->pThread[threadID].R[j]);
		}
	}
	else
	{
		return PX_FALSE;
	}

	old_T=Ins->T;
	if(!PX_VMThreadSwitch(Ins,threadID))
	{
		return PX_FALSE;
	}

	for (i=paramcount-1;i>=0;i--)
	{
		PX_VM_PUSH(Ins,args[i]);
	}

	PX_VM_PUSH(Ins,PX_VM_Variable_int(-1));
	
	PX_VMRunThread(Ins,-1);	
	Ins->T=old_T;
	return PX_TRUE;
}

px_int PX_VMGetFreeThread(PX_VM* Ins,px_int reservedThread)
{
	px_int i;
	for (i = reservedThread+1; i < Ins->maxThreadCount; i++)
	{
		if (!Ins->pThread[i].Activated)
			return i;
	}
	return -1;
}



px_bool PX_VMBeginThreadFunction(PX_VM *Ins,px_int threadID,const px_char *func,PX_VM_VARIABLE args[],px_int paramcount)
{
	px_int i,j,ip=-1;
	px_int old_T;
	px_char	uprname[__PX_SCRIPT_ASM_MNEMONIC_NAME_LEN];
	if (!Ins->binsize) return PX_FALSE;
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
			PX_VM_VariableFree(Ins,&Ins->pThread[threadID].R[j]);
		}
	}
	else
	{
		return PX_FALSE;
	}

	old_T=Ins->T;
	if(!PX_VMThreadSwitch(Ins,threadID))
	{
		return PX_FALSE;
	}

	for (i=paramcount-1;i>=0;i--)
	{
		PX_VM_PUSH(Ins,args[i]);
	}

	PX_VM_PUSH(Ins,PX_VM_Variable_int(-1));

	if(!PX_VMThreadSwitch(Ins,old_T))
	{
		return PX_FALSE;
	}
	return PX_TRUE;
}

px_bool PX_VMBeginThreadFunctionIndex(PX_VM *Ins,px_int threadID,px_int funcIndex,PX_VM_VARIABLE args[],px_int paramcount)
{
	px_int i,j,ip;
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
		for(j=0;j<PX_VM_REG_COUNT;j++)
		{
			PX_VM_VariableFree(Ins,&Ins->pThread[threadID].R[j]);
		}
	}
	else
	{
		return PX_FALSE;
	}

	old_T=Ins->T;
	if(!PX_VMThreadSwitch(Ins,threadID))
	{
		return PX_FALSE;
	}


	for (i=paramcount-1;i>=0;i--)
	{
		PX_VM_PUSH(Ins,args[i]);
	}
	

	PX_VM_PUSH(Ins,PX_VM_Variable_int(-1));

	if(!PX_VMThreadSwitch(Ins,old_T))
	{
		return PX_FALSE;
	}
	return PX_TRUE;
}


px_int PX_VMGetFreeThreadId(PX_VM *Ins)
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

px_void PX_VMSleep(PX_VM* Ins, px_int thread, px_uint sleepms)
{
	if (thread>=0&&thread<Ins->maxThreadCount)
	{
		Ins->pThread[thread].sleep = sleepms;
	}
}

px_void PX_VMSuspend(PX_VM* Ins)
{
	Ins->Suspend = PX_TRUE;
	
}

px_void PX_VMContinue(PX_VM* Ins)
{
	Ins->Suspend = PX_FALSE;
}


px_void PX_VMDebugContinue(PX_VM* Ins)
{
	px_int i;
	Ins->Suspend = PX_FALSE;
	if (Ins->debug)
	{
		for (i = 0; i < PX_COUNTOF(Ins->breakpoints); i++)
		{
			if (Ins->breakpoints[i] == -1)
			{
				continue;
			}

			if (Ins->pThread[Ins->T].IP == Ins->breakpoints[i])
			{
				Ins->breakpoint_next = PX_TRUE;
				break;
			}
		}
	}
}


px_void PX_VMReset(PX_VM* Ins)
{
	px_int i;
	for (i = 0; i < Ins->maxThreadCount; i++)
	{
		if (Ins->pThread[i].Activated)
		{
			PX_VMThreadStop(Ins, i);
		}
	}
	Ins->Suspend = PX_FALSE;
	PX_VMRunFunction(Ins, 0, "_BOOT", 0, 0);
}

px_bool PX_VMFree(PX_VM *Ins)
{
	px_int i,j;
	for (i=0;i<Ins->VM_memsize;i++)
	{
		if (Ins->_mem[i].type==PX_VM_VARIABLE_TYPE_STRING)
		{
			PX_StringFree(&Ins->_mem[i]._string);
		}
		if (Ins->_mem[i].type==PX_VM_VARIABLE_TYPE_MEMORY)
		{
			PX_MemoryFree(&Ins->_mem[i]._memory);
		}
	}

	
	if (Ins->pThread)
	{
		//Free Thread
		for (i=0;i<Ins->maxThreadCount;i++)
		{
				for (j=0;j<PX_VM_REG_COUNT;j++)
				{
					if (Ins->pThread[i].R[j].type==PX_VM_VARIABLE_TYPE_STRING)
					{
						PX_StringFree(&Ins->pThread[i].R[j]._string);
					}
					if (Ins->pThread[i].R[j].type==PX_VM_VARIABLE_TYPE_MEMORY)
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

	PX_memset(Ins, 0, sizeof(PX_VM));

	return PX_TRUE;
}

px_bool PX_ScriptVM_InstanceInitialize(PX_VM* Ins, px_memorypool* mp, px_byte* code, px_int size)
{
	return PX_VMInitialize(Ins, mp, code, size);
}

px_bool PX_VMRegistHostFunction(PX_VM *Ins,const px_char *name,PX_VM_Function_Modules funcModules,px_void *userptr)
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

px_void PX_VM_RET(PX_VM *Ins,PX_VM_VARIABLE cVar)
{
	PX_VM_VARIABLE *pVar;
	pVar=&Ins->pThread[Ins->T].R[1];

	PX_VM_VariableFree(Ins,pVar);
	
	if (cVar.type==PX_VM_VARIABLE_TYPE_STRING)
	{
		PX_StringInitialize(Ins->mp,&pVar->_string);
		PX_StringCat(&pVar->_string,cVar._string.buffer);
		pVar->type=cVar.type;
	}
	else if (cVar.type==PX_VM_VARIABLE_TYPE_MEMORY)
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

px_void PX_VM_RET_String(PX_VM* Ins, const px_char* pstr)
{
	PX_VM_VARIABLE* pVar;
	pVar = &Ins->pThread[Ins->T].R[1];
	PX_VM_VariableFree(Ins, pVar);
	pVar->type = PX_VM_VARIABLE_TYPE_STRING;

	PX_StringInitialize(Ins->mp, &pVar->_string);
	PX_StringCat(&pVar->_string, pstr);
}

px_void PX_VM_RET_int(PX_VM* Ins, px_int _int)
{
	PX_VM_VARIABLE* pVar;
	pVar = &Ins->pThread[Ins->T].R[1];
	PX_VM_VariableFree(Ins, pVar);
	pVar->type = PX_VM_VARIABLE_TYPE_INT;
	pVar->_int = _int;
}

px_void PX_VM_RET_ptr(PX_VM* Ins, px_void *ptr)
{
	PX_VM_VARIABLE* pVar;
	pVar = &Ins->pThread[Ins->T].R[1];
	PX_VM_VariableFree(Ins, pVar);
	pVar->type = PX_VM_VARIABLE_TYPE_INT;
	pVar->_userptr = ptr;
}

px_void PX_VM_RET_float(PX_VM* Ins, px_float _float)
{
	PX_VM_VARIABLE* pVar;
	pVar = &Ins->pThread[Ins->T].R[1];
	PX_VM_VariableFree(Ins, pVar);
	pVar->type = PX_VM_VARIABLE_TYPE_FLOAT;
	pVar->_float = _float;
}

px_void PX_VM_RET_memory(PX_VM* Ins, const px_byte* data,px_int size)
{
	PX_VM_VARIABLE* pVar;
	pVar = &Ins->pThread[Ins->T].R[1];

	PX_VM_VariableFree(Ins, pVar);
	pVar->type = PX_VM_VARIABLE_TYPE_MEMORY;

	PX_MemoryInitialize(Ins->mp, &pVar->_memory);
	PX_MemoryAlloc(&pVar->_memory, size);
	PX_MemoryCat(&pVar->_memory, data, size);
}

px_void PX_VM_PUSH(PX_VM *Ins,PX_VM_VARIABLE cVar)
{
	PX_VM_VARIABLE *pVar;
	PX_VM_Thread *pT=&Ins->pThread[Ins->T];
	if (pT->SP>0)
	{
		pT->SP-=1;
	}
	else
		return;
	
	pVar=&Ins->_mem[pT->SP];

	if (pVar->type==PX_VM_VARIABLE_TYPE_STRING)
	{
		PX_StringFree(&pVar->_string);
	}

	if (pVar->type==PX_VM_VARIABLE_TYPE_MEMORY)
	{
		PX_MemoryFree(&pVar->_memory);
	}


	if (cVar.type==PX_VM_VARIABLE_TYPE_STRING)
	{
		PX_StringInitialize(Ins->mp,&pVar->_string);
		PX_StringCat(&pVar->_string,cVar._string.buffer);
		pVar->type=cVar.type;
	}
	else if (cVar.type==PX_VM_VARIABLE_TYPE_MEMORY)
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

px_void PX_VM_Sleep(PX_VM* Ins, px_uint sleep)
{
	Ins->pThread[Ins->T].sleep = sleep;
}

px_void PX_VM_ThreadSleep(PX_VM* Ins, px_int threadid, px_uint sleep)
{
	if (threadid>=0&&threadid<Ins->maxThreadCount)
	{
		Ins->pThread[threadid].sleep = sleep;
	}
}

px_void PX_VM_POPN(PX_VM *Ins,px_int T,px_int n)
{
	PX_VM_VARIABLE *pVar;
	px_int i;
	PX_VM_Thread *pT=&Ins->pThread[T];

	for (i=0;i<n;i++)
	{
		pVar=&Ins->_mem[pT->SP+i];

		if (pVar->type==PX_VM_VARIABLE_TYPE_STRING)
		{
			PX_StringFree(&pVar->_string);
		}

		if (pVar->type==PX_VM_VARIABLE_TYPE_MEMORY)
		{
			PX_MemoryFree(&pVar->_memory);
		}

		Ins->_mem[pT->SP+i].type=PX_VM_VARIABLE_TYPE_INT;
		Ins->_mem[pT->SP+i]._int=0;
	}
	pT->SP+=n;
	if (pT->SP>Ins->VM_memsize)
	{
		PX_VM_Error(Ins,"host call Stack overflow!");
	}
}

PX_VM_VARIABLE PX_VM_Variable_int(px_int _int)
{
	PX_VM_VARIABLE var;
	var.type=PX_VM_VARIABLE_TYPE_INT;
	var._int=_int;
	return var;
}

PX_VM_VARIABLE PX_VM_Variable_handle(px_void *_ptr)
{
	PX_VM_VARIABLE var;
	var.type = PX_VM_VARIABLE_TYPE_HANDLE;
	var._userptr = _ptr;
	return var;
}

PX_VM_VARIABLE PX_VM_Variable_float(px_float _float)
{
	PX_VM_VARIABLE var;
	var.type=PX_VM_VARIABLE_TYPE_FLOAT;
	var._float=_float;
	return var;
}

PX_VM_VARIABLE PX_VM_Variable_string(px_string _string)
{
	PX_VM_VARIABLE var;
	var.type=PX_VM_VARIABLE_TYPE_STRING;
	var._string=_string;
	return var;
}

PX_VM_VARIABLE PX_VM_Variable_memory(px_byte *buffer,px_int _size)
{
	PX_VM_VARIABLE var;
	var.type=PX_VM_VARIABLE_TYPE_MEMORY;
	var._memory.allocsize=0;
	var._memory.buffer=buffer;
	var._memory.usedsize=_size;
	return var;
}

PX_VM_VARIABLE PX_VM_Variable_const_string(const px_char *buffer)
{
	PX_VM_VARIABLE var;
	var.type=PX_VM_VARIABLE_TYPE_STRING;
	var._string.buffer=(px_char *)buffer;
	var._string.bufferlen=0;
	var._string.mp=PX_NULL;
	return var;
}

PX_VM_VARIABLE PX_VM_Variable_const_memory(const px_byte *buffer,px_int _size)
{
	PX_VM_VARIABLE var;
	var.type=PX_VM_VARIABLE_TYPE_MEMORY;
	var._memory.allocsize=0;
	var._memory.buffer=(px_byte *)buffer;
	var._memory.usedsize=_size;
	return var;
}


px_int PX_VM_GetFunctionIndex(PX_VM *Ins,px_char *func)
{
	px_int i;
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

px_bool PX_VMDebuggerMapInitialize(px_memorypool* mp, PX_VM_DebuggerMap* pDebugMap)
{
	pDebugMap->mp = mp;
	if (!PX_StringInitialize(mp, &pDebugMap->source))return PX_FALSE;
	if (!PX_VectorInitialize(mp, &pDebugMap->map, sizeof(PX_SCRIPT_ASM_SOURCE_MAP), 64))return PX_FALSE;
	return PX_TRUE;
}

px_void PX_VMDebuggerMapFree(PX_VM_DebuggerMap* pDebugMap)
{
	PX_StringFree(&pDebugMap->source);
	PX_VectorFree(&pDebugMap->map);
}

px_bool PX_VMLocalAlloc(PX_VM *Ins,px_int size,PX_VM_MEMORY_PTR *mem_ptr)
{
	px_int i;
	PX_VM_Thread *pT=&Ins->pThread[Ins->T];

	if (size>pT->SP-Ins->VM_Globalsize)
	{
		return PX_FALSE;
	}
	pT->SP-=size;
	for (i=pT->SP;i<pT->SP+size;i++)
	{
		if (Ins->_mem[i].type==PX_VM_VARIABLE_TYPE_MEMORY)
		{
			PX_MemoryFree(&Ins->_mem[i]._memory);
		}
		if (Ins->_mem[i].type==PX_VM_VARIABLE_TYPE_STRING)
		{
			PX_StringFree(&Ins->_mem[i]._string);
		}
		Ins->_mem[i].type=PX_VM_VARIABLE_TYPE_INT;
	}
	mem_ptr->ptr= pT->SP;
	mem_ptr->size=size;
	return PX_TRUE;
}

px_bool PX_VMLocalFree(PX_VM *Ins,PX_VM_MEMORY_PTR *mem_ptr)
{
	px_int i;
	PX_VM_Thread *pT=&Ins->pThread[Ins->T];

	if (pT->SP!=mem_ptr->ptr)
	{
		return PX_FALSE;
	}
	for (i=pT->SP;i<pT->SP+mem_ptr->size;i++)
	{
		if (Ins->_mem[i].type==PX_VM_VARIABLE_TYPE_MEMORY)
		{
			PX_MemoryFree(&Ins->_mem[i]._memory);
		}
		if (Ins->_mem[i].type==PX_VM_VARIABLE_TYPE_STRING)
		{
			PX_StringFree(&Ins->_mem[i]._string);
		}
		Ins->_mem[i].type=PX_VM_VARIABLE_TYPE_INT;
	}
	pT->SP+=mem_ptr->size;
	return PX_TRUE;
}


px_bool PX_VMThreadSwitch(PX_VM *Ins,px_int T)
{
	if (T<Ins->maxThreadCount&&Ins->pThread[T].Activated)
	{
		Ins->T=T;
		return PX_TRUE;
	}
	return PX_FALSE;
}

px_void PX_VMThreadStop(PX_VM *Ins,px_int ThreadId)
{
	px_int popn;
	if (ThreadId<Ins->maxThreadCount)
	{
		if (Ins->pThread[ThreadId].Activated)
		{
			popn=Ins->VM_memsize-Ins->VM_Stacksize*ThreadId-Ins->pThread[ThreadId].SP;
			PX_VM_POPN(Ins,ThreadId,popn);
			Ins->pThread[ThreadId].Activated=PX_FALSE;
			Ins->pThread[ThreadId].suspend = PX_FALSE;
		}
	}
}


px_void PX_VMThreadClear(PX_VM *Ins,px_int ThreadId)
{
	px_int i,popn;
	if (ThreadId<Ins->maxThreadCount)
	{
		    //clear stack
			popn=Ins->VM_memsize-Ins->VM_Stacksize*ThreadId-Ins->pThread[ThreadId].SP;
			PX_VM_POPN(Ins,ThreadId,popn);
			//Clear registers
			for (i=0;i<PX_VM_REG_COUNT;i++)
			 {
			 	PX_VM_VariableFree(Ins,&Ins->pThread[ThreadId].R[i]);
			 }
			Ins->pThread[ThreadId].Activated=PX_FALSE;
	}
}

px_void PX_VMThreadSuspend(PX_VM *Ins,px_int ThreadId)
{
	if (ThreadId<Ins->maxThreadCount)
	{
		if (Ins->pThread[ThreadId].Activated)
		{
			Ins->pThread[ThreadId].suspend=PX_TRUE;
		}
	}
}

px_void PX_VMThreadResume(PX_VM *Ins,px_int ThreadId)
{
	if (ThreadId<Ins->maxThreadCount)
	{
		if (Ins->pThread[ThreadId].Activated)
		{
			Ins->pThread[ThreadId].suspend=PX_FALSE;
		}
	}
}

px_bool PX_VMIsRuning(PX_VM* Ins)
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



px_void PX_VMRun(PX_VM* Ins, px_int tick, px_dword elapsed)
{
	px_int i, activating = 0;
	if (Ins->binsize==0)
	{
		return;
	}
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

	while (1)
	{
		px_int ActivatingThreadCount = 0;
		for (i = 0; i < Ins->maxThreadCount; i++)
		{
			if (Ins->pThread[i].Activated)
			{
				if (Ins->pThread[i].sleep > elapsed)
				{
					Ins->pThread[i].sleep -= elapsed;
				}
				else
				{
					Ins->pThread[i].sleep = 0;
				}

				PX_VMThreadSwitch(Ins, i);
				if (tick>=0)
				{
					if (tick < PX_VM_ATOM_INSTRUCTMENTS)
					{
						PX_VM_RUNRETURN ret = PX_VMRunThread(Ins, tick);
						if (ret == PX_VM_RUNRETURN_WAIT || ret == PX_VM_RUNRETURN_SUSPEND)
						{
							continue;
						}
						else
						{
							ActivatingThreadCount++;
							tick = 0;
						}
					}
					else
					{
						PX_VM_RUNRETURN ret = PX_VMRunThread(Ins, PX_VM_ATOM_INSTRUCTMENTS);
						if (ret == PX_VM_RUNRETURN_WAIT || ret == PX_VM_RUNRETURN_SUSPEND)
						{
							continue;
						}
						else
						{
							ActivatingThreadCount++;
							tick -= PX_VM_ATOM_INSTRUCTMENTS;
						}

					}

					if (tick <= 0)
					{
						return;
					}
				}
				else
				{
					PX_VM_RUNRETURN ret = PX_VMRunThread(Ins, PX_VM_ATOM_INSTRUCTMENTS);
					if (ret == PX_VM_RUNRETURN_WAIT || ret == PX_VM_RUNRETURN_SUSPEND)
					{
						continue;
					}
					ActivatingThreadCount++;
				}
			}
		}
		if (ActivatingThreadCount==0)
		{
			break;
		}
	}
}

px_void PX_VMTriggerBreakPoint(PX_VM* Ins, px_int IP)
{
	px_int i;
	(Ins->debug = PX_TRUE);
	for (i = 0; i < PX_COUNTOF(Ins->breakpoints); i++)
	{
		if (Ins->breakpoints[i] == IP)
		{
			Ins->breakpoints[i] = -1;
			return;
		}
	}

	for (i = 0; i < PX_COUNTOF(Ins->breakpoints); i++)
	{
		if (Ins->breakpoints[i] == -1)
		{
			Ins->breakpoints[i] = IP;
			return;
		}
	}

}



px_void PX_VMAddBreakPoint(PX_VM* Ins, px_int IP)
{
	px_int i;
	(Ins->debug = PX_TRUE);
	for (i=0;i<PX_COUNTOF(Ins->breakpoints);i++)
	{
		if (Ins->breakpoints[i]==IP)
		{
			return;
		}
	}

	for (i = 0; i < PX_COUNTOF(Ins->breakpoints); i++)
	{
		if (Ins->breakpoints[i] == -1)
		{
			Ins->breakpoints[i] = IP;
			return;
		}
	}

}

px_void PX_VMRemoveBreakPoint(PX_VM* Ins, px_int IP)
{
	px_int i;
	for (i = 0; i < PX_COUNTOF(Ins->breakpoints); i++)
	{
		if (Ins->breakpoints[i] == IP)
		{
			Ins->breakpoints[i] = -1;
			return;
		}
	}

	for (i = 0; i < PX_COUNTOF(Ins->breakpoints); i++)
	{
		if (Ins->breakpoints[i] != -1)
		{
			return;
		}
	}
	(Ins->debug = PX_FALSE);
}
px_bool  PX_VMDebuggerGetVarParamName(PX_VM* Ins, px_char optype, px_int param, px_char content[], px_int size)
{
	PX_VM_Thread* pT = Ins->pThread + Ins->T;
	switch (optype)
	{
	case PX_SCRIPT_ASM_OPTYPE_IP:
	{
		return PX_FALSE;
	}
	break;

	case PX_SCRIPT_ASM_OPTYPE_SP:
	{
		return PX_FALSE;
	}
	break;

	case PX_SCRIPT_ASM_OPTYPE_BP:
	{
		return PX_FALSE;
	}
	break;
	case PX_SCRIPT_ASM_OPTYPE_HOST:
	case PX_SCRIPT_ASM_OPTYPE_INT:
	{
		return PX_FALSE;
	}
	break;
	case PX_SCRIPT_ASM_OPTYPE_FLOAT:
	{
		//Must be IEEE 754 format
		return PX_FALSE;
	}
	break;
	case PX_SCRIPT_ASM_OPTYPE_STRING:
	{
		return PX_FALSE;
	}
	break;

	case PX_SCRIPT_ASM_OPTYPE_MEMORY:
	{
		return PX_FALSE;
	}
	break;

	case PX_SCRIPT_ASM_OPTYPE_REG:
	{
		PX_sprintf1(content, size, "R%1", PX_STRINGFORMAT_INT(param));
	}
	break;
	case PX_SCRIPT_ASM_OPTYPE_LOCAL_CONST:
	{
		PX_sprintf1(content, size, "LOCAL[%1]", PX_STRINGFORMAT_INT(param));
	}
	break;
	case PX_SCRIPT_ASM_OPTYPE_LOCAL_REGREF:
	{
		PX_sprintf1(content, size, "LOCAL[R%1]", PX_STRINGFORMAT_INT(param));
	}
	break;
	case PX_SCRIPT_ASM_OPTYPE_LOCAL_GLOBALREF:
	{
		PX_sprintf1(content, size, "GLOBAL[LOCAL[%1]]", PX_STRINGFORMAT_INT(param));
	}
	break;
	case PX_SCRIPT_ASM_OPTYPE_LOCAL_LOCALREF:
	{
		PX_sprintf1(content, size, "LOCAL[LOCAL[%1]]", PX_STRINGFORMAT_INT(param));
	}
	break;
	case PX_SCRIPT_ASM_OPTYPE_GLOBAL_CONST:
	{
		PX_sprintf1(content, size, "GLOBAL[%1]", PX_STRINGFORMAT_INT(param));
	}
	break;
	case PX_SCRIPT_ASM_OPTYPE_GLOBAL_SPREF:
	{
		PX_sprintf1(content, size, "GLOBAL[%1]", PX_STRINGFORMAT_INT(pT->SP + param));
	}
	break;
	case PX_SCRIPT_ASM_OPTYPE_GLOBAL_REGREF:
	{
		PX_sprintf1(content, size, "GLOBAL[R%1]", PX_STRINGFORMAT_INT(param));
	}
	break;
	case PX_SCRIPT_ASM_OPTYPE_GLOBAL_GLOBALREF:
	{
		PX_sprintf1(content, size, "GLOBAL[GLOBAL[%1]]", PX_STRINGFORMAT_INT(param));
	}
	break;
	case PX_SCRIPT_ASM_OPTYPE_GLOBAL_LOCALREF:
	{
		PX_sprintf1(content, size, "GLOBAL[LOCAL[%1]]", PX_STRINGFORMAT_INT(param));
	}
	break;

	default:
	{
		return PX_FALSE;
	}
	}
	return PX_TRUE;
}
px_void  PX_VMDebuggerGetParamName(PX_VM* Ins, px_char optype, px_int param, px_char content[], px_int size)
{
	PX_VM_Thread* pT = Ins->pThread + Ins->T;
	switch (optype)
	{
	case PX_SCRIPT_ASM_OPTYPE_IP:
	{
		PX_sprintf0(content, size, "IP");
	}
	break;

	case PX_SCRIPT_ASM_OPTYPE_SP:
	{
		PX_sprintf0(content, size, "SP");
	}
	break;

	case PX_SCRIPT_ASM_OPTYPE_BP:
	{
		PX_sprintf0(content, size, "BP");
	}
	break;
	case PX_SCRIPT_ASM_OPTYPE_HOST:
	case PX_SCRIPT_ASM_OPTYPE_INT:
	{
		PX_sprintf1(content, size, "%1", PX_STRINGFORMAT_INT(param));
	}
	break;
	case PX_SCRIPT_ASM_OPTYPE_FLOAT:
	{
		//Must be IEEE 754 format
		PX_sprintf1(content, size, "%1.6", PX_STRINGFORMAT_FLOAT(*(px_float *)&param));
	}
	break;
	case PX_SCRIPT_ASM_OPTYPE_STRING:
	{
		PX_sprintf1(content, size, "%1", PX_STRINGFORMAT_STRING(Ins->_string + param));
	}
	break;

	case PX_SCRIPT_ASM_OPTYPE_MEMORY:
	{
		PX_sprintf1(content, size, "memory ptr: %1", PX_STRINGFORMAT_INT(param));
	}
	break;

	case PX_SCRIPT_ASM_OPTYPE_REG:
	{
		PX_sprintf1(content, size, "R%1", PX_STRINGFORMAT_INT(param));
	}
	break;
	case PX_SCRIPT_ASM_OPTYPE_LOCAL_CONST:
	{
		PX_sprintf1(content, size, "LOCAL[%1]", PX_STRINGFORMAT_INT(param));
	}
	break;
	case PX_SCRIPT_ASM_OPTYPE_LOCAL_REGREF:
	{
		PX_sprintf1(content, size, "LOCAL[R%1]", PX_STRINGFORMAT_INT(param));
	}
	break;
	case PX_SCRIPT_ASM_OPTYPE_LOCAL_GLOBALREF:
	{
		PX_sprintf1(content, size, "GLOBAL[LOCAL[%1]]", PX_STRINGFORMAT_INT(param));
	}
	break;
	case PX_SCRIPT_ASM_OPTYPE_LOCAL_LOCALREF:
	{
		PX_sprintf1(content, size, "LOCAL[LOCAL[%1]]", PX_STRINGFORMAT_INT(param));
	}
	break;
	case PX_SCRIPT_ASM_OPTYPE_GLOBAL_CONST:
	{
		PX_sprintf1(content, size, "GLOBAL[%1]", PX_STRINGFORMAT_INT(param));
	}
	break;
	case PX_SCRIPT_ASM_OPTYPE_GLOBAL_SPREF:
	{
		PX_sprintf1(content, size, "GLOBAL[%1]", PX_STRINGFORMAT_INT(pT->SP + param));
	}
	break;
	case PX_SCRIPT_ASM_OPTYPE_GLOBAL_REGREF:
	{
		PX_sprintf1(content, size, "GLOBAL[R%1]", PX_STRINGFORMAT_INT(param));
	}
	break;
	case PX_SCRIPT_ASM_OPTYPE_GLOBAL_GLOBALREF:
	{
		PX_sprintf1(content, size, "GLOBAL[GLOBAL[%1]]", PX_STRINGFORMAT_INT(param));
	}
	break;
	case PX_SCRIPT_ASM_OPTYPE_GLOBAL_LOCALREF:
	{
		PX_sprintf1(content, size, "GLOBAL[LOCAL[%1]]", PX_STRINGFORMAT_INT(param));
	}
	break;

	default:
	{
		PX_VM_Error(Ins, "Unknow parameter");
	}
	}
}
px_int PX_VMDebuggerInstruction(PX_VM* Ins, px_int IP, px_char* opcode, px_char opcode_name[16], px_int* paramCount, px_char opType[3], px_int param[3])
{
	px_char opCode;
	px_int i;
	PX_VM_Thread* pT = &Ins->pThread[Ins->T];
	if (IP >= Ins->binsize)
	{
		return 0;
	}
	opCode = *(Ins->_bin + IP);
	*opcode = opCode;

	opType[0] = *(Ins->_bin + IP + 1);
	opType[1] = *(Ins->_bin + IP + 2);
	opType[2] = *(Ins->_bin + IP + 3);


	switch (opCode)
	{
	case PX_SCRIPT_ASM_INSTR_OPCODE_MOV:
	{
		PX_strset(opcode_name,"MOV");
		*paramCount = 2;
		for (i = 0; i < *paramCount; i++)
			param[i] = (*(((px_int32*)(Ins->_bin + IP + 4)) + i));
		return 4 + 2 * 4;
	}
	break;
	case PX_SCRIPT_ASM_INSTR_OPCODE_ADD:
	{
		PX_strset(opcode_name, "ADD");
		*paramCount = 2;
		for (i = 0; i < *paramCount; i++)
			param[i] = (*(((px_int32*)(Ins->_bin + IP + 4)) + i));
		return 4 + 2 * 4;
	}
	break;
	case PX_SCRIPT_ASM_INSTR_OPCODE_SUB:
	{
		PX_strset(opcode_name, "SUB");
		*paramCount = 2;
		for (i = 0; i < *paramCount; i++)
			param[i] = (*(((px_int32*)(Ins->_bin + IP + 4)) + i));
		return 4 + 2 * 4;
	}
	break;
	case PX_SCRIPT_ASM_INSTR_OPCODE_DIV:
	{
		PX_strset(opcode_name, "DIV");
		*paramCount = 2;
		for (i = 0; i < *paramCount; i++)
			param[i] = (*(((px_int32*)(Ins->_bin + IP + 4)) + i));
		return 4 + 2 * 4;
	}
	break;
	case PX_SCRIPT_ASM_INSTR_OPCODE_MUL:
	{
		PX_strset(opcode_name, "MUL");
		*paramCount = 2;
		for (i = 0; i < *paramCount; i++)
			param[i] = (*(((px_int32*)(Ins->_bin + IP + 4)) + i));
		return 4 + 2 * 4;
	}
	break;
	case PX_SCRIPT_ASM_INSTR_OPCODE_MOD:
	{
		PX_strset(opcode_name, "MOD");
		*paramCount = 2;
		for (i = 0; i < *paramCount; i++)
			param[i] = (*(((px_int32*)(Ins->_bin + IP + 4)) + i));
		return 4 + 2 * 4;
	}
	break;
	case PX_SCRIPT_ASM_INSTR_OPCODE_SHL:
	{
		PX_strset(opcode_name, "SHL");
		*paramCount = 2;
		for (i = 0; i < *paramCount; i++)
			param[i] = (*(((px_int32*)(Ins->_bin + IP + 4)) + i));
		return 4 + 2 * 4;
	}
	break;
	case PX_SCRIPT_ASM_INSTR_OPCODE_SHR:
	{
		PX_strset(opcode_name, "SHR");
		*paramCount = 2;
		for (i = 0; i < *paramCount; i++)
			param[i] = (*(((px_int32*)(Ins->_bin + IP + 4)) + i));
		return 4 + 2 * 4;
	}
	break;
	case PX_SCRIPT_ASM_INSTR_OPCODE_AND:
	{
		PX_strset(opcode_name, "AND");
		*paramCount = 2;
		for (i = 0; i < *paramCount; i++)
			param[i] = (*(((px_int32*)(Ins->_bin + IP + 4)) + i));
		return 4 + 2 * 4;
	}
	break;
	case PX_SCRIPT_ASM_INSTR_OPCODE_ANDL:
	{
		PX_strset(opcode_name, "ANDL");
		*paramCount = 2;
		for (i = 0; i < *paramCount; i++)
			param[i] = (*(((px_int32*)(Ins->_bin + IP + 4)) + i));
		return 4 + 2 * 4;
	}
	break;
	case PX_SCRIPT_ASM_INSTR_OPCODE_ORL:
	{
		PX_strset(opcode_name, "ORL");
		*paramCount = 2;
		for (i = 0; i < *paramCount; i++)
			param[i] = (*(((px_int32*)(Ins->_bin + IP + 4)) + i));
		return 4 + 2 * 4;
	}
	break;

	case PX_SCRIPT_ASM_INSTR_OPCODE_OR:
	{
		PX_strset(opcode_name, "OR");
		*paramCount = 2;
		for (i = 0; i < *paramCount; i++)
			param[i] = (*(((px_int32*)(Ins->_bin + IP + 4)) + i));
		return 4 + 2 * 4;
	}
	break;
	case PX_SCRIPT_ASM_INSTR_OPCODE_XOR:
	{
		PX_strset(opcode_name, "XOR");
		*paramCount = 2;
		for (i = 0; i < *paramCount; i++)
			param[i] = (*(((px_int32*)(Ins->_bin + IP + 4)) + i));
		return 4 + 2 * 4;
	}
	break;
	case PX_SCRIPT_ASM_INSTR_OPCODE_POW:
	{
		PX_strset(opcode_name, "POW");
		*paramCount = 2;
		for (i = 0; i < *paramCount; i++)
			param[i] = (*(((px_int32*)(Ins->_bin + IP + 4)) + i));
		return 4 + 2 * 4;
	}
	break;
	case PX_SCRIPT_ASM_INSTR_OPCODE_SIN:
	{
		PX_strset(opcode_name, "SIN");
		*paramCount = 2;
		for (i = 0; i < *paramCount; i++)
			param[i] = (*(((px_int32*)(Ins->_bin + IP + 4)) + i));
		return 4 + 2 * 4;
	}
	break;
	case PX_SCRIPT_ASM_INSTR_OPCODE_COS:
	{
		PX_strset(opcode_name, "COS");
		*paramCount = 2;
		for (i = 0; i < *paramCount; i++)
			param[i] = (*(((px_int32*)(Ins->_bin + IP + 4)) + i));
		return 4 + 2 * 4;
	}
	break;
	case PX_SCRIPT_ASM_INSTR_OPCODE_INT:
	{
		PX_strset(opcode_name, "INT");
		*paramCount = 1;
		for (i = 0; i < *paramCount; i++)
			param[i] = (*(((px_int32*)(Ins->_bin + IP + 4)) + i));
		return 4 + 1 * 4;
	}
	break;
	case PX_SCRIPT_ASM_INSTR_OPCODE_FLT:
	{
		PX_strset(opcode_name, "FLT");
		*paramCount = 1;
		for (i = 0; i < *paramCount; i++)
			param[i] = (*(((px_int32*)(Ins->_bin + IP + 4)) + i));
		return 4 + 1 * 4;
	}
	break;
	case PX_SCRIPT_ASM_INSTR_OPCODE_STRLEN:
	{
		PX_strset(opcode_name, "STRLEN");
		*paramCount = 2;
		for (i = 0; i < *paramCount; i++)
			param[i] = (*(((px_int32*)(Ins->_bin + IP + 4)) + i));
		return 4 + 2 * 4;
	}
	break;
	case PX_SCRIPT_ASM_INSTR_OPCODE_STRCAT:
	{
		PX_strset(opcode_name, "STRCAT");
		*paramCount = 2;
		for (i = 0; i < *paramCount; i++)
			param[i] = (*(((px_int32*)(Ins->_bin + IP + 4)) + i));
		return 4 + 2 * 4;
	}
	break;
	case PX_SCRIPT_ASM_INSTR_OPCODE_STRREP:
	{
		PX_strset(opcode_name, "STRREP");
		*paramCount = 3;
		for (i = 0; i < *paramCount; i++)
			param[i] = (*(((px_int32*)(Ins->_bin + IP + 4)) + i));
		return 4 + 3 * 4;
	}
	break;
	case PX_SCRIPT_ASM_INSTR_OPCODE_STRCHR:
	{
		PX_strset(opcode_name, "STRCHR");
		*paramCount = 3;
		for (i = 0; i < *paramCount; i++)
			param[i] = (*(((px_int32*)(Ins->_bin + IP + 4)) + i));
		return 4 + 3 * 4;
	}
	break;

	case PX_SCRIPT_ASM_INSTR_OPCODE_STRTOI:
	{
		PX_strset(opcode_name, "STRTOI");
		*paramCount = 2;
		for (i = 0; i < *paramCount; i++)
			param[i] = (*(((px_int32*)(Ins->_bin + IP + 4)) + i));
		return 4 + 2 * 4;
	}
	break;
	case PX_SCRIPT_ASM_INSTR_OPCODE_STRTOF:
	{
		PX_strset(opcode_name, "STRTOF");
		*paramCount = 2;
		for (i = 0; i < *paramCount; i++)
			param[i] = (*(((px_int32*)(Ins->_bin + IP + 4)) + i));
		return 4 + 2 * 4;
	}
	break;

	case PX_SCRIPT_ASM_INSTR_OPCODE_STRFRI:
	{
		PX_strset(opcode_name, "STRFRI");
		*paramCount = 2;
		for (i = 0; i < *paramCount; i++)
			param[i] = (*(((px_int32*)(Ins->_bin + IP + 4)) + i));
		return 4 + 2 * 4;
	}
	break;

	case PX_SCRIPT_ASM_INSTR_OPCODE_STRFRF:
	{
		PX_strset(opcode_name, "STRFRF");
		*paramCount = 2;
		for (i = 0; i < *paramCount; i++)
			param[i] = (*(((px_int32*)(Ins->_bin + IP + 4)) + i));
		return 4 + 2 * 4;
	}
	break;
	case PX_SCRIPT_ASM_INSTR_OPCODE_STRTMEM:
	{
		PX_strset(opcode_name, "STRMEM");
		*paramCount = 1;
		for (i = 0; i < *paramCount; i++)
			param[i] = (*(((px_int32*)(Ins->_bin + IP + 4)) + i));
		return 4 + 1 * 4;
	}
	break;

	case PX_SCRIPT_ASM_INSTR_OPCODE_STRSET:
	{
		PX_strset(opcode_name, "STRSET");
		*paramCount = 2;
		for (i = 0; i < *paramCount; i++)
			param[i] = (*(((px_int32*)(Ins->_bin + IP + 4)) + i));
		return 4 + 2 * 4;
	}
	break;

	case PX_SCRIPT_ASM_INSTR_OPCODE_STRFIND:
	{
		PX_strset(opcode_name, "STRFIND");
		*paramCount = 3;
		for (i = 0; i < *paramCount; i++)
			param[i] = (*(((px_int32*)(Ins->_bin + IP + 4)) + i));
		return 4 + 3 * 4;
	}
	break;

	case PX_SCRIPT_ASM_INSTR_OPCODE_ASC:
	{
		PX_strset(opcode_name, "ASC");
		*paramCount = 2;
		for (i = 0; i < *paramCount; i++)
			param[i] = (*(((px_int32*)(Ins->_bin + IP + 4)) + i));
		return 4 + 2 * 4;
	}
	break;

	case PX_SCRIPT_ASM_INSTR_OPCODE_MEMBYTE:
	{
		PX_strset(opcode_name, "MEMBYTE");
		*paramCount = 3;
		for (i = 0; i < *paramCount; i++)
			param[i] = (*(((px_int32*)(Ins->_bin + IP + 4)) + i));
		return 4 + 3 * 4;
	}
	break;

	case PX_SCRIPT_ASM_INSTR_OPCODE_MEMSET:
	{
		PX_strset(opcode_name, "MEMSET");
		*paramCount = 3;
		for (i = 0; i < *paramCount; i++)
			param[i] = (*(((px_int32*)(Ins->_bin + IP + 4)) + i));
		return 4 + 3 * 4;
	}
	break;

	case PX_SCRIPT_ASM_INSTR_OPCODE_MEMTRM:
	{
		PX_strset(opcode_name, "MEMTRM");
		*paramCount = 3;
		for (i = 0; i < *paramCount; i++)
			param[i] = (*(((px_int32*)(Ins->_bin + IP + 4)) + i));
		return 4 + 3 * 4;
	}
	break;

	case PX_SCRIPT_ASM_INSTR_OPCODE_MEMFIND:
	{
		PX_strset(opcode_name, "MEMFIND");
		*paramCount = 3;
		for (i = 0; i < *paramCount; i++)
			param[i] = (*(((px_int32*)(Ins->_bin + IP + 4)) + i));
		return 4 + 3 * 4;
	}
	break;

	case PX_SCRIPT_ASM_INSTR_OPCODE_MEMLEN:
	{
		PX_strset(opcode_name, "MEMLEN");
		*paramCount = 2;
		for (i = 0; i < *paramCount; i++)
			param[i] = (*(((px_int32*)(Ins->_bin + IP + 4)) + i));
		return 4 + 2 * 4;
	}
	break;

	case PX_SCRIPT_ASM_INSTR_OPCODE_MEMCAT:
	{
		PX_strset(opcode_name, "MEMCAT");
		*paramCount = 2;
		for (i = 0; i < *paramCount; i++)
			param[i] = (*(((px_int32*)(Ins->_bin + IP + 4)) + i));
		return 4 + 2 * 4;
	}
	break;
	case PX_SCRIPT_ASM_INSTR_OPCODE_MEMTSTR:
	{
		PX_strset(opcode_name, "MEMSTR");
		*paramCount = 2;
		for (i = 0; i < *paramCount; i++)
			param[i] = (*(((px_int32*)(Ins->_bin + IP + 4)) + i));
		return 4 + 1 * 4;
	}
	break;
	case PX_SCRIPT_ASM_INSTR_OPCODE_DATACPY:
	{
		PX_strset(opcode_name, "DATACPY");
		*paramCount = 3;
		for (i = 0; i < *paramCount; i++)
			param[i] = (*(((px_int32*)(Ins->_bin + IP + 4)) + i));
		return 4 + 3 * 4;
	}
	break;
	case PX_SCRIPT_ASM_INSTR_OPCODE_NEG:
	{
		PX_strset(opcode_name, "NEG");
		*paramCount = 1;
		for (i = 0; i < *paramCount; i++)
			param[i] = (*(((px_int32*)(Ins->_bin + IP + 4)) + i));
		return 4 + 1 * 4;
	}
	break;
	case PX_SCRIPT_ASM_INSTR_OPCODE_INV:
	{
		PX_strset(opcode_name, "INV");
		*paramCount = 1;
		for (i = 0; i < *paramCount; i++)
			param[i] = (*(((px_int32*)(Ins->_bin + IP + 4)) + i));
		return 4 + 1 * 4;
	}
	break;

	case PX_SCRIPT_ASM_INSTR_OPCODE_NOT:
	{
		PX_strset(opcode_name, "NOT");
		*paramCount = 1;
		for (i = 0; i < *paramCount; i++)
			param[i] = (*(((px_int32*)(Ins->_bin + IP + 4)) + i));
		return 4 + 1 * 4;
	}
	break;

	case PX_SCRIPT_ASM_INSTR_OPCODE_JMP:
	{
		PX_strset(opcode_name, "JMP");
		*paramCount = 1;
		for (i = 0; i < *paramCount; i++)
			param[i] = (*(((px_int32*)(Ins->_bin + IP + 4)) + i));
		return 4 + 1 * 4;
	}
	break;

	case PX_SCRIPT_ASM_INSTR_OPCODE_JE:
	{
		PX_strset(opcode_name, "JE");
		*paramCount = 3;
		for (i = 0; i < *paramCount; i++)
			param[i] = (*(((px_int32*)(Ins->_bin + IP + 4)) + i));
		return 4 + 3 * 4;
	}
	break;

	case PX_SCRIPT_ASM_INSTR_OPCODE_JNE:
	{
		PX_strset(opcode_name, "JNE");
		*paramCount = 3;
		for (i = 0; i < *paramCount; i++)
			param[i] = (*(((px_int32*)(Ins->_bin + IP + 4)) + i));
		return 4 + 3 * 4;
	}
	break;

	case PX_SCRIPT_ASM_INSTR_OPCODE_JL:
	{
		PX_strset(opcode_name, "JL");
		*paramCount = 3;
		for (i = 0; i < *paramCount; i++)
			param[i] = (*(((px_int32*)(Ins->_bin + IP + 4)) + i));
		return 4 + 3 * 4;

	}
	break;

	case PX_SCRIPT_ASM_INSTR_OPCODE_JLE:
	{
		PX_strset(opcode_name, "JLE");
		*paramCount = 3;
		for (i = 0; i < *paramCount; i++)
			param[i] = (*(((px_int32*)(Ins->_bin + IP + 4)) + i));
		return 4 + 3 * 4;

	}
	break;

	case PX_SCRIPT_ASM_INSTR_OPCODE_JG:
	{
		PX_strset(opcode_name, "JG");
		*paramCount = 3;
		for (i = 0; i < *paramCount; i++)
			param[i] = (*(((px_int32*)(Ins->_bin + IP + 4)) + i));
		return 4 + 3 * 4;

	}
	break;

	case PX_SCRIPT_ASM_INSTR_OPCODE_JGE:
	{
		PX_strset(opcode_name, "JGE");
		*paramCount = 3;
		for (i = 0; i < *paramCount; i++)
			param[i] = (*(((px_int32*)(Ins->_bin + IP + 4)) + i));
		return 4 + 3 * 4;
	}
	break;
	case PX_SCRIPT_ASM_INSTR_OPCODE_LGE:
	{
		PX_strset(opcode_name, "LGE");
		*paramCount = 3;
		for (i = 0; i < *paramCount; i++)
			param[i] = (*(((px_int32*)(Ins->_bin + IP + 4)) + i));
		return 4 + 3 * 4;
	}
	break;

	case PX_SCRIPT_ASM_INSTR_OPCODE_LGNE:
	{
		PX_strset(opcode_name, "LGNE");
		*paramCount = 3;
		for (i = 0; i < *paramCount; i++)
			param[i] = (*(((px_int32*)(Ins->_bin + IP + 4)) + i));
		return 4 + 3 * 4;
	}
	break;

	case PX_SCRIPT_ASM_INSTR_OPCODE_LGZ:
	{
		PX_strset(opcode_name, "LGZ");
		*paramCount = 1;
		for (i = 0; i < *paramCount; i++)
			param[i] = (*(((px_int32*)(Ins->_bin + IP + 4)) + i));
		return 4 + 1 * 4;
	}
	break;

	case PX_SCRIPT_ASM_INSTR_OPCODE_LGGZ:
	{
		PX_strset(opcode_name, "LGGZ");
		*paramCount = 1;
		for (i = 0; i < *paramCount; i++)
			param[i] = (*(((px_int32*)(Ins->_bin + IP + 4)) + i));
		return 4 + 1 * 4;
	}
	break;
	case PX_SCRIPT_ASM_INSTR_OPCODE_LGGEZ:
	{
		PX_strset(opcode_name, "LGGEZ");
		*paramCount = 1;
		for (i = 0; i < *paramCount; i++)
			param[i] = (*(((px_int32*)(Ins->_bin + IP + 4)) + i));
		return 4 + 1 * 4;
	}
	break;
	case PX_SCRIPT_ASM_INSTR_OPCODE_LGLZ:
	{
		PX_strset(opcode_name, "LGLZ");
		*paramCount = 1;
		for (i = 0; i < *paramCount; i++)
			param[i] = (*(((px_int32*)(Ins->_bin + IP + 4)) + i));
		return 4 + 1 * 4;
	}
	break;
	case PX_SCRIPT_ASM_INSTR_OPCODE_LGLEZ:
	{
		PX_strset(opcode_name, "LGLEZ");
		*paramCount = 1;
		for (i = 0; i < *paramCount; i++)
			param[i] = (*(((px_int32*)(Ins->_bin + IP + 4)) + i));
		return 4 + 1 * 4;
	}
	break;


	case PX_SCRIPT_ASM_INSTR_OPCODE_CALL:
	{
		PX_strset(opcode_name, "CALL");
		*paramCount = 1;
		for (i = 0; i < *paramCount; i++)
			param[i] = (*(((px_int32*)(Ins->_bin + IP + 4)) + i));
		return 4 + 1 * 4;
	}
	break;

	case PX_SCRIPT_ASM_INSTR_OPCODE_PUSH:
	{
		PX_strset(opcode_name, "PUSH");
		*paramCount = 1;
		for (i = 0; i < *paramCount; i++)
			param[i] = (*(((px_int32*)(Ins->_bin + IP + 4)) + i));
		return 4 + 1 * 4;
	}
	break;

	case PX_SCRIPT_ASM_INSTR_OPCODE_POP:
	{
		PX_strset(opcode_name, "POP");
		*paramCount = 1;
		for (i = 0; i < *paramCount; i++)
			param[i] = (*(((px_int32*)(Ins->_bin + IP + 4)) + i));
		return 4 + 1 * 4;
	}
	break;

	case PX_SCRIPT_ASM_INSTR_OPCODE_POPN:
	{
		PX_strset(opcode_name, "POPN");
		*paramCount = 1;
		for (i = 0; i < *paramCount; i++)
			param[i] = (*(((px_int32*)(Ins->_bin + IP + 4)) + i));
		return 4 + 1 * 4;
	}
	break;
	case PX_SCRIPT_ASM_INSTR_OPCODE_ADR:
	{
		PX_strset(opcode_name, "ADR");
		*paramCount = 1;
		for (i = 0; i < *paramCount; i++)
			param[i] = (*(((px_int32*)(Ins->_bin + IP + 4)) + i));
		return 4 + 1 * 4;
	}
	break;

	case PX_SCRIPT_ASM_INSTR_OPCODE_RET:
	{
		PX_strset(opcode_name, "RET");
		*paramCount = 0;
		for (i = 0; i < *paramCount; i++)
			param[i] = (*(((px_int32*)(Ins->_bin + IP + 4)) + i));
		return 4;
	}
	break;
	case PX_SCRIPT_ASM_INSTR_OPCODE_NOP:
	{
		PX_strset(opcode_name, "NOP");
		*paramCount = 0;
		for (i = 0; i < *paramCount; i++)
			param[i] = (*(((px_int32*)(Ins->_bin + IP + 4)) + i));
		return 4;
	}
	break;
	case PX_SCRIPT_ASM_INSTR_OPCODE_BPX:
	{
		PX_strset(opcode_name, "BPX");
		*paramCount = 1;
		for (i = 0; i < *paramCount; i++)
			param[i] = (*(((px_int32*)(Ins->_bin + IP + 4)) + i));
		return 8;
	}
	break;
	case PX_SCRIPT_ASM_INSTR_OPCODE_WAIT:
	{
		PX_strset(opcode_name, "WAIT");
		*paramCount = 1;
		for (i = 0; i < *paramCount; i++)
			param[i] = (*(((px_int32*)(Ins->_bin + IP + 4)) + i));
		return 8;
	}
	break;
	case PX_SCRIPT_ASM_INSTR_OPCODE_SIGNAL:
	{
		PX_strset(opcode_name, "SIGNAL");
		*paramCount = 2;
		for (i = 0; i < *paramCount; i++)
			param[i] = (*(((px_int32*)(Ins->_bin + IP + 4)) + i));
		return 4 + 2 * 4;
	}
	break;
	default:
		*paramCount = 0;
		PX_VM_Error(Ins, "unknow instruction");
	}
	*paramCount = 0;
	return 0;
}

px_int PX_VMDebuggerMapIpToLine(PX_VM_DebuggerMap* pDebugMap, px_int ip)
{
	px_int i;
	for (i=0;i<pDebugMap->map.size;i++)
	{
		PX_SCRIPT_ASM_SOURCE_MAP* pmap = PX_VECTORAT(PX_SCRIPT_ASM_SOURCE_MAP, &pDebugMap->map, i);
		if (pmap->instr_addr==ip)
		{
			return pmap->map_to_source_line;
		}
	}
	return -1;
}

px_int PX_VMDebuggerMapLineToIp(PX_VM_DebuggerMap* pDebugMap, px_int line)
{
	px_int i;
	for (i = 0; i < pDebugMap->map.size; i++)
	{
		PX_SCRIPT_ASM_SOURCE_MAP* pmap = PX_VECTORAT(PX_SCRIPT_ASM_SOURCE_MAP, &pDebugMap->map, i);
		if (pmap->map_to_source_line == line)
		{
			return pmap->instr_addr;
		}
	}
	return -1;
}

