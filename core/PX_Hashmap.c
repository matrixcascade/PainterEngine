/*
 * Generic map implementation.
 */
#include "PX_Hashmap.h"


px_bool PX_MapInitialize(px_memorypool *mp,px_map *m)
{
	m->mp=mp;
	m->root.rb_node = PX_NULL;
	m->size = 0;

	return PX_TRUE;
}




px_map_element * px_rbtree_search(struct px_rb_root *root,const char string[])
{
	struct px_rb_node *node = root->rb_node;

	while (node) {
		px_map_element *data =(px_map_element *)node;
		int result;

		result = PX_strcmp(string, data->string);

		if (result < 0)
			node = node->rb_left;
		else if (result > 0)
			node = node->rb_right;
		else
			return data;
	}
	return PX_NULL;
}

px_bool px_rbtree_insert(struct px_rb_root *root, px_map_element *data)
{
	struct px_rb_node **newnode = &(root->rb_node), *parent = PX_NULL;

	/* Figure out where to put new node */
	while (*newnode) {
		px_map_element *__this = (px_map_element *)(*newnode);
		int result = PX_strcmp(data->string, __this->string);

		parent = *newnode;
		if (result < 0)
			newnode = &((*newnode)->rb_left);
		else if (result > 0)
			newnode = &((*newnode)->rb_right);
		else
			return PX_FALSE;
	}

	/* Add new node and rebalance tree. */
	rb_link_node(&data->node, parent, newnode);
	rb_insert_color(&data->node, root);

	return PX_TRUE;
}



PX_HASHMAP_RETURN PX_MapPut(px_map * m,const px_char stringkey[], px_void * value)
{
	px_map_element *newnode = (px_map_element *)MP_Malloc(m->mp,sizeof(px_map_element));
	newnode->Ptr=value;
	PX_strcpy(newnode->string,stringkey,sizeof(newnode->string));
	if(px_rbtree_insert(&m->root,newnode))
	{
		m->size++;
		return PX_HASHMAP_RETURN_OK;
	}
	else
		return PX_HASHMAP_RETURN_NULL;
}

PX_HASHMAP_RETURN PX_MapPutInt(px_map * m,const px_char stringkey[], px_int value)
{
	px_map_element *newnode = (px_map_element *)MP_Malloc(m->mp,sizeof(px_map_element));
	newnode->_int=value;
	PX_strcpy(newnode->string,stringkey,sizeof(newnode->string));
	if(px_rbtree_insert(&m->root,newnode))
	{
		m->size++;
		return PX_HASHMAP_RETURN_OK;
	}
	else
		return PX_HASHMAP_RETURN_NULL;
}

PX_HASHMAP_RETURN PX_MapPutFloat(px_map * m,const px_char stringkey[], px_float value)
{
	px_map_element *newnode = (px_map_element *)MP_Malloc(m->mp,sizeof(px_map_element));
	newnode->_float=value;
	PX_strcpy(newnode->string,stringkey,sizeof(newnode->string));
	if(px_rbtree_insert(&m->root,newnode))
	{
		m->size++;
		return PX_HASHMAP_RETURN_OK;
	}
	else
		return PX_HASHMAP_RETURN_NULL;
}

px_void * PX_MapGet(px_map * m, const px_char stringkey[])
{
	px_map_element *p=px_rbtree_search(&m->root,stringkey);
	if (p)
	{
		return p->Ptr;
	}
	return PX_NULL;	
}

px_void PX_MapFree(px_map * m)
{
	struct px_rb_node *node;
	m->size=0;
	node = rb_first(&m->root);
	while (node)
	{
		struct px_rb_node *p_delete_node;
		p_delete_node=node;
		node = rb_next(node);
		rb_erase(p_delete_node,&m->root);
		MP_Free(m->mp,p_delete_node);
	}
	m->size=0;
}

px_int PX_MapGetSize(px_map * m)
{
	return m->size;
}

PX_HASHMAP_RETURN PX_MapErase(px_map * m,const px_char stringkey[])
{
	px_map_element *data = px_rbtree_search(&m->root,stringkey);
	if (data) {
		m->size--;
		rb_erase(&data->node, &m->root);
		MP_Free(m->mp,data);
		return PX_HASHMAP_RETURN_OK;
	}
	return PX_HASHMAP_RETURN_NULL;
}

px_map_element * PX_MapFirst(px_map * m)
{
	struct px_rb_node *node;
	m->size=0;
	node = rb_first(&m->root);
	return (px_map_element *)node;
}

px_map_element * PX_MapNext(px_map * m,px_map_element *node)
{
	return (px_map_element *)rb_next((struct px_rb_node *)node);
}

px_bool PX_MapGetInt(px_map * m, const px_char stringkey[],px_int *v)
{
	px_map_element *p=px_rbtree_search(&m->root,stringkey);
	if (p)
	{
		*v= p->_int;
		return PX_TRUE;
	}
	return PX_FALSE;
}


px_bool PX_MapGetFloat(px_map * m, const px_char stringkey[],px_float *v)
{
	px_map_element *p=px_rbtree_search(&m->root,stringkey);
	if (p)
	{
		*v= p->_float;
		return PX_TRUE;
	}
	return PX_FALSE;
}
