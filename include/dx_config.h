/* Copyright (c) Microsoft Corporation. All rights reserved.
   Licensed under the MIT License. */

#pragma once
#include <stdbool.h>

// Stub version - Azure IoT support has been removed

/// <summary>
/// Connection types to use when connecting to the Azure IoT Hub.
/// </summary>
typedef enum {
    DX_CONNECTION_TYPE_NOT_DEFINED = 0,
    DX_CONNECTION_TYPE_DPS = 1,
    DX_CONNECTION_TYPE_HOSTNAME = 2,
    DX_CONNECTION_TYPE_STRING = 3
} ConnectionType;

typedef struct {
    const char* idScope;
    const char* device_id;
    const char* device_key;
    const char* hostname;
    const char* connection_string;
    const char* network_interface;
    ConnectionType connectionType;
} DX_USER_CONFIG;

/// <summary>
/// Stub implementation - does nothing and returns false
/// </summary>
bool dx_configParseCmdLineArguments(int argc, char* argv[], DX_USER_CONFIG *userConfig);