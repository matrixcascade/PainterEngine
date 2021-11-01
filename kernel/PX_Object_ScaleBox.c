#include "PX_Object_ScaleBox.h"


px_void PX_Object_ScaleBoxRender(px_surface *psurface, PX_Object *pObject,px_uint elapsed)
{

	PX_Object_ScaleBox * pScaleBox=PX_Object_GetScaleBox(pObject);
	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;

	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;

	switch (pScaleBox->editpt_count)
	{
	case 0:
	default:
		return;
	case 1:
		PX_GeoDrawLine(psurface,(px_int)(objx+pScaleBox->edit_pt[0].x),(px_int)(objx+pScaleBox->edit_pt[0].y),\
			(px_int)(objx+pScaleBox->cursor_pt.x),(px_int)(objx+pScaleBox->cursor_pt.y),
			1,
			(pScaleBox->linecolor)
			);
		PX_GeoDrawSolidCircle(psurface,(px_int)(objx+pScaleBox->edit_pt[0].x),(px_int)(objx+pScaleBox->edit_pt[0].y),3,pScaleBox->linecolor);
		PX_GeoDrawSolidCircle(psurface,(px_int)(objx+pScaleBox->cursor_pt.x),(px_int)(objx+pScaleBox->cursor_pt.y),3,pScaleBox->linecolor);
		break;
	case 2:
		{
			px_point2D pt[4];

			PX_memcpy(pt,pScaleBox->edit_pt,2*sizeof(px_point2D));
			pt[2]=pScaleBox->cursor_pt;
			pt[3]=PX_Point2DAdd(pt[0],PX_Point2DSub(pt[2],pt[1]));

			PX_GeoDrawTriangle(psurface,pt[0],pt[1],pt[2],pScaleBox->regioncolor);
			PX_GeoDrawTriangle(psurface,pt[0],pt[2],pt[3],pScaleBox->regioncolor);

			PX_GeoDrawLine(psurface,(px_int)(objx+pt[0].x),(px_int)(objx+pt[0].y),\
				(px_int)(objx+pt[1].x),(px_int)(objx+pt[1].y),
				1,
				(pScaleBox->linecolor)
				);

			PX_GeoDrawLine(psurface,(px_int)(objx+pt[1].x),(px_int)(objx+pt[1].y),\
				(px_int)(objx+pt[2].x),(px_int)(objx+pt[2].y),
				1,
				(pScaleBox->linecolor)
				);

			PX_GeoDrawLine(psurface,(px_int)(objx+pt[2].x),(px_int)(objx+pt[2].y),\
				(px_int)(objx+pt[3].x),(px_int)(objx+pt[3].y),
				1,
				(pScaleBox->linecolor)
				);

			PX_GeoDrawLine(psurface,(px_int)(objx+pt[0].x),(px_int)(objx+pt[0].y),\
				(px_int)(objx+pt[3].x),(px_int)(objx+pt[3].y),
				1,
				(pScaleBox->linecolor)
				);

			PX_GeoDrawSolidCircle(psurface,(px_int)(objx+pt[0].x),(px_int)(objx+pt[0].y),3,pScaleBox->linecolor);
			PX_GeoDrawSolidCircle(psurface,(px_int)(objx+pt[1].x),(px_int)(objx+pt[1].y),3,pScaleBox->linecolor);
			PX_GeoDrawSolidCircle(psurface,(px_int)(objx+pt[2].x),(px_int)(objx+pt[2].y),3,pScaleBox->linecolor);
			PX_GeoDrawSolidCircle(psurface,(px_int)(objx+pt[3].x),(px_int)(objx+pt[3].y),3,pScaleBox->linecolor);
		}
		break;
	case 3:
		{
			px_point2D mpt;

			PX_GeoDrawTriangle(psurface,pScaleBox->edit_pt[0],pScaleBox->edit_pt[1],pScaleBox->edit_pt[2],pScaleBox->regioncolor);
			PX_GeoDrawTriangle(psurface,pScaleBox->edit_pt[0],pScaleBox->edit_pt[2],pScaleBox->edit_pt[3],pScaleBox->regioncolor);

			PX_GeoDrawTriangle(psurface,pScaleBox->scale_pt[0],pScaleBox->scale_pt[1],pScaleBox->scale_pt[2],pScaleBox->regioncolor);
			PX_GeoDrawTriangle(psurface,pScaleBox->scale_pt[0],pScaleBox->scale_pt[2],pScaleBox->scale_pt[3],pScaleBox->regioncolor);


			PX_GeoDrawLine(psurface,(px_int)(objx+pScaleBox->edit_pt[0].x),(px_int)(objx+pScaleBox->edit_pt[0].y),\
				(px_int)(objx+pScaleBox->edit_pt[1].x),(px_int)(objx+pScaleBox->edit_pt[1].y),
				1,
				(pScaleBox->linecolor)
				);
			PX_GeoDrawLine(psurface,(px_int)(objx+pScaleBox->scale_pt[0].x),(px_int)(objx+pScaleBox->scale_pt[0].y),\
				(px_int)(objx+pScaleBox->scale_pt[1].x),(px_int)(objx+pScaleBox->scale_pt[1].y),
				1,
				(pScaleBox->linecolor)
				);


			PX_GeoDrawLine(psurface,(px_int)(objx+pScaleBox->edit_pt[1].x),(px_int)(objx+pScaleBox->edit_pt[1].y),\
				(px_int)(objx+pScaleBox->edit_pt[2].x),(px_int)(objx+pScaleBox->edit_pt[2].y),
				1,
				(pScaleBox->linecolor)
				);

			PX_GeoDrawLine(psurface,(px_int)(objx+pScaleBox->scale_pt[1].x),(px_int)(objx+pScaleBox->scale_pt[1].y),\
				(px_int)(objx+pScaleBox->scale_pt[2].x),(px_int)(objx+pScaleBox->scale_pt[2].y),
				1,
				(pScaleBox->linecolor)
				);

			PX_GeoDrawLine(psurface,(px_int)(objx+pScaleBox->edit_pt[2].x),(px_int)(objx+pScaleBox->edit_pt[2].y),\
				(px_int)(objx+pScaleBox->edit_pt[3].x),(px_int)(objx+pScaleBox->edit_pt[3].y),
				1,
				(pScaleBox->linecolor)
				);
			PX_GeoDrawLine(psurface,(px_int)(objx+pScaleBox->scale_pt[2].x),(px_int)(objx+pScaleBox->scale_pt[2].y),\
				(px_int)(objx+pScaleBox->scale_pt[3].x),(px_int)(objx+pScaleBox->scale_pt[3].y),
				1,
				(pScaleBox->linecolor)
				);

			PX_GeoDrawLine(psurface,(px_int)(objx+pScaleBox->edit_pt[0].x),(px_int)(objx+pScaleBox->edit_pt[0].y),\
				(px_int)(objx+pScaleBox->edit_pt[3].x),(px_int)(objx+pScaleBox->edit_pt[3].y),
				1,
				(pScaleBox->linecolor)
				);
			PX_GeoDrawLine(psurface,(px_int)(objx+pScaleBox->scale_pt[0].x),(px_int)(objx+pScaleBox->scale_pt[0].y),\
				(px_int)(objx+pScaleBox->scale_pt[3].x),(px_int)(objx+pScaleBox->scale_pt[3].y),
				1,
				(pScaleBox->linecolor)
				);

			mpt.x=(pScaleBox->scale_pt[0].x+pScaleBox->scale_pt[1].x)/2;
			mpt.y=(pScaleBox->scale_pt[0].y+pScaleBox->scale_pt[1].y)/2;
			PX_GeoDrawSolidCircle(psurface,(px_int)(objx+mpt.x),(px_int)(objx+mpt.y),3,pScaleBox->linecolor);
			if (pScaleBox->current_cursorpt==0||pScaleBox->current_selectpt==0)
			{
				PX_GeoDrawCircle(psurface,(px_int)(objx+mpt.x),(px_int)(objx+mpt.y),5,1,pScaleBox->linecolor);
			}

			mpt.x=(pScaleBox->scale_pt[2].x+pScaleBox->scale_pt[1].x)/2;
			mpt.y=(pScaleBox->scale_pt[2].y+pScaleBox->scale_pt[1].y)/2;
			PX_GeoDrawSolidCircle(psurface,(px_int)(objx+mpt.x),(px_int)(objx+mpt.y),3,pScaleBox->linecolor);
			if (pScaleBox->current_cursorpt==1||pScaleBox->current_selectpt==1)
			{
				PX_GeoDrawCircle(psurface,(px_int)(objx+mpt.x),(px_int)(objx+mpt.y),5,1,pScaleBox->linecolor);
			}

			mpt.x=(pScaleBox->scale_pt[2].x+pScaleBox->scale_pt[3].x)/2;
			mpt.y=(pScaleBox->scale_pt[2].y+pScaleBox->scale_pt[3].y)/2;
			PX_GeoDrawSolidCircle(psurface,(px_int)(objx+mpt.x),(px_int)(objx+mpt.y),3,pScaleBox->linecolor);
			if (pScaleBox->current_cursorpt==2||pScaleBox->current_selectpt==2)
			{
				PX_GeoDrawCircle(psurface,(px_int)(objx+mpt.x),(px_int)(objx+mpt.y),5,1,pScaleBox->linecolor);
			}

			mpt.x=(pScaleBox->scale_pt[0].x+pScaleBox->scale_pt[3].x)/2;
			mpt.y=(pScaleBox->scale_pt[0].y+pScaleBox->scale_pt[3].y)/2;
			PX_GeoDrawSolidCircle(psurface,(px_int)(objx+mpt.x),(px_int)(objx+mpt.y),3,pScaleBox->linecolor);
			if (pScaleBox->current_cursorpt==3||pScaleBox->current_selectpt==3)
			{
				PX_GeoDrawCircle(psurface,(px_int)(objx+mpt.x),(px_int)(objx+mpt.y),5,1,pScaleBox->linecolor);
			}

		}
		break;
	}
}

