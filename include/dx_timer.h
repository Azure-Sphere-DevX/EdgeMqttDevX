#pragma once

#include "dx_terminate.h"
#include <stdbool.h>
#include <stdatomic.h>
#include <uv.h>

#define DX_TIMER_HANDLER(name)                \
	void name(EventLoopTimer *eventLoopTimer) \
	{                                         \
        if (ConsumeEventLoopTimerEvent(eventLoopTimer) != 0)     \
        {                                                        \
            dx_terminate(DX_ExitCode_ConsumeEventLoopTimeEvent); \
            return;                                              \
        }

#define DX_TIMER_HANDLER_END }

#define DX_DECLARE_TIMER_HANDLER(name) void name(EventLoopTimer *eventLoopTimer)

typedef uv_timer_t EventLoopTimer;
typedef struct timespec timespec;

typedef struct
{
	void (*handler)(uv_timer_t *handle);
	struct timespec *delay;
	struct timespec *repeat;
	const char *name;
	bool initialized;
	uv_timer_t timer_handle;
} DX_TIMER_BINDING;

int ConsumeEventLoopTimerEvent(EventLoopTimer *eventLoopTimer);
void dx_eventLoopRun(void);
void dx_eventLoopStop(void);

bool dx_timerChange(DX_TIMER_BINDING *timer, const struct timespec *period);
bool dx_timerOneShotSet(DX_TIMER_BINDING *timer, const struct timespec *delay);
bool dx_timerStart(DX_TIMER_BINDING *timer);
bool dx_timerStateSet(DX_TIMER_BINDING *timer, bool state);
void dx_timerEventLoopStop(void);
void dx_timerSetStart(DX_TIMER_BINDING *timerSet[], size_t timerCount);
void dx_timerSetStop(DX_TIMER_BINDING *timerSet[], size_t timerCount);
void dx_timerStop(DX_TIMER_BINDING *timer);

// High-precision monotonic millisecond timer functions
// These functions provide drift-free timing using uv_hrtime() monotonic clock
void dx_initMonotonicMillisecondTimer(atomic_uint_fast64_t *tick_count_ptr);
void dx_updateMonotonicMillisecondTick(atomic_uint_fast64_t *tick_count_ptr);
