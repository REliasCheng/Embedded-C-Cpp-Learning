#include "firmware_console.h"

#include <stdio.h>
#include <string.h>

int main(void)
{
    FirmwareConsole console;
    char input[64];
    char response[64];
    CommandResult result;

    if (!firmware_console_init(&console)) {
        fputs("console initialization failed\n", stderr);
        return 1;
    }

    puts("commands: led on|off, mode 0..3, status");
    while (fgets(input, sizeof(input), stdin) != NULL) {
        size_t i;
        for (i = 0U; i < strlen(input); ++i) {
            if (!firmware_console_feed(&console, (uint8_t)input[i])) {
                fputs("input buffer full\n", stderr);
                break;
            }
        }
        while (firmware_console_process(&console, response, sizeof(response), &result)) {
            if (result == COMMAND_OK) {
                puts(response);
            } else {
                printf("command error: %d\n", (int)result);
            }
        }
        firmware_console_tick(&console);
        firmware_console_tick(&console);
    }
    return 0;
}
