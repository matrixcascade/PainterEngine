#ifndef PX_DELAUNAY_H
#define PX_DELAUNAY_H
#include "PX_3D.h"

px_bool PX_DelaunaryPointsBuild(px_memorypool *mp,px_point2D pt[],px_int count,px_vector *out_triangles);

#endif