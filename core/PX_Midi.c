#include "PX_Midi.h"
px_bool PX_MidiInitialize(PX_Midi* midi, px_memorypool* mp, PX_MidiCallback callback, px_void* userptr)
{
	PX_memset(midi, 0, sizeof(PX_Midi));
	midi->mp = mp;
	midi->callback = callback;
	midi->userptr = userptr;
	midi->Qn = 640;
	return PX_TRUE;
}

typedef struct
{
	px_char MThd[4];
	px_word length[2];//6
	px_word ffff;
	px_word nnnn;
	px_word dddd;
}PX_Midi_MThd;

typedef struct
{
	px_char MTrk[4];
	px_dword length;
}PX_Midi_MTrk;

px_void PX_MidiUpdate_handleTick(PX_Midi* pmidi, px_int iTrack)
{
	PX_Midi_Track* pTrack = PX_VECTORAT(PX_Midi_Track, &pmidi->track, iTrack);
	pTrack->wait_tick += PX_MidiReadTick(pmidi,iTrack);
}
px_void PX_MidiUpdate_handleOpcode(PX_Midi* pmidi, px_int iTrack)
{
	PX_Midi_Track* pTrack = PX_VECTORAT(PX_Midi_Track, &pmidi->track, iTrack);
	px_byte pl;

	if (pTrack->nextType == PX_MIDI_NEXT_TYPE_ERROR)
	{
		return;
	}
	if (pTrack->ip >= pTrack->payloadSize)
	{
		pTrack->nextType = PX_MIDI_NEXT_TYPE_END;
		return;
	}
	pl = pTrack->payload[pTrack->ip];
	pTrack->ip++;

	do
	{
		switch (pl & 0xf0)
		{
		case 0x80:
		case 0x90:
		case 0xA0:
		{
			px_int track = (pl & 0x0f);
			px_byte note;
			px_byte v;
			pTrack->lastPlayload = pl;
			note = pTrack->payload[pTrack->ip];
			pTrack->ip++;
			v = pTrack->payload[pTrack->ip];
			pTrack->ip++;
			if (pTrack->ip >= pTrack->payloadSize)
				pTrack->nextType = PX_MIDI_NEXT_TYPE_END;
			else
				pTrack->nextType = PX_MIDI_NEXT_TYPE_TICK;

			pmidi->callback(pmidi, pl & 0xf0, pl & 0xf, pTrack->instrument, note, v, pmidi->userptr);
			return;
		}
		break;
		case 0xB0:
		{
			pTrack->lastPlayload = pl;
			pTrack->ip += 2;
		}
		break;
		case 0xC0:
		{
			px_int track = (pl & 0x0f);
			pTrack->lastPlayload = pl;
			pTrack->instrument = pTrack->payload[pTrack->ip];
			pTrack->ip++;
		}
		break;
		case 0xD0:
		{
			pTrack->lastPlayload = pl;
			pTrack->ip++;

		}
		break;
		case 0xE0:
		{
			pTrack->lastPlayload = pl;
			pTrack->ip++;
			pTrack->ip++;
		}
		break;
		case 0xF0:
		{
			pTrack->lastPlayload = pl;
			if (pl == 0xff)
			{
				px_int type = pTrack->payload[pTrack->ip++];
				px_int size = pTrack->payload[pTrack->ip++];
				pTrack->ip += size;
			}
			else
			{
				px_int size = pTrack->payload[pTrack->ip++];
				pTrack->ip += size;
			}

		}
		break;
		default:
			pl = pTrack->lastPlayload;
			pTrack->ip--;
			continue;
		}

		if (pTrack->ip >= pTrack->payloadSize)
			pTrack->nextType = PX_MIDI_NEXT_TYPE_END;
		else
			pTrack->nextType = PX_MIDI_NEXT_TYPE_TICK;

		break;
	} while (PX_TRUE);

	return;
}


