#ifndef COUNTER_H
#define COUNTER_H

#include <stdbool.h>

void counter_reset(void);
bool counter_add(int delta, int *new_value);
int counter_value(void);

#endif
