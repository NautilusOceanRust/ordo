/**
 * @file database.h
 * @brief Interface for the SQLite database management module.
 *
 * This file defines the `Database` structure and the functions to interact
 * with the task database, including initialization, closing, and
 * CRUD (Create, Read, Update, Delete) operations.
 */

#ifndef DATABASE_H
#define DATABASE_H

#define DATABASE_NAME "ordo.db"

#include "error.h"
#include "task.h"
#include "task_list.h"
#include <sqlite3.h>

/**
 * @struct Database
 * @brief Structure to manage the database connection and prepared statements.
 *
 * Groups the SQLite connection and all pre-compiled `sqlite3_stmt` to
 * optimize repeated database operations.
 */
typedef struct {
  sqlite3 *db;                                /**< Pointer to the SQLite database connection. */
  sqlite3_stmt *add_task_stmt;                /**< Statement to add a new task. */
  sqlite3_stmt *remove_task_stmt;             /**< Statement to move a task to the trash. */
  sqlite3_stmt *toggle_task_status_stmt;      /**< Statement to toggle the state (completed/pending) of a task. */
  sqlite3_stmt *update_task_description_stmt; /**< Statement to update the description of a task. */
  sqlite3_stmt *load_tasks_stmt;              /**< Statement to load active tasks. */
  sqlite3_stmt *restore_task_stmt;            /**< Statement to restore a task from the trash. */
  sqlite3_stmt *perm_delete_task_stmt;        /**< Statement to permanently delete a task. */
  sqlite3_stmt *load_deleted_tasks_stmt;      /**< Statement to load tasks from the trash. */
} Database;

/**
 * @brief Initializes the database connection and prepares the statements.
 * @param[in] db_conn An open SQLite connection.
 * @param[out] db Pointer to the `Database` structure to be initialized.
 * @return `ORDO_OK` on success, or an error code on failure.
 */
OrdoResult database_init(sqlite3 *db_conn, Database *db);

/**
 * @brief Finalizes the prepared statements and closes the database connection.
 * @param[in] db Pointer to the `Database` structure to be finalized.
 */
void database_close(Database *db);

/**
 * @brief Loads the active tasks (not in the trash) from the database.
 * @param[in] db Pointer to the `Database` structure.
 * @param[out] list Pointer to the `TaskList` where the tasks will be loaded.
 * @return `ORDO_OK` on success.
 */
OrdoResult database_load_tasks(Database *db, TaskList *list);

/**
 * @brief Loads the tasks from the trash (removed) from the database.
 * @param[in] db Pointer to the `Database` structure.
 * @param[out] list Pointer to the `TaskList` where the tasks will be loaded.
 * @return `ORDO_OK` on success.
 */
OrdoResult database_load_deleted_tasks(Database *db, TaskList *list);

/**
 * @brief Adds a new task to the database.
 * @param[in] db Pointer to the `Database` structure.
 * @param[in] description The description of the new task.
 * @param[out] new_task_id Pointer to store the ID of the newly created task.
 * @return `ORDO_OK` on success.
 */
OrdoResult database_add_task(Database *db, const char *description,
                             int *new_task_id);

/**
 * @brief Moves a task to the trash (soft delete).
 * @param[in] db Pointer to the `Database` structure.
 * @param[in] task_id The ID of the task to be moved to the trash.
 * @return `ORDO_OK` on success.
 */
OrdoResult database_remove_task(Database *db, int task_id);

/**
 * @brief Restores a task from the trash.
 * @param[in] db Pointer to the `Database` structure.
 * @param[in] task_id The ID of the task to be restored.
 * @return `ORDO_OK` on success.
 */
OrdoResult database_restore_task(Database *db, int task_id);

/**
 * @brief Permanently deletes a task from the database.
 * @param[in] db Pointer to the `Database` structure.
 * @param[in] task_id The ID of the task to be permanently deleted.
 * @return `ORDO_OK` on success.
 */
OrdoResult database_permanently_delete_task(Database *db, int task_id);

/**
 * @brief Toggles the status of a task between completed and pending.
 * @param[in] db Pointer to the `Database` structure.
 * @param[in] task_id The ID of the task to be modified.
 * @param[in] current_status The current status of the task (0 for pending, 1 for completed).
 * @return `ORDO_OK` on success.
 */
OrdoResult database_toggle_task_status(Database *db, int task_id,
                                       int current_status);

/**
 * @brief Updates the description of an existing task.
 * @param[in] db Pointer to the `Database` structure.
 * @param[in] task_id The ID of the task to be updated.
 * @param[in] new_description The new description for the task.
 * @return `ORDO_OK` on success.
 */
OrdoResult database_update_task_description(Database *db, int task_id,
                                            const char *new_description);

#endif // DATABASE_H
