#ifndef PIXELSES_MEMORYPOOL
#define PIXELSES_MEMORYPOOL
#include "PX_Typedef.h"

#define __PX_MEMORYPOOL_ALIGN_BYTES (sizeof(void *))
#define PX_MEMORYPOOL_DEBUG_CHECK


typedef enum
{
	PX_MEMORYPOOL_ERROR_OUTOFMEMORY,
	PX_MEMORYPOOL_ERROR_INVALID_ACCESS,
	PX_MEMORYPOOL_ERROR_INVALID_ADDRESS
}PX_MEMORYPOOL_ERROR;

typedef px_void (*PX_MP_ErrorCall)(PX_MEMORYPOOL_ERROR);

#if defined(PX_DEBUG_MODE) && defined(PX_MEMORYPOOL_DEBUG_CHECK)
typedef struct 
{
	px_void *addr;
	px_void *startAddr;
	px_void *endAddr;
}MP_alloc_debug;


typedef struct  
{
	px_dword append;
}MP_Append_data;
#define MP_APPENDDATA_MAGIC 0x31415926
#endif

typedef struct _memoryPool
{
	px_void *AllocAddr;
	px_void *StartAddr;
	px_void *EndAddr;
	px_uint32 Size;
	px_uint32 FreeSize;
	px_uint32 nodeCount;
	px_uint32 FreeTableCount;
	px_uint32 MaxMemoryfragSize;
	PX_MP_ErrorCall ErrorCall_Ptr;
#if defined(PX_DEBUG_MODE) && defined(PX_MEMORYPOOL_DEBUG_CHECK)
	MP_alloc_debug DEBUG_allocdata[1024];
	px_bool enable_allocdata_tracert;
#endif
}px_memorypool;


#if defined(PX_DEBUG_MODE) && defined(PX_MEMORYPOOL_DEBUG_CHECK)
px_void MP_UnreleaseInfo(px_memorypool *mp);
#define MP_DEBUG_AID(x) MP_UnreleaseInfo(x)
#else
#define MP_DEBUG_AID(x)
#endif


//Create a memory pool & return a MemoryPool structure
//MemoryAddr :Start address of memory
//MemorySize :Size of memory pool
px_memorypool	MP_Create	(px_void *MemoryAddr,px_uint MemorySize);

//Get memory size of Ptr
//Pool: Pool MemoryPool structure pointer
//Ptr: memory pointer
//Return - if succeeded return the size of Ptr,else return zero
px_uint MP_Size(px_memorypool *Pool,px_void *Ptr);

//Alloc a memory from memory pool
//Pool: Pool MemoryPool structure pointer
//Size: Size of alloc
//Return - if succeeded return the begin address of memories
//         if faith return null           
px_void		*MP_Malloc	(px_memorypool *Pool,px_uint Size);
#define		PX_Malloc(t,mp,s) ((t *)MP_Malloc(mp,s))

//Free the memory from memory pool
//Pool: Pool MemoryPool structure pointer
//pAddress: Pointer memory need to be free
px_void		MP_Free		(px_memorypool *Pool,px_void *pAddress);
#define     PX_Free		MP_Free
px_void		MP_Release	(px_memorypool *Pool);
px_void     MP_Reset    (px_memorypool *Pool);


//Register memoryPool error
px_void    MP_ErrorCatch(px_memorypool *Pool,PX_MP_ErrorCall ErrorCall);

#endif

