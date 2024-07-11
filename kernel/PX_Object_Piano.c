#include "PX_Object_Piano.h"

PX_Object_Piano* PX_Object_GetPiano(PX_Object* pObject)
{
	PX_Object_Piano *pDesc = (PX_Object_Piano*)PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_PIANO);
	PX_ASSERTIF(pDesc == PX_NULL);
	return pDesc;
}

PX_OBJECT_RENDER_FUNCTION(PX_Object_PianoRender)
{
	px_int step=0, i,j=2;
	px_float x,white_key_w,keyw, keyh;

	PX_Object_Piano* pPiano = PX_Object_GetPiano(pObject);
	const px_char* note_content[] = { "A","#A","B", "C","#C","D","#D","E","F","#F","G","#G", };
	px_float objx, objy, objWidth, objHeight;
	px_float inheritX, inheritY;
	PX_ObjectGetInheritXY(pObject, &inheritX, &inheritY);

	objx = (pObject->x + inheritX);
	objy = (pObject->y + inheritY);
	objWidth = pObject->Width;
	objHeight = pObject->Height;

	keyh = objHeight;
	white_key_w = objWidth / 52;
	if (white_key_w <=2||!keyh)
	{
		return;
	}
	while (j--)
	{
		step = 0;
		for (i = 0; i < 88; i++)
		{
			px_color color;
			if (note_content[i % 12][0] == '#')
			{
				if (j == 1)continue;
				keyh = objHeight *5/8;
				keyw = objWidth / 52 *4 / 5;
				x = (objx + step * white_key_w - white_key_w*2 / 5);
				color = PX_COLOR_BLACK;
			}
			else
			{
				if (j == 0) 
				{
					step++;
					continue;
				}
				keyh = objHeight;
				keyw = objWidth / 52;
				x = (objx + step * white_key_w);
				color = PX_COLOR_WHITE;
				step++;
			}
			PX_GeoDrawRect(psurface, (px_int)x, (px_int)objy, (px_int)(x + keyw), (px_int)(objy + keyh), color);
			PX_GeoDrawBorder(psurface, (px_int)x , (px_int)objy, (px_int)(x + keyw), (px_int)(objy + keyh), 1, PX_COLOR_BLACK);

			switch (pPiano->keyState[i])
			{
			case PX_OBJECT_PIANO_KEYSTATE_STANDBY:
				break;
			case PX_OBJECT_PIANO_KEYSTATE_DOWN:
				PX_GeoDrawRect(psurface, (px_int)x, (px_int)objy, (px_int)(x + keyw), (px_int)(objy + keyh), PX_COLOR(128, 64, 64, 255) );
				break;
			case PX_OBJECT_PIANO_KEYSTATE_CURSOR:
				PX_GeoDrawRect(psurface, (px_int)x, (px_int)objy, (px_int)(x + keyw), (px_int)(objy + keyh), PX_COLOR(96, 192, 255, 64));
				break;
			case PX_OBJECT_PIANO_KEYSTATE_ONFOCUS:
				PX_GeoDrawRect(psurface, (px_int)x, (px_int)objy, (px_int)(x + keyw), (px_int)(objy + keyh), PX_COLOR(96, 255, 192, 64));
				break;
			}
		}
	}
	
}

px_int PX_Object_PianoCursorInkeyIndex(PX_Object* pObject, px_float cursorx, px_float cursory)
{
	px_int step = 0, x,i, j = 2;
	px_float  white_key_w, keyw, keyh;
	px_int lastIndex = -1;
	PX_Object_Piano* pPiano = PX_Object_GetPiano(pObject);
	const px_char* note_content[] = { "A","#A","B", "C","#C","D","#D","E","F","#F","G","#G", };
	px_float objx, objy, objWidth, objHeight;
	px_float inheritX, inheritY;
	PX_ObjectGetInheritXY(pObject, &inheritX, &inheritY);

	objx = (pObject->x + inheritX);
	objy = (pObject->y + inheritY);
	objWidth = pObject->Width;
	objHeight = pObject->Height;

	keyh = objHeight;
	white_key_w = objWidth / 52;

	while (j--)
	{
		step = 0;
		for (i = 0; i < 88; i++)
		{
			if (note_content[i % 12][0] == '#')
			{
				if (j == 1)continue;
				keyh = objHeight * 5 / 8;
				keyw = objWidth / 52 * 4 / 5;
				x =(px_int)(objx + step * white_key_w - white_key_w * 2 / 5);
			}
			else
			{
				if (j == 0)
				{
					step++;
					continue;
				}
				keyh = objHeight;
				keyw = objWidth / 52;
				x = (px_int)(objx + step * white_key_w);
				step++;
			}
			if (PX_isXYInRegion(cursorx, cursory,x*1.0f,objy,keyw, keyh))
			{
				lastIndex = i;
			}
		}
	}
	return lastIndex;
}

PX_OBJECT_EVENT_FUNCTION(PX_Object_PianoOnCursorMove)
{
	px_int i;
	px_int index;
	PX_Object_Piano* pPiano = PX_Object_GetPiano(pObject);

	for (i = 0; i < 88; i++)
	{
		if(pPiano->keyState[i]!= PX_OBJECT_PIANO_KEYSTATE_ONFOCUS)
			pPiano->keyState[i] = PX_OBJECT_PIANO_KEYSTATE_STANDBY;
	}
	index = PX_Object_PianoCursorInkeyIndex(pObject, PX_Object_Event_GetCursorX(e), PX_Object_Event_GetCursorY(e));
	if (index!=-1)
	{
		if (pPiano->keyState[index] != PX_OBJECT_PIANO_KEYSTATE_ONFOCUS)
			pPiano->keyState[index] =PX_OBJECT_PIANO_KEYSTATE_CURSOR;
	}

}

PX_OBJECT_EVENT_FUNCTION(PX_Object_PianoOnCursorUp)
{
	px_int i;
	px_int index;
	PX_Object_Piano* pPiano = PX_Object_GetPiano(pObject);

	for (i = 0; i < 88; i++)
	{
		if (pPiano->keyState[i] != PX_OBJECT_PIANO_KEYSTATE_ONFOCUS)
			pPiano->keyState[i] = PX_OBJECT_PIANO_KEYSTATE_STANDBY;
	}
	index = PX_Object_PianoCursorInkeyIndex(pObject, PX_Object_Event_GetCursorX(e), PX_Object_Event_GetCursorY(e));
	if (index != -1)
	{
		if (pPiano->keyState[index] != PX_OBJECT_PIANO_KEYSTATE_ONFOCUS)
			pPiano->keyState[index] = PX_OBJECT_PIANO_KEYSTATE_CURSOR;
	}

}

PX_OBJECT_EVENT_FUNCTION(PX_Object_PianoOnCursorDown)
{
	px_int index;
	PX_Object_Piano* pPiano = PX_Object_GetPiano(pObject);
	if (!PX_ObjectIsCursorInRegion(pObject, e))
	{
		return;
	}
	index = PX_Object_PianoCursorInkeyIndex(pObject, PX_Object_Event_GetCursorX(e), PX_Object_Event_GetCursorY(e));
	if (index != -1)
	{
		if (pPiano->keyState[index] != PX_OBJECT_PIANO_KEYSTATE_ONFOCUS)
			pPiano->keyState[index] = PX_OBJECT_PIANO_KEYSTATE_DOWN;
		pPiano->lastKeyDownIndex = index;
		PX_ObjectPostEvent(pObject, PX_OBJECT_BUILD_EVENT_INT(PX_OBJECT_EVENT_VALUECHANGED, index));

		if (pPiano->pPiano)
		{
			PX_PianoTriggerIndex(pPiano->pPiano, index, 1);
		}

		if (pPiano->pPianoModel)
		{
			PX_PianoModelTriggerIndex(pPiano->pPianoModel, index);
		}

	}
}

PX_OBJECT_EVENT_FUNCTION(PX_Object_PianoOnCursorDrag)
{
	px_int i;
	px_int index;
	PX_Object_Piano* pPiano = PX_Object_GetPiano(pObject);
	for (i = 0; i < 88; i++)
	{
		if (pPiano->keyState[i] != PX_OBJECT_PIANO_KEYSTATE_ONFOCUS)
			pPiano->keyState[i] = PX_OBJECT_PIANO_KEYSTATE_STANDBY;
	}
	index = PX_Object_PianoCursorInkeyIndex(pObject, PX_Object_Event_GetCursorX(e), PX_Object_Event_GetCursorY(e));
	if (index != -1)
	{
		if (pPiano->keyState[index] != PX_OBJECT_PIANO_KEYSTATE_ONFOCUS)
			pPiano->keyState[index] = PX_OBJECT_PIANO_KEYSTATE_DOWN;
		if (pPiano->lastKeyDownIndex!=index)
		{
			pPiano->lastKeyDownIndex = index;
			PX_ObjectPostEvent(pObject, PX_OBJECT_BUILD_EVENT_INT(PX_OBJECT_EVENT_VALUECHANGED, index));
		}
		
	}
}

PX_Object* PX_Object_PianoAttachObject( PX_Object* pObject,px_int attachIndex, px_float x, px_float y, px_float width, px_float height)
{
	px_memorypool* mp=pObject->mp;
	PX_Object_Piano Piano = {0}, * pdesc;

	PX_ASSERTIF(pObject == PX_NULL);
	PX_ASSERTIF(attachIndex < 0 || attachIndex >= PX_COUNTOF(pObject->pObjectDesc));
	PX_ASSERTIF(pObject->pObjectDesc[attachIndex] != PX_NULL);
	pdesc = (PX_Object_Piano*)PX_ObjectCreateDesc(pObject, attachIndex, PX_OBJECT_TYPE_PIANO, 0, PX_Object_PianoRender, 0, &Piano, sizeof(Piano));
	PX_ASSERTIF(pdesc == PX_NULL);

	PX_ObjectRegisterEvent(pObject, PX_OBJECT_EVENT_CURSORMOVE, PX_Object_PianoOnCursorMove, PX_NULL);
	PX_ObjectRegisterEvent(pObject, PX_OBJECT_EVENT_CURSORDOWN, PX_Object_PianoOnCursorDown, PX_NULL);
	PX_ObjectRegisterEvent(pObject, PX_OBJECT_EVENT_CURSORDRAG, PX_Object_PianoOnCursorDrag, PX_NULL);
	PX_ObjectRegisterEvent(pObject, PX_OBJECT_EVENT_CURSORUP, PX_Object_PianoOnCursorUp, PX_NULL);
	return pObject;
}


