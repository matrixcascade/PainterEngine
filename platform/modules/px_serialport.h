
#ifndef PX_SERIALPORT_H
#define PX_SERIALPORT_H

typedef struct
{
	long long Handle;
	unsigned int BaudRate;
    unsigned int DataBits;
	char ParityType;
	unsigned int StopBit;
}PX_SerialPort;


const char* PX_SerialPortEnumComName(int index);
const int PX_SerialPortInitialize(PX_SerialPort *com,const char *name,unsigned int baudRate,unsigned int DataBits/*5,6,7,8*/,char ParityType/*'O','E','N'*/,unsigned int stopBit/*0,1,2*/);
const int PX_SerialPortReset(PX_SerialPort *com,unsigned int baudRate,unsigned int DataBits/*5,6,7,8*/,char ParityType/*'O','E','N'*/,unsigned int stopBit/*0,1,2*/);
const int PX_SerialPortWrite(PX_SerialPort *com,void *data,int size);
const int PX_SerialPortRead(PX_SerialPort *com,void *data,int size);
void PX_SerialPortClose(PX_SerialPort *com);
#endif
