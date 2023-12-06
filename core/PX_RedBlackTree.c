#include "PX_RedBlackTree.h"

px_bool PX_RBTreeInitialize(px_memorypool* mp, PX_RBTree* tree)
{
	tree->mp = mp;
	tree->root = PX_NULL;
	tree->size = 0;
	return PX_TRUE;
}

px_void PX_RBNodeRotateLeft(PX_RBTree *tree, PX_RBNode* node)
{
	PX_RBNode* right = node->right;
	PX_RBNode* parent = node->parent;

	node->right = right->left;
	if (right->left)
		right->left->parent = node;

	right->left = node;
	right->parent = parent;

	if (parent)
	{
		if (node == parent->left)
			parent->left = right;
		else
			parent->right = right;
	}
	else
	{
		tree->root = right;
	}
	node->parent = right;	
}

px_void PX_RBNodeRotateRight(PX_RBTree* tree, PX_RBNode* node)
{
	PX_RBNode* left = node->left;
	PX_RBNode* parent = node->parent;

	node->left = left->right;
	if (left->right)
		left->right->parent = node;

	left->right = node;
	left->parent = parent;

	if (parent)
	{
		if (node == parent->right)
			parent->right = left;
		else
			parent->left = left;
	}
	else
	{
		tree->root = left;
	}
	node->parent = left;
}
typedef enum
{
	PX_RBTREE_MATCH_OK,
	PX_RBTREE_MATCH_LEFT,
	PX_RBTREE_MATCH_RIGHT,
}PX_RBTREE_MATCH;

PX_RBTREE_MATCH PX_RBNodeMatchCode(PX_RBNode* test, px_dword keyvalue,const px_byte code[],px_int codesize)
{
	if (keyvalue == test->keyvalue)
	{
		px_int cmp;
		if (test->codesize < codesize)
		{
			return PX_RBTREE_MATCH_LEFT;
		}
		else if (test->codesize > codesize)
		{
			return PX_RBTREE_MATCH_RIGHT;
		}
		cmp = PX_memcmp(test->code, code, codesize);
		if (cmp < 0)
		{
			return PX_RBTREE_MATCH_LEFT;
		}
		else if (cmp > 0)
		{
			return PX_RBTREE_MATCH_RIGHT;
		}
		else
		{
			return PX_RBTREE_MATCH_OK;
		}
	}
	else
	{
		if (keyvalue < test->keyvalue)
			return PX_RBTREE_MATCH_LEFT;
		else
			return PX_RBTREE_MATCH_RIGHT;
	}
}

PX_RBTREE_MATCH PX_RBNodeMatch(PX_RBNode* testSrc, PX_RBNode* testDest)
{
	return PX_RBNodeMatchCode(testDest, testSrc->keyvalue, testSrc->code, testSrc->codesize);
}

PX_RBNode* PX_RBTreeGetNode(PX_RBTree* tree, const px_byte code[],px_int size)
{
	//find node
	PX_RBNode* node = tree->root;
	PX_RBTREE_MATCH cmp;
	px_dword hash = PX_MurmurHash(code, size);
	while (node)
	{
		cmp = PX_RBNodeMatchCode(node,hash,code, size);
		switch (cmp)
		{
			case PX_RBTREE_MATCH_OK:
				return node;
			case PX_RBTREE_MATCH_LEFT:
				node = node->left;
				break;
			case PX_RBTREE_MATCH_RIGHT:
				node = node->right;
				break;
		default:
			break;
		}
	}
	return PX_NULL;
}

