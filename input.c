#include "input.h"
#include "app.h"
#include "database.h"
#include "error.h"
#include "settings_ui.h"
#include "ui.h"
#include "utils.h"

// Função auxiliar para exibir mensagens de resultado na UI
static void handle_result(OrdoResult result, AppState *app) {
  switch (result) {
  case ORDO_OK:
    break;
  case ORDO_ERROR_EMPTY_DESCRIPTION:
    ui_show_message("ADD_EMPTY", true, &app->config);
    break;
  case ORDO_ERROR_TASK_NOT_FOUND:
    ui_show_message("TASK_NOT_FOUND", true, &app->config);
    break;
  case ORDO_ADD_SUCCESS:
    ui_show_message("ADD_SUCCESS", false, &app->config);
    break;
  case ORDO_REMOVE_SUCCESS:
    ui_show_message("REMOVE_SUCCESS", false, &app->config);
    break;
  case ORDO_EDIT_SUCCESS:
    ui_show_message("EDIT_SUCCESS", false, &app->config);
    break;
  case ORDO_TOGGLE_SUCCESS_DONE:
    ui_show_message("TOGGLE_SUCCESS_DONE", false, &app->config);
    break;
  case ORDO_TOGGLE_SUCCESS_REOPEN:
    ui_show_message("TOGGLE_SUCCESS_REOPEN", false, &app->config);
    break;
  case ORDO_TRASH_SUCCESS:
    ui_show_message("TRASH_SUCCESS", false, &app->config);
    break;
  case ORDO_RESTORE_SUCCESS:
    ui_show_message("RESTORE_SUCCESS", false, &app->config);
    break;
  case ORDO_PERM_DELETE_SUCCESS:
    ui_show_message("PERM_DELETE_SUCCESS", false, &app->config);
    break;
  case ORDO_UNDO_SUCCESS:
    ui_show_message("UNDO_SUCCESS", false, &app->config);
    break;
  case ORDO_UNDO_FAILURE:
    ui_show_message("UNDO_FAILURE", true, &app->config);
    break;
  case ORDO_REDO_SUCCESS:
    ui_show_message("REDO_SUCCESS", false, &app->config);
    break;
  case ORDO_REDO_FAILURE:
    ui_show_message("REDO_FAILURE", true, &app->config);
    break;
  default:
    ui_show_message("GENERIC_ERROR", true, &app->config);
    break;
  }
}

// Lida com a entrada do usuário na visão principal
static void handle_main_view_input(AppState *app, int choice) {
  OrdoResult result = ORDO_OK;
  int task_id = (app->task_list.count > 0)
                    ? app->task_list.tasks[app->current_selection].id
                    : -1;

  if (choice != 'u' && choice != 'y') {
    undo_manager_clear_redo(&app->undo_manager);
  }

  switch (choice) {
  case '1': { // Adicionar Tarefa
    auto_free_ptr char *new_desc = NULL;
    result = ui_add_task(&app->config, &new_desc);
    if (result == ORDO_OK) {
      int new_id = -1;
      result = database_add_task(&app->db, new_desc, &new_id);
      if (result == ORDO_OK) {
        undo_manager_push(&app->undo_manager, ACTION_ADD, new_id, NULL, NULL, 0);
        handle_result(ORDO_ADD_SUCCESS, app);
        app->refresh_tasks = true;
      } else {
        handle_result(result, app);
      }
    } else {
      handle_result(result, app);
    }
    break;
  }
  case '2': { // Mover para a Lixeira
    if (task_id != -1) {
      Tarefa *task = &app->task_list.tasks[app->current_selection];
      undo_manager_push(&app->undo_manager, ACTION_DELETE, task_id,
                        task->descricao, NULL, task->concluida);
      result = database_remove_task(&app->db, task_id);
      if (result == ORDO_OK) {
        handle_result(ORDO_TRASH_SUCCESS, app);
        app->refresh_tasks = true;
      } else {
        // A ação falhou, então removemos do histórico de undo
        app->undo_manager.undo_top--;
        handle_result(result, app);
      }
    }
    break;
  }
  case '3': { // Editar Tarefa
    if (task_id != -1) {
      auto_free_ptr char *new_desc = NULL;
      result = ui_edit_task(&app->config, &new_desc);
      if (result == ORDO_OK) {
        result = database_update_task_description(&app->db, task_id, new_desc);
        if (result == ORDO_OK) {
          undo_manager_push(&app->undo_manager, ACTION_EDIT, task_id,
                            app->task_list.tasks[app->current_selection].descricao,
                            new_desc, 0);
          handle_result(ORDO_EDIT_SUCCESS, app);
          app->refresh_tasks = true;
        } else {
          handle_result(result, app);
        }
      } else {
        handle_result(result, app);
      }
    }
    break;
  }
  case '4': { // Marcar como Concluída/Pendente
    if (task_id != -1) {
      Tarefa *task = &app->task_list.tasks[app->current_selection];
      undo_manager_push(&app->undo_manager, ACTION_TOGGLE, task_id, NULL, NULL,
                        task->concluida);
      result = database_toggle_task_status(&app->db, task_id, task->concluida);
      if (result == ORDO_OK) {
        app->refresh_tasks = true;
        OrdoResult msg =
            task->concluida ? ORDO_TOGGLE_SUCCESS_REOPEN : ORDO_TOGGLE_SUCCESS_DONE;
        handle_result(msg, app);
      } else {
        app->undo_manager.undo_top--;
        handle_result(result, app);
      }
    }
    break;
  }
  case '5':
    if (task_id != -1)
      ui_display_full_task(&app->task_list.tasks[app->current_selection],
                           &app->config);
    break;
  case '6':
    settings_ui_show(&app->config);
    app->refresh_tasks = true;
    break;
  case 'u': { // Desfazer
    if (undo_manager_perform_undo(&app->undo_manager, app)) {
      handle_result(ORDO_UNDO_SUCCESS, app);
      app->refresh_tasks = true;
    } else {
      handle_result(ORDO_UNDO_FAILURE, app);
    }
    break;
  }
  case 'y': { // Refazer
    if (undo_manager_perform_redo(&app->undo_manager, app)) {
      handle_result(ORDO_REDO_SUCCESS, app);
      app->refresh_tasks = true;
    } else {
      handle_result(ORDO_REDO_FAILURE, app);
    }
    break;
  }
  }
}

