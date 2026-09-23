#include "task_manager.h"

void task_manager_init(TaskManager *manager)
{
    if (manager != NULL) {
        manager->count = 0U;
    }
}

bool task_manager_add(TaskManager *manager, const char *name, uint32_t period_ticks,
                      TaskCallback callback, void *context, size_t *task_id)
{
    TaskSlot *slot;
    if (manager == NULL || name == NULL || callback == NULL || period_ticks == 0U ||
        manager->count == TASK_MANAGER_CAPACITY) {
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
    if (manager == NULL) {
        return;
    }
    for (i = 0U; i < manager->count; ++i) {
        TaskSlot *slot = &manager->tasks[i];
        if (slot->state != TASK_RUNNING) {
            continue;
        }
        if (--slot->remaining_ticks == 0U) {
            slot->remaining_ticks = slot->period_ticks;
            slot->callback(slot->context);
            ++slot->run_count;
        }
    }
}