PX_Object *PX_Object_PianoCreate(px_memorypool* mp, PX_Object* Parent, px_float x, px_float y, px_float width, px_float height)
{
	PX_Object* pObject;
	pObject=PX_ObjectCreate(mp, Parent, x, y, 0, width, height, 0);
	if (!pObject)
	{
		return PX_NULL;
	}
	if (!PX_Object_PianoAttachObject(pObject, 0, x, y, width, height))
	{
		PX_ObjectDelete(pObject);
		return PX_NULL;
	}

	return pObject;
}

px_void PX_Object_PianoLinkPiano(PX_Object* pObject, PX_Piano* pPiano)
{
	PX_Object_Piano* pObjectPiano = PX_Object_GetPiano(pObject);
	pObjectPiano->pPiano = pPiano;

}

px_void PX_Object_PianoLinkPianoModel(PX_Object* pObject, PX_PianoModel* pPianoModel)
{
	PX_Object_Piano* pObjectPiano = PX_Object_GetPiano(pObject);
	pObjectPiano->pPianoModel = pPianoModel;
}

px_void PX_Object_PianoSetKeyFocus(PX_Object* pObject, px_int index)
{
	PX_Object_Piano* pPiano = PX_Object_GetPiano(pObject);
	if (index >= 0 && index < 88)
	{
		pPiano->keyState[index] = PX_OBJECT_PIANO_KEYSTATE_ONFOCUS;
	}
}

px_void PX_Object_PianoSetKeyDown(PX_Object* pObject, px_int index,px_bool bdown)
{
	PX_Object_Piano* pPiano = PX_Object_GetPiano(pObject);
	if (bdown)
		pPiano->keyState[index] = PX_OBJECT_PIANO_KEYSTATE_DOWN;
	else
		pPiano->keyState[index] = PX_OBJECT_PIANO_KEYSTATE_STANDBY;
}

px_void PX_Object_PianoClearKeyState(PX_Object* pObject)
{
	px_int i;
	PX_Object_Piano* pPiano = PX_Object_GetPiano(pObject);
	for (i = 0; i < 88; i++)
	{
		pPiano->keyState[i] = PX_OBJECT_PIANO_KEYSTATE_STANDBY;
	}
}

PX_OBJECT_RENDER_FUNCTION(PX_Object_PianoTuneOnRender)
{
	px_float step,oftx,ofty;
	PX_Object_PianoTune* pTune=PX_ObjectGetDesc(PX_Object_PianoTune, pObject);
	oftx = 5;
	ofty = 5;
	step = 52;
	
	if (pObject->Width<24)
	{
		return;
	}
	pTune->scrollArea->x = pObject->x;
	pTune->scrollArea->y = pObject->y;
	pTune->scrollArea->Width = pObject->Width-24;
	pTune->scrollArea->Height = pObject->Height;

////////////////////////////////////////////////////////
//frequency
	

	pTune->label_f->x = oftx;
	pTune->label_f->y = ofty;
	pTune->label_f->Width = pTune->scrollArea->Width*5/6;

	pTune->sliderbar_f->x = oftx ;
	pTune->sliderbar_f->y = ofty+28;
	pTune->sliderbar_f->Width = pTune->scrollArea->Width*5/6;

	ofty += step;
////////////////////////////////////////////////////////
//weight
	
	pTune->label_weight->x = oftx;
	pTune->label_weight->y = ofty;
	pTune->label_weight->Width = pTune->scrollArea->Width*5/6;

	pTune->sliderbar_weight->x = oftx;
	pTune->sliderbar_weight->y = ofty + 28;
	pTune->sliderbar_weight->Width = pTune->scrollArea->Width*5/6;

	ofty += step;

////////////////////////////////////////////////////////
//hammer_type 
	pTune->label_hammer_type->x = oftx;
	pTune->label_hammer_type->y = ofty;
	pTune->label_hammer_type->Width = pTune->scrollArea->Width * 5 / 6;

	pTune->selectbox_hammer_type->x = oftx;
	pTune->selectbox_hammer_type->y = ofty + 28;
	pTune->selectbox_hammer_type->Width = pTune->scrollArea->Width * 5 / 6;

	ofty += step;
////////////////////////////////////////////////////////
//minr
	pTune->label_minr->x = oftx;
	pTune->label_minr->y = ofty;
	pTune->label_minr->Width = pTune->scrollArea->Width*5/6;

	pTune->sliderbar_minr->x = oftx;
	pTune->sliderbar_minr->y = ofty + 28;
	pTune->sliderbar_minr->Width = pTune->scrollArea->Width*5/6;

	ofty += step;

////////////////////////////////////////////////////////
//maxr
	pTune->label_maxr->x = oftx;
	pTune->label_maxr->y = ofty;
	pTune->label_maxr->Width = pTune->scrollArea->Width*5/6;

	pTune->sliderbar_maxr->x = oftx;
	pTune->sliderbar_maxr->y = ofty + 28;
	pTune->sliderbar_maxr->Width = pTune->scrollArea->Width*5/6;

	ofty += step;
////////////////////////////////////////////////////////
//amprl
	pTune->label_amprl->x = oftx;
	pTune->label_amprl->y = ofty;
	pTune->label_amprl->Width = pTune->scrollArea->Width*5/6;

	pTune->sliderbar_amprl->x = oftx;
	pTune->sliderbar_amprl->y = ofty + 28;
	pTune->sliderbar_amprl->Width = pTune->scrollArea->Width*5/6;

	ofty += step;

////////////////////////////////////////////////////////
//amprr
	pTune->label_amprr->x = oftx;
	pTune->label_amprr->y = ofty;
	pTune->label_amprr->Width = pTune->scrollArea->Width*5/6;

	pTune->sliderbar_amprr->x = oftx;
	pTune->sliderbar_amprr->y = ofty + 28;
	pTune->sliderbar_amprr->Width = pTune->scrollArea->Width*5/6;

	ofty += step;

////////////////////////////////////////////////////////
//label_mult_radius_core_string
	pTune->label_mult_radius_core_string->x = oftx;
	pTune->label_mult_radius_core_string->y = ofty;
	pTune->label_mult_radius_core_string->Width = pTune->scrollArea->Width*5/6;

	pTune->sliderbar_mult_radius_core_string->x = oftx;
	pTune->sliderbar_mult_radius_core_string->y = ofty + 28;
	pTune->sliderbar_mult_radius_core_string->Width = pTune->scrollArea->Width*5/6;

	ofty += step;

////////////////////////////////////////////////////////
//minL
	pTune->label_minL->x = oftx;
	pTune->label_minL->y = ofty;
	pTune->label_minL->Width = pTune->scrollArea->Width*5/6;

	pTune->sliderbar_minL->x = oftx;
	pTune->sliderbar_minL->y = ofty + 28;
	pTune->sliderbar_minL->Width = pTune->scrollArea->Width*5/6;

	ofty += step;

////////////////////////////////////////////////////////
//maxL
	pTune->label_maxL->x = oftx;
	pTune->label_maxL->y = ofty;
	pTune->label_maxL->Width = pTune->scrollArea->Width*5/6;

	pTune->sliderbar_maxL->x = oftx;
	pTune->sliderbar_maxL->y = ofty + 28;
	pTune->sliderbar_maxL->Width = pTune->scrollArea->Width*5/6;

	ofty += step;

////////////////////////////////////////////////////////
//ampLl
	pTune->label_ampLl->x = oftx;
	pTune->label_ampLl->y = ofty;
	pTune->label_ampLl->Width = pTune->scrollArea->Width*5/6;

	pTune->sliderbar_ampLl->x = oftx;
	pTune->sliderbar_ampLl->y = ofty + 28;
	pTune->sliderbar_ampLl->Width = pTune->scrollArea->Width*5/6;

	ofty += step;

////////////////////////////////////////////////////////
//ampLr
	pTune->label_ampLr->x = oftx;
	pTune->label_ampLr->y = ofty;
	pTune->label_ampLr->Width = pTune->scrollArea->Width*5/6;

	pTune->sliderbar_ampLr->x = oftx;
	pTune->sliderbar_ampLr->y = ofty + 28;
	pTune->sliderbar_ampLr->Width = pTune->scrollArea->Width*5/6;

	ofty += step;

////////////////////////////////////////////////////////
//mult_density_string
	pTune->label_mult_density_string->x = oftx;
	pTune->label_mult_density_string->y = ofty;
	pTune->label_mult_density_string->Width = pTune->scrollArea->Width*5/6;

	pTune->sliderbar_mult_density_string->x = oftx;
	pTune->sliderbar_mult_density_string->y = ofty + 28;
	pTune->sliderbar_mult_density_string->Width = pTune->scrollArea->Width*5/6;

	ofty += step;

////////////////////////////////////////////////////////
//mult_modulus_string
	pTune->label_mult_modulus_string->x = oftx;
	pTune->label_mult_modulus_string->y = ofty;
	pTune->label_mult_modulus_string->Width = pTune->scrollArea->Width*5/6;

	pTune->sliderbar_mult_modulus_string->x = oftx;
	pTune->sliderbar_mult_modulus_string->y = ofty + 28;
	pTune->sliderbar_mult_modulus_string->Width = pTune->scrollArea->Width*5/6;

	ofty += step;

////////////////////////////////////////////////////////
//mult_impedance_bridge
	pTune->label_mult_impedance_bridge->x = oftx;
	pTune->label_mult_impedance_bridge->y = ofty;
	pTune->label_mult_impedance_bridge->Width = pTune->scrollArea->Width*5/6;

	pTune->sliderbar_mult_impedance_bridge->x = oftx;
	pTune->sliderbar_mult_impedance_bridge->y = ofty + 28;
	pTune->sliderbar_mult_impedance_bridge->Width = pTune->scrollArea->Width*5/6;

	ofty += step;

////////////////////////////////////////////////////////
//mult_impedance_hammer
	pTune->label_mult_impedance_hammer->x = oftx;
	pTune->label_mult_impedance_hammer->y = ofty;
	pTune->label_mult_impedance_hammer->Width = pTune->scrollArea->Width*5/6;

	pTune->sliderbar_mult_impedance_hammer->x = oftx;
	pTune->sliderbar_mult_impedance_hammer->y = ofty + 28;
	pTune->sliderbar_mult_impedance_hammer->Width = pTune->scrollArea->Width*5/6;

	ofty += step;


////////////////////////////////////////////////////////
//mult_mass_hammer
	pTune->label_mult_mass_hammer->x = oftx;
	pTune->label_mult_mass_hammer->y = ofty;
	pTune->label_mult_mass_hammer->Width = pTune->scrollArea->Width*5/6;

	pTune->sliderbar_mult_mass_hammer->x = oftx;
	pTune->sliderbar_mult_mass_hammer->y = ofty + 28;
	pTune->sliderbar_mult_mass_hammer->Width = pTune->scrollArea->Width*5/6;

	ofty += step;

////////////////////////////////////////////////////////
//mult_force_hammer
	pTune->label_mult_force_hammer->x = oftx;
	pTune->label_mult_force_hammer->y = ofty;
	pTune->label_mult_force_hammer->Width = pTune->scrollArea->Width*5/6;

	pTune->sliderbar_mult_force_hammer->x = oftx;
	pTune->sliderbar_mult_force_hammer->y = ofty + 28;
	pTune->sliderbar_mult_force_hammer->Width = pTune->scrollArea->Width*5/6;

	ofty += step;

////////////////////////////////////////////////////////
//mult_hysteresis_hammer
	pTune->label_mult_hysteresis_hammer->x = oftx;
	pTune->label_mult_hysteresis_hammer->y = ofty;
	pTune->label_mult_hysteresis_hammer->Width = pTune->scrollArea->Width*5/6;

	pTune->sliderbar_mult_hysteresis_hammer->x = oftx;
	pTune->sliderbar_mult_hysteresis_hammer->y = ofty + 28;
	pTune->sliderbar_mult_hysteresis_hammer->Width = pTune->scrollArea->Width*5/6;

	ofty += step;

////////////////////////////////////////////////////////
//mult_hysteresis_hammer
	pTune->label_mult_stiffness_exponent_hammer->x = oftx;
	pTune->label_mult_stiffness_exponent_hammer->y = ofty;
	pTune->label_mult_stiffness_exponent_hammer->Width = pTune->scrollArea->Width*5/6;

	pTune->sliderbar_mult_stiffness_exponent_hammer->x = oftx;
	pTune->sliderbar_mult_stiffness_exponent_hammer->y = ofty + 28;
	pTune->sliderbar_mult_stiffness_exponent_hammer->Width = pTune->scrollArea->Width*5/6;

	ofty += step;

////////////////////////////////////////////////////////
//position_hammer
	pTune->label_position_hammer->x = oftx;
	pTune->label_position_hammer->y = ofty;
	pTune->label_position_hammer->Width = pTune->scrollArea->Width*5/6;

	pTune->sliderbar_position_hammer->x = oftx;
	pTune->sliderbar_position_hammer->y = ofty + 28;
	pTune->sliderbar_position_hammer->Width = pTune->scrollArea->Width*5/6;

	ofty += step;

////////////////////////////////////////////////////////
//mult_loss_filter
	pTune->label_mult_loss_filter->x = oftx;
	pTune->label_mult_loss_filter->y = ofty;
	pTune->label_mult_loss_filter->Width = pTune->scrollArea->Width*5/6;

	pTune->sliderbar_mult_loss_filter->x = oftx;
	pTune->sliderbar_mult_loss_filter->y = ofty + 28;
	pTune->sliderbar_mult_loss_filter->Width = pTune->scrollArea->Width*5/6;

	ofty += step;

////////////////////////////////////////////////////////
//detune
	pTune->label_detune->x = oftx;
	pTune->label_detune->y = ofty;
	pTune->label_detune->Width = pTune->scrollArea->Width*5/6;

	pTune->sliderbar_detune->x = oftx;
	pTune->sliderbar_detune->y = ofty + 28;
	pTune->sliderbar_detune->Width = pTune->scrollArea->Width*5/6;

	ofty += step;

////////////////////////////////////////////////////////
//soundboard
	pTune->label_eq1->x = oftx;
	pTune->label_eq1->y = ofty;
	pTune->label_eq1->Width = pTune->scrollArea->Width * 5 / 6;

	pTune->sliderbar_eq1->x = oftx;
	pTune->sliderbar_eq1->y = ofty + 28;
	pTune->sliderbar_eq1->Width = pTune->scrollArea->Width * 5 / 6;

	ofty += step;

	pTune->label_eq2->x = oftx;
	pTune->label_eq2->y = ofty;
	pTune->label_eq2->Width = pTune->scrollArea->Width * 5 / 6;

	pTune->sliderbar_eq2->x = oftx;
	pTune->sliderbar_eq2->y = ofty + 28;
	pTune->sliderbar_eq2->Width = pTune->scrollArea->Width * 5 / 6;

	ofty += step;


	pTune->label_eq3->x = oftx;
	pTune->label_eq3->y = ofty;
	pTune->label_eq3->Width = pTune->scrollArea->Width * 5 / 6;

	pTune->sliderbar_eq3->x = oftx;
	pTune->sliderbar_eq3->y = ofty + 28;
	pTune->sliderbar_eq3->Width = pTune->scrollArea->Width * 5 / 6;

	ofty += step;

	pTune->label_c1->x = oftx;
	pTune->label_c1->y = ofty;
	pTune->label_c1->Width = pTune->scrollArea->Width * 5 / 6;

	pTune->sliderbar_c1->x = oftx;
	pTune->sliderbar_c1->y = ofty + 28;
	pTune->sliderbar_c1->Width = pTune->scrollArea->Width * 5 / 6;

	ofty += step;

	pTune->label_c3->x = oftx;
	pTune->label_c3->y = ofty;
	pTune->label_c3->Width = pTune->scrollArea->Width * 5 / 6;

	pTune->sliderbar_c3->x = oftx;
	pTune->sliderbar_c3->y = ofty + 28;
	pTune->sliderbar_c3->Width = pTune->scrollArea->Width * 5 / 6;

	ofty += step;
}

