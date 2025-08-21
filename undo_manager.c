#include "undo_manager.h"
#include "app.h" // For database access
#include "error.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>

// Internal helper function to push a command onto a stack
static void push_command(Command *stack, int *top, const Command *cmd) {
  if (*top >= UNDO_STACK_SIZE - 1) {
    // If the stack is full, discard the oldest item
    safe_memmove(&stack[0], &stack[1], (UNDO_STACK_SIZE - 1) * sizeof(Command));
    *top = UNDO_STACK_SIZE - 2;
  }
  (*top)++;
  stack[*top] = *cmd;
}

void undo_manager_init(UndoManager *manager) {
  manager->undo_top = -1;
  manager->redo_top = -1;
}

void undo_manager_push(UndoManager *manager, ActionType type, int task_id,
                       const char *old_data, const char *new_data,
                       int old_state) {
  Command cmd;
  cmd.type = type;
  cmd.task_id = task_id;
  cmd.old_state = old_state;

  if (old_data) {
    safe_snprintf(cmd.old_data, MAX_DESCRICAO, "%s", old_data);
  } else {
    cmd.old_data[0] = '\0';
  }

  if (new_data) {
    safe_snprintf(cmd.new_data, MAX_DESCRICAO, "%s", new_data);
  } else {
    cmd.new_data[0] = '\0';
  }

  push_command(manager->undo_stack, &manager->undo_top, &cmd);
}

void undo_manager_clear_redo(UndoManager *manager) {
  manager->redo_top = -1;
}

bool undo_manager_perform_undo(UndoManager *manager, AppState *app) {
  if (manager->undo_top < 0) {
    return false;
  }

  Command cmd = manager->undo_stack[manager->undo_top--];
  OrdoResult result = ORDO_OK;

  switch (cmd.type) {
  case ACTION_ADD:
    result = database_remove_task(&app->db, cmd.task_id);
    break;
  case ACTION_DELETE:
    result = database_restore_task(&app->db, cmd.task_id);
    break;
  case ACTION_EDIT:
    result =
        database_update_task_description(&app->db, cmd.task_id, cmd.old_data);
    break;
  case ACTION_TOGGLE:
    result = database_toggle_task_status(&app->db, cmd.task_id, !cmd.old_state);
    break;
  }

  if (result == ORDO_OK) {
    push_command(manager->redo_stack, &manager->redo_top, &cmd);
    return true;
  }

  // If the operation fails, restore the command to the undo stack
  manager->undo_top++;
  return false;
}

bool undo_manager_perform_redo(UndoManager *manager, AppState *app) {
  if (manager->redo_top < 0) {
    return false;
  }

  Command cmd = manager->redo_stack[manager->redo_top--];
  OrdoResult result = ORDO_OK;

  switch (cmd.type) {
  case ACTION_ADD:
    result = database_restore_task(&app->db, cmd.task_id);
    break;
  case ACTION_DELETE:
    result = database_remove_task(&app->db, cmd.task_id);
    break;
  case ACTION_EDIT:
    result =
        database_update_task_description(&app->db, cmd.task_id, cmd.new_data);
    break;
  case ACTION_TOGGLE:
    result = database_toggle_task_status(&app->db, cmd.task_id, cmd.old_state);
    break;
  }

  if (result == ORDO_OK) {
    push_command(manager->undo_stack, &manager->undo_top, &cmd);
    return true;
  }

  // If the operation fails, restore the command to the redo stack
  manager->redo_top++;
  return false;
}
