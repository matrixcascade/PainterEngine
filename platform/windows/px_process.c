#include "px_process.h"


int PX_ProcessIsEnd(HANDLE hProcess)
{
	DWORD exitCode;
    if (hProcess==0)
    {
        return 1;
    }
    if (!GetExitCodeProcess(hProcess, &exitCode))
    {
        return 1;
    }
	return 0;
}

int PX_ProcessRead(HANDLE hOutputRead, char buffer[], int buffersize)
{
    DWORD bytesRead;

    if (ReadFile(hOutputRead, buffer, buffersize - 1, &bytesRead, NULL) && bytesRead > 0) {
        buffer[bytesRead] = '\0';
        return bytesRead;
    }
    return 0;
}

int PX_ProcessWrite(HANDLE hInputWrite, char buffer[], int buffersize)
{
    DWORD bytesWrite;

    if (WriteFile(hInputWrite, buffer, buffersize - 1, &bytesWrite, NULL) && bytesWrite > 0) 
    {
        return bytesWrite;
    }
    return 0;
}

typedef struct {
    HANDLE hpiProcess;
    HANDLE hpiThread;
    HANDLE hread;
    HANDLE hwrite;
    HANDLE hthread;
}END_PROCESS_INFORMATION;

static void ThreadEndOfProcess(void* ptr)
{

    END_PROCESS_INFORMATION * end_process_information = (END_PROCESS_INFORMATION*)ptr;
    WaitForSingleObject(end_process_information->hpiProcess, INFINITE);
    CloseHandle(end_process_information->hread);
    CloseHandle(end_process_information->hwrite);
    CloseHandle(end_process_information->hthread);
    CloseHandle(end_process_information->hpiProcess);
    CloseHandle(end_process_information->hpiThread);
    free(end_process_information);
}

int PX_ProcessRun(char exec_path[],HANDLE *process_handle,HANDLE * hChildStdoutRead,HANDLE * hChildStdinWrite)
{
    END_PROCESS_INFORMATION* info;
    HANDLE hChildStdinReadDummy=0, hChildStdoutWriteDummy=0;
    SECURITY_ATTRIBUTES saAttr;
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    DWORD dwMode;
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;

    if (!CreatePipe(&hChildStdinReadDummy, hChildStdinWrite, &saAttr, 0)) {
        return 0;
    }

    if (!CreatePipe(hChildStdoutRead, &hChildStdoutWriteDummy, &saAttr, 0)) {
        return 0;
    }

    dwMode = PIPE_READMODE_BYTE | PIPE_NOWAIT;

    if (!SetNamedPipeHandleState(*hChildStdoutRead, &dwMode, NULL, NULL)) {
        return 0;
    }


    if (!SetHandleInformation(*hChildStdinWrite, HANDLE_FLAG_INHERIT, 0)) {
        return 0;
    }

    if (!SetHandleInformation(*hChildStdoutRead, HANDLE_FLAG_INHERIT, 0)) {
        return 0;
    }

    

    ZeroMemory(&si, sizeof(STARTUPINFO));
    si.cb = sizeof(STARTUPINFO);
    si.hStdInput = hChildStdinReadDummy;
    si.hStdOutput = hChildStdoutWriteDummy;
    si.dwFlags |= STARTF_USESTDHANDLES;
    si.dwFlags |= STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;

    if (!CreateProcessA(exec_path, NULL, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
        return 0;
    }

    CloseHandle(hChildStdinReadDummy);
    CloseHandle(hChildStdoutWriteDummy);

    info=(END_PROCESS_INFORMATION*)malloc(sizeof(END_PROCESS_INFORMATION));
    if (!info)
    {
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        CloseHandle(*hChildStdoutRead);
        CloseHandle(*hChildStdinWrite);
        return 0;
    }
    info->hpiProcess = pi.hProcess;
    info->hpiThread = pi.hThread;
    info->hread = *hChildStdoutRead;
    info->hwrite = *hChildStdinWrite;
    *process_handle= pi.hProcess;
    info->hthread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadEndOfProcess, info, 0, NULL);
    return 1;
}
