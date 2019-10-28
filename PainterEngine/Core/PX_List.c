#include "PX_List.h"

px_void PX_ListInit(px_memorypool *mp,px_list *list)
{
	list->end=PX_NULL;
	list->head=PX_NULL;
	list->size=0;
	list->mp=mp;
}

px_void *PX_ListPush(px_list *list,px_void *data,px_int size)
{
	px_list_node *node;
	if (list->head==PX_NULL)
	{
		list->head=(px_list_node *)MP_Malloc(list->mp,sizeof(px_list_node));
		list->head->pdata=MP_Malloc(list->mp,size);
		PX_memcpy(list->head->pdata,data,size);
		list->head->pnext=PX_NULL;
		list->head->ppre=PX_NULL;
		list->end=list->head;
		list->size++;
		return list->head->pdata;
	}
	else
	{
		node=(px_list_node *)MP_Malloc(list->mp,sizeof(px_list_node));
		node->pdata=MP_Malloc(list->mp,size);
		PX_memcpy(node->pdata,data,size);
		node->pnext=PX_NULL;
		node->ppre=list->end;
		list->end->pnext=node;
		list->end=node;
		list->size++;
		return node->pdata;
	}
}

px_bool PX_ListPop(px_list *list,px_list_node *node)
{
	if (!node)
	{
		PX_ERROR("Error List node pop");
		return PX_FALSE;
	}
	MP_Free(list->mp,node->pdata);
	if (node==list->head)
	{
		list->head=list->head->pnext;
		if (list->head)
		{
			list->head->ppre=PX_NULL;
		}
		MP_Free(list->mp,node);
		list->size--;
		if (list->size==0)
		{
			list->head=PX_NULL;
			list->end=PX_NULL;
		}
		return PX_TRUE;
	}

	if (node==list->end)
	{
		list->end=list->end->ppre;
		if (list->end)
		{
			list->end->pnext=PX_NULL;
		}
		MP_Free(list->mp,node);
		list->size--;
		if (list->size==0)
		{
			list->head=PX_NULL;
			list->end=PX_NULL;
		}
		return PX_TRUE;
	}
	node->ppre->pnext=node->pnext;
	node->pnext->ppre=node->ppre;
	MP_Free(list->mp,node);
	list->size--;

	return PX_TRUE;
}

px_list_node* PX_ListAt(px_list *list,px_int index)
{
	px_list_node *node=list->head;
	if (index>=list->size)
	{
		return PX_NULL;
	}
	while (index--)
	{
		node=node->pnext;
	}
	return node;
}

px_int PX_ListSize(px_list *list)
{
	return list->size;
}

px_void PX_ListClear(px_list *list)
{
	px_list_node *node=list->head;
	px_list_node *pn;
	while (node)
	{
		pn=node;
		node=node->pnext;
		MP_Free(list->mp,pn->pdata);
		MP_Free(list->mp,pn);
	}
	list->end=PX_NULL;
	list->head=PX_NULL;
	list->size=0;
}

px_void PX_ListFree(px_list *list)
{
	PX_ListClear(list);
}
