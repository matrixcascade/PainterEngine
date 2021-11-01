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

int PX_ThreadCreate(px_thread *thread,func_thread func,void *ptr);

#endif
