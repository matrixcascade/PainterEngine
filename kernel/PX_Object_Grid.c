#include "PX_Object_Grid.h"

PX_OBJECT_EVENT_FUNCTION(PX_Object_GridOnChanged)
{
	PX_Object_Event ne = { 0 };
	PX_Object* pGridObject = (PX_Object*)ptr;
	PX_Object_Event_SetDataPtr(&ne, pObject);
	ne.Event = PX_OBJECT_EVENT_VALUECHANGED;
	PX_ObjectExecuteEvent(pGridObject, ne);
}


PX_OBJECT_RENDER_FUNCTION(PX_Object_GridRender)
{
	//draw border
	px_rect rect=PX_ObjectGetRect(pObject);
	PX_Object_grid* pGrid = PX_ObjectGetDesc(PX_Object_grid, pObject);
	px_int i, j;
	for (i = 0; i < pGrid->xcount; i++)
	{
		for (j = 0; j < pGrid->ycount; j++)
		{
			PX_Object_GridCell* pCell = &pGrid->cell[j*pGrid->xcount + i];
			if (pCell->startx==i&&pCell->starty==j)
			{
				PX_GeoDrawBorder(psurface, (px_int)(rect.x + i*pGrid->xsize), (px_int)(rect.y + j*pGrid->ysize),\
					(px_int)(rect.x + (i + pCell->xsize)*pGrid->xsize),\
					(px_int)(rect.y + (j + pCell->ysize)*pGrid->ysize),\
					1,\
					pGrid->bordercolor);
			}
		}
	}
}

PX_OBJECT_FREE_FUNCTION(PX_Object_GridFree)
{
	PX_Object_grid* pGrid = PX_ObjectGetDesc(PX_Object_grid, pObject);
	MP_Free(pGrid->mp,pGrid->cell);
	PX_VectorFree(&pGrid->objects);
}

PX_Object* PX_Object_GridCreate(px_memorypool* mp, PX_Object* Parent, px_int x, px_int y, px_int xcount, px_int ycount, px_float xsize, px_float ysize)
{
	px_int i;
	PX_Object* pObject;
	PX_Object_grid* pGrid;
	pObject = PX_ObjectCreateEx(mp, Parent, (px_float)x, (px_float)y, 0, (px_float)xcount*xsize, (px_float)ycount*ysize, 0, PX_OBJECT_TYPE_GRID, 0, PX_Object_GridRender, PX_Object_GridFree, 0, sizeof(PX_Object_grid));
	if (pObject == PX_NULL)
	{
		return PX_NULL;
	}
	pGrid = PX_ObjectGetDescIndex(PX_Object_grid, pObject,0);
	pGrid->mp = mp;
	pGrid->xcount = xcount;
	pGrid->ycount = ycount;
	pGrid->xsize = xsize;
	pGrid->ysize = ysize;
	pGrid->bordercolor = PX_COLOR_BORDERCOLOR;
	pGrid->cell = (PX_Object_GridCell *)MP_Malloc(mp, sizeof(PX_Object_GridCell)*xcount*ycount);
	if (pGrid->cell == PX_NULL)
	{
		PX_ObjectDelete(pObject);
		return PX_NULL;
	}
	for (i = 0; i < xcount; i++)
	{
		for (px_int j = 0; j < ycount; j++)
		{
			pGrid->cell[j*xcount + i].object = PX_NULL;
			pGrid->cell[j*xcount + i].startx = i;
			pGrid->cell[j*xcount + i].starty = j;
			pGrid->cell[j*xcount + i].xsize = 1;
			pGrid->cell[j*xcount + i].ysize = 1;
		}
	}
	PX_VectorInitialize(mp, &pGrid->objects, sizeof(PX_Object*), 4);
	return pObject;
}

px_void PX_Object_GridClear(PX_Object* pObject)
{
	PX_Object_grid* pGrid = PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_GRID);
	px_int i;
	for (i = 0; i < pGrid->objects.size; i++)
	{
		PX_Object* pObject = *PX_VECTORAT(PX_Object * ,&pGrid->objects, i);
		if (pObject)
		{
			PX_ObjectDelete(pObject);
		}
	}
	PX_VectorClear(&pGrid->objects);
	for (i = 0; i < pGrid->xcount*pGrid->ycount; i++)
	{
		pGrid->cell[i].object = PX_NULL;
	}
	
}

