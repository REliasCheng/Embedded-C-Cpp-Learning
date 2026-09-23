#ifndef COMMAND_H
#define COMMAND_H

#include "device.h"
#include "ring_buffer.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define COMMAND_LINE_CAPACITY 48U

typedef enum {
    COMMAND_OK,
    COMMAND_EMPTY,
    COMMAND_UNKNOWN,
    COMMAND_BAD_ARGUMENT,
    COMMAND_INPUT_OVERFLOW,
    COMMAND_RESPONSE_TOO_SMALL
} CommandResult;

typedef struct {
    RingBuffer rx;
    DeviceState device;
    char line[COMMAND_LINE_CAPACITY];
    size_t line_length;
    bool dropping_line;
} CommandEngine;

void command_engine_init(CommandEngine *engine);
bool command_engine_feed(CommandEngine *engine, uint8_t byte);
bool command_engine_process(CommandEngine *engine, char *response, size_t response_size, CommandResult *result);

#endif
