#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define RING_BUFFER_CAPACITY 64U

typedef struct {
    uint8_t data[RING_BUFFER_CAPACITY];
    size_t head;
    size_t tail;
    size_t count;
    bool overflowed;
} RingBuffer;

void ring_buffer_init(RingBuffer *buffer);
bool ring_buffer_push(RingBuffer *buffer, uint8_t value);
bool ring_buffer_pop(RingBuffer *buffer, uint8_t *value);
bool ring_buffer_take_overflow(RingBuffer *buffer);

#endif
