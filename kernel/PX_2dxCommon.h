#ifndef PX_2DXCOMMON_H
#define PX_2DXCOMMON_H
#include "../core/PX_Core.h"
typedef struct
{
	px_dword magic;
	px_dword framecount;
	px_dword animationCount;
}PX_2DX_Header;


typedef struct  
{
	px_dword size;
}PX_2DX_CODE_Header;

typedef enum
{
	PX_2DX_OPCODE_FRAME =1,
	PX_2DX_OPCODE_SLEEP,
	PX_2DX_OPCODE_GOTO,
	PX_2DX_OPCODE_LOOP,
	PX_2DX_OPCODE_END,
	PX_2DX_OPCODE_CLIPX,
	PX_2DX_OPCODE_CLIPY,
	PX_2DX_OPCODE_CLIPW,
	PX_2DX_OPCODE_CLIPH,
	PX_2DX_OPCODE_CLIPI,
	PX_2DX_OPCODE_CLIPINC,
	PX_2DX_OPCODE_CLIPDEC,
}PX_2DX_OPCODE;

//Instrument format
//opcode 1byte
//other  3byte

typedef struct
{
	px_word  opcode;
	px_word  param;
}PX_2DX_INSTR;

//frame [index] //opcode 1 byte //param  3 byte
//clipx [x] //opcode 1 byte //param  3 byte
//clipy [y] //opcode 1 byte //param  3 byte
//clipw [width] //opcode 1 byte //param  3 byte
//cliph [height] //opcode 1 byte //param  3 byte
//clipI [index] //opcode 1 byte //param  3 byte
//sleep [index] //opcode 1 byte //param  3 byte
//goto  [index] //jmp addr 3 byte //loop times 4 byte

#endif
