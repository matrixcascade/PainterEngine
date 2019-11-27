#include "PX_MODBUS.h"

px_int PX_ModbusPacketBuild(PX_ModbusPacket *packet,px_byte addr,px_byte opcode,px_word startAddr,px_word regcount, px_byte *buffer,px_int size)
{
	packet->magic=0;
	packet->protocol=0;
	packet->size=size+9;
	packet->startAddress=startAddr;
	packet->regcount=regcount;
	packet->opcode=opcode;
	PX_memcpy(packet->data,buffer,size);
	return size+2;
}
