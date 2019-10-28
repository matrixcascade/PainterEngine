#include "PX_String.h"

px_int PX_StringToInteger(px_string *str)
{
	if (str->buffer[0]=='0'&&(str->buffer[1]=='x'||str->buffer[1]=='X'))
	{
		return PX_htoi(str->buffer+2);
	}
	else
		return PX_atoi(str->buffer);
}

px_void PX_StringTrim(px_string *str)
{
	if (str->buffer)
	{
		px_int loft=0;
		px_int i;
		while (str->buffer[loft]==' ')
		{
			loft++;
		}
		i=0;
		while(PX_TRUE)
		{
			if (str->buffer[loft]=='\0'||loft>=str->bufferlen)
			{
				break;
			}
			str->buffer[i++]=str->buffer[loft++];
		}
		str->buffer[i]='\0';
		i=PX_strlen(str->buffer);
		if (i)
		{
			i--;
		}
		else
			return;

		while (i&&str->buffer[i]==' ')
		{ 
			str->buffer[i--]='\0';
		}
	}
}

px_void PX_StringTrimLeft(px_string *str,px_int leftCount)
{
	px_int i;
	if (leftCount==0)
	{
		return;
	}
	if (leftCount>=PX_strlen(str->buffer))
	{
		PX_StringClear(str);
	}
	else
	{
		for (i=0;str->buffer[i+leftCount];i++)
		{
			str->buffer[i]=str->buffer[i+leftCount];
		}
		str->buffer[i]='\0';
	}
}

px_void PX_StringTrimRight(px_string *str,px_int RightCount)
{
	if (RightCount==0)
	{
		return;
	}
	if (RightCount>=PX_strlen(str->buffer))
	{
		PX_StringClear(str);
	}
	else
	{
		str->buffer[PX_strlen(str->buffer)-RightCount]='\0';
	}
}

px_void PX_StringFree(px_string *str)
{
	if(str->buffer&&str->bufferlen)
	MP_Free(str->mp,str->buffer);

	str->buffer=PX_NULL;
	str->bufferlen=0;
}

px_bool PX_StringInit(px_memorypool *mp,px_string *str)
{
	str->buffer=(px_char *)MP_Malloc(mp,8);
	if(!str->buffer)return PX_FALSE;
	str->bufferlen=8;
	PX_memset(str->buffer,0,8);
	str->mp=mp;
	return PX_TRUE;
}

px_void PX_StringInitAlloc(px_memorypool *mp,px_string *str,px_int allocSize)
{
	int size=8;
	while(size<allocSize)
	{
		size<<=1;
	}
	str->buffer=(px_char *)MP_Malloc(mp,allocSize);
	str->bufferlen=allocSize;
	PX_memset(str->buffer,0,size);
	str->mp=mp;
}

