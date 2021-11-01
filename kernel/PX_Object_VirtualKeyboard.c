#include "PX_Object_VirtualKeyboard.h"
PX_Object_VirtualKeyBoard *PX_Object_GetVirtualKeyBoard(PX_Object *pObject)
{
	if (!pObject)
	{
		return PX_NULL;
	}
	if (pObject->Type==PX_OBJECT_TYPE_VKEYBOARD)
	{
		return (PX_Object_VirtualKeyBoard *)pObject->pObject;
	}
	return PX_NULL;
}

px_void PX_Object_VirtualKeyBoardRender(px_surface *psurface, PX_Object *pObject,px_uint elapsed)
{
	px_int i;
	px_int oftx;
	px_int ofty;
	PX_Object_VirtualKeyBoard *keyBoard=(PX_Object_VirtualKeyBoard *)pObject->pObject;
	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;

	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;

	oftx=(px_int)objx;
	ofty=(px_int)objy;

	//clear background
	PX_SurfaceClear(psurface,(px_int)objx,(px_int)objy,(px_int)(objx+objWidth),(px_int)(objy+objHeight),keyBoard->backgroundColor);

	for (i=0;i<PX_COUNTOF(keyBoard->Keys);i++)
	{
		//draw rect
		
		if (keyBoard->Keys[i].bDown||keyBoard->Keys[i].bhold)
		{
			PX_GeoDrawRect(psurface,oftx+keyBoard->Keys[i].x,ofty+keyBoard->Keys[i].y,oftx+keyBoard->Keys[i].x+keyBoard->Keys[i].width,ofty+keyBoard->Keys[i].y+keyBoard->Keys[i].height,keyBoard->pushColor);
		}
		else if (keyBoard->Keys[i].bCursor)
		{
			PX_GeoDrawRect(psurface,oftx+keyBoard->Keys[i].x,ofty+keyBoard->Keys[i].y,oftx+keyBoard->Keys[i].x+keyBoard->Keys[i].width,ofty+keyBoard->Keys[i].y+keyBoard->Keys[i].height,keyBoard->cursorColor);
		}

		PX_GeoDrawBorder(psurface,oftx+keyBoard->Keys[i].x,ofty+keyBoard->Keys[i].y,oftx+keyBoard->Keys[i].x+keyBoard->Keys[i].width,ofty+keyBoard->Keys[i].y+keyBoard->Keys[i].height,1,keyBoard->borderColor);
		
		if (keyBoard->bShift||keyBoard->bUpper)
		{
			PX_FontModuleDrawText(psurface,PX_NULL,oftx+keyBoard->Keys[i].x+2,ofty+keyBoard->Keys[i].y+2,PX_ALIGN_LEFTTOP,keyBoard->Keys[i].u_key,keyBoard->borderColor);
		}
		else
		{
			PX_FontModuleDrawText(psurface,PX_NULL,oftx+keyBoard->Keys[i].x+2,ofty+keyBoard->Keys[i].y+2,PX_ALIGN_LEFTTOP,keyBoard->Keys[i].d_key,keyBoard->borderColor);
		}
	}

}

px_void PX_Object_VirtualKeyBoardOnCursorUp(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	px_float x=PX_Object_Event_GetCursorX(e);
	px_float y=PX_Object_Event_GetCursorY(e);
	px_int i;
	PX_Object_VirtualKeyBoard *pkb=(PX_Object_VirtualKeyBoard *)pObject->pObject;
	if (!PX_ObjectIsPointInRegion(pObject,(px_float)x,(px_float)y))
	{
		return;
	}

	for (i=0;i<PX_COUNTOF(pkb->Keys);i++)
	{
		
			pkb->Keys[i].bDown=PX_FALSE;
	}
}

px_void PX_Object_VirtualKeyBoardOnCursorMove(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	px_float x=PX_Object_Event_GetCursorX(e);
	px_float y=PX_Object_Event_GetCursorY(e);
	px_int i;
	px_int kh=(px_int)(pObject->Height/5);
	PX_Object_VirtualKeyBoard *pkb=(PX_Object_VirtualKeyBoard *)pObject->pObject;
	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;

	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;


	x=(PX_Object_Event_GetCursorX(e)-objx);
	y=(PX_Object_Event_GetCursorY(e)-objy);


	for (i=0;i<PX_COUNTOF(pkb->Keys);i++)
	{
		if(!pkb->Keys[i].bDown)
		{
			pkb->Keys[i].bCursor=PX_FALSE;
		}
		else
		{
			if (!PX_isPointInRect(PX_POINT((px_float)x,(px_float)y,0),PX_RECT((px_float)pkb->Keys[i].x,(px_float)pkb->Keys[i].y,(px_float)pkb->Keys[i].width,(px_float)pkb->Keys[i].height)))
				pkb->Keys[i].bDown=PX_FALSE;
		}
	}

	if (y<kh)
	{
		for (i=0;i<=13;i++)
		{
			if (PX_isPointInRect(PX_POINT((px_float)x,(px_float)y,0),PX_RECT((px_float)pkb->Keys[i].x,(px_float)pkb->Keys[i].y,(px_float)pkb->Keys[i].width,(px_float)pkb->Keys[i].height)))
			{
				pkb->Keys[i].bCursor=PX_TRUE;
				break;
			}
		}
	}
	else if (y<2*kh)
	{
		for (i=14;i<=27;i++)
		{
			if (PX_isPointInRect(PX_POINT((px_float)x,(px_float)y,0),PX_RECT((px_float)pkb->Keys[i].x,(px_float)pkb->Keys[i].y,(px_float)pkb->Keys[i].width,(px_float)pkb->Keys[i].height)))
			{
				pkb->Keys[i].bCursor=PX_TRUE;
				break;
			}
		}
	}
	else if (y<3*kh)
	{
		for (i=28;i<=40;i++)
		{
			if (PX_isPointInRect(PX_POINT((px_float)x,(px_float)y,0),PX_RECT((px_float)pkb->Keys[i].x,(px_float)pkb->Keys[i].y,(px_float)pkb->Keys[i].width,(px_float)pkb->Keys[i].height)))
			{
				pkb->Keys[i].bCursor=PX_TRUE;
				break;
			}
		}
	}
	else if (y<4*kh)
	{
		for (i=41;i<=52;i++)
		{
			if (PX_isPointInRect(PX_POINT((px_float)x,(px_float)y,0),PX_RECT((px_float)pkb->Keys[i].x,(px_float)pkb->Keys[i].y,(px_float)pkb->Keys[i].width,(px_float)pkb->Keys[i].height)))
			{
				pkb->Keys[i].bCursor=PX_TRUE;
				break;
			}
		}
	}
	else
	{
		for (i=53;i<=55;i++)
		{
			if (PX_isPointInRect(PX_POINT((px_float)x,(px_float)y,0),PX_RECT((px_float)pkb->Keys[i].x,(px_float)pkb->Keys[i].y,(px_float)pkb->Keys[i].width,(px_float)pkb->Keys[i].height)))
			{
				pkb->Keys[i].bCursor=PX_TRUE;
				break;
			}
		}
	}

	
	
}

