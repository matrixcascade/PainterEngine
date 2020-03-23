#include "Platform_Windows_SerialPort.h"

const UCHAR* PX_SerialPortEnumComName(int index)
{
	static UCHAR szPortName[25];
	HKEY hKey;
	CHAR Name[25];
	LONG Status;
	DWORD dwIndex = 0;
	DWORD dwName;
	DWORD dwSizeofPortName;
	DWORD Type;

	//RegDisableReflectionKey(HKEY_LOCAL_MACHINE);
	LPCSTR data_Set="HARDWARE\\DEVICEMAP\\SERIALCOMM\\";
	long ret0 = (RegOpenKeyExA(HKEY_LOCAL_MACHINE, data_Set, 0, KEY_READ, &hKey));
	if(ret0 != ERROR_SUCCESS)
	{
		return 0;
	}

	do
	{
		dwName = sizeof(Name);
		dwSizeofPortName = sizeof(szPortName);

		Status = RegEnumValueA(hKey, dwIndex, Name, &dwName, NULL, &Type,
			szPortName, &dwSizeofPortName);
		dwIndex++;
		if((Status == ERROR_SUCCESS)||(Status == ERROR_MORE_DATA))
		{
			if (dwIndex==index)
			{
				return szPortName;
			}
		}
	} while((Status == ERROR_SUCCESS)||(Status == ERROR_MORE_DATA));
	RegCloseKey(hKey);
	return NULL;
}

const BOOL PX_SerialPortInitialize(PX_SerialPort *com,const char *name,unsigned int baudRate,unsigned int DataBits,char ParityType,unsigned int stopBit)
{
	COMMTIMEOUTS  CommTimeouts;  
	DCB  dcb; 

	com->Handle=CreateFileA(name, 
		GENERIC_READ | GENERIC_WRITE,   
		0,                            /** Share mode,No share if zero */   
		NULL,                         /** */   
		OPEN_EXISTING,                /** This device should be existing */   
		0,      
		0);      

	if (com->Handle==INVALID_HANDLE_VALUE)
	{
		goto _ERROR;
	}

	
	CommTimeouts.ReadIntervalTimeout         = 0;  
	CommTimeouts.ReadTotalTimeoutMultiplier  = 0;  
	CommTimeouts.ReadTotalTimeoutConstant    = 0;  
	CommTimeouts.WriteTotalTimeoutMultiplier = 0;  
	CommTimeouts.WriteTotalTimeoutConstant   = 0;   

	if(!SetCommTimeouts(com->Handle, &CommTimeouts))
	{
		goto _ERROR;
	}

	if(!GetCommState(com->Handle, &dcb))
	{
		goto _ERROR;
	}

	//Setup BCD
	dcb.ByteSize=DataBits;
	dcb.BaudRate=baudRate;
	dcb.StopBits=stopBit;
	dcb.Parity=ParityType;
	dcb.fRtsControl = RTS_CONTROL_ENABLE;   

	com->BaudRate=baudRate;
	com->DataBits=DataBits;
	com->ParityType=ParityType;
	com->StopBit=stopBit;

	if(!SetCommState(com->Handle, &dcb))
	{
		goto _ERROR;
	}

	return 1;
_ERROR:
	CloseHandle(com->Handle);
	return 0;
}

size_t PX_SerialPortGetBufferBytes(PX_SerialPort *com)
{
	DWORD dwError = 0;
	COMSTAT  comstat;  
	UINT BytesInQue = 0;  

	memset(&comstat, 0, sizeof(COMSTAT));  

	
	if ( ClearCommError(com->Handle, &dwError, &comstat) )  
	{  
		BytesInQue = comstat.cbInQue; 
	}  

	return BytesInQue;  
}

const int PX_SerialPortWrite(PX_SerialPort *com,void *data,int size)
{
	DWORD WriteSize=0;

	if (com->Handle==INVALID_HANDLE_VALUE)
	{
		return 0;
	}
	

	if (!WriteFile(com->Handle, data, size, &WriteSize, NULL))    
	{  
		DWORD dwError = GetLastError();  
		PurgeComm(com->Handle, PURGE_RXCLEAR | PURGE_RXABORT);
		return WriteSize;  
	} 
	return WriteSize;
}

const BOOL PX_SerialPortRead(PX_SerialPort *com,void *data,int size)
{
	int     Offset;
	int	QueSize,rSize;

	if(com->Handle!=INVALID_HANDLE_VALUE)
	{
		UINT BytesInQue = PX_SerialPortGetBufferBytes(com);  

		/*if Buffer is empty,sleep 10ms*/
		if ( BytesInQue == 0 )  
		{  
			return 0;
		}  

		Offset=0;
		QueSize=BytesInQue;

		if (QueSize>size)
		{
			QueSize=size;
			BytesInQue=QueSize;
		}

		while(QueSize>0)
		{
			ReadFile(com->Handle,(char *)data+Offset,QueSize,&rSize,NULL);
			Offset+=rSize;
			QueSize-=rSize;
		}
		return Offset;
	}
	return 0;
}

void PX_SerialPortClose(PX_SerialPort *com)
{
	if (com->Handle!=0&&com->Handle!=INVALID_HANDLE_VALUE)
	{
		CloseHandle(com->Handle);
		com->Handle=NULL;
	}
	
}

