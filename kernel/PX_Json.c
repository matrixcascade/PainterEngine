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
	for (i=0;i<json_value->_object.values.size;i++)
	{
		PX_Json_Value *pValue=PX_LISTAT(PX_Json_Value,&json_value->_object.values,i);
		if (PX_strequ(name,pValue->name.buffer))
		{
			return pValue;
		}
	}
	return PX_NULL;
}

PX_Json_Value * PX_JsonGetObjectValueByIndex(PX_Json_Value *json_value,px_int i)
{
	if (i<0||i>=json_value->_object.values.size)
	{
		return PX_NULL;
	}
	return PX_LISTAT(PX_Json_Value,&json_value->_object.values,i);
}

PX_Json_Value* PX_JsonValueGetValue(PX_Json_Value* it, const px_char _payload[])
{
	px_int r_offset = 0;
	px_int s_offset = 0;
	px_char payload[256] = { 0 };
	px_char* lexeme = PX_NULL;
	px_int i = 0, array = -1;

	if (PX_strlen(_payload) >= sizeof(payload))
	{
		return PX_NULL;
	}
	PX_strset(payload, _payload);

	while (payload[r_offset] != 0)
	{

		while (PX_TRUE)
		{
			if (payload[s_offset] == '.')
			{
				payload[s_offset] = '\0';
				s_offset++;
				break;
			}
			if (payload[s_offset] == '\0')
			{
				break;
			}
			s_offset++;
		}

		lexeme = payload + r_offset;
		i = PX_strlen(lexeme);
		//array?
		if (lexeme[i - 1] == ']')
		{

			lexeme[i - 1] = '\0';
			i--;
			while (i > 0)
			{
				if (lexeme[i] == '[')
				{
					lexeme[i] = '\0';
					array = PX_atoi(lexeme + i + 1);
					break;
				}
				i--;
			}
		}
		else
		{
			array = -1;
		}
		if (array != -1)
		{
			it = PX_JsonGetObjectValue(it, lexeme);
			if (it && it->type == PX_JSON_VALUE_TYPE_ARRAY)
			{
				it = PX_JsonGetArrayValue(it, array);
			}
			else
			{
				return PX_NULL;
			}

		}
		else
		{
			it = PX_JsonGetObjectValue(it, lexeme);
		}

		r_offset = s_offset;
		if (!it)
		{
			break;
		}

	}
	return it;
}
PX_Json_Value * PX_JsonGetValue(PX_Json *json,const px_char _payload[])
{
	return PX_JsonValueGetValue(&json->rootValue, _payload);
}

PX_Json_Value * PX_JsonGetArrayValue(PX_Json_Value *value,px_int i)
{
	if (value->type!=PX_JSON_VALUE_TYPE_ARRAY)
	{
		return PX_NULL;
	}
	if (i<value->_array.size)
	{
		return PX_LISTAT(PX_Json_Value,&value->_array,i);
	}
	return PX_NULL;
}

px_bool PX_JsonInitialize(px_memorypool *mp,PX_Json *pjson)
{
	PX_memset(pjson,0,sizeof(PX_Json));
	pjson->mp=mp;
	pjson->rootValue.type=PX_JSON_VALUE_TYPE_OBJECT;
	PX_StringInitialize(mp,&pjson->rootValue.name);
	PX_ListInitialize(pjson->mp,&pjson->rootValue._object.values);
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
			if(lexer->Symbol==']')
			{
				return PX_TRUE;
			}

			if(lexer->Symbol!='{')
			{
				goto _ERROR;
			}	
		}
		
		PX_LexerSetState(state);
		if (!PX_JsonInterpret_Value(pjson,lexer,&array_value,PX_TRUE))
		{
			goto _ERROR;
		}

		if(!PX_ListPush(&_value->_array,&array_value,sizeof(array_value)))
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
_ERROR:
	for (i=0;i<_value->_array.size;i++)
	{
		PX_JsonFreeValue(pjson,PX_LISTAT(PX_Json_Value,&_value->_array,i));
	}
	PX_ListFree(&_value->_array);
	return PX_FALSE;
}






