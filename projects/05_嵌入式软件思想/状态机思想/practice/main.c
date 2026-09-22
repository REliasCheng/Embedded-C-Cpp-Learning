#include "key_fsm.h"

#include <assert.h>
#include <stdio.h>

int main(void)
{
    KeyMachine machine;
    key_machine_init(&machine);
    assert(machine.state == KEY_IDLE);
    assert(!key_machine_dispatch(&machine, KEY_PRESS));
    assert(key_machine_dispatch(&machine, KEY_RELEASE));
    assert(machine.state == KEY_IDLE);

    key_machine_dispatch(&machine, KEY_PRESS);
    for (int i = 0; i < 3; ++i) {
        assert(!key_machine_dispatch(&machine, KEY_TICK));
    }
    assert(machine.state == KEY_HELD);
    assert(!key_machine_dispatch(&machine, KEY_RELEASE));
    assert(machine.state == KEY_IDLE);
    printf("short and long sequences passed\n");
    return 0;
}
