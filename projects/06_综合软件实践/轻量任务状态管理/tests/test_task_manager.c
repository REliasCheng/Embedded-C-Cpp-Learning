#include "task_manager.h"

#include <assert.h>
#include <stdio.h>

static void increment(void *context)
{
    unsigned *value = context;
    ++(*value);
}

int main(void)
{
    TaskManager manager;
    size_t sensor_id;
    size_t display_id;
    unsigned sensor_runs = 0U;
    unsigned display_runs = 0U;
    unsigned i;

    task_manager_init(&manager);
    assert(task_manager_add(&manager, "sensor", 2U, increment, &sensor_runs, &sensor_id));
    assert(task_manager_add(&manager, "display", 3U, increment, &display_runs, &display_id));
    assert(task_manager_start(&manager, sensor_id));
    assert(task_manager_start(&manager, display_id));

    for (i = 0U; i < 6U; ++i) {
        task_manager_tick(&manager);
    }
    assert(sensor_runs == 3U);
    assert(display_runs == 2U);

    assert(task_manager_fault(&manager, sensor_id));
    for (i = 0U; i < 3U; ++i) {
        task_manager_tick(&manager);
    }
    assert(sensor_runs == 3U);
    assert(display_runs == 3U);
    assert(!task_manager_start(&manager, sensor_id));

    assert(task_manager_reset(&manager, sensor_id));
    assert(task_manager_start(&manager, sensor_id));
    task_manager_tick(&manager);
    task_manager_tick(&manager);
    assert(sensor_runs == 4U);
    assert(task_manager_stop(&manager, display_id));

    puts("task manager tests passed");
    return 0;
}