px_bool PX_RBTreeInsertNode(PX_RBTree* tree, PX_RBNode* node)
{
	//insert node
	PX_RBNode* parent, * gparent,*uncle,*pdetectNode;
	PX_RBNode** recursionPos;

	recursionPos = &tree->root;
	parent = PX_NULL;
	while (*recursionPos)
	{
		parent = *recursionPos;
		switch (PX_RBNodeMatch(node,(*recursionPos)))
		{
			case PX_RBTREE_MATCH_OK:
				return PX_FALSE;
			case PX_RBTREE_MATCH_LEFT:
				recursionPos = &(*recursionPos)->left;
				break;
			case PX_RBTREE_MATCH_RIGHT:
				recursionPos = &(*recursionPos)->right;
				break;
		default:
			break;
		}
	}
	/* Add new node*/
	*recursionPos = node;
	node->parent = parent;
	node->left = node->right = PX_NULL;
	node->color = PX_RBNODE_COLOR_RED;
	pdetectNode = (*recursionPos);

	while (PX_TRUE)
	{
		parent = pdetectNode->parent;
		if (!parent|| parent->color == PX_RBNODE_COLOR_BLACK)
		{
			tree->root->color=PX_RBNODE_COLOR_BLACK;
			break;
		}


		gparent = parent->parent;
		
		if (parent==gparent->left)
		{
			uncle = gparent->right;
		}
		else
		{
			uncle = gparent->left;
		}

		if (uncle!=PX_NULL&&uncle->color==PX_RBNODE_COLOR_RED)
		{
			uncle->color = PX_RBNODE_COLOR_BLACK;
			parent->color = PX_RBNODE_COLOR_BLACK;
			gparent->color = PX_RBNODE_COLOR_RED;
			pdetectNode = gparent;
			continue;
		}

		if (parent == gparent->left&&pdetectNode==parent->right)
		{
			PX_RBNodeRotateLeft(tree, parent);
			pdetectNode = parent;
			parent = pdetectNode->parent;
		}
		else if (parent == gparent->right&&pdetectNode==parent->left)
		{
			PX_RBNodeRotateRight(tree, parent);
			pdetectNode = parent;
			parent = pdetectNode->parent;
		}
		
		if (parent==gparent->left&&pdetectNode==parent->left)
		{
			gparent->color = PX_RBNODE_COLOR_RED;
			parent->color = PX_RBNODE_COLOR_BLACK;
			PX_RBNodeRotateRight(tree, gparent);
		}
		else if (parent == gparent->right && pdetectNode == parent->right)
		{
			gparent->color = PX_RBNODE_COLOR_RED;
			parent->color = PX_RBNODE_COLOR_BLACK;
			PX_RBNodeRotateLeft(tree, gparent);
		}
	
	}
	tree->size++;
	return PX_TRUE;
}

px_bool PX_RBTreePutInt(PX_RBTree* tree, const px_byte code[], px_int codesize, px_int integer)
{
	PX_RBNode* node;

	if (PX_RBTreeGetNode(tree, code, codesize))
		return PX_FALSE;

	node = (PX_RBNode*)MP_Malloc(tree->mp, sizeof(PX_RBNode));
	if (!node)
	{
		return PX_FALSE;
	}
	PX_memset(node, 0, sizeof(PX_RBNode));
	node->color = PX_RBNODE_COLOR_RED;
	node->codesize = codesize;
	PX_memcpy(node->code, code, codesize);
	node->keyvalue=PX_MurmurHash(code,codesize);


	node->_int = integer;
	return PX_RBTreeInsertNode(tree, node);
}

px_bool PX_RBTreePutPtr(PX_RBTree* tree, const px_byte code[], px_int codesize, px_void *ptr)
{
	PX_RBNode* node;

	if (PX_RBTreeGetNode(tree, code, codesize))
		return PX_FALSE;

	node = (PX_RBNode*)MP_Malloc(tree->mp, sizeof(PX_RBNode));
	if (!node)
	{
		return PX_FALSE;
	}
	PX_memset(node, 0, sizeof(PX_RBNode));
	node->color = PX_RBNODE_COLOR_RED;
	node->codesize = codesize;
	PX_memcpy(node->code, code, codesize);
	node->keyvalue = PX_MurmurHash(code, codesize);
	node->_ptr = ptr;

	return PX_RBTreeInsertNode(tree, node);
}

px_bool PX_RBTreePutFloat(PX_RBTree* tree, const px_byte code[], px_int codesize, px_float value)
{
	PX_RBNode* node;

	if (PX_RBTreeGetNode(tree, code, codesize))
		return PX_FALSE;

	node = (PX_RBNode*)MP_Malloc(tree->mp, sizeof(PX_RBNode));
	if (!node)
	{
		return PX_FALSE;
	}
	PX_memset(node, 0, sizeof(PX_RBNode));
	node->color = PX_RBNODE_COLOR_RED;
	node->codesize = codesize;
	PX_memcpy(node->code, code, codesize);
	node->keyvalue = PX_MurmurHash(code, codesize);

	node->_float = value;

	return PX_RBTreeInsertNode(tree, node);
}


