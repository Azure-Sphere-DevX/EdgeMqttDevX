/* Copyright (c) Microsoft Corporation. All rights reserved.
   Licensed under the MIT License. */

#pragma once

#include <stdbool.h>
#include <stddef.h>

// Stub version - Azure IoT support has been removed

#define DX_DEVICE_TWIN_HANDLER(name, deviceTwinBinding) \
	void name(DX_DEVICE_TWIN_BINDING *deviceTwinBinding)      \
	{

#define DX_DEVICE_TWIN_HANDLER_END \
	}

#define DX_DECLARE_DEVICE_TWIN_HANDLER(name) \
	void name(DX_DEVICE_TWIN_BINDING *deviceTwinBinding);

typedef enum
{
	DX_TYPE_UNKNOWN = 0,
	DX_DEVICE_TWIN_BOOL = 1,
	DX_DEVICE_TWIN_FLOAT = 2,
	DX_DEVICE_TWIN_DOUBLE = 3,
	DX_DEVICE_TWIN_INT = 4,
	DX_DEVICE_TWIN_STRING = 5,
	DX_DEVICE_TWIN_JSON_OBJECT = 6
} DX_DEVICE_TWIN_TYPE;

typedef struct _deviceTwinBinding
{
	const char *propertyName;
	void *propertyValue;
	int propertyVersion;
	bool propertyUpdated;
	DX_DEVICE_TWIN_TYPE twinType;
	void (*handler)(struct _deviceTwinBinding *deviceTwinBinding);
	void *context;
} DX_DEVICE_TWIN_BINDING;

typedef enum
{
	DX_DEVICE_TWIN_RESPONSE_COMPLETED = 200,
	DX_DEVICE_TWIN_RESPONSE_ERROR = 500,
	DX_DEVICE_TWIN_REPONSE_INVALID = 404
} DX_DEVICE_TWIN_RESPONSE_CODE;

/// <summary>
/// IoT Plug and Play acknowledge receipt of a device twin message with new state and status code.
/// Stub implementation - does nothing and returns false
/// </summary>
/// <param name="deviceTwinBinding"></param>
/// <param name="state"></param>
/// <param name="statusCode"></param>
/// <returns></returns>
bool dx_deviceTwinAckDesiredValue(DX_DEVICE_TWIN_BINDING *deviceTwinBinding, void *state, DX_DEVICE_TWIN_RESPONSE_CODE statusCode);

/// <summary>
/// Update device twin state.
/// Stub implementation - does nothing and returns false
/// </summary>
/// <param name="deviceTwinBinding"></param>
/// <param name="state"></param>
/// <returns></returns>
bool dx_deviceTwinReportValue(DX_DEVICE_TWIN_BINDING *deviceTwinBinding, void *state);

/// <summary>
/// Close all device twins, deallocate backing storage for each twin, and stop inbound and outbound device twin updates.
/// Stub implementation - does nothing
/// </summary>
/// <param name=""></param>
void dx_deviceTwinUnsubscribe(void);

/// <summary>
/// Open device twins and start processing of device twins.
/// Stub implementation - does nothing
/// </summary>
/// <param name="deviceTwins"></param>
/// <param name="deviceTwinCount"></param>
void dx_deviceTwinSubscribe(DX_DEVICE_TWIN_BINDING *deviceTwins[], size_t deviceTwinCount);