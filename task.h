#ifndef TASK_H
#define TASK_H

// Define a estrutura de uma tarefa.
// Usada em toda a aplicação para representar uma única tarefa.

#define MAX_DESCRICAO 200

typedef struct {
  int id;
  char descricao[MAX_DESCRICAO];
  int concluida;
  int is_deleted;
} Tarefa;

#endif // TASK_H
