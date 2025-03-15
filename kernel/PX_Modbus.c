#include "PX_Modbus.h"
px_bool PX_ModbusRTU_WriteCoilFrame(px_memory* pframe, px_byte slaveAddr, px_word startAddr, px_bool b)
{
	px_byte frame[8];
	px_word crc;
	px_int i = 0;
	frame[i++] = slaveAddr;
	frame[i++] = 0x05;
	frame[i++] = (startAddr >> 8) & 0xff;
	frame[i++] = startAddr & 0xff;
	frame[i++] = b ? 0xff : 0x00;
	frame[i++] = 0x00;
	crc = PX_crc16(frame, i);
	frame[i++] = crc & 0xff;
	frame[i++] = (crc >> 8) & 0xff;
	return PX_MemoryCat(pframe, frame, i);
}
px_bool PX_ModbusRTU_WriteRegFrame(px_memory* pframe, px_byte slaveAddr, px_word startAddr, px_word regdata)
{
	px_byte frame[8];
	px_word crc;
	px_int i = 0;
	frame[i++] = slaveAddr;
	frame[i++] = 0x06;
	frame[i++] = (startAddr >> 8) & 0xff;
	frame[i++] = startAddr & 0xff;
	frame[i++] = (regdata >> 8) & 0xff;
	frame[i++] = regdata & 0xff;
	crc = PX_crc16(frame, i);
	frame[i++] = crc & 0xff;
	frame[i++] = (crc >> 8) & 0xff;
	return PX_MemoryCat(pframe, frame, i);
	
}
px_bool PX_ModbusRTU_ReadCoilFrame(px_memory* pframe, px_byte slaveAddr, px_word startAddr, px_word count)
{
	px_byte frame[8];
	px_word crc;
	px_int i = 0;
	frame[i++] = slaveAddr;
	frame[i++] = 0x01;
	frame[i++] = (startAddr >> 8) & 0xff;
	frame[i++] = startAddr & 0xff;
	frame[i++] = (count >> 8) & 0xff;
	frame[i++] = count & 0xff;
	crc = PX_crc16(frame, i);
	frame[i++] = crc & 0xff;
	frame[i++] = (crc >> 8) & 0xff;
	return PX_MemoryCat(pframe, frame, i);
}
px_bool PX_ModbusRTU_ReadRegFrame(px_memory* pframe, px_byte slaveAddr, px_word startAddr, px_word count)
{
	px_byte frame[8];
	px_word crc;
	px_int i = 0;
	frame[i++] = slaveAddr;
	frame[i++] = 0x03;
	frame[i++] = (startAddr >> 8) & 0xff;
	frame[i++] = startAddr & 0xff;
	frame[i++] = (count >> 8) & 0xff;
	frame[i++] = count & 0xff;
	crc = PX_crc16(frame, i);
	frame[i++] = crc & 0xff;
	frame[i++] = (crc >> 8) & 0xff;
	return PX_MemoryCat(pframe, frame, i);
}

px_bool PX_ModbusASCII_WriteCoilFrame(px_memory* pframe, px_byte slaveAddr, px_word startAddr, px_bool b)
{
	px_byte frame[10];
	px_char ascii_frame[33] = {0};
	px_word crc;
	px_int i = 0;
	frame[i++] = slaveAddr;
	frame[i++] = 0x05;
	frame[i++] = (startAddr >> 8) & 0xff;
	frame[i++] = startAddr & 0xff;
	frame[i++] = b ? 0xff : 0x00;
	frame[i++] = 0x00;
	crc = PX_crc16(frame, i);
	frame[i++] = crc & 0xff;
	frame[i++] = (crc >> 8) & 0xff;
	frame[i++] = 0x0d;
	frame[i++] = 0x0a;
	PX_BufferToHexString( frame, i, ascii_frame);
	return PX_MemoryCat(pframe, ascii_frame, i * 2);
}
px_bool PX_ModbusASCII_WriteRegFrame(px_memory* pframe, px_byte slaveAddr, px_word startAddr, px_word regdata)
{
	px_byte frame[10];
	px_char ascii_frame[33] = { 0 };
	px_word crc;
	px_int i = 0;
	frame[i++] = slaveAddr;
	frame[i++] = 0x06;
	frame[i++] = (startAddr >> 8) & 0xff;
	frame[i++] = startAddr & 0xff;
	frame[i++] = (regdata >> 8) & 0xff;
	frame[i++] = regdata & 0xff;
	crc = PX_crc16(frame, i);
	frame[i++] = crc & 0xff;
	frame[i++] = (crc >> 8) & 0xff;
	frame[i++] = 0x0d;
	frame[i++] = 0x0a;
	PX_BufferToHexString(frame, i, ascii_frame);
	return PX_MemoryCat(pframe, ascii_frame, i * 2);
}

px_bool PX_ModbusASCII_ReadCoilFrame(px_memory* pframe, px_byte slaveAddr, px_word startAddr, px_word count)
{
	px_byte frame[10];
	px_char ascii_frame[33] = { 0 };
	px_word crc;
	px_int i = 0;
	frame[i++] = slaveAddr;
	frame[i++] = 0x01;
	frame[i++] = (startAddr >> 8) & 0xff;
	frame[i++] = startAddr & 0xff;
	frame[i++] = (count >> 8) & 0xff;
	frame[i++] = count & 0xff;
	crc = PX_crc16(frame, i);
	frame[i++] = crc & 0xff;
	frame[i++] = (crc >> 8) & 0xff;
	frame[i++] = 0x0d;
	frame[i++] = 0x0a;
	PX_BufferToHexString(frame, i, ascii_frame);
	return PX_MemoryCat(pframe, ascii_frame, i * 2);
}

px_bool PX_ModbusASCII_ReadRegFrame(px_memory* pframe, px_byte slaveAddr, px_word startAddr, px_word count)
{
	px_byte frame[10];
	px_char ascii_frame[33] = { 0 };
	px_word crc;
	px_int i = 0;
	frame[i++] = slaveAddr;
	frame[i++] = 0x03;
	frame[i++] = (startAddr >> 8) & 0xff;
	frame[i++] = startAddr & 0xff;
	frame[i++] = (count >> 8) & 0xff;
	frame[i++] = count & 0xff;
	crc = PX_crc16(frame, i);
	frame[i++] = crc & 0xff;
	frame[i++] = (crc >> 8) & 0xff;
	frame[i++] = 0x0d;
	frame[i++] = 0x0a;
	PX_BufferToHexString(frame, i, ascii_frame);
	return PX_MemoryCat(pframe, ascii_frame, i * 2);
}