px_void PX_Object_VirtualKeyBoardOnCursorDown(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	px_float x=PX_Object_Event_GetCursorX(e);
	px_float y=PX_Object_Event_GetCursorY(e);
	px_int i;
	px_int kh=(px_int)(pObject->Height/5);
	PX_Object_VirtualKeyBoard *pkb=(PX_Object_VirtualKeyBoard *)pObject->pObject;

	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;

	PX_Object_Event stringEvent;

	px_char strEve[2]={0};

	stringEvent.Event=PX_OBJECT_EVENT_STRING;
	PX_Object_Event_SetStringPtr(&stringEvent,strEve);


	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;

	if (!PX_ObjectIsPointInRegion(pObject,(px_float)x,(px_float)y))
	{
		PX_ObjectClearFocus(pObject);
		return;
	}

	PX_ObjectSetFocus(pObject);

	x=(PX_Object_Event_GetCursorX(e)-objx);
	y=(PX_Object_Event_GetCursorY(e)-objy);


	if (y<kh)
	{
		for (i=0;i<=13;i++)
		{
			if (PX_isPointInRect(PX_POINT((px_float)x,(px_float)y,0),PX_RECT((px_float)pkb->Keys[i].x,(px_float)pkb->Keys[i].y,(px_float)pkb->Keys[i].width,(px_float)pkb->Keys[i].height)))
			{
				pkb->Keys[i].bDown=PX_TRUE;
				break;
			}
		}
	}
	else if (y<2*kh)
	{
		for (i=14;i<=27;i++)
		{
			if (PX_isPointInRect(PX_POINT((px_float)x,(px_float)y,0),PX_RECT((px_float)pkb->Keys[i].x,(px_float)pkb->Keys[i].y,(px_float)pkb->Keys[i].width,(px_float)pkb->Keys[i].height)))
			{
				pkb->Keys[i].bDown=PX_TRUE;
				break;
			}
		}
	}
	else if (y<3*kh)
	{
		for (i=28;i<=40;i++)
		{
			if (PX_isPointInRect(PX_POINT((px_float)x,(px_float)y,0),PX_RECT((px_float)pkb->Keys[i].x,(px_float)pkb->Keys[i].y,(px_float)pkb->Keys[i].width,(px_float)pkb->Keys[i].height)))
			{
				pkb->Keys[i].bDown=PX_TRUE;
				break;
			}
		}
	}
	else if (y<4*kh)
	{
		for (i=41;i<=52;i++)
		{
			if (PX_isPointInRect(PX_POINT((px_float)x,(px_float)y,0),PX_RECT((px_float)pkb->Keys[i].x,(px_float)pkb->Keys[i].y,(px_float)pkb->Keys[i].width,(px_float)pkb->Keys[i].height)))
			{
				pkb->Keys[i].bDown=PX_TRUE;
				break;
			}
		}
	}
	else
	{
		for (i=53;i<=55;i++)
		{
			if (PX_isPointInRect(PX_POINT((px_float)x,(px_float)y,0),PX_RECT((px_float)pkb->Keys[i].x,(px_float)pkb->Keys[i].y,(px_float)pkb->Keys[i].width,(px_float)pkb->Keys[i].height)))
			{
				pkb->Keys[i].bDown=PX_TRUE;
				break;
			}
		}
	}

	if (i==13)
	{
		pkb->functionCode=8;
		if (pkb->LinkerObject)
		{
			strEve[0]=pkb->functionCode;
			PX_ObjectPostEvent(pkb->LinkerObject,stringEvent);
		}
	}
	else if(i==14)
	{
		pkb->functionCode='\t';
		if (pkb->LinkerObject)
		{
			strEve[0]=pkb->functionCode;
			PX_ObjectPostEvent(pkb->LinkerObject,stringEvent);
		}
	}
	else if(i==28)
	{
		pkb->bUpper=!pkb->bUpper;
		if (pkb->bUpper)
		{
			pkb->Keys[28].bhold=PX_TRUE;
		}
		else
		{
			pkb->Keys[28].bhold=PX_FALSE;
		}
		pkb->functionCode=0;
	}
	else if(i==40)
	{
		pkb->functionCode='\r';
		if (pkb->LinkerObject)
		{
			strEve[0]=pkb->functionCode;
			PX_ObjectPostEvent(pkb->LinkerObject,stringEvent);
		}
	}
	else if(i==41)
	{
		pkb->bShift=!pkb->bShift;
		if (pkb->bShift)
		{
			pkb->Keys[41].bhold=PX_TRUE;
		}
		else
		{
			pkb->Keys[41].bhold=PX_FALSE;
		}
		pkb->functionCode=0;
	}
	else if(i==52)
	{
		pkb->bShift=!pkb->bShift;
		if (pkb->bShift)
		{
			pkb->Keys[52].bhold=PX_TRUE;
		}
		else
		{
			pkb->Keys[52].bhold=PX_FALSE;
		}
		pkb->functionCode=0;
	}
	else if(i==53)
	{
		pkb->bCtrl=!pkb->bCtrl;
		if (pkb->bCtrl)
		{
			pkb->Keys[53].bhold=PX_TRUE;
		}
		else
		{
			pkb->Keys[53].bhold=PX_FALSE;
		}
		pkb->functionCode=0;
	}
	else if(i==54)
	{
		pkb->bAlt=!pkb->bAlt;
		if (pkb->bAlt)
		{
			pkb->Keys[54].bhold=PX_TRUE;
		}
		else
		{
			pkb->Keys[54].bhold=PX_FALSE;
		}
		pkb->functionCode=0;
	}
	else if(i==55)
	{
		pkb->functionCode=' ';
		if (pkb->LinkerObject)
		{
			strEve[0]=pkb->functionCode;
			PX_ObjectPostEvent(pkb->LinkerObject,stringEvent);
		}
	}
	else
	{
		if (pkb->bUpper||pkb->bShift)
		{
			pkb->functionCode=pkb->Keys[i].u_key[0];
		}
		else
		{
			pkb->functionCode=pkb->Keys[i].d_key[0];
		}

		pkb->bShift=PX_FALSE;
		pkb->Keys[52].bhold=PX_FALSE;
		pkb->Keys[41].bhold=PX_FALSE;
		if (pkb->LinkerObject)
		{
			strEve[0]=pkb->functionCode;
			PX_ObjectPostEvent(pkb->LinkerObject,stringEvent);
		}
	}

	
}