px_bool PX_JsonInterpret_Value(PX_Json *pjson,px_lexer *lexer,PX_Json_Value *_value,px_bool arrayElement)
{
	PX_LEXER_STATE state;
	PX_LEXER_LEXEME_TYPE type;


	PX_memset(_value,0,sizeof(PX_Json_Value));
	PX_StringInitialize(pjson->mp,&_value->name);
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
			PX_ListInitialize(pjson->mp,&_value->_array);
			if(!PX_JsonInterpret_Value_Array(pjson,lexer,_value))
			{
				
				goto _ERROR;
			}
			else
				return PX_TRUE;
		}
		else
		if(lexer->Symbol=='{')
		{
			PX_LexerSetState(state);
			_value->type=PX_JSON_VALUE_TYPE_OBJECT;
			if(!PX_JsonInterpret_Object(pjson,lexer,&_value->_object))
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

			if (PX_strlen(lexer->CurLexeme.buffer)>=3)
			{
				if (lexer->CurLexeme.buffer[0]=='0'&&(lexer->CurLexeme.buffer[1]=='x'||lexer->CurLexeme.buffer[1]=='X'))
				{
					_value->type=PX_JSON_VALUE_TYPE_NUMBER;
					_value->_number=(px_dword)PX_htoi(lexer->CurLexeme.buffer+2);
					return PX_TRUE;
				}
			}
		}
		else if(type==PX_LEXER_LEXEME_TYPE_CONATINER&&lexer->CurrentContainerType==json_quotes)
		{
			px_int i;
			_value->type=PX_JSON_VALUE_TYPE_STRING;
			if(!PX_StringInitialize(pjson->mp,&_value->_string))goto _ERROR;;
			PX_LexerGetIncludedString(lexer,&lexer->CurLexeme);
			
			if(!PX_StringCopy(&_value->_string,&lexer->CurLexeme))
			{
				PX_StringFree(&_value->_string);
				goto _ERROR;
			}
			
			for (i=0;_value->_string.buffer[i];i++)
			{
				if (_value->_string.buffer[i]=='\\')
				{
					if (_value->_string.buffer[i+1]=='\\')
					{
						PX_StringRemoveChar(&_value->_string,i+1);
					}
					if (_value->_string.buffer[i+1]=='n')
					{
						_value->_string.buffer[i]='\n';
						PX_StringRemoveChar(&_value->_string,i+1);
					}
					if (_value->_string.buffer[i+1]=='r')
					{
						_value->_string.buffer[i]='\r';
						PX_StringRemoveChar(&_value->_string,i+1);
					}
					if (_value->_string.buffer[i+1]=='t')
					{
						_value->_string.buffer[i]='\t';
						PX_StringRemoveChar(&_value->_string,i+1);
					}
				}
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
	
	if (!json_Object->values.mp)
	{
		PX_ListInitialize(pjson->mp,&json_Object->values);
	}
	

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
	
	PX_LexerInitialize(&lexer,pjson->mp);
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

	if(!PX_JsonInterpret_Object(pjson,&lexer,&pjson->rootValue._object))
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
		if(!PX_StringCatCharEx(_out,'"'))return PX_FALSE;
		if(!PX_StringCatEx(_out,pValue->name.buffer)) return PX_FALSE;
		if(!PX_StringCatCharEx(_out,'"'))return PX_FALSE;
		//:
		if(!PX_StringCatCharEx(_out,':')) return PX_FALSE;
	}

	switch(pValue->type)
	{
	case PX_JSON_VALUE_TYPE_ARRAY:
		{
			px_int i;
			if(!PX_StringCatCharEx(_out,'['))return PX_FALSE;
			for (i=0;i<pValue->_array.size;i++)
			{
				PX_Json_Value *pv=PX_LISTAT(PX_Json_Value,&pValue->_array,i);
				if(!PX_JsonBuild_Value(pv,_out,PX_TRUE))return PX_FALSE;
				if(i!=pValue->_array.size-1)if(!PX_StringCatCharEx(_out,','))return PX_FALSE;
			}
			if(!PX_StringCatCharEx(_out,']'))return PX_FALSE;
		}
		break;
	case PX_JSON_VALUE_TYPE_BOOLEAN:
		{
			if (pValue->_boolean)
			{
				if(!PX_StringCatEx(_out,"true"))return PX_FALSE;
			}
			else
			{
				if(!PX_StringCatEx(_out,"false"))return PX_FALSE;
			}
		}
		break;
	case PX_JSON_VALUE_TYPE_NULL:
		{
			if(!PX_StringCatEx(_out,"null"))return PX_FALSE;
		}
		break;
	case PX_JSON_VALUE_TYPE_NUMBER:
		{
			if(!PX_StringCatEx(_out,PX_ftos((px_float)pValue->_number,6).data))return PX_FALSE;
		}
		break;
	case PX_JSON_VALUE_TYPE_STRING:
		{
			if(!PX_StringCatEx(_out,"\""))return PX_FALSE;
			if(!PX_StringCatEx(_out,pValue->_string.buffer))return PX_FALSE;
			if(!PX_StringCatEx(_out,"\""))return PX_FALSE;
		}
		break;
	case PX_JSON_VALUE_TYPE_OBJECT:
		{
			px_int i;
			if(!PX_StringCatEx(_out,"{\n")) return PX_FALSE;
			for (i=0;i<pValue->_object.values.size;i++)
			{
				PX_Json_Value *ptmpValue=PX_LISTAT(PX_Json_Value,&pValue->_object.values,i);
				if(!PX_JsonBuild_Value(ptmpValue,_out,PX_FALSE))return PX_FALSE;
				if(i!=pValue->_object.values.size-1)if(!PX_StringCatEx(_out,",")) return PX_FALSE;
				if(!PX_StringCatEx(_out,"\n")) return PX_FALSE;
			}
			if(!PX_StringCatEx(_out,"}")) return PX_FALSE;
		}
		break;
	}
	return PX_TRUE;
}


