#ifndef __PX_LOG_H
#define __PX_LOG_H

#ifdef PX_DEBUG_MODE
#define PX_ASSERTIF(x) do{if(!(x))break; PX_ASSERT();}while (1);
#define PX_ASSERTIFX(x,log) do{if(!(x))break; PX_ASSERT();}while (1);
#else
#define PX_ASSERTIF(x)
#define PX_ASSERTIFX(x,log)
#endif

#include "stdio.h"
#define PX_printf printf

px_void PX_LOG(const px_char fmt[]);
px_void PX_ERROR(const px_char fmt[]);
px_char *PX_GETLOG(void);
px_void PX_ASSERT(void);
px_void PX_NOP(px_void);
#endif
