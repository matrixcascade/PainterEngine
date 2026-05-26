#ifndef PX_OBJECT_TREE_H
#define PX_OBJECT_TREE_H
#include "PX_Object.h"

typedef struct
{
	px_int width;
	px_bool babi;
	px_bool bcursor;
	px_int deep;
	px_string content;
	px_string payload;
}PX_Object_TreeNode;

typedef struct
{
	px_bool display_name;
	px_bool need_rerender;
	px_int yoffset;
	px_int ycount;
	px_int xoffset;
	px_abi tree_root_abi;
	px_int item_height;
	px_vector nodes;
	px_texture render_target;
	px_color text_color,select_color,border_color,background_color;
	PX_Object* hslider, *vslider;
	PX_Object_TreeNode* pcurrent_select_node;
	PX_FontModule* fm;
}PX_Object_Tree;

PX_Object* PX_Object_TreeCreate(px_memorypool* mp, PX_Object* Parent, px_int x, px_int y, px_int Width, px_int Height,px_int item_height, PX_FontModule* fm);
px_bool PX_Object_TreeAddAbi(PX_Object* pObject, const px_char name[], px_abi* pAbi);
px_bool PX_Object_TreeSetAbi(PX_Object* pObject, px_abi* pAbi);
px_void PX_Object_TreeClear(PX_Object* pObject);
PX_Object_TreeNode* PX_Object_TreeGetCurrentSelectNode(PX_Object* pObject);
#endif

