#ifndef PX_DELAUNAY_H
#define PX_DELAUNAY_H
#include "PX_3D.h"

typedef struct
{
	px_int index1;
	px_int index2;
}PX_Delaunay_Edge;

typedef struct
{
	px_int index1;
	px_int index2;
	px_int index3;
}PX_Delaunay_Triangle;

typedef enum
{
	PX_DELAUNAY_RETURN_TYPE_TRIANGLE,
	PX_DELAUNAY_RETURN_TYPE_TRIANGLE_INDEX,
}PX_DELAUNAY_RETURN_TYPE;
px_bool PX_DelaunaryPointsBuild(px_memorypool *mp,px_point2D pt[],px_int count,px_vector *out_triangles,PX_DELAUNAY_RETURN_TYPE type);

#endif
