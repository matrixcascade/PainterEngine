#ifndef __PX_SCRIPT_VM_H
#define __PX_SCRIPT_VM_H
#include "PX_ScriptCore.h"
#define PX_SCRIPTVM_DEFALUT_FLOAT_PRECCISION 6
#define PX_SCRIPTVM_REG_COUNT 8
#define PX_SCRIPTVM_SIGNAL_COUNT 16
#define PX_SCRIPTVM_DEBUGGER_CMD_LEN 128
#define PX_SCRIPTVM_ATOM_INSTRUCTMENTS 64
#define PX_SCRIPTVM_TICK_INFINITE (-1)
typedef struct
{
	px_bool Activated;
	px_bool suspend;
	px_uint sleep;
	px_int	IP,SP,BP;
	PX_SCRIPTVM_VARIABLE R[PX_SCRIPTVM_REG_COUNT]; 
}PX_ScriptVM_InstanceThread;

typedef struct 
{
	px_int VM_Globalsize;
	px_int VM_Stacksize;
	px_int VM_memsize;

	px_int stringsize;
	px_int memorysize;
	px_int binsize;
	px_int funcCount;
	px_int hostCount;
	px_int maxThreadCount;
	
	px_bool signal[PX_SCRIPTVM_SIGNAL_COUNT];
	px_bool Suspend;

	px_int	T;

	px_byte * _bin;
	px_byte *_memory;
	px_char *_string;
	px_memorypool *mp;

	union
	{
		px_void    *user_ptr;
		px_int      user_int;
		px_float    user_float;
	};


	PX_SCRIPT_EXPORT_FUNCTION *_func;
	PX_SCRIPT_ASM_HOST_NODE *_host;
	PX_SCRIPTVM_VARIABLE *_mem;
	PX_ScriptVM_InstanceThread *pThread;

	px_bool debug;
	px_bool bp_next;
	px_int  bp_IP;

}PX_ScriptVM_Instance;

typedef struct
{
	px_int ptr;
	px_int size;
}PX_SCRIPTVM_MEMORY_PTR;

typedef enum
{
	PX_SCRIPTVM_RUNRETURN_ERROR =0,
	PX_SCRIPTVM_RUNRETURN_TIMEOUT,
	PX_SCRIPTVM_RUNRETURN_END,
	PX_SCRIPTVM_RUNRETURN_SUSPEND,
	PX_SCRIPTVM_RUNRETURN_WAIT,
}PX_SCRIPTVM_RUNRETURN;

typedef px_bool (*PX_ScriptVM_Function_Modules)(PX_ScriptVM_Instance *Ins,px_void *userptr);

px_int PX_ScriptVM_GetFunctionIndex(PX_ScriptVM_Instance *Ins,px_char *func);

px_bool             PX_ScriptVM_InstanceThreadSwitch(PX_ScriptVM_Instance *Ins,px_int T);
px_bool				PX_ScriptVM_InstanceRunFunction(PX_ScriptVM_Instance *Ins,px_int threadID,const px_char *functionName,PX_SCRIPTVM_VARIABLE args[],px_int paramcount);
px_bool				PX_ScriptVM_InstanceRunFunctionIndex(PX_ScriptVM_Instance *Ins,px_int threadID,px_int funcIndex,PX_SCRIPTVM_VARIABLE args[],px_int paramcount);
px_bool				PX_ScriptVM_InstanceBeginThreadFunction(PX_ScriptVM_Instance *Ins,px_int threadID,const px_char *functionName,PX_SCRIPTVM_VARIABLE args[],px_int paramcount);
px_bool				PX_ScriptVM_InstanceBeginThreadFunctionIndex(PX_ScriptVM_Instance *Ins,px_int threadID,px_int funcIndex,PX_SCRIPTVM_VARIABLE args[],px_int paramcount);
px_int				PX_ScriptVM_InstanceGetFreeThreadId(PX_ScriptVM_Instance *Ins);
		
PX_SCRIPTVM_RUNRETURN PX_ScriptVM_InstanceRunThread(PX_ScriptVM_Instance *Ins,px_int tick);