PX_Object* PX_Object_VirtualKeyBoardCreate(px_memorypool *mp, PX_Object *Parent,px_int x,px_int y,px_int width,px_int height)
{
	px_int i;
	px_int kw,kh;
	px_int xoffset,yoffset;
	PX_Object_VirtualKeyBoard keyboard;
	px_char line1_upper[]="~!@#$%^&*()_+";
	px_char line1_lower[]="`1234567890-=";
	px_char line2_upper[]="QWERTYUIOP{}|";
	px_char line2_lower[]="qwertyuiop[]\\";
	px_char line3_upper[]="ASDFGHJKL:\"";
	px_char line3_lower[]="asdfghjkl;'";
	px_char line4_upper[]="ZXCVBNM<>?";
	px_char line4_lower[]="zxcvbnm,./";


	PX_Object *keyboardObject;

	if (width<24*15||height<16*5)
	{
		return PX_NULL;
	}
	PX_memset(&keyboard,0,sizeof(PX_Object_VirtualKeyBoard));

	kw=width/15;
	kh=height/5;

	xoffset=0;
	yoffset=0;
	for (i=0;i<=12;i++)
	{
		keyboard.Keys[i].d_key[0]=line1_lower[i];
		keyboard.Keys[i].u_key[0]=line1_upper[i];
		keyboard.Keys[i].x=xoffset;
		keyboard.Keys[i].y=yoffset;
		keyboard.Keys[i].width=kw;
		keyboard.Keys[i].height=kh;
		xoffset+=kw;
	}

	PX_strset(keyboard.Keys[13].d_key,"Back");
	PX_strset(keyboard.Keys[13].u_key,"Back");
	keyboard.Keys[13].x=xoffset;
	keyboard.Keys[13].y=0;
	keyboard.Keys[13].width=kw*2;
	keyboard.Keys[13].height=kh;


	xoffset=0;
	yoffset=kh;

	PX_strset(keyboard.Keys[14].d_key,"Tab");
	PX_strset(keyboard.Keys[14].u_key,"Tab");

	keyboard.Keys[14].x=xoffset;
	keyboard.Keys[14].y=kh;
	keyboard.Keys[14].width=(px_int)(kw*1.5);
	keyboard.Keys[14].height=kh;

	xoffset+=keyboard.Keys[14].width;

	for (i=15;i<=26;i++)
	{
		keyboard.Keys[i].d_key[0]=line2_lower[i-15];
		keyboard.Keys[i].u_key[0]=line2_upper[i-15];
		keyboard.Keys[i].x=xoffset;
		keyboard.Keys[i].y=yoffset;
		keyboard.Keys[i].width=kw;
		keyboard.Keys[i].height=kh;
		xoffset+=keyboard.Keys[i].width;
	}
	PX_strset(keyboard.Keys[27].d_key,"\\");
	PX_strset(keyboard.Keys[27].u_key,"|");
	keyboard.Keys[27].x=xoffset;
	keyboard.Keys[27].y=yoffset;
	keyboard.Keys[27].width=(px_int)(kw*1.5);;
	keyboard.Keys[27].height=kh;

	xoffset=0;
	yoffset=2*kh;

	PX_strset(keyboard.Keys[28].d_key,"Upper");
	PX_strset(keyboard.Keys[28].u_key,"Upper");
	keyboard.Keys[28].x=xoffset;
	keyboard.Keys[28].y=yoffset;
	keyboard.Keys[28].width=(px_int)(kw*2);
	keyboard.Keys[28].height=kh;
	xoffset+=keyboard.Keys[28].width;

	for (i=29;i<=39;i++)
	{
		keyboard.Keys[i].d_key[0]=line3_lower[i-29];
		keyboard.Keys[i].u_key[0]=line3_upper[i-29];
		keyboard.Keys[i].x=xoffset;
		keyboard.Keys[i].y=yoffset;
		keyboard.Keys[i].width=kw;
		keyboard.Keys[i].height=kh;
		xoffset+=keyboard.Keys[i].width;
	}

	PX_strset(keyboard.Keys[40].d_key,"Enter");
	PX_strset(keyboard.Keys[40].u_key,"Enter");
	keyboard.Keys[40].x=xoffset;
	keyboard.Keys[40].y=yoffset;
	keyboard.Keys[40].width=(px_int)(kw*2);;
	keyboard.Keys[40].height=kh;
	xoffset+=keyboard.Keys[40].width;

	xoffset=0;
	yoffset=3*kh;

	PX_strset(keyboard.Keys[41].d_key,"Shift");
	PX_strset(keyboard.Keys[41].u_key,"Shift");
	keyboard.Keys[41].x=xoffset;
	keyboard.Keys[41].y=yoffset;
	keyboard.Keys[41].width=(px_int)(kw*2.5);;
	keyboard.Keys[41].height=kh;
	xoffset+=keyboard.Keys[41].width;

	for (i=42;i<=51;i++)
	{
		keyboard.Keys[i].d_key[0]=line4_lower[i-42];
		keyboard.Keys[i].u_key[0]=line4_upper[i-42];
		keyboard.Keys[i].x=xoffset;
		keyboard.Keys[i].y=yoffset;
		keyboard.Keys[i].width=kw;
		keyboard.Keys[i].height=kh;
		xoffset+=keyboard.Keys[i].width;
	}

	PX_strset(keyboard.Keys[52].d_key,"Shift");
	PX_strset(keyboard.Keys[52].u_key,"Shift");
	keyboard.Keys[52].x=xoffset;
	keyboard.Keys[52].y=yoffset;
	keyboard.Keys[52].width=(px_int)(kw*2.5);;
	keyboard.Keys[52].height=kh;
	xoffset+=keyboard.Keys[52].width;

	xoffset=0;
	yoffset=4*kh;
	PX_strset(keyboard.Keys[53].d_key,"Ctrl");
	PX_strset(keyboard.Keys[53].u_key,"Ctrl");
	keyboard.Keys[53].x=xoffset;
	keyboard.Keys[53].y=yoffset;
	keyboard.Keys[53].width=(px_int)(kw*1.5);;
	keyboard.Keys[53].height=kh;
	xoffset+=keyboard.Keys[53].width;

	PX_strset(keyboard.Keys[54].d_key,"Alt");
	PX_strset(keyboard.Keys[54].u_key,"Alt");
	keyboard.Keys[54].x=xoffset;
	keyboard.Keys[54].y=yoffset;
	keyboard.Keys[54].width=(px_int)(kw*1.5);;
	keyboard.Keys[54].height=kh;
	xoffset+=keyboard.Keys[54].width;

	PX_strset(keyboard.Keys[55].d_key,"");
	PX_strset(keyboard.Keys[55].u_key,"");
	keyboard.Keys[55].x=xoffset;
	keyboard.Keys[55].y=yoffset;
	keyboard.Keys[55].width=(px_int)(kw*12);;
	keyboard.Keys[55].height=kh;
	xoffset+=keyboard.Keys[55].width;

	keyboard.cursorColor=PX_COLOR(255,192,192,192);
	keyboard.pushColor=PX_COLOR(255,128,128,128);
	keyboard.backgroundColor=PX_COLOR(255,255,255,255);
	keyboard.borderColor=PX_COLOR(255,0,0,0);

	keyboard.LinkerObject=PX_NULL;

	keyboardObject=PX_ObjectCreateEx(mp,Parent,(px_float)x,(px_float)y,0,(px_float)width,(px_float)height,0,PX_OBJECT_TYPE_VKEYBOARD,PX_NULL,PX_Object_VirtualKeyBoardRender,PX_NULL,&keyboard,sizeof(PX_Object_VirtualKeyBoard));

	PX_ObjectRegisterEvent(keyboardObject,PX_OBJECT_EVENT_CURSORMOVE,PX_Object_VirtualKeyBoardOnCursorMove,PX_NULL);
	PX_ObjectRegisterEvent(keyboardObject,PX_OBJECT_EVENT_CURSORDRAG,PX_Object_VirtualKeyBoardOnCursorMove,PX_NULL);
	PX_ObjectRegisterEvent(keyboardObject,PX_OBJECT_EVENT_CURSORDOWN,PX_Object_VirtualKeyBoardOnCursorDown,PX_NULL);
	PX_ObjectRegisterEvent(keyboardObject,PX_OBJECT_EVENT_CURSORUP,PX_Object_VirtualKeyBoardOnCursorUp,PX_NULL);

	keyboardObject->OnLostFocusReleaseEvent=PX_FALSE;

	return keyboardObject;
}

