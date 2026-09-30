#include "command.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef CommandResult (*CommandHandler)(DeviceState *device, int argc, char **argv, char *response, size_t response_size);

typedef struct {
    const char *name;
    CommandHandler handler;
} CommandEntry;

static CommandResult write_response(char *response, size_t response_size, const char *text)
{
    int written;
    if (response == NULL || response_size == 0U) {
        return COMMAND_RESPONSE_TOO_SMALL;
    }
    written = snprintf(response, response_size, "%s", text);
    return written < 0 || (size_t)written >= response_size ? COMMAND_RESPONSE_TOO_SMALL : COMMAND_OK;
}

static CommandResult handle_led(DeviceState *device, int argc, char **argv, char *response, size_t response_size)
{
    if (argc != 2) {
        return COMMAND_BAD_ARGUMENT;
    }
    if (strcmp(argv[1], "on") == 0) {
        device->led_on = true;
    } else if (strcmp(argv[1], "off") == 0) {
        device->led_on = false;
    } else {
        return COMMAND_BAD_ARGUMENT;
    }
    ++device->accepted_commands;
    return write_response(response, response_size, device->led_on ? "OK LED=ON" : "OK LED=OFF");
}

static CommandResult handle_mode(DeviceState *device, int argc, char **argv, char *response, size_t response_size)
{
    char *end = NULL;
    unsigned long mode;
    if (argc != 2) {
        return COMMAND_BAD_ARGUMENT;
    }
    mode = strtoul(argv[1], &end, 10);
    if (end == argv[1] || *end != '\0' || mode > 3UL) {
        return COMMAND_BAD_ARGUMENT;
    }
    device->mode = (unsigned)mode;
    ++device->accepted_commands;
    if (snprintf(response, response_size, "OK MODE=%u", device->mode) >= (int)response_size) {
        return COMMAND_RESPONSE_TOO_SMALL;
    }
    return COMMAND_OK;
}

static CommandResult handle_status(DeviceState *device, int argc, char **argv, char *response, size_t response_size)
{
    int written;
    (void)argv;
    if (argc != 1) {
        return COMMAND_BAD_ARGUMENT;
    }
    ++device->accepted_commands;
    written = snprintf(response, response_size, "LED=%s MODE=%u COUNT=%u",
                       device->led_on ? "ON" : "OFF", device->mode, device->accepted_commands);
    return written < 0 || (size_t)written >= response_size ? COMMAND_RESPONSE_TOO_SMALL : COMMAND_OK;
}

static const CommandEntry COMMANDS[] = {
    {"led", handle_led},
    {"mode", handle_mode},
    {"status", handle_status},
};

static CommandResult execute_line(CommandEngine *engine, char *response, size_t response_size)
{
    char *argv[4];
    int argc = 0;
    char *token = strtok(engine->line, " ");
    size_t i;
    while (token != NULL && argc < 4) {
        argv[argc++] = token;
        token = strtok(NULL, " ");
    }
    if (argc == 0) {
        return COMMAND_EMPTY;
    }
    if (token != NULL) {
        return COMMAND_BAD_ARGUMENT;
    }
    for (i = 0U; i < sizeof(COMMANDS) / sizeof(COMMANDS[0]); ++i) {
        if (strcmp(argv[0], COMMANDS[i].name) == 0) {
            return COMMANDS[i].handler(&engine->device, argc, argv, response, response_size);
        }
    }
    return COMMAND_UNKNOWN;
}

void command_engine_init(CommandEngine *engine)
{
    if (engine == NULL) {
        return;
    }
    ring_buffer_init(&engine->rx);
    device_init(&engine->device);
    engine->line_length = 0U;
    engine->dropping_line = false;
}

bool command_engine_feed(CommandEngine *engine, uint8_t byte)
{
    return engine != NULL && ring_buffer_push(&engine->rx, byte);
}

bool command_engine_process(CommandEngine *engine, char *response, size_t response_size, CommandResult *result)
{
    uint8_t byte;
    if (engine == NULL || result == NULL) {
        return false;
    }
    if (ring_buffer_take_overflow(&engine->rx)) {
        engine->line_length = 0U;
        engine->dropping_line = true;
    }
    while (ring_buffer_pop(&engine->rx, &byte)) {
        if (byte == '\r') {
            continue;
        }
        if (byte == '\n') {
            if (engine->dropping_line) {
                engine->dropping_line = false;
                engine->line_length = 0U;
                *result = COMMAND_INPUT_OVERFLOW;
                (void)write_response(response, response_size, "ERR INPUT_OVERFLOW");
                return true;
            }
            engine->line[engine->line_length] = '\0';
            *result = execute_line(engine, response, response_size);
            engine->line_length = 0U;
            return true;
        }
        if (engine->dropping_line) {
            continue;
        }
        if (engine->line_length + 1U >= COMMAND_LINE_CAPACITY) {
            engine->dropping_line = true;
            engine->line_length = 0U;
        } else {
            engine->line[engine->line_length++] = (char)byte;
        }
    }
    return false;
}
