#pragma once

#include "dx_exit_codes.h"

int dx_getTerminationExitCode(void);
void dx_registerTerminationHandler(void);
void dx_terminate(int exitCode);
