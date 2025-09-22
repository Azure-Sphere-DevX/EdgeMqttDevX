#pragma once

#include "applibs/gpio.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// typedef int GPIO_Value_Type;
// typedef bool GPIO_Value;


typedef enum
{
	DX_DIRECTION_UNKNOWN,
	DX_INPUT,
	DX_OUTPUT
} DX_GPIO_DIRECTION;

typedef enum {
	DX_GPIO_DETECT_LOW,
	DX_GPIO_DETECT_HIGH,
	DX_GPIO_DETECT_BOTH
} DX_GPIO_INPUT_DETECT;

typedef struct
{
	int fd;
	int pin;
	bool initialState;
	bool invertPin;
	DX_GPIO_INPUT_DETECT detect;
	char* name;
	DX_GPIO_DIRECTION direction;
    bool pullup;
	bool opened;
} DX_GPIO_BINDING;

bool dx_gpioOpen(DX_GPIO_BINDING* gpio);
bool dx_gpioStateGet(DX_GPIO_BINDING *peripheral, uint8_t *oldState);
void dx_gpioClose(DX_GPIO_BINDING* gpio);
void dx_gpioOff(DX_GPIO_BINDING* gpio);
void dx_gpioOn(DX_GPIO_BINDING* gpio);
void dx_gpioSetClose(DX_GPIO_BINDING** gpioSet, size_t gpioSetCount);
void dx_gpioSetOpen(DX_GPIO_BINDING** gpioSet, size_t gpioSetCount);
void dx_gpioStateSet(DX_GPIO_BINDING* gpio, bool state);