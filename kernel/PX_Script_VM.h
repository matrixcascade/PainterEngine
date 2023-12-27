#ifndef __PX_SCRIPT_VM_H
#define __PX_SCRIPT_VM_H
#include "PX_ScriptCore.h"
#define PX_VM_DEFALUT_FLOAT_PRECCISION 6
#define PX_VM_REG_COUNT 8
#define PX_VM_SIGNAL_COUNT 16
#define PX_VM_ATOM_INSTRUCTMENTS 64
#define PX_VM_TICK_INFINITE (-1)
#define PX_VM_MAX_BREAKPOINT_COUNT 32
typedef struct
{
	px_bool Activated;
	px_bool suspend;
	px_uint sleep;
	px_int	IP,SP,BP;
	PX_VM_VARIABLE R[PX_VM_REG_COUNT]; 
}PX_VM_Thread;

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
	
	px_bool signal[PX_VM_SIGNAL_COUNT];
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


	PX_VM_EXPORT_FUNCTION *_func;
	PX_ASM_HOST_NODE *_host;
	PX_VM_VARIABLE *_mem;
	PX_VM_Thread *pThread;

	px_bool debug;
	px_bool breakpoint_next;
	px_int  breakpoints[PX_VM_MAX_BREAKPOINT_COUNT];

}PX_VM;




typedef struct
{
	px_int ptr;
	px_int size;
}PX_VM_MEMORY_PTR;

typedef enum
{
	PX_VM_RUNRETURN_ERROR =0,
	PX_VM_RUNRETURN_TIMEOUT,
	PX_VM_RUNRETURN_END,
	PX_VM_RUNRETURN_SUSPEND,
	PX_VM_RUNRETURN_WAIT,
}PX_VM_RUNRETURN;

typedef px_bool (*PX_VM_Function_Modules)(PX_VM *Ins,px_void *userptr);
#define PX_VM_HOST_FUNCTION(name) px_bool name(PX_VM *Ins,px_void *userptr)

px_int				PX_VM_GetFunctionIndex(PX_VM *Ins,px_char *func);
px_bool             PX_VMThreadSwitch(PX_VM *Ins,px_int T);
px_bool				PX_VMRunFunction(PX_VM *Ins,px_int threadID,const px_char *functionName,PX_VM_VARIABLE args[],px_int paramcount);
px_bool				PX_VMCallFunction(PX_VM* Ins, px_int threadID, const px_char* functionName, PX_VM_VARIABLE args[], px_int paramcount);
px_bool				PX_VMRunFunctionIndex(PX_VM* Ins, px_int threadID, px_int funcIndex, PX_VM_VARIABLE args[], px_int paramcount);
px_int				PX_VMGetFreeThread(PX_VM* Ins, px_int reservedThread);
px_bool				PX_VMBeginThreadFunction(PX_VM *Ins,px_int threadID,const px_char *functionName,PX_VM_VARIABLE args[],px_int paramcount);
px_bool				PX_VMBeginThreadFunctionIndex(PX_VM *Ins,px_int threadID,px_int funcIndex,PX_VM_VARIABLE args[],px_int paramcount);
px_int				PX_VMGetFreeThreadId(PX_VM *Ins);
px_void				PX_VMSleep(PX_VM* Ins, px_int thread, px_uint sleepms);

px_void				PX_VMRun(PX_VM* Ins, px_int tick, px_dword elapsed);
px_void				PX_VMTriggerBreakPoint(PX_VM* Ins, px_int IP);
px_void				PX_VMAddBreakPoint(PX_VM* Ins, px_int IP);
px_void				PX_VMRemoveBreakPoint(PX_VM* Ins, px_int IP);

px_void				PX_VMSuspend(PX_VM* Ins);
px_void				PX_VMContinue(PX_VM* Ins);
px_void				PX_VMDebugContinue(PX_VM* Ins);
px_void				PX_VMReset(PX_VM* Ins);
px_void				PX_VMThreadStop(PX_VM* Ins, px_int ThreadId);
px_void				PX_VMThreadClear(PX_VM *Ins,px_int ThreadId);
px_void				PX_VMThreadSuspend(PX_VM *Ins,px_int ThreadId);
px_void				PX_VMThreadResume(PX_VM *Ins,px_int ThreadId);


