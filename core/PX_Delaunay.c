#include "PX_Delaunay.h"
#include "PX_Quicksort.h"


typedef enum
{
	PX_DELAUNAY_ROUND_TEST_INSIDE,
	PX_DELAUNAY_ROUND_TEST_OUTSIDE,
	PX_DELAUNAY_ROUND_TEST_DELAUNAY,
}PX_DELAUNAY_ROUND_TEST;

px_bool PX_DelaunaryVerifyCircle(px_point2D pt[],px_int p1,px_int p2,px_int p3)
{
	if (pt[p1].x==pt[p2].x&&pt[p1].x==pt[p3].x)
	{
		return PX_FALSE;
	}
	if ((pt[p1].y-pt[p2].y)/(pt[p1].x-pt[p2].x)==(pt[p1].y-pt[p3].y)/(pt[p1].x-pt[p3].x))
	{
		return PX_FALSE;
	}
	return PX_TRUE;
}

PX_DELAUNAY_ROUND_TEST PX_DelaunaryTest(px_point2D pt_array[],px_int testIndex,PX_Delaunay_Triangle triangle)
{
	px_float x,y,r;
	px_float x1,x2,x3,y1,y2,y3;
	px_float testx,testy;
	px_float d;
	x1=pt_array[triangle.index1].x;
	y1=pt_array[triangle.index1].y;

	x2=pt_array[triangle.index2].x;
	y2=pt_array[triangle.index2].y;


	x3=pt_array[triangle.index3].x;
	y3=pt_array[triangle.index3].y;

	testx=pt_array[testIndex].x;
	testy=pt_array[testIndex].y;

	x=((y2-y1)*(y3*y3-y1*y1+x3*x3-x1*x1)-(y3-y1)*(y2*y2-y1*y1+x2*x2-x1*x1))/(2.0f*((x3-x1)*(y2-y1)-(x2-x1)*(y3-y1)));
	y=((x2-x1)*(x3*x3-x1*x1+y3*y3-y1*y1)-(x3-x1)*(x2*x2-x1*x1+y2*y2-y1*y1))/(2.0f*((y3-y1)*(x2-x1)-(y2-y1)*(x3-x1)));
	r=PX_sqrt((x1-x)*(x1-x)+(y1-y)*(y1-y));

	d=((testx-x)*(testx-x)+(testy-y)*(testy-y));

	if (d>r*r)
	{
		if (testx>x+r)
		{
			return PX_DELAUNAY_ROUND_TEST_DELAUNAY;
		}
		else
		{
			return PX_DELAUNAY_ROUND_TEST_OUTSIDE;
		}
	}
	else
	{
		return PX_DELAUNAY_ROUND_TEST_INSIDE;
	}
}

px_bool PX_DelaunaryTriangleNeighbor(PX_Delaunay_Triangle *t1,PX_Delaunay_Triangle *t2)
{
	px_int c1_3[3];
	px_int c2_3[3];
	px_int i,j,k=0;
	c1_3[0]=t1->index1;
	c1_3[1]=t1->index2;
	c1_3[2]=t1->index3;

	c2_3[0]=t2->index1;
	c2_3[1]=t2->index2;
	c2_3[2]=t2->index3;

	for (i=0;i<3;i++)
	{
		for (j=0;j<3;j++)
		{
			if (c1_3[i]==c2_3[j])
			{
				k++;
			}
		}
	}
	return k==2;
}

px_void PX_DelaunaryTriangleSwitch(PX_Delaunay_Triangle *t1,PX_Delaunay_Triangle *t2)
{
	px_int c1_3[3];
	px_int c2_3[3];
	px_int c[3],d[3];
	px_int j,k;
	c1_3[0]=t1->index1;
	c1_3[1]=t1->index2;
	c1_3[2]=t1->index3;

	c2_3[0]=t2->index1;
	c2_3[1]=t2->index2;
	c2_3[2]=t2->index3;

	j=0;k=0;
	if (c1_3[0]==c2_3[0]||c1_3[0]==c2_3[1]||c1_3[0]==c2_3[2])
	{
		c[j]=c1_3[0];
		j++;
	}
	else
	{
		d[k]=c1_3[0];
		k++;
	}

	if (c1_3[1]==c2_3[0]||c1_3[1]==c2_3[1]||c1_3[1]==c2_3[2])
	{
		c[j]=c1_3[1];
		j++;
	}
	else
	{
		d[k]=c1_3[1];
		k++;
	}
	
	if (c1_3[2]==c2_3[0]||c1_3[2]==c2_3[1]||c1_3[2]==c2_3[2])
	{
		c[j]=c1_3[2];
		j++;
	}
	else
	{
		d[k]=c1_3[2];
		k++;
	}

	if (c2_3[0]!=c1_3[0]&&c2_3[0]!=c1_3[1]&&c2_3[0]!=c1_3[2])
	{
		d[k]=c2_3[0];
	}

	if (c2_3[1]!=c1_3[0]&&c2_3[1]!=c1_3[1]&&c2_3[1]!=c1_3[2])
	{
		d[k]=c2_3[1];
	}

	if (c2_3[2]!=c1_3[0]&&c2_3[2]!=c1_3[1]&&c2_3[2]!=c1_3[2])
	{
		d[k]=c2_3[2];
	}

	t1->index1=c[0];
	t1->index2=d[1];
	t1->index3=d[2];

	t2->index1=c[1];
	t2->index2=d[1];
	t2->index3=d[2];

}

