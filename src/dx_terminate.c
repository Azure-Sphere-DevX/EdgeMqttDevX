/* Copyright (c) Microsoft Corporation. All rights reserved.
   Licensed under the MIT License. */
   
#include "dx_terminate.h"

#include <dx_timer.h>
#include <signal.h>
#include <stdbool.h>
#include <string.h>

static volatile sig_atomic_t _exitCode = 0;

static void dx_terminationHandler(int signalNumber)
{
    // Don't use Log_Debug here, as it is not guaranteed to be async-signal-safe.
    _exitCode = DX_ExitCode_TermHandler_SigTerm;
    dx_eventLoopStop();
}

void dx_registerTerminationHandler(void)
{
    struct sigaction action;
    memset(&action, 0, sizeof(struct sigaction));
    action.sa_handler = dx_terminationHandler;
    sigaction(SIGTERM, &action, NULL);
}

void dx_terminate(int exitCode)
{
    _exitCode = exitCode;
    dx_eventLoopStop();
}

int dx_getTerminationExitCode(void)
{
    return _exitCode;
}
