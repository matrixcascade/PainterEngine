#include "../modules/px_time.h"
#include "px_uefi.h"

int PX_TimeGetYear()
{
  EFI_TIME EfiTime;

  gRT->GetTime (&EfiTime, NULL);

  return EfiTime.Year;
}

int PX_TimeGetMonth()
{
  EFI_TIME EfiTime;

  gRT->GetTime (&EfiTime, NULL);

  return EfiTime.Month;
}

int PX_TimeGetDay()
{
  EFI_TIME EfiTime;

  gRT->GetTime (&EfiTime, NULL);

  return EfiTime.Day;
}

int PX_TimeGetHour()
{
  EFI_TIME EfiTime;

  gRT->GetTime (&EfiTime, NULL);

  return EfiTime.Hour;
}

int PX_TimeGetMinute()
{
  EFI_TIME EfiTime;

  gRT->GetTime (&EfiTime, NULL);

  return EfiTime.Minute;
}

int PX_TimeGetSecond()
{
  EFI_TIME EfiTime;

  gRT->GetTime (&EfiTime, NULL);

  return EfiTime.Second;
}

#ifdef REAL_TIMER_LIB
// NOTICE: Need real TimerLib instance
unsigned int PX_TimeGetTime()
{
  UINT32 TimeStampInMs;

  TimeStampInMs = (UINT32) DivU64x32 (GetTimeInNanoSecond (GetPerformanceCounter()), 1000 * 1000);

  return TimeStampInMs;
}

// NOTICE: Need real TimerLib instance
unsigned int PX_TimeGetTimeUs()
{
  UINT32 TimeStampInUs;

  TimeStampInUs = (UINT32) DivU64x32 (GetTimeInNanoSecond (GetPerformanceCounter()), 1000);

  return TimeStampInUs;
}
#endif

px_timem PX_LocalTime(long long timet)
{
  px_timem t;
  EFI_TIME EfiTime;

  gRT->GetTime (&EfiTime, NULL);

  t.year = EfiTime.Year;
  t.month = EfiTime.Month;
  t.day = EfiTime.Day;
  t.hour = EfiTime.Hour;
  t.minute = EfiTime.Minute;
  t.second = EfiTime.Second;

  return t;
}

// Define EPOCH (1970-JANUARY-01) in the Julian Date representation
#define EPOCH_JULIAN_DATE  2440588

// Seconds per unit
#define SEC_PER_MIN   ((UINTN)    60)
#define SEC_PER_HOUR  ((UINTN)  3600)
#define SEC_PER_DAY   ((UINTN) 86400)

/**
  Calculate Epoch days.

  @param    Time  The UEFI time to be calculated.

  @return   Number of days.

**/
UINTN
EFIAPI
EfiGetEpochDays (
  IN  EFI_TIME  *Time
  )
{
  UINTN  a;
  UINTN  y;
  UINTN  m;
  UINTN  JulianDate; // Absolute Julian Date representation of the supplied Time
  UINTN  EpochDays;  // Number of days elapsed since EPOCH_JULIAN_DAY

  a = (14 - Time->Month) / 12;
  y = Time->Year + 4800 - a;
  m = Time->Month + (12*a) - 3;

  JulianDate = Time->Day + ((153*m + 2)/5) + (365*y) + (y/4) - (y/100) + (y/400) - 32045;

  ASSERT (JulianDate >= EPOCH_JULIAN_DATE);
  EpochDays = JulianDate - EPOCH_JULIAN_DATE;

  return EpochDays;
}

/**
  Converts EFI_TIME to Epoch seconds (elapsed since 1970 JANUARY 01, 00:00:00 UTC).

  @param    Time  The UEFI time to be converted.

  @return   Number of seconds.

**/
UINTN
EFIAPI
EfiTimeToEpoch (
  IN  EFI_TIME  *Time
  )
{
  UINTN  EpochDays;  // Number of days elapsed since EPOCH_JULIAN_DAY
  UINTN  EpochSeconds;

  EpochDays = EfiGetEpochDays (Time);

  EpochSeconds = (EpochDays * SEC_PER_DAY) + ((UINTN)Time->Hour * SEC_PER_HOUR) + (Time->Minute * SEC_PER_MIN) + Time->Second;

  return EpochSeconds;
}

unsigned long long PX_Time()
{
  EFI_TIME EfiTime;

  gRT->GetTime (&EfiTime, NULL);

  return EfiTimeToEpoch(&EfiTime);
}

void PX_Sleep(unsigned int ms)
{
  gBS->Stall (1000 * ms);
}

