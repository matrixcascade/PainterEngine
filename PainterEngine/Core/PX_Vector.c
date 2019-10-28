#include "PX_Vector.h"

px_bool PX_VectorInit(px_memorypool *mp,px_vector *vec,px_int nodeSize,px_int init_size)
{
	vec->allocsize=init_size;
	vec->nodesize=nodeSize;
	vec->size=0;
	vec->mp=mp;


	if (init_size==0)
	{
		vec->data=PX_NULL;
		return PX_TRUE;
	}

	if(!(vec->data=MP_Malloc(mp,init_size*nodeSize)))
		return PX_FALSE;
	
	return PX_TRUE;
}


px_bool PX_VectorSet(px_vector *vec,px_uint index,px_void *data)
{
	px_void *old;
	px_uint allocsize=1;
	if (index>(px_uint)vec->allocsize-1)
	{
		while(allocsize<=(px_uint)index) allocsize<<=1;

		old=vec->data;
		vec->data=MP_Malloc(vec->mp,allocsize*vec->nodesize);
		if (vec->data==PX_NULL)
		{
			return PX_FALSE;
		}
		PX_memcpy(vec->data,old,vec->allocsize*vec->nodesize);
		vec->allocsize=allocsize;
		MP_Free(vec->mp,old);
	}
	if (index>(px_uint)vec->size-1)
	{
		vec->size=index+1;
	}
	PX_memcpy((px_byte *)vec->data+index*vec->nodesize,data,vec->nodesize);
	return PX_TRUE;
}


px_bool PX_VectorPushback(px_vector *vec,px_void *data)
{
	px_void *old;
	if (vec->size<vec->allocsize)
	{
		PX_memcpy((px_byte *)vec->data+vec->size*vec->nodesize,data,vec->nodesize);
		vec->size++;
	}
	else
	{
		px_int allocSize=vec->allocsize;
		if (allocSize==0)
		{
			allocSize=2;
		}
		else
		{
			allocSize=vec->allocsize*2;
		}

		old=vec->data;
		vec->data=MP_Malloc(vec->mp,allocSize*vec->nodesize);
		if (vec->data==PX_NULL)
		{
			return PX_FALSE;
		}
		PX_memcpy(vec->data,old,vec->allocsize*vec->nodesize);
		vec->allocsize=allocSize;
		PX_memcpy((px_byte *)vec->data+vec->size*vec->nodesize,data,vec->nodesize);
		vec->size++;
		if(old)
		MP_Free(vec->mp,old);
	}
	return PX_TRUE;
}

px_bool PX_VectorErase(px_vector *vec,px_int index)
{
	if (index>=vec->size)
	{
		PX_ERROR("Vector Erase Error!");
		return PX_FALSE;
	}
	PX_memcpy((px_byte *)vec->data+index*vec->nodesize,(px_byte *)vec->data+(index+1)*vec->nodesize,(vec->size-index-1)*vec->nodesize);
	vec->size--;
	return PX_TRUE;
}

px_bool PX_VectorPop(px_vector *vec)
{
	vec->size--;
	return PX_TRUE;
}

px_bool PX_VectorAllocSize(px_vector *vec,px_int size)
{
	if (size<vec->size)
	{
		return PX_FALSE;
	}
	vec->size=size;
	return PX_TRUE;
}


