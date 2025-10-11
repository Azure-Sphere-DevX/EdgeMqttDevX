/* Copyright (c) Microsoft Corporation. All rights reserved.
   Licensed under the MIT License. */
   
#include "dx_timer.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <stdatomic.h>

// High-precision monotonic timer state for millisecond tick count calculation
static uint64_t timer_start_time_ns = 0;

bool dx_timerChange(DX_TIMER_BINDING *timer, const struct timespec *repeat)
{
	if (!timer->initialized)
	{
		return false;
	}

	uint64_t timer_ms = repeat->tv_sec * 1000;
	timer_ms          = timer_ms + repeat->tv_nsec / 1000000;
	uv_timer_set_repeat(&timer->timer_handle, timer_ms);

	return true;
}

bool dx_timerStart(DX_TIMER_BINDING *timer)
{
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
			uint64_t timer_ms = timer->delay->tv_sec * 1000;
			timer_ms          = timer_ms + timer->delay->tv_nsec / 1000000;
			uv_timer_start(&timer->timer_handle, timer->handler, timer_ms, 0);
		}
		else if (timer->repeat)
		{
			uint64_t timer_ms = timer->repeat->tv_sec * 1000;
			timer_ms          = timer_ms + timer->repeat->tv_nsec / 1000000;
			uv_timer_start(&timer->timer_handle, timer->handler, timer_ms, timer_ms);
		}

		timer->initialized = true;
	}

	return true;
}

void dx_timerSetStart(DX_TIMER_BINDING *timerSet[], size_t timerCount)
{
	for (int i = 0; i < timerCount; i++)
	{
		if (!dx_timerStart(timerSet[i]))
		{
			break;
		};
	}
}

void dx_timerStop(DX_TIMER_BINDING *timer)
{
	if (timer->initialized)
	{
		uv_timer_stop(&timer->timer_handle);
		timer->initialized = false;
	}
}

void dx_timerSetStop(DX_TIMER_BINDING *timerSet[], size_t timerCount)
{
	for (int i = 0; i < timerCount; i++)
	{
		dx_timerStop(timerSet[i]);
	}
}

bool dx_timerStateSet(DX_TIMER_BINDING *timer, bool state)
{
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

	if (timer->initialized)
	{
		int64_t period_ms = period->tv_sec * 1000;
		period_ms         = period_ms + period->tv_nsec / 1000000;

		uv_update_time(uv_default_loop());
		uv_timer_start(&timer->timer_handle, timer->handler, period_ms, 0);

		result = true;
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
/// Call this once at startup before using dx_updateMonotonicMillisecondTick()
/// </summary>
/// <param name="tick_count_ptr">Pointer to the atomic millisecond tick counter to initialize</param>
void dx_initMonotonicMillisecondTimer(atomic_uint_fast64_t *tick_count_ptr)
{
    timer_start_time_ns = uv_hrtime();
    
    // Initialize the tick count to 0
    atomic_store_explicit(tick_count_ptr, 0, memory_order_relaxed);
}

/// <summary>
/// Update millisecond tick count using high-precision monotonic clock
/// This function is optimized for performance and eliminates timer drift
/// 
/// PERFORMANCE BENEFITS:
/// - Uses uv_hrtime() nanosecond precision monotonic timing (no drift)
/// - Single atomic store operation (no fetch_add overhead)
/// - Branch-prediction friendly (single fast path)
/// - Compiler can optimize division by constant (1,000,000)
/// 
/// ACCURACY BENEFITS:  
/// - Immune to timer scheduling delays or system load
/// - Monotonic clock never goes backward (immune to system clock changes)
/// - Nanosecond precision provides microsecond-accurate millisecond values
/// - Eliminates cumulative drift from timer inaccuracies
/// </summary>
/// <param name="tick_count_ptr">Pointer to the atomic millisecond tick counter to update</param>
void dx_updateMonotonicMillisecondTick(atomic_uint_fast64_t *tick_count_ptr)
{
    if (tick_count_ptr == NULL) {
        return; // Invalid pointer
    }
    
    // Get current monotonic time in nanoseconds
    uint64_t current_time_ns = uv_hrtime();
    
    // Calculate elapsed milliseconds since initialization
    // Division by 1,000,000 converts nanoseconds to milliseconds
    // This is optimized by compiler to a fast integer operation
    uint64_t elapsed_ms = (current_time_ns - timer_start_time_ns) / 1000000ULL;
    
    // Atomically update the tick count with the calculated value
    // Uses relaxed ordering for maximum performance in high-frequency updates
    atomic_store_explicit(tick_count_ptr, elapsed_ms, memory_order_relaxed);
}