px_void PX_RBTreeDisconnectNode(PX_RBTree* tree, PX_RBNode* pDeleteNode)
{ 
	PX_RBNode* pParent,*pBrother,*pUpdateNode;

	if (pDeleteNode ==tree->root)
	{
		if (pDeleteNode->left==PX_NULL && pDeleteNode->right == PX_NULL)
		{
			tree->root = PX_NULL;
			return;
		}
		
		if (pDeleteNode->left&&!pDeleteNode->right)
		{
			PX_RBNodeRotateRight(tree, pDeleteNode);
			tree->root->right = PX_NULL;
			pDeleteNode->parent = PX_NULL;
			return;
		}
		if (!pDeleteNode->left && pDeleteNode->right)
		{
			PX_RBNodeRotateLeft(tree, pDeleteNode);
			tree->root->left = PX_NULL;
			pDeleteNode->parent = PX_NULL;
			return;
		}
	}

	pParent = pDeleteNode->parent;

	if (pDeleteNode->color == PX_RBNODE_COLOR_RED)
	{
		if (pParent->left == pDeleteNode)
			pParent->left = PX_NULL;
		else
			pParent->right = PX_NULL;
		return;
	}

	else
	{
		if (pDeleteNode->left != PX_NULL)
		{
			if (pParent->left == pDeleteNode)
				pParent->left = pDeleteNode->left;
			else
				pParent->right = pDeleteNode->left;

			pDeleteNode->left->parent = pParent;
			pDeleteNode->left->color = PX_RBNODE_COLOR_BLACK;
			return;
		}
		else if (pDeleteNode->right != PX_NULL)
		{
			if (pParent->left == pDeleteNode)
				pParent->left = pDeleteNode->right;
			else
				pParent->right = pDeleteNode->left;

			pDeleteNode->right->parent = pParent;
			pDeleteNode->right->color = PX_RBNODE_COLOR_BLACK;
			return;
		}
		else
		{
			if (pParent->left == pDeleteNode)
				pParent->left = PX_NULL;
			else
				pParent->right = PX_NULL;

			pUpdateNode = PX_NULL;

			pBrother = pParent->left== pUpdateNode ? pParent->right:pParent->left;

			while (1)
			{
				if (pBrother->color == PX_RBNODE_COLOR_RED)
				{
					if (pBrother == pParent->right)
					{
						pParent->color = PX_RBNODE_COLOR_RED;
						pBrother->color = PX_RBNODE_COLOR_BLACK;
						PX_RBNodeRotateLeft(tree,pParent);
						pBrother = pParent->left == pUpdateNode ? pParent->right : pParent->left;
						continue;
					}
					else
					{
						pParent->color = PX_RBNODE_COLOR_RED;
						pBrother->color = PX_RBNODE_COLOR_BLACK;
						PX_RBNodeRotateRight(tree, pParent);
						pBrother = pParent->left == pUpdateNode ? pParent->right : pParent->left;
						continue;
					}
				}
				else
				{
					if (pBrother == pParent->right)
					{
						if (pBrother->left && pBrother->left->color == PX_RBNODE_COLOR_RED && (pBrother->right == PX_NULL|| pBrother->right->color== PX_RBNODE_COLOR_BLACK))
						{
							pBrother->color = PX_RBNODE_COLOR_RED;
							pBrother->left->color = PX_RBNODE_COLOR_BLACK;
							PX_RBNodeRotateRight(tree,pBrother);

							pBrother = pParent->left == pUpdateNode ? pParent->right : pParent->left;
							continue;
						}

						else if (pBrother->right && pBrother->right->color == PX_RBNODE_COLOR_RED)
						{
							pBrother->color = pParent->color;
							pParent->color = PX_RBNODE_COLOR_BLACK;
							pBrother->right->color = PX_RBNODE_COLOR_BLACK;
							PX_RBNodeRotateLeft(tree,pParent);
							return;
						}
						else
						{
							if (pParent->color == PX_RBNODE_COLOR_RED)
							{
								pBrother->color = PX_RBNODE_COLOR_RED;
								pParent->color = PX_RBNODE_COLOR_BLACK;
								return;
							}
							else
							{
								pBrother->color = PX_RBNODE_COLOR_RED;
								pUpdateNode = pParent;
								if (tree->root == pUpdateNode)
								{
									pUpdateNode->color = PX_RBNODE_COLOR_BLACK;
									return;
								}
								pParent = pUpdateNode->parent;
								pBrother = pParent->left == pUpdateNode ? pParent->right : pParent->left;

								continue;
							}
						}
					}
					else
					{
						if (pBrother->left && pBrother->left->color == PX_RBNODE_COLOR_RED)
						{
							pBrother->color = pParent->color;
							pParent->color = PX_RBNODE_COLOR_BLACK;
							pBrother->left->color = PX_RBNODE_COLOR_BLACK;
							PX_RBNodeRotateRight(tree,pParent);
							return;
						}
						else if ((pBrother->left == PX_NULL||pBrother->left->color== PX_RBNODE_COLOR_BLACK) && pBrother->right && pBrother->right->color == PX_RBNODE_COLOR_RED)
						{
							pBrother->color = PX_RBNODE_COLOR_RED;
							pBrother->right->color = PX_RBNODE_COLOR_BLACK;
							PX_RBNodeRotateLeft(tree,pBrother);
							pBrother = pParent->left == pUpdateNode ? pParent->right : pParent->left;
							continue;
						}
						else
						{
							if (pParent->color == PX_RBNODE_COLOR_RED)
							{
								pBrother->color = PX_RBNODE_COLOR_RED;
								pParent->color = PX_RBNODE_COLOR_BLACK;
								return;
							}
							else
							{
								pBrother->color = PX_RBNODE_COLOR_RED;
								pUpdateNode = pParent;
								if (pUpdateNode == tree->root)
								{
									pUpdateNode->color = PX_RBNODE_COLOR_BLACK;
									return;
								}
								pParent = pUpdateNode->parent;
								pBrother = pParent->left == pUpdateNode ? pParent->right : pParent->left;

								continue;
							}
						}
					}
				}
			}
		}
	}
}