//%s %d %f
// px_void PX_StringFormat(px_string *str,px_char fmt[],...)
// {
// 	_px_va_list ap;
// 	px_char *p, *sval,*oldptr;
// 	px_int ival,oft=0;
// 	px_uint finalLen=0;
// 	px_double dval;
// 	px_char dat[32];
// 	px_uchar shl=0;
// 
// 	_px_va_start(ap, fmt);
// 	for (p = fmt; *p; p++) {
// 		if(*p != '%') {
// 			finalLen++;
// 			continue;
// 		}
// 		switch(*++p) {
// 		case 'd':
// 			ival = _px_va_arg(ap, px_int);
// 			finalLen +=PX_itoa(ival,dat,sizeof dat,10);
// 			break;
// 		case 'f':
// 			dval = _px_va_arg(ap, px_double);
// 			finalLen +=PX_ftoa((px_float)dval,dat,sizeof dat,6);
// 			break;
// 		case 's':
// 			sval = _px_va_arg(ap, char *); 
// 			finalLen +=px_strlen(sval);
// 			break;
// 		default:
// 			finalLen+=1;
// 			break;
// 		}
// 	}
// 	_px_va_end(ap);
// 	finalLen++;
// 
// 	
// 	oldptr=str->buffer;
// 	while ((px_uint)(1<<++shl)<=finalLen);
// 	str->bufferlen=(1<<shl);
// 	str->buffer=(px_char *)MP_Malloc(str->mp,str->bufferlen);
// 	px_memset(str->buffer,0,str->bufferlen);
// 	_px_va_start(ap, fmt);
// 	for (p = fmt; *p; p++) {
// 		if(*p != '%') {
// 			str->buffer[oft++]=*p;
// 			continue;
// 		}
// 		switch(*++p) {
// 		case 'd':
// 			ival = _px_va_arg(ap, px_int);
// 			oft+=PX_itoa(ival,dat,sizeof dat,10);
// 		    px_strcat(str->buffer,dat);
// 			break;
// 		case 'f':
// 			dval = _px_va_arg(ap, px_double);
// 			oft+=PX_ftoa((px_float)dval,dat,sizeof dat,6);
// 			px_strcat(str->buffer,dat);
// 			break;
// 		case 's':
// 			sval = _px_va_arg(ap, char *); 
// 			oft+=px_strlen(sval);
// 			px_strcat(str->buffer,sval);
// 			break;
// 		default:
// 			str->buffer[oft++]=*p;
// 			break;
// 		}
// 	}
// 	_px_va_end(ap);
// 	str->buffer[oft]='\0';
// 
// 	if(oldptr)
// 	MP_Free(str->mp,oldptr);
// }


px_bool PX_StringFormat8(px_string *str,const px_char fmt[],px_stringformat _1, px_stringformat _2, px_stringformat _3, px_stringformat _4,px_stringformat _5, px_stringformat _6, px_stringformat _7, px_stringformat _8)
{
	px_char *oldptr;
	px_uint finalLen=0;
	px_uchar shl=0;
	
	finalLen=PX_sprintf8(PX_NULL,0,fmt,_1,_2,_3,_4,_5,_6,_7,_8);

	oldptr=str->buffer;

	while ((px_uint)(1<<++shl)<=finalLen);
	str->bufferlen=(1<<shl);
	str->buffer=(px_char *)MP_Malloc(str->mp,str->bufferlen);
	if (!str->buffer)
	{
		return PX_FALSE;
	}
	
	finalLen=PX_sprintf8(str->buffer,str->bufferlen,fmt,_1,_2,_3,_4,_5,_6,_7,_8);

	if(oldptr)
		MP_Free(str->mp,oldptr);

	return PX_TRUE;
}
px_bool PX_StringFormat7(px_string *str,const px_char fmt[],px_stringformat _1, px_stringformat _2, px_stringformat _3, px_stringformat _4,px_stringformat _5, px_stringformat _6, px_stringformat _7)
{
	return PX_StringFormat8(str,fmt,_1,_2,_3,_4,_5,_6,_7,PX_STRINGFORMAT_INT(0));
}

px_bool PX_StringFormat6(px_string *str,const px_char fmt[],px_stringformat _1, px_stringformat _2, px_stringformat _3, px_stringformat _4,px_stringformat _5, px_stringformat _6)
{
	return PX_StringFormat8(str,fmt,_1,_2,_3,_4,_5,_6,PX_STRINGFORMAT_INT(0),PX_STRINGFORMAT_INT(0));
}

px_bool PX_StringFormat5(px_string *str,const px_char fmt[],px_stringformat _1, px_stringformat _2, px_stringformat _3, px_stringformat _4,px_stringformat _5)
{
	return PX_StringFormat8(str,fmt,_1,_2,_3,_4,_5,PX_STRINGFORMAT_INT(0),PX_STRINGFORMAT_INT(0),PX_STRINGFORMAT_INT(0));
}

