#include "PX_PointsMesh.h"


px_bool PX_PointsMeshBuild(px_memorypool *mp,px_point2D limit_line_pt[],px_int line_pt_count,px_point2D pt[],px_int pt_count,px_vector *out_triangles,PX_DELAUNAY_RETURN_TYPE type)
{
	px_int i;
	px_float left=0,top=0,right=0,bottom=0;
	px_float offsetx,offsety,width,height;
	px_texture testTexture;
	if (pt_count<2)
	{
		return PX_FALSE;
	}
	
	left=pt[0].x;
	right=pt[0].x;
	bottom=pt[0].y;
	top=pt[0].y;


	for (i=0;i<pt_count;i++)
	{
		if (pt[i].x<left)
		{
			left=pt[i].x;
		}
		if (pt[i].x>right)
		{
			right=pt[i].x;
		}

		if (pt[i].y<top)
		{
			top=pt[i].y;
		}
		if (pt[i].y>bottom)
		{
			bottom=pt[i].y;
		}
	}

	left-=2;
	top-=2;
	right+=2;
	bottom+=2;

	offsetx=-left;
	offsety=-top;

	width=right-left;
	height=bottom-top;

	
	PX_TextureCreate(mp,&testTexture,(px_int)(width+1),(px_int)(height+1));
	for (i=1;i<line_pt_count;i++)
	{
		PX_GeoDrawBresenhamLine(&testTexture,(px_int)(limit_line_pt[i-1].x+offsetx),(px_int)(limit_line_pt[i-1].y+offsety),(px_int)(limit_line_pt[i].x+offsetx),(px_int)(limit_line_pt[i].y+offsety),PX_COLOR(255,0,0,0));
	}
	PX_TextureFill(mp,&testTexture,0,0,PX_COLOR(0,0,0,0),PX_COLOR(0,255,255,255));

	if(!PX_DelaunaryPointsBuild(mp,pt,pt_count,out_triangles,type))
	{
		PX_TextureFree(&testTexture);
		return PX_FALSE;
	}

	for (i=0;i<out_triangles->size;i++)
	{
		px_int testx=0,testy=0;

		switch(type)
		{
		case PX_DELAUNAY_RETURN_TYPE_TRIANGLE:
			{
				px_triangle *ptriangle=PX_VECTORAT(px_triangle,out_triangles,i);
				testx=(px_int)((ptriangle->Vertex_1.x+ptriangle->Vertex_2.x+ptriangle->Vertex_3.x)/3+offsetx);
				testy=(px_int)((ptriangle->Vertex_1.y+ptriangle->Vertex_2.y+ptriangle->Vertex_3.y)/3+offsety);
			}
			break;
		case PX_DELAUNAY_RETURN_TYPE_TRIANGLE_INDEX:
			{
				PX_Delaunay_Triangle *ptriangle=PX_VECTORAT(PX_Delaunay_Triangle,out_triangles,i);
				px_point2D v1,v2,v3;
				v1=pt[ptriangle->index1];
				v2=pt[ptriangle->index2];
				v3=pt[ptriangle->index3];
				testx=(px_int)((v1.x+v2.x+v3.x)/3+offsetx);
				testy=(px_int)((v1.y+v2.y+v3.y)/3+offsety);
			}
			break;
		}

		if (PX_SurfaceGetPixel(&testTexture,testx,testy)._argb.ucolor==0x00ffffff)
		{
			//remove
			PX_VectorErase(out_triangles,i);
			i--;
		}
	}
	PX_TextureFree(&testTexture);
	return PX_TRUE;
}

