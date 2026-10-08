#include "command.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define COMMAND_MAX_ARGUMENTS 4U
#define COMMAND_RESPONSE_CAPACITY 48U

typedef CommandResult (*CommandHandler)(DeviceState *device, int argc, char **argv, char *response, size_t response_size);

typedef struct {
    const char *name;
    CommandHandler handler;
} CommandEntry;

static CommandResult write_response(char *response, size_t response_size, const char *text)
{
    size_t text_length;

    if (response == NULL || text == NULL) {
        return COMMAND_RESPONSE_TOO_SMALL;
    }
    text_length = strlen(text);
    if (response_size <= text_length) {
        return COMMAND_RESPONSE_TOO_SMALL;
    }
    memcpy(response, text, text_length + 1U);
    return COMMAND_OK;
}

static CommandResult handle_led(DeviceState *device, int argc, char **argv, char *response, size_t response_size)
{
    bool new_led_state;
    CommandResult response_result;

    if (argc != 2) {
        return COMMAND_BAD_ARGUMENT;
    }
    if (strcmp(argv[1], "on") == 0) {
        new_led_state = true;
    } else if (strcmp(argv[1], "off") == 0) {
        new_led_state = false;
    } else {
        return COMMAND_BAD_ARGUMENT;
    }

    response_result = write_response(response, response_size,
                                     new_led_state ? "OK LED=ON" : "OK LED=OFF");
    if (response_result != COMMAND_OK) {
        return response_result;
    }

    device->led_on = new_led_state;
    ++device->accepted_commands;
    return COMMAND_OK;
}

static CommandResult handle_mode(DeviceState *device, int argc, char **argv, char *response, size_t response_size)
{
    char *end = NULL;
    unsigned long mode;
    char prepared_response[COMMAND_RESPONSE_CAPACITY];
    int written;
    CommandResult response_result;

    if (argc != 2) {
        return COMMAND_BAD_ARGUMENT;
    }
    mode = strtoul(argv[1], &end, 10);
    if (end == argv[1] || *end != '\0' || mode > 3UL) {
        return COMMAND_BAD_ARGUMENT;
    }

    written = snprintf(prepared_response, sizeof(prepared_response), "OK MODE=%lu", mode);
    if (written < 0 || (size_t)written >= sizeof(prepared_response)) {
        return COMMAND_RESPONSE_TOO_SMALL;
    }
    response_result = write_response(response, response_size, prepared_response);
    if (response_result != COMMAND_OK) {
        return response_result;
    }

    device->mode = (unsigned)mode;
    ++device->accepted_commands;
    return COMMAND_OK;
}

static CommandResult handle_status(DeviceState *device, int argc, char **argv, char *response, size_t response_size)
{
    char prepared_response[COMMAND_RESPONSE_CAPACITY];
    unsigned next_count;
    int written;
    CommandResult response_result;

    (void)argv;
    if (argc != 1) {
        return COMMAND_BAD_ARGUMENT;
    }

    next_count = device->accepted_commands + 1U;
    written = snprintf(prepared_response, sizeof(prepared_response),
                       "LED=%s MODE=%u COUNT=%u",
                       device->led_on ? "ON" : "OFF", device->mode, next_count);
    if (written < 0 || (size_t)written >= sizeof(prepared_response)) {
        return COMMAND_RESPONSE_TOO_SMALL;
    }
    response_result = write_response(response, response_size, prepared_response);
    if (response_result != COMMAND_OK) {
        return response_result;
    }

    device->accepted_commands = next_count;
    return COMMAND_OK;
}

static const CommandEntry COMMANDS[] = {
    {"led", handle_led},
    {"mode", handle_mode},
    {"status", handle_status},
};

static bool tokenize_line(char *line, char **argv, size_t argv_capacity, size_t *argc)
{
    char *cursor;
    size_t count = 0U;

    if (line == NULL || argv == NULL || argc == NULL || argv_capacity == 0U) {
        return false;
    }

    cursor = line;
    while (*cursor != '\0') {
        while (*cursor == ' ') {
            ++cursor;
        }
        if (*cursor == '\0') {
            break;
        }
        if (count == argv_capacity) {
            return false;
        }

        argv[count++] = cursor;
        while (*cursor != '\0' && *cursor != ' ') {
            ++cursor;
        }
        if (*cursor == ' ') {
            *cursor = '\0';
            ++cursor;
        }
    }

    *argc = count;
    return true;
}

static CommandResult execute_line(CommandEngine *engine, char *response, size_t response_size)
{
    char *argv[COMMAND_MAX_ARGUMENTS];
    size_t argc = 0U;
    size_t i;

    if (!tokenize_line(engine->line, argv, COMMAND_MAX_ARGUMENTS, &argc)) {
        return COMMAND_BAD_ARGUMENT;
    }
    if (argc == 0U) {
        return COMMAND_EMPTY;
    }
    for (i = 0U; i < sizeof(COMMANDS) / sizeof(COMMANDS[0]); ++i) {
        if (strcmp(argv[0], COMMANDS[i].name) == 0) {
            return COMMANDS[i].handler(&engine->device, (int)argc, argv,
                                       response, response_size);
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
                CommandResult response_result;

                engine->dropping_line = false;
                engine->line_length = 0U;
                response_result = write_response(response, response_size,
                                                 "ERR INPUT_OVERFLOW");
                *result = response_result == COMMAND_OK
                              ? COMMAND_INPUT_OVERFLOW
                              : response_result;
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
