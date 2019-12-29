#include "PX_MODBUS.h"

px_int PX_ModbusPacketBuild(PX_ModbusPacket *packet,px_byte opcode,px_word startAddr,px_word regcount, px_byte *buffer,px_int size)
{
	packet->magic=0;
	packet->protocol=0;
	packet->size=size+6;
	packet->startAddress[0]=0xff&(startAddr>>8);
	packet->startAddress[1]=0xff&(startAddr);
	packet->regcount[0]=0xff&(regcount>>8);
	packet->regcount[1]=0xff&(regcount);
	packet->opcode=opcode;
	packet->unit=1;
	if(size)
	PX_memcpy(packet->data,buffer,size);
	return size+9;
}

px_int PX_ModbusWrite(PX_ModbusPacket *packet,px_word startAddr,px_word regcount, px_word *buffer,px_int size)
{
	return PX_ModbusPacketBuild(packet,0x06,startAddr,regcount,(px_byte *)buffer,size*2);
}

px_int PX_ModbusRead(PX_ModbusPacket *packet,px_word startAddr,px_word regcount)
{
	return PX_ModbusPacketBuild(packet,0x03,startAddr,regcount,"",0);
}
