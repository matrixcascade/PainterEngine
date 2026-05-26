#ifndef __PX_LOG_H
#define __PX_LOG_H

#ifdef PX_DEBUG_MODE
#include <stdio.h>
#define PX_ASSERTIF(x) do{if(!(x))break; PX_ASSERT();}while (1);
#define PX_ASSERTIFX(x,log) do{if(!(x))break; PX_ASSERT();}while (1);
#define PX_ASSERTX(log) PX_ASSERT()
#define PX_printf printf
#else
#define PX_ASSERTIF(x)
#define PX_ASSERTIFX(x,log)
#define PX_ASSERTX(x)
#define PX_printf
#endif



px_void PX_LOG(const px_char fmt[]);
px_void PX_ERROR(const px_char fmt[]);
px_char *PX_GETLOG(void);
px_void PX_ASSERT(void);
px_void PX_DEBUG_BREAK(void);
px_void PX_TERMINATE(const px_char _msg[]);
px_void PX_CRASH(const px_char _msg[]);
px_void PX_NOP(px_void);
#endif
