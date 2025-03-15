//Modbus RTU 
//Code by DBinary 2019-11-25 for PainterEngine

#ifndef PX_MODBUS_H
#define PX_MODBUS_H
#include "../core/PX_Core.h"

px_bool PX_ModbusRTU_WriteCoilFrame(px_memory *pframe, px_byte slaveAddr, px_word startAddr, px_bool b);
px_bool PX_ModbusRTU_WriteRegFrame(px_memory* pframe, px_byte slaveAddr, px_word startAddr, px_word regdata);
px_bool PX_ModbusRTU_ReadCoilFrame(px_memory* pframe, px_byte slaveAddr, px_word startAddr, px_word count);
px_bool PX_ModbusRTU_ReadRegFrame(px_memory* pframe, px_byte slaveAddr, px_word startAddr, px_word count);

px_bool PX_ModbusASCII_WriteCoilFrame(px_memory* pframe, px_byte slaveAddr, px_word startAddr, px_bool b);
px_bool PX_ModbusASCII_WriteRegFrame(px_memory* pframe, px_byte slaveAddr, px_word startAddr, px_word regdata);
px_bool PX_ModbusASCII_ReadCoilFrame(px_memory* pframe, px_byte slaveAddr, px_word startAddr, px_word count);
px_bool PX_ModbusASCII_ReadRegFrame(px_memory* pframe, px_byte slaveAddr, px_word startAddr, px_word count);
#endif

