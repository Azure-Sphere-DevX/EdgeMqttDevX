/* Copyright (c) Microsoft Corporation. All rights reserved.
   Licensed under the MIT License. */

#include "dx_azure_iot.h"
#include "dx_device_twins.h"
#include "dx_config.h"

// Stub implementations for Azure IoT functions
// These provide API compatibility but no Azure IoT functionality

bool dx_isAzureConnected(void) {
    return false;  // Always return disconnected
}

bool dx_azurePublish(const void *message, size_t messageLength, DX_MESSAGE_PROPERTY **messageProperties, 
                     size_t messagePropertyCount, DX_MESSAGE_CONTENT_PROPERTIES *messageContentProperties) {
    // Do nothing, return false to indicate failure
    return false;
}

void dx_azureConnect(void *userConfig, const char *networkInterface, const char *plugAndPlayModelId) {
    // Do nothing
}

void dx_azureToDeviceStop(void) {
    // Do nothing
}

void dx_azureRegisterMessageReceivedNotification(void *messageReceivedCallback) {
    // Do nothing
}

bool dx_azureRegisterConnectionChangedNotification(void (*connectionStatusCallback)(bool connected)) {
    // Do nothing, return false
    return false;
}

void dx_azureUnregisterConnectionChangedNotification(void (*connectionStatusCallback)(bool connected)) {
    // Do nothing
}

void *dx_azureRegisterDeviceTwinCallback(void *deviceTwinCallbackHandler) {
    // Do nothing, return NULL
    return NULL;
}

void dx_azureRegisterDirectMethodCallback(void *directMethodCallbackHandler) {
    // Do nothing
}

// Device twin stub implementations

bool dx_deviceTwinAckDesiredValue(DX_DEVICE_TWIN_BINDING *deviceTwinBinding, void *state, DX_DEVICE_TWIN_RESPONSE_CODE statusCode) {
    // Do nothing, return false
    return false;
}

bool dx_deviceTwinReportValue(DX_DEVICE_TWIN_BINDING *deviceTwinBinding, void *state) {
    // Do nothing, return false
    return false;
}

void dx_deviceTwinUnsubscribe(void) {
    // Do nothing
}

void dx_deviceTwinSubscribe(DX_DEVICE_TWIN_BINDING *deviceTwins[], size_t deviceTwinCount) {
    // Do nothing
}

// Config stub implementations

bool dx_configParseCmdLineArguments(int argc, char* argv[], DX_USER_CONFIG *userConfig) {
    // Do nothing, return false
    return false;
}