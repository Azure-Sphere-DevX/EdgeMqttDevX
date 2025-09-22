/* Copyright (c) Microsoft Corporation. All rights reserved.
   Licensed under the MIT License. */

#pragma once

#include <stdbool.h>
#include <stddef.h>

// Stub version - Azure IoT support has been removed

#ifndef IOT_HUB_POLL_TIME_SECONDS
#define IOT_HUB_POLL_TIME_SECONDS 0
#endif

#ifndef IOT_HUB_POLL_TIME_NANOSECONDS
#define IOT_HUB_POLL_TIME_NANOSECONDS 100000000
#endif

typedef struct DX_MESSAGE_PROPERTY {
    const char *key;
    const char *value;
} DX_MESSAGE_PROPERTY;

typedef struct DX_MESSAGE_CONTENT_PROPERTIES {
    const char *contentEncoding;
    const char *contentType;
} DX_MESSAGE_CONTENT_PROPERTIES;

/// <summary>
/// Check if there is a network connection and an authenticated connection to Azure IoT Hub/Central
/// Stub implementation - always returns false
/// </summary>
/// <param name=""></param>
/// <returns></returns>
bool dx_isAzureConnected(void);

/// <summary>
/// Send message to Azure IoT Hub/Central with application and content properties.
/// Stub implementation - does nothing and returns false
/// </summary>
/// <param name="msg"></param>
/// <param name="messageProperties"></param>
/// <param name="messagePropertyCount"></param>
/// <param name="messageContentProperties"></param>
/// <returns></returns>
bool dx_azurePublish(const void *message, size_t messageLength, DX_MESSAGE_PROPERTY **messageProperties, size_t messagePropertyCount,
                     DX_MESSAGE_CONTENT_PROPERTIES *messageContentProperties);

/// <summary>
/// Initialise Azure IoT Hub/Connection connection
/// Stub implementation - does nothing
/// </summary>
void dx_azureConnect(void *userConfig, const char *networkInterface, const char *plugAndPlayModelId);

/// <summary>
/// Stop Cloud to device messaging
/// Stub implementation - does nothing
/// </summary>
/// <param name=""></param>
void dx_azureToDeviceStop(void);

/// <summary>
/// Register for new message received from Azure IoT
/// Stub implementation - does nothing
/// </summary>
void dx_azureRegisterMessageReceivedNotification(void *messageReceivedCallback);

/// <summary>
/// Register to be notified of change in Azure IoT Connection status
/// Stub implementation - does nothing and returns false
/// </summary>
/// <param name="connectionStatusCallback"></param>
/// <returns></returns>
bool dx_azureRegisterConnectionChangedNotification(void (*connectionStatusCallback)(bool connected));

/// <summary>
/// Unregister a callback to be notified of change in Azure IoT Connection status
/// Stub implementation - does nothing
/// </summary>
/// <param name="connectionStatusCallback"></param>
void dx_azureUnregisterConnectionChangedNotification(void (*connectionStatusCallback)(bool connected));

/// <summary>
/// Register Device Twin callback to process an Azure IoT device twin message
/// Stub implementation - does nothing and returns NULL
/// </summary>
void *dx_azureRegisterDeviceTwinCallback(void *deviceTwinCallbackHandler);

/// <summary>
/// Register Direct Method callback to process an Azure IoT direct method message
/// Stub implementation - does nothing
/// </summary>
void dx_azureRegisterDirectMethodCallback(void *directMethodCallbackHandler);