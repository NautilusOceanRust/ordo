/**
 * @file app.h
 * @brief Central declarations for the Ordo application state and lifecycle.
 *
 * This file defines the main `AppState` structure that encapsulates the entire
 * state of the application, as well as the functions to initialize and destroy the application.
 */

#ifndef APP_H
#define APP_H

#include "config.h"
#include "database.h"
#include "task_list.h"
#include "undo_manager.h"
#include <stdbool.h>
#include <sqlite3.h>

/**
 * @enum AppView
 * @brief Enum to control the view (screen) currently displayed in the UI.
 */
typedef enum {
  VIEW_MAIN,  /**< The main view, showing active tasks. */
  VIEW_TRASH  /**< The trash view, showing removed tasks. */
} AppView;

/**
 * @struct AppState
 * @brief Central structure that contains the entire state of the application.
 *
 * This structure aggregates all the main components of the application, such as
 * the database handler, settings, task lists, and UI state.
 */
typedef struct AppState {
  Database db;                  /**< Database handler. */
  AppConfig config;             /**< Application settings (theme, language). */
  TaskList task_list;           /**< The currently displayed task list. */
  UndoManager undo_manager;     /**< Manager for undo/redo actions. */

  AppView current_view;         /**< The current view (screen). */
  int current_selection;        /**< The index of the currently selected task. */
  int scroll_offset;            /**< The scroll offset in the task list. */

  bool should_exit;             /**< Flag to signal the end of the application. */
  bool refresh_tasks;           /**< Flag to signal the need to reload tasks from the DB. */
} AppState;

// --- Lifecycle Functions ---

/**
 * @brief Initializes the application.
 *
 * Sets up the locale, initializes the database, the UI, the internationalization
 * manager, and loads the settings.
 *
 * @param[out] app Pointer to the AppState structure to be initialized.
 * @return `true` if initialization is successful, `false` otherwise.
 */
bool app_init(AppState *app);

/**
 * @brief Frees all resources allocated by the application.
 *
 * Closes the UI, the database, and frees the memory used by the task list
 * and other components.
 *
 * @param[in] app Pointer to the AppState structure to be destroyed.
 */
void app_destroy(AppState *app);

#endif // APP_H
