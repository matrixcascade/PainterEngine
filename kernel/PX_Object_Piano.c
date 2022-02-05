#include "PX_Object_Piano.h"

px_void PX_Object_PianoRender(px_surface* psurface, PX_Object* pObject, px_dword elapsed)
{
	px_int step=0, i,j=2;
	px_int x,white_key_w,keyw, keyh;

	PX_Object_Piano* pPiano = (PX_Object_Piano*)pObject->pObject;
	const px_char* note_content[] = { "A","#A","B", "C","#C","D","#D","E","F","#F","G","#G", };
	px_float objx, objy, objWidth, objHeight;
	px_float inheritX, inheritY;
	PX_ObjectGetInheritXY(pObject, &inheritX, &inheritY);

	objx = (pObject->x + inheritX);
	objy = (pObject->y + inheritY);
	objWidth = pObject->Width;
	objHeight = pObject->Height;

	keyh = (px_int)objHeight;
	white_key_w = (px_int)objWidth / 52;
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
				keyh = (px_int)objHeight / 2;
				keyw = (px_int)objWidth / 52 *4 / 5;
				x = (px_int)(objx + step * white_key_w - white_key_w*2 / 5);
				color = PX_COLOR_BLACK;
			}
			else
			{
				if (j == 0) 
				{
					step++;
					continue;
				}
				keyh = (px_int)objHeight;
				keyw = (px_int)objWidth / 52;
				x = (px_int)(objx + step * white_key_w);
				color = PX_COLOR_WHITE;
				step++;
			}
			PX_GeoDrawRect(psurface, x, (px_int)objy, x + keyw, (px_int)(objy + keyh), color);
			PX_GeoDrawBorder(psurface,x , (px_int)objy, (px_int)x + keyw, (px_int)(objy + keyh), 1, PX_COLOR_BLACK);

			switch (pPiano->keyState[i])
			{
			case PX_OBJECT_PIANO_KEYSTATE_STANDBY:
				break;
			case PX_OBJECT_PIANO_KEYSTATE_DOWN:
				PX_GeoDrawRect(psurface, x, (px_int)objy, x + keyw, (px_int)(objy + keyh), PX_COLOR(128, 64, 64, 255) );
				break;
			case PX_OBJECT_PIANO_KEYSTATE_CURSOR:
				PX_GeoDrawRect(psurface, x, (px_int)objy, x + keyw, (px_int)(objy + keyh), PX_COLOR(96, 192, 255, 64));
				break;
			}
		}
	}
	
}

px_int PX_Object_PianoCursorInkeyIndex(PX_Object* pObject, px_float cursorx, px_float cursory)
{
	px_int step = 0, x,i, j = 2;
	px_int  white_key_w, keyw, keyh;
	px_int lastIndex = -1;
	PX_Object_Piano* pPiano = (PX_Object_Piano*)pObject->pObject;
	const px_char* note_content[] = { "A","#A","B", "C","#C","D","#D","E","F","#F","G","#G", };
	px_float objx, objy, objWidth, objHeight;
	px_float inheritX, inheritY;
	PX_ObjectGetInheritXY(pObject, &inheritX, &inheritY);

	objx = (pObject->x + inheritX);
	objy = (pObject->y + inheritY);
	objWidth = pObject->Width;
	objHeight = pObject->Height;

	keyh = (px_int)objHeight;
	white_key_w = (px_int)objWidth / 52;

	while (j--)
	{
		step = 0;
		for (i = 0; i < 88; i++)
		{
			if (note_content[i % 12][0] == '#')
			{
				if (j == 1)continue;
				keyh = (px_int)objHeight / 2;
				keyw = (px_int)objWidth / 52 * 4 / 5;
				x = (px_int)(objx + step * white_key_w - white_key_w * 2 / 5);
			}
			else
			{
				if (j == 0)
				{
					step++;
					continue;
				}
				keyh = (px_int)objHeight;
				keyw = (px_int)objWidth / 52;
				x = (px_int)(objx + step * white_key_w);
				step++;
			}
			if (PX_isXYInRegion(cursorx, cursory,(px_float)x,objy,(px_float)keyw, (px_float)keyh))
			{
				lastIndex = i;
			}
		}
	}
	return lastIndex;
}

