#ifndef PX_QUADTREE_H
#define PX_QUADTREE_H
#include "PX_List.h"
#include "PX_Vector.h"

#define PX_AABB_TEST_COUNT 32

typedef struct  _PX_AABB
{
	px_float Left,Right,Bottom,Top;
	px_int   objectCount;
	px_int	 deep;
	px_list  dataList;
	struct  _PX_AABB *Area1;
	struct  _PX_AABB *Area2;
	struct  _PX_AABB *Area3;
	struct  _PX_AABB *Area4;
}PX_Quadtree_AABB;

typedef struct
{
	px_void *ptr;
}PX_Quadtree_UserData;
typedef struct
{
	px_float x,y,height,width;
	PX_Quadtree_UserData userdata;
}PX_Quadtree_AABB_BOX;


typedef struct
{
	px_memorypool *mp;
	px_int PX_AABB_MAX_DEEP;
	PX_Quadtree_AABB *AABB;
	px_vector boxes;
	px_vector Impacts;
}PX_Quadtree;

px_bool PX_QuadtreeCreate(px_memorypool *mp,PX_Quadtree *pQuadtree,px_float mapStartX,px_float mapStartY,px_float mapWidth,px_float mapHeight,px_int ObjectsCount,px_int deep);
px_void PX_QuadtreeAddNode(PX_Quadtree *pQuadtree,px_float CenterX,px_float CenterY,px_float width,px_float height,PX_Quadtree_UserData userData);
px_void PX_QuadtreeTestNode(PX_Quadtree *pQuadtree,px_float CenterX,px_float CenterY,px_float width,px_float height,PX_Quadtree_UserData user);
px_void PX_QuadtreeResetTest(PX_Quadtree *pQuadtree);
#endif
