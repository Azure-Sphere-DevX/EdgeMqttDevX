/* Copyright (c) Microsoft Corporation. All rights reserved.
   Licensed under the MIT License. */
   
#include "dx_terminate.h"

#include <dx_timer.h>
#include <signal.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

static volatile sig_atomic_t _exitCode = 0;

/// <summary>
/// Signal handler for graceful termination - called when SIGTERM, SIGINT, or SIGHUP is received
/// Note: Only async-signal-safe functions should be called from signal handlers
/// </summary>
/// <param name="signalNumber">The signal number that triggered the handler</param>
static void dx_terminationHandler(int signalNumber)
{
    // Don't use Log_Debug here, as it is not guaranteed to be async-signal-safe.
    // Set exit code based on signal received (unused parameter warning suppression)
    (void)signalNumber;
    _exitCode = DX_ExitCode_TermHandler_SigTerm;
    dx_eventLoopStop();
}

/// <summary>
/// Registers signal handlers for graceful termination on SIGTERM, SIGINT, and SIGHUP
/// </summary>
void dx_registerTerminationHandler(void)
{
    struct sigaction action;
    memset(&action, 0, sizeof(struct sigaction));
    action.sa_handler = dx_terminationHandler;
    action.sa_flags = SA_RESTART; // Restart interrupted system calls
    
    // Block other signals during handler execution
    sigemptyset(&action.sa_mask);
    sigaddset(&action.sa_mask, SIGTERM);
    sigaddset(&action.sa_mask, SIGINT);
    sigaddset(&action.sa_mask, SIGHUP);
    
    // Register handlers for common termination signals
    if (sigaction(SIGTERM, &action, NULL) == -1) {
        fprintf(stderr, "Warning: Failed to register SIGTERM handler: %s\n", strerror(errno));
    }
    
    if (sigaction(SIGINT, &action, NULL) == -1) {
        fprintf(stderr, "Warning: Failed to register SIGINT handler: %s\n", strerror(errno));
    }
    
    if (sigaction(SIGHUP, &action, NULL) == -1) {
        fprintf(stderr, "Warning: Failed to register SIGHUP handler: %s\n", strerror(errno));
    }
}

/// <summary>
/// Initiates graceful application termination with the specified exit code
/// </summary>
/// <param name="exitCode">Exit code to return to the system (0-255)</param>
void dx_terminate(int exitCode)
{
    // Validate exit code range (standard Unix exit codes are 0-255)
    if (exitCode < 0 || exitCode > 255) {
        fprintf(stderr, "Warning: Exit code %d is outside valid range (0-255), clamping to 255\n", exitCode);
        exitCode = 255;
    }
    
    _exitCode = exitCode;
    dx_eventLoopStop();
}

/// <summary>
/// Returns the current termination exit code
/// </summary>
/// <returns>The exit code that will be returned when the application terminates</returns>
int dx_getTerminationExitCode(void)
{
    return _exitCode;
}
