#include <windows.h>
#include <stdio.h>

int PX_ProcessRun(char exec_path[], HANDLE* process_handle, HANDLE* hChildStdoutRead, HANDLE* hChildStdoutWrite);
int PX_ProcessWrite(HANDLE hInputWrite, char buffer[], int buffersize);
int PX_ProcessRead(HANDLE hOutputRead, char buffer[], int buffersize);
int PX_ProcessIsEnd(HANDLE hProcess);