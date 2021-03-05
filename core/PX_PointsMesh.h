#ifndef PX_POINTSMESH_H
#define PX_POINTSMESH_H

#include "PX_Delaunay.h"
#include "PX_Texture.h"

px_bool PX_PointsMeshBuild(px_memorypool *mp,px_point2D limit_line_pt[],px_int line_pt_count,px_point2D pt[],px_int pt_count,px_vector *out_triangles,PX_DELAUNAY_RETURN_TYPE type);


#endif
