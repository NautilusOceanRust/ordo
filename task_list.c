#include "task_list.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INITIAL_CAPACITY 10

void task_list_init(TaskList *list) {
    list->tasks = malloc(INITIAL_CAPACITY * sizeof(Task));
    if (list->tasks == NULL) {
        // In a real-world application, more robust error handling would be needed,
        // such as returning an error code.
        exit(EXIT_FAILURE); 
    }
    list->count = 0;
    list->capacity = INITIAL_CAPACITY;
}

void task_list_free(TaskList *list) {
    if (list) {
        free(list->tasks);
        list->tasks = NULL;
        list->count = 0;
        list->capacity = 0;
    }
}

bool task_list_add(TaskList *list, const Task *task) {
    if (list->count >= list->capacity) {
        int new_capacity = list->capacity * 2;
        Task *new_tasks = realloc(list->tasks, new_capacity * sizeof(Task));
        if (new_tasks == NULL) {
            return false; // Reallocation failed
        }
        list->tasks = new_tasks;
        list->capacity = new_capacity;
    }
    
    // Copy the task data to the list.
    list->tasks[list->count].id = task->id;
    list->tasks[list->count].concluida = task->concluida;
    safe_snprintf(list->tasks[list->count].descricao, MAX_DESCRICAO, "%s", task->descricao);

    list->count++;
    return true;
}

void task_list_remove(TaskList *list, int index) {
    if (index < 0 || index >= list->count) {
        return; // Invalid index
    }

    // Move subsequent tasks to fill the space.
    if (index < list->count - 1) {
        safe_memmove(&list->tasks[index], &list->tasks[index + 1], (list->count - index - 1) * sizeof(Task));
    }

    list->count--;
}

void task_list_clear(TaskList *list) {
    // Just reset the count. The allocated memory can be reused.
    list->count = 0;
}
