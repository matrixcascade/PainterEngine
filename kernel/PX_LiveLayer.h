#ifndef PX_LIVELAYER_H
#define PX_LIVELAYER_H
#include "../core/PX_Core.h"
#include "PX_Animation.h"
#include "PX_Json.h"

typedef struct
{
	px_point position;
	px_float weight;
}PX_LiveVertex;

typedef struct
{
	px_int index1;
	px_int index2;
	px_int index3;
}PX_LiveTriangle;


typedef enum 
{
	PX_LIVE_DATA_TYPE_TEXTURE,
	PX_LIVE_DATA_TYPE_ANIMATION,
}PX_LIVE_DATA_TYPE;

struct _PX_LiveLayer
{
	px_vector parent_Nodes;
	px_vector children_Nodes;

	px_point TextureOffset;//static
	px_point keyPoint;//static
	px_point controlPoint;//static

	px_point currentKeyPoint;//dynamic
	px_point currentControlPoint;//dynamic
	px_vector vertices;
	px_vector triangles;

	px_bool showLinker;
	px_bool showMesh;


	PX_LIVE_DATA_TYPE type;
	px_void *data;
};
typedef struct _PX_LiveLayer PX_LiveLayer;

typedef struct
{
	px_int root_node;
	px_point transform;
	px_float rotation;
	px_dword elpased;
}PX_LiveAnimationKey;

typedef struct
{
	px_list AnimationKeys;
}PX_LiveAnimationArray;

typedef struct
{
	px_char Name[32];
	px_vector AnimationArrays;//PX_LiveAnimationKey
	px_dword elpased;
}PX_LiveAnimation;
#endif

