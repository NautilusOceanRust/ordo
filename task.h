#ifndef TASK_H
#define TASK_H

// Defines the structure of a task.
// Used throughout the application to represent a single task.

#define MAX_DESCRICAO 200

typedef struct {
  int id;
  char descricao[MAX_DESCRICAO];
  int concluida;
  int is_deleted;
} Task;

#endif // TASK_H