px_void PX_Object_PianoOnCursorMove(PX_Object* pObject, PX_Object_Event e, px_void* ptr)
{
	px_int i;
	px_int index;
	PX_Object_Piano* pPiano = PX_ObjectGetDesc(PX_Object_Piano, pObject);

	for (i = 0; i < 88; i++)
	{
		pPiano->keyState[i] = PX_OBJECT_PIANO_KEYSTATE_STANDBY;
	}
	index = PX_Object_PianoCursorInkeyIndex(pObject, PX_Object_Event_GetCursorX(e), PX_Object_Event_GetCursorY(e));
	if (index!=-1)
	{
		pPiano->keyState[index] =PX_OBJECT_PIANO_KEYSTATE_CURSOR;
	}

}

px_void PX_Object_PianoOnCursorUp(PX_Object* pObject, PX_Object_Event e, px_void* ptr)
{
	px_int i;
	px_int index;
	PX_Object_Piano* pPiano = PX_ObjectGetDesc(PX_Object_Piano, pObject);

	for (i = 0; i < 88; i++)
	{
		pPiano->keyState[i] = PX_OBJECT_PIANO_KEYSTATE_STANDBY;
	}
	index = PX_Object_PianoCursorInkeyIndex(pObject, PX_Object_Event_GetCursorX(e), PX_Object_Event_GetCursorY(e));
	if (index != -1)
	{
		pPiano->keyState[index] = PX_OBJECT_PIANO_KEYSTATE_CURSOR;
	}

}

px_void PX_Object_PianoOnCursorDown(PX_Object* pObject, PX_Object_Event e, px_void* ptr)
{
	px_int index;
	PX_Object_Piano* pPiano = PX_ObjectGetDesc(PX_Object_Piano, pObject);
	if (!PX_ObjectIsCursorInRegion(pObject, e))
	{
		return;
	}
	index = PX_Object_PianoCursorInkeyIndex(pObject, PX_Object_Event_GetCursorX(e), PX_Object_Event_GetCursorY(e));
	if (index != -1)
	{
		pPiano->keyState[index] = PX_OBJECT_PIANO_KEYSTATE_DOWN;
		pPiano->lastKeyDownIndex = index;
		PX_ObjectPostEvent(pObject, PX_OBJECT_BUILD_EVENT_INT(PX_OBJECT_EVENT_VALUECHANGED, index));
	}
}

px_void PX_Object_PianoOnCursorDrag(PX_Object* pObject, PX_Object_Event e, px_void* ptr)
{
	px_int i;
	px_int index;
	PX_Object_Piano* pPiano = PX_ObjectGetDesc(PX_Object_Piano, pObject);
	for (i = 0; i < 88; i++)
	{
		pPiano->keyState[i] = PX_OBJECT_PIANO_KEYSTATE_STANDBY;
	}
	index = PX_Object_PianoCursorInkeyIndex(pObject, PX_Object_Event_GetCursorX(e), PX_Object_Event_GetCursorY(e));
	if (index != -1)
	{
		pPiano->keyState[index] = PX_OBJECT_PIANO_KEYSTATE_DOWN;
		if (pPiano->lastKeyDownIndex!=index)
		{
			pPiano->lastKeyDownIndex = index;
			PX_ObjectPostEvent(pObject, PX_OBJECT_BUILD_EVENT_INT(PX_OBJECT_EVENT_VALUECHANGED, index));
		}
		
	}
}

PX_Object *PX_Object_PianoCreate(px_memorypool* mp, PX_Object* Parent, px_float x, px_float y, px_float width, px_float height)
{
	PX_Object_Piano Piano;
	PX_Object* pObject;
	PX_zeromemory(&Piano, sizeof(Piano));
	pObject=PX_ObjectCreateEx(mp, Parent, x, y, 0, width, height, 0, PX_OBJECT_TYPE_PIANO, PX_NULL, PX_Object_PianoRender, PX_NULL, &Piano, sizeof(Piano));
	PX_ObjectRegisterEvent(pObject, PX_OBJECT_EVENT_CURSORMOVE, PX_Object_PianoOnCursorMove, PX_NULL);
	PX_ObjectRegisterEvent(pObject, PX_OBJECT_EVENT_CURSORDOWN, PX_Object_PianoOnCursorDown, PX_NULL);
	PX_ObjectRegisterEvent(pObject, PX_OBJECT_EVENT_CURSORDRAG, PX_Object_PianoOnCursorDrag, PX_NULL);
	PX_ObjectRegisterEvent(pObject, PX_OBJECT_EVENT_CURSORUP, PX_Object_PianoOnCursorUp, PX_NULL);
	return pObject;
}