px_void PX_Object_PianoTune_LocalToUI(PX_Object* pObject)
{
	px_int ivalue;
	px_char content[64];
	PX_Object_PianoTune* pTune = (PX_Object_PianoTune *)PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_PIANOTUNE);

	ivalue = (px_int)(pTune->key_param.f * 100);
	PX_Object_SliderBarSetValue(pTune->sliderbar_f, ivalue);
	PX_sprintf1(content, sizeof(content), "frequency:%1.2", PX_STRINGFORMAT_FLOAT(pTune->key_param.f));
	PX_Object_LabelSetText(pTune->label_f, content);

	ivalue = (px_int)(pTune->key_param.weight * 100);
	PX_Object_SliderBarSetValue(pTune->sliderbar_weight, ivalue);
	PX_sprintf1(content, sizeof(content), "Key pulsation velocity weight:%1.2", PX_STRINGFORMAT_FLOAT(pTune->key_param.weight));
	PX_Object_LabelSetText(pTune->label_weight, content);

	ivalue = (px_int)(pTune->key_param.minr * 1000);
	PX_Object_SliderBarSetValue(pTune->sliderbar_minr, ivalue);
	PX_sprintf1(content, sizeof(content), "minimum string radio:%1.2", PX_STRINGFORMAT_FLOAT(pTune->key_param.minr));
	PX_Object_LabelSetText(pTune->label_minr, content);

	ivalue = (px_int)(pTune->key_param.maxr * 100);
	PX_Object_SliderBarSetValue(pTune->sliderbar_maxr, ivalue);
	PX_sprintf1(content, sizeof(content), "maximum string radio:%1.2", PX_STRINGFORMAT_FLOAT(pTune->key_param.maxr));
	PX_Object_LabelSetText(pTune->label_maxr, content);

	ivalue = (px_int)(pTune->key_param.amprl * 100);
	PX_Object_SliderBarSetValue(pTune->sliderbar_amprl, ivalue);
	PX_sprintf1(content, sizeof(content), "radius left sigmoidal shape:%1.2", PX_STRINGFORMAT_FLOAT(pTune->key_param.amprl));
	PX_Object_LabelSetText(pTune->label_amprl, content);

	ivalue = (px_int)(pTune->key_param.amprr * 100);
	PX_Object_SliderBarSetValue(pTune->sliderbar_amprr, ivalue);
	PX_sprintf1(content, sizeof(content), "radius right sigmoidal shape:%1.2", PX_STRINGFORMAT_FLOAT(pTune->key_param.amprr));
	PX_Object_LabelSetText(pTune->label_amprr, content);

	ivalue = (px_int)(pTune->key_param.mult_radius_core_string * 1000);
	PX_Object_SliderBarSetValue(pTune->sliderbar_mult_radius_core_string, ivalue);
	PX_sprintf1(content, sizeof(content), "cord nucleus radio:%1.2", PX_STRINGFORMAT_FLOAT(pTune->key_param.mult_radius_core_string));
	PX_Object_LabelSetText(pTune->label_mult_radius_core_string, content);

	ivalue = (px_int)(pTune->key_param.minL * 100);
	PX_Object_SliderBarSetValue(pTune->sliderbar_minL, ivalue);
	PX_sprintf1(content, sizeof(content), "minimum string length:%1.2", PX_STRINGFORMAT_FLOAT(pTune->key_param.minL));
	PX_Object_LabelSetText(pTune->label_minL, content);

	ivalue = (px_int)(pTune->key_param.maxL * 100);
	PX_Object_SliderBarSetValue(pTune->sliderbar_maxL, ivalue);
	PX_sprintf1(content, sizeof(content), "maximum string length:%1.2", PX_STRINGFORMAT_FLOAT(pTune->key_param.maxL));
	PX_Object_LabelSetText(pTune->label_maxL, content);

	ivalue =-(px_int)(pTune->key_param.ampLl * 100);
	PX_Object_SliderBarSetValue(pTune->sliderbar_ampLl, ivalue);
	PX_sprintf1(content, sizeof(content), "length left sigmoidal shape:%1.2", PX_STRINGFORMAT_FLOAT(pTune->key_param.ampLl));
	PX_Object_LabelSetText(pTune->label_ampLl, content);

	ivalue = (px_int)(pTune->key_param.ampLr * 100);
	PX_Object_SliderBarSetValue(pTune->sliderbar_ampLr, ivalue);
	PX_sprintf1(content, sizeof(content), "length right sigmoidal shape:%1.2", PX_STRINGFORMAT_FLOAT(pTune->key_param.ampLr));
	PX_Object_LabelSetText(pTune->label_ampLr, content);

	ivalue = (px_int)(pTune->key_param.mult_density_string * 100);
	PX_Object_SliderBarSetValue(pTune->sliderbar_mult_density_string, ivalue);
	PX_sprintf1(content, sizeof(content), "string density:%1.2", PX_STRINGFORMAT_FLOAT(pTune->key_param.mult_density_string));
	PX_Object_LabelSetText(pTune->label_mult_density_string, content);

	ivalue = (px_int)(pTune->key_param.mult_modulus_string * 100);
	PX_Object_SliderBarSetValue(pTune->sliderbar_mult_modulus_string, ivalue);
	PX_sprintf1(content, sizeof(content), "young modulus:%1.2", PX_STRINGFORMAT_FLOAT(pTune->key_param.mult_modulus_string));
	PX_Object_LabelSetText(pTune->label_mult_modulus_string, content);

	ivalue = (px_int)(pTune->key_param.mult_impedance_bridge * 100);
	PX_Object_SliderBarSetValue(pTune->sliderbar_mult_impedance_bridge, ivalue);
	PX_sprintf1(content, sizeof(content), "bridge impedance:%1.2", PX_STRINGFORMAT_FLOAT(pTune->key_param.mult_impedance_bridge));
	PX_Object_LabelSetText(pTune->label_mult_impedance_bridge, content);

	ivalue = (px_int)(pTune->key_param.mult_impedance_hammer * 100);
	PX_Object_SliderBarSetValue(pTune->sliderbar_mult_impedance_hammer, ivalue);
	PX_sprintf1(content, sizeof(content), "hammer impedance:%1.2", PX_STRINGFORMAT_FLOAT(pTune->key_param.mult_impedance_hammer));
	PX_Object_LabelSetText(pTune->label_mult_impedance_hammer, content);


	ivalue = (px_int)(pTune->key_param.mult_mass_hammer * 100);
	PX_Object_SliderBarSetValue(pTune->sliderbar_mult_mass_hammer, ivalue);
	PX_sprintf1(content, sizeof(content), "mass hammer:%1.2", PX_STRINGFORMAT_FLOAT(pTune->key_param.mult_mass_hammer));
	PX_Object_LabelSetText(pTune->label_mult_mass_hammer, content);

	ivalue = (px_int)(pTune->key_param.mult_force_hammer * 100);
	PX_Object_SliderBarSetValue(pTune->sliderbar_mult_force_hammer, ivalue);
	PX_sprintf1(content, sizeof(content), "force hammer:%1.2", PX_STRINGFORMAT_FLOAT(pTune->key_param.mult_force_hammer));
	PX_Object_LabelSetText(pTune->label_mult_force_hammer, content);

	ivalue = (px_int)(pTune->key_param.mult_hysteresis_hammer * 100);
	PX_Object_SliderBarSetValue(pTune->sliderbar_mult_hysteresis_hammer, ivalue);
	PX_sprintf1(content, sizeof(content), "hysteresys hammer:%1.2", PX_STRINGFORMAT_FLOAT(pTune->key_param.mult_hysteresis_hammer));
	PX_Object_LabelSetText(pTune->label_mult_hysteresis_hammer, content);

	ivalue = (px_int)(pTune->key_param.mult_stiffness_exponent_hammer * 100);
	PX_Object_SliderBarSetValue(pTune->sliderbar_mult_stiffness_exponent_hammer, ivalue);
	PX_sprintf1(content, sizeof(content), "stiffness exponent hammer:%1.2", PX_STRINGFORMAT_FLOAT(pTune->key_param.mult_stiffness_exponent_hammer));
	PX_Object_LabelSetText(pTune->label_mult_stiffness_exponent_hammer, content);

	ivalue = (px_int)(pTune->key_param.position_hammer * 1000);
	PX_Object_SliderBarSetValue(pTune->sliderbar_position_hammer, ivalue);
	PX_sprintf1(content, sizeof(content), "position hammer:%1.2", PX_STRINGFORMAT_FLOAT(pTune->key_param.position_hammer));
	PX_Object_LabelSetText(pTune->label_position_hammer, content);

	ivalue = (px_int)(pTune->key_param.mult_loss_filter * 100);
	PX_Object_SliderBarSetValue(pTune->sliderbar_mult_loss_filter, ivalue);
	PX_sprintf1(content, sizeof(content), "string loss factor:%1.2", PX_STRINGFORMAT_FLOAT(pTune->key_param.mult_loss_filter));
	PX_Object_LabelSetText(pTune->label_mult_loss_filter, content);

	ivalue = (px_int)(pTune->key_param.detune * 1000000);
	PX_Object_SliderBarSetValue(pTune->sliderbar_detune, ivalue);
	PX_sprintf1(content, sizeof(content), "detuning among 3 strings:%1.6", PX_STRINGFORMAT_FLOAT(pTune->key_param.detune));
	PX_Object_LabelSetText(pTune->label_detune, content);

	ivalue = pTune->key_param.hammer_type;
	PX_Object_SelectBarSetCurrentIndex(pTune->selectbox_hammer_type, ivalue);
	switch (ivalue)
	{
	case 0:
		PX_Object_LabelSetText(pTune->label_hammer_type, "Stulov Hammer");
		break;
	default:
		PX_Object_LabelSetText(pTune->label_hammer_type, "Banks Hammer");
		break;
	}

	ivalue = (px_int)(pTune->soundboard_param.eq1);
	PX_Object_SliderBarSetValue(pTune->sliderbar_eq1, ivalue);
	PX_sprintf1(content, sizeof(content), "global soundboard eq1:%1", PX_STRINGFORMAT_INT((px_int)pTune->soundboard_param.eq1));
	PX_Object_LabelSetText(pTune->label_eq1, content);

	ivalue = (px_int)(pTune->soundboard_param.eq2);
	PX_Object_SliderBarSetValue(pTune->sliderbar_eq2, ivalue);
	PX_sprintf1(content, sizeof(content), "global soundboard eq2:%1", PX_STRINGFORMAT_INT((px_int)pTune->soundboard_param.eq2));
	PX_Object_LabelSetText(pTune->label_eq2, content);

	ivalue = (px_int)(pTune->soundboard_param.eq3);
	PX_Object_SliderBarSetValue(pTune->sliderbar_eq3, ivalue);
	PX_sprintf1(content, sizeof(content), "global soundboard eq3:%1", PX_STRINGFORMAT_INT((px_int)pTune->soundboard_param.eq3));
	PX_Object_LabelSetText(pTune->label_eq3, content);

	ivalue = (px_int)(pTune->soundboard_param.c1);
	PX_Object_SliderBarSetValue(pTune->sliderbar_c1, ivalue);
	PX_sprintf1(content, sizeof(content), "global soundboard c1:%1", PX_STRINGFORMAT_INT((px_int)pTune->soundboard_param.c1));
	PX_Object_LabelSetText(pTune->label_c1, content);

	ivalue = (px_int)(pTune->soundboard_param.c3);
	PX_Object_SliderBarSetValue(pTune->sliderbar_c3, ivalue);
	PX_sprintf1(content, sizeof(content), "global soundboard c3:%1", PX_STRINGFORMAT_INT((px_int)pTune->soundboard_param.c3));
	PX_Object_LabelSetText(pTune->label_c3, content);
	
}


