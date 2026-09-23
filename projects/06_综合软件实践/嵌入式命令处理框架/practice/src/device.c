#include "device.h"

#include <stddef.h>

void device_init(DeviceState *device)
{
    if (device == NULL) {
        return;
    }
    device->led_on = false;
    device->mode = 0U;
    device->accepted_commands = 0U;
}
