#include "command.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

static CommandResult run_line(CommandEngine *engine, const char *line, char *response, size_t response_size)
{
    CommandResult result = COMMAND_EMPTY;
    size_t i;
    for (i = 0U; line[i] != '\0'; ++i) {
        assert(command_engine_feed(engine, (unsigned char)line[i]));
    }
    assert(command_engine_process(engine, response, response_size, &result));
    return result;
}

int main(void)
{
    CommandEngine engine;
    CommandResult result;
    char response[64];
    char long_line[COMMAND_LINE_CAPACITY + 8U];
    size_t i;

    command_engine_init(&engine);
    assert(run_line(&engine, "led on\n", response, sizeof(response)) == COMMAND_OK);
    assert(engine.device.led_on);
    assert(strcmp(response, "OK LED=ON") == 0);

    assert(run_line(&engine, "mode 3\n", response, sizeof(response)) == COMMAND_OK);
    assert(engine.device.mode == 3U);
    assert(run_line(&engine, "mode 9\n", response, sizeof(response)) == COMMAND_BAD_ARGUMENT);
    assert(run_line(&engine, "unknown\n", response, sizeof(response)) == COMMAND_UNKNOWN);
    assert(run_line(&engine, "status\n", response, sizeof(response)) == COMMAND_OK);
    assert(strstr(response, "LED=ON") != NULL);

    for (i = 0U; i + 2U < sizeof(long_line); ++i) {
        long_line[i] = 'x';
    }
    long_line[i++] = '\n';
    long_line[i] = '\0';
    result = run_line(&engine, long_line, response, sizeof(response));
    assert(result == COMMAND_INPUT_OVERFLOW);
    assert(run_line(&engine, "led off\n", response, sizeof(response)) == COMMAND_OK);
    assert(!engine.device.led_on);

    puts("command framework tests passed");
    return 0;
}
