#include "PX_Typedef.h"


static void px_triangle_copy_point( px_point2D* p, px_point f )
{
	p->x = f.x;
	p->y = f.y;
}


static px_float px_triangleget_vector4_det( px_point v1, px_point v2, px_point v3, px_point v4 )
{
	px_float a[3][3];

	a[0][0] = v1.x - v4.x;
	a[1][0] = v2.x - v4.x;
	a[2][0] = v3.x - v4.x;

	a[0][1] = v1.y - v4.y;
	a[1][1] = v2.y - v4.y;
	a[2][1] = v3.y - v4.y;

	a[0][2] = v1.z - v4.z;
	a[1][2] = v2.z - v4.z;
	a[2][2] = v3.z - v4.z;


	return a[0][0] * a[1][1] * a[2][2] 
	+ a[0][1] * a[1][2] * a[2][0] 
	+ a[0][2] * a[1][0] * a[2][1] 
	- a[0][2] * a[1][1] * a[2][0] 
	- a[0][1] * a[1][0] * a[2][2] 
	- a[0][0] * a[1][2] * a[2][1];
}


static px_double px_triangle_direction( px_point2D p1, px_point2D p2, px_point2D p ){
	return ( p.x - p1.x ) * ( p2.y - p1.y ) - ( p2.x - p1.x ) * ( p.y - p1.y )   ;
}


static px_int px_triangle_on_segment( px_point2D p1, px_point2D p2, px_point2D p ){
	px_double max = p1.x > p2.x ? p1.x : p2.x ;
	px_double min = p1.x < p2.x ? p1.x : p2.x ;
	px_double max1 = p1.y > p2.y ? p1.y : p2.y ;
	px_double min1 = p1.y < p2.y ? p1.y : p2.y ;
	if ( p.x >= min && p.x <= max && p.y >= min1 && p.y <= max1 )
	{
		return 1;
	}
	else
	{
		return 0;
	}
}


static px_int px_triangle_segments_intersert( px_point2D p1, px_point2D p2, px_point2D p3, px_point2D p4 ){
	px_double d1, d2, d3, d4;
	d1 = px_triangle_direction( p3, p4, p1 );
	d2 = px_triangle_direction( p3, p4, p2 );
	d3 = px_triangle_direction( p1, p2, p3 );
	d4 = px_triangle_direction( p1, p2, p4 );
	if ( d1 * d2 < 0 && d3 * d4 < 0 )
	{
		return 1;
	}
	else if ( d1 == 0 && px_triangle_on_segment( p3, p4, p1 ) == 1 )
	{
		return 1;
	}
	else if ( d2 == 0 && px_triangle_on_segment( p3, p4, p2 ) == 1 )
	{
		return 1;
	}
	else if ( d3 == 0 && px_triangle_on_segment( p1, p2, p3 ) == 1 )
	{
		return 1;
	}
	else if ( d4 == 0 && px_triangle_on_segment( p1, p2, p4 ) == 1 )
	{
		return 1;
	}
	return 0;
}

static px_bool px_triangle_line_triangle_intersert_inSamePlane( px_triangle* tri, px_point f1, px_point f2 )
{
	px_point2D p1, p2, p3, p4;

	px_triangle_copy_point( &p1, f1 );

	px_triangle_copy_point( &p2, f2 );

	px_triangle_copy_point( &p3, tri->Vertex_1 );

	px_triangle_copy_point( &p4, tri->Vertex_2 );

	if ( px_triangle_segments_intersert( p1, p2, p3, p4 ) )
	{
		return PX_TRUE;
	}

	px_triangle_copy_point( &p3, tri->Vertex_2 );

	px_triangle_copy_point( &p4, tri->Vertex_3 );

	if ( px_triangle_segments_intersert( p1, p2, p3, p4 ) )
	{
		return PX_TRUE;
	}

	px_triangle_copy_point( &p3, tri->Vertex_1 );

	px_triangle_copy_point( &p4, tri->Vertex_3 );

	if ( px_triangle_segments_intersert( p1, p2, p3, p4 ) )
	{
		return PX_TRUE;
	}

	return PX_FALSE;
} 


static void px_triangle_get_central_point(px_point *centralPoint,px_triangle* tri)
{
	centralPoint->x=(tri->Vertex_1.x+tri->Vertex_2.x+tri->Vertex_3.x)/3;

	centralPoint->y=(tri->Vertex_1.y+tri->Vertex_2.y+tri->Vertex_3.y)/3;

	centralPoint->z=(tri->Vertex_1.z+tri->Vertex_2.z+tri->Vertex_3.z)/3;
}



static px_point px_triangle_get_vector_diff( const px_point a, const px_point b )
{
	px_point result;

	result.x = b.x  - a.x ;

	result.y = b.y - a.y;

	result.z = b.z - a.z;
	return result;
} 