px_bool PX_StringFormat4(px_string *str,const px_char fmt[],px_stringformat _1, px_stringformat _2, px_stringformat _3, px_stringformat _4)
{
	return PX_StringFormat8(str,fmt,_1,_2,_3,_4,PX_STRINGFORMAT_INT(0),PX_STRINGFORMAT_INT(0),PX_STRINGFORMAT_INT(0),PX_STRINGFORMAT_INT(0));
}

px_bool PX_StringFormat3(px_string *str,const px_char fmt[],px_stringformat _1, px_stringformat _2, px_stringformat _3)
{
	return PX_StringFormat8(str,fmt,_1,_2,_3,PX_STRINGFORMAT_INT(0),PX_STRINGFORMAT_INT(0),PX_STRINGFORMAT_INT(0),PX_STRINGFORMAT_INT(0),PX_STRINGFORMAT_INT(0));
}

px_bool PX_StringFormat2(px_string *str,const px_char fmt[],px_stringformat _1, px_stringformat _2)
{
	return PX_StringFormat8(str,fmt,_1,_2,PX_STRINGFORMAT_INT(0),PX_STRINGFORMAT_INT(0),PX_STRINGFORMAT_INT(0),PX_STRINGFORMAT_INT(0),PX_STRINGFORMAT_INT(0),PX_STRINGFORMAT_INT(0));
}

px_bool PX_StringFormat1(px_string *str,const px_char fmt[],px_stringformat _1)
{
	return PX_StringFormat8(str,fmt,_1,PX_STRINGFORMAT_INT(0),PX_STRINGFORMAT_INT(0),PX_STRINGFORMAT_INT(0),PX_STRINGFORMAT_INT(0),PX_STRINGFORMAT_INT(0),PX_STRINGFORMAT_INT(0),PX_STRINGFORMAT_INT(0));
}

px_bool PX_StringSet(px_string *str,const px_char fmt[])
{
	px_char *oldptr;
	px_uint finalLen=0;
	px_uchar shl=0;

	finalLen=PX_strlen(fmt);

	if (finalLen<(px_uint)str->bufferlen)
	{
		PX_strset(str->buffer,fmt);
		return PX_TRUE;
	}

	oldptr=str->buffer;

	while ((px_uint)(1<<++shl)<=finalLen);
	str->bufferlen=(1<<shl);
	str->buffer=(px_char *)MP_Malloc(str->mp,str->bufferlen);
	if (!str->buffer)
	{
		return PX_FALSE;
	}
	PX_strset(str->buffer,fmt);

	if(oldptr)
		MP_Free(str->mp,oldptr);
	return PX_TRUE;
}

px_float PX_StringToFloat(px_string *str)
{
	return PX_atof(PX_STRING_DATA(str));
}

px_bool PX_StringIsNumeric(px_string *str)
{
	return PX_strIsNumeric(str->buffer);
}

px_bool PX_StringIsFloat(px_string *str)
{
	return PX_strIsFloat(str->buffer);
}

px_bool PX_StringCat(px_string *str,const px_char *str2)
{
	px_uchar shl=0;
	px_char *old=str->buffer;
	px_int length=PX_strlen(str->buffer)+PX_strlen(str2);
	if (length<str->bufferlen)
	{
		PX_strcat(str->buffer,str2);
		return PX_TRUE;
	}

	while ((px_int)(1<<++shl)<=length);
	str->bufferlen=(1<<shl);
	str->buffer=(px_char *)MP_Malloc(str->mp,str->bufferlen);
	if(!str->buffer)return PX_FALSE;
	str->buffer[0]='\0';
	PX_strcat(str->buffer,old);
	PX_strcat(str->buffer,str2);
	if(old)
	MP_Free(str->mp,old);
	return PX_TRUE;
}

px_void PX_StringClear(px_string *str)
{
	str->buffer[0]='\0';
}

