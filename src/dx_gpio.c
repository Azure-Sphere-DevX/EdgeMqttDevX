/* Copyright (c) Microsoft Corporation. All rights reserved.
   Licensed under the MIT License. */

#include "dx_gpio.h"

#include <gpiod.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>

#define MAX_CHIP_NUMBER 6

// https://libgpiod-dlang.dpldocs.info/gpiod.html

typedef struct
{
    int count;
    struct gpiod_chip *chip;
} GPIO_CHIP_T;

GPIO_CHIP_T gpio_chips[MAX_CHIP_NUMBER];

static void close_chip(int chip_number)
{
    if (gpio_chips[chip_number].count == 0)
    {
        gpiod_chip_close(gpio_chips[chip_number].chip);
        gpio_chips[chip_number].chip = NULL;
    }
}

bool dx_gpioOpen(DX_GPIO_BINDING *gpio_binding)
{
    if (gpio_binding->__line_handle)
    {
        return true;
    }

    // clang-format off
    if (DX_GPIO_DIRECTION_UNKNOWN == gpio_binding->direction ||
        gpio_binding->chip_number < 0 ||
        gpio_binding->chip_number >= MAX_CHIP_NUMBER ||
        gpio_binding->line_number < 0)
    {
        return false;
    }
    // clang-format on

    if (!gpio_chips[gpio_binding->chip_number].chip)
    {
        gpio_chips[gpio_binding->chip_number].chip = gpiod_chip_open_by_number(gpio_binding->chip_number);
        if (!gpio_chips[gpio_binding->chip_number].chip)
        {
            return false;
        }
    }

    gpio_binding->__line_handle = gpiod_chip_get_line(gpio_chips[gpio_binding->chip_number].chip, gpio_binding->line_number);
    if (!gpio_binding->__line_handle)
    {
        close_chip(gpio_binding->chip_number);
        return false;
    }

    if (DX_GPIO_INPUT == gpio_binding->direction)
    {
        if (gpiod_line_request_input(gpio_binding->__line_handle, gpio_binding->name) == -1)
        {
            close_chip(gpio_binding->chip_number);
            return false;
        }
    }
    else
    {
        if (gpiod_line_request_output(gpio_binding->__line_handle, gpio_binding->name, (int)gpio_binding->initial_state) == -1)
        {
            close_chip(gpio_binding->chip_number);
            return false;
        }
    }

    gpio_chips[gpio_binding->chip_number].count++;

    return true;
}

bool dx_gpioClose(DX_GPIO_BINDING *gpio_binding)
{
    if (gpio_binding->__line_handle)
    {
        gpiod_line_release(gpio_binding->__line_handle);

        gpio_binding->__line_handle = NULL;

        gpio_chips[gpio_binding->chip_number].count--;
        close_chip(gpio_binding->chip_number);
    }

    return true;
}

bool dx_gpioOn(DX_GPIO_BINDING *gpio_binding)
{
    return dx_gpioStateSet(gpio_binding, true);
}

bool dx_gpioOff(DX_GPIO_BINDING *gpio_binding)
{
    return dx_gpioStateSet(gpio_binding, false);
}

bool dx_gpioStateSet(DX_GPIO_BINDING *gpio_binding, bool state)
{
    if (!gpio_binding->__line_handle)
    {
        return false;
    }

    return 0 == gpiod_line_set_value(gpio_binding->__line_handle, (int)state);
}

int dx_gpioStateGet(DX_GPIO_BINDING *gpio_binding)
{
    if (!gpio_binding->__line_handle)
    {
        return false;
    }

    return gpiod_line_get_value(gpio_binding->__line_handle);
}

void dx_gpioSetOpen(DX_GPIO_BINDING **gpio_bindings, size_t gpio_bindings_count)
{
    for (int i = 0; i < gpio_bindings_count; i++)
    {
        if (!dx_gpioOpen(gpio_bindings[i]))
        {
            break;
        }
    }
}

void dx_gpioSetClose(DX_GPIO_BINDING **gpio_bindings, size_t gpio_bindings_count)
{
    for (int i = 0; i < gpio_bindings_count; i++)
    {
        dx_gpioClose(gpio_bindings[i]);
    }
}