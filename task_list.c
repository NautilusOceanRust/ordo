#include "task_list.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INITIAL_CAPACITY 10

void task_list_init(TaskList *list) {
    list->tasks = malloc(INITIAL_CAPACITY * sizeof(Tarefa));
    if (list->tasks == NULL) {
        // Em um aplicativo real, um tratamento de erro mais robusto seria necessário.
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

bool task_list_add(TaskList *list, const Tarefa *task) {
    if (list->count >= list->capacity) {
        int new_capacity = list->capacity * 2;
        Tarefa *new_tasks = realloc(list->tasks, new_capacity * sizeof(Tarefa));
        if (new_tasks == NULL) {
            return false; // Falha na realocação
        }
        list->tasks = new_tasks;
        list->capacity = new_capacity;
    }
    
    // Copia os dados da tarefa para a lista.
    list->tasks[list->count].id = task->id;
    list->tasks[list->count].concluida = task->concluida;
    safe_snprintf(list->tasks[list->count].descricao, MAX_DESCRICAO, "%s", task->descricao);

    list->count++;
    return true;
}

void task_list_remove(TaskList *list, int index) {
    if (index < 0 || index >= list->count) {
        return; // Índice inválido
    }

    // Move as tarefas subsequentes para preencher o espaço.
    if (index < list->count - 1) {
        safe_memmove(&list->tasks[index], &list->tasks[index + 1], (list->count - index - 1) * sizeof(Tarefa));
    }

    list->count--;
}

void task_list_clear(TaskList *list) {
    // Apenas redefine a contagem. A memória alocada pode ser reutilizada.
    list->count = 0;
}
