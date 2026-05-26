#ifndef PX_VMC_H 
#define PX_VMC_H
#include "PX_Syntax.h"
#include "PX_Syntax_source.h"

typedef enum
{
	PX_MACHINE_STATE_IDLE = 0,
	PX_MACHINE_STATE_RUNNING,
	PX_MACHINE_STATE_PAUSE,
	PX_MACHINE_STATE_ERROR
}PX_MACHINE_STATE;

//common register r0-r3
//sp, bp, ip, flag ,module_base, sleep,

typedef struct
{
	PX_MACHINE_STATE state;
	union 
	{
		px_dword r[12];
		struct
		{
			px_dword r0,r1,r2,r3,ip, sp, bp;
			union
			{
				px_dword flag;
				struct
				{
					px_dword Z : 1;//(zero)
					px_dword N : 1;//(sign)
					px_dword C : 1;//(carry)
					px_dword V : 1;//(overflow)
					px_dword C0 : 1;//ST0>STi 000
					px_dword C2 : 1;//ST0<STi 100
					px_dword C3 : 1;//ST0==STi 111
					px_dword T : 1;//step trap
					px_dword reserved : 24;
				};
				px_dword sleep,module_base;
			};
		};
		
	};
	px_float32 f[4];
}PX_Machine_Thread;


typedef struct
{
	px_memorypool* mp;
	PX_MACHINE_STATE state;
	px_string  message;
	px_vector  threads;//PX_Machine_Thread
	px_int     T;//current thread index
	px_byte*   runtime_memory;
	px_uint    runtime_memory_size;
	px_uint    thread_stack_size;
	px_uint    ip_breakpoint[32];//-1 for unused
	px_uint    memory_breakpoint[2];
	px_uint    memory_breakpoint_size[2];//0 for unused
	px_byte    memory_breakpoint_mask[2];//1 for read, 2 for write
	px_dword   last_request_id;
}PX_Machine;

px_bool PX_Machine_Initialize(px_memorypool* mp, PX_Machine* pMachine);
px_void PX_Machine_Free(PX_Machine* pMachine);
px_void PX_Machine_Update(PX_Machine* pmac, px_int tick, px_dword elapsed);

px_bool PX_Machine_Execute(PX_Machine* pMachine, const px_byte abi_payload[], px_int payload_size, px_abi* abi_writer_return);
px_void PX_Machine_Message(PX_Machine* pMachine, const px_char* message);
px_string* PX_Machine_GetMessage(PX_Machine* pMachine);
#endif