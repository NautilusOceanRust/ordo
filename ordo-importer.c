#include <cjson/cJSON.h>
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Definição da estrutura da tarefa, para referência.
// O programa não usa esta struct diretamente, mas interage
// com o banco de dados que armazena esses campos.
/*
#define MAX_DESCRICAO 200
typedef struct {
  int id;
  char descricao[MAX_DESCRICAO];
  int concluida;
} Tarefa;
*/

// Função para processar um arquivo de texto
int process_txt_file(FILE *fp, sqlite3 *db, sqlite3_stmt *stmt) {
  char buffer[202]; // +2 para \n e \0
  int contador = 0;
  while (fgets(buffer, sizeof(buffer), fp)) {
    // Remove a quebra de linha de forma idiomática e segura
    buffer[strcspn(buffer, "\n")] = 0;

    // Ignorar linhas em branco
    if (strlen(buffer) == 0) {
      continue;
    }

    // Vincular a descrição da tarefa à statement.
    // SQLITE_TRANSIENT faz uma cópia do buffer, o que é seguro dentro de um
    // loop.
    if (sqlite3_bind_text(stmt, 1, buffer, -1, SQLITE_TRANSIENT) != SQLITE_OK) {
      fprintf(stderr, "Erro ao vincular texto: %s\n", sqlite3_errmsg(db));
      continue;
    }

    // Executar a statement
    if (sqlite3_step(stmt) != SQLITE_DONE) {
      fprintf(stderr, "Erro ao inserir tarefa: %s\n", sqlite3_errmsg(db));
    } else {
      contador++;
    }

    // Limpa os valores vinculados da statement preparada.
    sqlite3_clear_bindings(stmt);

    // Resetar a statement para a próxima iteração
    sqlite3_reset(stmt);
  }
  return contador;
}

#define JSON_CHUNK_SIZE 4096

// Função para processar um arquivo JSON de forma mais eficiente em termos de memória
int process_json_file(FILE *fp, sqlite3 *db, sqlite3_stmt *stmt) {
    char *buffer = NULL;
    char *temp_buffer;
    size_t buffer_size = 0;
    size_t bytes_read;
    size_t total_bytes = 0;

    // Lê o arquivo em pedaços (chunks)
    while ((bytes_read = fread(buffer + total_bytes, 1, JSON_CHUNK_SIZE, fp)) > 0) {
        total_bytes += bytes_read;
        buffer_size += JSON_CHUNK_SIZE;
        temp_buffer = realloc(buffer, buffer_size);
        if (temp_buffer == NULL) {
            fprintf(stderr, "Erro: Falha ao realocar memória para o buffer do arquivo.\n");
            free(buffer);
            return 0;
        }
        buffer = temp_buffer;
    }

    // Adiciona o terminador nulo
    temp_buffer = realloc(buffer, total_bytes + 1);
    if (temp_buffer == NULL) {
        fprintf(stderr, "Erro: Falha ao realocar memória para o terminador nulo.\n");
        free(buffer);
        return 0;
    }
    buffer = temp_buffer;
    buffer[total_bytes] = '\0';

    cJSON *json = cJSON_Parse(buffer);
    free(buffer);

    if (json == NULL) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) {
            fprintf(stderr, "Erro ao processar o arquivo JSON: %s\n", error_ptr);
        }
        return 0;
    }

    cJSON *tasks = cJSON_GetObjectItemCaseSensitive(json, "tasks");
    int contador = 0;
    cJSON *task;
    cJSON_ArrayForEach(task, tasks) {
        if (cJSON_IsString(task) && (task->valuestring != NULL)) {
            if (sqlite3_bind_text(stmt, 1, task->valuestring, -1, SQLITE_TRANSIENT) != SQLITE_OK) {
                fprintf(stderr, "Erro ao vincular texto: %s\n", sqlite3_errmsg(db));
                continue;
            }

            if (sqlite3_step(stmt) != SQLITE_DONE) {
                fprintf(stderr, "Erro ao inserir tarefa: %s\n", sqlite3_errmsg(db));
            } else {
                contador++;
            }

            sqlite3_clear_bindings(stmt);
            sqlite3_reset(stmt);
        }
    }

    cJSON_Delete(json);
    return contador;
}


// Função para detectar o tipo de arquivo
const char *get_file_type(const char *filename) {
  const char *dot = strrchr(filename, '.');
  if (!dot || dot == filename)
    return "";
  return dot + 1;
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    fprintf(stderr, "Uso: %s <arquivo_entrada> <arquivo_db_saida>\n", argv[0]);
    return EXIT_FAILURE;
  }

  const char *arquivo_entrada = argv[1];
  const char *arquivo_db = argv[2];
  const char *file_type = get_file_type(arquivo_entrada);

  // 1. Abrir o arquivo de entrada
  FILE *fp = fopen(arquivo_entrada, "r");
  if (fp == NULL) {
    fprintf(stderr, "Erro: Não foi possível abrir o arquivo de entrada '%s'.\n",
            arquivo_entrada);
    return EXIT_FAILURE;
  }

  // 2. Abrir/Criar o banco de dados SQLite
  sqlite3 *db;
  if (sqlite3_open(arquivo_db, &db)) {
    fprintf(stderr, "Erro: Não foi possível abrir o banco de dados: %s\n",
            sqlite3_errmsg(db));
    fclose(fp);
    return EXIT_FAILURE;
  }

  // 3. Criar a tabela de tarefas se ela não existir
  char *err_msg = 0;
  const char *sql_create = "CREATE TABLE IF NOT EXISTS tarefas ("                           "id INTEGER PRIMARY KEY AUTOINCREMENT, "                           "descricao TEXT NOT NULL, "                           "concluida INTEGER NOT NULL DEFAULT 0);";

  if (sqlite3_exec(db, sql_create, 0, 0, &err_msg) != SQLITE_OK) {
    fprintf(stderr, "Erro SQL ao criar tabela: %s\n", err_msg);
    sqlite3_free(err_msg);
    sqlite3_close(db);
    fclose(fp);
    return EXIT_FAILURE;
  }

  // 4. Preparar a statement de inserção
  sqlite3_stmt *stmt;
  const char *sql_insert = "INSERT INTO tarefas (descricao) VALUES (?);";
  if (sqlite3_prepare_v2(db, sql_insert, -1, &stmt, NULL) != SQLITE_OK) {
    fprintf(stderr, "Erro ao preparar statement de inserção: %s\n",
            sqlite3_errmsg(db));
    sqlite3_close(db);
    fclose(fp);
    return EXIT_FAILURE;
  }

  printf("Iniciando a conversão de '%s' para '%s'..\n", arquivo_entrada,
         arquivo_db);

  int contador = 0;
  if (strcmp(file_type, "txt") == 0) {
    contador = process_txt_file(fp, db, stmt);
  } else if (strcmp(file_type, "json") == 0) {
    contador = process_json_file(fp, db, stmt);
  } else {
    fprintf(stderr, "Erro: Tipo de arquivo não suportado: %s\n", file_type);
  }

  printf("%d tarefas foram importadas com sucesso!\n", contador);

  // 6. Limpeza
  sqlite3_finalize(stmt);
  sqlite3_close(db);
  fclose(fp);

  return EXIT_SUCCESS;
}
