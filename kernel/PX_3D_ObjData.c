#include "PX_3D_ObjData.h"

static PX_LEXER_LEXEME_TYPE PX_3D_ObjectDataNextTokenSN(px_lexer *lexer)
{
	PX_LEXER_LEXEME_TYPE type;
	while ((type= PX_LexerGetNextLexeme(lexer))==PX_LEXER_LEXEME_TYPE_SPACER||type==PX_LEXER_LEXEME_TYPE_NEWLINE);

	return type;
}

px_bool PX_3D_ObjectDataInitialize(px_memorypool *mp,PX_3D_ObjectData *ObjectData)
{
	ObjectData->mp=mp;
	
	if(!PX_VectorInitialize(mp,&ObjectData->v,sizeof(PX_3D_ObjectData_v),8)) return PX_FALSE;
	if(!PX_VectorInitialize(mp,&ObjectData->vt,sizeof(PX_3D_ObjectData_vt),8)) return PX_FALSE;
	if(!PX_VectorInitialize(mp,&ObjectData->vn,sizeof(PX_3D_ObjectData_vn),8)) return PX_FALSE;
	if(!PX_VectorInitialize(mp,&ObjectData->face,sizeof(PX_3D_ObjectDataFace),8)) return PX_FALSE;
	if(!PX_VectorInitialize(mp,&ObjectData->mtlFile,sizeof(px_string),8)) return PX_FALSE;
	if(!PX_VectorInitialize(mp,&ObjectData->mtlName,sizeof(px_string),8)) return PX_FALSE;
	return PX_TRUE;
}