px_void PX_Object_PianoTuneOnRender(px_surface* psurface, PX_Object* pObject, px_dword elapsed)
{
	px_float step,oftx,ofty;
	PX_Object_Piano_Tune* pTune=PX_ObjectGetDesc(PX_Object_Piano_Tune, pObject);
	oftx = pObject->x;
	ofty = pObject->y;
	step = pObject->Width;
	
////////////////////////////////////////////////////////
//frequency
	pTune->label_f->x = oftx;
	pTune->label_f->y = ofty;
	pTune->label_f->Width = pObject->Width;

	pTune->sliderbar_f->x = oftx ;
	pTune->sliderbar_f->y = ofty+28;
	pTune->sliderbar_f->Width = pObject->Width;

	ofty += step;
////////////////////////////////////////////////////////
//weight
	
	pTune->label_weight->x = oftx;
	pTune->label_weight->y = ofty;
	pTune->label_weight->Width = pObject->Width;

	pTune->sliderbar_weight->x = oftx;
	pTune->sliderbar_weight->y = ofty + 28;
	pTune->sliderbar_weight->Width = pObject->Width;

	ofty += step;
////////////////////////////////////////////////////////
//minr
	pTune->label_minr->x = oftx;
	pTune->label_minr->y = ofty;
	pTune->label_minr->Width = pObject->Width;

	pTune->sliderbar_minr->x = oftx;
	pTune->sliderbar_minr->y = ofty + 28;
	pTune->sliderbar_minr->Width = pObject->Width;

	ofty += step;

////////////////////////////////////////////////////////
//maxr
	pTune->label_maxr->x = oftx;
	pTune->label_maxr->y = ofty;
	pTune->label_maxr->Width = pObject->Width;

	pTune->sliderbar_maxr->x = oftx;
	pTune->sliderbar_maxr->y = ofty + 28;
	pTune->sliderbar_maxr->Width = pObject->Width;

	ofty += step;
////////////////////////////////////////////////////////
//amprl
	pTune->label_amprl->x = oftx;
	pTune->label_amprl->y = ofty;
	pTune->label_amprl->Width = pObject->Width;

	pTune->sliderbar_amprl->x = oftx;
	pTune->sliderbar_amprl->y = ofty + 28;
	pTune->sliderbar_amprl->Width = pObject->Width;

	ofty += step;

////////////////////////////////////////////////////////
//amprr
	pTune->label_amprr->x = oftx;
	pTune->label_amprr->y = ofty;
	pTune->label_amprr->Width = pObject->Width;

	pTune->sliderbar_amprr->x = oftx;
	pTune->sliderbar_amprr->y = ofty + 28;
	pTune->sliderbar_amprr->Width = pObject->Width;

	ofty += step;

////////////////////////////////////////////////////////
//label_mult_radius_core_string
	pTune->label_mult_radius_core_string->x = oftx;
	pTune->label_mult_radius_core_string->y = ofty;
	pTune->label_mult_radius_core_string->Width = pObject->Width;

	pTune->sliderbar_mult_radius_core_string->x = oftx;
	pTune->sliderbar_mult_radius_core_string->y = ofty + 28;
	pTune->sliderbar_mult_radius_core_string->Width = pObject->Width;

	ofty += step;

////////////////////////////////////////////////////////
//minL
	pTune->label_minL->x = oftx;
	pTune->label_minL->y = ofty;
	pTune->label_minL->Width = pObject->Width;

	pTune->sliderbar_minL->x = oftx;
	pTune->sliderbar_minL->y = ofty + 28;
	pTune->sliderbar_minL->Width = pObject->Width;

	ofty += step;

////////////////////////////////////////////////////////
//maxL
	pTune->label_maxL->x = oftx;
	pTune->label_maxL->y = ofty;
	pTune->label_maxL->Width = pObject->Width;

	pTune->sliderbar_maxL->x = oftx;
	pTune->sliderbar_maxL->y = ofty + 28;
	pTune->sliderbar_maxL->Width = pObject->Width;

	ofty += step;

////////////////////////////////////////////////////////
//ampLl
	pTune->label_ampLl->x = oftx;
	pTune->label_ampLl->y = ofty;
	pTune->label_ampLl->Width = pObject->Width;

	pTune->sliderbar_ampLl->x = oftx;
	pTune->sliderbar_ampLl->y = ofty + 28;
	pTune->sliderbar_ampLl->Width = pObject->Width;

	ofty += step;

////////////////////////////////////////////////////////
//ampLr
	pTune->label_ampLr->x = oftx;
	pTune->label_ampLr->y = ofty;
	pTune->label_ampLr->Width = pObject->Width;

	pTune->sliderbar_ampLr->x = oftx;
	pTune->sliderbar_ampLr->y = ofty + 28;
	pTune->sliderbar_ampLr->Width = pObject->Width;

	ofty += step;

////////////////////////////////////////////////////////
//mult_density_string
	pTune->label_mult_density_string->x = oftx;
	pTune->label_mult_density_string->y = ofty;
	pTune->label_mult_density_string->Width = pObject->Width;

	pTune->sliderbar_mult_density_string->x = oftx;
	pTune->sliderbar_mult_density_string->y = ofty + 28;
	pTune->sliderbar_mult_density_string->Width = pObject->Width;

	ofty += step;

////////////////////////////////////////////////////////
//mult_modulus_string
	pTune->label_mult_modulus_string->x = oftx;
	pTune->label_mult_modulus_string->y = ofty;
	pTune->label_mult_modulus_string->Width = pObject->Width;

	pTune->sliderbar_mult_modulus_string->x = oftx;
	pTune->sliderbar_mult_modulus_string->y = ofty + 28;
	pTune->sliderbar_mult_modulus_string->Width = pObject->Width;

	ofty += step;

////////////////////////////////////////////////////////
//mult_impedance_bridge
	pTune->label_mult_impedance_bridge->x = oftx;
	pTune->label_mult_impedance_bridge->y = ofty;
	pTune->label_mult_impedance_bridge->Width = pObject->Width;

	pTune->sliderbar_mult_impedance_bridge->x = oftx;
	pTune->sliderbar_mult_impedance_bridge->y = ofty + 28;
	pTune->sliderbar_mult_impedance_bridge->Width = pObject->Width;

	ofty += step;

////////////////////////////////////////////////////////
//mult_impedance_hammer
	pTune->label_mult_impedance_hammer->x = oftx;
	pTune->label_mult_impedance_hammer->y = ofty;
	pTune->label_mult_impedance_hammer->Width = pObject->Width;

	pTune->sliderbar_mult_impedance_hammer->x = oftx;
	pTune->sliderbar_mult_impedance_hammer->y = ofty + 28;
	pTune->sliderbar_mult_impedance_hammer->Width = pObject->Width;

	ofty += step;


////////////////////////////////////////////////////////
//mult_mass_hammer
	pTune->label_mult_mass_hammer->x = oftx;
	pTune->label_mult_mass_hammer->y = ofty;
	pTune->label_mult_mass_hammer->Width = pObject->Width;

	pTune->sliderbar_mult_mass_hammer->x = oftx;
	pTune->sliderbar_mult_mass_hammer->y = ofty + 28;
	pTune->sliderbar_mult_mass_hammer->Width = pObject->Width;

	ofty += step;

////////////////////////////////////////////////////////
//mult_force_hammer
	pTune->label_mult_force_hammer->x = oftx;
	pTune->label_mult_force_hammer->y = ofty;
	pTune->label_mult_force_hammer->Width = pObject->Width;

	pTune->sliderbar_mult_force_hammer->x = oftx;
	pTune->sliderbar_mult_force_hammer->y = ofty + 28;
	pTune->sliderbar_mult_force_hammer->Width = pObject->Width;

	ofty += step;

////////////////////////////////////////////////////////
//mult_hysteresis_hammer
	pTune->label_mult_hysteresis_hammer->x = oftx;
	pTune->label_mult_hysteresis_hammer->y = ofty;
	pTune->label_mult_hysteresis_hammer->Width = pObject->Width;

	pTune->sliderbar_mult_hysteresis_hammer->x = oftx;
	pTune->sliderbar_mult_hysteresis_hammer->y = ofty + 28;
	pTune->sliderbar_mult_hysteresis_hammer->Width = pObject->Width;

	ofty += step;

////////////////////////////////////////////////////////
//mult_hysteresis_hammer
	pTune->label_mult_stiffness_exponent_hammer->x = oftx;
	pTune->label_mult_stiffness_exponent_hammer->y = ofty;
	pTune->label_mult_stiffness_exponent_hammer->Width = pObject->Width;

	pTune->sliderbar_mult_stiffness_exponent_hammer->x = oftx;
	pTune->sliderbar_mult_stiffness_exponent_hammer->y = ofty + 28;
	pTune->sliderbar_mult_stiffness_exponent_hammer->Width = pObject->Width;

	ofty += step;

////////////////////////////////////////////////////////
//position_hammer
	pTune->label_position_hammer->x = oftx;
	pTune->label_position_hammer->y = ofty;
	pTune->label_position_hammer->Width = pObject->Width;

	pTune->sliderbar_position_hammer->x = oftx;
	pTune->sliderbar_position_hammer->y = ofty + 28;
	pTune->sliderbar_position_hammer->Width = pObject->Width;

////////////////////////////////////////////////////////
//mult_loss_filter
	pTune->label_mult_loss_filter->x = oftx;
	pTune->label_mult_loss_filter->y = ofty;
	pTune->label_mult_loss_filter->Width = pObject->Width;

	pTune->sliderbar_mult_loss_filter->x = oftx;
	pTune->sliderbar_mult_loss_filter->y = ofty + 28;
	pTune->sliderbar_mult_loss_filter->Width = pObject->Width;

////////////////////////////////////////////////////////
//detune
	pTune->label_detune->x = oftx;
	pTune->label_detune->y = ofty;
	pTune->label_detune->Width = pObject->Width;

	pTune->sliderbar_detune->x = oftx;
	pTune->sliderbar_detune->y = ofty + 28;
	pTune->sliderbar_detune->Width = pObject->Width;

////////////////////////////////////////////////////////
//hammer_type 
	pTune->label_hammer_type->x = oftx;
	pTune->label_hammer_type->y = ofty;
	pTune->label_hammer_type->Width = pObject->Width;

	pTune->selectbox_hammer_type->x = oftx;
	pTune->selectbox_hammer_type->y = ofty + 28;
	pTune->selectbox_hammer_type->Width = pObject->Width;
}

