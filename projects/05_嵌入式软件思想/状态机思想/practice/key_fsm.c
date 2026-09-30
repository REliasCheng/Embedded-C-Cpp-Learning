#include "key_fsm.h"

void key_machine_init(KeyMachine *machine)
{
    if (machine != 0) {
        machine->state = KEY_IDLE;
        machine->ticks = 0;
    }
}

/* Return true only for a completed short press. No hardware I/O is used. */
bool key_machine_dispatch(KeyMachine *machine, KeyEvent event)
{
    if (machine == 0) {
        return false;
    }

    switch (machine->state) {
    case KEY_IDLE:
        if (event == KEY_PRESS) {
            machine->state = KEY_PRESSED;
            machine->ticks = 0;
        }
        break;
    case KEY_PRESSED:
        if (event == KEY_RELEASE) {
            machine->state = KEY_IDLE;
            machine->ticks = 0;
            return true;
        }
        if (event == KEY_TICK) {
            if (++machine->ticks >= 3) {
                machine->state = KEY_HELD;
            }
        }
        break;
    case KEY_HELD:
        if (event == KEY_RELEASE) {
            machine->state = KEY_IDLE;
            machine->ticks = 0;
        }
        break;
    }
    return false;
}
