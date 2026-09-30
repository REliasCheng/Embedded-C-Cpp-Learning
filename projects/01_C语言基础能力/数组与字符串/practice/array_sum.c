#include <assert.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

/* Personal practice: pass the buffer length explicitly. */
static bool sum_ints(const int *values, size_t count, int64_t *result)
{
    if (result == NULL || (values == NULL && count != 0)) {
        return false;
    }

    int64_t total = 0;
    for (size_t i = 0; i < count; ++i) {
        int64_t value = values[i];
        if ((value > 0 && total > INT64_MAX - value) ||
            (value < 0 && total < INT64_MIN - value)) {
            return false;
        }
        total += value;
    }
    *result = total;
    return true;
}

int main(void)
{
    const int values[] = {10, 20, 30, 40, 50};
    int64_t result = -1;

    assert(sum_ints(values, sizeof values / sizeof values[0], &result));
    assert(result == 150);
    printf("sum=%" PRId64 "\n", result);
    assert(sum_ints(NULL, 0, &result) && result == 0);
    assert(!sum_ints(NULL, 1, &result));
    assert(!sum_ints(values, 5, NULL));

    return 0;
}