PX_OBJECT_EVENT_FUNCTION(PX_Object_PianoTune_fOnChanged)
{
	px_float fvalue;
	PX_Object_PianoTune* pTune = (PX_Object_PianoTune*)PX_ObjectGetDescByType((PX_Object*)ptr, PX_OBJECT_TYPE_PIANOTUNE);
	fvalue = PX_Object_SliderBarGetValue(pObject) / 100.f;
	pTune->key_param.f = fvalue;
	PX_Object_PianoTune_LocalToUI((PX_Object*)ptr);
	PX_ObjectExecuteEvent((PX_Object*)ptr, PX_OBJECT_BUILD_EVENT(PX_OBJECT_EVENT_VALUECHANGED));
}

PX_OBJECT_EVENT_FUNCTION(PX_Object_PianoTune_weightOnChanged)
{
	px_float fvalue;
	PX_Object_PianoTune* pTune = (PX_Object_PianoTune*)PX_ObjectGetDescByType((PX_Object*)ptr, PX_OBJECT_TYPE_PIANOTUNE);
	fvalue = PX_Object_SliderBarGetValue(pObject) / 100.f;
	pTune->key_param.weight = fvalue;
	PX_Object_PianoTune_LocalToUI((PX_Object*)ptr);
	PX_ObjectExecuteEvent((PX_Object*)ptr, PX_OBJECT_BUILD_EVENT(PX_OBJECT_EVENT_VALUECHANGED));
}

PX_OBJECT_EVENT_FUNCTION(PX_Object_PianoTune_minrOnChanged)
{
	px_float fvalue;
	PX_Object_PianoTune* pTune = (PX_Object_PianoTune*)PX_ObjectGetDescByType((PX_Object*)ptr, PX_OBJECT_TYPE_PIANOTUNE);
	fvalue = PX_Object_SliderBarGetValue(pObject) / 1000.f;
	pTune->key_param.minr = fvalue;
	PX_Object_PianoTune_LocalToUI((PX_Object*)ptr);
	PX_ObjectExecuteEvent((PX_Object*)ptr, PX_OBJECT_BUILD_EVENT(PX_OBJECT_EVENT_VALUECHANGED));
}

PX_OBJECT_EVENT_FUNCTION(PX_Object_PianoTune_maxrOnChanged)
{
	px_float fvalue;
	PX_Object_PianoTune* pTune = (PX_Object_PianoTune*)PX_ObjectGetDescByType((PX_Object*)ptr, PX_OBJECT_TYPE_PIANOTUNE);
	fvalue = PX_Object_SliderBarGetValue(pObject) / 100.f;
	pTune->key_param.maxr = fvalue;
	PX_Object_PianoTune_LocalToUI((PX_Object*)ptr);
	PX_ObjectExecuteEvent((PX_Object*)ptr, PX_OBJECT_BUILD_EVENT(PX_OBJECT_EVENT_VALUECHANGED));
}

PX_OBJECT_EVENT_FUNCTION(PX_Object_PianoTune_amprlOnChanged)
{
	px_float fvalue;
	PX_Object_PianoTune* pTune = (PX_Object_PianoTune*)PX_ObjectGetDescByType((PX_Object*)ptr, PX_OBJECT_TYPE_PIANOTUNE);
	fvalue = PX_Object_SliderBarGetValue(pObject) / 100.f;
	pTune->key_param.amprl = fvalue;
	PX_Object_PianoTune_LocalToUI((PX_Object*)ptr);
	PX_ObjectExecuteEvent((PX_Object*)ptr, PX_OBJECT_BUILD_EVENT(PX_OBJECT_EVENT_VALUECHANGED));
}

PX_OBJECT_EVENT_FUNCTION(PX_Object_PianoTune_amprrOnChanged)
{
	px_float fvalue;
	PX_Object_PianoTune* pTune = (PX_Object_PianoTune*)PX_ObjectGetDescByType((PX_Object*)ptr, PX_OBJECT_TYPE_PIANOTUNE);
	fvalue = PX_Object_SliderBarGetValue(pObject) / 100.f;
	pTune->key_param.amprr = fvalue;
	PX_Object_PianoTune_LocalToUI((PX_Object*)ptr);
	PX_ObjectExecuteEvent((PX_Object*)ptr, PX_OBJECT_BUILD_EVENT(PX_OBJECT_EVENT_VALUECHANGED));
}

