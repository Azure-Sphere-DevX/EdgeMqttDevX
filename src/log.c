/* Copyright (c) Microsoft Corporation. All rights reserved.
   Licensed under the MIT License. */

#include "applibs/log.h"
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

// Mutex for thread-safe logging operations
static pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;

/// <summary>
/// Thread-safe debug logging function with input validation and error handling
/// Outputs formatted debug messages to stdout with proper error checking
/// </summary>
/// <param name="fmt">Format string (printf-style), must not be NULL</param>
/// <param name="...">Variable arguments matching the format string</param>
void Log_Debug(const char *fmt, ...)
{
    // Validate input parameter
    if (fmt == NULL) {
        // Use a safe, direct write to stderr for this error case
        const char *error_msg = "Log_Debug: NULL format string passed\n";
        if (write(STDERR_FILENO, error_msg, strlen(error_msg)) == -1) {
            // If even stderr write fails, we can't do much more
        }
        return;
    }

    // Lock mutex for thread safety
    if (pthread_mutex_lock(&log_mutex) != 0) {
        // If mutex lock fails, proceed without locking (degraded mode)
        // but still try to log the message
    }

    va_list args;
    va_start(args, fmt);
    
    // Use flockfile for additional stdio thread safety
    flockfile(stdout);
    
    int result = vprintf(fmt, args);
    
    // Check for output errors
    if (result < 0) {
        // If vprintf failed, try to report the error to stderr
        // Use a simple error message to avoid potential recursion
        const char *error_msg = "Log_Debug: Output error occurred\n";
        if (write(STDERR_FILENO, error_msg, strlen(error_msg)) == -1) {
            // If stderr write also fails, we've done our best
        }
    } else {
        // Ensure output is flushed for immediate visibility
        fflush(stdout);
    }
    
    funlockfile(stdout);
    va_end(args);

    // Unlock mutex
    pthread_mutex_unlock(&log_mutex);
}