px_void PX_Object_VirtualKeyBoardSetBackgroundColor(PX_Object *pObject,px_color Color)
{
	PX_Object_VirtualKeyBoard *pkb=PX_Object_GetVirtualKeyBoard(pObject);
	if (pkb)
	{
		pkb->backgroundColor=Color;
	}
}

px_void PX_Object_VirtualKeyBoardSetBorderColor(PX_Object *pObject,px_color Color)
{
	PX_Object_VirtualKeyBoard *pkb=PX_Object_GetVirtualKeyBoard(pObject);
	if (pkb)
	{
		pkb->borderColor=Color;
	}
}

px_void PX_Object_VirtualKeyBoardSetCursorColor(PX_Object *pObject,px_color Color)
{
	PX_Object_VirtualKeyBoard *pkb=PX_Object_GetVirtualKeyBoard(pObject);
	if (pkb)
	{
		pkb->cursorColor=Color;
	}
}

px_void PX_Object_VirtualKeyBoardSetPushColor(PX_Object *pObject,px_color Color)
{
	PX_Object_VirtualKeyBoard *pkb=PX_Object_GetVirtualKeyBoard(pObject);
	if (pkb)
	{
		pkb->pushColor=Color;
	}
}

px_char PX_Object_VirtualKeyBoardGetCode(PX_Object *pObject)
{
	PX_Object_VirtualKeyBoard *pkb=PX_Object_GetVirtualKeyBoard(pObject);
	if (pkb)
	{
		px_char ch= pkb->functionCode;
		pkb->functionCode=0;
		return ch;
	}
	return 0;
}

