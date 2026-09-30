#include "counter.h"

#include <assert.h>
#include <limits.h>
#include <stdio.h>

int main(void)
{
    int current = -1;
    counter_reset();
    assert(counter_add(2, &current) && current == 2);
    assert(counter_add(-1, &current) && current == 1);
    assert(!counter_add(INT_MAX, &current));
    assert(counter_value() == 1);
    printf("counter=%d\n", counter_value());
    return 0;
}
