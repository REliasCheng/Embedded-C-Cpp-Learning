#include "task_manager.h"

#include <assert.h>
#include <stdio.h>

static void increment(void *context)
{
    unsigned *value = context;
    ++(*value);
}

static void test_schedule_lifecycle(void)
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

    assert(task_manager_stop(&manager, display_id));
    for (i = 0U; i < 3U; ++i) {
        task_manager_tick(&manager);
    }
    assert(sensor_runs == 4U);
    assert(display_runs == 2U);
}

static void test_fault_and_reset(void)
{
    TaskManager manager;
    size_t task_id;
    unsigned runs = 0U;

    task_manager_init(&manager);
    assert(task_manager_add(&manager, "worker", 1U, increment, &runs, &task_id));
    assert(task_manager_start(&manager, task_id));
    task_manager_tick(&manager);
    assert(runs == 1U);

    assert(task_manager_fault(&manager, task_id));
    task_manager_tick(&manager);
    assert(runs == 1U);
    assert(!task_manager_start(&manager, task_id));

    assert(task_manager_reset(&manager, task_id));
    assert(task_manager_start(&manager, task_id));
    task_manager_tick(&manager);
    assert(runs == 2U);
}

static void test_invalid_arguments_and_ids(void)
{
    TaskManager manager;
    size_t task_id = 99U;
    unsigned runs = 0U;

    task_manager_init(NULL);
    task_manager_tick(NULL);
    task_manager_init(&manager);
    task_manager_tick(&manager);

    assert(!task_manager_add(NULL, "task", 1U, increment, &runs, &task_id));
    assert(!task_manager_add(&manager, NULL, 1U, increment, &runs, &task_id));
    assert(!task_manager_add(&manager, "task", 0U, increment, &runs, &task_id));
    assert(!task_manager_add(&manager, "task", 1U, NULL, &runs, &task_id));
    assert(task_id == 99U);
    assert(manager.count == 0U);

    assert(!task_manager_start(&manager, 0U));
    assert(!task_manager_stop(&manager, 0U));
    assert(!task_manager_fault(&manager, 0U));
    assert(!task_manager_reset(&manager, 0U));
    assert(!task_manager_start(NULL, 0U));
}

static void test_capacity_and_duplicate_names(void)
{
    struct {
        TaskManager manager;
        unsigned guard;
    } guarded = {0};
    size_t task_ids[TASK_MANAGER_CAPACITY];
    size_t rejected_id = 77U;
    unsigned runs[TASK_MANAGER_CAPACITY] = {0U};
    size_t i;

    guarded.guard = 0xC0FFEEU;
    task_manager_init(&guarded.manager);
    for (i = 0U; i < TASK_MANAGER_CAPACITY; ++i) {
        assert(task_manager_add(&guarded.manager, "duplicate-name", 1U,
                                increment, &runs[i], &task_ids[i]));
        assert(task_ids[i] == i);
    }

    assert(!task_manager_add(&guarded.manager, "overflow", 1U,
                             increment, &runs[0], &rejected_id));
    assert(guarded.manager.count == TASK_MANAGER_CAPACITY);
    assert(rejected_id == 77U);
    assert(guarded.guard == 0xC0FFEEU);

    for (i = 0U; i < TASK_MANAGER_CAPACITY; ++i) {
        assert(task_manager_start(&guarded.manager, task_ids[i]));
    }
    task_manager_tick(&guarded.manager);
    for (i = 0U; i < TASK_MANAGER_CAPACITY; ++i) {
        assert(runs[i] == 1U);
    }
}

int main(void)
{
    test_schedule_lifecycle();
    test_fault_and_reset();
    test_invalid_arguments_and_ids();
    test_capacity_and_duplicate_names();

    puts("task manager tests passed (4 cases)");
    return 0;
}