PX_OBJECT_EVENT_FUNCTION(PX_Object_PianoTune_mult_radius_core_stringOnChanged)
{
	px_float fvalue;
	PX_Object_PianoTune* pTune = (PX_Object_PianoTune*)PX_ObjectGetDescByType((PX_Object*)ptr, PX_OBJECT_TYPE_PIANOTUNE);
	fvalue = PX_Object_SliderBarGetValue(pObject) / 1000.f;
	pTune->key_param.mult_radius_core_string = fvalue;
	PX_Object_PianoTune_LocalToUI((PX_Object*)ptr);
	PX_ObjectExecuteEvent((PX_Object*)ptr, PX_OBJECT_BUILD_EVENT(PX_OBJECT_EVENT_VALUECHANGED));
}

PX_OBJECT_EVENT_FUNCTION(PX_Object_PianoTune_minLOnChanged)
{
	px_float fvalue;
	PX_Object_PianoTune* pTune = (PX_Object_PianoTune*)PX_ObjectGetDescByType((PX_Object*)ptr, PX_OBJECT_TYPE_PIANOTUNE);
	fvalue = PX_Object_SliderBarGetValue(pObject) / 100.f;
	pTune->key_param.minL = fvalue;
	PX_Object_PianoTune_LocalToUI((PX_Object*)ptr);
	PX_ObjectExecuteEvent((PX_Object*)ptr, PX_OBJECT_BUILD_EVENT(PX_OBJECT_EVENT_VALUECHANGED));
}

PX_OBJECT_EVENT_FUNCTION(PX_Object_PianoTune_maxLOnChanged)
{
	px_float fvalue;
	PX_Object_PianoTune* pTune = (PX_Object_PianoTune*)PX_ObjectGetDescByType((PX_Object*)ptr, PX_OBJECT_TYPE_PIANOTUNE);
	fvalue = PX_Object_SliderBarGetValue(pObject) / 100.f;
	pTune->key_param.maxL = fvalue;
	PX_Object_PianoTune_LocalToUI((PX_Object*)ptr);
	PX_ObjectExecuteEvent((PX_Object*)ptr, PX_OBJECT_BUILD_EVENT(PX_OBJECT_EVENT_VALUECHANGED));
}

PX_OBJECT_EVENT_FUNCTION(PX_Object_PianoTune_ampLlOnChanged)
{
	px_float fvalue;
	PX_Object_PianoTune* pTune = (PX_Object_PianoTune*)PX_ObjectGetDescByType((PX_Object*)ptr, PX_OBJECT_TYPE_PIANOTUNE);
	fvalue = -PX_Object_SliderBarGetValue(pObject) / 100.f;
	pTune->key_param.ampLl = fvalue;
	PX_Object_PianoTune_LocalToUI((PX_Object*)ptr);
	PX_ObjectExecuteEvent((PX_Object*)ptr, PX_OBJECT_BUILD_EVENT(PX_OBJECT_EVENT_VALUECHANGED));
}

PX_OBJECT_EVENT_FUNCTION(PX_Object_PianoTune_ampLrOnChanged)
{
	px_float fvalue;
	PX_Object_PianoTune* pTune = (PX_Object_PianoTune*)PX_ObjectGetDescByType((PX_Object*)ptr, PX_OBJECT_TYPE_PIANOTUNE);
	fvalue = PX_Object_SliderBarGetValue(pObject) / 100.f;
	pTune->key_param.ampLr = fvalue;
	PX_Object_PianoTune_LocalToUI((PX_Object*)ptr);
	PX_ObjectExecuteEvent((PX_Object*)ptr, PX_OBJECT_BUILD_EVENT(PX_OBJECT_EVENT_VALUECHANGED));
}

PX_OBJECT_EVENT_FUNCTION(PX_Object_PianoTune_mult_density_stringOnChanged)
{
	px_float fvalue;
	PX_Object_PianoTune* pTune = (PX_Object_PianoTune*)PX_ObjectGetDescByType((PX_Object*)ptr, PX_OBJECT_TYPE_PIANOTUNE);
	fvalue = PX_Object_SliderBarGetValue(pObject) / 100.f;
	pTune->key_param.mult_density_string = fvalue;
	PX_Object_PianoTune_LocalToUI((PX_Object*)ptr);
	PX_ObjectExecuteEvent((PX_Object*)ptr, PX_OBJECT_BUILD_EVENT(PX_OBJECT_EVENT_VALUECHANGED));
}

PX_OBJECT_EVENT_FUNCTION(PX_Object_PianoTune_mult_modulus_stringOnChanged)
{
	px_float fvalue;
	PX_Object_PianoTune* pTune = (PX_Object_PianoTune*)PX_ObjectGetDescByType((PX_Object*)ptr, PX_OBJECT_TYPE_PIANOTUNE);
	fvalue = PX_Object_SliderBarGetValue(pObject) / 100.f;
	pTune->key_param.mult_modulus_string = fvalue;
	PX_Object_PianoTune_LocalToUI((PX_Object*)ptr);
	PX_ObjectExecuteEvent((PX_Object*)ptr, PX_OBJECT_BUILD_EVENT(PX_OBJECT_EVENT_VALUECHANGED));
}


PX_OBJECT_EVENT_FUNCTION(PX_Object_PianoTune_mult_impedance_bridgeOnChanged)
{
	px_float fvalue;
	PX_Object_PianoTune* pTune = (PX_Object_PianoTune*)PX_ObjectGetDescByType((PX_Object*)ptr, PX_OBJECT_TYPE_PIANOTUNE);
	fvalue = PX_Object_SliderBarGetValue(pObject) / 100.f;
	pTune->key_param.mult_impedance_bridge = fvalue;
	PX_Object_PianoTune_LocalToUI((PX_Object*)ptr);
	PX_ObjectExecuteEvent((PX_Object*)ptr, PX_OBJECT_BUILD_EVENT(PX_OBJECT_EVENT_VALUECHANGED));
}

PX_OBJECT_EVENT_FUNCTION(PX_Object_PianoTune_mult_impedance_hammerOnChanged)
{
	px_float fvalue;
	PX_Object_PianoTune* pTune = (PX_Object_PianoTune*)PX_ObjectGetDescByType((PX_Object*)ptr, PX_OBJECT_TYPE_PIANOTUNE);
	fvalue = PX_Object_SliderBarGetValue(pObject) / 100.f;
	pTune->key_param.mult_impedance_hammer = fvalue;
	PX_Object_PianoTune_LocalToUI((PX_Object*)ptr);
	PX_ObjectExecuteEvent((PX_Object*)ptr, PX_OBJECT_BUILD_EVENT(PX_OBJECT_EVENT_VALUECHANGED));
}

PX_OBJECT_EVENT_FUNCTION(PX_Object_PianoTune_mult_mass_hammerOnChanged)
{
	px_float fvalue;
	PX_Object_PianoTune* pTune = (PX_Object_PianoTune*)PX_ObjectGetDescByType((PX_Object*)ptr, PX_OBJECT_TYPE_PIANOTUNE);
	fvalue = PX_Object_SliderBarGetValue(pObject) / 100.f;
	pTune->key_param.mult_mass_hammer = fvalue;
	PX_Object_PianoTune_LocalToUI((PX_Object*)ptr);
	PX_ObjectExecuteEvent((PX_Object*)ptr, PX_OBJECT_BUILD_EVENT(PX_OBJECT_EVENT_VALUECHANGED));
}

PX_OBJECT_EVENT_FUNCTION(PX_Object_PianoTune_mult_force_hammerOnChanged)
{
	px_float fvalue;
	PX_Object_PianoTune* pTune = (PX_Object_PianoTune*)PX_ObjectGetDescByType((PX_Object*)ptr, PX_OBJECT_TYPE_PIANOTUNE);
	fvalue = PX_Object_SliderBarGetValue(pObject) / 100.f;
	pTune->key_param.mult_force_hammer = fvalue;
	PX_Object_PianoTune_LocalToUI((PX_Object*)ptr);
	PX_ObjectExecuteEvent((PX_Object*)ptr, PX_OBJECT_BUILD_EVENT(PX_OBJECT_EVENT_VALUECHANGED));
}

PX_OBJECT_EVENT_FUNCTION(PX_Object_PianoTune_mult_hysteresis_hammerOnChanged)
{
	px_float fvalue;
	PX_Object_PianoTune* pTune = (PX_Object_PianoTune*)PX_ObjectGetDescByType((PX_Object*)ptr, PX_OBJECT_TYPE_PIANOTUNE);
	fvalue = PX_Object_SliderBarGetValue(pObject) / 100.f;
	pTune->key_param.mult_hysteresis_hammer = fvalue;
	PX_Object_PianoTune_LocalToUI((PX_Object*)ptr);
	PX_ObjectExecuteEvent((PX_Object*)ptr, PX_OBJECT_BUILD_EVENT(PX_OBJECT_EVENT_VALUECHANGED));
}

PX_OBJECT_EVENT_FUNCTION(PX_Object_PianoTune_mult_stiffness_exponent_hammerOnChanged)
{
	px_float fvalue;
	PX_Object_PianoTune* pTune = (PX_Object_PianoTune*)PX_ObjectGetDescByType((PX_Object*)ptr, PX_OBJECT_TYPE_PIANOTUNE);
	fvalue = PX_Object_SliderBarGetValue(pObject) / 100.f;
	pTune->key_param.mult_stiffness_exponent_hammer = fvalue;
	PX_Object_PianoTune_LocalToUI((PX_Object*)ptr);
	PX_ObjectExecuteEvent((PX_Object*)ptr, PX_OBJECT_BUILD_EVENT(PX_OBJECT_EVENT_VALUECHANGED));
}


PX_OBJECT_EVENT_FUNCTION(PX_Object_PianoTune_position_hammerOnChanged)
{
	px_float fvalue;
	PX_Object_PianoTune* pTune = (PX_Object_PianoTune*)PX_ObjectGetDescByType((PX_Object*)ptr, PX_OBJECT_TYPE_PIANOTUNE);
	fvalue = PX_Object_SliderBarGetValue(pObject) / 1000.f;
	pTune->key_param.position_hammer = fvalue;
	PX_Object_PianoTune_LocalToUI((PX_Object*)ptr);
	PX_ObjectExecuteEvent((PX_Object*)ptr, PX_OBJECT_BUILD_EVENT(PX_OBJECT_EVENT_VALUECHANGED));
}

