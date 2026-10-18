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

typedef struct {
    TaskManager *manager;
    unsigned replacement_runs;
    size_t replacement_id;
} ReplacementContext;

static void replace_schedule(void *context)
{
    ReplacementContext *replacement = context;
    task_manager_init(replacement->manager);
    assert(task_manager_add(replacement->manager, "replacement", 1U, increment,
                            &replacement->replacement_runs, &replacement->replacement_id));
    assert(task_manager_start(replacement->manager, replacement->replacement_id));
}

static void test_callback_reinitialization_does_not_count_replacement(void)
{
    TaskManager manager;
    ReplacementContext replacement = {&manager, 0U, 99U};
    size_t original_id;

    task_manager_init(&manager);
    assert(task_manager_add(&manager, "original", 1U, replace_schedule,
                            &replacement, &original_id));
    assert(task_manager_start(&manager, original_id));
    task_manager_tick(&manager);
    assert(manager.count == 1U);
    assert(replacement.replacement_id == 0U);
    assert(replacement.replacement_runs == 0U);
    assert(manager.tasks[0].run_count == 0U);

    task_manager_tick(&manager);
    assert(replacement.replacement_runs == 1U);
    assert(manager.tasks[0].run_count == 1U);
}

typedef struct {
    TaskManager *manager;
    unsigned appended_runs;
    size_t appended_id;
} AppendContext;

static void append_running_task(void *context)
{
    AppendContext *append = context;
    assert(task_manager_add(append->manager, "appended", 1U, increment,
                            &append->appended_runs, &append->appended_id));
    assert(task_manager_start(append->manager, append->appended_id));
}

static void test_callback_append_waits_until_next_tick(void)
{
    TaskManager manager;
    AppendContext append = {&manager, 0U, 99U};
    size_t original_id;

    task_manager_init(&manager);
    assert(task_manager_add(&manager, "original", 1U, append_running_task,
                            &append, &original_id));
    assert(task_manager_start(&manager, original_id));
    task_manager_tick(&manager);
    assert(append.appended_id == 1U);
    assert(append.appended_runs == 0U);
    task_manager_tick(&manager);
    assert(append.appended_runs == 1U);
}

typedef struct {
    TaskManager *manager;
    unsigned replacement_runs[2];
} ReplaceAllContext;

static void replace_multiple_tasks(void *context)
{
    ReplaceAllContext *replacement = context;
    size_t first_id;
    size_t second_id;

    task_manager_init(replacement->manager);
    assert(task_manager_add(replacement->manager, "first", 1U, increment,
                            &replacement->replacement_runs[0], &first_id));
    assert(task_manager_add(replacement->manager, "second", 1U, increment,
                            &replacement->replacement_runs[1], &second_id));
    assert(task_manager_start(replacement->manager, first_id));
    assert(task_manager_start(replacement->manager, second_id));
}

static void test_callback_reinitialization_stops_current_tick(void)
{
    TaskManager manager;
    ReplaceAllContext replacement = {&manager, {0U, 0U}};
    size_t original_id;
    size_t other_id;
    unsigned old_other_runs = 0U;

    task_manager_init(&manager);
    assert(task_manager_add(&manager, "original", 1U, replace_multiple_tasks,
                            &replacement, &original_id));
    assert(task_manager_add(&manager, "old other", 1U, increment,
                            &old_other_runs, &other_id));
    assert(task_manager_start(&manager, original_id));
    assert(task_manager_start(&manager, other_id));
    task_manager_tick(&manager);
    assert(old_other_runs == 0U);
    assert(replacement.replacement_runs[0] == 0U);
    assert(replacement.replacement_runs[1] == 0U);
    task_manager_tick(&manager);
    assert(replacement.replacement_runs[0] == 1U);
    assert(replacement.replacement_runs[1] == 1U);
}

typedef struct {
    TaskManager *manager;
    size_t other_id;
} StopOtherContext;

static void stop_other_task(void *context)
{
    StopOtherContext *stop = context;
    assert(task_manager_stop(stop->manager, stop->other_id));
}

static void test_callback_stop_prevents_later_run(void)
{
    TaskManager manager;
    StopOtherContext stop = {&manager, 99U};
    size_t first_id;
    unsigned other_runs = 0U;

    task_manager_init(&manager);
    assert(task_manager_add(&manager, "stopper", 1U, stop_other_task,
                            &stop, &first_id));
    assert(task_manager_add(&manager, "other", 1U, increment,
                            &other_runs, &stop.other_id));
    assert(task_manager_start(&manager, first_id));
    assert(task_manager_start(&manager, stop.other_id));
    task_manager_tick(&manager);
    assert(other_runs == 0U);
    assert(manager.tasks[stop.other_id].run_count == 0U);
    assert(manager.tasks[stop.other_id].state == TASK_STOPPED);
}

int main(void)
{
    test_schedule_lifecycle();
    test_fault_and_reset();
    test_invalid_arguments_and_ids();
    test_capacity_and_duplicate_names();
    test_callback_reinitialization_does_not_count_replacement();
    test_callback_append_waits_until_next_tick();
    test_callback_reinitialization_stops_current_tick();
    test_callback_stop_prevents_later_run();

    puts("task manager tests passed (8 cases)");
    return 0;
}
