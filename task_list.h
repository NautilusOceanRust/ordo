/**
 * @file task_list.h
 * @brief Define uma estrutura de dados de lista dinâmica para gerenciar tarefas.
 *
 * Este módulo fornece uma abstração de uma lista de tarefas que pode crescer
 * dinamicamente, evitando o gerenciamento manual de memória no resto da aplicação.
 */

#ifndef TASK_LIST_H
#define TASK_LIST_H

#include "task.h"
#include <stdbool.h>

/**
 * @struct TaskList
 * @brief Estrutura para encapsular uma lista dinâmica de tarefas.
 */
typedef struct {
    Tarefa *tasks;      /**< Ponteiro para o array de tarefas alocado dinamicamente. */
    int count;          /**< O número atual de tarefas na lista. */
    int capacity;       /**< A capacidade atual do array de tarefas. */
} TaskList;

// --- Funções de Ciclo de Vida ---

/**
 * @brief Inicializa uma nova lista de tarefas.
 *
 * Aloca a memória inicial para a lista de tarefas.
 * @param[out] list Ponteiro para a `TaskList` a ser inicializada.
 */
void task_list_init(TaskList *list);

/**
 * @brief Libera a memória alocada pela lista de tarefas.
 * @param[in] list Ponteiro para a `TaskList` a ser liberada.
 */
void task_list_free(TaskList *list);

// --- Funções de Manipulação ---

/**
 * @brief Adiciona uma cópia de uma tarefa à lista.
 *
 * A função gerencia a realocação de memória se a capacidade atual for excedida.
 * @param[in,out] list Ponteiro para a `TaskList`.
 * @param[in] task Ponteiro para a tarefa a ser adicionada. Uma cópia será feita.
 * @return `true` se a tarefa foi adicionada com sucesso, `false` se a realocação de memória falhar.
 */
bool task_list_add(TaskList *list, const Tarefa *task);

/**
 * @brief Remove uma tarefa da lista com base em seu índice.
 * @param[in,out] list Ponteiro para a `TaskList`.
 * @param[in] index O índice da tarefa a ser removida.
 */
void task_list_remove(TaskList *list, int index);

/**
 * @brief Limpa a lista de tarefas.
 *
 * Redefine a contagem de tarefas para zero, mas não libera a memória alocada,
 * permitindo a reutilização.
 * @param[in,out] list Ponteiro para a `TaskList` a ser limpa.
 */
void task_list_clear(TaskList *list); 

#endif // TASK_LIST_H
