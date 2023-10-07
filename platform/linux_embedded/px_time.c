#include "../modules/px_time.h"
#include "time.h"
#include <unistd.h>

unsigned int PX_TimeGetTime()
{
	unsigned int uptime = 0;
	struct timespec on;
	if(clock_gettime(CLOCK_MONOTONIC, &on) == 0)
		uptime = on.tv_sec*1000 + on.tv_nsec/1000000;
	return uptime;
}

int PX_TimeGetYear()
{
	time_t timep;
	struct tm *p;
	time (&timep);
	p=gmtime(&timep);

	return 1900+p->tm_year;
}

int PX_TimeGetMouth()
{
	time_t timep;
	struct tm *p;
	time (&timep);
	p=gmtime(&timep);

	return 1900+p->tm_year;
}

int PX_TimeGetDay()
{
	time_t timep;
	struct tm *p;
	time (&timep);
	p=gmtime(&timep);

	return 1900+p->tm_mon;
}

int PX_TimeGetHour()
{
	time_t timep;
	struct tm *p;
	time (&timep);
	p=gmtime(&timep);

	return 1900+p->tm_hour;
}

int PX_TimeGetMinute()
{
	time_t timep;
	struct tm *p;
	time (&timep);
	p=gmtime(&timep);

	return 1900+p->tm_min;
}

int PX_TimeGetSecond()
{
	time_t timep;
	struct tm *p;
	time (&timep);
	p=gmtime(&timep);

	return 1900+p->tm_sec;
}

void PX_Sleep(unsigned int ms)
{
	usleep(ms*1000);
}