px_bool PX_StringCatChar(px_string *str,px_char ch)
{
	px_uchar shl=0;
	px_char *old=str->buffer;
	px_int length=PX_strlen(str->buffer)+1;
	if (length<str->bufferlen)
	{
		str->buffer[length-1]=ch;
		str->buffer[length]='\0';
		return PX_TRUE;
	}

	while ((px_int)(1<<++shl)<=length);
	str->bufferlen=(1<<shl);
	str->buffer=(px_char *)MP_Malloc(str->mp,str->bufferlen);
	if(!str->buffer)return PX_FALSE;
	str->buffer[0]='\0';
	PX_strcat(str->buffer,old);
	str->buffer[length-1]=ch;
	str->buffer[length]='\0';
	if(old)
	MP_Free(str->mp,old);
	return PX_TRUE;
}

px_int PX_StringLen(px_string *str)
{
	return PX_strlen(str->buffer);
}

px_bool PX_StringCopy(px_string *dest,px_string *res)
{
	if (res==dest)
	{
		PX_ERROR("px_sting Self-copied error");
		return PX_FALSE;
	}
	dest->buffer[0]='\0';
	return PX_StringCat(dest,res->buffer);
}

px_bool PX_StringInsertChar(px_string *str,px_int index,px_char ch)
{
	px_int cpysize=PX_strlen(str->buffer+index);
	if (PX_StringCatChar(str,' '))
	{
		PX_memcpy(str->buffer+index+1,str->buffer+index,cpysize);
		str->buffer[index]=ch;
	}
	return PX_FALSE;
}

px_bool PX_StringRemoveChar(px_string *str,px_int index)
{
	if (index>=0&&index<PX_strlen(str->buffer))
	{
		PX_memcpy(str->buffer+index,str->buffer+index+1,PX_strlen(str->buffer+index));
		return PX_TRUE;
	}
	return PX_FALSE;
}

px_void PX_StringReplace(px_string *str,px_char *source,px_char *replaceto)
{
	px_string tempstr;
	int i;
	if (PX_StringLen(str)==0)
	{
		return;   
	}
	PX_StringInit(str->mp,&tempstr);
	PX_StringCopy(&tempstr,str);
	PX_StringClear(str);

	for (i=0;i<=PX_StringLen(&tempstr)-PX_strlen(source);i++)
	{
		if (PX_memequ(tempstr.buffer+i,source,PX_strlen(source)))
		{
			PX_StringCat(str,replaceto);
			i+=PX_strlen(source)-1;
		}
		else
		{
			PX_StringCatChar(str,tempstr.buffer[i]);
		}
	}
	PX_StringCat(str,tempstr.buffer+i);
	PX_StringFree(&tempstr);
}

px_void PX_StringReplaceRange(px_string *str,px_int startindex,px_int endindex,px_char *replaceto)
{
	px_string tempStr;

	if (startindex>endindex)
	{
		PX_ERROR("string trim error");
		return;
	}

	if (endindex>=PX_strlen(str->buffer))
	{
		PX_ERROR("string trim error");
		return;
	}

	PX_StringInit(str->mp,&tempStr);
	PX_StringCopy(&tempStr,str);

	PX_StringClear(str);

	tempStr.buffer[startindex]=0;
	PX_StringCat(str,tempStr.buffer);
	PX_StringCat(str,replaceto);
	PX_StringCat(str,tempStr.buffer+endindex+1);
	PX_StringFree(&tempStr);
}

#define PX_STRING_TRIMER_REG_COUNT 16