px_void PX_Object_GridClearRange(PX_Object* pObject, px_int x, px_int y, px_int xcount, px_int ycount)
{
	PX_Object_grid* pGrid = PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_GRID);
	px_int i, j;
	for (i = 0; i < xcount; i++)
	{
		for (j = 0; j < ycount; j++)
		{
			if (x + i >= 0 && x + i < pGrid->xcount&&y + j >= 0 && y + j < pGrid->ycount)
			{
				PX_Object* pObject = pGrid->cell[(y + j) * pGrid->xcount + x + i].object;
				if (pObject)
				{
					//clear range & delete object once
					px_int startx= pGrid->cell[(y + j) * pGrid->xcount + x + i].startx;
					px_int starty= pGrid->cell[(y + j) * pGrid->xcount + x + i].starty;
					px_int xsize= pGrid->cell[(y + j) * pGrid->xcount + x + i].xsize;
					px_int ysize= pGrid->cell[(y + j) * pGrid->xcount + x + i].ysize;
					px_int k, l;
					for (k = 0; k < xsize; k++)
					{
						for (l = 0; l < ysize; l++)
						{
							pGrid->cell[(starty + l) * pGrid->xcount + startx + k].object = PX_NULL;
						}
					}
					PX_ObjectDelete(pObject);
				}
			}
		}
	}

	
}

px_void PX_Object_GridVisibleRange(PX_Object* pObject, px_int x, px_int y, px_int xcount, px_int ycount,px_bool visible)
{
	PX_Object_grid* pGrid = PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_GRID);
	px_int i, j;
	for (i = 0; i < xcount; i++)
	{
		for (j = 0; j < ycount; j++)
		{
			if (x + i >= 0 && x + i < pGrid->xcount&&y + j >= 0 && y + j < pGrid->ycount)
			{
				PX_Object* pObject = pGrid->cell[(y + j) * pGrid->xcount + x + i].object;
				if (pObject)
				{
					PX_ObjectSetVisible(pObject, visible);
				}
			}
		}
	}
	
}

PX_Object * PX_Object_GridGetCellObject(PX_Object * pObject, px_int x, px_int y)
{
	PX_Object_grid* pGrid = (PX_Object_grid*)PX_ObjectGetDescByType( pObject,PX_OBJECT_TYPE_GRID);
	if (x >= 0 && x < pGrid->xcount&&y >= 0 && y < pGrid->ycount)
	{
		return pGrid->cell[y*pGrid->xcount + x].object;
	}
	PX_ASSERT();
	return PX_NULL;
}

px_bool PX_Object_GridSetCellObject(PX_Object * pObject, px_int x, px_int y,px_int xcount,px_int ycount, PX_Object * pCellObject)
{
	PX_Object_grid* pGrid = (PX_Object_grid*)PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_GRID);
	px_int i, j;
	for (i = 0; i < xcount; i++)
	{
		for (j = 0; j < ycount; j++)
		{
			if (x + i >= 0 && x + i < pGrid->xcount&&y + j >= 0 && y + j < pGrid->ycount)
			{
				if (pGrid->cell[(y + j) * pGrid->xcount + x + i].object==PX_NULL)
				{
					pGrid->cell[(y + j) * pGrid->xcount + x + i].object = pCellObject;
					pGrid->cell[(y + j) * pGrid->xcount + x + i].startx = x;
					pGrid->cell[(y + j) * pGrid->xcount + x + i].starty = y;
					pGrid->cell[(y + j) * pGrid->xcount + x + i].xsize = xcount;
					pGrid->cell[(y + j) * pGrid->xcount + x + i].ysize = ycount;
				}
				else
				{
					//restore
					for (i = 0; i < xcount; i++)
					{
						for (j = 0; j < ycount; j++)
						{
							pGrid->cell[(y + j) * pGrid->xcount + x + i].object = PX_NULL;
						}
					}
					return PX_FALSE;
				}
			}
			else
			{
				PX_ASSERT();
				return PX_FALSE;
			}
		}
	}
	PX_VectorPushback(&pGrid->objects, &pCellObject);
	return PX_TRUE;
}