PX_OBJECT_EVENT_FUNCTION(PX_Object_PianoTune_mult_loss_filterOnChanged)
{
	px_float fvalue;
	PX_Object_PianoTune* pTune = (PX_Object_PianoTune*)PX_ObjectGetDescByType((PX_Object*)ptr, PX_OBJECT_TYPE_PIANOTUNE);
	fvalue = PX_Object_SliderBarGetValue(pObject) / 100.f;
	pTune->key_param.mult_loss_filter = fvalue;
	PX_Object_PianoTune_LocalToUI((PX_Object*)ptr);
	PX_ObjectExecuteEvent((PX_Object*)ptr, PX_OBJECT_BUILD_EVENT(PX_OBJECT_EVENT_VALUECHANGED));
}

PX_OBJECT_EVENT_FUNCTION(PX_Object_PianoTune_detuneOnChanged)
{
	px_float fvalue;
	PX_Object_PianoTune* pTune = (PX_Object_PianoTune*)PX_ObjectGetDescByType((PX_Object*)ptr, PX_OBJECT_TYPE_PIANOTUNE);
	fvalue = PX_Object_SliderBarGetValue(pObject) / 1000000.f;
	pTune->key_param.detune = fvalue;
	PX_Object_PianoTune_LocalToUI((PX_Object*)ptr);
	PX_ObjectExecuteEvent((PX_Object*)ptr, PX_OBJECT_BUILD_EVENT(PX_OBJECT_EVENT_VALUECHANGED));
}

PX_OBJECT_EVENT_FUNCTION(PX_Object_PianoTune_hammer_typeOnChanged)
{
	
	PX_Object_PianoTune* pTune = (PX_Object_PianoTune*)PX_ObjectGetDescByType((PX_Object*)ptr, PX_OBJECT_TYPE_PIANOTUNE);
	pTune->key_param.hammer_type = PX_Object_SelectBarGetCurrentIndex(pObject);
	PX_Object_PianoTune_LocalToUI((PX_Object*)ptr);
	PX_ObjectExecuteEvent((PX_Object*)ptr, PX_OBJECT_BUILD_EVENT(PX_OBJECT_EVENT_VALUECHANGED));
}

PX_OBJECT_EVENT_FUNCTION(PX_Object_PianoTune_eq1OnChanged)
{
	px_float fvalue;
	PX_Object_PianoTune* pTune = (PX_Object_PianoTune*)PX_ObjectGetDescByType((PX_Object*)ptr, PX_OBJECT_TYPE_PIANOTUNE);
	fvalue = (px_float)PX_Object_SliderBarGetValue(pObject) ;
	pTune->soundboard_param.eq1 = fvalue;
	PX_Object_PianoTune_LocalToUI((PX_Object*)ptr);
	PX_ObjectExecuteEvent((PX_Object*)ptr, PX_OBJECT_BUILD_EVENT(PX_OBJECT_EVENT_VALUECHANGED));
}

PX_OBJECT_EVENT_FUNCTION(PX_Object_PianoTune_eq2OnChanged)
{
	px_float fvalue;
	PX_Object_PianoTune* pTune = (PX_Object_PianoTune*)PX_ObjectGetDescByType((PX_Object*)ptr, PX_OBJECT_TYPE_PIANOTUNE);
	fvalue = (px_float)PX_Object_SliderBarGetValue(pObject);
	pTune->soundboard_param.eq2 = fvalue;
	PX_Object_PianoTune_LocalToUI((PX_Object*)ptr);
	PX_ObjectExecuteEvent((PX_Object*)ptr, PX_OBJECT_BUILD_EVENT(PX_OBJECT_EVENT_VALUECHANGED));
}

PX_OBJECT_EVENT_FUNCTION(PX_Object_PianoTune_eq3OnChanged)
{
	px_float fvalue;
	PX_Object_PianoTune* pTune = (PX_Object_PianoTune*)PX_ObjectGetDescByType((PX_Object*)ptr, PX_OBJECT_TYPE_PIANOTUNE);
	fvalue = (px_float)PX_Object_SliderBarGetValue(pObject);
	pTune->soundboard_param.eq3 = fvalue;
	PX_Object_PianoTune_LocalToUI((PX_Object*)ptr);
	PX_ObjectExecuteEvent((PX_Object*)ptr, PX_OBJECT_BUILD_EVENT(PX_OBJECT_EVENT_VALUECHANGED));
}

PX_OBJECT_EVENT_FUNCTION(PX_Object_PianoTune_c1OnChanged)
{
	px_float fvalue;
	PX_Object_PianoTune* pTune = (PX_Object_PianoTune*)PX_ObjectGetDescByType((PX_Object*)ptr, PX_OBJECT_TYPE_PIANOTUNE);
	fvalue = (px_float)PX_Object_SliderBarGetValue(pObject);
	pTune->soundboard_param.c1 = fvalue;
	PX_Object_PianoTune_LocalToUI((PX_Object*)ptr);
	PX_ObjectExecuteEvent((PX_Object*)ptr, PX_OBJECT_BUILD_EVENT(PX_OBJECT_EVENT_VALUECHANGED));
}

PX_OBJECT_EVENT_FUNCTION(PX_Object_PianoTune_c3OnChanged)
{
	px_float fvalue;
	PX_Object_PianoTune* pTune = (PX_Object_PianoTune*)PX_ObjectGetDescByType((PX_Object*)ptr, PX_OBJECT_TYPE_PIANOTUNE);
	fvalue = (px_float)PX_Object_SliderBarGetValue(pObject);
	pTune->soundboard_param.c3 = fvalue;
	PX_Object_PianoTune_LocalToUI((PX_Object*)ptr);
	PX_ObjectExecuteEvent((PX_Object*)ptr, PX_OBJECT_BUILD_EVENT(PX_OBJECT_EVENT_VALUECHANGED));
}