px_bool PX_DelaunaryPointsBuild(px_memorypool *mp,px_point2D pt[],px_int count,px_vector *out_triangles,PX_DELAUNAY_RETURN_TYPE type)
{
	
	PX_QuickSortAtom *pAtoms;
	px_point2D *sortPt;
	px_int *mapPt;
	//edge
	px_float minx,miny,maxx,maxy=0;
	px_int i,j;
	px_vector edges;
	px_vector triangles;
	px_vector DelaunaryTriangle;


	PX_VectorInitialize(mp,&triangles,sizeof(PX_Delaunay_Triangle),1);
	PX_VectorInitialize(mp,&DelaunaryTriangle,sizeof(PX_Delaunay_Triangle),1);
	PX_VectorInitialize(mp,&edges,sizeof(PX_Delaunay_Edge),1);

	pAtoms=(PX_QuickSortAtom *)MP_Malloc(mp,sizeof(PX_QuickSortAtom)*count);
	sortPt=(px_point2D *)MP_Malloc(mp,sizeof(px_point2D)*(count+3));
	mapPt=(px_int *)MP_Malloc(mp,sizeof(px_int)*(count));

	if (!pAtoms)
	{
		return PX_FALSE;
	}
	for (i=0;i<count;i++)
	{
		pAtoms[i].pData=pt+i;
		pAtoms[i].weight=pt[i].x;
	}
	PX_Quicksort_MinToMax(pAtoms,0,count-1);
	for (i=0;i<count;i++)
	{
		sortPt[i]=*(px_point2D *)pAtoms[i].pData;
		mapPt[i]=(px_int)(((px_byte *)pAtoms[i].pData-(px_byte *)pt)/sizeof(px_point2D));
	}
	MP_Free(mp,pAtoms);
	//build huge triangle
	minx=sortPt[0].x;
	maxx=sortPt[count-1].x;

	miny=sortPt[0].y;
	maxy=sortPt[0].y;
	for (i=0;i<count;i++)
	{
		if (sortPt[i].y>maxy)
		{
			maxy=sortPt[i].y;
		}

		if (sortPt[i].y<miny)
		{
			miny=sortPt[i].y;
		}
	}

	minx-=1;
	maxx+=1;
	miny-=1;
	maxy+=1;

	do 
	{
		px_point2D *tpt=&sortPt[count];
		px_float hl=maxx-minx;
		px_float vl=maxy-miny;
		PX_Delaunay_Triangle temp_triangle;
		
		tpt[0].x=minx+(maxx-minx)/2;
		tpt[0].y=miny-hl/2;


		tpt[1].x=minx+(maxx-minx)/2-hl/2-vl;
		tpt[1].y=maxy;

		tpt[2].x=minx+(maxx-minx)/2+hl/2+vl;
		tpt[2].y=maxy;
	
		temp_triangle.index1=count;
		temp_triangle.index2=count+1;
		temp_triangle.index3=count+2;
		
		PX_VectorPushback(&triangles,&temp_triangle);
	} while (0);


	for (i=0;i<count;i++)
	{
		PX_VectorClear(&edges);

		for (j=0;j<triangles.size;j++)
		{
			PX_Delaunay_Triangle temp_triangle;

			temp_triangle=*PX_VECTORAT(PX_Delaunay_Triangle,&triangles,j);

			switch (PX_DelaunaryTest(sortPt,i,temp_triangle))
			{
			case PX_DELAUNAY_ROUND_TEST_OUTSIDE:
				break;
			case PX_DELAUNAY_ROUND_TEST_INSIDE:
				{
					PX_Delaunay_Edge edge;
					PX_VectorErase(&triangles,j);
					edge.index1=temp_triangle.index1;
					edge.index2=temp_triangle.index2;
					PX_VectorPushback(&edges,&edge);

					edge.index1=temp_triangle.index2;
					edge.index2=temp_triangle.index3;
					PX_VectorPushback(&edges,&edge);

					edge.index1=temp_triangle.index3;
					edge.index2=temp_triangle.index1;
					PX_VectorPushback(&edges,&edge);
					j--;
				}
				break;
			case PX_DELAUNAY_ROUND_TEST_DELAUNAY:
				{
					PX_VectorPushback(&DelaunaryTriangle,&temp_triangle);
					PX_VectorErase(&triangles,j);
					j--;
				}
				break;
			}
		}

		//remove repeated edges
		for (j=0;j<edges.size-1;j++)
		{
			px_int k;
			px_bool bremove=PX_FALSE;
			PX_Delaunay_Edge *pEdge=PX_VECTORAT(PX_Delaunay_Edge,&edges,j);
			for (k=j+1;k<edges.size;k++)
			{
				PX_Delaunay_Edge *pCompareEdge=PX_VECTORAT(PX_Delaunay_Edge,&edges,k);
				if (pEdge->index1==pCompareEdge->index1&&pEdge->index2==pCompareEdge->index2)
				{
					PX_VectorErase(&edges,k);
					k--;
					bremove=PX_TRUE;
				}
				if (pEdge->index1==pCompareEdge->index2&&pEdge->index2==pCompareEdge->index1)
				{
					PX_VectorErase(&edges,k);
					k--;
					bremove=PX_TRUE;
				}
			}
			if (bremove)
			{
				PX_VectorErase(&edges,j);
				j--;
			}
		}

		//generate triangles
		for (j=0;j<edges.size;j++)
		{
			PX_Delaunay_Triangle gtrianle;
			PX_Delaunay_Edge *pEdge=PX_VECTORAT(PX_Delaunay_Edge,&edges,j);

			if(PX_DelaunaryVerifyCircle(sortPt,i,pEdge->index1,pEdge->index2))
			{
				gtrianle.index1=i;
				gtrianle.index2=pEdge->index1;
				gtrianle.index3=pEdge->index2;

				PX_VectorPushback(&triangles,&gtrianle);
			}
		}
	}

	for (i=0;i<triangles.size;i++)
	{
		PX_Delaunay_Triangle *pTriangle=PX_VECTORAT(PX_Delaunay_Triangle,&triangles,i);
		PX_VectorPushback(&DelaunaryTriangle,pTriangle);
	}

	//Concave update
	do 
	{
		px_bool done=PX_FALSE;
		px_int index;
		px_vector sortTriangles;
		PX_VectorInitialize(mp,&sortTriangles,sizeof(PX_Delaunay_Triangle *),1);
		for (i=0;i<DelaunaryTriangle.size;i++)
		{
			px_int k=0;
			PX_Delaunay_Triangle *pTriangle=PX_VECTORAT(PX_Delaunay_Triangle,&DelaunaryTriangle,i);
			if (pTriangle->index1-count>=0)
			{
				k++;
			}
			if (pTriangle->index2-count>=0)
			{
				k++;
			}
			if (pTriangle->index3-count>=0)
			{
				k++;
			}
			if (k==1)
			{
				PX_VectorPushback(&sortTriangles,&pTriangle);
			}
		}

		index=0;

		while (index<sortTriangles.size)
		{
			PX_Delaunay_Triangle *pTriangle1=*PX_VECTORAT(PX_Delaunay_Triangle *,&sortTriangles,index);
			for (i=index+1;i<sortTriangles.size;i++)
			{
					PX_Delaunay_Triangle *pTriangle2=*PX_VECTORAT(PX_Delaunay_Triangle *,&sortTriangles,i);
					if (PX_DelaunaryTriangleNeighbor(pTriangle1,pTriangle2))
					{
						px_int pO=0,pA=0,pB=0,pC=0;
						px_point t1v,t2v,t3v;

						if(pTriangle1->index1!=pTriangle2->index1&&pTriangle1->index1!=pTriangle2->index2&&pTriangle1->index1!=pTriangle2->index3)
							pA=pTriangle1->index1;
						else
						if(pTriangle1->index2!=pTriangle2->index1&&pTriangle1->index2!=pTriangle2->index2&&pTriangle1->index2!=pTriangle2->index3)
							pA=pTriangle1->index2;
						else
							pA=pTriangle1->index3;

						if(pTriangle2->index1!=pTriangle1->index1&&pTriangle2->index1!=pTriangle1->index2&&pTriangle2->index1!=pTriangle1->index3)
							pB=pTriangle2->index1;
						else
							if(pTriangle2->index2!=pTriangle1->index1&&pTriangle2->index2!=pTriangle1->index2&&pTriangle2->index2!=pTriangle1->index3)
								pB=pTriangle2->index2;
							else
								pB=pTriangle2->index3;

						if(pTriangle1->index1-count>=0)
							pC=pTriangle1->index1;

						if(pTriangle1->index2-count>=0)
							pC=pTriangle1->index2;

						if(pTriangle1->index3-count>=0)
							pC=pTriangle1->index3;


						if(pTriangle1->index1-count<0&&pTriangle1->index1!=pA)
							pO=pTriangle1->index1;

						if(pTriangle1->index2-count<0&&pTriangle1->index2!=pA)
							pO=pTriangle1->index2;

						if(pTriangle1->index3-count<0&&pTriangle1->index3!=pA)
							pO=pTriangle1->index3;

						t1v.x=sortPt[pO].x-sortPt[pA].x;
						t1v.y=sortPt[pO].y-sortPt[pA].y;
						t1v.z=0;

						t2v.x=sortPt[pB].x-sortPt[pO].x;
						t2v.y=sortPt[pB].y-sortPt[pO].y;
						t2v.z=0;

						t3v.x=sortPt[pC].x-sortPt[pB].x;
						t3v.y=sortPt[pC].y-sortPt[pB].y;
						t3v.z=0;

						if (PX_PointCross(t1v,t2v).z*PX_PointCross(t2v,t3v).z>0)
						{
							pTriangle1->index1=pC;
							pTriangle1->index2=pA;
							pTriangle1->index3=pB;

							pTriangle2->index1=pO;
							pTriangle2->index2=pA;
							pTriangle2->index3=pB;


							PX_VectorErase(&sortTriangles,i);
							index=-1;
							break;
						}
					}
			}
			index++;
		}
		PX_VectorFree(&sortTriangles);
	} while (0);
	


	for (i=0;i<DelaunaryTriangle.size;i++)
	{
 		PX_Delaunay_Triangle *pTriangle=PX_VECTORAT(PX_Delaunay_Triangle,&DelaunaryTriangle,i);
		if (pTriangle->index1-count>=0||pTriangle->index2-count>=0||pTriangle->index3-count>=0)
		{
			PX_VectorErase(&DelaunaryTriangle,i);
			i--;
		}
		else
		{
			switch (type)
			{
			case PX_DELAUNAY_RETURN_TYPE_TRIANGLE:
				{
					px_triangle dtriangle;
					dtriangle.Vertex_1.x=sortPt[pTriangle->index1].x;
					dtriangle.Vertex_1.y=sortPt[pTriangle->index1].y;
					dtriangle.Vertex_1.z=0;

					dtriangle.Vertex_2.x=sortPt[pTriangle->index2].x;
					dtriangle.Vertex_2.y=sortPt[pTriangle->index2].y;
					dtriangle.Vertex_2.z=0;

					dtriangle.Vertex_3.x=sortPt[pTriangle->index3].x;
					dtriangle.Vertex_3.y=sortPt[pTriangle->index3].y;
					dtriangle.Vertex_3.z=0;

					PX_VectorPushback(out_triangles,&dtriangle);
				}
				break;
			case PX_DELAUNAY_RETURN_TYPE_TRIANGLE_INDEX:
				{
					PX_Delaunay_Triangle mapTriangleIndex;
					mapTriangleIndex.index1=mapPt[pTriangle->index1];
					mapTriangleIndex.index2=mapPt[pTriangle->index2];
					mapTriangleIndex.index3=mapPt[pTriangle->index3];
					PX_VectorPushback(out_triangles,&mapTriangleIndex);
				}
				break;
			default:
				break;
			}
			
		}
	}

	PX_VectorFree(&triangles);
	PX_VectorFree(&DelaunaryTriangle);
	PX_VectorFree(&edges);
	MP_Free(mp,mapPt);
	MP_Free(mp,sortPt);
	return PX_TRUE;
}

