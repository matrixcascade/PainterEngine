#include "PX_Typedef.h"

void PX_ASSERT()
{
#ifdef PX_DEBUG_MODE
	__debugbreak();
#endif
}

void PX_ERROR(px_char fmt[])
{
	//PX_ASSERT();
	//while(1);
}


px_char * PX_GETLOG()
{
	return "";
}

void PX_LOG(px_char fmt[])
{
	
	//while(1);
}
