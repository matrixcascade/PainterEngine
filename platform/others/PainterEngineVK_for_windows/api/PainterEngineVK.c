#include "PainterEngineVK.h"

HANDLE hDriver = INVALID_HANDLE_VALUE;
px_bool __is_64bits_OS;
TCHAR DriverPath[MAX_PATH];


typedef UINT(WINAPI* GETSYSTEMWOW64DIRECTORY)(LPTSTR, UINT);

#define SCAN_CODE_STACK_SIZE 1024
#define SEARCH_LOG_SIZE 1024
typedef struct PX_VKdata_Intialize
{
	ULONG ret;
	char content[SEARCH_LOG_SIZE];
}PX_VKdata_Intialize;

typedef struct PX_VKdata_AddEvent
{
	ULONG scan_code;
	ULONG Iskeydown;
	ULONG ret;
}PX_VKdata_AddEvent;

typedef struct PX_VKdata_GetEvent
{
	USHORT scan_code_stack[SCAN_CODE_STACK_SIZE];
	ULONG  scan_code_size;
}PX_VKdata_GetEvent;

typedef enum
{
	PX_DRIVER_IOCTL_OPCODE_INIT = 0x801,
	PX_DRIVER_IOCTL_OPCODE_ADDKEYEVENT = 0x802,
	PX_DRIVER_IOCTL_OPCODE_GETKEYEVENT = 0x803,
}PX_DRIVER_IOCTL_OPCODE;

#define PX_DRIVER_IOCTL_CODE_INIT CTL_CODE(FILE_DEVICE_UNKNOWN, PX_DRIVER_IOCTL_OPCODE_INIT, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define PX_DRIVER_IOCTL_CODE_ADDEVENT CTL_CODE(FILE_DEVICE_UNKNOWN, PX_DRIVER_IOCTL_OPCODE_ADDKEYEVENT, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define PX_DRIVER_IOCTL_CODE_GETEVENT CTL_CODE(FILE_DEVICE_UNKNOWN, PX_DRIVER_IOCTL_OPCODE_GETKEYEVENT, METHOD_BUFFERED, FILE_ANY_ACCESS)


static px_bool PX_GetDriverPath()
{
	LPTSTR pszSlash;

	if (!GetModuleFileName(GetModuleHandle(NULL), DriverPath, sizeof(DriverPath)))
		return PX_FALSE;

	pszSlash = _tcsrchr(DriverPath, TEXT('\\'));

	if (pszSlash)
		pszSlash[1] = 0;
	else
		return PX_FALSE;

	if (__is_64bits_OS)
		_tcscat_s(DriverPath, MAX_PATH, TEXT("PainterEngineVK64.sys"));
	else
		_tcscat_s(DriverPath, MAX_PATH, TEXT("PainterEngineVK32.sys"));

	return PX_TRUE;
}

static px_bool  PX_StopPainterEngineVKDriver()
{
	SC_HANDLE hSCManager;
	SC_HANDLE hService;
	SERVICE_STATUS ServiceStatus;
	px_bool bResult;

	hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

	if (hSCManager)
	{
		hService = OpenService(hSCManager, TEXT("PainterEngineVK"), SERVICE_ALL_ACCESS);

		CloseServiceHandle(hSCManager);

		if (hService)
		{
			DWORD lasterr;
			bResult = ControlService(hService, SERVICE_CONTROL_STOP, &ServiceStatus);
			lasterr = GetLastError();
			CloseServiceHandle(hService);
		}
		else
			return PX_FALSE;
	}
	else
		return PX_FALSE;

	return bResult;
}

px_bool PX_PainterEngineVKUninstall()
{
	SC_HANDLE hSCManager;
	SC_HANDLE hService;
	LPQUERY_SERVICE_CONFIG pServiceConfig;
	DWORD dwBytesNeeded=0;
	DWORD cbBufSize=0;
	px_bool bResult=0;

	PX_StopPainterEngineVKDriver();

	hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

	if (!hSCManager)
	{
		return PX_FALSE;
	}

	hService = OpenService(hSCManager, _T("PainterEngineVK"), SERVICE_ALL_ACCESS);
	CloseServiceHandle(hSCManager);

	if (!hService)
	{
		return PX_FALSE;
	}

	QueryServiceConfig(hService, NULL, 0, &dwBytesNeeded);

	if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
	{
		cbBufSize = dwBytesNeeded;
		pServiceConfig = (LPQUERY_SERVICE_CONFIG)malloc(cbBufSize);

		if (!pServiceConfig)
		{
			return PX_FALSE;
		}

		bResult = QueryServiceConfig(hService, pServiceConfig, cbBufSize, &dwBytesNeeded);

		if (!bResult)
		{
			free(pServiceConfig);
			CloseServiceHandle(hService);
			return bResult;
		}

		if (pServiceConfig->dwStartType == SERVICE_DEMAND_START)
		{
			bResult = DeleteService(hService);
		}
	}

	CloseServiceHandle(hService);

	return bResult;
}

