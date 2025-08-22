#include "app.h"
#include "i18n.h"
#include "theme.h"
#include "ui.h"
#include "utils.h"
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#include <windows.h>
#endif

bool app_init(AppState *app) {
  setlocale(LC_ALL, "");
#ifdef _WIN32
  SetConsoleOutputCP(CP_UTF8);
  SetConsoleCP(CP_UTF8);
#endif

  *app = (AppState){0}; // Zero out the structure

  config_load(&app->config);
  theme_load_external();

  char db_path[MAX_PATH];
  if (obterCaminhoBancoDeDados(db_path, sizeof(db_path))) {
    return false;
  }

  sqlite3 *db_conn;
  if (sqlite3_open(db_path, &db_conn)) {
    fprintf(stderr, "Error opening database '%s': %s\n", db_path,
            sqlite3_errmsg(db_conn));
    return false;
  }

  if (database_init(db_conn, &app->db) != ORDO_OK) {
    sqlite3_close(db_conn);
    return false;
  }

  i18n_init(app->config.lang);
  ui_init(&app->config);
  timeout(1000); // Para o getch() não bloquear indefinidamente

  task_list_init(&app->task_list);
  undo_manager_init(&app->undo_manager);

  app->current_view = VIEW_MAIN;
  app->refresh_tasks = true;
  app->should_exit = false;

  return true;
}

void app_destroy(AppState *app) {
  task_list_free(&app->task_list);
  ui_destroy();
  database_close(&app->db);
  i18n_destroy();
  theme_free_external();
}
