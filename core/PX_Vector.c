#include "PX_Vector.h"

px_bool PX_VectorInitialize(px_memorypool *mp,px_vector *vec,px_int nodeSize,px_int init_size)
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
		if (data)
		{
			PX_memcpy((px_byte*)vec->data + vec->size * vec->nodesize, data, vec->nodesize);
		}
		else
		{
			PX_memset((px_byte*)vec->data + vec->size * vec->nodesize, 0, vec->nodesize);
		}
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

		if (data)
		{
			PX_memcpy((px_byte*)vec->data + vec->size * vec->nodesize, data, vec->nodesize);
		}
		else
		{
			PX_memset((px_byte*)vec->data + vec->size * vec->nodesize, 0, vec->nodesize);
		}
		vec->size++;
		if(old)
		MP_Free(vec->mp,old);
	}
	return PX_TRUE;
}


px_bool PX_VectorPushTo(px_vector *vec,px_void *data,px_int index)
{
	if(!PX_VectorPushback(vec,data)) return PX_FALSE;
	if (index<0||index>=vec->size)
	{
		return PX_TRUE;
	}
#ifndef PX_DEBUG_MODE
	if (index<0)
	{
		PX_ASSERT();
		return PX_FALSE;
	}
#endif
	PX_memcpy((px_byte *)vec->data+(index+1)*vec->nodesize,(px_byte *)vec->data+(index)*vec->nodesize,(vec->size-index-1)*vec->nodesize);
	PX_memcpy((px_byte *)vec->data+(index)*vec->nodesize,data,vec->nodesize);
	return PX_TRUE;
}

px_bool PX_VectorErase(px_vector *vec,px_int index)
{
	px_int i;
	if (index<0||index>=vec->size)
	{
		PX_ERROR("Vector Erase Error!");
		return PX_FALSE;
	}
	for (i=index;i<vec->size-1;i++)
	{
		PX_memcpy((px_byte *)vec->data+i*vec->nodesize,(px_byte *)vec->data+(i+1)*vec->nodesize,vec->nodesize);
	}
	vec->size--;
	return PX_TRUE;
}

px_bool PX_VectorPop(px_vector *vec)
{
	if (vec->size==0)
	{
		return PX_FALSE;
	}
	vec->size--;
	return PX_TRUE;
}


px_bool PX_VectorCopy(px_vector *destvec,px_vector *resvec)
{
	if (destvec->nodesize!=resvec->nodesize)
	{
		return PX_FALSE;
	}
	if (destvec->allocsize<resvec->allocsize)
	{
		PX_VectorResize(destvec,resvec->allocsize);
	}
	PX_memcpy(destvec->data,resvec->data,resvec->nodesize*resvec->size);
	destvec->size=resvec->size;
	return PX_TRUE;
}


px_void PX_VectorFree(px_vector *vec)
{
	if((vec->data)!=PX_NULL) 
		MP_Free(vec->mp,vec->data);
}


px_bool PX_VectorResize(px_vector *vec,px_int size)
{
	if (size!=vec->size)
	{
		PX_VectorFree(vec);
		if(!PX_VectorInitialize(vec->mp,vec,vec->nodesize,size))return PX_FALSE;
		PX_memset(vec->data,0,vec->nodesize*vec->allocsize);
		vec->size=size;
		return PX_TRUE;
	}
	return PX_FALSE;
}

px_bool PX_VectorCheckIndex(px_vector *vec,px_int index)
{
	if (index<0||index>=vec->size)
	{
		return PX_FALSE;
	}
	return PX_TRUE;
}

px_bool PX_VectorAllocSize(px_vector *vec,px_int size)
{
	if (size<0||size<vec->size)
	{
		return PX_FALSE;
	}
	vec->size=size;
	return PX_TRUE;
}