px_bool PX_StringTrimer_Solve(px_string *pstring,px_char *parseCode,px_char *ReplaceCode)
{
	px_int oft,j,k,resi,repj,regindex;
	px_char regbuf[3];
	px_char endc;
	px_bool ret;
	px_string reg[PX_STRING_TRIMER_REG_COUNT],matchedString,replaceString;
	px_bool mark[PX_STRING_TRIMER_REG_COUNT],matchd;
	ret=PX_FALSE;
	for (oft=0;oft<PX_STRING_TRIMER_REG_COUNT;oft++)
	{
		PX_StringInit(pstring->mp,reg+oft);
		mark[oft]=PX_FALSE;
	}

	PX_StringInit(pstring->mp,&matchedString);

	oft=0;
	
	while(pstring->buffer[oft])
	{
		repj=0;
		resi=oft;
		matchd=PX_FALSE;

		while(PX_TRUE)
		{
			
			if (parseCode[repj]=='%')
			{
				j=0;
				repj++;

				PX_StringClear(&matchedString);

				while(PX_TRUE)
				{
					if (parseCode[repj]>='0'&&parseCode[repj]<='9')
					{
						regbuf[j++]=parseCode[repj++];
						if (j>2)
						{
							goto _DONE;
						}
						regbuf[j]=0;
						
					}
					else
						break;
					
				}
				if (j==0)
				{
					goto _DONE;
				}
				regindex=PX_atoi(regbuf);
				if (regindex>=PX_STRING_TRIMER_REG_COUNT)
				{
					goto _DONE;
				}
				endc=parseCode[repj];

				while (PX_TRUE)
				{
					if (pstring->buffer[resi]==endc)
					{
						break;
					}
					if (pstring->buffer[resi]=='\0')
					{
						goto _DONE;
					}
					PX_StringCatChar(&matchedString,pstring->buffer[resi]);
					resi++;
				}

				if (mark[regindex])
				{
					//reg 0 as wildcard
					if (PX_strequ(reg[regindex].buffer,matchedString.buffer)||!regindex)
					{
						matchd=PX_TRUE;
					}
					else
					{
						matchd=PX_FALSE;
						break;
					}
				}
				else
				{
					PX_StringCopy(&reg[regindex],&matchedString);
					mark[regindex]=PX_TRUE;
				}
				
			}
			else
			{
				if (parseCode[repj]=='\0')
				{
					matchd=PX_TRUE;
					break;
				}
				if (pstring->buffer[resi]=='\0')
				{
					break;
				}
				if (pstring->buffer[resi]==parseCode[repj])
				{
					resi++;
					repj++;
				}
				else
					break;
			}
		}



		if (matchd)
		{
			PX_StringInit(pstring->mp,&replaceString);
			j=0;
			k=0;
			while(PX_TRUE)
			{
				if (ReplaceCode[j]==0)
				{
					break;
				}
				if (ReplaceCode[j]=='%')
				{
					j++;
					k=0;
					while(PX_TRUE)
					{
						if (ReplaceCode[j]>='0'&&ReplaceCode[j]<='9')
						{
							regbuf[k++]=ReplaceCode[j++];
							
							if (k>2)
							{
								PX_StringFree(&replaceString);
								goto _DONE;
							}

							regbuf[k]='\0';
							
							regindex=PX_atoi(regbuf);
							if (regindex>=PX_STRING_TRIMER_REG_COUNT)
							{
								PX_StringFree(&replaceString);
								goto _DONE;
							}
							PX_StringCat(&replaceString,reg[regindex].buffer);
						}
						else
							break;
					}
				}
				else
				{
					PX_StringCatChar(&replaceString,ReplaceCode[j++]);
				}
			}


			PX_StringReplaceRange(pstring,oft,resi-1,replaceString.buffer);
			ret=PX_TRUE;

			oft+=PX_strlen(replaceString.buffer);

			PX_StringFree(&replaceString);
		}
		else
		{
			oft++;
		}

		for (k=0;k<PX_STRING_TRIMER_REG_COUNT;k++)
		{
			mark[k]=PX_FALSE;
		}
	}

_DONE:
	for (oft=0;oft<PX_STRING_TRIMER_REG_COUNT;oft++)
	{
		PX_StringFree(reg+oft);
	}
	PX_StringFree(&matchedString);
	return ret;
}

px_void PX_StringInitFromConst(px_string *str,const px_char *constchar)
{
	str->buffer=(px_char *)constchar;
	str->mp=PX_NULL;
	str->bufferlen=0;

}


