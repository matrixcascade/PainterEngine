#include "PX_Json.h"
static px_uint json_quotes;

px_bool PX_JsonInterpret_Object(PX_Json *pjson,px_lexer *lexer,PX_Json_Object *json_Object);
px_bool PX_JsonInterpret_Value(PX_Json *pjson,px_lexer *lexer,PX_Json_Value *_value,px_bool arrayElement);
px_void PX_JsonFreeValue(PX_Json *pjson,PX_Json_Value *json_Value);

PX_LEXER_LEXEME_TYPE PX_JsonNextToken(px_lexer *lexer)
{
	PX_LEXER_LEXEME_TYPE type;
	while ((type= PX_LexerGetNextLexeme(lexer))==PX_LEXER_LEXEME_TYPE_SPACER||type==PX_LEXER_LEXEME_TYPE_NEWLINE);

	return type;
}

PX_Json_Value * PX_JsonGetObjectValue(PX_Json_Value *json_value,const px_char name[])
{
	px_int i;
	if (json_value->type!=PX_JSON_VALUE_TYPE_OBJECT)
	{
		return PX_NULL;
	}
	for (i=0;i<json_value->json_Object.values.size;i++)
	{
		PX_Json_Value *pValue=PX_LISTAT(PX_Json_Value,&json_value->json_Object.values,i);
		if (PX_strequ(name,pValue->name.buffer))
		{
			return pValue;
		}
	}
	return PX_NULL;
}

PX_Json_Value * PX_JsonGetArrayValue(PX_Json_Value *value,px_int i)
{
	if (value->type!=PX_JSON_VALUE_TYPE_ARRAY)
	{
		return PX_NULL;
	}
	if (i<value->_Array.size)
	{
		return PX_LISTAT(PX_Json_Value,&value->_Array,i);
	}
	return PX_NULL;
}

px_bool PX_JsonInitialize(px_memorypool *mp,PX_Json *pjson)
{
	pjson->mp=mp;
	pjson->rootValue.type=PX_JSON_VALUE_TYPE_OBJECT;
	return PX_TRUE;	
}



static px_bool PX_JsonInterpret_Value_Array(PX_Json *pjson,px_lexer *lexer,PX_Json_Value *_value)
{
	PX_LEXER_LEXEME_TYPE type;
	PX_Json_Value array_value;
	PX_LEXER_STATE state;
	px_int i;
	type=PX_JsonNextToken(lexer);

	if (type!=PX_LEXER_LEXEME_TYPE_DELIMITER||lexer->Symbol!='[')
		goto _ERROR;

	while (PX_TRUE)
	{
		state=PX_LexerGetState(lexer);
		type=PX_JsonNextToken(lexer);
		if (type==PX_LEXER_LEXEME_TYPE_DELIMITER)
		{
			if(lexer->Symbol!=']')
				goto _ERROR;
			else
				return PX_TRUE;
		}
		else
		{
			PX_LexerSetState(state);
			if (!PX_JsonInterpret_Value(pjson,lexer,&array_value,PX_TRUE))
			{
				goto _ERROR;
			}

			if(!PX_ListPush(&_value->_Array,&array_value,sizeof(array_value)))
				goto _ERROR;
			
			type=PX_JsonNextToken(lexer);
			if (type==PX_LEXER_LEXEME_TYPE_DELIMITER)
			{
				if (lexer->Symbol==',')
				{
					continue;
				}

				if (lexer->Symbol==']')
				{
					return PX_TRUE;
				}
				goto _ERROR;
			}
			else
			{
				goto _ERROR;
			}
		}
	}
_ERROR:
	for (i=0;i<_value->_Array.size;i++)
	{
		PX_JsonFreeValue(pjson,PX_LISTAT(PX_Json_Value,&_value->_Array,i));
	}
	PX_ListFree(&_value->_Array);
	return PX_FALSE;
}






