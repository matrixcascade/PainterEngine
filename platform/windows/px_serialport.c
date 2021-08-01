
#include "../modules/px_serialPort.h"
#include "windows.h"
const  char* PX_SerialPortEnumComName(int index)
{
	static  char szPortName[25];
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
			(LPBYTE)szPortName, &dwSizeofPortName);
		
		if((Status == ERROR_SUCCESS)||(Status == ERROR_MORE_DATA))
		{
			if (dwIndex==index)
			{
				return szPortName;
			}
		}
		dwIndex++;
	} while((Status == ERROR_SUCCESS)||(Status == ERROR_MORE_DATA));
	RegCloseKey(hKey);
	return NULL;
}

const BOOL PX_SerialPortInitialize(PX_SerialPort *com,const char *name,unsigned int baudRate,unsigned int DataBits,char ParityType,unsigned int stopBit)
{
	COMMTIMEOUTS  CommTimeouts;  
	DCB  dcb; 

	if (strlen(name)>4&&strlen(name)<6)
	{
		char comName[16]="\\\\.\\";
		strcat_s(comName,16,name);
		com->Handle=(long long)CreateFileA(comName, 
			GENERIC_READ | GENERIC_WRITE,   
			0,                            /** Share mode,No share if zero */   
			NULL,                         /** */   
			OPEN_EXISTING,                /** This device should be existing */   
			0,      
			0); 
	}
	else
	{
		com->Handle=(long long)CreateFileA(name, 
			GENERIC_READ | GENERIC_WRITE,   
			0,                            /** Share mode,No share if zero */   
			NULL,                         /** */   
			OPEN_EXISTING,                /** This device should be existing */   
			0,      
			0); 
	}
	     

	if (com->Handle==-1)
	{
		goto _ERROR;
	}

	
	CommTimeouts.ReadIntervalTimeout         = 0;  
	CommTimeouts.ReadTotalTimeoutMultiplier  = 0;  
	CommTimeouts.ReadTotalTimeoutConstant    = 0;  
	CommTimeouts.WriteTotalTimeoutMultiplier = 0;  
	CommTimeouts.WriteTotalTimeoutConstant   = 0;   

	if(!SetCommTimeouts((HANDLE)com->Handle, &CommTimeouts))
	{
		goto _ERROR;
	}

	if(!GetCommState((HANDLE)com->Handle, &dcb))
	{
		goto _ERROR;
	}

	//Setup BCD
	dcb.DCBlength = sizeof(DCB);
	dcb.ByteSize=DataBits;
	dcb.BaudRate=baudRate;
	dcb.StopBits=stopBit-1;
	switch (ParityType)
	{
	default:
	case 'N':
	case 'n':
		dcb.Parity=NOPARITY ;
		break;
	case 'O':
	case 'o':
		dcb.Parity=ODDPARITY;
		break;
	case 'E':
	case 'e':
		dcb.Parity=EVENPARITY;
		break;
	case 'M':
	case 'm':
		dcb.Parity=MARKPARITY;
		break;
	}
	
	dcb.fRtsControl = RTS_CONTROL_ENABLE;   

	com->BaudRate=baudRate;
	com->DataBits=DataBits;
	com->ParityType=ParityType;
	com->StopBit=stopBit;
	

	if(!SetCommState((HANDLE)com->Handle, &dcb))
	{
		goto _ERROR;
	}

	return 1;
_ERROR:
	CloseHandle((HANDLE)com->Handle);
	return 0;
}

const int PX_SerialPortReset(PX_SerialPort *com,unsigned int baudRate,unsigned int DataBits,char ParityType,unsigned int stopBit)
{
	DCB  dcb; 

	if(!GetCommState((HANDLE)com->Handle, &dcb))
	{
		goto _ERROR;
	}

	dcb.ByteSize=DataBits;
	dcb.BaudRate=baudRate;
	dcb.StopBits=stopBit-1;
	switch (ParityType)
	{
	default:
	case 'N':
	case 'n':
		dcb.Parity=NOPARITY ;
		break;
	case 'O':
	case 'o':
		dcb.Parity=ODDPARITY;
		break;
	case 'E':
	case 'e':
		dcb.Parity=EVENPARITY;
		break;
	case 'M':
	case 'm':
		dcb.Parity=MARKPARITY;
		break;
	}
	dcb.fRtsControl = RTS_CONTROL_ENABLE;   

	com->BaudRate=baudRate;
	com->DataBits=DataBits;
	com->ParityType=ParityType;
	com->StopBit=stopBit;

	if(!SetCommState((HANDLE)com->Handle, &dcb))
	{
		goto _ERROR;
	}

	return 1;
_ERROR:
	CloseHandle((HANDLE)com->Handle);
	return 0;
}



size_t PX_SerialPortGetBufferBytes(PX_SerialPort *com)
{
	DWORD dwError = 0;
	COMSTAT  comstat;  
	UINT BytesInQue = 0;  

	memset(&comstat, 0, sizeof(COMSTAT));  

	
	if ( ClearCommError((HANDLE)com->Handle, &dwError, &comstat) )  
	{  
		BytesInQue = comstat.cbInQue; 
	}  

	return BytesInQue;  
}

const int PX_SerialPortWrite(PX_SerialPort *com,void *data,int size)
{
	DWORD WriteSize=0;

	if (com->Handle==-1)
	{
		return 0;
	}
	

	if (!WriteFile((HANDLE)com->Handle, data, size, &WriteSize, NULL))    
	{  
		DWORD dwError = GetLastError();  
		PurgeComm((HANDLE)com->Handle, PURGE_RXCLEAR | PURGE_RXABORT);
		return WriteSize;  
	} 
	return WriteSize;
}

const int PX_SerialPortRead(PX_SerialPort *com,void *data,int size)
{
	int     Offset;
	int	QueSize;
	DWORD   rSize;

	if(com->Handle!=-1)
	{
		int BytesInQue = (int)PX_SerialPortGetBufferBytes(com);  

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
			ReadFile((HANDLE)com->Handle,(char *)data+Offset,QueSize,&rSize,NULL);
			Offset+=rSize;
			QueSize-=rSize;
		}
		return Offset;
	}
	return 0;
}

void PX_SerialPortClose(PX_SerialPort *com)
{
	if (com->Handle!=0&&com->Handle!=-1)
	{
		CloseHandle((HANDLE)com->Handle);
		com->Handle=0;
	}
	
}

