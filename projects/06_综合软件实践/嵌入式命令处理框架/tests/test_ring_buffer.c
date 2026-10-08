#include "ring_buffer.h"

#include <assert.h>
#include <stdio.h>

static void test_null_and_empty_arguments(void)
{
    RingBuffer buffer;
    uint8_t value = 0xAAU;

    ring_buffer_init(NULL);
    ring_buffer_init(&buffer);
    assert(!ring_buffer_pop(&buffer, &value));
    assert(value == 0xAAU);
    assert(!ring_buffer_pop(NULL, &value));
    assert(!ring_buffer_pop(&buffer, NULL));
    assert(!ring_buffer_push(NULL, 1U));
    assert(!ring_buffer_take_overflow(NULL));
}

static void test_fifo_full_and_overflow(void)
{
    RingBuffer buffer;
    uint8_t value;
    size_t i;

    ring_buffer_init(&buffer);
    for (i = 0U; i < RING_BUFFER_CAPACITY; ++i) {
        assert(ring_buffer_push(&buffer, (uint8_t)i));
    }
    assert(buffer.count == RING_BUFFER_CAPACITY);
    assert(!ring_buffer_push(&buffer, 0xFFU));
    assert(ring_buffer_take_overflow(&buffer));
    assert(!ring_buffer_take_overflow(&buffer));

    for (i = 0U; i < RING_BUFFER_CAPACITY; ++i) {
        assert(ring_buffer_pop(&buffer, &value));
        assert(value == (uint8_t)i);
    }
    assert(buffer.count == 0U);
    assert(!ring_buffer_pop(&buffer, &value));
}

static void test_wrap_around_and_reset(void)
{
    RingBuffer buffer;
    uint8_t value;
    size_t i;
    const size_t half = RING_BUFFER_CAPACITY / 2U;

    ring_buffer_init(&buffer);
    for (i = 0U; i < RING_BUFFER_CAPACITY; ++i) {
        assert(ring_buffer_push(&buffer, (uint8_t)i));
    }
    for (i = 0U; i < half; ++i) {
        assert(ring_buffer_pop(&buffer, &value));
        assert(value == (uint8_t)i);
    }
    for (i = 0U; i < half; ++i) {
        assert(ring_buffer_push(&buffer, (uint8_t)(RING_BUFFER_CAPACITY + i)));
    }
    for (i = half; i < RING_BUFFER_CAPACITY + half; ++i) {
        assert(ring_buffer_pop(&buffer, &value));
        assert(value == (uint8_t)i);
    }

    assert(ring_buffer_push(&buffer, 42U));
    ring_buffer_init(&buffer);
    assert(buffer.count == 0U);
    assert(buffer.head == 0U);
    assert(buffer.tail == 0U);
    assert(!buffer.overflowed);
    assert(!ring_buffer_pop(&buffer, &value));
}

int main(void)
{
    test_null_and_empty_arguments();
    test_fifo_full_and_overflow();
    test_wrap_around_and_reset();

    puts("ring buffer tests passed (3 cases)");
    return 0;
}
