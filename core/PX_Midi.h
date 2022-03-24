#ifndef PX_MIDI_H
#define PX_MIDI_H
#include "PX_Vector.h"
#include "PX_Memory.h"

struct _PX_Midi;

typedef px_void (*PX_MidiCallback)(struct _PX_Midi* midi, px_byte opcode,px_byte  track,px_byte  instrument, px_byte Note, px_byte velocity, px_void* userptr);

typedef enum
{
	PX_MIDI_NEXT_TYPE_TICK,
	PX_MIDI_NEXT_TYPE_OPCODE,
	PX_MIDI_NEXT_TYPE_END,
	PX_MIDI_NEXT_TYPE_ERROR,
}PX_MIDI_NEXT_TYPE;

typedef enum 
{
	PX_MIDI_OPCODE_KEYDOWN,
	PX_MIDI_OPCODE_KEYUP,
	PX_MIDI_OPCODE_UNDEFINE,
}PX_MIDI_OPCODE;

typedef struct 
{
	px_byte instrument;
	px_float wait_tick;
	px_byte *payload;
	px_int ip;
	px_int payloadSize;
	px_byte lastPlayload;
	PX_MIDI_NEXT_TYPE nextType;
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
PX_MIDI_NEXT_TYPE PX_MidiGetNextType(PX_Midi* pmidi, px_int track);
px_void PX_MidiFree(PX_Midi* pmidi);

px_int PX_MidiGetTrackCount(PX_Midi* pmidi);
px_int PX_MidiReadTick(PX_Midi* pmidi, px_int iTrack);

typedef struct  
{
	PX_MIDI_OPCODE opcode;
	px_int note;
	px_int v;
	px_int track;
}PX_Midi_Note;
PX_Midi_Note PX_MidiReadNote(PX_Midi* pmidi, px_int iTrack);
px_int PX_MidiNoteToPianoKey(px_int note);
#endif