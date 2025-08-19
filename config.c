#include "config.h"
#include "platform_utils.h" // Inclui o novo módulo
#include "theme.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LEN 256

// (O resto das definições de cores padrão permanece o mesmo)
#define DEFAULT_COLOR_HEADER_PAIR 1
#define DEFAULT_COLOR_HEADER_FG COLOR_YELLOW
#define DEFAULT_COLOR_HEADER_BG COLOR_BLACK
#define DEFAULT_COLOR_DONE_PAIR 2
#define DEFAULT_COLOR_DONE_FG COLOR_GREEN
#define DEFAULT_COLOR_DONE_BG COLOR_BLACK
#define DEFAULT_COLOR_PENDING_PAIR 3
#define DEFAULT_COLOR_PENDING_FG COLOR_WHITE
#define DEFAULT_COLOR_PENDING_BG COLOR_BLACK
#define DEFAULT_COLOR_ERROR_PAIR 4
#define DEFAULT_COLOR_ERROR_FG COLOR_RED
#define DEFAULT_COLOR_ERROR_BG COLOR_BLACK
#define DEFAULT_COLOR_SUCCESS_PAIR 5
#define DEFAULT_COLOR_SUCCESS_FG COLOR_GREEN
#define DEFAULT_COLOR_SUCCESS_BG COLOR_BLACK
#define DEFAULT_COLOR_TASK_DONE_PAIR 6
#define DEFAULT_COLOR_TASK_DONE_FG COLOR_WHITE
#define DEFAULT_COLOR_TASK_DONE_BG COLOR_BLACK
#define DEFAULT_COLOR_TASK_PENDING_PAIR 7
#define DEFAULT_COLOR_TASK_PENDING_FG COLOR_WHITE
#define DEFAULT_COLOR_TASK_PENDING_BG COLOR_BLACK

// Funcao para definir valores padrao
void set_default_config(AppConfig *config) {
  config->color_pair_header = DEFAULT_COLOR_HEADER_PAIR;
  config->color_pair_status_done = DEFAULT_COLOR_DONE_PAIR;
  config->color_pair_status_pending = DEFAULT_COLOR_PENDING_PAIR;
  config->color_pair_error = DEFAULT_COLOR_ERROR_PAIR;
  config->color_pair_success = DEFAULT_COLOR_SUCCESS_PAIR;
  config->color_pair_task_done = DEFAULT_COLOR_TASK_DONE_PAIR;
  config->color_pair_task_pending = DEFAULT_COLOR_TASK_PENDING_PAIR;
  snprintf(config->lang, sizeof(config->lang), "en");
  snprintf(config->theme_name, sizeof(config->theme_name), "Ordo Classic");
}

// Funcao para criar o arquivo de configuracao com valores padrao
static void create_default_config_file(const char *path) {
  FILE *file = fopen(path, "w");
  if (!file)
    return;

  fprintf(file, "# Ordo Configuration File\n");
  fprintf(file, "# Default language (ISO 639-1 code)\n");
  fprintf(file, "lang = en\n\n");
  fprintf(file, "# Theme name\n");
  fprintf(file, "theme = Ordo Classic\n");

  fclose(file);
}

void config_load(AppConfig *config) {
  set_default_config(config);

  char *config_dir = platform_get_config_dir();
  if (!config_dir)
    return; // Falha ao obter o diretório de configuração

  platform_create_dir_recursive(config_dir);

  char *config_file_path = path_join(config_dir, "config.ini");
  if (!config_file_path) {
    free(config_dir);
    return;
  }

  FILE *file = fopen(config_file_path, "r");
  if (!file) {
    create_default_config_file(config_file_path);
    file = fopen(config_file_path, "r");
    if (!file) {
      free(config_dir);
      free(config_file_path);
      return;
    }
  }
  free(config_file_path);

  char line[MAX_LINE_LEN];

  while (fgets(line, sizeof(line), file)) {
    // Remove newline characters from the end of the line
    line[strcspn(line, "\r\n")] = 0;

    if (line[0] == '#' || line[0] == '\0')
      continue;

    char *key = line;
    char *value = strchr(line, '=');
    if (value) {
      *value = '\0';
      value++;

      char *trimmed_key = trim(key);
      char *trimmed_value = trim(value);

      if (trimmed_key && trimmed_value) {
        if (strcmp(trimmed_key, "lang") == 0) {
          snprintf(config->lang, sizeof(config->lang), "%s", trimmed_value);
        } else if (strcmp(trimmed_key, "theme") == 0) {
          snprintf(config->theme_name, sizeof(config->theme_name), "%s", trimmed_value);
        }
      }
      free(trimmed_key);
      free(trimmed_value);
    }
  }
  fclose(file);
  free(config_dir);
}

void config_save(const AppConfig *config) {
  char *config_dir = platform_get_config_dir();
  if (!config_dir)
    return;

  platform_create_dir_recursive(config_dir);

  char *config_file_path = path_join(config_dir, "config.ini");
  if (!config_file_path) {
    free(config_dir);
    return;
  }

  FILE *file = fopen(config_file_path, "w");
  if (!file) {
    free(config_dir);
    free(config_file_path);
    return;
  }
  free(config_file_path);

  fprintf(file, "# Ordo Configuration File\n");
  fprintf(file, "lang = %s\n", config->lang);
  fprintf(file, "theme = %s\n", config->theme_name);

  fclose(file);
  free(config_dir);
}

void config_init_color_pairs(const AppConfig *config) {
  const OrdoTheme *theme = theme_get_by_name(config->theme_name);
  if (theme) {
    init_pair(config->color_pair_header, theme->header_fg, theme->header_bg);
    init_pair(config->color_pair_status_done, theme->done_fg, theme->done_bg);
    init_pair(config->color_pair_status_pending, theme->pending_fg,
              theme->pending_bg);
    init_pair(config->color_pair_error, theme->error_fg, theme->error_bg);
    init_pair(config->color_pair_success, theme->success_fg, theme->success_bg);
    init_pair(config->color_pair_task_done, theme->task_done_fg,
              theme->task_done_bg);
    init_pair(config->color_pair_task_pending, theme->task_pending_fg,
              theme->task_pending_bg);
  } else {
    // Fallback para cores padrão
    init_pair(DEFAULT_COLOR_HEADER_PAIR, DEFAULT_COLOR_HEADER_FG,
              DEFAULT_COLOR_HEADER_BG);
    init_pair(DEFAULT_COLOR_DONE_PAIR, DEFAULT_COLOR_DONE_FG,
              DEFAULT_COLOR_DONE_BG);
    init_pair(DEFAULT_COLOR_PENDING_PAIR, DEFAULT_COLOR_PENDING_FG,
              DEFAULT_COLOR_PENDING_BG);
    init_pair(DEFAULT_COLOR_ERROR_PAIR, DEFAULT_COLOR_ERROR_FG,
              DEFAULT_COLOR_ERROR_BG);
    init_pair(DEFAULT_COLOR_SUCCESS_PAIR, DEFAULT_COLOR_SUCCESS_FG,
              DEFAULT_COLOR_SUCCESS_BG);
    init_pair(DEFAULT_COLOR_TASK_DONE_PAIR, DEFAULT_COLOR_TASK_DONE_FG,
              DEFAULT_COLOR_TASK_DONE_BG);
    init_pair(DEFAULT_COLOR_TASK_PENDING_PAIR, DEFAULT_COLOR_TASK_PENDING_FG,
              DEFAULT_COLOR_TASK_PENDING_BG);
  }
}