px_bool PX_MidiLoad(PX_Midi* pmidi, const px_byte data[], px_int datasize)
{
	px_int i;
	px_int oft=0;
	PX_Midi_MThd *pMThdheader;
	PX_Midi_MTrk* pMTrkHeader;
	if (datasize<sizeof(PX_Midi_MThd))
	{
		return PX_FALSE;
	}
	pMThdheader = ((PX_Midi_MThd*)data);
	oft += sizeof(PX_Midi_MThd);
	if (!PX_memequ(pMThdheader->MThd,"MThd",4))
	{
		return PX_FALSE;
	}
	if (pMThdheader->length[1]!=0x0600)
	{
		return PX_FALSE;
	}
	if (pMThdheader->ffff!=0 && pMThdheader->ffff != 0x0100)
	{
		return PX_FALSE;//not supported
	}
	if (!PX_VectorInitialize(pmidi->mp,&pmidi->track,sizeof(PX_Midi_Track),pMThdheader->nnnn>>8))
	{
		return PX_FALSE;
	}
	pmidi->TPQN = PX_htons(pMThdheader->dddd);

	for (i=0;i< (pMThdheader->nnnn >> 8);i++)
	{
		px_int MTrkSize;
		PX_Midi_Track Track = {0};

		if (oft+(px_int)sizeof(PX_Midi_MTrk)>datasize)
		{
			goto _ERROR;
		}
		pMTrkHeader = (PX_Midi_MTrk *)(data + oft);
		if (!PX_memequ(pMTrkHeader->MTrk,"MTrk",4))
		{
			goto _ERROR;
		}
		oft += sizeof(PX_Midi_MTrk);
		MTrkSize = PX_htonl(pMTrkHeader->length);
		if (oft+MTrkSize> datasize)
		{
			goto _ERROR;
		}
		
		Track.instrument = 0;
		Track.ip = 0;
		Track.payloadSize = MTrkSize;
		Track.payload = PX_Malloc(px_byte, pmidi->mp, MTrkSize);
		if (!Track.payload)
		{
			goto _ERROR;
		}
		PX_memcpy(Track.payload, data + oft, MTrkSize);
		Track.wait_tick = 0;
		oft += MTrkSize;
		PX_VectorPushback(&pmidi->track, &Track);//will be succeed
	}

	//check
	for (i = 0;i<pmidi->track.size;i++)
	{

		while (PX_TRUE)
		{
			if (PX_MidiGetNextType(pmidi,i)==PX_MIDI_NEXT_TYPE_ERROR)
			{
				goto _ERROR;
			}
			else if (PX_MidiGetNextType(pmidi, i) == PX_MIDI_NEXT_TYPE_END)
			{
				break;
			}
			else if (PX_MidiGetNextType(pmidi, i) == PX_MIDI_NEXT_TYPE_TICK)
			{
				PX_MidiReadTick(pmidi, i);
			}
			else if (PX_MidiGetNextType(pmidi, i) == PX_MIDI_NEXT_TYPE_OPCODE)
			{
				PX_MidiReadNote(pmidi, i);
			}
		}
	}
	PX_MidiReset(pmidi);

	return PX_TRUE;
_ERROR:
	
	for (i = 0; i < pmidi->track.size; i++)
	{
		PX_Midi_Track* pTrack=PX_VECTORAT(PX_Midi_Track,&pmidi->track,i);
		PX_Free(pmidi->mp, pTrack->payload);
	}
	PX_VectorFree(&pmidi->track);
	return PX_FALSE;
}


px_bool PX_MidiIsEnd(PX_Midi* pmidi)
{
	px_int i;
	for (i = 0; i < pmidi->track.size; i++)
	{
		PX_Midi_Track* pTrack = PX_VECTORAT(PX_Midi_Track, &pmidi->track, i);
		if (pTrack->ip<pTrack->payloadSize)
		{
			return PX_FALSE;
		}
	}
	return PX_TRUE;
}

px_void PX_MidiPlay(PX_Midi* pmidi)
{
	pmidi->state = PX_Midi_State_Play;
}

px_void PX_MidiPause(PX_Midi* pmidi)
{
	pmidi->state = PX_Midi_State_Stop;
}

px_void PX_MidiReset(PX_Midi* pmidi)
{
	px_int i;
	for (i = 0; i < pmidi->track.size; i++)
	{
		PX_Midi_Track* pTrack = PX_VECTORAT(PX_Midi_Track, &pmidi->track, i);
		pTrack->ip = 0;
		pTrack->wait_tick = 0;
		pTrack->instrument = 0;
		pTrack->nextType = PX_MIDI_NEXT_TYPE_TICK;
	}
}

px_void PX_MidiSetQuarterNoteDuration(PX_Midi* pmidi, px_int qn)
{
	pmidi->Qn = qn;
}

px_void PX_MidiNoteToString( px_byte note, px_char out[8])
{
	const px_char* note_content[] = { "C","#C","D","#D","E","F","#F","G","#G","A","#A","B" };
	PX_sprintf2(out, 8, "%1%2", PX_STRINGFORMAT_STRING(note_content[note % 12]), PX_STRINGFORMAT_INT(note / 12-1));
}