px_void PX_Object_VirtualKeyBoardSetLinkerObject(PX_Object *pObject,PX_Object *linker)
{
	PX_Object_VirtualKeyBoard *pkb=PX_Object_GetVirtualKeyBoard(pObject);
	if (pkb)
	{
		pkb->LinkerObject=linker;
	}
}

//////////////////////////////////////////////////////////////////////////
//NumberKey
PX_Object_VirtualNumberKeyBoard *PX_Object_GetVirtualNumberKeyBoard(PX_Object *pObject)
{
	if (!pObject)
	{
		return PX_NULL;
	}
	if (pObject->Type==PX_OBJECT_TYPE_VNKEYBOARD)
	{
		return (PX_Object_VirtualNumberKeyBoard *)pObject->pObject;
	}
	return PX_NULL;
}

px_void PX_Object_VirtualNumberKeyBoardRender(px_surface *psurface, PX_Object *pObject,px_uint elapsed)
{
	px_int i;
	px_int oftx;
	px_int ofty;
	PX_Object_VirtualNumberKeyBoard *keyBoard=(PX_Object_VirtualNumberKeyBoard *)pObject->pObject;

	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;

	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;

	oftx=(px_int)objx;
	ofty=(px_int)objy;
	//clear background
	PX_SurfaceClear(psurface,(px_int)objx,(px_int)objy,(px_int)(objx+objWidth-1),(px_int)(objy+objHeight-1),keyBoard->backgroundColor);

	for (i=0;i<PX_COUNTOF(keyBoard->Keys);i++)
	{
		//draw rect

		if (keyBoard->Keys[i].bDown||keyBoard->Keys[i].bhold)
		{
			PX_GeoDrawRect(psurface,oftx+keyBoard->Keys[i].x,ofty+keyBoard->Keys[i].y,oftx+keyBoard->Keys[i].x+keyBoard->Keys[i].width,ofty+keyBoard->Keys[i].y+keyBoard->Keys[i].height,keyBoard->pushColor);
		}
		else if (keyBoard->Keys[i].bCursor)
		{
			PX_GeoDrawRect(psurface,oftx+keyBoard->Keys[i].x,ofty+keyBoard->Keys[i].y,oftx+keyBoard->Keys[i].x+keyBoard->Keys[i].width,ofty+keyBoard->Keys[i].y+keyBoard->Keys[i].height,keyBoard->cursorColor);
		}

		PX_GeoDrawBorder(psurface,oftx+keyBoard->Keys[i].x,ofty+keyBoard->Keys[i].y,oftx+keyBoard->Keys[i].x+keyBoard->Keys[i].width,ofty+keyBoard->Keys[i].y+keyBoard->Keys[i].height,1,keyBoard->borderColor);

		PX_FontModuleDrawText(psurface,PX_NULL,oftx+keyBoard->Keys[i].x+2,ofty+keyBoard->Keys[i].y+2,PX_ALIGN_LEFTTOP,keyBoard->Keys[i].d_key,keyBoard->borderColor);
	}

}

px_void PX_Object_VirtualNumberKeyBoardOnCursorUp(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	px_float x=PX_Object_Event_GetCursorX(e);
	px_float y=PX_Object_Event_GetCursorY(e);
	px_int i;
	PX_Object_VirtualNumberKeyBoard *pkb=(PX_Object_VirtualNumberKeyBoard *)pObject->pObject;
	if (!PX_ObjectIsPointInRegion(pObject,(px_float)x,(px_float)y))
	{
		return;
	}

	for (i=0;i<PX_COUNTOF(pkb->Keys);i++)
	{
		pkb->Keys[i].bDown=PX_FALSE;
	}
}

