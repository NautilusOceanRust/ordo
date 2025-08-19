#ifndef UNDO_MANAGER_H
#define UNDO_MANAGER_H

#include "task.h"
#include <stdbool.h>

// Forward declaration para evitar dependência circular
struct AppState;

// Define os tipos de ações que podem ser desfeitas
typedef enum {
  ACTION_ADD,
  ACTION_DELETE,
  ACTION_EDIT,
  ACTION_TOGGLE
} ActionType;

// Estrutura que armazena uma única ação para o histórico
typedef struct {
  ActionType type;
  int task_id;
  char old_data[MAX_DESCRICAO];
  char new_data[MAX_DESCRICAO];
  int old_state;
} Command;

#define UNDO_STACK_SIZE 10

// Estrutura que gerencia as pilhas de undo e redo
typedef struct {
  Command undo_stack[UNDO_STACK_SIZE];
  int undo_top;
  Command redo_stack[UNDO_STACK_SIZE];
  int redo_top;
} UndoManager;

// --- Funções Públicas ---
void undo_manager_init(UndoManager *manager);
void undo_manager_push(UndoManager *manager, ActionType type, int task_id,
                       const char *old_data, const char *new_data,
                       int old_state);
void undo_manager_clear_redo(UndoManager *manager);
bool undo_manager_perform_undo(UndoManager *manager, struct AppState *app);
bool undo_manager_perform_redo(UndoManager *manager, struct AppState *app);

#endif // UNDO_MANAGER_H
