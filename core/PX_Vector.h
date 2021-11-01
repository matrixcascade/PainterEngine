#ifndef __PX_VECTOR_H
#define __PX_VECTOR_H
#include "PX_MemoryPool.h"
typedef struct __px_vector
{
	px_void *data;
	px_int nodesize;
	px_int size;
	px_int allocsize;
	px_memorypool *mp;
}px_vector;

px_bool PX_VectorInitialize(px_memorypool *mp,px_vector *vec,px_int nodeSize,px_int init_size); 
px_bool PX_VectorSet(px_vector *vec,px_uint index,px_void *data);
px_bool PX_VectorAllocSize(px_vector *vec,px_int size);
px_bool PX_VectorPushback(px_vector *vec,px_void *data);
px_bool PX_VectorPushTo(px_vector *vec,px_void *data,px_int index);
px_bool PX_VectorErase(px_vector *vec,px_int index);
px_bool PX_VectorPop(px_vector *vec);
px_bool PX_VectorCopy(px_vector *destvec,px_vector *resvec);
px_void PX_VectorFree(px_vector *vec);
px_bool PX_VectorResize(px_vector *vec,px_int size);
px_bool PX_VectorCheckIndex(px_vector *vec,px_int index);
#define PX_VectorSize(x) ((x)->size)

#ifdef PX_DEBUG_MODE
#define PX_VECTORAT(t,vec,i) (sizeof(t)==(vec)->nodesize?((t *)((px_byte *)((vec)->data)+(vec)->nodesize*(i))):PX_NULL)
#else
#define PX_VECTORAT(t,vec,i) ((t *)((px_byte *)((vec)->data)+(vec)->nodesize*(i)))
#endif

#define PX_VECTORLAST(t,vec) PX_VECTORAT(t,vec,(vec)->size-1)

#define PX_VectorClear(vec) ((vec)->size=0)

#endif