px_void PX_Object_VirtualNumberKeyBoardOnCursorMove(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	px_float x=PX_Object_Event_GetCursorX(e);
	px_float y=PX_Object_Event_GetCursorY(e);
	px_int i;
	px_int kh=(px_int)(pObject->Height/5);
	PX_Object_VirtualNumberKeyBoard *pkb=(PX_Object_VirtualNumberKeyBoard *)pObject->pObject;
	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;

	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;



	x=(PX_Object_Event_GetCursorX(e)-objx);
	y=(PX_Object_Event_GetCursorY(e)-objy);


	for (i=0;i<PX_COUNTOF(pkb->Keys);i++)
	{
		if(!pkb->Keys[i].bDown)
		{
			pkb->Keys[i].bCursor=PX_FALSE;
		}
		else
		{
			if (!PX_isPointInRect(PX_POINT((px_float)x,(px_float)y,0),PX_RECT((px_float)pkb->Keys[i].x,(px_float)pkb->Keys[i].y,(px_float)pkb->Keys[i].width,(px_float)pkb->Keys[i].height)))
				pkb->Keys[i].bDown=PX_FALSE;
		}
	}

	if (y<kh)
	{
		for (i=0;i<=3;i++)
		{
			if (PX_isPointInRect(PX_POINT((px_float)x,(px_float)y,0),PX_RECT((px_float)pkb->Keys[i].x,(px_float)pkb->Keys[i].y,(px_float)pkb->Keys[i].width,(px_float)pkb->Keys[i].height)))
			{
				pkb->Keys[i].bCursor=PX_TRUE;
				break;
			}
		}
	}
	else if (y<2*kh)
	{
		for (i=4;i<=7;i++)
		{
			if (PX_isPointInRect(PX_POINT((px_float)x,(px_float)y,0),PX_RECT((px_float)pkb->Keys[i].x,(px_float)pkb->Keys[i].y,(px_float)pkb->Keys[i].width,(px_float)pkb->Keys[i].height)))
			{
				pkb->Keys[i].bCursor=PX_TRUE;
				break;
			}
		}
	}
	else if (y<3*kh)
	{
		for (i=7;i<=10;i++)
		{
			if (PX_isPointInRect(PX_POINT((px_float)x,(px_float)y,0),PX_RECT((px_float)pkb->Keys[i].x,(px_float)pkb->Keys[i].y,(px_float)pkb->Keys[i].width,(px_float)pkb->Keys[i].height)))
			{
				pkb->Keys[i].bCursor=PX_TRUE;
				break;
			}
		}
	}
	else if (y<4*kh)
	{
		for (i=11;i<=14;i++)
		{
			if (PX_isPointInRect(PX_POINT((px_float)x,(px_float)y,0),PX_RECT((px_float)pkb->Keys[i].x,(px_float)pkb->Keys[i].y,(px_float)pkb->Keys[i].width,(px_float)pkb->Keys[i].height)))
			{
				pkb->Keys[i].bCursor=PX_TRUE;
				break;
			}
		}
	}
	else
	{
		for (i=14;i<=16;i++)
		{
			if (PX_isPointInRect(PX_POINT((px_float)x,(px_float)y,0),PX_RECT((px_float)pkb->Keys[i].x,(px_float)pkb->Keys[i].y,(px_float)pkb->Keys[i].width,(px_float)pkb->Keys[i].height)))
			{
				pkb->Keys[i].bCursor=PX_TRUE;
				break;
			}
		}
	}



}

