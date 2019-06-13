#include "PX_Quadtree.h"

//#include "stdio.h"

static px_bool PX_Quadtree_InitAABB(px_memorypool *mp,px_int PX_AABB_MAX_DEEP,PX_Quadtree_AABB *aabb)
{
	if (aabb)
	{
		if (aabb->deep>=PX_AABB_MAX_DEEP)
		{
			PX_ListInit(mp,&aabb->dataList);
			return PX_TRUE;
		}
		else
		{
			aabb->Area1=(PX_Quadtree_AABB *)MP_Malloc(mp,sizeof(PX_Quadtree_AABB));
			aabb->Area1->deep=aabb->deep+1;
			aabb->Area1->Left=aabb->Left;
			aabb->Area1->Right=(aabb->Left+aabb->Right)/2;
			aabb->Area1->Top=aabb->Top;
			aabb->Area1->Bottom=(aabb->Top+aabb->Bottom)/2;
			aabb->Area1->objectCount=0;
			aabb->Area1->Area1=PX_NULL;
			aabb->Area1->Area2=PX_NULL;
			aabb->Area1->Area3=PX_NULL;
			aabb->Area1->Area4=PX_NULL;
			aabb->Area1->dataList.size=0;
			aabb->Area1->dataList.head=PX_NULL;
			aabb->Area1->dataList.end=PX_NULL;
			aabb->Area1->dataList.mp=PX_NULL;
			if(!PX_Quadtree_InitAABB(mp,PX_AABB_MAX_DEEP,aabb->Area1)) return PX_FALSE;

			aabb->Area2=(PX_Quadtree_AABB *)MP_Malloc(mp,sizeof(PX_Quadtree_AABB));
			aabb->Area2->deep=aabb->deep+1;
			aabb->Area2->Left=aabb->Area1->Left+(aabb->Right-aabb->Left)/2;
			aabb->Area2->Right=aabb->Area1->Right+(aabb->Right-aabb->Left)/2;
			aabb->Area2->Top=aabb->Area1->Top;
			aabb->Area2->Bottom=aabb->Area1->Bottom;
			aabb->Area2->objectCount=0;
			aabb->Area2->Area1=PX_NULL;
			aabb->Area2->Area2=PX_NULL;
			aabb->Area2->Area3=PX_NULL;
			aabb->Area2->Area4=PX_NULL;
			aabb->Area2->dataList.size=0;
			aabb->Area2->dataList.head=PX_NULL;
			aabb->Area2->dataList.end=PX_NULL;
			aabb->Area2->dataList.mp=PX_NULL;
			if(!PX_Quadtree_InitAABB(mp,PX_AABB_MAX_DEEP,aabb->Area2)) return PX_FALSE;

			aabb->Area3=(PX_Quadtree_AABB *)MP_Malloc(mp,sizeof(PX_Quadtree_AABB));
			aabb->Area3->deep=aabb->deep+1;
			aabb->Area3->Left=aabb->Area1->Left;
			aabb->Area3->Right=aabb->Area1->Right;
			aabb->Area3->Top=aabb->Area1->Top+(aabb->Bottom-aabb->Top)/2;
			aabb->Area3->Bottom=aabb->Area1->Bottom+(aabb->Bottom-aabb->Top)/2;;
			aabb->Area3->objectCount=0;
			aabb->Area3->Area1=PX_NULL;
			aabb->Area3->Area2=PX_NULL;
			aabb->Area3->Area3=PX_NULL;
			aabb->Area3->Area4=PX_NULL;
			aabb->Area3->dataList.size=0;
			aabb->Area3->dataList.head=PX_NULL;
			aabb->Area3->dataList.end=PX_NULL;
			aabb->Area3->dataList.mp=PX_NULL;
			if(!PX_Quadtree_InitAABB(mp,PX_AABB_MAX_DEEP,aabb->Area3)) return PX_FALSE;

			aabb->Area4=(PX_Quadtree_AABB *)MP_Malloc(mp,sizeof(PX_Quadtree_AABB));
			aabb->Area4->deep=aabb->deep+1;
			aabb->Area4->Left=aabb->Area1->Left+(aabb->Right-aabb->Left)/2;
			aabb->Area4->Right=aabb->Area1->Right+(aabb->Right-aabb->Left)/2;
			aabb->Area4->Top=aabb->Area1->Top+(aabb->Bottom-aabb->Top)/2;
			aabb->Area4->Bottom=aabb->Area1->Bottom+(aabb->Bottom-aabb->Top)/2;
			aabb->Area4->objectCount=0;
			aabb->Area4->Area1=PX_NULL;
			aabb->Area4->Area2=PX_NULL;
			aabb->Area4->Area3=PX_NULL;
			aabb->Area4->Area4=PX_NULL;
			aabb->Area4->dataList.size=0;
			aabb->Area4->dataList.head=PX_NULL;
			aabb->Area4->dataList.end=PX_NULL;
			aabb->Area4->dataList.mp=PX_NULL;
			if(!PX_Quadtree_InitAABB(mp,PX_AABB_MAX_DEEP,aabb->Area4)) return PX_FALSE;

			return PX_TRUE;
		}
	}
	return PX_FALSE;
}