static px_bool px_triangle_is_point_within_triangle( px_triangle* tri, px_point pt )
{
	px_point v0=px_triangle_get_vector_diff(  tri->Vertex_1, tri->Vertex_3 );   
	px_point v1=px_triangle_get_vector_diff(  tri->Vertex_1, tri->Vertex_2 );  
	px_point v2=px_triangle_get_vector_diff(  tri->Vertex_1, pt );

	px_float dot00 = PX_PointDot( v0, v0 ) ;   
	px_float dot01 = PX_PointDot( v0, v1 ) ;    
	px_float dot02 = PX_PointDot( v0, v2 ) ;    
	px_float dot11 = PX_PointDot( v1, v1 ) ;   
	px_float dot12 = PX_PointDot( v1, v2 ) ;    
	px_float inverDeno = 1 / ( dot00* dot11 - dot01* dot01 ) ;    
	px_float u = ( dot11* dot02 - dot01* dot12 ) * inverDeno ;   
	px_float v;
	if ( u < 0 || u > 1 ) // if u out of range, return directly
	{
		return PX_FALSE ;
	}    
	v = ( dot00* dot12 - dot01* dot02 ) * inverDeno ;    
	if ( v < 0 || v > 1 ) // if v out of range, return directly
	{
		return PX_FALSE ;
	}    
	return u + v <= 1 ;
} 


static px_bool px_triangle_intersert_inSamePlane( px_triangle* tri1, px_triangle* tri2 )
{
	if ( px_triangle_line_triangle_intersert_inSamePlane( tri2, tri1->Vertex_1, tri1->Vertex_2 ) )
	{
		return PX_TRUE;
	}
	else if ( px_triangle_line_triangle_intersert_inSamePlane( tri2, tri1->Vertex_2, tri1->Vertex_3 ) )
	{
		return PX_TRUE;
	}
	else if ( px_triangle_line_triangle_intersert_inSamePlane( tri2, tri1->Vertex_1, tri1->Vertex_3 ) )
	{
		return PX_TRUE;
	}
	else
	{
		px_point centralPoint1,centralPoint2;

		px_triangle_get_central_point(&centralPoint1,tri1);

		px_triangle_get_central_point(&centralPoint2,tri2);

		if(px_triangle_is_point_within_triangle( tri2, centralPoint1 ) || px_triangle_is_point_within_triangle( tri1, centralPoint2 ) )
		{
			return PX_TRUE;
		}

		return PX_FALSE;
	}
} 

