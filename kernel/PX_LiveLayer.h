#ifndef PX_LIVELAYER_H
#define PX_LIVELAYER_H
#include "../core/PX_Core.h"
#include "PX_Animation.h"
#include "PX_Json.h"

#define  PX_LIVE_ID_MAX_LEN 32

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

typedef struct  
{
	px_char id[PX_LIVE_ID_MAX_LEN];
	px_texture Texture;
	px_int textureOffsetX;
	px_int textureOffsetY;
}PX_LiveTexture;


struct _PX_LiveLayer
{
	px_char id[PX_LIVE_ID_MAX_LEN];

	px_vector parent_Nodes;
	px_vector children_Nodes;

	px_point keyPoint;//static
	px_point linkPoint;

	px_point translation;//dynamic
	px_float rotationAngle;

	px_vector vertices;
	px_vector triangles;

	PX_TEXTURERENDER_BLEND blend;
	px_bool visible;
	px_bool showLinker;
	px_bool showMesh;

	PX_LiveTexture *pLiveTexture;
};
typedef struct _PX_LiveLayer PX_LiveLayer;

#endif

