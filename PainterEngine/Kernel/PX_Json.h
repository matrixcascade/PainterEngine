#ifndef PX_JSON_H
#define PX_JSON_H
#include "PX_Lexer.h"



typedef enum
{
	PX_JSON_VALUE_TYPE_STRING,
	PX_JSON_VALUE_TYPE_NUMBER,
	PX_JSON_VALUE_TYPE_BOOLEAN,
	PX_JSON_VALUE_TYPE_NULL,
	PX_JSON_VALUE_TYPE_OBJECT,
	PX_JSON_VALUE_TYPE_ARRAY,
}PX_JSON_VALUE_TYPE;

typedef struct _PX_Json_Object
{
	px_list values;
}PX_Json_Object;

typedef struct
{
	PX_JSON_VALUE_TYPE type;
	px_string name;
	union
	{
		px_string _string;
		px_double _number;
		px_bool   _boolean;
		PX_Json_Object json_Object;
		px_list _Array;
	};
}PX_Json_Value;

typedef struct _PX_Json_Object PX_Json_Object;

typedef struct  
{
	px_memorypool *mp;
	PX_Json_Value rootValue;
}PX_Json;


PX_Json_Value *PX_JsonGetObjectValue(PX_Json_Value *json_value,const px_char name[]);
PX_Json_Value * PX_JsonGetArrayValue(PX_Json_Value *value,px_int i);


px_bool PX_JsonInitialize(px_memorypool *mp,PX_Json *pjson);

px_bool PX_JsonParse(PX_Json *pjson,const px_char *json_content);
px_bool PX_JsonBuild_Value(PX_Json_Value *pValue,px_string *_out,px_bool bArrayValue);
px_bool PX_JsonBuild(PX_Json *pjson,px_string *_out);
px_void PX_JsonFree(PX_Json *pjson);


px_bool PX_JsonCreateObjectValue(px_memorypool *mp,PX_Json_Value *pValue,const px_char name[]);
px_bool PX_JsonCreateStringValue(px_memorypool *mp,PX_Json_Value *pValue,const px_char name[],const px_char text[]);
px_bool PX_JsonCreateNumberValue(px_memorypool *mp,PX_Json_Value *pValue,const px_char name[],px_double value);
px_bool PX_JsonCreateArrayValue(px_memorypool *mp,PX_Json_Value *pValue,const px_char name[]);
#endif