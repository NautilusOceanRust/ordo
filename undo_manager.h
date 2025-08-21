#ifndef UNDO_MANAGER_H
#define UNDO_MANAGER_H

#include "task.h"
#include <stdbool.h>

// Forward declaration to avoid circular dependency
struct AppState;

// Defines the types of actions that can be undone
typedef enum {
  ACTION_ADD,
  ACTION_DELETE,
  ACTION_EDIT,
  ACTION_TOGGLE
} ActionType;

// Structure that stores a single action for the history
typedef struct {
  ActionType type;
  int task_id;
  char old_data[MAX_DESCRICAO];
  char new_data[MAX_DESCRICAO];
  int old_state;
} Command;

#define UNDO_STACK_SIZE 10

// Structure that manages the undo and redo stacks
typedef struct {
  Command undo_stack[UNDO_STACK_SIZE];
  int undo_top;
  Command redo_stack[UNDO_STACK_SIZE];
  int redo_top;
} UndoManager;

// --- Public Functions ---
void undo_manager_init(UndoManager *manager);
void undo_manager_push(UndoManager *manager, ActionType type, int task_id,
                       const char *old_data, const char *new_data,
                       int old_state);
void undo_manager_clear_redo(UndoManager *manager);
bool undo_manager_perform_undo(UndoManager *manager, struct AppState *app);
bool undo_manager_perform_redo(UndoManager *manager, struct AppState *app);

#endif // UNDO_MANAGER_H
