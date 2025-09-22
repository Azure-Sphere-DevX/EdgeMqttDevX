/* Copyright (c) Microsoft Corporation. All rights reserved.
   Licensed under the MIT License. */

#pragma once
#include <stdbool.h>

// Stub version - Azure IoT support has been removed

/// <summary>
/// Connection types - simplified for MQTT-only operation
/// </summary>
typedef enum {
    DX_CONNECTION_TYPE_NOT_DEFINED = 0,
    DX_CONNECTION_TYPE_MQTT = 1
} ConnectionType;

typedef struct {
    const char* mqtt_host;
    const char* mqtt_port;
    const char* mqtt_client_id;
    const char* mqtt_username;
    const char* mqtt_password;
    const char* network_interface;
    ConnectionType connectionType;
} DX_USER_CONFIG;

/// <summary>
/// Stub implementation - does nothing and returns false
/// </summary>
bool dx_configParseCmdLineArguments(int argc, char* argv[], DX_USER_CONFIG *userConfig);