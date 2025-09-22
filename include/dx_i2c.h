#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef int I2C_InterfaceId;

typedef struct {
    int fd;
    I2C_InterfaceId interfaceId;
    int i2c_address;
    uint32_t speedInHz;
    char *name;
    bool opened;
} DX_I2C_BINDING;

bool dx_i2cClose(DX_I2C_BINDING *i2c_binding);
bool dx_i2cOpen(DX_I2C_BINDING *i2c_binding);
void dx_i2cSetClose(DX_I2C_BINDING **i2cSet, size_t i2cSetCount);
void dx_i2cSetOpen(DX_I2C_BINDING **i2cSet, size_t i2cSetCount);
bool dx_i2cWriteData(DX_I2C_BINDING *i2c_binding, unsigned i2c_reg, char *buf, unsigned count);