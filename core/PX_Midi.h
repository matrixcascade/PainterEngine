#ifndef PX_MIDI_H
#define PX_MIDI_H
#include "PX_Vector.h"
#include "PX_Memory.h"

struct _PX_Midi;

typedef px_void (*PX_MidiCallback)(struct _PX_Midi* midi, px_byte opcode,px_byte  track,px_byte  instrument, px_byte Note, px_byte velocity, px_void* userptr);

typedef struct 
{
	px_byte instrument;
	px_float wait_tick;
	px_byte *payload;
	px_int ip;
	px_int payloadSize;
}PX_Midi_Track;

typedef enum
{
	PX_Midi_State_Stop=0,
	PX_Midi_State_Play,
}PX_Midi_State;

typedef struct _PX_Midi
{
	px_void* userptr;
	px_memorypool* mp;
	px_int TPQN;
	px_int Qn;
	PX_MidiCallback callback;
	px_vector track;
	PX_Midi_State state;
}PX_Midi;

px_bool PX_MidiInitialize(PX_Midi* pmidi, px_memorypool* mp, PX_MidiCallback callback,px_void *userptr);
px_bool PX_MidiLoad(PX_Midi* pmidi, const px_byte data[], px_int datasize);
px_bool PX_MidiIsEnd(PX_Midi* pmidi);
px_void PX_MidiPlay(PX_Midi* pmidi);
px_void PX_MidiPause(PX_Midi* pmidi);
px_void PX_MidiReset(PX_Midi* pmidi);
px_void PX_MidiSetQuarterNoteDuration(PX_Midi* pmidi, px_int qn);
px_void PX_MidiNoteToString( px_byte note,px_char out[8]);
px_void PX_MidiUpdate(PX_Midi* pmidi, px_dword elapsed);
px_void PX_MidiFree(PX_Midi* pmidi);

px_int PX_MidiGetTrackCount(PX_Midi* pmidi);
px_int PX_MidiReadTick(PX_Midi* pmidi, px_int iTrack);
px_int PX_MidiReadNote(PX_Midi* pmidi, px_int iTrack);
#endif