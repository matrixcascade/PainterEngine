#include "PX_Quadtree.h"

//#include "stdio.h"

static px_bool PX_Quadtree_InitAABB(px_memorypool *mp,px_int PX_AABB_MAX_DEEP,PX_Quadtree_AABB *aabb)
{
	if (aabb)
	{
		if (aabb->deep>=PX_AABB_MAX_DEEP)
		{
			PX_ListInitialize(mp,&aabb->dataList);
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
	if(!PX_VectorInitialize(mp,&pQuadtree->boxes,sizeof(PX_Quadtree_AABB_BOX),ObjectsCount))
	{
		PX_ASSERT();
		return PX_FALSE;
	}
	if(!PX_VectorInitialize(mp,&pQuadtree->Impacts,sizeof(PX_Quadtree_UserData),ObjectsCount))
	{
		PX_ASSERT();
		PX_VectorFree(&pQuadtree->boxes);
		return PX_FALSE;
	}
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

static px_void PX_QuadtreeTest(PX_Quadtree *pQuadtree,PX_Quadtree_AABB *aabb,PX_Quadtree_AABB_BOX testBox)
{
	px_list_node *pnode=aabb->dataList.head;
	px_rect r1,r2;
	PX_Quadtree_AABB_BOX *pBox;


	while (pnode)
	{
		pBox=PX_VECTORAT(PX_Quadtree_AABB_BOX,&pQuadtree->boxes,*(px_int *)pnode->pdata);
		if (pBox->userdata.ptr!=testBox.userdata.ptr)
		{
			r1.x=pBox->x-pBox->width/2;
			r1.y=pBox->y-pBox->height/2;
			r1.width=pBox->width;
			r1.height=pBox->height;

			r2.x=testBox.x-testBox.width/2;
			r2.y=testBox.y-testBox.height/2;
			r2.width=testBox.width;
			r2.height=testBox.height;

			if (PX_isRectCrossRect(r1,r2))
			{
				px_int i;
				for (i=0;i<pQuadtree->Impacts.size;i++)
				{
					if (PX_VECTORAT(PX_Quadtree_UserData,&pQuadtree->Impacts,i)->ptr==pBox->userdata.ptr)
					{
						break;
					}
				}

				if(i==pQuadtree->Impacts.size)
				PX_VectorPushback(&pQuadtree->Impacts,&pBox->userdata);
				
			}
		}
		pnode=pnode->pnext;
	}

}

static px_void PX_QuadtreeSortBox(PX_Quadtree *pQuadtree,PX_Quadtree_AABB *aabb,px_int index)
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
			PX_ListPush(&aabb->dataList,&index,sizeof(px_int));
		}
		else
		{
			PX_QuadtreeSortBox(pQuadtree,aabb->Area1,index);
			PX_QuadtreeSortBox(pQuadtree,aabb->Area2,index);
			PX_QuadtreeSortBox(pQuadtree,aabb->Area3,index);
			PX_QuadtreeSortBox(pQuadtree,aabb->Area4,index);
		}
	}
}


static px_void PX_QuadtreeTestBox(PX_Quadtree *pQuadtree,PX_Quadtree_AABB *aabb,PX_Quadtree_AABB_BOX Box)
{
	px_rect r1,r2;
	r1.x=Box.x-Box.width/2;
	r1.y=Box.y-Box.height/2;
	r1.width=Box.width;
	r1.height=Box.height;

	r2.x=aabb->Left;
	r2.y=aabb->Top;
	r2.width=aabb->Right-aabb->Left+1;
	r2.height=aabb->Bottom-aabb->Top+1;
	if (PX_isRectCrossRect(r1,r2))
	{
		if (aabb->deep>=pQuadtree->PX_AABB_MAX_DEEP)
		{
			PX_QuadtreeTest(pQuadtree,aabb,Box);
		}
		else
		{
			PX_QuadtreeTestBox(pQuadtree,aabb->Area1,Box);
			PX_QuadtreeTestBox(pQuadtree,aabb->Area2,Box);
			PX_QuadtreeTestBox(pQuadtree,aabb->Area3,Box);
			PX_QuadtreeTestBox(pQuadtree,aabb->Area4,Box);
		}
	}
}


px_void PX_QuadtreeAddNode(PX_Quadtree *pQuadtree,px_float x,px_float y,px_float width,px_float height,PX_Quadtree_UserData userData)
{
	PX_Quadtree_AABB_BOX box;
	box.height=height;
	box.width=width;
	box.x=x;
	box.y=y;
	box.userdata=userData;
	PX_VectorPushback(&pQuadtree->boxes,&box);

	PX_QuadtreeSortBox(pQuadtree,pQuadtree->AABB,pQuadtree->boxes.size-1);
}

px_void PX_QuadtreeTestNode(PX_Quadtree *pQuadtree,px_float x,px_float y,px_float width,px_float height,PX_Quadtree_UserData userData)
{
	PX_Quadtree_AABB_BOX box;
	box.height=height;
	box.width=width;
	box.x=x;
	box.y=y;
	box.userdata=userData;
	if (pQuadtree->boxes.size)
	{
		PX_QuadtreeTestBox(pQuadtree,pQuadtree->AABB,box);
	}
}

px_void PX_QuadtreeResetTest(PX_Quadtree *pQuadtree)
{
	PX_VectorClear(&pQuadtree->Impacts);
}