px_void PX_Object_VirtualNumberKeyBoardOnCursorDown(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	px_float x=PX_Object_Event_GetCursorX(e);
	px_float y=PX_Object_Event_GetCursorY(e);
	px_int i;
	px_int kh=(px_int)(pObject->Height/5);
	PX_Object_Event stringEvent;
	PX_Object_VirtualKeyBoard *pkb=(PX_Object_VirtualKeyBoard *)pObject->pObject;
	PX_Object_Event ke;
	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;

	px_char strEve[2]={0};

	stringEvent.Event=PX_OBJECT_EVENT_STRING;
	PX_Object_Event_SetStringPtr(&stringEvent,strEve);


	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;

	if (!PX_ObjectIsPointInRegion(pObject,(px_float)x,(px_float)y))
	{
		return;
	}

	x=(PX_Object_Event_GetCursorX(e)-objx);
	y=(PX_Object_Event_GetCursorY(e)-objy);
	ke.Event=PX_OBJECT_EVENT_KEYDOWN;

	if (y<kh)
	{
		for (i=0;i<=3;i++)
		{
			if (PX_isPointInRect(PX_POINT((px_float)x,(px_float)y,0),PX_RECT((px_float)pkb->Keys[i].x,(px_float)pkb->Keys[i].y,(px_float)pkb->Keys[i].width,(px_float)pkb->Keys[i].height)))
			{
				pkb->Keys[i].bDown=PX_TRUE;
				if (i!=0)
				{
					
					PX_Object_Event_SetKeyDown(&ke,pkb->Keys[i].u_key[0]);
					PX_ObjectExecuteEvent(pObject,ke);
					strEve[0]=pkb->Keys[i].u_key[0];
					PX_ObjectPostEvent(pkb->LinkerObject,stringEvent);
				}
				else
				{
					PX_Object_Event_SetKeyDown(&ke,8);
					PX_ObjectExecuteEvent(pObject,ke);
					strEve[0]=8;
					PX_ObjectPostEvent(pkb->LinkerObject,stringEvent);
				}
				
				break;
			}
		}
	}
	else if (y<2*kh)
	{
		for (i=4;i<=7;i++)
		{
			if (PX_isPointInRect(PX_POINT((px_float)x,(px_float)y,0),PX_RECT((px_float)pkb->Keys[i].x,(px_float)pkb->Keys[i].y,(px_float)pkb->Keys[i].width,(px_float)pkb->Keys[i].height)))
			{
				PX_Object_Event_SetKeyDown(&ke,pkb->Keys[i].u_key[0]);
				PX_ObjectExecuteEvent(pObject,ke);
				strEve[0]=pkb->Keys[i].u_key[0];
				PX_ObjectPostEvent(pkb->LinkerObject,stringEvent);
				pkb->Keys[i].bDown=PX_TRUE;

				break;
			}
		}
	}
	else if (y<3*kh)
	{
		for (i=7;i<=10;i++)
		{
			if (PX_isPointInRect(PX_POINT((px_float)x,(px_float)y,0),PX_RECT((px_float)pkb->Keys[i].x,(px_float)pkb->Keys[i].y,(px_float)pkb->Keys[i].width,(px_float)pkb->Keys[i].height)))
			{
				PX_Object_Event_SetKeyDown(&ke,pkb->Keys[i].u_key[0]);
				PX_ObjectExecuteEvent(pObject,ke);
				strEve[0]=pkb->Keys[i].u_key[0];
				PX_ObjectPostEvent(pkb->LinkerObject,stringEvent);
				pkb->Keys[i].bDown=PX_TRUE;
				break;
			}
		}
	}
	else if (y<4*kh)
	{
		for (i=11;i<=14;i++)
		{
			if (PX_isPointInRect(PX_POINT((px_float)x,(px_float)y,0),PX_RECT((px_float)pkb->Keys[i].x,(px_float)pkb->Keys[i].y,(px_float)pkb->Keys[i].width,(px_float)pkb->Keys[i].height)))
			{
				if (i!=14)
				{
					PX_Object_Event_SetKeyDown(&ke,pkb->Keys[i].u_key[0]);
					PX_ObjectExecuteEvent(pObject,ke);
					strEve[0]=pkb->Keys[i].u_key[0];
					PX_ObjectPostEvent(pkb->LinkerObject,stringEvent);
				}
				else
				{
					PX_Object_Event_SetKeyDown(&ke,'\r');
					PX_ObjectExecuteEvent(pObject,ke);
					strEve[0]='\r';
					PX_ObjectPostEvent(pkb->LinkerObject,stringEvent);
				}
				pkb->Keys[i].bDown=PX_TRUE;
				break;
			}
		}
	}
	else
	{
		for (i=14;i<=16;i++)
		{
			if (PX_isPointInRect(PX_POINT((px_float)x,(px_float)y,0),PX_RECT((px_float)pkb->Keys[i].x,(px_float)pkb->Keys[i].y,(px_float)pkb->Keys[i].width,(px_float)pkb->Keys[i].height)))
			{
				if (i!=14)
				{
					PX_Object_Event_SetKeyDown(&ke,pkb->Keys[i].u_key[0]);
					PX_ObjectExecuteEvent(pObject,ke);
					strEve[0]=pkb->Keys[i].u_key[0];
					PX_ObjectPostEvent(pkb->LinkerObject,stringEvent);
				}
				else
				{
					PX_Object_Event_SetKeyDown(&ke,'\r');
					PX_ObjectExecuteEvent(pObject,ke);
					strEve[0]='\r';
					PX_ObjectPostEvent(pkb->LinkerObject,stringEvent);
				}
				pkb->Keys[i].bDown=PX_TRUE;
				break;
			}
		}
	}
}
PX_Object* PX_Object_VirtualNumberKeyBoardCreate(px_memorypool *mp, PX_Object *Parent,px_int x,px_int y,px_int width,px_int height)
{
	px_int i;
	px_int kw,kh;
	px_int xoffset,yoffset;
	PX_Object_VirtualNumberKeyBoard keyboard;
	px_char line1_lower[]=" /*-";
	px_char line2_lower[]="789+";
	px_char line3_lower[]="456";
	px_char line4_lower[]="123 ";
	px_char line5_lower[]="0.";

	PX_Object *keyboardObject;

	if (width<16*4||height<16*5)
	{
		return PX_NULL;
	}
	PX_memset(&keyboard,0,sizeof(PX_Object_VirtualNumberKeyBoard));

	kw=width/4;
	kh=height/5;

	xoffset=0;
	yoffset=0;

	for (i=0;i<=3;i++)
	{
		keyboard.Keys[i].d_key[0]=line1_lower[i];
		keyboard.Keys[i].u_key[0]=line1_lower[i];
		keyboard.Keys[i].x=xoffset;
		keyboard.Keys[i].y=yoffset;
		keyboard.Keys[i].width=kw;
		keyboard.Keys[i].height=kh;
		xoffset+=kw;
	}
	PX_strset(keyboard.Keys[0].d_key,"<--");
	PX_strset(keyboard.Keys[0].u_key,"<--");
	xoffset=0;
	yoffset=1*kh;

	for (i=4;i<=7;i++)
	{
		keyboard.Keys[i].d_key[0]=line2_lower[i-4];
		keyboard.Keys[i].u_key[0]=line2_lower[i-4];
		keyboard.Keys[i].x=xoffset;
		keyboard.Keys[i].y=yoffset;
		keyboard.Keys[i].width=kw;
		keyboard.Keys[i].height=kh;
		xoffset+=keyboard.Keys[i].width;
	}
	keyboard.Keys[7].height*=2;

	xoffset=0;
	yoffset=2*kh;
	for (i=8;i<=10;i++)
	{
		keyboard.Keys[i].d_key[0]=line3_lower[i-8];
		keyboard.Keys[i].u_key[0]=line3_lower[i-8];
		keyboard.Keys[i].x=xoffset;
		keyboard.Keys[i].y=yoffset;
		keyboard.Keys[i].width=kw;
		keyboard.Keys[i].height=kh;
		xoffset+=keyboard.Keys[i].width;
	}

	xoffset=0;
	yoffset=3*kh;
	for (i=11;i<=14;i++)
	{
		keyboard.Keys[i].d_key[0]=line4_lower[i-11];
		keyboard.Keys[i].u_key[0]=line4_lower[i-11];
		keyboard.Keys[i].x=xoffset;
		keyboard.Keys[i].y=yoffset;
		keyboard.Keys[i].width=kw;
		keyboard.Keys[i].height=kh;
		xoffset+=keyboard.Keys[i].width;
	}

	
	PX_strset(keyboard.Keys[14].d_key,"Enter");
	PX_strset(keyboard.Keys[14].u_key,"Enter");
	keyboard.Keys[14].height*=2;

	xoffset=0;
	yoffset=4*kh;
	
	keyboard.Keys[15].d_key[0]='0';
	keyboard.Keys[15].u_key[0]='0';
	keyboard.Keys[15].x=xoffset;
	keyboard.Keys[15].y=yoffset;
	keyboard.Keys[15].width=kw*2;
	keyboard.Keys[15].height=kh;
	xoffset+=keyboard.Keys[15].width;


	keyboard.Keys[16].d_key[0]='.';
	keyboard.Keys[16].u_key[0]='.';
	keyboard.Keys[16].x=xoffset;
	keyboard.Keys[16].y=yoffset;
	keyboard.Keys[16].width=kw;
	keyboard.Keys[16].height=kh;
	xoffset+=keyboard.Keys[16].width;

	keyboard.cursorColor=PX_COLOR(255,192,192,192);
	keyboard.pushColor=PX_COLOR(255,128,128,128);
	keyboard.backgroundColor=PX_COLOR(255,255,255,255);
	keyboard.borderColor=PX_COLOR(255,0,0,0);
	keyboard.LinkerObject=PX_NULL;
	keyboardObject=PX_ObjectCreateEx(mp,Parent,(px_float)x,(px_float)y,0,(px_float)width,(px_float)height,0,PX_OBJECT_TYPE_VNKEYBOARD,PX_NULL,PX_Object_VirtualNumberKeyBoardRender,PX_NULL,&keyboard,sizeof(PX_Object_VirtualNumberKeyBoard));

	PX_ObjectRegisterEvent(keyboardObject,PX_OBJECT_EVENT_CURSORMOVE,PX_Object_VirtualNumberKeyBoardOnCursorMove,PX_NULL);
	PX_ObjectRegisterEvent(keyboardObject,PX_OBJECT_EVENT_CURSORDRAG,PX_Object_VirtualNumberKeyBoardOnCursorMove,PX_NULL);
	PX_ObjectRegisterEvent(keyboardObject,PX_OBJECT_EVENT_CURSORDOWN,PX_Object_VirtualNumberKeyBoardOnCursorDown,PX_NULL);
	PX_ObjectRegisterEvent(keyboardObject,PX_OBJECT_EVENT_CURSORUP,PX_Object_VirtualNumberKeyBoardOnCursorUp,PX_NULL);
	return keyboardObject;
}

