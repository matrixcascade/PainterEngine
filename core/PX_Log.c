#include "PX_Typedef.h"

#if defined(__unix__) || defined(__unix) || defined(__APPLE__) || defined(__linux__) || defined(_POSIX_VERSION)
#include <signal.h>
#define PX_HAS_SIGNAL_H 1
#else
#define PX_HAS_SIGNAL_H 0
#endif

px_void PX_DEBUG_BREAK(void)
{
   //make breakpoint
#if defined(_MSC_VER)
	__debugbreak();
#elif defined(__clang__)
	#if __has_builtin(__builtin_debugtrap)
		__builtin_debugtrap();
	#elif __has_builtin(__builtin_trap)
		__builtin_trap();
   #elif PX_HAS_SIGNAL_H
		raise(SIGTRAP);
	#else
     *((volatile px_int *)0) = 0;
	#endif
#elif defined(__GNUC__)
	__builtin_trap();
#elif PX_HAS_SIGNAL_H
	raise(SIGTRAP);
#else
 *((volatile px_int *)0) = 0;
#endif
}

px_void PX_ASSERT(void)
{
#ifdef PX_DEBUG_MODE
	PX_DEBUG_BREAK();
#endif
}

px_void PX_ERROR(const px_char fmt[])
{
	PX_ASSERT();
}


px_char * PX_GETLOG(void)
{
	return (px_char *)"";
}

px_void PX_LOG(const px_char fmt[])
{
	int errcode;
	errcode = 0;
}

px_void PX_NOP()
{

}

px_void PX_TERMINATE(const px_char _msg[])
{
	PX_ASSERTX(_msg);
	while (1);
}

px_void PX_CRASH(const px_char _msg[])
{
	PX_ASSERTX(_msg);
	while (1);
}