px_bool PX_TriangleIsCross( px_triangle* tri1, px_triangle* tri2 )
{
	px_float p1_tri2_vertex1 = px_triangleget_vector4_det( tri1->Vertex_1, tri1->Vertex_2, tri1->Vertex_3, tri2->Vertex_1 );

	px_float p1_tri2_vertex2 = px_triangleget_vector4_det( tri1->Vertex_1, tri1->Vertex_2, tri1->Vertex_3, tri2->Vertex_2 );

	px_float p1_tri2_vertex3 = px_triangleget_vector4_det( tri1->Vertex_1, tri1->Vertex_2, tri1->Vertex_3, tri2->Vertex_3 );


	px_float p2_tri1_vertex1;

	px_float p2_tri1_vertex2;

	px_float p2_tri1_vertex3;

	px_point tri1_a , tri1_b , tri1_c , tri2_a , tri2_b, tri2_c ;

	px_point m;
	px_float im;


	if ( p1_tri2_vertex1 > 0 && p1_tri2_vertex2 > 0 && p1_tri2_vertex3 > 0 )
	{
		return PX_FALSE;
	}

	if ( p1_tri2_vertex1 < 0 && p1_tri2_vertex2 < 0 && p1_tri2_vertex3 < 0 )
	{
		return PX_FALSE;
	}


	if ( p1_tri2_vertex1 == 0 && p1_tri2_vertex2 == 0 && p1_tri2_vertex3 == 0 )
	{
		if ( px_triangle_intersert_inSamePlane( tri1, tri2 ) )
		{
			return PX_TRUE;
		}
		else
		{
			return PX_FALSE;
		}
	}


	if ( p1_tri2_vertex1 == 0 && p1_tri2_vertex2 * p1_tri2_vertex3 > 0 )
	{
		if ( px_triangle_is_point_within_triangle( tri1, tri2->Vertex_1 ) )
		{
			return PX_TRUE;
		}
		else
		{
			return PX_FALSE;
		}
	}
	else if ( p1_tri2_vertex2 == 0 && p1_tri2_vertex1 * p1_tri2_vertex3 > 0 )
	{
		if ( px_triangle_is_point_within_triangle( tri1, tri2->Vertex_2 ) )
		{
			return PX_TRUE;
		}
		else
		{
			return PX_FALSE;
		}
	}
	else if ( p1_tri2_vertex3 == 0 && p1_tri2_vertex1 * p1_tri2_vertex2 > 0 )
	{
		if ( px_triangle_is_point_within_triangle( tri1, tri2->Vertex_3 ) )
		{
			return PX_TRUE;
		}
		else
		{
			return PX_FALSE;
		}
	}



	p2_tri1_vertex1 = px_triangleget_vector4_det( tri2->Vertex_1, tri2->Vertex_2, tri2->Vertex_3, tri1->Vertex_1 );

	p2_tri1_vertex2 = px_triangleget_vector4_det( tri2->Vertex_1, tri2->Vertex_2, tri2->Vertex_3, tri1->Vertex_2 );

	p2_tri1_vertex3 = px_triangleget_vector4_det( tri2->Vertex_1, tri2->Vertex_2, tri2->Vertex_3, tri1->Vertex_3 );


	if ( p2_tri1_vertex1 > 0 && p2_tri1_vertex2 > 0 && p2_tri1_vertex3 > 0 )
	{
		return PX_FALSE;
	}

	if ( p2_tri1_vertex1 < 0 && p2_tri1_vertex2 < 0 && p2_tri1_vertex3 < 0 )
	{
		return PX_FALSE;
	}


	if ( p2_tri1_vertex1 == 0 && p2_tri1_vertex2 * p2_tri1_vertex3 > 0 )
	{
		if ( px_triangle_is_point_within_triangle( tri2, tri1->Vertex_1 ) )
		{
			return PX_TRUE;
		}
		else
		{
			return PX_FALSE;
		}
	}

	if ( p2_tri1_vertex2 == 0 && p2_tri1_vertex1 * p2_tri1_vertex3 > 0 )
	{
		if ( px_triangle_is_point_within_triangle( tri2, tri1->Vertex_2 ) )
		{
			return PX_TRUE;
		}
		else
		{
			return PX_FALSE;
		}
	}

	if ( p2_tri1_vertex3 == 0 && p2_tri1_vertex1 * p2_tri1_vertex2 > 0 )
	{
		if ( px_triangle_is_point_within_triangle( tri2, tri1->Vertex_3 ) )
		{
			return PX_TRUE;
		}
		else
		{
			return PX_FALSE;
		}
	}



	tri1_a = tri1->Vertex_1, tri1_b = tri1->Vertex_2, tri1_c = tri1->Vertex_3
		, tri2_a = tri2->Vertex_1, tri2_b = tri2->Vertex_2, tri2_c = tri2->Vertex_3;


	if ( p2_tri1_vertex2 * p2_tri1_vertex3 >= 0 && p2_tri1_vertex1 != 0 )
	{
		if ( p2_tri1_vertex1 < 0 )
		{
			m = tri2_b;
			tri2_b = tri2_c;
			tri2_c = m;

			im = p1_tri2_vertex2;
			p1_tri2_vertex2 = p1_tri2_vertex3;
			p1_tri2_vertex3 = im;
		}
	}
	else if ( p2_tri1_vertex1 * p2_tri1_vertex3 >= 0 && p2_tri1_vertex2 != 0 )
	{
		m = tri1_a;
		tri1_a = tri1_b;
		tri1_b = tri1_c;
		tri1_c = m;

		if ( p2_tri1_vertex2 < 0 )
		{
			m = tri2_b;
			tri2_b = tri2_c;
			tri2_c = m;

			im = p1_tri2_vertex2;
			p1_tri2_vertex2 = p1_tri2_vertex3;
			p1_tri2_vertex3 = im;
		}
	}
	else if ( p2_tri1_vertex1 * p2_tri1_vertex2 >= 0 && p2_tri1_vertex3 != 0 )
	{
		m = tri1_a;

		tri1_a = tri1_c;

		tri1_c = tri1_b;

		tri1_b = m;

		if ( p2_tri1_vertex3 < 0 )
		{
			m = tri2_b;
			tri2_b = tri2_c;
			tri2_c = m;

			im = p1_tri2_vertex2;
			p1_tri2_vertex2 = p1_tri2_vertex3;
			p1_tri2_vertex3 = im;
		}
	}

	if ( p1_tri2_vertex2 * p1_tri2_vertex3 >= 0 && p1_tri2_vertex1 != 0 )
	{
		if ( p1_tri2_vertex1 < 0 )
		{
			m = tri1_b;
			tri1_b = tri1_c;
			tri1_c = m;
		}
	}
	else if ( p1_tri2_vertex1 * p1_tri2_vertex3 >= 0 && p1_tri2_vertex2 != 0 )
	{
		m = tri2_a;

		tri2_a = tri2_b;

		tri2_b = tri2_c;

		tri2_c = m;

		if ( p1_tri2_vertex2 < 0 )
		{
			m = tri1_b;
			tri1_b = tri1_c;
			tri1_c = m;
		}
	}
	else if ( p1_tri2_vertex1 * p1_tri2_vertex2 >= 0 && p1_tri2_vertex3 != 0 )
	{
		m = tri2_a;

		tri2_a = tri2_c;

		tri2_c = tri2_b;

		tri2_b = m;

		if ( p1_tri2_vertex3 < 0 )
		{
			m = tri1_b;
			tri1_b = tri1_c;
			tri1_c = m;
		}
	}

	if ( px_triangleget_vector4_det( tri1_a, tri1_b, tri2_a, tri2_b ) <= 0 && px_triangleget_vector4_det( tri1_a, tri1_c, tri2_c, tri2_a ) <= 0 )
	{
		return PX_TRUE;
	}
	else
	{
		return PX_TRUE;
	}
}