px_bool PX_JsonBuild(PX_Json *pjson,px_string *_out)
{
	PX_StringUpdateExReg(_out);
	return PX_JsonBuild_Value(&pjson->rootValue,_out,PX_TRUE);
}

static px_void PX_JsonDeleteValue(PX_Json *pjson,PX_Json_Value *pValue)
{
	switch (pValue->type)
	{
	case PX_JSON_VALUE_TYPE_ARRAY:
		{
			px_int i;
			for (i=0;i<pValue->_array.size;i++)
			{
				PX_Json_Value *pSubValue=PX_LISTAT(PX_Json_Value,&pValue->_array,i);
				PX_JsonDeleteValue(pjson,pSubValue);
			}
			PX_ListFree(&pValue->_array);
		}
		break;
	case PX_JSON_VALUE_TYPE_OBJECT:
		{
			px_int i;
			for (i=0;i<pValue->_object.values.size;i++)
			{
				PX_Json_Value *pSubValue=PX_LISTAT(PX_Json_Value,&pValue->_object.values,i);
				PX_JsonDeleteValue(pjson,pSubValue);
			}
			PX_ListFree(&pValue->_object.values);
		}
		break;
	case PX_JSON_VALUE_TYPE_STRING:
		{
			PX_StringFree(&pValue->_string);
		}
		break;
	default:
		break;
	}
}


px_void PX_JsonDelete(PX_Json *pjson,const px_char _payload[])
{
	px_int i;
	PX_Json_Value *pParentValue,*pValue;
	px_char payload[256]={0};


	if (PX_strlen(_payload)>=sizeof(payload))
	{
		return;
	}

	pValue=PX_JsonGetValue(pjson,_payload);
	if (!pValue)
	{
		return;
	}

	PX_strset(payload,_payload);

	if (payload[PX_strlen(payload)-1]==']')
	{
		//array
		for (i=PX_strlen(payload);i>0;i--)
		{
			if (payload[i]=='[')
			{
				break;
			}
		}
	}
	else
	{
		for (i=PX_strlen(payload);i>0;i--)
		{
			if (payload[i]=='.')
			{
				break;
			}
		}
	}
	payload[i]='\0';
	pParentValue=PX_JsonGetValue(pjson,payload);

	if (!pParentValue)
	{
		return;
	}
	switch(pParentValue->type)
	{
	case PX_JSON_VALUE_TYPE_ARRAY:
		{
			px_int j;
			for (j=0;j<pParentValue->_array.size;j++)
			{
				PX_Json_Value *pTarget;
				pTarget=PX_LISTAT(PX_Json_Value,&pParentValue->_array,j);
				if (pTarget==pValue)
				{
					PX_JsonDeleteValue(pjson,pTarget);
					PX_ListErase(&pParentValue->_array,j);
					return;
				}
			}
		}
		break;
	case PX_JSON_VALUE_TYPE_OBJECT:
		{
			px_int j;
			for (j=0;j<pParentValue->_object.values.size;j++)
			{
				PX_Json_Value *pTarget;
				pTarget=PX_LISTAT(PX_Json_Value,&pParentValue->_object.values,j);
				if (pTarget==pValue)
				{
					PX_JsonDeleteValue(pjson,pTarget);
					PX_ListErase(&pParentValue->_object.values,j);
					return;
				}
			}
		}
		break;
	default:
		break;
	}


}