// Lida com a entrada do usuário na visão da lixeira
static void handle_trash_view_input(AppState *app, int choice) {
  OrdoResult result = ORDO_OK;
  if (app->task_list.count <= 0) return;

  int task_id = app->task_list.tasks[app->current_selection].id;

  switch (choice) {
  case 'r': // Restaurar
    result = database_restore_task(&app->db, task_id);
    if (result == ORDO_OK) {
      handle_result(ORDO_RESTORE_SUCCESS, app);
      app->refresh_tasks = true;
    } else {
      handle_result(result, app);
    }
    break;
  case 'd': // Deletar Permanentemente
    if (ui_confirm_action("CONFIRM_PERM_DELETE_PROMPT", "MENU_PERM_DELETE",
                          &app->config)) {
      result = database_permanently_delete_task(&app->db, task_id);
      if (result == ORDO_OK) {
        handle_result(ORDO_PERM_DELETE_SUCCESS, app);
        app->refresh_tasks = true;
      } else {
        handle_result(result, app);
      }
    }
    break;
  }
}

// --- Máscaras de bits para eventos de scroll do mouse ---
// Agrupa todos os eventos possíveis para scroll para cima (Botão 4)
#define SCROLL_UP_EVENTS                                                       
  (BUTTON4_PRESSED | BUTTON4_RELEASED | BUTTON4_CLICKED |                      
   BUTTON4_DOUBLE_CLICKED | BUTTON4_TRIPLE_CLICKED)

// Agrupa todos os eventos possíveis para scroll para baixo (Botão 5)
#define SCROLL_DOWN_EVENTS                                                     
  (BUTTON5_PRESSED | BUTTON5_RELEASED | BUTTON5_CLICKED |                      
   BUTTON5_DOUBLE_CLICKED | BUTTON5_TRIPLE_CLICKED)

// Função principal que processa toda a entrada do teclado e mouse
void input_handle(AppState *app, int key) {
  if (key == ERR) { // Timeout, nenhuma tecla pressionada
    return;
  }

  // Ações globais
  if (key == '7' || key == 'q') {
    if (app->current_view == VIEW_TRASH) {
      app->current_view = VIEW_MAIN;
      app->refresh_tasks = true;
    } else {
      app->should_exit = true;
    }
    return;
  }
  if (key == 't') {
    app->current_view = (app->current_view == VIEW_MAIN) ? VIEW_TRASH : VIEW_MAIN;
    app->refresh_tasks = true;
    return;
  }
  if (key == KEY_UP) {
    if (app->task_list.count > 0)
      app->current_selection = (app->current_selection - 1 + app->task_list.count) %
          app->task_list.count;
    return;
  }
  if (key == KEY_DOWN) {
    if (app->task_list.count > 0)
      app->current_selection = (app->current_selection + 1) % app->task_list.count;
    return;
  }
  if (key == KEY_RESIZE) {
    // O layout será redesenhado no próximo loop, não precisa de ação aqui
    return;
  }
  if (key == KEY_MOUSE) {
    MEVENT event;
    if (getmouse(&event) == OK) {
      extern WINDOW *content_win, *sidebar_win;
      if (wenclose(content_win, event.y, event.x)) {
        if (event.bstate & SCROLL_UP_EVENTS) { // Scroll up
          if (app->scroll_offset > 0)
            app->scroll_offset--;
        } else if (event.bstate & SCROLL_DOWN_EVENTS) { // Scroll down
          int height = getmaxy(content_win) - 2;
          if (app->scroll_offset + height < app->task_list.count)
            app->scroll_offset++;
        } else if (event.bstate & (BUTTON1_CLICKED | BUTTON1_DOUBLE_CLICKED)) {
          int clicked_y = event.y - getbegy(content_win) - 1;
          if (clicked_y >= 0) {
            int clicked_task = clicked_y + app->scroll_offset;
            if (clicked_task < app->task_list.count) {
              app->current_selection = clicked_task;
              if (event.bstate & BUTTON1_DOUBLE_CLICKED) {
                ui_display_full_task(
                    &app->task_list.tasks[app->current_selection],
                    &app->config);
              }
            }
          }
        }
      } else if (wenclose(sidebar_win, event.y, event.x)) {
        key = ui_handle_sidebar_click(event, app->current_view);
      }
    }
  }

  // Ações específicas da visão
  if (app->current_view == VIEW_MAIN) {
    handle_main_view_input(app, key);
  } else { // VIEW_TRASH
    handle_trash_view_input(app, key);
  }
}