px_bool PX_JsonInterpret_Value(PX_Json *pjson,px_lexer *lexer,PX_Json_Value *_value,px_bool arrayElement)
{
	PX_LEXER_STATE state;
	PX_LEXER_LEXEME_TYPE type;


	PX_memset(_value,0,sizeof(PX_Json_Value));
	PX_StringInit(pjson->mp,&_value->name);
	if (!arrayElement)
	{
		type=PX_JsonNextToken(lexer);
		if (type!=PX_LEXER_LEXEME_TYPE_CONATINER)//{name
		{
			goto _ERROR;
		}
		PX_LexerGetIncludedString(lexer,&lexer->CurLexeme);
		PX_StringCopy(&_value->name,&lexer->CurLexeme);

		type=PX_JsonNextToken(lexer);
		if (type!=PX_LEXER_LEXEME_TYPE_DELIMITER||lexer->Symbol!=':')//{\spacer name :
		{
			goto _ERROR;
		}
	}

	state=PX_LexerGetState(lexer);
	type=PX_JsonNextToken(lexer);

	if (type==PX_LEXER_LEXEME_TYPE_DELIMITER)
	{
		if(lexer->Symbol=='[')
		{
			PX_LexerSetState(state);
			_value->type=PX_JSON_VALUE_TYPE_ARRAY;
			PX_ListInit(pjson->mp,&_value->_Array);
			if(!PX_JsonInterpret_Value_Array(pjson,lexer,_value))
			{
				
				goto _ERROR;
			}
			else
				return PX_TRUE;
		}

		if(lexer->Symbol=='{')
		{
			PX_LexerSetState(state);
			_value->type=PX_JSON_VALUE_TYPE_OBJECT;
			if(!PX_JsonInterpret_Object(pjson,lexer,&_value->json_Object))
			{
				return PX_FALSE;
			}
			return PX_TRUE;

		}

		goto _ERROR;
		
	}
	else
	{
		if (type==PX_LEXER_LEXEME_TYPE_TOKEN)
		{
			if(PX_StringIsNumeric(&lexer->CurLexeme))
			{
				_value->type=PX_JSON_VALUE_TYPE_NUMBER;
				_value->_number=PX_atof(lexer->CurLexeme.buffer);
				return PX_TRUE;
			}

			if(PX_strequ(lexer->CurLexeme.buffer,"true"))
			{
				_value->type=PX_JSON_VALUE_TYPE_BOOLEAN;
				_value->_boolean=PX_TRUE;
				return PX_TRUE;
			}

			if(PX_strequ(lexer->CurLexeme.buffer,"false"))
			{
				_value->type=PX_JSON_VALUE_TYPE_BOOLEAN;
				_value->_boolean=PX_FALSE;
				return PX_TRUE;
			}

			if(PX_strequ(lexer->CurLexeme.buffer,"null"))
			{
				_value->type=PX_JSON_VALUE_TYPE_NULL;
				return PX_TRUE;
			}
		}
		else if(type==PX_LEXER_LEXEME_TYPE_CONATINER&&lexer->CurrentContainerType==json_quotes)
		{
			_value->type=PX_JSON_VALUE_TYPE_STRING;
			if(!PX_StringInit(pjson->mp,&_value->_string))goto _ERROR;;
			PX_LexerGetIncludedString(lexer,&lexer->CurLexeme);
			if(!PX_StringCopy(&_value->_string,&lexer->CurLexeme))
			{
				PX_StringFree(&_value->_string);
				goto _ERROR;
			}
			return PX_TRUE;
		}
		
		goto _ERROR;

	}
_ERROR:
	PX_StringFree(&_value->name);
	return PX_FALSE;
}

px_bool PX_JsonInterpret_Object(PX_Json *pjson,px_lexer *lexer,PX_Json_Object *json_Object)
{
	PX_LEXER_LEXEME_TYPE type;
	PX_Json_Value _value;
	PX_LEXER_STATE state;
	px_int i;
	PX_memset(json_Object,0,sizeof(PX_Json_Object));

	PX_ListInit(pjson->mp,&json_Object->values);

	while (PX_TRUE)
	{
		type=PX_JsonNextToken(lexer);
		if (type==PX_LEXER_LEXEME_TYPE_END)
		{
			goto _ERROR;
		}

		if (type==PX_LEXER_LEXEME_TYPE_DELIMITER&&lexer->Symbol=='{')//{
		{
			state=PX_LexerGetState(lexer);
			type=PX_JsonNextToken(lexer);
			if (type==PX_LEXER_LEXEME_TYPE_DELIMITER&&lexer->Symbol=='}')//{}
			{
				return PX_TRUE;
			}
			else
			{
				PX_LexerSetState(state);
				while (PX_TRUE)
				{
					

					if (!PX_JsonInterpret_Value(pjson,lexer,&_value,PX_FALSE))
					{
						goto _ERROR;
					}

					if(!PX_ListPush(&json_Object->values,&_value,sizeof(_value)))
						goto _ERROR;

					type=PX_JsonNextToken(lexer);
					if (type!=PX_LEXER_LEXEME_TYPE_DELIMITER)
					{
						goto _ERROR;
					}

					if (lexer->Symbol=='}')
					{
						return PX_TRUE;
					}
					else if(lexer->Symbol==',')
					{
						continue;
					}
					else
					{
						PX_StringFree(&_value.name);
						goto _ERROR;
					}
				}
			}
		}
	}
_ERROR:
	//////////////////////////////////////////////////////////////////////////
	//remove object
	
	for (i=0;i<json_Object->values.size;i++)
	{
		PX_JsonFreeValue(pjson,PX_LISTAT(PX_Json_Value,&json_Object->values,i));
	}
	PX_ListFree(&json_Object->values);

	return PX_FALSE;
}

