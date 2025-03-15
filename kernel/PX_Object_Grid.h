#ifndef PX_OBJECT_GRID_H
#define PX_OBJECT_GRID_H
#include "PX_Object.h"

typedef struct
{
	px_int startx;
	px_int starty;
	px_int xsize;
	px_int ysize;
	PX_Object* object;
}PX_Object_GridCell;

typedef struct
{
	px_memorypool *mp;
	px_int xcount;
	px_int ycount;
	px_float xsize, ysize;
	px_color bordercolor;
	PX_Object_GridCell* cell;
	px_vector objects;
}PX_Object_grid;

PX_Object * PX_Object_GridCreate(px_memorypool *mp, PX_Object *Parent, px_int x, px_int y, px_int xcount, px_int ycount, px_float xsize, px_float ysize);
px_void PX_Object_GridClear(PX_Object * pObject);
px_void PX_Object_GridClearRange(PX_Object* pObject,px_int x,px_int y,px_int xcount,px_int ycount);
px_void PX_Object_GridVisibleRange(PX_Object *pObject, px_int x, px_int y, px_int xcount, px_int ycount,px_bool visible);
PX_Object* PX_Object_GridSetLabel(PX_Object *pObject, px_int x, px_int y,px_int xsize,px_int ysize,const px_char label[], PX_FontModule* fontmodule);
PX_Object* PX_Object_GridSetSelectBar(PX_Object *pObject, px_int x, px_int y, px_int xsize, px_int ysize, PX_FontModule* fontmodule);
PX_Object* PX_Object_GridSetButton(PX_Object *pObject, px_int x, px_int y, px_int xsize, px_int ysize, const px_char label[], PX_FontModule* fontmodule);
PX_Object* PX_Object_GridSetSliderBar(PX_Object *pObject, px_int x, px_int y, px_int xsize, px_int ysize, PX_FontModule* fontmodule);
PX_Object* PX_Object_GridSetEdit(PX_Object *pObject, px_int x, px_int y, px_int xsize, px_int ysize, PX_FontModule* fontmodule);
PX_Object* PX_Object_GridSetCheckBox(PX_Object* pObject, px_int x, px_int y, px_int xsize, px_int ysize, const px_char content[], PX_FontModule* fontmodule);
PX_Object* PX_Object_GridSetImage(PX_Object *pObject, px_int x, px_int y, px_int xsize, px_int ysize);
PX_Object* PX_Object_GridSetEmpty(PX_Object* pObject, px_int x, px_int y, px_int xsize, px_int ysize);
PX_Object* PX_Object_GridGetObject(PX_Object* pObject, px_int x, px_int y);
const px_char* PX_Object_GridGetText(PX_Object* pObject, px_int x, px_int y);
px_void PX_Object_GridSetText(PX_Object* pObject, px_int x, px_int y, const px_char text[]);
px_void PX_Object_GridSetIndex(PX_Object* pObject, px_int x, px_int y, px_int index);
px_int PX_Object_GridGetSelectIndex(PX_Object* pObject, px_int x, px_int y);
px_bool PX_Object_GridExportToAbi(PX_Object* pObject, px_abi* pAbi);
px_bool PX_Object_GridImportFromAbi(PX_Object* pObject, px_abi* pAbi);
#endif // !PX_OBJECT_GRID_H
