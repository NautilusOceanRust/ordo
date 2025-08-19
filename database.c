#include "database.h"
#include "task_list.h"
#include "utils.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Helper function to prepare a statement
static OrdoResult prepare_stmt(sqlite3 *db, sqlite3_stmt **stmt,
                               const char *sql) {
  if (sqlite3_prepare_v2(db, sql, -1, stmt, NULL) != SQLITE_OK) {
    fprintf(stderr, "Error preparing statement \'%s\': %s\n", sql,
            sqlite3_errmsg(db));
    return ORDO_ERROR_DATABASE;
  }
  return ORDO_OK;
}

static OrdoResult migrate_database_schema(Database *db) {
  sqlite3_stmt *stmt;
  const char *sql = "PRAGMA table_info(tarefas);";
  bool column_exists = false;

  if (sqlite3_prepare_v2(db->db, sql, -1, &stmt, NULL) != SQLITE_OK) {
    fprintf(stderr, "Error preparing PRAGMA statement: %s\n",
            sqlite3_errmsg(db->db));
    return ORDO_ERROR_DATABASE;
  }

  while (sqlite3_step(stmt) == SQLITE_ROW) {
    const unsigned char *col_name = sqlite3_column_text(stmt, 1);
    if (col_name && strcmp((const char *)col_name, "is_deleted") == 0) {
      column_exists = true;
      break;
    }
  }
  sqlite3_finalize(stmt);

  if (!column_exists) {
    char *errMsg = 0;
    const char *sql_alter =
        "ALTER TABLE tarefas ADD COLUMN is_deleted INTEGER NOT NULL DEFAULT 0;";
    if (sqlite3_exec(db->db, sql_alter, 0, 0, &errMsg) != SQLITE_OK) {
      fprintf(stderr, "Error adding column to table: %s\n", errMsg);
      sqlite3_free(errMsg);
      return ORDO_ERROR_DATABASE;
    }
  }
  return ORDO_OK;
}

OrdoResult database_init(sqlite3 *db_conn, Database *db) {
  *db = (Database){0};
  db->db = db_conn;

  char *errMsg = 0;
  const char *sql_create =
      "CREATE TABLE IF NOT EXISTS tarefas (id INTEGER PRIMARY KEY, descricao "
      "TEXT NOT NULL, concluida INTEGER NOT NULL DEFAULT 0, is_deleted INTEGER "
      "NOT NULL DEFAULT 0);";
  if (sqlite3_exec(db->db, sql_create, 0, 0, &errMsg) != SQLITE_OK) {
    fprintf(stderr, "Error creating table: %s\n", errMsg);
    sqlite3_free(errMsg);
    sqlite3_close(db->db);
    return ORDO_ERROR_DATABASE;
  }

  if (migrate_database_schema(db) != ORDO_OK) {
    sqlite3_close(db->db);
    return ORDO_ERROR_DATABASE;
  }

  // Prepare all statements. If any fail, clean up and return an error.
  if (prepare_stmt(db->db, &db->add_task_stmt,
                   "INSERT INTO tarefas (descricao, concluida) VALUES (?, 0);") != ORDO_OK ||
      prepare_stmt(db->db, &db->remove_task_stmt,
                   "UPDATE tarefas SET is_deleted = 1 WHERE id = ?;") != ORDO_OK ||
      prepare_stmt(db->db, &db->toggle_task_status_stmt,
                   "UPDATE tarefas SET concluida = ? WHERE id = ?;") != ORDO_OK ||
      prepare_stmt(db->db, &db->update_task_description_stmt,
                   "UPDATE tarefas SET descricao = ? WHERE id = ?;") != ORDO_OK ||
      prepare_stmt(db->db, &db->load_tasks_stmt,
                   "SELECT id, descricao, concluida FROM tarefas "
                   "WHERE is_deleted = 0 ORDER BY id;") != ORDO_OK ||
      prepare_stmt(db->db, &db->restore_task_stmt,
                   "UPDATE tarefas SET is_deleted = 0 WHERE id = ?;") != ORDO_OK ||
      prepare_stmt(db->db, &db->perm_delete_task_stmt,
                   "DELETE FROM tarefas WHERE id = ?;") != ORDO_OK ||
      prepare_stmt(db->db, &db->load_deleted_tasks_stmt,
                   "SELECT id, descricao, concluida FROM tarefas "
                   "WHERE is_deleted = 1 ORDER BY id;") != ORDO_OK) {
    database_close(db); // Ensures cleanup in case of failure
    return ORDO_ERROR_DATABASE;
  }

  return ORDO_OK;
}

