#include "PX_Modbus.h"

PX_ModbusTCP_Write PX_ModbusTCPWriteSingleReg(px_word counter, px_byte unit, px_word startAddr, px_word regdata)
{
	PX_ModbusTCP_Write write;
	write.counter = counter;
	write.magic = 0;
	write.size[0] = 0;
	write.size[1] = 6;
	write.unit = unit;
	write.opcode = 0x06;
	write.startAddress[1] = startAddr & 0xff;
	write.startAddress[0] = (startAddr>>8) & 0xff;
	write.regdata[1] = regdata & 0xff;
	write.regdata[0] = (regdata >> 8) & 0xff;
	return write;
}

PX_ModbusTCP_Write PX_ModbusTCPWriteSingleBool(px_word counter, px_byte unit, px_word startAddr, px_bool b)
{
	PX_ModbusTCP_Write write;
	write.counter = counter;
	write.magic = 0;
	write.size[0] = 0;
	write.size[1] = 6;
	write.unit = unit;
	write.opcode = 0x06;
	write.startAddress[1] = startAddr & 0xff;
	write.startAddress[0] = (startAddr >> 8) & 0xff;
	if (b)
	{
		write.regdata[1] = 0xff;
		write.regdata[0] = 0xff;
	}
	else
	{
		write.regdata[1] = 0;
		write.regdata[0] = 0;
	}
	
	return write;
}


PX_ModbusTCP_Read PX_ModbusTCPReadReg(px_word counter, px_byte unit, px_word startAddr, px_int count)
{
	PX_ModbusTCP_Read read;
	read.counter = counter;
	read.magic = 0;
	read.size[0] = 0;
	read.size[1] = 6;
	read.unit = unit;
	read.opcode = 0x03;
	read.startAddress[1] = startAddr & 0xff;
	read.startAddress[0] = (startAddr >> 8) & 0xff;
	read.regcount[0] = (count>>8)&0xff;
	read.regcount[1] = count&0xff;
	return read;
}

PX_ModbusTCP_Read PX_ModbusTCPReadBool(px_word counter, px_byte unit, px_word startAddr, px_int count)
{
	PX_ModbusTCP_Read read;
	read.counter = counter;
	read.magic = 0;
	read.size[0] = 0;
	read.size[1] = 6;
	read.unit = unit;
	read.opcode = 0x01;
	read.startAddress[1] = startAddr & 0xff;
	read.startAddress[0] = (startAddr >> 8) & 0xff;
	read.regcount[0] = (count >> 8) & 0xff;
	read.regcount[1] = count & 0xff;
	return read;
}
