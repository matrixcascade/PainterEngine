#ifndef __PX_LIST_H
#define __PX_LIST_H
#include "PX_MemoryPool.h"
typedef struct __px_list_node
{
	px_void *pdata;
	struct __px_list_node *pnext;
	struct __px_list_node *ppre;
}px_list_node;

typedef struct __px_list
{
	px_memorypool *mp;
	px_int size;
	struct __px_list_node *head;
	struct __px_list_node *end;
}px_list;




//////////////////////////////////////////////////////////////////////////
//List
px_void PX_ListInitialize(px_memorypool *mp,px_list *list);
px_void *PX_ListPush(px_list *list,px_void *data,px_int size);
px_bool PX_ListPop(px_list *list,px_list_node *node);
px_bool PX_ListErase(px_list *list,px_int i);
px_list_node* PX_ListNodeAt(px_list *list,px_int index);
px_list_node* PX_ListNodeNext(px_list_node* node);


px_void PX_ListMove(px_list *list,px_int index,px_int moveto);
px_int	PX_ListSize(px_list *list);
px_void PX_ListClear(px_list *list);
px_void PX_ListFree(px_list *list);

#define PX_LIST_NODEDATA(x) ((x)->pdata)
#define PX_LIST_NODETDATA(t,x) ((t *)(x->pdata))
#define PX_LISTAT(t,x,i) ((t *)(PX_ListNodeAt(x,i)->pdata))	
#endif
