#include "task_manager.h"

/* Main-loop only: distinguishes a new schedule from slots reused in a callback. */
static uint64_t next_generation = UINT64_C(0);

void task_manager_init(TaskManager *manager)
{
    if (manager != NULL) {
        ++next_generation;
        manager->generation = next_generation;
        manager->count = 0U;
    }
}

bool task_manager_add(TaskManager *manager, const char *name, uint32_t period_ticks,
                      TaskCallback callback, void *context, size_t *task_id)
{
    TaskSlot *slot;
    if (manager == NULL || name == NULL || callback == NULL || period_ticks == 0U ||
        manager->count >= TASK_MANAGER_CAPACITY) {
        return false;
    }
    slot = &manager->tasks[manager->count];
    slot->name = name;
    slot->period_ticks = period_ticks;
    slot->remaining_ticks = period_ticks;
    slot->run_count = 0U;
    slot->state = TASK_STOPPED;
    slot->callback = callback;
    slot->context = context;
    if (task_id != NULL) {
        *task_id = manager->count;
    }
    ++manager->count;
    return true;
}

static bool valid_task(const TaskManager *manager, size_t task_id)
{
    return manager != NULL && task_id < manager->count;
}

bool task_manager_start(TaskManager *manager, size_t task_id)
{
    if (!valid_task(manager, task_id) || manager->tasks[task_id].state == TASK_FAULTED) {
        return false;
    }
    manager->tasks[task_id].remaining_ticks = manager->tasks[task_id].period_ticks;
    manager->tasks[task_id].state = TASK_RUNNING;
    return true;
}

bool task_manager_stop(TaskManager *manager, size_t task_id)
{
    if (!valid_task(manager, task_id)) {
        return false;
    }
    manager->tasks[task_id].state = TASK_STOPPED;
    return true;
}

bool task_manager_fault(TaskManager *manager, size_t task_id)
{
    if (!valid_task(manager, task_id)) {
        return false;
    }
    manager->tasks[task_id].state = TASK_FAULTED;
    return true;
}

bool task_manager_reset(TaskManager *manager, size_t task_id)
{
    if (!valid_task(manager, task_id)) {
        return false;
    }
    manager->tasks[task_id].remaining_ticks = manager->tasks[task_id].period_ticks;
    manager->tasks[task_id].state = TASK_STOPPED;
    return true;
}

void task_manager_tick(TaskManager *manager)
{
    size_t i;
    size_t initial_count;
    uint64_t generation;
    if (manager == NULL) {
        return;
    }
    initial_count = manager->count;
    if (initial_count > TASK_MANAGER_CAPACITY) {
        return;
    }
    generation = manager->generation;
    for (i = 0U; i < initial_count; ++i) {
        TaskSlot *slot = &manager->tasks[i];
        if (manager->generation != generation || i >= manager->count) {
            return;
        }
        if (slot->state != TASK_RUNNING) {
            continue;
        }
        if (--slot->remaining_ticks == 0U) {
            slot->remaining_ticks = slot->period_ticks;
            ++slot->run_count;
            slot->callback(slot->context);
            if (manager->generation != generation) {
                return;
            }
        }
    }
}
