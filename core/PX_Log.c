#include "PX_Typedef.h"


px_void PX_ASSERT(void)
{
#ifdef PX_DEBUG_MODE
	while(1) *(char *)(0)=1;
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