px_void PX_MidiUpdate(PX_Midi* pmidi, px_dword elapsed)
{
	px_int i;
	if (pmidi->state!=PX_Midi_State_Play)
	{
		return;
	}
	for (i=0;i<pmidi->track.size;i++)
	{
		PX_Midi_Track* pTrack = PX_VECTORAT(PX_Midi_Track, &pmidi->track, i);
		px_float elapsed_tick = pmidi->TPQN*1.0f / pmidi->Qn * elapsed;
		pTrack->wait_tick -= elapsed_tick;

		while (pTrack->wait_tick<=0&&pTrack->ip<pTrack->payloadSize)
		{
			if (pTrack->nextType == PX_MIDI_NEXT_TYPE_TICK)
			{
				PX_MidiUpdate_handleTick(pmidi, i);
			}
			else if (pTrack->nextType == PX_MIDI_NEXT_TYPE_OPCODE)
			{
				PX_MidiUpdate_handleOpcode(pmidi, i);
			}
			else
				break;
			
		}

	}
}

PX_MIDI_NEXT_TYPE PX_MidiGetNextType(PX_Midi* pmidi,px_int track)
{
	PX_Midi_Track* pTrack = PX_VECTORAT(PX_Midi_Track, &pmidi->track, track);
	return pTrack->nextType;
}

px_void PX_MidiFree(PX_Midi* pmidi)
{
	px_int i;
	for (i = 0; i < pmidi->track.size; i++)
	{
		PX_Midi_Track* pTrack = PX_VECTORAT(PX_Midi_Track, &pmidi->track, i);
		PX_Free(pmidi->mp, pTrack->payload);
	}
	PX_VectorFree(&pmidi->track);
}

px_int PX_MidiGetTrackCount(PX_Midi* pmidi)
{
	return pmidi->track.size;
}

px_int PX_MidiReadTick(PX_Midi* pmidi, px_int iTrack)
{
	px_int delay = 0;
	px_int counter = 0;
	px_int ip;
	px_byte pl;
	PX_Midi_Track* pTrack = PX_VECTORAT(PX_Midi_Track, &pmidi->track, iTrack);
	
	if (pTrack->nextType == PX_MIDI_NEXT_TYPE_ERROR)
	{
		return -1;
	}
	
	if (pTrack->ip >= pTrack->payloadSize)
	{
		pTrack->nextType = PX_MIDI_NEXT_TYPE_END;
		return -1;
	}
	ip = pTrack->ip;
	pl = pTrack->payload[ip];
	ip++;
	while (pl & 0x80)
	{
		counter++;
		delay = ((pl & 0x7f) << 7);
		pl = pTrack->payload[ip];
		ip++;
		if (counter == 3)
		{
			break ;
		}
		if (ip >= pTrack->payloadSize)
		{
			pTrack->nextType = PX_MIDI_NEXT_TYPE_END;
			return -1;
		}
	}
	delay += pl;
	pTrack->ip = ip;
	if (pTrack->ip >= pTrack->payloadSize)
		pTrack->nextType = PX_MIDI_NEXT_TYPE_END;
	else
		pTrack->nextType = PX_MIDI_NEXT_TYPE_OPCODE;
	return delay;
}
PX_Midi_Note PX_MidiReadNote(PX_Midi* pmidi, px_int iTrack)
{
	PX_Midi_Track* pTrack = PX_VECTORAT(PX_Midi_Track, &pmidi->track, iTrack);
	px_byte pl;
	PX_Midi_Note note;
	note.opcode = PX_MIDI_OPCODE_UNDEFINE;
	note.note = -1;
	note.v = 0;
	if (pTrack->nextType==PX_MIDI_NEXT_TYPE_ERROR)
	{
		return note;
	}
	if (pTrack->ip >= pTrack->payloadSize)
	{
		pTrack->nextType = PX_MIDI_NEXT_TYPE_END;
		return note;
	}
	pl = pTrack->payload[pTrack->ip];
	pTrack->ip++;

	do 
	{
		switch (pl & 0xf0)
		{
		case 0x80:
		case 0x90:
		case 0xA0:
		{
			px_int track = (pl & 0x0f);
			px_byte bnote;
			px_byte v;
			pTrack->lastPlayload = pl;
			bnote = pTrack->payload[pTrack->ip];
			pTrack->ip++;
			v = pTrack->payload[pTrack->ip];
			pTrack->ip++;
			if (pTrack->ip >= pTrack->payloadSize)
				pTrack->nextType = PX_MIDI_NEXT_TYPE_END;
			else
				pTrack->nextType = PX_MIDI_NEXT_TYPE_TICK;
			if ((pl & 0xf0) == 0x90)
			{
				note.opcode = PX_MIDI_OPCODE_KEYDOWN;
				note.note = bnote;
				note.v = v;
				note.track = track;
			}
			else
			{
				note.opcode = PX_MIDI_OPCODE_KEYUP;
				note.note = bnote;
				note.v = v;
				note.track = track;
			}
			return note ;
		}
		break;
		case 0xB0:
		{
			pTrack->lastPlayload = pl;
			pTrack->ip += 2;
		}
		break;
		case 0xC0:
		{
			px_int track = (pl & 0x0f);
			pTrack->lastPlayload = pl;
			pTrack->instrument = pTrack->payload[pTrack->ip];
			pTrack->ip++;
		}
		break;
		case 0xD0:
		{
			pTrack->lastPlayload = pl;
			pTrack->ip++;

		}
		break;
		case 0xE0:
		{
			pTrack->lastPlayload = pl;
			pTrack->ip++;
			pTrack->ip++;
		}
		break;
		case 0xF0:
		{
			pTrack->lastPlayload = pl;
			if (pl == 0xff)
			{
				px_int type = pTrack->payload[pTrack->ip++];
				px_int size = pTrack->payload[pTrack->ip++];
				pTrack->ip += size;
			}
			else
			{
				px_int size = pTrack->payload[pTrack->ip++];
				pTrack->ip += size;
			}

		}
		break;
		default:
			pl = pTrack->lastPlayload;
			pTrack->ip--;
			continue;
		}

		if (pTrack->ip >= pTrack->payloadSize)
			pTrack->nextType = PX_MIDI_NEXT_TYPE_END;
		else
			pTrack->nextType = PX_MIDI_NEXT_TYPE_TICK;

		break;
	} while (PX_TRUE);
	
	return note;
}

