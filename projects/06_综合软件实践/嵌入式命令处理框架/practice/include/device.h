#ifndef DEVICE_H
#define DEVICE_H

#include <stdbool.h>

typedef struct {
    bool led_on;
    unsigned mode;
    unsigned accepted_commands;
} DeviceState;

void device_init(DeviceState *device);

#endif
