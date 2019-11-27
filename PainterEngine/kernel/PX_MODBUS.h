//Modbus RTU 
//Code by DBinary 2019-11-25 for PainterEngine

#ifndef PX_MODBUS_H
#define PX_MODBUS_H
#include "../core/PX_Core.h"


typedef struct
{
	px_word  protocol;
	px_word  magic;
	px_byte size;
	px_byte unit;
	px_byte opcode;
	px_word startAddress;
	px_word regcount;
	px_byte data[250];
}PX_ModbusPacket;

px_int PX_ModbusPacketBuild(PX_ModbusPacket *packet,px_byte addr,px_byte opcode,px_word startAddr,px_word regcount, px_byte *buffer,px_int size); 

#endif

