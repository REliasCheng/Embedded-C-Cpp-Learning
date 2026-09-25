#include "firmware_console.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

static CommandResult run_command(FirmwareConsole *console, const char *line,
                                 char *response, size_t response_size)
{
    CommandResult result = COMMAND_EMPTY;
    size_t i;
    for (i = 0U; line[i] != '\0'; ++i) {
        assert(firmware_console_feed(console, (uint8_t)line[i]));
    }
    assert(firmware_console_process(console, response, response_size, &result));
    return result;
}

int main(void)
{
    FirmwareConsole console;
    char response[64];

    assert(firmware_console_init(&console));
    assert(run_command(&console, "mode 1\n", response, sizeof(response)) == COMMAND_OK);
    assert(strcmp(response, "OK MODE=1") == 0);
    assert(!console.command.device.led_on);

    firmware_console_tick(&console);
    assert(console.heartbeat_count == 0U);
    firmware_console_tick(&console);
    assert(console.heartbeat_count == 1U);
    assert(console.command.device.led_on);

    assert(run_command(&console, "led off\n", response, sizeof(response)) == COMMAND_OK);
    assert(!console.command.device.led_on);
    assert(run_command(&console, "status\n", response, sizeof(response)) == COMMAND_OK);
    assert(strcmp(response, "LED=OFF MODE=1 COUNT=3") == 0);

    assert(run_command(&console, "mode 0\n", response, sizeof(response)) == COMMAND_OK);
    firmware_console_tick(&console);
    firmware_console_tick(&console);
    assert(console.heartbeat_count == 2U);
    assert(!console.command.device.led_on);

    puts("firmware console integration tests passed");
    return 0;
}
