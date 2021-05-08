#include "PX_Typedef.h"


void PX_ASSERT()
{
#ifdef PX_DEBUG_MODE
*(char *)(0)=1;
#endif
}

void PX_ERROR(px_char fmt[])
{
	PX_ASSERT();
}


px_char * PX_GETLOG()
{
	return "";
}

void PX_LOG(px_char fmt[])
{
	
	//while(1);
}
