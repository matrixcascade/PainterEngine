#ifndef PLATFORM_WINDOWS_SERIALPORT_H
#define PLATFORM_WINDOWS_SERIALPORT_H
#include <Windows.h>

typedef struct
{
	HANDLE Handle;
	UINT BaudRate;
	UINT DataBits;
	char ParityType;
	UINT StopBit;
}PX_SerialPort;


const UCHAR* PX_SerialPortEnumComName(int index);
const int PX_SerialPortInitialize(PX_SerialPort *com,const char *name,unsigned int baudRate,unsigned int DataBits,char ParityType,unsigned int stopBit);
const int PX_SerialPortWrite(PX_SerialPort *com,void *data,int size);
const int PX_SerialPortRead(PX_SerialPort *com,void *data,int size);
void PX_SerialPortClose(PX_SerialPort *com);

#endif