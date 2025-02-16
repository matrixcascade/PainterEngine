#include "platform/modules/px_time.h"
#include "time.h"
#include <unistd.h>

px_timem PX_LocalTime(long long timet)
{
	px_timem timem;
	struct tm *p;
	p=localtime((const time_t *) &timet);
	timem.year=1900+p->tm_year;
	timem.month=p->tm_mon+1;
	timem.day=p->tm_mday;
	timem.hour=p->tm_hour;
	timem.minute=p->tm_min;
	timem.second=p->tm_sec;
	return timem;
}

unsigned int PX_TimeGetTime()
{
	unsigned int uptime = 0;
	struct timespec on;
	if(clock_gettime(1, &on) == 0)
		uptime = on.tv_sec*1000 + on.tv_nsec/1000000;
	return uptime;
}

unsigned int PX_TimeGetTimeUs()
{
	unsigned int uptime = 0;
	struct timespec on;
	if(clock_gettime(1, &on) == 0)
		uptime = on.tv_sec*1000000 + on.tv_nsec/1000;
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

int PX_TimeGetMonth()
{
	time_t timep;
	struct tm *p;
	time (&timep);
	p=gmtime(&timep);

	return p->tm_mon+1;
}

int PX_TimeGetDay()
{
	time_t timep;
	struct tm *p;
	time (&timep);
	p=gmtime(&timep);

	return p->tm_mday;
}

int PX_TimeGetHour()
{
	time_t timep;
	struct tm *p;
	time (&timep);
	p=gmtime(&timep);

	return 8+p->tm_hour;
}

int PX_TimeGetMinute()
{
	time_t timep;
	struct tm *p;
	time (&timep);
	p=gmtime(&timep);

	return p->tm_min;
}

int PX_TimeGetSecond()
{
	time_t timep;
	struct tm *p;
	time (&timep);
	p=gmtime(&timep);

	return p->tm_sec;
}

void PX_Sleep(unsigned int ms)
{
	usleep(ms*1000);
}
