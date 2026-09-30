#ifndef KEY_FSM_H
#define KEY_FSM_H

#include <stdbool.h>

typedef enum {
    KEY_IDLE,
    KEY_PRESSED,
    KEY_HELD
} KeyState;

typedef enum {
    KEY_PRESS,
    KEY_RELEASE,
    KEY_TICK
} KeyEvent;

typedef struct {
    KeyState state;
    unsigned ticks;
} KeyMachine;

void key_machine_init(KeyMachine *machine);
bool key_machine_dispatch(KeyMachine *machine, KeyEvent event);

#endif