px_bool PX_Object_GridCellIsEmpty(PX_Object* pObject, px_int x, px_int y, px_int xcount, px_int ycount)
{
	PX_Object_grid* pGrid = (PX_Object_grid*)PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_GRID);
	px_int i, j;
	for (i = 0; i < xcount; i++)
	{
		for (j = 0; j < ycount; j++)
		{
			if (x + i >= 0 && x + i < pGrid->xcount && y + j >= 0 && y + j < pGrid->ycount)
			{
				if (pGrid->cell[(y + j) * pGrid->xcount + x + i].object != PX_NULL)
				{
					return PX_FALSE;
				}
			}
			else
			{
				PX_ASSERT();
				return PX_FALSE;
			}
		}
	}
	return PX_TRUE;
}

PX_Object* PX_Object_GridSetLabel(PX_Object* pObject, px_int x, px_int y, px_int xsize, px_int ysize, const px_char label[], PX_FontModule* fontmodule)
{
	PX_Object_grid* pGrid = (PX_Object_grid*)PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_GRID);
	if (PX_Object_GridCellIsEmpty(pObject, x, y, xsize, ysize))
	{
		PX_Object* pLabel = PX_Object_LabelCreate(pGrid->mp,pObject,(px_int)(x*pGrid->xsize), (px_int)(y*pGrid->ysize), (px_int)(xsize*pGrid->xsize), (px_int)(ysize*pGrid->ysize), label, fontmodule,PX_COLOR_FONTCOLOR);
		if (pLabel)
		{
			PX_Object_LabelSetAlign(pLabel, PX_ALIGN_CENTER);
			PX_Object_GridSetCellObject(pObject, x, y, xsize, ysize, pLabel);
			return pLabel;
		}
		return PX_NULL;
	}
	else
	{
		PX_ASSERT();
	}
	return PX_NULL;
}
PX_Object* PX_Object_GridSetSelectBar(PX_Object* pObject, px_int x, px_int y, px_int xsize, px_int ysize, PX_FontModule* fontmodule)
{
	PX_Object_grid* pGrid = (PX_Object_grid*)PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_GRID);
	if (PX_Object_GridCellIsEmpty(pObject, x, y, xsize, ysize))
	{
		PX_Object* pSelectBar = PX_Object_SelectBarCreate(pGrid->mp,pObject, (px_int)(x*pGrid->xsize), (px_int)(y*pGrid->ysize), (px_int)(xsize*pGrid->xsize), (px_int)(ysize*pGrid->ysize), fontmodule);
		if (pSelectBar)
		{
			PX_Object_GridSetCellObject(pObject, x, y, xsize, ysize, pSelectBar);
			PX_ObjectRegisterEvent(pSelectBar, PX_OBJECT_EVENT_VALUECHANGED, PX_Object_GridOnChanged, pObject);
			return pSelectBar;
		}
		return PX_NULL;
	}
	return PX_NULL;
}

PX_Object* PX_Object_GridGetObject(PX_Object* pObject, px_int x, px_int y)
{
	PX_Object_grid* pGrid = (PX_Object_grid*)PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_GRID);
	if (x >= 0 && x < pGrid->xcount&&y >= 0 && y < pGrid->ycount)
	{
		return pGrid->cell[y*pGrid->xcount + x].object;
	}
	return PX_NULL;
}

const px_char* PX_Object_GridGetText(PX_Object* pObject, px_int x, px_int y)
{
	PX_Object* pGridObject = PX_Object_GridGetObject(pObject, x, y);
	if (pGridObject)
	{
		if (PX_ObjectCheckType(pGridObject, PX_OBJECT_TYPE_LABEL))
		{
			return PX_Object_LabelGetText(pGridObject);
		}
		else if (PX_ObjectCheckType(pGridObject, PX_OBJECT_TYPE_EDIT))
		{
			return PX_Object_EditGetText(pGridObject);
		}
		else if (PX_ObjectCheckType(pGridObject, PX_OBJECT_TYPE_CHECKBOX))
		{
			return PX_Object_CheckBoxGetText(pGridObject);
		}
		else if (PX_ObjectCheckType(pGridObject, PX_OBJECT_TYPE_BUTTON))
		{
			return PX_Object_PushButtonGetText(pGridObject);
		}
		else if (PX_ObjectCheckType(pGridObject, PX_OBJECT_TYPE_SELECTBAR))
		{
			return PX_Object_SelectBarGetCurrentText(pGridObject);
		}
		else
		{
			return "";
		}
	}
	PX_ASSERT();
	return PX_NULL;
}