px_bool PX_3D_ObjectDataLoad(PX_3D_ObjectData *ObjectData,const px_byte *data,px_int size)
{
	PX_LEXER_STATE state;
	px_lexer Lexer;
	PX_LEXER_LEXEME_TYPE type;
	PX_LexerInitialize(&Lexer,ObjectData->mp);
	PX_LexerRegisterSpacer(&Lexer,' ');
	PX_LexerRegisterSpacer(&Lexer,'\t');
	PX_LexerRegisterComment(&Lexer,"#","\n");
	PX_LexerRegisterDelimiter(&Lexer,'/');
	PX_LexerSetTokenCase(&Lexer,PX_LEXER_LEXEME_CASE_LOWER);

	if(!PX_LexerLoadSourceFromMemory(&Lexer,(const px_char *)data)) return PX_FALSE;
	
	while (PX_TRUE)
	{
		type=PX_3D_ObjectDataNextTokenSN(&Lexer);
		if (type==PX_LEXER_LEXEME_TYPE_TOKEN)
		{
			//g s o <param> note:The flag is useless,we register it but not solve it 

			if (PX_strequ(Lexer.CurLexeme.buffer,"g")\
				||PX_strequ(Lexer.CurLexeme.buffer,"s")\
				||PX_strequ(Lexer.CurLexeme.buffer,"o"))
			{
				while (PX_TRUE)
				{
					type=PX_LexerGetNextLexeme(&Lexer);
					if (type==PX_LEXER_LEXEME_TYPE_NEWLINE||type==PX_LEXER_LEXEME_TYPE_END)
					{
						break;
					}
				}
				continue;
			}


			if (PX_strequ(Lexer.CurLexeme.buffer,"mtllib"))
			{
				type=PX_3D_ObjectDataNextTokenSN(&Lexer);
				if (type==PX_LEXER_LEXEME_TYPE_TOKEN)
				{
					px_string strName;
					if(!PX_StringInitialize(ObjectData->mp,&strName))
					{
						goto _ERROR;
					}
					PX_StringSet(&strName,Lexer.CurLexeme.buffer);
					if(!PX_VectorPushback(&ObjectData->mtlFile,&strName)) goto _ERROR;
				}
				else
				{
					goto _ERROR;
				}
				continue;
			}

			//usemtl <param>
			if (PX_strequ(Lexer.CurLexeme.buffer,"usemtl"))
			{
				type=PX_3D_ObjectDataNextTokenSN(&Lexer);
				if (type==PX_LEXER_LEXEME_TYPE_TOKEN)
				{
					px_string strName;
					if(!PX_StringInitialize(ObjectData->mp,&strName))
					{
						goto _ERROR;
					}
					PX_StringSet(&strName,Lexer.CurLexeme.buffer);
					if(!PX_VectorPushback(&ObjectData->mtlName,&strName)) goto _ERROR;
				}
				else
				{
					goto _ERROR;
				}
				continue;
			}

			//v <param> <param> <param>
			if (PX_strequ(Lexer.CurLexeme.buffer,"v"))
			{
				px_int i;
				px_float v[3];
				PX_3D_ObjectData_v ov;

				for (i=0;i<3;i++)
				{
					type=PX_3D_ObjectDataNextTokenSN(&Lexer);
					if (!PX_LexerIsLememeIsNumeric(&Lexer))
					{
						PX_LOG("<ERROR> Numeric block is expected but not found");
						goto _ERROR;
					}
					v[i]=(px_float)PX_atof(Lexer.CurLexeme.buffer);
					
				}
				ov.x=v[0];
				ov.y=v[1];
				ov.z=v[2];
				if(!PX_VectorPushback(&ObjectData->v,&ov)) goto _ERROR;
				continue;
			}

			//vn <param>/<param>/<param>
			if (PX_strequ(Lexer.CurLexeme.buffer,"vn"))
			{
				px_int i;
				px_float v[3];
				PX_3D_ObjectData_vn ov;
				for (i=0;i<3;i++)
				{
					type=PX_3D_ObjectDataNextTokenSN(&Lexer);
					if (!PX_LexerIsLememeIsNumeric(&Lexer))
					{
						PX_LOG("<ERROR> Numeric block is expected but not found");
						goto _ERROR;
					}
					v[i]=(px_float)PX_atof(Lexer.CurLexeme.buffer);
				}
				ov.x=v[0];
				ov.y=v[1];
				ov.z=v[2];
				if(!PX_VectorPushback(&ObjectData->vn,&ov)) goto _ERROR;



				continue;
			}

			//vt <param> <param>
			if (PX_strequ(Lexer.CurLexeme.buffer,"vt"))
			{
				px_int i;
				px_float v[2];
				PX_3D_ObjectData_vt ov;
				for (i=0;i<2;i++)
				{
					type=PX_3D_ObjectDataNextTokenSN(&Lexer);
					if (!PX_LexerIsLememeIsNumeric(&Lexer))
					{
						PX_LOG("<ERROR> Numeric block is expected but not found");
						goto _ERROR;
					}
					v[i]=(px_float)PX_atof(Lexer.CurLexeme.buffer);
				}
				ov.u=v[0];
				ov.v=v[1];
				if(!PX_VectorPushback(&ObjectData->vt,&ov)) goto _ERROR;

				while (PX_TRUE)
				{
					type=PX_LexerGetNextLexeme(&Lexer);
					if (type==PX_LEXER_LEXEME_TYPE_NEWLINE||type==PX_LEXER_LEXEME_TYPE_END)
					{
						break;
					}
				}

				continue;
			}

			//f <v/vt/vn> <v/vt/vn> <v/vt/vn>
			PX_LexerGetState(&Lexer);
			if (PX_strequ(Lexer.CurLexeme.buffer,"f"))
			{
				PX_3D_ObjectDataFace Face;
				px_int v;
				px_int i;

				//	Face.mtlFileNameIndex=GetMtlFileIndex();
				Face.mtlNameIndex=ObjectData->mtlName.size-1;
				Face.mtlFileNameIndex=ObjectData->mtlFile.size-1;

				for (i=0;i<3;i++)
				{
					Face.v[i].v=-1;
					Face.v[i].vt=-1;
					Face.v[i].vn=-1;
					type=PX_3D_ObjectDataNextTokenSN(&Lexer);
					if (!PX_LexerIsLememeIsNumeric(&Lexer))
					{
						PX_LOG("<ERROR> Numeric block is expected but not found");
						goto _ERROR;
					}
					v=PX_atoi(Lexer.CurLexeme.buffer);
					
					if(v>ObjectData->v.size)
						goto _ERROR;

					Face.v[i].v=v;

					state=PX_LexerGetState(&Lexer);
					type=PX_3D_ObjectDataNextTokenSN(&Lexer);
					
					// vt
					if (type==PX_LEXER_LEXEME_TYPE_DELIMITER&&Lexer.Symbol=='/')
					{
						state=PX_LexerGetState(&Lexer);
						type=PX_3D_ObjectDataNextTokenSN(&Lexer);
						if (PX_LexerIsLememeIsNumeric(&Lexer))
						{
							v=PX_atoi(Lexer.CurLexeme.buffer);
							if(v>ObjectData->vt.size)
								goto _ERROR;
							Face.v[i].vt=v;
						}
						else
						{
							PX_LexerSetState(state);
						}
					}
					else
					{
						PX_LexerSetState(state);
						continue;
					}

					// vn
					state=PX_LexerGetState(&Lexer);
					type=PX_3D_ObjectDataNextTokenSN(&Lexer);

					if (type==PX_LEXER_LEXEME_TYPE_DELIMITER&&Lexer.Symbol=='/')
					{
						state=PX_LexerGetState(&Lexer);
						type=PX_3D_ObjectDataNextTokenSN(&Lexer);
						if (PX_LexerIsLememeIsNumeric(&Lexer))
						{
							v=PX_atoi(Lexer.CurLexeme.buffer);
							if(v>ObjectData->vn.size)
								goto _ERROR;
							Face.v[i].vn=v;
						}
						else
						{
							PX_LexerSetState(state);
						}
					}
					else
					{
						PX_LexerSetState(state);
						continue;
					}

				}
				if(!PX_VectorPushback(&ObjectData->face,&Face)) goto _ERROR;
				continue;
			}
		}

		if (type==PX_LEXER_LEXEME_TYPE_END)
		{
			break;
		}

		goto _ERROR;

	}
	PX_LexerFree(&Lexer);
	return PX_TRUE;
_ERROR:
	PX_3D_ObjectDataFree(ObjectData);
	PX_LexerFree(&Lexer);
	return PX_FALSE;
}