px_void PX_Object_ScaleBoxOnCursorDown(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Object_ScaleBox *pscalebox=PX_Object_GetScaleBox(pObject);
	px_float inheritX,inheritY;
	px_float objx,objy,x,y;
	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);
	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	x=PX_Object_Event_GetCursorX(e);
	y=PX_Object_Event_GetCursorY(e);
	if (pscalebox->editpt_count==3)
	{
		px_int i;
		px_point2D select_pt[4];//01,12,23,30

		select_pt[0].x=(pscalebox->scale_pt[0].x+pscalebox->scale_pt[1].x)/2;
		select_pt[0].y=(pscalebox->scale_pt[0].y+pscalebox->scale_pt[1].y)/2;

		select_pt[1].x=(pscalebox->scale_pt[2].x+pscalebox->scale_pt[1].x)/2;
		select_pt[1].y=(pscalebox->scale_pt[2].y+pscalebox->scale_pt[1].y)/2;

		select_pt[2].x=(pscalebox->scale_pt[2].x+pscalebox->scale_pt[3].x)/2;
		select_pt[2].y=(pscalebox->scale_pt[2].y+pscalebox->scale_pt[3].y)/2;

		select_pt[3].x=(pscalebox->scale_pt[0].x+pscalebox->scale_pt[3].x)/2;
		select_pt[3].y=(pscalebox->scale_pt[0].y+pscalebox->scale_pt[3].y)/2;

		pscalebox->current_selectpt=-1;
		for (i=0;i<4;i++)
		{
			if (PX_isPoint2DInCircle(PX_POINT2D(x,y),select_pt[i],5))
			{
				pscalebox->current_selectpt=i;
				break;
			}
		}
	}
	else
	{
		pscalebox->edit_pt[pscalebox->editpt_count]=PX_POINT2D(PX_Object_Event_GetCursorX(e)-inheritX,PX_Object_Event_GetCursorY(e)-inheritY);
		pscalebox->editpt_count++;
		if (pscalebox->editpt_count==3)
		{
			PX_memcpy(pscalebox->scale_pt,pscalebox->edit_pt,3*sizeof(px_point2D));
			pscalebox->edit_pt[3]=PX_Point2DAdd(pscalebox->edit_pt[0],PX_Point2DSub(pscalebox->edit_pt[2],pscalebox->edit_pt[1]));
			pscalebox->scale_pt[3]=pscalebox->edit_pt[3];
		}

	}
}
px_void PX_Object_ScaleBoxOnCursorDrag(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Object_ScaleBox *pscalebox=PX_Object_GetScaleBox(pObject);
	px_float inheritX,inheritY;
	px_float objx,objy,x,y,d;
	px_point2D base;

	px_point2D p0,p1,p2,v0,v1,v2;

	if (pscalebox->current_selectpt==-1)
	{
		return;
	}
	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);
	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	x=PX_Object_Event_GetCursorX(e)-objx;
	y=PX_Object_Event_GetCursorY(e)-objy;

	switch(pscalebox->current_cursorpt)
	{
	case 0:
		{
			p0=pscalebox->scale_pt[2];
			p1=pscalebox->scale_pt[1];
			p2=pscalebox->scale_pt[3];
		}
		break;
	case 1:
		{
			p0=pscalebox->scale_pt[0];
			p1=pscalebox->scale_pt[1];
			p2=pscalebox->scale_pt[3];
		}
		break;
	case 2:
		{
			p0=pscalebox->scale_pt[1];
			p1=pscalebox->scale_pt[2];
			p2=pscalebox->scale_pt[0];
		}
		break;
	case 3:
		{
			p0=pscalebox->scale_pt[1];
			p1=pscalebox->scale_pt[0];
			p2=pscalebox->scale_pt[2];
		}
		break;
	default:
		return;
	}
	v0=PX_Point2DNormalization(PX_Point2DSub(p1,p0)) ;
	v1=PX_Point2DNormalization(PX_Point2DSub(p2,p0));
	v2=(PX_Point2DSub(PX_POINT2D(x,y),p0));

	base=PX_Point2DBase(v0,v1,v2);
	d=base.x;
	switch(pscalebox->current_cursorpt)
	{
	case 0:
		{
			pscalebox->scale_pt[1]=PX_Point2DAdd(p0,PX_Point2DMul((v0),d));
			pscalebox->scale_pt[0]=PX_Point2DAdd(p2,PX_Point2DMul((v0),d));
		}
		break;
	case 1:
		{
			pscalebox->scale_pt[1]=PX_Point2DAdd(p0,PX_Point2DMul((v0),d));
			pscalebox->scale_pt[2]=PX_Point2DAdd(p2,PX_Point2DMul((v0),d));
		}
		break;
	case 2:
		{
			pscalebox->scale_pt[2]=PX_Point2DAdd(p0,PX_Point2DMul((v0),d));
			pscalebox->scale_pt[3]=PX_Point2DAdd(p2,PX_Point2DMul((v0),d));
		}
		break;
	case 3:
		{
			pscalebox->scale_pt[0]=PX_Point2DAdd(p0,PX_Point2DMul((v0),d));
			pscalebox->scale_pt[3]=PX_Point2DAdd(p2,PX_Point2DMul((v0),d));
		}
		break;
	}
}