px_void PX_Object_GridSetText(PX_Object* pObject, px_int x, px_int y, const px_char text[])
{
	PX_Object* pGridObject = PX_Object_GridGetObject(pObject, x, y);
	if (pGridObject)
	{
		if (PX_ObjectCheckType(pGridObject, PX_OBJECT_TYPE_LABEL))
		{
			PX_Object_LabelSetText(pGridObject, text);
		}
		else if (PX_ObjectCheckType(pGridObject, PX_OBJECT_TYPE_EDIT))
		{
			PX_Object_EditSetText(pGridObject, text);
		}
		else if (PX_ObjectCheckType(pGridObject, PX_OBJECT_TYPE_CHECKBOX))
		{
			PX_Object_CheckBoxSetText(pGridObject, text);
		}
		else if (PX_ObjectCheckType(pGridObject, PX_OBJECT_TYPE_BUTTON))
		{
			PX_Object_PushButtonSetText(pGridObject, text);
		}
		else if (PX_ObjectCheckType(pGridObject, PX_OBJECT_TYPE_SELECTBAR))
		{
			PX_Object_SelectBarSetCurrentText(pGridObject, text);
		}
	}
}


px_void PX_Object_GridSetIndex(PX_Object* pObject, px_int x, px_int y, px_int index)
{
	PX_Object* pGridObject = PX_Object_GridGetObject(pObject, x, y);
	if (pGridObject)
	{
		if (PX_ObjectCheckType(pGridObject, PX_OBJECT_TYPE_SELECTBAR))
		{
			PX_Object_SelectBarSetCurrentIndex(pGridObject, index);
		}
	}
}

px_int PX_Object_GridGetSelectIndex(PX_Object* pObject, px_int x, px_int y)
{
	PX_Object* pGridObject = PX_Object_GridGetObject(pObject, x, y);
	if (pGridObject)
	{
		if (PX_ObjectCheckType(pGridObject, PX_OBJECT_TYPE_SELECTBAR))
		{
			return PX_Object_SelectBarGetCurrentIndex(pGridObject);
		}
	}
	return -1;
}

