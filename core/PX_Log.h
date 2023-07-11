#ifndef __PX_LOG_H
#define __PX_LOG_H

#ifdef PX_DEBUG_MODE
#define PX_ASSERTIF(x) do{if(x) PX_ASSERT();}while (0);
#else
#define PX_ASSERTIF(x)
#endif

px_void PX_LOG(const px_char fmt[]);
px_void PX_ERROR(const px_char fmt[]);
px_char *PX_GETLOG(void);
px_void PX_ASSERT(void);
px_void PX_NOP(px_void);
#endif