px_bool PX_VMIsRuning(PX_VM* Ins);
px_bool PX_VMInitialize(PX_VM* Ins, px_memorypool* mp, const px_byte* code, px_int size);
px_bool PX_VMLocalAlloc(PX_VM *Ins,px_int size,PX_VM_MEMORY_PTR *mem_ptr);
px_bool PX_VMLocalFree(PX_VM *Ins,PX_VM_MEMORY_PTR *mem_ptr);
px_bool PX_VMRegistHostFunction(PX_VM *Ins,const px_char *name,PX_VM_Function_Modules funcModules,px_void *userptr);
px_bool PX_VMFree(PX_VM *Ins);

#define  PX_VM_STACK_THREAD(Ins,i,T) ((Ins)->_mem[(Ins)->pThread[T].SP+i])
#define  PX_VM_STACK(Ins,i) ((Ins)->_mem[(Ins)->pThread[(Ins)->T].SP+i])
#define  PX_VM_HOSTPARAM(Ins,i) PX_VM_STACK(Ins,i)
#define  PX_VM_LOCALPARAM(Ins,i) ((Ins)->_mem[(Ins)->pThread[(Ins)->T].BP-i])
#define  PX_VM_GLOBAL(Ins,i) ((Ins)->_mem[i])
#define  PX_VM_RETURN_POINTER(Ins,oft) ((Ins)->_mem[((Ins)->pThread[(Ins)->T].R[1])._int+(oft)])
#define  PX_VM_REG_RETURN(Ins)   ((Ins)->pThread[(Ins)->T].R[1])

px_void  PX_VM_POPN(PX_VM *Ins,px_int n,px_int Threadid);
px_void  PX_VM_RET(PX_VM *Ins,PX_VM_VARIABLE ret);
px_void	 PX_VM_RET_String(PX_VM* Ins, const px_char* pstr);
px_void  PX_VM_RET_int(PX_VM* Ins, px_int _int);
px_void PX_VM_RET_ptr(PX_VM* Ins, px_void* ptr);
#define PX_VM_RET_handle(Ins,ptr) PX_VM_RET_ptr(Ins,ptr);
px_void  PX_VM_RET_float(PX_VM* Ins, px_float _float);
px_void  PX_VM_RET_memory(PX_VM* Ins, const px_byte* data, px_int size);
px_void  PX_VM_PUSH(PX_VM* Ins, PX_VM_VARIABLE val);
px_void  PX_VM_Sleep(PX_VM* Ins, px_uint sleep);
px_void  PX_VM_ThreadSleep(PX_VM* Ins,px_int threadid, px_uint sleep);
PX_VM_VARIABLE PX_VM_Variable_int(px_int _int);
PX_VM_VARIABLE PX_VM_Variable_handle(px_void* _ptr);
PX_VM_VARIABLE PX_VM_Variable_float(px_float _float);
PX_VM_VARIABLE PX_VM_Variable_string(px_string _ref_string);
PX_VM_VARIABLE PX_VM_Variable_memory(px_byte *buffer,px_int _size);
PX_VM_VARIABLE PX_VM_Variable_const_string(const px_char *buffer);
PX_VM_VARIABLE PX_VM_Variable_const_memory(const px_byte *buffer,px_int _size);
PX_VM_VARIABLE PX_VM_VaribaleCopy(PX_VM *Ins,PX_VM_VARIABLE var,px_bool *bOutofMemory);
PX_VM_VARIABLE*	PX_VMGetVariablePointer(PX_VM* Ins, px_char optype, px_int param);
px_bool				PX_VMDebuggerMapInitialize(px_memorypool* mp, PX_VM_DebuggerMap* pDebugMap);
px_void				PX_VMDebuggerMapFree(PX_VM_DebuggerMap* pDebugMap);
px_int				PX_VMDebuggerMapIpToLine(PX_VM_DebuggerMap* pDebugMap, px_int ip);
px_int				PX_VMDebuggerMapLineToIp(PX_VM_DebuggerMap* pDebugMap, px_int line);
px_bool				PX_VMDebuggerGetVarParamName(PX_VM* Ins, px_char optype, px_int param, px_char content[], px_int size);
px_void				PX_VMDebuggerGetParamName(PX_VM* Ins, px_char optype, px_int param, px_char content[], px_int size);
px_int				PX_VMDebuggerInstruction(PX_VM* Ins, px_int IP, px_char* opcode, px_char opcode_name[16], px_int* paramCount, px_char opType[3], px_int param[3]);
#endif
