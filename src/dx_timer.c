/* Copyright (c) Microsoft Corporation. All rights reserved.
   Licensed under the MIT License. */
   
#include "dx_timer.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <limits.h>

// High-precision monotonic timer state for millisecond tick count calculation
static uint64_t timer_start_time_ns = 0;

// Helper function to safely convert timespec to milliseconds with overflow protection
static bool timespec_to_ms(const struct timespec *ts, uint64_t *result_ms)
{
	if (ts == NULL || result_ms == NULL)
	{
		return false;
	}

	// Check for negative values
	if (ts->tv_sec < 0 || ts->tv_nsec < 0)
	{
		return false;
	}

	// Check for overflow when converting seconds to milliseconds
	if (ts->tv_sec > (UINT64_MAX / 1000))
	{
		return false;
	}

	uint64_t sec_ms = (uint64_t)ts->tv_sec * 1000;
	uint64_t nsec_ms = (uint64_t)ts->tv_nsec / 1000000;

	// Check for overflow when adding nsec_ms
	if (sec_ms > UINT64_MAX - nsec_ms)
	{
		return false;
	}

	*result_ms = sec_ms + nsec_ms;
	return true;
}

bool dx_timerChange(DX_TIMER_BINDING *timer, const struct timespec *repeat)
{
	if (timer == NULL || repeat == NULL || !timer->initialized)
	{
		return false;
	}

	uint64_t timer_ms;
	if (!timespec_to_ms(repeat, &timer_ms))
	{
		return false;
	}

	uv_timer_set_repeat(&timer->timer_handle, timer_ms);

	return true;
}

bool dx_timerStart(DX_TIMER_BINDING *timer)
{
	if (timer == NULL)
	{
		return false;
	}

	if (!timer->initialized)
	{
		if (timer->delay && timer->repeat)
		{
			printf("Can't specify both a timer delay and a repeat period for timer %s\n", timer->name);
			dx_terminate(DX_ExitCode_Create_Timer_Failed);

			return false;
		}

		uv_timer_init(uv_default_loop(), &timer->timer_handle);

		if (timer->delay)
		{
			uint64_t timer_ms;
			if (!timespec_to_ms(timer->delay, &timer_ms))
			{
				return false;
			}
			if (uv_timer_start(&timer->timer_handle, timer->handler, timer_ms, 0) != 0)
			{
				return false;
			}
		}
		else if (timer->repeat)
		{
			uint64_t timer_ms;
			if (!timespec_to_ms(timer->repeat, &timer_ms))
			{
				return false;
			}
			if (uv_timer_start(&timer->timer_handle, timer->handler, timer_ms, timer_ms) != 0)
			{
				return false;
			}
		}

		timer->initialized = true;
	}

	return true;
}

bool dx_timerSetStart(DX_TIMER_BINDING *timerSet[], size_t timerCount)
{
	if (timerSet == NULL)
	{
		return false;
	}

	for (size_t i = 0; i < timerCount; i++)
	{
		if (timerSet[i] == NULL || !dx_timerStart(timerSet[i]))
		{
			// Stop any timers that were successfully started before this failure
			for (size_t j = 0; j < i; j++)
			{
				if (timerSet[j] != NULL)
				{
					dx_timerStop(timerSet[j]);
				}
			}
			return false;
		}
	}
	return true;
}

void dx_timerStop(DX_TIMER_BINDING *timer)
{
	if (timer == NULL)
	{
		return;
	}

	if (timer->initialized)
	{
		uv_timer_stop(&timer->timer_handle);
		timer->initialized = false;
	}
}

void dx_timerSetStop(DX_TIMER_BINDING *timerSet[], size_t timerCount)
{
	if (timerSet == NULL)
	{
		return;
	}

	for (size_t i = 0; i < timerCount; i++)
	{
		if (timerSet[i] != NULL)
		{
			dx_timerStop(timerSet[i]);
		}
	}
}

bool dx_timerStateSet(DX_TIMER_BINDING *timer, bool state)
{
	if (timer == NULL)
	{
		return false;
	}

	if (state)
	{
		return dx_timerStart(timer);
	}
	else
	{
		dx_timerStop(timer);
		return true;
	}
}

void dx_timerEventLoopStop(void)
{
	uv_loop_close(uv_default_loop());
}

bool dx_timerOneShotSet(DX_TIMER_BINDING *timer, const struct timespec *period)
{
	bool result = false;

	if (timer == NULL || period == NULL)
	{
		return false;
	}

	if (timer->initialized)
	{
		uint64_t period_ms;
		if (!timespec_to_ms(period, &period_ms))
		{
			return false;
		}

		uv_update_time(uv_default_loop());
		if (uv_timer_start(&timer->timer_handle, timer->handler, period_ms, 0) == 0)
		{
			result = true;
		}
	}

	return result;
}

int ConsumeEventLoopTimerEvent(EventLoopTimer *eventLoopTimer)
{
	return 0;
}

void dx_eventLoopRun(void)
{
	uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}

void dx_eventLoopStop(void)
{
	uv_stop(uv_default_loop());
}

/// <summary>
/// Initialize monotonic timer system for high-precision millisecond tracking
/// Call this once at startup before using dx_getElapsedMilliseconds()
/// </summary>
void dx_initMonotonicMillisecondTimer(void)
{
    timer_start_time_ns = uv_hrtime();
}

/// <summary>
/// Calculate elapsed milliseconds since initialization
/// </summary>
/// <returns>Elapsed milliseconds since dx_initMonotonicMillisecondTimer() was called</returns>
inline uint64_t dx_getElapsedMilliseconds(void)
{
    return (uv_hrtime() - timer_start_time_ns) / 1000000ULL;
}
