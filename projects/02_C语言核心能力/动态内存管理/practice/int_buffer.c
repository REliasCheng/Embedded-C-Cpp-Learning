#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/* Personal practice: the caller owns the allocated buffer. */
static bool int_buffer_create(size_t count, int **buffer)
{
    if (buffer == NULL) {
        return false;
    }
    *buffer = NULL;
    if (count == 0) {
        return true;
    }
    if (count > SIZE_MAX / sizeof(int)) {
        return false;
    }

    *buffer = calloc(count, sizeof(int));
    return *buffer != NULL;
}

static void int_buffer_release(int **buffer)
{
    if (buffer != NULL) {
        free(*buffer);
        *buffer = NULL;
    }
}

int main(void)
{
    int *buffer = NULL;
    assert(int_buffer_create(3, &buffer));
    assert(buffer != NULL && buffer[0] == 0);
    buffer[1] = 200;
    assert(buffer[1] == 200);
    printf("buffer[1]=%d\n", buffer[1]);
    int_buffer_release(&buffer);
    assert(buffer == NULL);

    assert(int_buffer_create(0, &buffer) && buffer == NULL);
    assert(!int_buffer_create(SIZE_MAX, &buffer));
    assert(!int_buffer_create(1, NULL));
    return 0;
}
