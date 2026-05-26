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
	frame[i++] = (crc >> 8) & 0xff;
	frame[i++] = crc & 0xff;
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
	frame[i++] = (crc >> 8) & 0xff;
	frame[i++] = crc & 0xff;
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
	frame[i++] = (crc >> 8) & 0xff;
	frame[i++] = crc & 0xff;
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
	frame[i++] = (crc >> 8) & 0xff;
	frame[i++] = crc & 0xff;
	return PX_MemoryCat(pframe, frame, i);
}

px_int PX_ModbusRTU_ParseRecvFrame(const px_byte data[], px_int data_size)
{
	if (data_size < 5)
	{
		return 0; // Frame too short
	}
	px_byte slaveAddr = data[0];
	px_byte functionCode = data[1];
	px_word crc = (data[data_size - 2] << 8) | data[data_size - 1];
	// Validate CRC
	if (PX_crc16(data, data_size - 2) != crc)
	{
		return 0; // CRC mismatch
	}
	if (functionCode == 0x01 || functionCode == 0x03) // Read Coils or Read Registers
	{
		if (data_size < 5 + data[2]) // Data length check
		{
			return 0;
		}

		return data[2];
	}
	else if (functionCode == 0x05 || functionCode == 0x06) // Write Coil or Write Register
	{
		if (data_size < 8) // Minimum size for write responses
		{
			return 0;
		}

		return 4;
	}
	return 0;
	
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
	frame[i++] = (crc >> 8) & 0xff;
	frame[i++] = crc & 0xff;
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
	frame[i++] = (crc >> 8) & 0xff;
	frame[i++] = crc & 0xff;
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
	frame[i++] = (crc >> 8) & 0xff;
	frame[i++] = crc & 0xff;
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
	frame[i++] = (crc >> 8) & 0xff;
	frame[i++] = crc & 0xff;
	frame[i++] = 0x0d;
	frame[i++] = 0x0a;
	PX_BufferToHexString(frame, i, ascii_frame);
	return PX_MemoryCat(pframe, ascii_frame, i * 2);
}