/**
 * @file task_list.h
 * @brief Defines a dynamic list data structure for managing tasks.
 *
 * This module provides an abstraction of a task list that can grow
 * dynamically, avoiding manual memory management in the rest of the application.
 */

#ifndef TASK_LIST_H
#define TASK_LIST_H

#include "task.h"
#include <stdbool.h>

/**
 * @struct TaskList
 * @brief Structure to encapsulate a dynamic list of tasks.
 */
typedef struct {
    Task *tasks;      /**< Pointer to the dynamically allocated array of tasks. */
    int count;          /**< The current number of tasks in the list. */
    int capacity;       /**< The current capacity of the task array. */
} TaskList;

// --- Lifecycle Functions ---

/**
 * @brief Initializes a new task list.
 *
 * Allocates the initial memory for the task list.
 * @param[out] list Pointer to the `TaskList` to be initialized.
 */
void task_list_init(TaskList *list);

/**
 * @brief Frees the memory allocated by the task list.
 * @param[in] list Pointer to the `TaskList` to be freed.
 */
void task_list_free(TaskList *list);

// --- Manipulation Functions ---

/**
 * @brief Adds a copy of a task to the list.
 *
 * The function manages memory reallocation if the current capacity is exceeded.
 * @param[in,out] list Pointer to the `TaskList`.
 * @param[in] task Pointer to the task to be added. A copy will be made.
 * @return `true` if the task was added successfully, `false` if memory reallocation fails.
 */
bool task_list_add(TaskList *list, const Task *task);

/**
 * @brief Removes a task from the list based on its index.
 * @param[in,out] list Pointer to the `TaskList`.
 * @param[in] index The index of the task to be removed.
 */
void task_list_remove(TaskList *list, int index);

/**
 * @brief Clears the task list.
 *
 * Resets the task count to zero, but does not free the allocated memory,
 * allowing for reuse.
 * @param[in,out] list Pointer to the `TaskList` to be cleared.
 */
void task_list_clear(TaskList *list); 

#endif // TASK_LIST_H
