#include "platform/modules/px_thread.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void *PX_ThreadRun(void *ptr)
{
	px_thread *pthread=(px_thread *)ptr;
	pthread->isRun=1;
	pthread->pfunc(pthread->userptr);
	pthread->isRun=0;
	return 0;
}


int PX_ThreadCreate(px_thread *pthread,func_thread func,void *ptr)
{
    pthread_t t;
	pthread->pfunc=func;
	pthread->userptr=ptr;
	pthread->isRun=0;
    return pthread_create( &t, NULL, PX_ThreadRun, pthread)==0; 
}

void PX_MutexInitialize(px_mutex *pmutex)
{
	pmutex->handle=malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(pmutex->handle,NULL);
}

void PX_MutexLock(px_mutex *pmutex)
{
	pthread_mutex_lock(pmutex->handle);
}

void PX_MutexUnlock(px_mutex *pmutex)
{
	pthread_mutex_unlock(pmutex->handle);
}

void PX_MutexFree(px_mutex *pmutex)
{
	pthread_mutex_destroy(pmutex->handle);
	free(pmutex->handle);
}