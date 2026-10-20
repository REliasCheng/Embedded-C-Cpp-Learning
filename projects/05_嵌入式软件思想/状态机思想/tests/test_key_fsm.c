#include "key_fsm.h"

#include <assert.h>
#include <stdio.h>

static void test_short_press_and_threshold(void)
{
    KeyMachine machine;
    key_machine_init(&machine);
    assert(machine.state == KEY_IDLE);
    assert(!key_machine_dispatch(&machine, KEY_TICK));
    assert(!key_machine_dispatch(&machine, KEY_RELEASE));

    assert(!key_machine_dispatch(&machine, KEY_PRESS));
    assert(!key_machine_dispatch(&machine, KEY_TICK));
    assert(!key_machine_dispatch(&machine, KEY_TICK));
    assert(key_machine_dispatch(&machine, KEY_RELEASE));
    assert(machine.state == KEY_IDLE);
    assert(machine.ticks == 0);

    assert(!key_machine_dispatch(&machine, KEY_PRESS));
    assert(!key_machine_dispatch(&machine, KEY_TICK));
    assert(!key_machine_dispatch(&machine, KEY_TICK));
    assert(!key_machine_dispatch(&machine, KEY_TICK));
    assert(machine.state == KEY_HELD);
    assert(!key_machine_dispatch(&machine, KEY_RELEASE));
    assert(machine.state == KEY_IDLE);
    assert(machine.ticks == 0);
}

static void test_duplicate_events_and_next_cycle(void)
{
    KeyMachine machine;
    key_machine_init(&machine);
    assert(!key_machine_dispatch(&machine, KEY_PRESS));
    assert(!key_machine_dispatch(&machine, KEY_PRESS));
    assert(key_machine_dispatch(&machine, KEY_RELEASE));
    assert(!key_machine_dispatch(&machine, KEY_RELEASE));
    assert(!key_machine_dispatch(&machine, KEY_PRESS));
    assert(key_machine_dispatch(&machine, KEY_RELEASE));
}

static void test_reinitialization_and_null_input(void)
{
    KeyMachine machine;
    key_machine_init(0);
    assert(!key_machine_dispatch(0, KEY_PRESS));

    key_machine_init(&machine);
    assert(!key_machine_dispatch(&machine, KEY_PRESS));
    assert(!key_machine_dispatch(&machine, KEY_TICK));
    key_machine_init(&machine);
    assert(machine.state == KEY_IDLE);
    assert(machine.ticks == 0);
    assert(!key_machine_dispatch(&machine, KEY_RELEASE));
}

int main(void)
{
    test_short_press_and_threshold();
    test_duplicate_events_and_next_cycle();
    test_reinitialization_and_null_input();
    puts("key FSM host tests passed");
    return 0;
}
