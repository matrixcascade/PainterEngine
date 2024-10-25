#ifndef PX_THREAD
#define PX_THREAD

typedef void (*func_thread)(void *ptr);

typedef struct
{
	union
	{
		void *handle_ptr;
		unsigned int handle_uint;
		unsigned long handle_ulong;
	};
	func_thread pfunc;
	void *userptr;
	volatile int isRun;
}px_thread;

typedef struct
{
	void* handle;
}px_mutex;

int PX_ThreadCreate(px_thread *thread,func_thread func,void *ptr);
void PX_MutexInitialize(px_mutex* pmutex);
void PX_MutexLock(px_mutex* pmutex);
void PX_MutexUnlock(px_mutex* pmutex);
void PX_MutexFree(px_mutex* pmutex);
#endif
