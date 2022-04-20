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
	px_list values; //values
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
		PX_Json_Object _object;
		px_list _array;//values
	};
}PX_Json_Value;

typedef struct _PX_Json_Object PX_Json_Object;

typedef struct  
{
	px_memorypool *mp;
	PX_Json_Value rootValue;
}PX_Json;


PX_Json_Value *PX_JsonGetObjectValue(PX_Json_Value *json_value,const px_char name[]);
PX_Json_Value *PX_JsonGetObjectValueByIndex(PX_Json_Value *json_value,px_int i);
PX_Json_Value* PX_JsonValueGetValue(PX_Json_Value* it, const px_char _payload[]);
PX_Json_Value* PX_JsonGetValue(PX_Json* json, const px_char payload[]);
PX_Json_Value * PX_JsonGetArrayValue(PX_Json_Value *value,px_int i);


px_bool PX_JsonInitialize(px_memorypool *mp,PX_Json *pjson);

px_bool PX_JsonParse(PX_Json *pjson,const px_char *json_content);
px_bool PX_JsonBuild_Value(PX_Json_Value *pValue,px_string *_out,px_bool bArrayValue);
px_bool PX_JsonBuild(PX_Json *pjson,px_string *_out);
px_void PX_JsonDelete(PX_Json *pjson,const px_char payload[]);
px_void PX_JsonFree(PX_Json *pjson);


px_bool PX_JsonCreateObjectValue(px_memorypool *mp,PX_Json_Value *pValue,const px_char name[]);
px_bool PX_JsonCreateStringValue(px_memorypool *mp,PX_Json_Value *pValue,const px_char name[],const px_char text[]);
px_bool PX_JsonCreateNumberValue(px_memorypool *mp,PX_Json_Value *pValue,const px_char name[],px_double value);
px_bool PX_JsonCreateBooleanValue(px_memorypool *mp,PX_Json_Value *pValue,const px_char name[],px_bool b);
px_bool PX_JsonCreateArrayValue(px_memorypool *mp,PX_Json_Value *pValue,const px_char name[]);

px_bool PX_JsonAddString(PX_Json *pjson,const px_char parent_payload[],const px_char name[],const px_char text[]);
px_bool PX_JsonAddNumber(PX_Json *pjson,const px_char parent_payload[],const px_char name[],const px_double number);
px_bool PX_JsonAddBoolean(PX_Json *pjson,const px_char parent_payload[],const px_char name[],const px_bool b);
px_bool PX_JsonAddArray(PX_Json *pjson,const px_char parent_payload[],const px_char name[]);
px_bool PX_JsonAddObject(PX_Json *pjson,const px_char parent_payload[],const px_char name[]);

const px_char *PX_JsonGetString(PX_Json *pjson,const px_char payload[]);
px_double PX_JsonGetNumber(PX_Json *pjson,const px_char payload[]);
px_bool  PX_JsonGetBoolean(PX_Json *pjson,const px_char payload[]);

const px_char* PX_JsonValueGetString(PX_Json_Value* pValue, const px_char payload[]);
px_double PX_JsonValueGetNumber(PX_Json_Value* pValue, const px_char payload[]);
px_bool  PX_JsonValueGetBoolean(PX_Json_Value* pValue, const px_char payload[]);

px_bool PX_JsonSetString(PX_Json *pjson,const px_char payload[],const px_char text[]);
px_bool PX_JsonSetNumber(PX_Json *pjson,const px_char payload[],const px_double number);
px_bool PX_JsonSetBoolean(PX_Json *pjson,const px_char payload[],const px_bool b);

px_bool PX_JsonObjectAddValue(PX_Json_Value *pObject,PX_Json_Value *value);
px_bool PX_JsonArrayAddValue(PX_Json_Value *pArray,PX_Json_Value *value);

#endif

