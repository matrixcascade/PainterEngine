#ifndef __PX_STRING_H
#define __PX_STRING_H
#include "PX_MemoryPool.h"

typedef struct __px_string
{
	px_char *buffer;
	px_memorypool *mp;
	px_int bufferlen;
	px_int exreg_strlen;
}px_string;


#define PX_STRING_DATA(x) (x->buffer)

px_bool PX_StringInitialize(px_memorypool *mp,px_string *str);
px_void PX_StringInitFromConst(px_string *str,const px_char *constchar);
px_void PX_StringUpdateExReg(px_string* text);
px_int PX_StringToInteger(px_string* str);
px_float PX_StringToFloat(px_string *str);
px_void PX_StringTrim(px_string *str);
px_bool PX_StringCat(px_string *str,const px_char *str2);
px_void PX_StringClear(px_string *str);
px_bool PX_StringCatChar(px_string *str,px_char ch);
px_int  PX_StringLen(px_string *str);
px_void PX_StringFree(px_string *str);
px_bool PX_StringCopy(px_string *dest,px_string *res);
px_bool PX_StringInsertChar(px_string *str,px_int index,px_char ch);
px_bool PX_StringRemoveChar(px_string *str,px_int index);
px_void PX_StringReplaceRange(px_string *str,px_int startindex,px_int endindex,px_char *replaceto);
px_void PX_StringTrimLeft(px_string *str,px_int leftCount);
px_void PX_StringTrimRight(px_string *str,px_int RightCount);

px_bool PX_StringIsNumeric(px_string *str);
px_bool PX_StringIsFloat(px_string *str);

//px_void PX_StringFormat(px_string *str,px_char fmt[],...);
px_void PX_StringReplace(px_string *str,px_char *source,px_char *replaceto);
px_bool PX_StringInsert(px_string *str,px_int insertIndex,const px_char *InstrString);
px_bool PX_StringTrimer_Solve(px_string *pstring,px_char *parseCode,px_char *ReplaceCode);
px_void PX_StringInitAlloc(px_memorypool *mp,px_string *str,px_int allocSize);

px_bool PX_StringFormat8(px_string *str,const px_char fmt[],px_stringformat _1, px_stringformat _2, px_stringformat _3, px_stringformat _4,px_stringformat _5, px_stringformat _6, px_stringformat _7, px_stringformat _8);
px_bool PX_StringFormat7(px_string *str,const px_char fmt[],px_stringformat _1, px_stringformat _2, px_stringformat _3, px_stringformat _4,px_stringformat _5, px_stringformat _6, px_stringformat _7);
px_bool PX_StringFormat6(px_string *str,const px_char fmt[],px_stringformat _1, px_stringformat _2, px_stringformat _3, px_stringformat _4,px_stringformat _5, px_stringformat _6);
px_bool PX_StringFormat5(px_string *str,const px_char fmt[],px_stringformat _1, px_stringformat _2, px_stringformat _3, px_stringformat _4,px_stringformat _5);
px_bool PX_StringFormat4(px_string *str,const px_char fmt[],px_stringformat _1, px_stringformat _2, px_stringformat _3, px_stringformat _4);
px_bool PX_StringFormat3(px_string *str,const px_char fmt[],px_stringformat _1, px_stringformat _2, px_stringformat _3);
px_bool PX_StringFormat2(px_string *str,const px_char fmt[],px_stringformat _1, px_stringformat _2);
px_bool PX_StringFormat1(px_string *str,const px_char fmt[],px_stringformat _1);
px_bool PX_StringSet(px_string *str,const px_char fmt[]);

px_bool PX_StringCatEx(px_string *text,const px_char *str2);
px_void PX_StringBackspace(px_string *text);
px_void PX_StringBackspaceEx(px_string* text);
px_bool PX_StringCatCharEx(px_string* text, px_char ch);


#endif