px_void PX_JsonFreeValue(PX_Json *pjson,PX_Json_Value *json_Value)
{
	PX_StringFree(&json_Value->name);
	switch (json_Value->type)
	{
	case PX_JSON_VALUE_TYPE_ARRAY:
		{
			px_int i;
			for (i=0;i<json_Value->_array.size;i++)
			{
				PX_JsonFreeValue(pjson,PX_LISTAT(PX_Json_Value,&json_Value->_array,i));
			}
			PX_ListFree(&json_Value->_array);
		}
		break;
	case PX_JSON_VALUE_TYPE_STRING:
		PX_StringFree(&json_Value->_string);
		break;
	case PX_JSON_VALUE_TYPE_OBJECT:
		{
			px_int i;
			for (i=0;i<json_Value->_object.values.size;i++)
			{
				PX_JsonFreeValue(pjson,PX_LISTAT(PX_Json_Value,&json_Value->_object.values,i));
			}
			PX_ListFree(&json_Value->_object.values);
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

	if (!PX_StringInitialize(mp,&pValue->name))
	{
		return PX_FALSE;
	}
	if(!PX_StringSet(&pValue->name,name)) return PX_FALSE;
	PX_ListInitialize(mp,&pValue->_object.values);
	return PX_TRUE;
}

px_bool PX_JsonCreateStringValue(px_memorypool *mp,PX_Json_Value *pValue,const px_char name[],const px_char text[])
{
	PX_memset(pValue,0,sizeof(PX_Json_Value));
	pValue->type=PX_JSON_VALUE_TYPE_STRING;

	if (!PX_StringInitialize(mp,&pValue->name))
	{
		return PX_FALSE;
	}
	PX_StringSet(&pValue->name,name);

	if (!PX_StringInitialize(mp,&pValue->_string))
	{
		return PX_FALSE;
	}
	if(!PX_StringSet(&pValue->_string,text)) return PX_FALSE;
	return PX_TRUE;
}

px_bool PX_JsonCreateNumberValue(px_memorypool *mp,PX_Json_Value *pValue,const px_char name[],px_double value)
{
	PX_memset(pValue,0,sizeof(PX_Json_Value));
	pValue->type=PX_JSON_VALUE_TYPE_NUMBER;

	if (!PX_StringInitialize(mp,&pValue->name))
	{
		return PX_FALSE;
	}
	if(!PX_StringSet(&pValue->name,name)) return PX_FALSE;

	pValue->_number=value;
	return PX_TRUE;
}

px_bool PX_JsonCreateBooleanValue(px_memorypool *mp,PX_Json_Value *pValue,const px_char name[],px_bool b)
{
	PX_memset(pValue,0,sizeof(PX_Json_Value));
	pValue->type=PX_JSON_VALUE_TYPE_BOOLEAN;

	if (!PX_StringInitialize(mp,&pValue->name))
	{
		return PX_FALSE;
	}
	if(!PX_StringSet(&pValue->name,name)) return PX_FALSE;

	pValue->_boolean=b;
	return PX_TRUE;
}

px_bool PX_JsonCreateArrayValue(px_memorypool *mp,PX_Json_Value *pValue,const px_char name[])
{
	PX_memset(pValue,0,sizeof(PX_Json_Value));
	pValue->type=PX_JSON_VALUE_TYPE_ARRAY;

	if (!PX_StringInitialize(mp,&pValue->name))
	{
		return PX_FALSE;
	}
	if(!PX_StringSet(&pValue->name,name)) return PX_FALSE;
	PX_ListInitialize(mp,&pValue->_array);
	return PX_TRUE;
}

px_bool PX_JsonAddString(PX_Json *pjson,const px_char parent_payload[],const px_char name[],const px_char text[])
{
	PX_Json_Value *pValue=PX_JsonGetValue(pjson,parent_payload);
	
	PX_Json_Value newValue;
	if (!pValue)
	{
		return PX_FALSE;
	}
	switch (pValue->type)
	{
	case PX_JSON_VALUE_TYPE_ARRAY:
		{
			PX_Json_Value *pExistValue=PX_JsonGetObjectValue(pValue,name);
			if (!pExistValue)
			{
				PX_JsonCreateStringValue(pjson->mp,&newValue,name,text);
				PX_ListPush(&pValue->_array,&newValue,sizeof(newValue));
			}
			else
			{
				if (pExistValue->type!=PX_JSON_VALUE_TYPE_STRING)
				{
					return PX_FALSE;
				}
				PX_StringSet(&pExistValue->_string,text);
			}
			
			
			return PX_TRUE;
		}
		//break;
	case PX_JSON_VALUE_TYPE_OBJECT:
		{
			PX_Json_Value *pExistValue=PX_JsonGetObjectValue(pValue,name);
			if (!pExistValue)
			{
				PX_JsonCreateStringValue(pjson->mp,&newValue,name,text);
				PX_ListPush(&pValue->_object.values,&newValue,sizeof(newValue));
			}
			else
			{
				if (pExistValue->type!=PX_JSON_VALUE_TYPE_STRING)
				{
					return PX_FALSE;
				}
				PX_StringSet(&pExistValue->_string,text);
			}
			return PX_TRUE;
		}
		//break;
	default:
		return PX_FALSE;
	}

	return PX_FALSE;
}

px_bool PX_JsonAddNumber(PX_Json *pjson,const px_char parent_payload[],const px_char name[],const px_double number)
{
	PX_Json_Value *pValue=PX_JsonGetValue(pjson,parent_payload);
	PX_Json_Value newValue;
	if (!pValue)
	{
		return PX_FALSE;
	}
	switch (pValue->type)
	{
	case PX_JSON_VALUE_TYPE_ARRAY:
		{

			PX_Json_Value *pExistValue=PX_JsonGetObjectValue(pValue,name);
			if (!pExistValue)
			{
				PX_JsonCreateNumberValue(pjson->mp,&newValue,name,number);
				PX_ListPush(&pValue->_array,&newValue,sizeof(newValue));
			}
			else
			{
				if (pExistValue->type!=PX_JSON_VALUE_TYPE_NUMBER)
				{
					return PX_FALSE;
				}
				pExistValue->_number=number;
			}

			
			return PX_TRUE;
		}
		//break;
	case PX_JSON_VALUE_TYPE_OBJECT:
		{
			PX_Json_Value *pExistValue=PX_JsonGetObjectValue(pValue,name);
			if (!pExistValue)
			{
				PX_JsonCreateNumberValue(pjson->mp,&newValue,name,number);
				PX_ListPush(&pValue->_object.values,&newValue,sizeof(newValue));
			}
			else
			{
				if (pExistValue->type!=PX_JSON_VALUE_TYPE_NUMBER)
				{
					return PX_FALSE;
				}
				pExistValue->_number=number;
			}
			return PX_TRUE;
		}
		//break;
	default:
		return PX_FALSE;
	}

	return PX_FALSE;


}

px_bool PX_JsonAddBoolean(PX_Json *pjson,const px_char parent_payload[],const px_char name[],const px_bool b)
{
	PX_Json_Value *pValue=PX_JsonGetValue(pjson,parent_payload);
	PX_Json_Value newValue;
	if (!pValue)
	{
		return PX_FALSE;
	}
	switch (pValue->type)
	{
	case PX_JSON_VALUE_TYPE_ARRAY:
		{
			PX_Json_Value *pExistValue=PX_JsonGetObjectValue(pValue,name);
			if (!pExistValue)
			{
				PX_JsonCreateBooleanValue(pjson->mp,&newValue,name,b);
				PX_ListPush(&pValue->_array,&newValue,sizeof(newValue));
			}
			else
			{
				if (pExistValue->type!=PX_JSON_VALUE_TYPE_BOOLEAN)
				{
					return PX_FALSE;
				}
				pExistValue->_boolean=b;
			}

			return PX_TRUE;
		}
		//break;
	case PX_JSON_VALUE_TYPE_OBJECT:
		{
			PX_Json_Value *pExistValue=PX_JsonGetObjectValue(pValue,name);
			if (!pExistValue)
			{
				PX_JsonCreateBooleanValue(pjson->mp,&newValue,name,b);
				PX_ListPush(&pValue->_object.values,&newValue,sizeof(newValue));
			}
			else
			{
				if (pExistValue->type!=PX_JSON_VALUE_TYPE_BOOLEAN)
				{
					return PX_FALSE;
				}
				pExistValue->_boolean=b;
			}

			return PX_TRUE;
		}
		//break;
	default:
		return PX_FALSE;
	}

	return PX_FALSE;
}

px_bool PX_JsonAddArray(PX_Json *pjson,const px_char parent_payload[],const px_char name[])
{
	PX_Json_Value *pValue=PX_JsonGetValue(pjson,parent_payload);
	PX_Json_Value newValue;
	if (!pValue)
	{
		return PX_FALSE;
	}
	switch (pValue->type)
	{
	case PX_JSON_VALUE_TYPE_ARRAY:
		{
			PX_Json_Value *pExistValue=PX_JsonGetObjectValue(pValue,name);
			if (!pExistValue)
			{
				PX_JsonCreateArrayValue(pjson->mp,&newValue,name);
				PX_ListPush(&pValue->_array,&newValue,sizeof(newValue));
			}
			else
			{
				if (pExistValue->type!=PX_JSON_VALUE_TYPE_ARRAY)
				{
					return PX_FALSE;
				}
			}

			return PX_TRUE;
		}
		//break;
	case PX_JSON_VALUE_TYPE_OBJECT:
		{
			PX_Json_Value *pExistValue=PX_JsonGetObjectValue(pValue,name);
			if (!pExistValue)
			{
				PX_JsonCreateArrayValue(pjson->mp,&newValue,name);
				PX_ListPush(&pValue->_object.values,&newValue,sizeof(newValue));
			}
			else
			{
				if (pExistValue->type!=PX_JSON_VALUE_TYPE_ARRAY)
				{
					return PX_FALSE;
				}
			}
			
			return PX_TRUE;
		}
		//break;
	default:
		return PX_FALSE;
	}

	return PX_FALSE;
}

px_bool PX_JsonAddObject(PX_Json *pjson,const px_char parent_payload[],const px_char name[])
{
	PX_Json_Value *pValue=PX_JsonGetValue(pjson,parent_payload);
	PX_Json_Value newValue;
	if (!pValue)
	{
		return PX_FALSE;
	}
	switch (pValue->type)
	{
	case PX_JSON_VALUE_TYPE_ARRAY:
		{

			PX_Json_Value *pExistValue=PX_JsonGetObjectValue(pValue,name);
			if (!pExistValue)
			{
				PX_JsonCreateObjectValue(pjson->mp,&newValue,name);
				PX_ListPush(&pValue->_array,&newValue,sizeof(newValue));
			}
			else
			{
				if (pExistValue->type!=PX_JSON_VALUE_TYPE_OBJECT)
				{
					return PX_FALSE;
				}
			}

			
			return PX_TRUE;
		}
		//break;
	case PX_JSON_VALUE_TYPE_OBJECT:
		{

			PX_Json_Value *pExistValue=PX_JsonGetObjectValue(pValue,name);
			if (!pExistValue)
			{
				PX_JsonCreateObjectValue(pjson->mp,&newValue,name);
				PX_ListPush(&pValue->_object.values,&newValue,sizeof(newValue));
			}
			else
			{
				if (pExistValue->type!=PX_JSON_VALUE_TYPE_OBJECT)
				{
					return PX_FALSE;
				}
			}
			return PX_TRUE;
		}
		//break;
	default:
		return PX_FALSE;
	}

	return PX_FALSE;
}

const px_char * PX_JsonGetString(PX_Json *pjson,const px_char payload[])
{
	PX_Json_Value *pValue=PX_JsonGetValue(pjson,payload);
	if (pValue&&pValue->type==PX_JSON_VALUE_TYPE_STRING)
	{
		return pValue->_string.buffer;
	}
	return "";
}

px_double PX_JsonGetNumber(PX_Json *pjson,const px_char payload[])
{
	PX_Json_Value *pValue=PX_JsonGetValue(pjson,payload);
	if (pValue&&pValue->type==PX_JSON_VALUE_TYPE_NUMBER)
	{
		return pValue->_number;
	}
	return 0;
}

px_bool PX_JsonGetBoolean(PX_Json *pjson,const px_char payload[])
{
	PX_Json_Value *pValue=PX_JsonGetValue(pjson,payload);
	if (pValue&&pValue->type==PX_JSON_VALUE_TYPE_BOOLEAN)
	{
		return pValue->_boolean;
	}
	return 0;
}

const px_char* PX_JsonValueGetString(PX_Json_Value* _pValue, const px_char payload[])
{
	PX_Json_Value* pValue = PX_JsonValueGetValue(_pValue, payload);
	if (pValue && pValue->type == PX_JSON_VALUE_TYPE_STRING)
	{
		return pValue->_string.buffer;
	}
	return "";
}

px_double PX_JsonValueGetNumber(PX_Json_Value* _pValue, const px_char payload[])
{
	PX_Json_Value* pValue = PX_JsonValueGetValue(_pValue, payload);
	if (pValue && pValue->type == PX_JSON_VALUE_TYPE_NUMBER)
	{
		return pValue->_number;
	}
	return 0;
}

px_bool PX_JsonValueGetBoolean(PX_Json_Value* _pValue, const px_char payload[])
{
	PX_Json_Value* pValue = PX_JsonValueGetValue(_pValue, payload);
	if (pValue && pValue->type == PX_JSON_VALUE_TYPE_BOOLEAN)
	{
		return pValue->_boolean;
	}
	return 0;
}

px_bool PX_JsonSetString(PX_Json *pjson,const px_char payload[],const px_char text[])
{
	PX_Json_Value *pValue=PX_JsonGetValue(pjson,payload);
	if (pValue&&pValue->type==PX_JSON_VALUE_TYPE_STRING)
	{
		return PX_StringSet(&pValue->_string,text);
	}
	return PX_FALSE;
}

px_bool PX_JsonSetNumber(PX_Json *pjson,const px_char payload[],const px_double number)
{
	PX_Json_Value *pValue=PX_JsonGetValue(pjson,payload);
	if (pValue&&pValue->type==PX_JSON_VALUE_TYPE_NUMBER)
	{
		pValue->_number=number;
		return PX_TRUE;
	}
	return PX_FALSE;
}

px_bool PX_JsonSetBoolean(PX_Json *pjson,const px_char payload[],const px_bool b)
{
	PX_Json_Value *pValue=PX_JsonGetValue(pjson,payload);
	if (pValue&&pValue->type==PX_JSON_VALUE_TYPE_BOOLEAN)
	{
		pValue->_boolean=b;
		return PX_TRUE;
	}
	return PX_FALSE;
}

px_bool PX_JsonObjectAddValue(PX_Json_Value *pObject,PX_Json_Value *value)
{
	if (pObject->type==PX_JSON_VALUE_TYPE_OBJECT)
	{
		return PX_ListPush(&pObject->_object.values,value,sizeof(PX_Json_Value))!=PX_NULL;
	}
	return PX_FALSE;
}

px_bool PX_JsonArrayAddValue(PX_Json_Value *pArray,PX_Json_Value *value)
{
	if (pArray->type==PX_JSON_VALUE_TYPE_ARRAY)
	{
		return PX_ListPush(&pArray->_array,value,sizeof(PX_Json_Value))!=PX_NULL;
	}
	return PX_FALSE;
}

