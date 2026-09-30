#ifndef FIRMWARE_CONSOLE_H
#define FIRMWARE_CONSOLE_H

#include "command.h"
#include "task_manager.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct {
    CommandEngine command;
    TaskManager tasks;
    size_t heartbeat_task_id;
    unsigned heartbeat_count;
} FirmwareConsole;

bool firmware_console_init(FirmwareConsole *console);
bool firmware_console_feed(FirmwareConsole *console, uint8_t byte);
bool firmware_console_process(FirmwareConsole *console, char *response,
                              size_t response_size, CommandResult *result);
void firmware_console_tick(FirmwareConsole *console);

#endif