px_void PX_RBTreeDeleteNode(PX_RBTree* tree, PX_RBNode* node)
{
	PX_RBNode* pDeleteNode = node;
	//find predecess
	if (node->left)
	{
		pDeleteNode = node->left;
		while (pDeleteNode->right)
		{
			pDeleteNode = pDeleteNode->right;
		}
	}

	if (pDeleteNode != node)
	{
		//copy values
		node->codesize = pDeleteNode->codesize;
		PX_memcpy(node->code, pDeleteNode->code, pDeleteNode->codesize);
		node->keyvalue = pDeleteNode->keyvalue;
		PX_memcpy(node->_align, pDeleteNode->_align, sizeof(node->_align));
	}

	PX_RBTreeDisconnectNode(tree, pDeleteNode);
	MP_Free(tree->mp, pDeleteNode);
	tree->size--;
}


px_void PX_RBTreeFree(PX_RBTree* tree)
{
	PX_RBNode* node;
	while ((node=tree->root)!=PX_NULL)
	{
		PX_RBTreeDeleteNode(tree, node);
	}
}


px_int PX_RBTreeGetSize(PX_RBTree* tree)
{
	return tree->size;
}

px_bool PX_RBTreeGetInt(PX_RBTree* tree, const px_byte code[], px_int codesize, px_int* integer)
{
	PX_RBNode *pNode= PX_RBTreeGetNode(tree, code, codesize);
	if (pNode)
	{
		*integer = pNode->_int;
		return PX_TRUE;
	}
	else
	{
		return PX_FALSE;
	}

}
px_bool PX_RBTreeGetPtr(PX_RBTree* tree, const px_byte code[], px_int codesize, px_void** ptr)
{
	PX_RBNode *pNode= PX_RBTreeGetNode(tree, code, codesize);
	if (pNode)
	{
		*ptr = pNode->_ptr;
		return PX_TRUE;
	}
	else
	{
		return PX_FALSE;
	}
}

px_bool PX_RBTreeGetFloat(PX_RBTree* tree, const px_byte code[], px_int codesize, px_float* value)
{
	PX_RBNode *pNode= PX_RBTreeGetNode(tree, code, codesize);
	if (pNode)
	{
		*value = pNode->_float;
		return PX_TRUE;
	}
	else
	{
		return PX_FALSE;
	}
}

px_bool PX_RBTreeDelete(PX_RBTree* tree, const px_byte code[], px_int codesize)
{
	PX_RBNode *pNode= PX_RBTreeGetNode(tree, code, codesize);
	if (pNode)
	{
		PX_RBTreeDeleteNode(tree, pNode);
		return PX_TRUE;
	}
	else
	{
		return PX_FALSE;
	}
}


PX_RBNode* PX_RBTreeFirst(PX_RBTree* tree)
{
	PX_RBNode* n;

	n = tree->root;
	if (!n)
		return PX_NULL;
	while (n->left)
		n = n->left;
	return n;
}

PX_RBNode* PX_RBTreeNext(PX_RBNode* node)
{
	PX_RBNode* parent;

	if (node->parent == node)
		return PX_NULL;

	if (node->right) {
		node = node->right;
		while (node->left)
			node = node->left;
		return node;
	}


	while ((parent = node->parent) && node == parent->right)
		node = parent;

	return parent;
}