static px_bool PX_InstallPainterEngineVKDriver(PTSTR pDriverPath, px_bool IsDemandLoaded)
{
	SC_HANDLE hSCManager;
	SC_HANDLE hService;

	// Remove any previous instance of the driver

	PX_PainterEngineVKUninstall();

	hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

	if (hSCManager)
	{
		// Install the driver

		hService = CreateService(hSCManager,
			TEXT("PainterEngineVK"),
			TEXT("PainterEngineVK"),
			SERVICE_ALL_ACCESS,
			SERVICE_KERNEL_DRIVER,
			(IsDemandLoaded) ? SERVICE_DEMAND_START : SERVICE_SYSTEM_START,
			SERVICE_ERROR_NORMAL,
			pDriverPath,
			NULL,
			NULL,
			NULL,
			NULL,
			NULL);

		CloseServiceHandle(hSCManager);

		if (hService == NULL)
		{
			DWORD lasterr = GetLastError();
			if (lasterr==1073)
			{
				return PX_TRUE;
			}
			return PX_FALSE;
		}
			
	}
	else
		return PX_FALSE;

	CloseServiceHandle(hService);

	return PX_TRUE;
}

static px_bool  PX_StartPainterEngineVKDriver()
{
	SC_HANDLE hSCManager;
	SC_HANDLE hService;
	px_bool bResult;

	hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

	if (hSCManager)
	{
		hService = OpenService(hSCManager, TEXT("PainterEngineVK"), SERVICE_ALL_ACCESS);

		CloseServiceHandle(hSCManager);

		if (hService)
		{
			DWORD lasterr;
			bResult = StartService(hService, 0, NULL) || GetLastError() == ERROR_SERVICE_ALREADY_RUNNING;
			lasterr = GetLastError();
			CloseServiceHandle(hService);
		}
		else
			return PX_FALSE;
	}
	else
		return PX_FALSE;

	return bResult;
}

static px_bool PX_Is64BitOS()
{
#ifdef _WIN64
	return PX_TRUE;
#else
	GETSYSTEMWOW64DIRECTORY getSystemWow64Directory;
	HMODULE hKernel32;
	TCHAR Wow64Directory[MAX_PATH];

	hKernel32 = GetModuleHandle(TEXT("kernel32.dll"));
	if (hKernel32 == NULL)
	{
		return PX_FALSE;
	}

	getSystemWow64Directory = (GETSYSTEMWOW64DIRECTORY)GetProcAddress(hKernel32, TEXT("GetSystemWow64DirectoryW"));

	if (getSystemWow64Directory == NULL)
	{
		return PX_FALSE;
	}

	if ((getSystemWow64Directory(Wow64Directory, _countof(Wow64Directory)) == 0) &&
		(GetLastError() == ERROR_CALL_NOT_IMPLEMENTED)) {
		return FALSE;
	}
	return PX_TRUE;
#endif
}

px_bool PX_PainterEngineVKInstall()
{
	px_bool bResult;
	DWORD dwBytesReturned;

	__is_64bits_OS = PX_Is64BitOS();

	hDriver = CreateFile(TEXT("\\\\.\\PainterEngineVK"),
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (hDriver == INVALID_HANDLE_VALUE)
	{
		PX_GetDriverPath();

		bResult = PX_InstallPainterEngineVKDriver(DriverPath, PX_TRUE);

		if (!bResult)
			return PX_FALSE;

		bResult = PX_StartPainterEngineVKDriver();

		if (!bResult)
			return PX_FALSE;

		hDriver = CreateFile(TEXT("\\\\.\\PainterEngineVK"),
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL);

		if (hDriver == INVALID_HANDLE_VALUE)
			return PX_FALSE;
	}

	do
	{
		PX_VKdata_Intialize io = {0};
		if (!DeviceIoControl(hDriver, PX_DRIVER_IOCTL_CODE_INIT, &io,
			sizeof(io), &io, sizeof(io), &dwBytesReturned, NULL))
		{
			return PX_FALSE;
		}
		if (dwBytesReturned!= sizeof(io))
		{
			return PX_FALSE;
		}
		if (io.ret==0)
		{
			return PX_FALSE;
		}
	} while (0);
	return PX_TRUE;
}

px_bool PX_PainterEngineVKAddKeyEvent(px_uint scan_code,px_bool isKeydown)
{
	PX_VKdata_AddEvent dataio = { 0 };
	DWORD dwBytesReturned;
	dataio.scan_code = scan_code;
	dataio.Iskeydown = isKeydown;
	dataio.ret = 0;
	if (!DeviceIoControl(hDriver, PX_DRIVER_IOCTL_CODE_ADDEVENT, &dataio,
		sizeof(dataio), &dataio, sizeof(dataio), &dwBytesReturned, NULL))
	{
		return PX_FALSE;
	}
	if (dwBytesReturned != sizeof(dataio))
	{
		return PX_FALSE;
	}
	if (dataio.ret == 0)
	{
		return PX_FALSE;
	}
	return PX_TRUE;
}

px_uint PX_PainterEngineVKGetKeyEvent(px_word scan_code[], px_uint size)
{
	PX_VKdata_GetEvent dataio = { 0 };
	DWORD dwBytesReturned;
	px_uint i;
	if (!DeviceIoControl(hDriver, PX_DRIVER_IOCTL_CODE_GETEVENT, &dataio,
		sizeof(dataio), &dataio, sizeof(dataio), &dwBytesReturned, NULL))
	{
		return PX_FALSE;
	}
	if (dwBytesReturned != sizeof(dataio))
	{
		return PX_FALSE;
	}
	if (size> dataio.scan_code_size)
	{
		size = dataio.scan_code_size;
	}
	for (i=0;i<size;i++)
	{
		scan_code[i] = dataio.scan_code_stack[i];
	}
	return size;
}


px_bool PX_PainterEngineVKKeyDown(px_uint scan_code)
{
	return PX_PainterEngineVKAddKeyEvent(scan_code, 1);
}

px_bool PX_PainterEngineVKKeyUp(px_uint scan_code)
{
	return PX_PainterEngineVKAddKeyEvent(scan_code, 0);
}
