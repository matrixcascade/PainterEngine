/*
 * Generic map implementation.
 */
#include "PX_Hashmap.h"


px_bool PX_MapInitialize(px_memorypool *mp,px_map *m)
{
	m->mp=mp;
	return PX_RBTreeInitialize(mp, &m->tree);
}



PX_HASHMAP_RETURN PX_MapPut(px_map * m,const px_byte code[],px_int codesize, px_void * value)
{
	if(PX_RBTreePutPtr(&m->tree, code, codesize,value))
	{
		return PX_HASHMAP_RETURN_OK;
	}
	else
	{
		return PX_HASHMAP_RETURN_NULL;
	}
}

PX_HASHMAP_RETURN PX_MapPutInt(px_map* m, const px_byte code[], px_int codesize, px_int value)
{
	if (PX_RBTreePutInt(&m->tree,code, codesize, value))
	{
		return PX_HASHMAP_RETURN_OK;
	}
	else
	{
		return PX_HASHMAP_RETURN_NULL;
	}
}

PX_HASHMAP_RETURN PX_MapPutFloat(px_map * m, const px_byte code[], px_int codesize, px_float value)
{
	if (PX_RBTreePutFloat(&m->tree, code,codesize, value))
	{
		return PX_HASHMAP_RETURN_OK;
	}
	else
	{
		return PX_HASHMAP_RETURN_NULL;
	}
}



px_void PX_MapFree(px_map * m)
{
	PX_RBTreeFree(&m->tree);
}

px_int PX_MapGetSize(px_map * m)
{
	return m->tree.size;
}

PX_RBNode* PX_MapFirst(px_map* m)
{
	return PX_RBTreeFirst(&m->tree);
}
PX_RBNode* PX_MapNext(PX_RBNode* node)
{
	return PX_RBTreeNext(node);
}


PX_HASHMAP_RETURN PX_MapErase(px_map * m, const px_byte code[], px_int codesize)
{
	if (PX_RBTreeDelete(&m->tree, code,codesize))
	{
		return PX_HASHMAP_RETURN_OK;
	}
	else
	{
		return PX_HASHMAP_RETURN_NULL;
	}
}

px_void* PX_MapGet(px_map* m, const px_byte code[],px_int codesize)
{
	px_void* ptr;
	if (!PX_RBTreeGetPtr(&m->tree, code,codesize, &ptr))
		return PX_NULL;
	return ptr;
}

px_bool PX_MapGetInt(px_map * m, const px_byte code[], px_int codesize,px_int *v)
{
	return PX_RBTreeGetInt(&m->tree, code,codesize, v);
}


px_bool PX_MapGetFloat(px_map * m, const px_byte code[], px_int codesize, px_float *v)
{
	return PX_RBTreeGetFloat(&m->tree, code,codesize, v);
}