px_bool PX_Object_GridExportToAbi(PX_Object* pObject, px_abi* pAbi)
{
	PX_Object_grid* pGrid = (PX_Object_grid*)PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_GRID);
	px_int i, j;
	px_int count = 0;
	for (i = 0; i < pGrid->ycount; i++)
	{
		for (j = 0; j < pGrid->xcount; j++)
		{
			if (pGrid->cell[i * pGrid->xcount + j].object != PX_NULL)
			{
				if (pGrid->cell[i * pGrid->xcount + j].startx==j&& pGrid->cell[i * pGrid->xcount + j].starty == i)
				{
					//first cell
					px_char countcontent[8];
					px_abi abitoken;
					PX_itoa(count, countcontent, 8, 10);
					count++;
					PX_AbiCreateDynamicWriter(&abitoken, pGrid->mp);

					PX_AbiSet_int(&abitoken, "x",j);
					PX_AbiSet_int(&abitoken, "y", i);
					if (PX_ObjectCheckType(pGrid->cell[i * pGrid->xcount + j].object, PX_OBJECT_TYPE_LABEL))
					{
						PX_AbiSet_string(&abitoken, "type", "label");
						PX_AbiSet_string(&abitoken, "text", PX_Object_LabelGetText(pGrid->cell[i * pGrid->xcount + j].object));
					}
					else if (PX_ObjectCheckType(pGrid->cell[i * pGrid->xcount + j].object, PX_OBJECT_TYPE_EDIT))
					{
						PX_AbiSet_string(&abitoken, "type", "edit");
						PX_AbiSet_string(&abitoken, "text", PX_Object_EditGetText(pGrid->cell[i * pGrid->xcount + j].object));
					}
					else if (PX_ObjectCheckType(pGrid->cell[i * pGrid->xcount + j].object, PX_OBJECT_TYPE_CHECKBOX))
					{
						PX_AbiSet_string(&abitoken, "type", "checkbox");
						PX_AbiSet_string(&abitoken, "text", PX_Object_CheckBoxGetText(pGrid->cell[i * pGrid->xcount + j].object));
					}
					else if (PX_ObjectCheckType(pGrid->cell[i * pGrid->xcount + j].object, PX_OBJECT_TYPE_BUTTON))
					{
						PX_AbiSet_string(&abitoken, "type", "button");
						PX_AbiSet_string(&abitoken, "text", PX_Object_PushButtonGetText(pGrid->cell[i * pGrid->xcount + j].object));
					}
					else if (PX_ObjectCheckType(pGrid->cell[i * pGrid->xcount + j].object, PX_OBJECT_TYPE_SELECTBAR))
					{
						PX_AbiSet_string(&abitoken, "type", "selectbar");
						PX_AbiSet_string(&abitoken, "text", PX_Object_SelectBarGetCurrentText(pGrid->cell[i * pGrid->xcount + j].object));
					}
					else
					{
						PX_AbiFree(&abitoken);
						return PX_FALSE;
					}
					PX_AbiSet_Abi(pAbi, countcontent, &abitoken);
					PX_AbiFree(&abitoken);
				}
			}
		}
	}
	return PX_TRUE;
}

px_bool PX_Object_GridImportFromAbi( PX_Object* pObject, px_abi* pAbi)
{
	px_char countcontent[8];
	px_int count = 0;
	while (PX_TRUE)
	{
		px_abi grid_abi;
		const px_char* pType;
		const px_char* pText;
		px_int *px, *py;
		PX_itoa(count, countcontent, 8, 10);
		count++;
		if (!PX_AbiRead_Abi(pAbi, countcontent, &grid_abi))
		{
			break;
		}
		pType = PX_AbiGet_string(&grid_abi, "type");
		pText = PX_AbiGet_string(&grid_abi, "text");
		px = PX_AbiGet_int(&grid_abi, "x");
		py = PX_AbiGet_int(&grid_abi, "y");
		if (!pType || !pText || !px || !py)
		{
			PX_AbiFree(&grid_abi);
			return PX_FALSE;
		}
		PX_Object_GridSetText(pObject, *px, *py, pText);
	}
	return PX_TRUE;
	
}