px_bool PX_JsonParse(PX_Json *pjson,const px_char *json_content)
{
	px_lexer lexer;
	
	PX_LexerInit(&lexer,pjson->mp);
	PX_LexerRegisterDelimiter(&lexer,',');
	PX_LexerRegisterDelimiter(&lexer,':');
	PX_LexerRegisterDelimiter(&lexer,'{');
	PX_LexerRegisterDelimiter(&lexer,'}');
	PX_LexerRegisterDelimiter(&lexer,'[');
	PX_LexerRegisterDelimiter(&lexer,']');
	PX_LexerRegisterSpacer(&lexer,' ');
	PX_LexerRegisterSpacer(&lexer,'\t');
	PX_LexerRegisterComment(&lexer,"//","\n");
	json_quotes=PX_LexerRegisterContainer(&lexer,"\"","\"");
	if(!PX_LexerLoadSourceFromMemory(&lexer,json_content)) goto _ERROR;

	if(!PX_JsonInterpret_Object(pjson,&lexer,&pjson->rootValue.json_Object))
		goto _ERROR;
	PX_LexerFree(&lexer);
	return PX_TRUE;
_ERROR:
	PX_LexerFree(&lexer);
	return PX_FALSE;
}

px_bool PX_JsonBuild_Value(PX_Json_Value *pValue,px_string *_out,px_bool bArrayValue)
{
	if (!bArrayValue)
	{
		//name
		if(!PX_StringCatChar(_out,'"'))return PX_FALSE;
		if(!PX_StringCat(_out,pValue->name.buffer)) return PX_FALSE;
		if(!PX_StringCatChar(_out,'"'))return PX_FALSE;
		//:
		if(!PX_StringCatChar(_out,':')) return PX_FALSE;
	}

	switch(pValue->type)
	{
	case PX_JSON_VALUE_TYPE_ARRAY:
		{
			px_int i;
			if(!PX_StringCatChar(_out,'['))return PX_FALSE;
			for (i=0;i<pValue->_Array.size;i++)
			{
				PX_Json_Value *pv=PX_LISTAT(PX_Json_Value,&pValue->_Array,i);
				if(!PX_JsonBuild_Value(pv,_out,PX_TRUE))return PX_FALSE;
				if(i!=pValue->_Array.size-1)if(!PX_StringCatChar(_out,','))return PX_FALSE;
			}
			if(!PX_StringCatChar(_out,']'))return PX_FALSE;
		}
		break;
	case PX_JSON_VALUE_TYPE_BOOLEAN:
		{
			if (pValue->_boolean)
			{
				if(!PX_StringCat(_out,"true"))return PX_FALSE;
			}
			else
			{
				if(!PX_StringCat(_out,"false"))return PX_FALSE;
			}
		}
		break;
	case PX_JSON_VALUE_TYPE_NULL:
		{
			if(!PX_StringCat(_out,"null"))return PX_FALSE;
		}
		break;
	case PX_JSON_VALUE_TYPE_NUMBER:
		{
			if(!PX_StringCat(_out,PX_ftos((px_float)pValue->_number,6).data))return PX_FALSE;
		}
		break;
	case PX_JSON_VALUE_TYPE_STRING:
		{
			if(!PX_StringCat(_out,"\""))return PX_FALSE;
			if(!PX_StringCat(_out,pValue->_string.buffer))return PX_FALSE;
			if(!PX_StringCat(_out,"\""))return PX_FALSE;
		}
		break;
	case PX_JSON_VALUE_TYPE_OBJECT:
		{
			px_int i;
			if(!PX_StringCat(_out,"{\n")) return PX_FALSE;
			for (i=0;i<pValue->json_Object.values.size;i++)
			{
				PX_Json_Value *ptmpValue=PX_LISTAT(PX_Json_Value,&pValue->json_Object.values,i);
				if(!PX_JsonBuild_Value(ptmpValue,_out,PX_FALSE))return PX_FALSE;
				if(i!=pValue->json_Object.values.size-1)if(!PX_StringCat(_out,",")) return PX_FALSE;
				if(!PX_StringCat(_out,"\n")) return PX_FALSE;
			}
			if(!PX_StringCat(_out,"}")) return PX_FALSE;
		}
		break;
	}
	return PX_TRUE;
}


