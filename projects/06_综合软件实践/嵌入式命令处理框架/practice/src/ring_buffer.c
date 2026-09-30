#include "ring_buffer.h"

void ring_buffer_init(RingBuffer *buffer)
{
    if (buffer == NULL) {
        return;
    }
    buffer->head = 0U;
    buffer->tail = 0U;
    buffer->count = 0U;
    buffer->overflowed = false;
}

bool ring_buffer_push(RingBuffer *buffer, uint8_t value)
{
    if (buffer == NULL || buffer->count == RING_BUFFER_CAPACITY) {
        if (buffer != NULL) {
            buffer->overflowed = true;
        }
        return false;
    }
    buffer->data[buffer->head] = value;
    buffer->head = (buffer->head + 1U) % RING_BUFFER_CAPACITY;
    ++buffer->count;
    return true;
}

bool ring_buffer_pop(RingBuffer *buffer, uint8_t *value)
{
    if (buffer == NULL || value == NULL || buffer->count == 0U) {
        return false;
    }
    *value = buffer->data[buffer->tail];
    buffer->tail = (buffer->tail + 1U) % RING_BUFFER_CAPACITY;
    --buffer->count;
    return true;
}

bool ring_buffer_take_overflow(RingBuffer *buffer)
{
    bool overflowed;
    if (buffer == NULL) {
        return false;
    }
    overflowed = buffer->overflowed;
    buffer->overflowed = false;
    return overflowed;
}