px_void				PX_ScriptVM_InstanceRun(PX_ScriptVM_Instance *Ins,px_int tick);
px_void				PX_ScriptVM_InstanceRunTime(PX_ScriptVM_Instance* Ins, px_int tick,px_dword elapsed);
px_void				PX_ScriptVM_ThreadStop(PX_ScriptVM_Instance *Ins,px_int ThreadId);
px_void				PX_ScriptVM_ThreadClear(PX_ScriptVM_Instance *Ins,px_int ThreadId);
px_void				PX_ScriptVM_ThreadSuspend(PX_ScriptVM_Instance *Ins,px_int ThreadId);
px_void				PX_ScriptVM_ThreadResume(PX_ScriptVM_Instance *Ins,px_int ThreadId);

px_bool PX_ScriptVM_InstanceIsRuning(PX_ScriptVM_Instance* Ins);
px_bool PX_ScriptVM_InstanceInitialize(PX_ScriptVM_Instance* Ins, px_memorypool* mp, px_byte* code, px_int size);
px_bool PX_ScriptVM_LocalAlloc(PX_ScriptVM_Instance *Ins,int size,PX_SCRIPTVM_MEMORY_PTR *mem_ptr);
px_bool PX_ScriptVM_LocalFree(PX_ScriptVM_Instance *Ins,PX_SCRIPTVM_MEMORY_PTR *mem_ptr);
px_bool PX_ScriptVM_RegistryHostFunction(PX_ScriptVM_Instance *Ins,const px_char *name,PX_ScriptVM_Function_Modules funcModules,px_void *userptr);
px_bool PX_ScriptVM_InstanceFree(PX_ScriptVM_Instance *Ins);

#define  PX_ScriptVM_STACK(Ins,i) ((Ins)->_mem[(Ins)->pThread[(Ins)->T].SP+i])
#define  PX_ScriptVM_HOSTPARAM(Ins,i) PX_ScriptVM_STACK(Ins,i)
#define  PX_ScriptVM_LOCALPARAM(Ins,i) ((Ins)->_mem[(Ins)->pThread[(Ins)->T].BP-i])
#define  PX_ScriptVM_GLOBAL(Ins,i) ((Ins)->_mem[i])
#define  PX_ScriptVM_RETURN_POINTER(Ins,oft) ((Ins)->_mem[((Ins)->pThread[(Ins)->T].R[1])._int+(oft)])
#define  PX_ScriptVM_REG_RETURN(Ins)   ((Ins)->pThread[(Ins)->T].R[1])

px_void  PX_ScriptVM_POPN(PX_ScriptVM_Instance *Ins,px_int n,px_int Threadid);
px_void  PX_ScriptVM_RET(PX_ScriptVM_Instance *Ins,PX_SCRIPTVM_VARIABLE ret);
px_void  PX_ScriptVM_PUSH(PX_ScriptVM_Instance *Ins,PX_SCRIPTVM_VARIABLE val);
PX_SCRIPTVM_VARIABLE PX_ScriptVM_Variable_int(px_int _int);
PX_SCRIPTVM_VARIABLE PX_ScriptVM_Variable_float(px_float _float);
PX_SCRIPTVM_VARIABLE PX_ScriptVM_Variable_string(px_string _ref_string);
PX_SCRIPTVM_VARIABLE PX_ScriptVM_Variable_memory(px_byte *buffer,px_int _size);
PX_SCRIPTVM_VARIABLE PX_ScriptVM_Variable_const_string(px_char *buffer);
PX_SCRIPTVM_VARIABLE PX_ScriptVM_Variable_const_memory(px_byte *buffer,px_int _size);
PX_SCRIPTVM_VARIABLE PX_SCRIPTVM_VaribaleCopy(PX_ScriptVM_Instance *Ins,PX_SCRIPTVM_VARIABLE var,px_bool *bOutofMemory);
typedef px_int (* PX_ScriptVM_DebuggerPrint)(px_char fmr[],...);
typedef px_int (* PX_ScriptVM_DebuggerCommand)(px_char cmd[],px_int size);

px_int PX_ScriptVM_DebuggerEnable(PX_ScriptVM_Instance *Ins,PX_ScriptVM_DebuggerPrint _printFunc,PX_ScriptVM_DebuggerCommand _cmdFunc);


#endif