PX_Object* PX_Object_GridSetButton(PX_Object* pObject, px_int x, px_int y, px_int xsize, px_int ysize, const px_char label[], PX_FontModule* fontmodule)
{
	PX_Object_grid* pGrid = (PX_Object_grid*)PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_GRID);
	if (PX_Object_GridCellIsEmpty(pObject, x, y, xsize, ysize))
	{
		PX_Object* pButton = PX_Object_PushButtonCreate(pGrid->mp,pObject, (px_int)(x*pGrid->xsize), (px_int)(y*pGrid->ysize), (px_int)(xsize*pGrid->xsize), (px_int)(ysize*pGrid->ysize), label, fontmodule);
		if (pButton)
		{
			PX_Object_GridSetCellObject(pObject, x, y, xsize, ysize, pButton);
			return pButton;
		}
		return PX_NULL;
	}
	return PX_NULL;
}
PX_Object* PX_Object_GridSetSliderBar(PX_Object* pObject, px_int x, px_int y, px_int xsize, px_int ysize, PX_FontModule* fontmodule)
{
	PX_Object_grid* pGrid = (PX_Object_grid*)PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_GRID);
	if (PX_Object_GridCellIsEmpty(pObject, x, y, xsize, ysize))
	{
		PX_Object* pSliderBar = PX_Object_SliderBarCreate(pGrid->mp,pObject, (px_int)(x*pGrid->xsize), (px_int)(y*pGrid->ysize), (px_int)(xsize*pGrid->xsize), (px_int)(ysize*pGrid->ysize),PX_OBJECT_SLIDERBAR_TYPE_HORIZONTAL,PX_OBJECT_SLIDERBAR_STYLE_LINER);
		if (pSliderBar)
		{
			PX_Object_GridSetCellObject(pObject, x, y, xsize, ysize, pSliderBar);
			PX_ObjectRegisterEvent(pSliderBar, PX_OBJECT_EVENT_VALUECHANGED, PX_Object_GridOnChanged, pObject);
			return pSliderBar;
		}
		return PX_NULL;
	}
	return PX_NULL;
}
PX_Object* PX_Object_GridSetEdit(PX_Object* pObject, px_int x, px_int y, px_int xsize, px_int ysize, PX_FontModule* fontmodule)
{
	PX_Object_grid* pGrid = (PX_Object_grid*)PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_GRID);
	if (PX_Object_GridCellIsEmpty(pObject, x, y, xsize, ysize))
	{
		PX_Object* pEdit = PX_Object_EditCreate(pGrid->mp,pObject, (px_int)(x*pGrid->xsize), (px_int)(y*pGrid->ysize), (px_int)(xsize*pGrid->xsize), (px_int)(ysize*pGrid->ysize), fontmodule);
		if (pEdit)
		{
			PX_Object_GridSetCellObject(pObject, x, y, xsize, ysize, pEdit);
			PX_ObjectRegisterEvent(pEdit, PX_OBJECT_EVENT_LOSTFOCUS, PX_Object_GridOnChanged, pObject);
			return pEdit;
		}
		return PX_NULL;
	}
	return PX_NULL;
}
PX_Object* PX_Object_GridSetCheckBox(PX_Object* pObject, px_int x, px_int y, px_int xsize, px_int ysize, const px_char content[], PX_FontModule* fontmodule)
{
	PX_Object_grid* pGrid = (PX_Object_grid*)PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_GRID);
	if (PX_Object_GridCellIsEmpty(pObject, x, y, xsize, ysize))
	{
		PX_Object* pCheckBox = PX_Object_CheckBoxCreate(pGrid->mp, pObject, (px_int)(x * pGrid->xsize), (px_int)(y * pGrid->ysize), (px_int)(xsize * pGrid->xsize), (px_int)(ysize * pGrid->ysize), content, fontmodule);
		if (pCheckBox)
		{
			PX_Object_GridSetCellObject(pObject, x, y, xsize, ysize, pCheckBox);
			PX_ObjectRegisterEvent(pCheckBox, PX_OBJECT_EVENT_VALUECHANGED, PX_Object_GridOnChanged, pObject);
			return pCheckBox;
		}
		return PX_NULL;
	}
	return PX_NULL;
}
PX_Object* PX_Object_GridSetImage(PX_Object* pObject, px_int x, px_int y, px_int xsize, px_int ysize)
{
	PX_Object_grid* pGrid = (PX_Object_grid*)PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_GRID);
	if (PX_Object_GridCellIsEmpty(pObject, x, y, xsize, ysize))
	{
		PX_Object* pImage = PX_Object_ImageCreate(pGrid->mp,pObject, (px_int)(x*pGrid->xsize), (px_int)(y*pGrid->ysize), (px_int)(xsize*pGrid->xsize), (px_int)(ysize*pGrid->ysize),0);
		if (pImage)
		{
			PX_Object_GridSetCellObject(pObject, x, y, xsize, ysize, pImage);
			return pImage;
		}
		return PX_NULL;
	}
	return PX_NULL;
}
PX_Object* PX_Object_GridSetEmpty(PX_Object* pObject, px_int x, px_int y, px_int xsize, px_int ysize)
{
	PX_Object_grid* pGrid = (PX_Object_grid*)PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_GRID);
	if (PX_Object_GridCellIsEmpty(pObject, x, y, xsize, ysize))
	{
		PX_Object* pEmpty = PX_NULL;
		PX_Object_GridSetCellObject(pObject, x, y, xsize, ysize, pEmpty);
		return pEmpty;
	}
	return PX_NULL;
}