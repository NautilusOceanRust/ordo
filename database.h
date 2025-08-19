/**
 * @file database.h
 * @brief Interface para o módulo de gerenciamento do banco de dados SQLite.
 *
 * Este arquivo define a estrutura `Database` e as funções para interagir
 * com o banco de dados de tarefas, incluindo inicialização, fechamento e
 * operações CRUD (Create, Read, Update, Delete).
 */

#ifndef DATABASE_H
#define DATABASE_H

#include "error.h"
#include "task.h"
#include "task_list.h"
#include <sqlite3.h>

/**
 * @struct Database
 * @brief Estrutura para gerenciar a conexão com o banco de dados e as statements preparadas.
 *
 * Agrupa a conexão do SQLite e todas as `sqlite3_stmt` pré-compiladas para
 * otimizar as operações repetidas no banco de dados.
 */
typedef struct {
  sqlite3 *db;                                /**< Ponteiro para a conexão do banco de dados SQLite. */
  sqlite3_stmt *add_task_stmt;                /**< Statement para adicionar uma nova tarefa. */
  sqlite3_stmt *remove_task_stmt;             /**< Statement para mover uma tarefa para a lixeira. */
  sqlite3_stmt *toggle_task_status_stmt;      /**< Statement para alternar o estado (concluída/pendente) de uma tarefa. */
  sqlite3_stmt *update_task_description_stmt; /**< Statement para atualizar a descrição de uma tarefa. */
  sqlite3_stmt *load_tasks_stmt;              /**< Statement para carregar as tarefas ativas. */
  sqlite3_stmt *restore_task_stmt;            /**< Statement para restaurar uma tarefa da lixeira. */
  sqlite3_stmt *perm_delete_task_stmt;        /**< Statement para deletar permanentemente uma tarefa. */
  sqlite3_stmt *load_deleted_tasks_stmt;      /**< Statement para carregar as tarefas da lixeira. */
} Database;

/**
 * @brief Inicializa a conexão com o banco de dados e prepara as statements.
 * @param[in] db_conn Uma conexão SQLite aberta.
 * @param[out] db Ponteiro para a estrutura `Database` a ser inicializada.
 * @return `ORDO_OK` em caso de sucesso, ou um código de erro em caso de falha.
 */
OrdoResult database_init(sqlite3 *db_conn, Database *db);

/**
 * @brief Finaliza as statements preparadas e fecha a conexão com o banco de dados.
 * @param[in] db Ponteiro para a estrutura `Database` a ser finalizada.
 */
void database_close(Database *db);

/**
 * @brief Carrega as tarefas ativas (não na lixeira) do banco de dados.
 * @param[in] db Ponteiro para a estrutura `Database`.
 * @param[out] list Ponteiro para a `TaskList` onde as tarefas serão carregadas.
 * @return `ORDO_OK` em caso de sucesso.
 */
OrdoResult database_load_tasks(Database *db, TaskList *list);

/**
 * @brief Carrega as tarefas da lixeira (removidas) do banco de dados.
 * @param[in] db Ponteiro para a estrutura `Database`.
 * @param[out] list Ponteiro para a `TaskList` onde as tarefas serão carregadas.
 * @return `ORDO_OK` em caso de sucesso.
 */
OrdoResult database_load_deleted_tasks(Database *db, TaskList *list);

/**
 * @brief Adiciona uma nova tarefa ao banco de dados.
 * @param[in] db Ponteiro para a estrutura `Database`.
 * @param[in] description A descrição da nova tarefa.
 * @param[out] new_task_id Ponteiro para armazenar o ID da tarefa recém-criada.
 * @return `ORDO_OK` em caso de sucesso.
 */
OrdoResult database_add_task(Database *db, const char *description,
                             int *new_task_id);

/**
 * @brief Move uma tarefa para a lixeira (soft delete).
 * @param[in] db Ponteiro para a estrutura `Database`.
 * @param[in] task_id O ID da tarefa a ser movida para a lixeira.
 * @return `ORDO_OK` em caso de sucesso.
 */
OrdoResult database_remove_task(Database *db, int task_id);

/**
 * @brief Restaura uma tarefa da lixeira.
 * @param[in] db Ponteiro para a estrutura `Database`.
 * @param[in] task_id O ID da tarefa a ser restaurada.
 * @return `ORDO_OK` em caso de sucesso.
 */
OrdoResult database_restore_task(Database *db, int task_id);

/**
 * @brief Deleta permanentemente uma tarefa do banco de dados.
 * @param[in] db Ponteiro para a estrutura `Database`.
 * @param[in] task_id O ID da tarefa a ser deletada permanentemente.
 * @return `ORDO_OK` em caso de sucesso.
 */
OrdoResult database_permanently_delete_task(Database *db, int task_id);

/**
 * @brief Alterna o estado de uma tarefa entre concluída e pendente.
 * @param[in] db Ponteiro para a estrutura `Database`.
 * @param[in] task_id O ID da tarefa a ser modificada.
 * @param[in] current_status O estado atual da tarefa (0 para pendente, 1 para concluída).
 * @return `ORDO_OK` em caso de sucesso.
 */
OrdoResult database_toggle_task_status(Database *db, int task_id,
                                       int current_status);

/**
 * @brief Atualiza a descrição de uma tarefa existente.
 * @param[in] db Ponteiro para a estrutura `Database`.
 * @param[in] task_id O ID da tarefa a ser atualizada.
 * @param[in] new_description A nova descrição para a tarefa.
 * @return `ORDO_OK` em caso de sucesso.
 */
OrdoResult database_update_task_description(Database *db, int task_id,
                                            const char *new_description);

#endif // DATABASE_H