px_void PX_Object_VirtualNumberKeyBoardSetBackgroundColor(PX_Object *pObject,px_color Color)
{
	PX_Object_VirtualNumberKeyBoard *pkb=PX_Object_GetVirtualNumberKeyBoard(pObject);
	if (pkb)
	{
		pkb->backgroundColor=Color;
	}
}

px_void PX_Object_VirtualNumberKeyBoardSetBorderColor(PX_Object *pObject,px_color Color)
{
	PX_Object_VirtualNumberKeyBoard *pkb=PX_Object_GetVirtualNumberKeyBoard(pObject);
	if (pkb)
	{
		pkb->borderColor=Color;
	}
}

px_void PX_Object_VirtualNumberKeyBoardSetCursorColor(PX_Object *pObject,px_color Color)
{
	PX_Object_VirtualNumberKeyBoard *pkb=PX_Object_GetVirtualNumberKeyBoard(pObject);
	if (pkb)
	{
		pkb->cursorColor=Color;
	}
}

px_void PX_Object_VirtualNumberKeyBoardSetPushColor(PX_Object *pObject,px_color Color)
{
	PX_Object_VirtualNumberKeyBoard *pkb=PX_Object_GetVirtualNumberKeyBoard(pObject);
	if (pkb)
	{
		pkb->pushColor=Color;
	}
}

px_char PX_Object_VirtualNumberKeyBoardGetCode(PX_Object *pObject)
{
	PX_Object_VirtualNumberKeyBoard *pkb=PX_Object_GetVirtualNumberKeyBoard(pObject);
	if (pkb)
	{
		px_char ch= pkb->functionCode;
		pkb->functionCode=0;
		return ch;
	}
	return 0;
}

px_void PX_Object_VirtualNumberKeyBoardSetLinkerObject(PX_Object *pObject,PX_Object *linker)
{
	PX_Object_VirtualNumberKeyBoard *pkb=PX_Object_GetVirtualNumberKeyBoard(pObject);
	if (pkb)
	{
		pkb->LinkerObject=linker;
	}
}
