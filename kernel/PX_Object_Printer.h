#ifndef PX_OBJECT_PRINTER_H
#define PX_OBJECT_PRINTER_H

#include "PX_Object.h"

#define PX_OBJECT_PRINTER_DEFAULT_MAX_COLUMN 128
#define PX_OBJECT_PRINTER_INPUT_CONTENT_SIZE 1024

typedef struct
{
	PX_Object* Area;
	PX_Object* Input;
	px_int max_column;
	px_int column;
	px_bool show;
	px_vector pObjects;
	px_int id;
	px_bool bInput;
	PX_FontModule* fm;
}PX_Object_Printer;

typedef struct
{
	px_int id;
	PX_Object* Object;
}PX_Object_PrinterLine;

PX_Object* PX_Object_PrinterCreate(px_memorypool* mp, PX_Object* Parent, px_int x, px_int y, px_int width, px_int height,PX_FontModule *fm);
PX_Object* PX_Object_PrinterPrintText(PX_Object* pObject, const px_char* text);
PX_Object* PX_Object_PrinterPrintImage(PX_Object* pObject, px_texture* pTexture);
PX_Object* PX_Object_PrinterGetObject(PX_Object* pObject, px_int id);
PX_Object* PX_Object_PrinterLastPrintText(PX_Object* pObject, const px_char* text);
px_void	   PX_Object_PrinterGets(PX_Object* pObject);
px_void    PX_Object_PrinterClear(PX_Object* pObject);
px_int	   PX_Object_PrinterGetLastCreateId(PX_Object* pObject);

#endif