px_bool PX_QuadtreeCreate(px_memorypool *mp,PX_Quadtree *pQuadtree,px_float mapStartX,px_float mapStartY,px_float mapWidth,px_float mapHeight,px_int ObjectsCount,px_int deep)
{
	pQuadtree->mp=mp;
	pQuadtree->PX_AABB_MAX_DEEP=deep;
	PX_VectorInit(mp,&pQuadtree->boxes,sizeof(PX_Quadtree_AABB),ObjectsCount);
	PX_VectorInit(mp,&pQuadtree->Impacts,sizeof(PX_Quadtree_AABB_ImpactInfo),ObjectsCount);
	pQuadtree->AABB=(PX_Quadtree_AABB *)MP_Malloc(mp,sizeof(PX_Quadtree_AABB));
	pQuadtree->AABB->deep=0;
	pQuadtree->AABB->Area1=PX_NULL;
	pQuadtree->AABB->Area2=PX_NULL;
	pQuadtree->AABB->Area3=PX_NULL;
	pQuadtree->AABB->Area4=PX_NULL;
	pQuadtree->AABB->Left=mapStartX;
	pQuadtree->AABB->Right=mapStartX+mapWidth;
	pQuadtree->AABB->Top=mapStartY;
	pQuadtree->AABB->Bottom=mapStartY+mapHeight;
	pQuadtree->AABB->objectCount=0;
	pQuadtree->AABB->dataList.size=0;
	pQuadtree->AABB->dataList.head=PX_NULL;
	pQuadtree->AABB->dataList.end=PX_NULL;
	pQuadtree->AABB->dataList.mp=PX_NULL;
	return PX_Quadtree_InitAABB(mp,pQuadtree->PX_AABB_MAX_DEEP,pQuadtree->AABB);
}

static px_void PX_QuadtreeTest(PX_Quadtree *pQuadtree,PX_Quadtree_AABB *aabb,px_int index)
{
	px_list_node *pnode=aabb->dataList.head;
	px_rect r1,r2;
	PX_Quadtree_AABB_BOX *pBox,*ptestBox;
	PX_Quadtree_AABB_ImpactInfo intersect;
	while (pnode)
	{
		pBox=PX_VECTORAT(PX_Quadtree_AABB_BOX,&pQuadtree->boxes,*(px_int *)pnode->pdata);
		ptestBox=PX_VECTORAT(PX_Quadtree_AABB_BOX,&pQuadtree->boxes,index);
		
		if (ptestBox->test!=index)
		{
			r1.x=pBox->x-pBox->width/2;
			r1.y=pBox->y-pBox->height/2;
			r1.width=pBox->width;
			r1.height=pBox->height;

			r2.x=ptestBox->x-ptestBox->width/2;
			r2.y=ptestBox->y-ptestBox->height/2;
			r2.width=ptestBox->width;
			r2.height=ptestBox->height;

			if (PX_isRectCrossRect(r1,r2))
			{
				if (PX_ABS(pBox->z-ptestBox->z)>=(pBox->length+ptestBox->length)/2)
				{
					ptestBox->test=index;
					intersect.box1Index=pBox->user;
					intersect.box2Index=ptestBox->user;
					//printf("intersect %d:%d\n",pBox->index,ptestBox->index);
					PX_VectorPushback(&pQuadtree->Impacts,&intersect);
				}
			}
		}
	
		pnode=pnode->pnext;
	}

}