px_void PX_Object_ScaleBoxOnCursorMove(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Object_ScaleBox *pscalebox=PX_Object_GetScaleBox(pObject);
	px_float inheritX,inheritY;
	px_float objx,objy,x,y;
	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);
	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	x=PX_Object_Event_GetCursorX(e);
	y=PX_Object_Event_GetCursorY(e);
	if (pscalebox->editpt_count==3)
	{
		px_int i;
		px_point2D select_pt[4];//01,12,23,30

		select_pt[0].x=(pscalebox->scale_pt[0].x+pscalebox->scale_pt[1].x)/2;
		select_pt[0].y=(pscalebox->scale_pt[0].y+pscalebox->scale_pt[1].y)/2;

		select_pt[1].x=(pscalebox->scale_pt[2].x+pscalebox->scale_pt[1].x)/2;
		select_pt[1].y=(pscalebox->scale_pt[2].y+pscalebox->scale_pt[1].y)/2;

		select_pt[2].x=(pscalebox->scale_pt[2].x+pscalebox->scale_pt[3].x)/2;
		select_pt[2].y=(pscalebox->scale_pt[2].y+pscalebox->scale_pt[3].y)/2;

		select_pt[3].x=(pscalebox->scale_pt[0].x+pscalebox->scale_pt[3].x)/2;
		select_pt[3].y=(pscalebox->scale_pt[0].y+pscalebox->scale_pt[3].y)/2;

		pscalebox->current_cursorpt=-1;
		for (i=0;i<4;i++)
		{
			if (PX_isPoint2DInCircle(PX_POINT2D(x,y),select_pt[i],5))
			{
				pscalebox->current_cursorpt=i;
				break;
			}
		}

	}
	else
	{
		pscalebox->cursor_pt=PX_POINT2D(PX_Object_Event_GetCursorX(e)-inheritX,PX_Object_Event_GetCursorY(e)-inheritY);
	}
}
PX_Object * PX_Object_ScaleBoxCreate(px_memorypool *mp,PX_Object *Parent)
{
	PX_Object_ScaleBox scaleBox;
	PX_Object *pObject;
	PX_memset(&scaleBox,0,sizeof(scaleBox));
	scaleBox.linecolor=PX_COLOR(204,0,0,255);
	scaleBox.regioncolor=PX_COLOR(64,0,0,255);
	scaleBox.current_cursorpt=-1;
	scaleBox.current_selectpt=-1;
	pObject=PX_ObjectCreateEx(mp,Parent,0,0,0,0,0,0,PX_OBJECT_TYPE_SCALEBOX,PX_NULL,PX_Object_ScaleBoxRender,PX_NULL,&scaleBox,sizeof(scaleBox));
	if (!pObject)
	{
		return pObject;
	}
	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_CURSORDOWN,PX_Object_ScaleBoxOnCursorDown,PX_NULL);
	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_CURSORMOVE,PX_Object_ScaleBoxOnCursorMove,PX_NULL);
	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_CURSORDRAG,PX_Object_ScaleBoxOnCursorDrag,PX_NULL);
	return pObject;
}

PX_Object_ScaleBox * PX_Object_GetScaleBox(PX_Object *Object)
{
	if (Object->Type==PX_OBJECT_TYPE_SCALEBOX)
	{
		return (PX_Object_ScaleBox *)Object->pObject;
	}
	return PX_NULL;
}

px_point2D * PX_Object_ScaleBoxGetPoints(PX_Object *Object)
{
	PX_Object_ScaleBox *pscalebox=PX_Object_GetScaleBox(Object);
	if (pscalebox)
	{
		return pscalebox->edit_pt;
	}
	return PX_NULL;
}

