#include "counter.h"

#include <limits.h>

/* Private to this translation unit, not defined in a header. */
static int value;

void counter_reset(void)
{
    value = 0;
}

bool counter_add(int delta, int *new_value)
{
    if (new_value == 0 ||
        (delta > 0 && value > INT_MAX - delta) ||
        (delta < 0 && value < INT_MIN - delta)) {
        return false;
    }
    value += delta;
    *new_value = value;
    return true;
}

int counter_value(void)
{
    return value;
}