void database_close(Database *db) {
  if (!db)
    return;
  sqlite3_finalize(db->add_task_stmt);
  sqlite3_finalize(db->remove_task_stmt);
  sqlite3_finalize(db->toggle_task_status_stmt);
  sqlite3_finalize(db->update_task_description_stmt);
  sqlite3_finalize(db->load_tasks_stmt);
  sqlite3_finalize(db->restore_task_stmt);
  sqlite3_finalize(db->perm_delete_task_stmt);
  sqlite3_finalize(db->load_deleted_tasks_stmt);

  if (db->db) {
    sqlite3_close(db->db);
  }
}

static OrdoResult load_tasks_generic(sqlite3_stmt *select_stmt,
                                     TaskList *list) {
  task_list_clear(list); // Clear the list before loading new tasks

  while (sqlite3_step(select_stmt) == SQLITE_ROW) {
    Tarefa task;
    task.id = sqlite3_column_int(select_stmt, 0);
    const unsigned char *desc = sqlite3_column_text(select_stmt, 1);
    // Safely copy the description to the task structure.
    if (desc) {
      snprintf(task.descricao, MAX_DESCRICAO, "%s", (const char *)desc);
    } else {
      task.descricao[0] = '\0';
    }
    task.concluida = sqlite3_column_int(select_stmt, 2);

    if (!task_list_add(list, &task)) {
      sqlite3_reset(select_stmt);
      return ORDO_ERROR_GENERIC; // Failed to add to the list
    }
  }

  sqlite3_reset(select_stmt);
  return ORDO_OK;
}

OrdoResult database_load_tasks(Database *db, TaskList *list) {
  return load_tasks_generic(db->load_tasks_stmt, list);
}

OrdoResult database_load_deleted_tasks(Database *db, TaskList *list) {
  return load_tasks_generic(db->load_deleted_tasks_stmt, list);
}

OrdoResult database_add_task(Database *db, const char *description,
                             int *new_task_id) {
  sqlite3_stmt *stmt = db->add_task_stmt;
  sqlite3_bind_text(stmt, 1, description, -1, SQLITE_TRANSIENT);

  if (sqlite3_step(stmt) != SQLITE_DONE) {
    sqlite3_reset(stmt);
    return ORDO_ERROR_DATABASE;
  }

  *new_task_id = (int)sqlite3_last_insert_rowid(db->db);
  sqlite3_reset(stmt);
  return ORDO_OK;
}

static OrdoResult execute_simple_update(Database *db, sqlite3_stmt *stmt,
                                        int task_id) {
  sqlite3_bind_int(stmt, 1, task_id);

  if (sqlite3_step(stmt) != SQLITE_DONE) {
    sqlite3_reset(stmt);
    return ORDO_ERROR_DATABASE;
  }

  if (sqlite3_changes(db->db) == 0) {
    sqlite3_reset(stmt);
    return ORDO_ERROR_TASK_NOT_FOUND;
  }

  sqlite3_reset(stmt);
  return ORDO_OK;
}

OrdoResult database_remove_task(Database *db, int task_id) {
  return execute_simple_update(db, db->remove_task_stmt, task_id);
}

OrdoResult database_restore_task(Database *db, int task_id) {
  return execute_simple_update(db, db->restore_task_stmt, task_id);
}

OrdoResult database_permanently_delete_task(Database *db, int task_id) {
  return execute_simple_update(db, db->perm_delete_task_stmt, task_id);
}

OrdoResult database_toggle_task_status(Database *db, int task_id,
                                       int current_status) {
  sqlite3_stmt *stmt = db->toggle_task_status_stmt;
  int new_status = !current_status;

  sqlite3_bind_int(stmt, 1, new_status);
  sqlite3_bind_int(stmt, 2, task_id);

  if (sqlite3_step(stmt) != SQLITE_DONE) {
    sqlite3_reset(stmt);
    return ORDO_ERROR_DATABASE;
  }

  if (sqlite3_changes(db->db) == 0) {
    sqlite3_reset(stmt);
    return ORDO_ERROR_TASK_NOT_FOUND;
  }

  sqlite3_reset(stmt);
  return ORDO_OK;
}

OrdoResult database_update_task_description(Database *db, int task_id,
                                            const char *new_description) {
  if (new_description == NULL || strlen(new_description) == 0) {
    return ORDO_ERROR_EMPTY_DESCRIPTION;
  }

  sqlite3_stmt *stmt = db->update_task_description_stmt;
  sqlite3_bind_text(stmt, 1, new_description, -1, SQLITE_TRANSIENT);
  sqlite3_bind_int(stmt, 2, task_id);

  if (sqlite3_step(stmt) != SQLITE_DONE) {
    sqlite3_reset(stmt);
    return ORDO_ERROR_DATABASE;
  }

  if (sqlite3_changes(db->db) == 0) {
    sqlite3_reset(stmt);
    return ORDO_ERROR_TASK_NOT_FOUND;
  }

  sqlite3_reset(stmt);
  return ORDO_OK;
}