px_int PX_MidiNoteToPianoKey(px_int note)
{
	px_int index = note - 21;
	if (index<0)
	{
		return -1;
	}
	if (index>=88)
	{
		return -1;
	}
	return index;
}

px_int PX_MidiNoteGetDurationTick(PX_Midi* pmidi, px_int iTrack, px_int note)
{
	PX_Midi_Track* pTrack = PX_VECTORAT(PX_Midi_Track, &pmidi->track, iTrack);
	px_byte pl;
	px_int ip= pTrack->ip;
	px_int lastpayload=0;
	px_int delaytick=0;
	PX_MIDI_NEXT_TYPE nextType= pTrack->nextType;

	while (PX_TRUE)
	{
		if (nextType == PX_MIDI_NEXT_TYPE_ERROR)
		{
			return delaytick;
		}
		if (ip >= pTrack->payloadSize)
		{
			nextType = PX_MIDI_NEXT_TYPE_END;
			return delaytick;
		}

		if (nextType==PX_MIDI_NEXT_TYPE_TICK)
		{
			px_int counter = 0;
			px_int delay = 0;
			pl = pTrack->payload[ip];
			ip++;
			while (pl & 0x80)
			{
				counter++;
				delay = ((pl & 0x7f) << 7);
				pl = pTrack->payload[ip];
				ip++;
				if (counter == 3)
				{
					break;
				}
				if (ip >= pTrack->payloadSize)
				{
					nextType = PX_MIDI_NEXT_TYPE_END;
					return -1;
				}
			}
			delay += pl;
			delaytick += delay;

			if (ip >= pTrack->payloadSize)
				nextType = PX_MIDI_NEXT_TYPE_END;
			else
				nextType = PX_MIDI_NEXT_TYPE_OPCODE;
		}
		else if (nextType == PX_MIDI_NEXT_TYPE_OPCODE)
		{
			pl = pTrack->payload[ip];
			ip++;
			do
			{
				switch (pl & 0xf0)
				{
				case 0x80:
				case 0x90:
				case 0xA0:
				{
					px_int track = (pl & 0x0f);
					px_byte bnote;
					px_byte v;
					lastpayload = pl;
					bnote = pTrack->payload[ip];
					ip++;
					v = pTrack->payload[ip];
					ip++;
					if (ip >= pTrack->payloadSize)
						nextType = PX_MIDI_NEXT_TYPE_END;
					else
						nextType = PX_MIDI_NEXT_TYPE_TICK;

					if (bnote==note)
					{
						return delaytick;
					}

				}
				break;
				case 0xB0:
				{
					lastpayload = pl;
					ip += 2;
				}
				break;
				case 0xC0:
				{
					px_int track = (pl & 0x0f);
					lastpayload = pl;
					ip++;
				}
				break;
				case 0xD0:
				{
					lastpayload = pl;
					ip++;

				}
				break;
				case 0xE0:
				{
					lastpayload = pl;
					ip++;
					ip++;
				}
				break;
				case 0xF0:
				{
					lastpayload = pl;
					if (pl == 0xff)
					{
						px_int type = pTrack->payload[ip++];
						px_int size = pTrack->payload[ip++];
						ip += size;
					}
					else
					{
						px_int size = pTrack->payload[ip++];
						ip += size;
					}

				}
				break;
				default:
					pl = lastpayload;
					ip--;
					continue;
				}

				if (ip >= pTrack->payloadSize)
					nextType = PX_MIDI_NEXT_TYPE_END;
				else
					nextType = PX_MIDI_NEXT_TYPE_TICK;

				break;
			} while (PX_TRUE);
		}
		
	}
	return note;
}