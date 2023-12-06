
#ifndef PX_RBTREE_H
#define PX_RBTREE_H

#include "PX_Murmur.h"
#include "PX_MemoryPool.h"

#define PX_RBNODE_CODE_MAX_LEN 64


typedef enum
{
	PX_RBNODE_COLOR_RED=0,
	PX_RBNODE_COLOR_BLACK=1,
}PX_RBNODE_COLOR;

typedef struct _PX_RBNode
{
	struct _PX_RBNode *left, *right, *parent;
	PX_RBNODE_COLOR color;
	px_dword    keyvalue;
	px_byte		code[PX_RBNODE_CODE_MAX_LEN];
	px_int		codesize;
	union
	{
		px_void*    _ptr;
		px_int      _int;
		px_float    _float;
		px_byte		_align[16];
	};
}PX_RBNode;


typedef struct _PX_RBTree
{
	px_memorypool* mp;
	PX_RBNode *root;
	px_int size;
}PX_RBTree;

px_bool PX_RBTreeInitialize(px_memorypool *mp,PX_RBTree *tree);
px_bool PX_RBTreeInsertNode(PX_RBTree* tree, PX_RBNode* node);
px_bool PX_RBTreePutInt(PX_RBTree* tree, const px_byte code[],px_int codesize, px_int integer);
px_bool PX_RBTreePutPtr(PX_RBTree* tree, const px_byte code[], px_int codesize ,px_void *ptr);
px_bool PX_RBTreePutFloat(PX_RBTree* tree, const px_byte code[], px_int codesize , px_float value);

PX_RBNode* PX_RBTreeGetNode(PX_RBTree* tree, const px_byte code[], px_int codesize);
px_bool PX_RBTreeGetInt(PX_RBTree* tree, const px_byte code[], px_int codesize, px_int *integer);
px_bool PX_RBTreeGetPtr(PX_RBTree* tree, const px_byte code[], px_int codesize, px_void** ptr);
px_bool PX_RBTreeGetFloat(PX_RBTree* tree, const px_byte code[], px_int codesize, px_float* value);
px_bool PX_RBTreeDelete(PX_RBTree* tree, const px_byte code[], px_int codesize);
px_void PX_RBTreeDeleteNode(PX_RBTree* tree, PX_RBNode* node);
px_void PX_RBTreeFree(PX_RBTree* tree);
PX_RBNode* PX_RBTreeFirst(PX_RBTree* tree);
PX_RBNode* PX_RBTreeNext(PX_RBNode *node);


#endif