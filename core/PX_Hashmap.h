
#ifndef PX_HASHMAP_H__
#define PX_HASHMAP_H__

#include "PX_MemoryPool.h"
#include "PX_rbtree.h"

#define PX_HASHMAP_STRINGKEY_LEN 64


typedef enum
{
	PX_HASHMAP_RETURN_OK,
	PX_HASHMAP_RETURN_NULL,
	PX_HASHMAP_RETURN_KEYEXIST,
	PX_HASHMAP_RETURN_OUTOFMEMORY,
	PX_HASHMAP_RETURN_FULL,
}PX_HASHMAP_RETURN;



typedef struct 
{
	px_memorypool *mp;
	px_int size;
	struct px_rb_root root;
}px_map;

typedef struct		__px_map_element
{
	struct px_rb_node node;//must be first member
	px_char		string[PX_HASHMAP_STRINGKEY_LEN];
	union
	{
		px_void		*Ptr;
		px_int      _int;
		px_float    _float;
	};

}px_map_element;



px_bool PX_MapInitialize(px_memorypool *mp,px_map *hashmap);

PX_HASHMAP_RETURN PX_MapPut(px_map * m,const px_char stringkey[], px_void * value);
PX_HASHMAP_RETURN PX_MapPutInt(px_map * m,const px_char stringkey[], px_int value);
PX_HASHMAP_RETURN PX_MapPutFloat(px_map * m,const px_char stringkey[], px_float value);
PX_HASHMAP_RETURN PX_MapErase(px_map * m,const px_char stringkey[]);
px_map_element *  PX_MapFirst(px_map * m);
px_map_element *  PX_MapNext(px_map * m,px_map_element *node);
px_void *PX_MapGet(px_map * m, const px_char stringkey[]);
px_bool PX_MapGetInt(px_map * m, const px_char stringkey[],px_int *_int);
px_bool PX_MapGetFloat(px_map * m, const px_char stringkey[],px_float *_float);
px_void PX_MapFree(px_map * m);
px_int PX_MapGetSize(px_map * m);


#endif