px_bool PX_JsonBuild(PX_Json *pjson,px_string *_out)
{
	return PX_JsonBuild_Value(&pjson->rootValue,_out,PX_TRUE);
}

px_void PX_JsonFreeValue(PX_Json *pjson,PX_Json_Value *json_Value)
{
	PX_StringFree(&json_Value->name);
	switch (json_Value->type)
	{
	case PX_JSON_VALUE_TYPE_ARRAY:
		{
			px_int i;
			for (i=0;i<json_Value->_Array.size;i++)
			{
				PX_JsonFreeValue(pjson,PX_LISTAT(PX_Json_Value,&json_Value->_Array,i));
			}
			PX_ListFree(&json_Value->_Array);
		}
		break;
	case PX_JSON_VALUE_TYPE_STRING:
		PX_StringFree(&json_Value->_string);
		break;
	case PX_JSON_VALUE_TYPE_OBJECT:
		{
			px_int i;
			for (i=0;i<json_Value->json_Object.values.size;i++)
			{
				PX_JsonFreeValue(pjson,PX_LISTAT(PX_Json_Value,&json_Value->json_Object.values,i));
			}
			PX_ListFree(&json_Value->json_Object.values);
		}
		break;
	case PX_JSON_VALUE_TYPE_BOOLEAN:
	case PX_JSON_VALUE_TYPE_NULL:
	case PX_JSON_VALUE_TYPE_NUMBER:
	default:
		break;
	}
}

px_void PX_JsonFree(PX_Json *pjson)
{
	PX_JsonFreeValue(pjson,&pjson->rootValue);
}

px_bool PX_JsonCreateObjectValue(px_memorypool *mp,PX_Json_Value *pValue,const px_char name[])
{
	PX_memset(pValue,0,sizeof(PX_Json_Value));
	pValue->type=PX_JSON_VALUE_TYPE_OBJECT;

	if (!PX_StringInit(mp,&pValue->name))
	{
		return PX_FALSE;
	}
	if(!PX_StringSet(&pValue->name,name)) return PX_FALSE;
	PX_ListInit(mp,&pValue->json_Object.values);
	return PX_TRUE;
}

px_bool PX_JsonCreateStringValue(px_memorypool *mp,PX_Json_Value *pValue,const px_char name[],const px_char text[])
{
	PX_memset(pValue,0,sizeof(PX_Json_Value));
	pValue->type=PX_JSON_VALUE_TYPE_STRING;

	if (!PX_StringInit(mp,&pValue->name))
	{
		return PX_FALSE;
	}
	PX_StringSet(&pValue->name,name);

	if (!PX_StringInit(mp,&pValue->_string))
	{
		return PX_FALSE;
	}
	if(!PX_StringSet(&pValue->_string,text)) return PX_FALSE;
	return PX_TRUE;
}

px_bool PX_JsonCreateNumberValue(px_memorypool *mp,PX_Json_Value *pValue,const px_char name[],px_double value)
{
	PX_memset(pValue,0,sizeof(PX_Json_Value));
	pValue->type=PX_JSON_VALUE_TYPE_STRING;

	if (!PX_StringInit(mp,&pValue->name))
	{
		return PX_FALSE;
	}
	if(!PX_StringSet(&pValue->name,name)) return PX_FALSE;

	pValue->_number=value;
	return PX_TRUE;
}

px_bool PX_JsonCreateArrayValue(px_memorypool *mp,PX_Json_Value *pValue,const px_char name[])
{
	PX_memset(pValue,0,sizeof(PX_Json_Value));
	pValue->type=PX_JSON_VALUE_TYPE_STRING;

	if (!PX_StringInit(mp,&pValue->name))
	{
		return PX_FALSE;
	}
	if(!PX_StringSet(&pValue->name,name)) return PX_FALSE;
	PX_ListInit(mp,&pValue->_Array);
	return PX_TRUE;
}
