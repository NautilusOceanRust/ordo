/**
 * @file app.h
 * @brief Declarações centrais para o estado e ciclo de vida da aplicação Ordo.
 *
 * Este arquivo define a estrutura principal `AppState` que encapsula todo o estado
 * da aplicação, bem como as funções para inicializar e destruir a aplicação.
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
 * @brief Enum para controlar a visão (tela) atualmente exibida na UI.
 */
typedef enum {
  VIEW_MAIN,  /**< A visão principal, mostrando tarefas ativas. */
  VIEW_TRASH  /**< A visão da lixeira, mostrando tarefas removidas. */
} AppView;

/**
 * @struct AppState
 * @brief Estrutura central que contém todo o estado da aplicação.
 *
 * Esta estrutura agrega todos os componentes principais da aplicação, como
 * o manipulador de banco de dados, configurações, listas de tarefas e o estado da UI.
 */
typedef struct AppState {
  Database db;                  /**< Manipulador do banco de dados. */
  AppConfig config;             /**< Configurações da aplicação (tema, idioma). */
  TaskList task_list;           /**< A lista de tarefas atualmente exibida. */
  UndoManager undo_manager;     /**< Gerenciador para ações de desfazer/refazer. */

  AppView current_view;         /**< A visão (tela) atual. */
  int current_selection;        /**< O índice da tarefa atualmente selecionada. */
  int scroll_offset;            /**< O deslocamento da rolagem na lista de tarefas. */

  bool should_exit;             /**< Flag para sinalizar o término da aplicação. */
  bool refresh_tasks;           /**< Flag para sinalizar a necessidade de recarregar as tarefas do DB. */
} AppState;

// --- Funções de Ciclo de Vida ---

/**
 * @brief Inicializa a aplicação.
 *
 * Configura o locale, inicializa o banco de dados, a UI, o gerenciador de
 * internacionalização e carrega as configurações.
 *
 * @param[out] app Ponteiro para a estrutura AppState a ser inicializada.
 * @return `true` se a inicialização for bem-sucedida, `false` caso contrário.
 */
bool app_init(AppState *app);

/**
 * @brief Libera todos os recursos alocados pela aplicação.
 *
 * Fecha a UI, o banco de dados e libera a memória usada pela lista de tarefas
 * e outros componentes.
 *
 * @param[in] app Ponteiro para a estrutura AppState a ser destruída.
 */
void app_destroy(AppState *app);

#endif // APP_H
