#include "../modules/px_thread.h"
#include <stdio.h>
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
    return pthread_create( &t, NULL, PX_ThreadRun, pthread)==0; 
}
