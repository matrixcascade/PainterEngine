#include "../modules/px_thread.h"
#include "windows.h"

DWORD WINAPI  PX_ThreadRun(void *ptr)
{
	px_thread *pthread=(px_thread *)ptr;
	pthread->isRun=1;
	pthread->pfunc(pthread->userptr);
	pthread->isRun=0;
	return 0;
}


int PX_ThreadCreate(px_thread *pthread,func_thread func,void *ptr)
{
	pthread->pfunc=func;
	pthread->userptr=ptr;
	pthread->isRun=0;
	if(CreateThread(NULL,0,PX_ThreadRun,pthread,0,&pthread->handle_ulong)!=INVALID_HANDLE_VALUE)
		return 1;
	else
		return 0;
}



void PX_MutexInitialize(px_mutex *pmutex)
{
	pmutex->handle=malloc(sizeof(CRITICAL_SECTION));
	if (pmutex->handle==0)
	{
		return;
	}
	InitializeCriticalSection((CRITICAL_SECTION *)pmutex->handle);
}

void PX_MutexLock(px_mutex *pmutex)
{
	EnterCriticalSection((CRITICAL_SECTION *)pmutex->handle);
}

void PX_MutexUnlock(px_mutex *pmutex)
{
	LeaveCriticalSection((CRITICAL_SECTION *)pmutex->handle);
}

void PX_MutexFree(px_mutex *pmutex)
{
	DeleteCriticalSection((CRITICAL_SECTION *)pmutex->handle);
	free(pmutex->handle);
}


