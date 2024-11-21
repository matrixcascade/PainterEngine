#include "../modules/px_time.h"
#include "time.h"
#include "windows.h"
#pragma comment (lib,"winmm.lib")


int PX_TimeGetYear()
{

	time_t timep;
	struct tm p;
	time (&timep);
	gmtime_s(&p,&timep);

	return 1900+p.tm_year;
}

int PX_TimeGetMonth()
{
	time_t timep;
	struct tm p;
	time(&timep);
	gmtime_s(&p, &timep);

	return  p.tm_mon+1;
}

int PX_TimeGetDay()
{
	time_t timep;
	struct tm p;
	time(&timep);
	gmtime_s(&p, &timep);

	return  p.tm_mday;
}

int PX_TimeGetHour()
{
	time_t timep;
	struct tm p;
	time(&timep);
	gmtime_s(&p, &timep);

	return  p.tm_hour;
}

int PX_TimeGetMinute()
{
	time_t timep;
	struct tm p;
	time(&timep);
	gmtime_s(&p, &timep);

	return  p.tm_min;
}

int PX_TimeGetSecond()
{
	time_t timep;
	struct tm p;
	time(&timep);
	gmtime_s(&p, &timep);

	return  p.tm_sec;
}


unsigned int PX_TimeGetTime()
{
	return timeGetTime();
}

void PX_Sleep(unsigned int ms)
{
	Sleep(ms);
}

unsigned int PX_TimeGetTimeUs()
{
	unsigned int time_us;
	LARGE_INTEGER nFreq,nCounter;
	if (QueryPerformanceFrequency(&nFreq))
	{
		if (QueryPerformanceCounter(&nCounter))
		{
			time_us=(unsigned int)(nCounter.QuadPart*1000000/nFreq.QuadPart);
			return time_us;
		}
	}
	return 0;
}