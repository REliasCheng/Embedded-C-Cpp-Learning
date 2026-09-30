#ifndef TASK_MANAGER_H
#define TASK_MANAGER_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define TASK_MANAGER_CAPACITY 4U

typedef enum {
    TASK_STOPPED,
    TASK_RUNNING,
    TASK_FAULTED
} TaskState;

typedef void (*TaskCallback)(void *context);

typedef struct {
    const char *name;
    uint32_t period_ticks;
    uint32_t remaining_ticks;
    uint32_t run_count;
    TaskState state;
    TaskCallback callback;
    void *context;
} TaskSlot;

typedef struct {
    TaskSlot tasks[TASK_MANAGER_CAPACITY];
    size_t count;
} TaskManager;

void task_manager_init(TaskManager *manager);
bool task_manager_add(TaskManager *manager, const char *name, uint32_t period_ticks,
                      TaskCallback callback, void *context, size_t *task_id);
bool task_manager_start(TaskManager *manager, size_t task_id);
bool task_manager_stop(TaskManager *manager, size_t task_id);
bool task_manager_fault(TaskManager *manager, size_t task_id);
bool task_manager_reset(TaskManager *manager, size_t task_id);
void task_manager_tick(TaskManager *manager);

#endif
