#ifndef PX_TIME_H
#define PX_TIME_H

typedef struct
{
	int year;
	int month;
	int day;
	int hour;
	int minute;
	int second;
}px_timem;

int PX_TimeGetYear();
int PX_TimeGetMonth();
int PX_TimeGetDay();
int PX_TimeGetHour();
int PX_TimeGetMinute();
int PX_TimeGetSecond();
unsigned int PX_TimeGetTime();
unsigned int PX_TimeGetTimeUs();
px_timem PX_LocalTime(long long timet);
void PX_Sleep(unsigned int ms);
#endif