PX_Object* PX_Object_PianoTuneAttachObject(PX_Object* pObject,px_int attachIndex, px_float x, px_float y, px_float _width, px_float height)
{
	px_memorypool* mp=pObject->mp;
	PX_Object_PianoTune *pTune;

	px_int width = (px_int)_width;

	PX_ASSERTIF(pObject == PX_NULL);
	PX_ASSERTIF(attachIndex < 0 || attachIndex >= PX_COUNTOF(pObject->pObjectDesc));
	PX_ASSERTIF(pObject->pObjectDesc[attachIndex] != PX_NULL);
	pTune = (PX_Object_PianoTune*)PX_ObjectCreateDesc(pObject, attachIndex, PX_OBJECT_TYPE_PIANOTUNE, 0, PX_Object_PianoTuneOnRender, 0, 0, sizeof(PX_Object_PianoTune));
	PX_ASSERTIF(pTune == PX_NULL);

	pTune->scrollArea = PX_Object_ScrollAreaCreate(mp, pObject, 0, 0, (px_int)width, (px_int)height);

	pTune->label_f = PX_Object_LabelCreate(mp, pTune->scrollArea, 0, 0, width, 24, "frequency:", PX_NULL, PX_OBJECT_UI_DEFAULT_FONTCOLOR);
	pTune->sliderbar_f = PX_Object_SliderBarCreate(mp, pTune->scrollArea, 0, 0, width,24,PX_OBJECT_SLIDERBAR_TYPE_HORIZONTAL,PX_OBJECT_SLIDERBAR_STYLE_BOX);
	PX_Object_SliderBarSetRange(pTune->sliderbar_f, 1000, 800000);
	PX_ObjectRegisterEvent(pTune->sliderbar_f, PX_OBJECT_EVENT_VALUECHANGED, PX_Object_PianoTune_fOnChanged, pObject);

	pTune->label_weight = PX_Object_LabelCreate(mp, pTune->scrollArea, 0, 0, width, 24, "Key pulsation velocity weight:", PX_NULL, PX_OBJECT_UI_DEFAULT_FONTCOLOR);
	pTune->sliderbar_weight = PX_Object_SliderBarCreate(mp, pTune->scrollArea, 0, 0, width, 24, PX_OBJECT_SLIDERBAR_TYPE_HORIZONTAL, PX_OBJECT_SLIDERBAR_STYLE_BOX);
	PX_Object_SliderBarSetRange(pTune->sliderbar_weight, 0, 1000);
	PX_ObjectRegisterEvent(pTune->sliderbar_weight, PX_OBJECT_EVENT_VALUECHANGED, PX_Object_PianoTune_weightOnChanged, pObject);

	pTune->label_minr = PX_Object_LabelCreate(mp, pTune->scrollArea, 0, 0, width, 24, "minimum string radio:", PX_NULL, PX_OBJECT_UI_DEFAULT_FONTCOLOR);
	pTune->sliderbar_minr = PX_Object_SliderBarCreate(mp, pTune->scrollArea, 0, 0, width, 24, PX_OBJECT_SLIDERBAR_TYPE_HORIZONTAL, PX_OBJECT_SLIDERBAR_STYLE_BOX);
	PX_Object_SliderBarSetRange(pTune->sliderbar_minr, 0, 1000);//0.00-10.00
	PX_ObjectRegisterEvent(pTune->sliderbar_minr, PX_OBJECT_EVENT_VALUECHANGED, PX_Object_PianoTune_minrOnChanged, pObject);

	pTune->label_maxr = PX_Object_LabelCreate(mp, pTune->scrollArea, 0, 0, width, 24, "maximum string radio:", PX_NULL, PX_OBJECT_UI_DEFAULT_FONTCOLOR);
	pTune->sliderbar_maxr = PX_Object_SliderBarCreate(mp, pTune->scrollArea, 0, 0, width, 24, PX_OBJECT_SLIDERBAR_TYPE_HORIZONTAL, PX_OBJECT_SLIDERBAR_STYLE_BOX);
	PX_Object_SliderBarSetRange(pTune->sliderbar_maxr, 0, 1000);//0.00-10.00
	PX_ObjectRegisterEvent(pTune->sliderbar_maxr, PX_OBJECT_EVENT_VALUECHANGED, PX_Object_PianoTune_maxrOnChanged, pObject);

	pTune->label_amprl = PX_Object_LabelCreate(mp, pTune->scrollArea, 0, 0, width, 24, "radius left sigmoidal shape:", PX_NULL, PX_OBJECT_UI_DEFAULT_FONTCOLOR);
	pTune->sliderbar_amprl = PX_Object_SliderBarCreate(mp, pTune->scrollArea, 0, 0, width, 24, PX_OBJECT_SLIDERBAR_TYPE_HORIZONTAL, PX_OBJECT_SLIDERBAR_STYLE_BOX);
	PX_Object_SliderBarSetRange(pTune->sliderbar_amprl, 0, 1000);//0.00-10.00
	PX_ObjectRegisterEvent(pTune->sliderbar_amprl, PX_OBJECT_EVENT_VALUECHANGED, PX_Object_PianoTune_amprlOnChanged, pObject);

	pTune->label_amprr = PX_Object_LabelCreate(mp, pTune->scrollArea, 0, 0, width, 24, "radius right sigmoidal shape:", PX_NULL, PX_OBJECT_UI_DEFAULT_FONTCOLOR);
	pTune->sliderbar_amprr = PX_Object_SliderBarCreate(mp, pTune->scrollArea, 0, 0, width, 24, PX_OBJECT_SLIDERBAR_TYPE_HORIZONTAL, PX_OBJECT_SLIDERBAR_STYLE_BOX);
	PX_Object_SliderBarSetRange(pTune->sliderbar_amprr, 0, 1000);//0.00-10.00
	PX_ObjectRegisterEvent(pTune->sliderbar_amprr, PX_OBJECT_EVENT_VALUECHANGED, PX_Object_PianoTune_amprrOnChanged, pObject);

	pTune->label_mult_radius_core_string = PX_Object_LabelCreate(mp, pTune->scrollArea, 0, 0, width, 24, "cord nucleus radio:", PX_NULL, PX_OBJECT_UI_DEFAULT_FONTCOLOR);
	pTune->sliderbar_mult_radius_core_string = PX_Object_SliderBarCreate(mp, pTune->scrollArea, 0, 0, width, 24, PX_OBJECT_SLIDERBAR_TYPE_HORIZONTAL, PX_OBJECT_SLIDERBAR_STYLE_BOX);
	PX_Object_SliderBarSetRange(pTune->sliderbar_mult_radius_core_string, 0, 2000);//0.000-2.000
	PX_ObjectRegisterEvent(pTune->sliderbar_mult_radius_core_string, PX_OBJECT_EVENT_VALUECHANGED, PX_Object_PianoTune_mult_radius_core_stringOnChanged, pObject);

	pTune->label_minL = PX_Object_LabelCreate(mp, pTune->scrollArea, 0, 0, width, 24, "minimum string length:", PX_NULL, PX_OBJECT_UI_DEFAULT_FONTCOLOR);
	pTune->sliderbar_minL = PX_Object_SliderBarCreate(mp, pTune->scrollArea, 0, 0, width, 24, PX_OBJECT_SLIDERBAR_TYPE_HORIZONTAL, PX_OBJECT_SLIDERBAR_STYLE_BOX);
	PX_Object_SliderBarSetRange(pTune->sliderbar_minL, 0, 1000);//0.000-0.1000
	PX_ObjectRegisterEvent(pTune->sliderbar_minL, PX_OBJECT_EVENT_VALUECHANGED, PX_Object_PianoTune_minLOnChanged, pObject);

	pTune->label_maxL = PX_Object_LabelCreate(mp, pTune->scrollArea, 0, 0, width, 24, "maximum string length.:", PX_NULL, PX_OBJECT_UI_DEFAULT_FONTCOLOR);
	pTune->sliderbar_maxL = PX_Object_SliderBarCreate(mp, pTune->scrollArea, 0, 0, width, 24, PX_OBJECT_SLIDERBAR_TYPE_HORIZONTAL, PX_OBJECT_SLIDERBAR_STYLE_BOX);
	PX_Object_SliderBarSetRange(pTune->sliderbar_maxL, 0, 1000);//00.00-10.00
	PX_ObjectRegisterEvent(pTune->sliderbar_maxL, PX_OBJECT_EVENT_VALUECHANGED, PX_Object_PianoTune_maxLOnChanged, pObject);

	pTune->label_ampLl = PX_Object_LabelCreate(mp, pTune->scrollArea, 0, 0, width, 24, "length left sigmoidal shape:", PX_NULL, PX_OBJECT_UI_DEFAULT_FONTCOLOR);
	pTune->sliderbar_ampLl = PX_Object_SliderBarCreate(mp, pTune->scrollArea, 0, 0, width, 24, PX_OBJECT_SLIDERBAR_TYPE_HORIZONTAL, PX_OBJECT_SLIDERBAR_STYLE_BOX);
	PX_Object_SliderBarSetRange(pTune->sliderbar_ampLl, 0, 1000);//-10-0
	PX_ObjectRegisterEvent(pTune->sliderbar_ampLl, PX_OBJECT_EVENT_VALUECHANGED, PX_Object_PianoTune_ampLlOnChanged, pObject);

	pTune->label_ampLr = PX_Object_LabelCreate(mp, pTune->scrollArea, 0, 0, width, 24, "length right sigmoidal shape:", PX_NULL, PX_OBJECT_UI_DEFAULT_FONTCOLOR);
	pTune->sliderbar_ampLr = PX_Object_SliderBarCreate(mp, pTune->scrollArea, 0, 0, width, 24, PX_OBJECT_SLIDERBAR_TYPE_HORIZONTAL, PX_OBJECT_SLIDERBAR_STYLE_BOX);
	PX_Object_SliderBarSetRange(pTune->sliderbar_ampLr, 0, 1000);//0-10
	PX_ObjectRegisterEvent(pTune->sliderbar_ampLr, PX_OBJECT_EVENT_VALUECHANGED, PX_Object_PianoTune_ampLrOnChanged, pObject);

	pTune->label_mult_density_string = PX_Object_LabelCreate(mp, pTune->scrollArea, 0, 0, width, 24, "string density:", PX_NULL, PX_OBJECT_UI_DEFAULT_FONTCOLOR);
	pTune->sliderbar_mult_density_string = PX_Object_SliderBarCreate(mp, pTune->scrollArea, 0, 0, width, 24, PX_OBJECT_SLIDERBAR_TYPE_HORIZONTAL, PX_OBJECT_SLIDERBAR_STYLE_BOX);
	PX_Object_SliderBarSetRange(pTune->sliderbar_mult_density_string, 0, 1000);//00.00-10.00
	PX_ObjectRegisterEvent(pTune->sliderbar_mult_density_string, PX_OBJECT_EVENT_VALUECHANGED, PX_Object_PianoTune_mult_density_stringOnChanged, pObject);

	pTune->label_mult_modulus_string = PX_Object_LabelCreate(mp, pTune->scrollArea, 0, 0, width, 24, "young modulus:", PX_NULL, PX_OBJECT_UI_DEFAULT_FONTCOLOR);
	pTune->sliderbar_mult_modulus_string = PX_Object_SliderBarCreate(mp, pTune->scrollArea, 0, 0, width, 24, PX_OBJECT_SLIDERBAR_TYPE_HORIZONTAL, PX_OBJECT_SLIDERBAR_STYLE_BOX);
	PX_Object_SliderBarSetRange(pTune->sliderbar_mult_modulus_string, 0, 1000);//00.00-10.00
	PX_ObjectRegisterEvent(pTune->sliderbar_mult_modulus_string, PX_OBJECT_EVENT_VALUECHANGED, PX_Object_PianoTune_mult_modulus_stringOnChanged, pObject);

	pTune->label_mult_impedance_bridge = PX_Object_LabelCreate(mp, pTune->scrollArea, 0, 0, width, 24, "bridge impedance:", PX_NULL, PX_OBJECT_UI_DEFAULT_FONTCOLOR);
	pTune->sliderbar_mult_impedance_bridge = PX_Object_SliderBarCreate(mp, pTune->scrollArea, 0, 0, width, 24, PX_OBJECT_SLIDERBAR_TYPE_HORIZONTAL, PX_OBJECT_SLIDERBAR_STYLE_BOX);
	PX_Object_SliderBarSetRange(pTune->sliderbar_mult_impedance_bridge, 0, 1000);//00.00-10.00
	PX_ObjectRegisterEvent(pTune->sliderbar_mult_impedance_bridge, PX_OBJECT_EVENT_VALUECHANGED, PX_Object_PianoTune_mult_impedance_bridgeOnChanged, pObject);

	pTune->label_mult_impedance_hammer = PX_Object_LabelCreate(mp, pTune->scrollArea, 0, 0, width, 24, "hammer impedance:", PX_NULL, PX_OBJECT_UI_DEFAULT_FONTCOLOR);
	pTune->sliderbar_mult_impedance_hammer = PX_Object_SliderBarCreate(mp, pTune->scrollArea, 0, 0, width, 24, PX_OBJECT_SLIDERBAR_TYPE_HORIZONTAL, PX_OBJECT_SLIDERBAR_STYLE_BOX);
	PX_Object_SliderBarSetRange(pTune->sliderbar_mult_impedance_hammer, 0, 1000);//00.00-10.00
	PX_ObjectRegisterEvent(pTune->sliderbar_mult_impedance_hammer, PX_OBJECT_EVENT_VALUECHANGED, PX_Object_PianoTune_mult_impedance_hammerOnChanged, pObject);

	pTune->label_mult_mass_hammer = PX_Object_LabelCreate(mp, pTune->scrollArea, 0, 0, width, 24, "mass hammer:", PX_NULL, PX_OBJECT_UI_DEFAULT_FONTCOLOR);
	pTune->sliderbar_mult_mass_hammer = PX_Object_SliderBarCreate(mp, pTune->scrollArea, 0, 0, width, 24, PX_OBJECT_SLIDERBAR_TYPE_HORIZONTAL, PX_OBJECT_SLIDERBAR_STYLE_BOX);
	PX_Object_SliderBarSetRange(pTune->sliderbar_mult_mass_hammer, 0, 1000);//00.00-10.00
	PX_ObjectRegisterEvent(pTune->sliderbar_mult_mass_hammer, PX_OBJECT_EVENT_VALUECHANGED, PX_Object_PianoTune_mult_mass_hammerOnChanged, pObject);

	pTune->label_mult_force_hammer = PX_Object_LabelCreate(mp, pTune->scrollArea, 0, 0, width, 24, "force hammer:", PX_NULL, PX_OBJECT_UI_DEFAULT_FONTCOLOR);
	pTune->sliderbar_mult_force_hammer = PX_Object_SliderBarCreate(mp, pTune->scrollArea, 0, 0, width, 24, PX_OBJECT_SLIDERBAR_TYPE_HORIZONTAL, PX_OBJECT_SLIDERBAR_STYLE_BOX);
	PX_Object_SliderBarSetRange(pTune->sliderbar_mult_mass_hammer, 0, 1000);//0.000-1.000
	PX_ObjectRegisterEvent(pTune->sliderbar_mult_force_hammer, PX_OBJECT_EVENT_VALUECHANGED, PX_Object_PianoTune_mult_force_hammerOnChanged, pObject);

	pTune->label_mult_hysteresis_hammer = PX_Object_LabelCreate(mp, pTune->scrollArea, 0, 0, width, 24, "hysteresys hammer:", PX_NULL, PX_OBJECT_UI_DEFAULT_FONTCOLOR);
	pTune->sliderbar_mult_hysteresis_hammer = PX_Object_SliderBarCreate(mp, pTune->scrollArea, 0, 0, width, 24, PX_OBJECT_SLIDERBAR_TYPE_HORIZONTAL, PX_OBJECT_SLIDERBAR_STYLE_BOX);
	PX_Object_SliderBarSetRange(pTune->sliderbar_mult_hysteresis_hammer, 0, 1000);//00.00-10.00
	PX_ObjectRegisterEvent(pTune->sliderbar_mult_hysteresis_hammer, PX_OBJECT_EVENT_VALUECHANGED, PX_Object_PianoTune_mult_hysteresis_hammerOnChanged, pObject);

	pTune->label_mult_stiffness_exponent_hammer = PX_Object_LabelCreate(mp, pTune->scrollArea, 0, 0, width, 24, "stiffness exponent hammer:", PX_NULL, PX_OBJECT_UI_DEFAULT_FONTCOLOR);
	pTune->sliderbar_mult_stiffness_exponent_hammer = PX_Object_SliderBarCreate(mp, pTune->scrollArea, 0, 0, width, 24, PX_OBJECT_SLIDERBAR_TYPE_HORIZONTAL, PX_OBJECT_SLIDERBAR_STYLE_BOX);
	PX_Object_SliderBarSetRange(pTune->sliderbar_mult_stiffness_exponent_hammer, 0, 1000);//00.00-10.00
	PX_ObjectRegisterEvent(pTune->sliderbar_mult_stiffness_exponent_hammer, PX_OBJECT_EVENT_VALUECHANGED, PX_Object_PianoTune_mult_stiffness_exponent_hammerOnChanged, pObject);

	pTune->label_position_hammer = PX_Object_LabelCreate(mp, pTune->scrollArea, 0, 0, width, 24, "position hammer:", PX_NULL, PX_OBJECT_UI_DEFAULT_FONTCOLOR);
	pTune->sliderbar_position_hammer = PX_Object_SliderBarCreate(mp, pTune->scrollArea, 0, 0, width, 24, PX_OBJECT_SLIDERBAR_TYPE_HORIZONTAL, PX_OBJECT_SLIDERBAR_STYLE_BOX);
	PX_Object_SliderBarSetRange(pTune->sliderbar_position_hammer, 0, 1000);//00.00-1.000
	PX_ObjectRegisterEvent(pTune->sliderbar_position_hammer, PX_OBJECT_EVENT_VALUECHANGED, PX_Object_PianoTune_position_hammerOnChanged, pObject);


	pTune->label_mult_loss_filter = PX_Object_LabelCreate(mp, pTune->scrollArea, 0, 0, width, 24, "string loss factor:", PX_NULL, PX_OBJECT_UI_DEFAULT_FONTCOLOR);
	pTune->sliderbar_mult_loss_filter = PX_Object_SliderBarCreate(mp, pTune->scrollArea, 0, 0, width, 24, PX_OBJECT_SLIDERBAR_TYPE_HORIZONTAL, PX_OBJECT_SLIDERBAR_STYLE_BOX);
	PX_Object_SliderBarSetRange(pTune->sliderbar_mult_loss_filter, 0, 1000);//0.0003
	PX_ObjectRegisterEvent(pTune->sliderbar_mult_loss_filter, PX_OBJECT_EVENT_VALUECHANGED, PX_Object_PianoTune_mult_loss_filterOnChanged, pObject);

	pTune->label_detune = PX_Object_LabelCreate(mp, pTune->scrollArea, 0, 0, width, 24, "detuning among 3 strings:", PX_NULL, PX_OBJECT_UI_DEFAULT_FONTCOLOR);
	pTune->sliderbar_detune = PX_Object_SliderBarCreate(mp, pTune->scrollArea, 0, 0, width, 24, PX_OBJECT_SLIDERBAR_TYPE_HORIZONTAL, PX_OBJECT_SLIDERBAR_STYLE_BOX);
	PX_Object_SliderBarSetRange(pTune->sliderbar_detune, 0, 1000);//0.00000-0.001000
	PX_ObjectRegisterEvent(pTune->sliderbar_detune, PX_OBJECT_EVENT_VALUECHANGED, PX_Object_PianoTune_detuneOnChanged, pObject);

	pTune->label_hammer_type = PX_Object_LabelCreate(mp, pTune->scrollArea, 0, 0, width, 24, "hammer type:", PX_NULL, PX_OBJECT_UI_DEFAULT_FONTCOLOR);
	pTune->selectbox_hammer_type = PX_Object_SelectBarCreate(mp, pTune->scrollArea, 0, 0, width, 24,PX_NULL);
	PX_Object_SelectBarAddItem(pTune->selectbox_hammer_type, "Stulov Hammer");
	PX_Object_SelectBarAddItem(pTune->selectbox_hammer_type, "Banks Hammer");
	PX_ObjectRegisterEvent(pTune->selectbox_hammer_type, PX_OBJECT_EVENT_VALUECHANGED, PX_Object_PianoTune_hammer_typeOnChanged, pObject);

	pTune->label_eq1 = PX_Object_LabelCreate(mp, pTune->scrollArea, 0, 0, width, 24, "global:soundboard eq1:", PX_NULL, PX_OBJECT_UI_DEFAULT_FONTCOLOR);
	pTune->sliderbar_eq1 = PX_Object_SliderBarCreate(mp, pTune->scrollArea, 0, 0, width, 24, PX_OBJECT_SLIDERBAR_TYPE_HORIZONTAL, PX_OBJECT_SLIDERBAR_STYLE_BOX);
	PX_Object_SliderBarSetRange(pTune->sliderbar_eq1, 0, 1000);
	PX_ObjectRegisterEvent(pTune->sliderbar_eq1, PX_OBJECT_EVENT_VALUECHANGED, PX_Object_PianoTune_eq1OnChanged, pObject);

	pTune->label_eq2 = PX_Object_LabelCreate(mp, pTune->scrollArea, 0, 0, width, 24, "global:soundboard eq2:", PX_NULL, PX_OBJECT_UI_DEFAULT_FONTCOLOR);
	pTune->sliderbar_eq2 = PX_Object_SliderBarCreate(mp, pTune->scrollArea, 0, 0, width, 24, PX_OBJECT_SLIDERBAR_TYPE_HORIZONTAL, PX_OBJECT_SLIDERBAR_STYLE_BOX);
	PX_Object_SliderBarSetRange(pTune->sliderbar_eq2, 0, 1000);
	PX_ObjectRegisterEvent(pTune->sliderbar_eq2, PX_OBJECT_EVENT_VALUECHANGED, PX_Object_PianoTune_eq2OnChanged, pObject);

	pTune->label_eq3 = PX_Object_LabelCreate(mp, pTune->scrollArea, 0, 0, width, 24, "global:soundboard eq3:", PX_NULL, PX_OBJECT_UI_DEFAULT_FONTCOLOR);
	pTune->sliderbar_eq3 = PX_Object_SliderBarCreate(mp, pTune->scrollArea, 0, 0, width, 24, PX_OBJECT_SLIDERBAR_TYPE_HORIZONTAL, PX_OBJECT_SLIDERBAR_STYLE_BOX);
	PX_Object_SliderBarSetRange(pTune->sliderbar_eq3, 0, 1000);
	PX_ObjectRegisterEvent(pTune->sliderbar_eq3, PX_OBJECT_EVENT_VALUECHANGED, PX_Object_PianoTune_eq3OnChanged, pObject);

	pTune->label_c1 = PX_Object_LabelCreate(mp, pTune->scrollArea, 0, 0, width, 24, "global:soundboard c1:", PX_NULL, PX_OBJECT_UI_DEFAULT_FONTCOLOR);
	pTune->sliderbar_c1= PX_Object_SliderBarCreate(mp, pTune->scrollArea, 0, 0, width, 24, PX_OBJECT_SLIDERBAR_TYPE_HORIZONTAL, PX_OBJECT_SLIDERBAR_STYLE_BOX);
	PX_Object_SliderBarSetRange(pTune->sliderbar_c1, 0, 1000);
	PX_ObjectRegisterEvent(pTune->sliderbar_c1, PX_OBJECT_EVENT_VALUECHANGED, PX_Object_PianoTune_c1OnChanged, pObject);

	pTune->label_c3 = PX_Object_LabelCreate(mp, pTune->scrollArea, 0, 0, width, 24, "global:soundboard c3:", PX_NULL, PX_OBJECT_UI_DEFAULT_FONTCOLOR);
	pTune->sliderbar_c3 = PX_Object_SliderBarCreate(mp, pTune->scrollArea, 0, 0, width, 24, PX_OBJECT_SLIDERBAR_TYPE_HORIZONTAL, PX_OBJECT_SLIDERBAR_STYLE_BOX);
	PX_Object_SliderBarSetRange(pTune->sliderbar_c3, 0, 1000);
	PX_ObjectRegisterEvent(pTune->sliderbar_c3, PX_OBJECT_EVENT_VALUECHANGED, PX_Object_PianoTune_c3OnChanged, pObject);
	return pObject;
}

PX_Object* PX_Object_PianoTuneCreate(px_memorypool* mp, PX_Object* Parent, px_float x, px_float y, px_float _width, px_float height)
{
	PX_Object* pObject;
	pObject=PX_ObjectCreate(mp, Parent, x, y, x + _width, y + height, 0,0);
	if (!pObject)
	{
		return PX_NULL;
	}
	return PX_Object_PianoTuneAttachObject(pObject, 0, x, y, _width, height);
}

px_void PX_Object_PianoTuneSetParametersEx(PX_Object* pObject,PX_PianoKey_Parameters param,PX_PianoSoundboard_Parameters sParam)
{
	PX_Object_PianoTune *pTune = (PX_Object_PianoTune*)PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_PIANOTUNE);
	PX_ASSERTIF(pTune==PX_NULL);
	pTune->key_param = param;
	pTune->soundboard_param = sParam;
	PX_Object_PianoTune_LocalToUI(pObject);
}

px_void PX_Object_PianoTuneSetParameters(PX_Object* pObject, PX_Piano*pPiano,px_int keyIndex)
{
	PX_Object_PianoTuneSetParametersEx(pObject, pPiano->keys[keyIndex].param,pPiano->soundboard.param);
}
