#ifndef PX_OBJECT_LABELCONTENT_H
#define PX_OBJECT_LABELCONTENT_H
#include "PX_Object.h"

typedef enum 
{
	PX_OBJECT_VARIOUS_EDIT_TYPE_INT,
	PX_OBJECT_VARIOUS_EDIT_TYPE_FLOAT,
	PX_OBJECT_VARIOUS_EDIT_TYPE_STRING,
	PX_OBJECT_VARIOUS_EDIT_TYPE_HEX,
}PX_OBJECT_VARIOUS_EIDT_TYPE;


typedef enum
{
	PX_OBJECT_VARIOUS_TYPE_LABEL,
	PX_OBJECT_VARIOUS_TYPE_EDIT,
	PX_OBJECT_VARIOUS_TYPE_SELECTBAR,
}PX_OBJECT_VARIOUS_TYPE;

typedef struct 
{
	PX_Object* label;
	PX_Object* various;
	px_bool editable;
	PX_OBJECT_VARIOUS_TYPE type;
}PX_Object_Various;

px_void PX_Object_VariousEditSetMaxLength(PX_Object* pObject, px_int len);
PX_Object* PX_Object_VariousCreate(px_memorypool* mp, PX_Object* Parent, px_int x, px_int y, px_int Width, px_int Height, const px_char* Text, PX_OBJECT_VARIOUS_TYPE type, PX_FontModule* fm);
PX_Object_Various* PX_Object_GetVarious(PX_Object* pObject);
px_void PX_Object_VariousSetEditStyle(PX_Object* pObject, PX_OBJECT_VARIOUS_EIDT_TYPE type);


const px_char* PX_Object_VariousGetText(PX_Object* pObject);
const px_char* PX_Object_VariousGetLabelText(PX_Object* pObject);
px_int PX_Object_VariousSelectBarGetCurrentIndex(PX_Object* pObject);
px_void	 PX_Object_VariousSetText(PX_Object* pObject, const px_char* Text);
px_void	 PX_Object_VariousAddItem(PX_Object* pObject, const px_char* ItemText);

px_void  PX_Object_VariousSetLabelText(PX_Object* pObject, const px_char* Text);
px_void		PX_Object_VariousSetTextColor(PX_Object* pObject, px_color Color);
px_void		PX_Object_VariousSetBackgroundColor(PX_Object* pObject, px_color Color);
px_void		PX_Object_VariousSetAlign(PX_Object* pObject, PX_ALIGN Align);

px_void	 PX_Object_VariousSetString(PX_Object* pObject, const px_char* Text);
px_void	 PX_Object_VariousSetFloat(PX_Object* pObject, px_float f);
px_void	 PX_Object_VariousSetInt(PX_Object* pObject, px_int i);
px_void PX_Object_VariousSetHex(PX_Object* pObject, px_dword i);
px_void	 PX_Object_VariousSetBool(PX_Object* pObject, px_bool b);
#endif