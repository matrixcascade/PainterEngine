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

	px_point offset;
	px_point keyPoint;
	px_point controlPoint;
	px_vector vertices;
	px_vector triangles;
	PX_LIVE_DATA_TYPE type;
	union
	{
		px_texture texture;
		PX_Animation animation;
	};
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

