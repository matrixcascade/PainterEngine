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
	px_float x,y,z,height,width,length;
	px_int user;
	px_int test;
}PX_Quadtree_AABB_BOX;

typedef struct
{
	px_int box1Index;
	px_int box2Index;
}PX_Quadtree_AABB_ImpactInfo;

typedef struct
{
	px_memorypool *mp;
	px_int PX_AABB_MAX_DEEP;
	PX_Quadtree_AABB *AABB;
	px_vector boxes;
	px_vector Impacts;
}PX_Quadtree;

px_bool PX_QuadtreeCreate(px_memorypool *mp,PX_Quadtree *pQuadtree,px_float mapStartX,px_float mapStartY,px_float mapWidth,px_float mapHeight,px_int ObjectsCount,px_int deep);
px_void PX_QuadtreeAddNode(PX_Quadtree *pQuadtree,px_float x,px_float y,px_float z,px_float width,px_float height,px_float length,px_int user);
px_void PX_QuadtreeTestNode(PX_Quadtree *pQuadtree,px_float x,px_float y,px_float z,px_float width,px_float height,px_float length,px_int user);
px_void PX_QuadtreeAddAndTestNode(PX_Quadtree *pQuadtree,px_float x,px_float y,px_float z,px_float width,px_float height,px_float length,px_int user);
px_void PX_QuadtreeReset(PX_Quadtree *pQuadtree);
#endif