PX_Object* PX_Object_PianoTuneCreate(px_memorypool* mp, PX_Object* Parent, px_float x, px_float y, px_float width, px_float height)
{
	PX_Object_Piano_Tune Tune,*pTune;
	PX_Object* pObject;
	PX_memset(&Tune, 0, sizeof(Tune));
	pObject = PX_ObjectCreateEx(mp, Parent, 0, 0, 0, 0, 0, 0, PX_OBJECT_TYPE_PIANOTUNE,PX_NULL, PX_Object_PianoTuneOnRender,PX_NULL,&Tune,sizeof(Tune));
	pTune = PX_ObjectGetDesc(PX_Object_Piano_Tune, pObject);

	pTune->label_f = PX_Object_LabelCreate(mp, pObject, 0, 0, width, 24, "frequency:", PX_NULL, PX_OBJECT_UI_DEFAULT_FONTCOLOR);
	pTune->sliderbar_f = PX_Object_SliderBarCreate(mp, pObject, 0, 0, width,24,PX_OBJECT_SLIDERBAR_TYPE_HORIZONTAL,PX_OBJECT_SLIDERBAR_STYLE_BOX);
	PX_Object_SliderBarSetRange(pTune->sliderbar_f, 10, 8000);

	pTune->label_weight = PX_Object_LabelCreate(mp, pObject, 0, 0, width, 24, "Key pulsation velocity weight:", PX_NULL, PX_OBJECT_UI_DEFAULT_FONTCOLOR);
	pTune->sliderbar_weight = PX_Object_SliderBarCreate(mp, pObject, 0, 0, width, 24, PX_OBJECT_SLIDERBAR_TYPE_HORIZONTAL, PX_OBJECT_SLIDERBAR_STYLE_BOX);
	PX_Object_SliderBarSetRange(pTune->sliderbar_weight, 0, 1000);

	pTune->label_minr = PX_Object_LabelCreate(mp, pObject, 0, 0, width, 24, "minimum string radio:", PX_NULL, PX_OBJECT_UI_DEFAULT_FONTCOLOR);
	pTune->sliderbar_minr = PX_Object_SliderBarCreate(mp, pObject, 0, 0, width, 24, PX_OBJECT_SLIDERBAR_TYPE_HORIZONTAL, PX_OBJECT_SLIDERBAR_STYLE_BOX);
	PX_Object_SliderBarSetRange(pTune->sliderbar_minr, 0, 1000);//0.00-10.00

	pTune->label_maxr = PX_Object_LabelCreate(mp, pObject, 0, 0, width, 24, "maximum string radio:", PX_NULL, PX_OBJECT_UI_DEFAULT_FONTCOLOR);
	pTune->sliderbar_maxr = PX_Object_SliderBarCreate(mp, pObject, 0, 0, width, 24, PX_OBJECT_SLIDERBAR_TYPE_HORIZONTAL, PX_OBJECT_SLIDERBAR_STYLE_BOX);
	PX_Object_SliderBarSetRange(pTune->sliderbar_minr, 0, 1000);//0.00-10.00

	pTune->label_amprl = PX_Object_LabelCreate(mp, pObject, 0, 0, width, 24, "radius left sigmoidal shape:", PX_NULL, PX_OBJECT_UI_DEFAULT_FONTCOLOR);
	pTune->sliderbar_amprl = PX_Object_SliderBarCreate(mp, pObject, 0, 0, width, 24, PX_OBJECT_SLIDERBAR_TYPE_HORIZONTAL, PX_OBJECT_SLIDERBAR_STYLE_BOX);
	PX_Object_SliderBarSetRange(pTune->sliderbar_amprl, 0, 1000);//0.00-10.00

	pTune->label_amprr = PX_Object_LabelCreate(mp, pObject, 0, 0, width, 24, "radius right sigmoidal shape:", PX_NULL, PX_OBJECT_UI_DEFAULT_FONTCOLOR);
	pTune->sliderbar_amprr = PX_Object_SliderBarCreate(mp, pObject, 0, 0, width, 24, PX_OBJECT_SLIDERBAR_TYPE_HORIZONTAL, PX_OBJECT_SLIDERBAR_STYLE_BOX);
	PX_Object_SliderBarSetRange(pTune->sliderbar_amprr, 0, 1000);//0.00-10.00

	pTune->label_mult_radius_core_string = PX_Object_LabelCreate(mp, pObject, 0, 0, width, 24, "cord nucleus radio:", PX_NULL, PX_OBJECT_UI_DEFAULT_FONTCOLOR);
	pTune->sliderbar_mult_radius_core_string = PX_Object_SliderBarCreate(mp, pObject, 0, 0, width, 24, PX_OBJECT_SLIDERBAR_TYPE_HORIZONTAL, PX_OBJECT_SLIDERBAR_STYLE_BOX);
	PX_Object_SliderBarSetRange(pTune->sliderbar_mult_radius_core_string, 0, 2000);//0.000-2.000

	pTune->label_minL = PX_Object_LabelCreate(mp, pObject, 0, 0, width, 24, "minimum string length:", PX_NULL, PX_OBJECT_UI_DEFAULT_FONTCOLOR);
	pTune->sliderbar_minL = PX_Object_SliderBarCreate(mp, pObject, 0, 0, width, 24, PX_OBJECT_SLIDERBAR_TYPE_HORIZONTAL, PX_OBJECT_SLIDERBAR_STYLE_BOX);
	PX_Object_SliderBarSetRange(pTune->sliderbar_minL, 0, 1000);//0.000-0.1000

	pTune->label_maxL = PX_Object_LabelCreate(mp, pObject, 0, 0, width, 24, "maximum string length.:", PX_NULL, PX_OBJECT_UI_DEFAULT_FONTCOLOR);
	pTune->sliderbar_maxL = PX_Object_SliderBarCreate(mp, pObject, 0, 0, width, 24, PX_OBJECT_SLIDERBAR_TYPE_HORIZONTAL, PX_OBJECT_SLIDERBAR_STYLE_BOX);
	PX_Object_SliderBarSetRange(pTune->sliderbar_maxL, 0, 1000);//00.00-10.00


	pTune->label_ampLl = PX_Object_LabelCreate(mp, pObject, 0, 0, width, 24, "length left sigmoidal shape:", PX_NULL, PX_OBJECT_UI_DEFAULT_FONTCOLOR);
	pTune->sliderbar_ampLl = PX_Object_SliderBarCreate(mp, pObject, 0, 0, width, 24, PX_OBJECT_SLIDERBAR_TYPE_HORIZONTAL, PX_OBJECT_SLIDERBAR_STYLE_BOX);
	PX_Object_SliderBarSetRange(pTune->sliderbar_ampLl, 0, 1000);//-10-0


	pTune->label_ampLr = PX_Object_LabelCreate(mp, pObject, 0, 0, width, 24, "length right sigmoidal shape:", PX_NULL, PX_OBJECT_UI_DEFAULT_FONTCOLOR);
	pTune->sliderbar_ampLr = PX_Object_SliderBarCreate(mp, pObject, 0, 0, width, 24, PX_OBJECT_SLIDERBAR_TYPE_HORIZONTAL, PX_OBJECT_SLIDERBAR_STYLE_BOX);
	PX_Object_SliderBarSetRange(pTune->sliderbar_ampLr, 0, 1000);//0-10


	pTune->label_mult_density_string = PX_Object_LabelCreate(mp, pObject, 0, 0, width, 24, "string density multiplier:", PX_NULL, PX_OBJECT_UI_DEFAULT_FONTCOLOR);
	pTune->sliderbar_mult_density_string = PX_Object_SliderBarCreate(mp, pObject, 0, 0, width, 24, PX_OBJECT_SLIDERBAR_TYPE_HORIZONTAL, PX_OBJECT_SLIDERBAR_STYLE_BOX);
	PX_Object_SliderBarSetRange(pTune->sliderbar_mult_density_string, 0, 1000);//00.00-10.00

	pTune->label_mult_modulus_string = PX_Object_LabelCreate(mp, pObject, 0, 0, width, 24, "young modulus multiplier:", PX_NULL, PX_OBJECT_UI_DEFAULT_FONTCOLOR);
	pTune->sliderbar_mult_modulus_string = PX_Object_SliderBarCreate(mp, pObject, 0, 0, width, 24, PX_OBJECT_SLIDERBAR_TYPE_HORIZONTAL, PX_OBJECT_SLIDERBAR_STYLE_BOX);
	PX_Object_SliderBarSetRange(pTune->sliderbar_mult_modulus_string, 0, 1000);//00.00-10.00


	pTune->label_mult_impedance_bridge = PX_Object_LabelCreate(mp, pObject, 0, 0, width, 24, "bridge impedance multiplier:", PX_NULL, PX_OBJECT_UI_DEFAULT_FONTCOLOR);
	pTune->sliderbar_mult_impedance_bridge = PX_Object_SliderBarCreate(mp, pObject, 0, 0, width, 24, PX_OBJECT_SLIDERBAR_TYPE_HORIZONTAL, PX_OBJECT_SLIDERBAR_STYLE_BOX);
	PX_Object_SliderBarSetRange(pTune->sliderbar_mult_impedance_bridge, 0, 1000);//00.00-10.00

	pTune->label_mult_impedance_hammer = PX_Object_LabelCreate(mp, pObject, 0, 0, width, 24, "hammer impedance multiplier:", PX_NULL, PX_OBJECT_UI_DEFAULT_FONTCOLOR);
	pTune->sliderbar_mult_impedance_hammer = PX_Object_SliderBarCreate(mp, pObject, 0, 0, width, 24, PX_OBJECT_SLIDERBAR_TYPE_HORIZONTAL, PX_OBJECT_SLIDERBAR_STYLE_BOX);
	PX_Object_SliderBarSetRange(pTune->sliderbar_mult_impedance_hammer, 0, 1000);//00.00-10.00


	pTune->label_mult_mass_hammer = PX_Object_LabelCreate(mp, pObject, 0, 0, width, 24, "mass hammer multiplier:", PX_NULL, PX_OBJECT_UI_DEFAULT_FONTCOLOR);
	pTune->sliderbar_mult_mass_hammer = PX_Object_SliderBarCreate(mp, pObject, 0, 0, width, 24, PX_OBJECT_SLIDERBAR_TYPE_HORIZONTAL, PX_OBJECT_SLIDERBAR_STYLE_BOX);
	PX_Object_SliderBarSetRange(pTune->sliderbar_mult_mass_hammer, 0, 1000);//00.00-10.00

	pTune->label_mult_force_hammer = PX_Object_LabelCreate(mp, pObject, 0, 0, width, 24, "force hammer multiplier:", PX_NULL, PX_OBJECT_UI_DEFAULT_FONTCOLOR);
	pTune->sliderbar_mult_force_hammer = PX_Object_SliderBarCreate(mp, pObject, 0, 0, width, 24, PX_OBJECT_SLIDERBAR_TYPE_HORIZONTAL, PX_OBJECT_SLIDERBAR_STYLE_BOX);
	PX_Object_SliderBarSetRange(pTune->sliderbar_mult_mass_hammer, 0, 1000);//0.000-1.000

	pTune->label_mult_hysteresis_hammer = PX_Object_LabelCreate(mp, pObject, 0, 0, width, 24, "hysteresys hammer multiplier:", PX_NULL, PX_OBJECT_UI_DEFAULT_FONTCOLOR);
	pTune->sliderbar_mult_hysteresis_hammer = PX_Object_SliderBarCreate(mp, pObject, 0, 0, width, 24, PX_OBJECT_SLIDERBAR_TYPE_HORIZONTAL, PX_OBJECT_SLIDERBAR_STYLE_BOX);
	PX_Object_SliderBarSetRange(pTune->sliderbar_mult_hysteresis_hammer, 0, 1000);//00.00-10.00

	pTune->label_mult_stiffness_exponent_hammer = PX_Object_LabelCreate(mp, pObject, 0, 0, width, 24, "stiffness exponent hammer multiplier:", PX_NULL, PX_OBJECT_UI_DEFAULT_FONTCOLOR);
	pTune->sliderbar_mult_stiffness_exponent_hammer = PX_Object_SliderBarCreate(mp, pObject, 0, 0, width, 24, PX_OBJECT_SLIDERBAR_TYPE_HORIZONTAL, PX_OBJECT_SLIDERBAR_STYLE_BOX);
	PX_Object_SliderBarSetRange(pTune->sliderbar_mult_stiffness_exponent_hammer, 0, 1000);//00.00-10.00


	pTune->label_position_hammer = PX_Object_LabelCreate(mp, pObject, 0, 0, width, 24, "position hammer:", PX_NULL, PX_OBJECT_UI_DEFAULT_FONTCOLOR);
	pTune->sliderbar_position_hammer = PX_Object_SliderBarCreate(mp, pObject, 0, 0, width, 24, PX_OBJECT_SLIDERBAR_TYPE_HORIZONTAL, PX_OBJECT_SLIDERBAR_STYLE_BOX);
	PX_Object_SliderBarSetRange(pTune->sliderbar_mult_stiffness_exponent_hammer, 0, 1000);//00.00-1.000

	pTune->label_mult_loss_filter = PX_Object_LabelCreate(mp, pObject, 0, 0, width, 24, "string loss factor:", PX_NULL, PX_OBJECT_UI_DEFAULT_FONTCOLOR);
	pTune->sliderbar_mult_loss_filter = PX_Object_SliderBarCreate(mp, pObject, 0, 0, width, 24, PX_OBJECT_SLIDERBAR_TYPE_HORIZONTAL, PX_OBJECT_SLIDERBAR_STYLE_BOX);
	PX_Object_SliderBarSetRange(pTune->sliderbar_mult_stiffness_exponent_hammer, 0, 1000);//00.00-1.000

	pTune->label_detune = PX_Object_LabelCreate(mp, pObject, 0, 0, width, 24, "detuning among 3 strings:", PX_NULL, PX_OBJECT_UI_DEFAULT_FONTCOLOR);
	pTune->sliderbar_detune = PX_Object_SliderBarCreate(mp, pObject, 0, 0, width, 24, PX_OBJECT_SLIDERBAR_TYPE_HORIZONTAL, PX_OBJECT_SLIDERBAR_STYLE_BOX);
	PX_Object_SliderBarSetRange(pTune->sliderbar_mult_stiffness_exponent_hammer, 0, 1000);//0.00000-0.001000

	pTune->label_hammer_type = PX_Object_LabelCreate(mp, pObject, 0, 0, width, 24, "detuning among 3 strings:", PX_NULL, PX_OBJECT_UI_DEFAULT_FONTCOLOR);
	pTune->selectbox_hammer_type = PX_Object_SelectBarCreate(mp, pObject, 0, 0, width, 24,PX_NULL);
	PX_Object_SelectBarAddItem(pTune->selectbox_hammer_type, "Stulov Hammer");
	PX_Object_SelectBarAddItem(pTune->selectbox_hammer_type, "BanksHammer");
	return PX_NULL;
}