static px_void PX_QuadtreeSortBox(PX_Quadtree *pQuadtree,PX_Quadtree_AABB *aabb,px_int index,px_bool btest)
{
	px_rect r1,r2;
	PX_Quadtree_AABB_BOX *pBox;
	pBox=PX_VECTORAT(PX_Quadtree_AABB_BOX,&pQuadtree->boxes,index);
	r1.x=pBox->x-pBox->width/2;
	r1.y=pBox->y-pBox->height/2;
	r1.width=pBox->width;
	r1.height=pBox->height;

	r2.x=aabb->Left;
	r2.y=aabb->Top;
	r2.width=aabb->Right-aabb->Left+1;
	r2.height=aabb->Bottom-aabb->Top+1;
	if (PX_isRectCrossRect(r1,r2))
	{
		if (aabb->deep>=pQuadtree->PX_AABB_MAX_DEEP)
		{
			if(btest)
			PX_QuadtreeTest(pQuadtree,aabb,index);
//			printf("Index %d at x:%f y:%f w:%f h:%f \n",index,r2.x,r2.y,r2.width,r2.height);
			PX_ListPush(&aabb->dataList,&index,sizeof(px_int));
		}
		else
		{
			PX_QuadtreeSortBox(pQuadtree,aabb->Area1,index,btest);
			PX_QuadtreeSortBox(pQuadtree,aabb->Area2,index,btest);
			PX_QuadtreeSortBox(pQuadtree,aabb->Area3,index,btest);
			PX_QuadtreeSortBox(pQuadtree,aabb->Area4,index,btest);
		}
	}
}
px_void PX_QuadtreeAddNode(PX_Quadtree *pQuadtree,px_float x,px_float y,px_float z,px_float width,px_float height,px_float length,px_int user)
{
	PX_Quadtree_AABB_BOX box;
	box.height=height;
	box.width=width;
	box.length=length;
	box.x=x;
	box.y=y;
	box.z=z;
	box.user=user;
	box.test=-1;
	PX_VectorPushback(&pQuadtree->boxes,&box);

	PX_QuadtreeSortBox(pQuadtree,pQuadtree->AABB,pQuadtree->boxes.size-1,PX_FALSE);
}

px_void PX_QuadtreeAddAndTestNode(PX_Quadtree *pQuadtree,px_float x,px_float y,px_float z,px_float width,px_float height,px_float length,px_int user)
{
	PX_Quadtree_AABB_BOX box;
	box.height=height;
	box.width=width;
	box.length=length;
	box.x=x;
	box.y=y;
	box.z=z;
	box.user=user;
	box.test=-1;
	PX_VectorPushback(&pQuadtree->boxes,&box);

	PX_QuadtreeSortBox(pQuadtree,pQuadtree->AABB,pQuadtree->boxes.size-1,PX_TRUE);
}

px_void PX_QuadtreeTestNode(PX_Quadtree *pQuadtree,px_float x,px_float y,px_float z,px_float width,px_float height,px_float length,px_int user)
{
	PX_Quadtree_AABB_BOX *ptestBox;
	PX_Quadtree_AABB_ImpactInfo Impacts;
	px_rect r1,r2;
	px_int i;
	
	r1.x=x-width/2;
	r1.y=y-height/2;
	r1.width=width;
	r1.height=height;

	for (i=0;i<pQuadtree->boxes.size;i++)
	{
		ptestBox=PX_VECTORAT(PX_Quadtree_AABB_BOX,&pQuadtree->boxes,i);

		if (ptestBox->test!=0xffffff)
		{
			r1.x=x-width/2;
			r1.y=y-height/2;
			r1.width=width;
			r1.height=height;

			r2.x=ptestBox->x-ptestBox->width/2;
			r2.y=ptestBox->y-ptestBox->height/2;
			r2.width=ptestBox->width;
			r2.height=ptestBox->height;

			if (PX_isRectCrossRect(r1,r2))
			{
				if (PX_ABS(z-ptestBox->z)>=(length+ptestBox->length)/2)
				{
					ptestBox->test=0xffffff;
					Impacts.box1Index=user;
					Impacts.box2Index=ptestBox->user;
					PX_VectorPushback(&pQuadtree->Impacts,&Impacts);
				}
			}
		}
	}
}