px_bool PX_3D_ObjectDataToRenderList(PX_3D_ObjectData *ObjectData,PX_3D_RenderList *renderList)
{
	px_int i,j;
	PX_3D_Face face;
	PX_3D_ObjectDataFace *pObjFace;
	for (i=0;i<ObjectData->face.size;i++)
	{
		pObjFace=PX_VECTORAT(PX_3D_ObjectDataFace,&ObjectData->face,i);
		PX_memset(&face,0,sizeof(PX_3D_Face));
		for (j=0;j<3;j++)
		{
			if(pObjFace->v[j].v>0&&pObjFace->v[j].v<=ObjectData->v.size)
			{
				PX_3D_ObjectData_v *pv=PX_VECTORAT(PX_3D_ObjectData_v,&ObjectData->v,pObjFace->v[j].v-1);
				face.vertex[j].position=PX_POINT4D(pv->x,pv->y,pv->z);
			}
			else
				goto _ERROR;

			if(pObjFace->v[j].vn>0&&pObjFace->v[j].vn<=ObjectData->vn.size)
			{
				PX_3D_ObjectData_vn *pvn=PX_VECTORAT(PX_3D_ObjectData_vn,&ObjectData->vn,pObjFace->v[j].vn-1);
				face.vertex[j].normal=PX_POINT4D(pvn->x,pvn->y,pvn->z);
			}
			else
				goto _ERROR;

			if(pObjFace->v[j].vt>0&&pObjFace->v[j].vt<=ObjectData->vt.size)
			{
				PX_3D_ObjectData_vt *pvt=PX_VECTORAT(PX_3D_ObjectData_vt,&ObjectData->vt,pObjFace->v[j].vt-1);
				face.vertex[j].u=pvt->u;
				face.vertex[j].v=pvt->v;
			}
			else
				goto _ERROR;

			face.vertex[j].clr=PX_COLOR(255,0,0,0);
		}
		if(!PX_3D_RenderListPush(renderList,face))
			goto _ERROR;
	}
	return PX_TRUE;
_ERROR:
	PX_3D_RenderListReset(renderList);
	return PX_FALSE;
}

px_void PX_3D_ObjectDataFree(PX_3D_ObjectData *ObjectData)
{
	px_int i;
	PX_VectorFree(&ObjectData->v);
	PX_VectorFree(&ObjectData->vt);
	PX_VectorFree(&ObjectData->vn);
	PX_VectorFree(&ObjectData->face);
	for (i=0;i<ObjectData->mtlName.size;i++)
	{
		PX_StringFree(PX_VECTORAT(px_string,&ObjectData->mtlName,i));
	}
	PX_VectorFree(&ObjectData->mtlName);

	for (i=0;i<ObjectData->mtlFile.size;i++)
	{
		PX_StringFree(PX_VECTORAT(px_string,&ObjectData->mtlFile,i));
	}
	PX_VectorFree(&ObjectData->mtlFile);
	
}

