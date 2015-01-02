#ifndef TIMER_H
#define TIMER_H

#include <time.h>
#include <sys/timeb.h>

#ifdef __MACH__
#include <mach/clock.h>
#include <mach/mach.h>
#endif

inline uint64_t ClockGetTimemS()
{
  struct timespec ts;
  #ifdef __MACH__ // OS X does not have clock_gettime, use clock_get_time
  clock_serv_t cclock;
  mach_timespec_t mts;
  host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
  clock_get_time(cclock, &mts);
  mach_port_deallocate(mach_task_self(), cclock);
  ts.tv_sec = mts.tv_sec;
  ts.tv_nsec = mts.tv_nsec;
  #else
  clock_gettime(CLOCK_REALTIME, &ts);
  #endif
  return (uint64_t)ts.tv_sec * 1000000000LL + (uint64_t)ts.tv_nsec;
}

struct Timer
{
	static void Init()
	{
	}
	void start()
	{
		startTime = ClockGetTimemS();
	}
	void stop()
	{
		stopTime = ClockGetTimemS();
	}
	double seconds() const
	{
		return milli_seconds() / 1000.;
	}
	double milli_seconds() const
	{
		return micro_seconds() / 1000.;
	}
	double micro_seconds() const
	{
		return nano_seconds() / 1000.;
	}
	double nano_seconds() const
	{
		return stopTime - startTime;
	}
	uint64_t startTime, stopTime;
};
#endif

