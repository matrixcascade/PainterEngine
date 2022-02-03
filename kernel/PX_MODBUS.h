//Modbus RTU 
//Code by DBinary 2019-11-25 for PainterEngine

#ifndef PX_MODBUS_H
#define PX_MODBUS_H
#include "../core/PX_Core.h"


typedef struct
{
	px_word  counter;
	px_word  magic;
	px_byte  size[2];
	px_byte unit;
	px_byte opcode;
	px_byte startAddress[2];
	px_byte regcount[2];
}PX_ModbusTCP_Read;

typedef struct
{
	px_word  counter;
	px_word  magic;
	px_byte  size[2];//5
	px_byte unit;
	px_byte opcode;//03
	px_byte bytesize;//02
}PX_ModbusTCP_ReadResponse;



typedef struct
{
	px_word  counter;
	px_word  magic;
	px_byte  size[2];
	px_byte unit;
	px_byte opcode;
	px_byte startAddress[2];
	px_byte regcount[2];
}PX_ModbusTCP_ReadBoolResponse;



typedef struct
{
	px_word  counter;
	px_word  magic;
	px_byte  size[2];
	px_byte unit;
	px_byte opcode;
	px_byte startAddress[2];
	px_byte regdata[2];
}PX_ModbusTCP_Write;

PX_ModbusTCP_Write PX_ModbusTCPWriteSingleReg(px_word counter,px_byte unit,px_word startAddr,px_word regdata);
PX_ModbusTCP_Write PX_ModbusTCPWriteSingleBool(px_word counter, px_byte unit, px_word startAddr, px_bool b);
PX_ModbusTCP_Read PX_ModbusTCPReadReg(px_word counter, px_byte unit, px_word startAddr,px_int count);
PX_ModbusTCP_Read PX_ModbusTCPReadBool(px_word counter, px_byte unit, px_word startAddr, px_int count);
#endif

