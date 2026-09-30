#include "firmware_console.h"

static void heartbeat_callback(void *context)
{
    FirmwareConsole *console = context;
    ++console->heartbeat_count;
    if (console->command.device.mode != 0U) {
        console->command.device.led_on = !console->command.device.led_on;
    }
}

bool firmware_console_init(FirmwareConsole *console)
{
    if (console == NULL) {
        return false;
    }
    command_engine_init(&console->command);
    task_manager_init(&console->tasks);
    console->heartbeat_count = 0U;
    if (!task_manager_add(&console->tasks, "heartbeat", 2U, heartbeat_callback,
                          console, &console->heartbeat_task_id)) {
        return false;
    }
    return task_manager_start(&console->tasks, console->heartbeat_task_id);
}

bool firmware_console_feed(FirmwareConsole *console, uint8_t byte)
{
    return console != NULL && command_engine_feed(&console->command, byte);
}

bool firmware_console_process(FirmwareConsole *console, char *response,
                              size_t response_size, CommandResult *result)
{
    return console != NULL && command_engine_process(&console->command, response,
                                                      response_size, result);
}

void firmware_console_tick(FirmwareConsole *console)
{
    if (console != NULL) {
        task_manager_tick(&console->tasks);
    }
}
