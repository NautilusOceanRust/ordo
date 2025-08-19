#define _POSIX_C_SOURCE 200809L
#include "theme.h"
#include "platform_utils.h" // Inclui o novo módulo
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

// Estrutura para mapear nomes de cores para valores ncurses
typedef struct {
  const char *name;
  short value;
} ColorMap;

// Mapa de nomes de cores para valores ncurses
static const ColorMap color_map[] = {
    {"COLOR_BLACK", COLOR_BLACK}, {"COLOR_RED", COLOR_RED},
    {"COLOR_GREEN", COLOR_GREEN}, {"COLOR_YELLOW", COLOR_YELLOW},
    {"COLOR_BLUE", COLOR_BLUE},   {"COLOR_MAGENTA", COLOR_MAGENTA},
    {"COLOR_CYAN", COLOR_CYAN},   {"COLOR_WHITE", COLOR_WHITE},
};
static const int num_colors = sizeof(color_map) / sizeof(ColorMap);

// Funcao para converter nome de cor em valor ncurses
static short color_name_to_value(const char *name) {
  for (int i = 0; i < num_colors; i++) {
    if (strcasecmp(color_map[i].name, name) == 0) {
      return color_map[i].value;
    }
  }
  return -1; // Cor nao encontrada
}

// Array com a definição de 40 temas de cores populares.
const OrdoTheme ordo_themes[] = {
    {"Ordo Classic", COLOR_YELLOW, COLOR_BLACK, COLOR_GREEN, COLOR_BLACK,
     COLOR_WHITE, COLOR_BLACK, COLOR_RED, COLOR_BLACK, COLOR_GREEN, COLOR_BLACK,
     COLOR_BLUE, COLOR_BLACK, COLOR_WHITE,
     COLOR_BLACK},
    {"Dracula", COLOR_CYAN, COLOR_BLACK, COLOR_GREEN, COLOR_BLACK, COLOR_WHITE,
     COLOR_BLACK, COLOR_MAGENTA, COLOR_BLACK, COLOR_GREEN, COLOR_BLACK,
     COLOR_CYAN, COLOR_BLACK, COLOR_WHITE,
     COLOR_BLACK},
    {"Solarized Light", COLOR_BLUE, COLOR_WHITE, COLOR_GREEN, COLOR_WHITE,
     COLOR_BLACK, COLOR_WHITE, COLOR_RED, COLOR_WHITE, COLOR_GREEN, COLOR_WHITE,
     COLOR_BLACK, COLOR_WHITE, COLOR_BLACK,
     COLOR_WHITE},
    {"Solarized Dark", COLOR_BLUE, COLOR_BLACK, COLOR_GREEN, COLOR_BLACK,
     COLOR_WHITE, COLOR_BLACK, COLOR_RED, COLOR_BLACK, COLOR_GREEN, COLOR_BLACK,
     COLOR_WHITE, COLOR_BLACK, COLOR_WHITE,
     COLOR_BLACK},
    {"Nord", COLOR_CYAN, COLOR_BLUE, COLOR_GREEN, COLOR_BLUE, COLOR_WHITE,
     COLOR_BLUE, COLOR_RED, COLOR_BLUE, COLOR_GREEN, COLOR_BLUE, COLOR_WHITE,
     COLOR_BLUE, COLOR_WHITE, COLOR_BLUE},
    {"Gruvbox Dark", COLOR_YELLOW, COLOR_BLACK, COLOR_GREEN, COLOR_BLACK,
     COLOR_WHITE, COLOR_BLACK, COLOR_RED, COLOR_BLACK, COLOR_GREEN, COLOR_BLACK,
     COLOR_WHITE, COLOR_BLACK, COLOR_WHITE,
     COLOR_BLACK},
    {"Monokai", COLOR_CYAN, COLOR_BLACK, COLOR_GREEN, COLOR_BLACK, COLOR_WHITE,
     COLOR_BLACK, COLOR_MAGENTA, COLOR_BLACK, COLOR_YELLOW, COLOR_BLACK,
     COLOR_WHITE, COLOR_BLACK, COLOR_WHITE,
     COLOR_BLACK},
    {"Tomorrow Night", COLOR_CYAN, COLOR_BLACK, COLOR_GREEN, COLOR_BLACK,
     COLOR_WHITE, COLOR_BLACK, COLOR_RED, COLOR_BLACK, COLOR_GREEN, COLOR_BLACK,
     COLOR_WHITE, COLOR_BLACK, COLOR_WHITE,
     COLOR_BLACK},
    {"Oceanic Next", COLOR_CYAN, COLOR_BLUE, COLOR_GREEN, COLOR_BLUE,
     COLOR_WHITE, COLOR_BLUE, COLOR_MAGENTA, COLOR_BLUE, COLOR_YELLOW,
     COLOR_BLUE, COLOR_WHITE, COLOR_BLUE, COLOR_WHITE,
     COLOR_BLUE},
    {"One Dark", COLOR_CYAN, COLOR_BLACK, COLOR_GREEN, COLOR_BLACK, COLOR_WHITE,
     COLOR_BLACK, COLOR_RED, COLOR_BLACK, COLOR_YELLOW, COLOR_BLACK,
     COLOR_WHITE, COLOR_BLACK, COLOR_WHITE,
     COLOR_BLACK},
    {"Material", COLOR_BLUE, COLOR_BLACK, COLOR_GREEN, COLOR_BLACK, COLOR_WHITE,
     COLOR_BLACK, COLOR_RED, COLOR_BLACK, COLOR_GREEN, COLOR_BLACK, COLOR_WHITE,
     COLOR_BLACK, COLOR_WHITE, COLOR_BLACK},
    {"Cobalt", COLOR_CYAN, COLOR_BLUE, COLOR_YELLOW, COLOR_BLUE, COLOR_WHITE,
     COLOR_BLUE, COLOR_RED, COLOR_BLUE, COLOR_YELLOW, COLOR_BLUE, COLOR_WHITE,
     COLOR_BLUE, COLOR_WHITE, COLOR_BLUE},
    {"Synthwave '84", COLOR_MAGENTA, COLOR_BLUE, COLOR_CYAN, COLOR_BLUE,
     COLOR_YELLOW, COLOR_BLUE, COLOR_RED, COLOR_BLUE, COLOR_CYAN, COLOR_BLUE,
     COLOR_YELLOW, COLOR_BLUE, COLOR_YELLOW,
     COLOR_BLUE},
    {"Night Owl", COLOR_YELLOW, COLOR_BLUE, COLOR_GREEN, COLOR_BLUE,
     COLOR_WHITE, COLOR_BLUE, COLOR_RED, COLOR_BLUE, COLOR_GREEN, COLOR_BLUE,
     COLOR_WHITE, COLOR_BLUE, COLOR_WHITE,
     COLOR_BLUE},
    {"Ayu Dark", COLOR_YELLOW, COLOR_BLACK, COLOR_GREEN, COLOR_BLACK,
     COLOR_CYAN, COLOR_BLACK, COLOR_RED, COLOR_BLACK, COLOR_GREEN, COLOR_BLACK,
     COLOR_CYAN, COLOR_BLACK, COLOR_CYAN,
     COLOR_BLACK},
    {"Panda", COLOR_CYAN, COLOR_BLUE, COLOR_GREEN, COLOR_BLUE, COLOR_WHITE,
     COLOR_BLUE, COLOR_RED, COLOR_BLUE, COLOR_GREEN, COLOR_BLUE, COLOR_WHITE,
     COLOR_BLUE, COLOR_WHITE, COLOR_BLUE},
    {"VS Code Dark+", COLOR_BLUE, COLOR_BLACK, COLOR_GREEN, COLOR_BLACK,
     COLOR_WHITE, COLOR_BLACK, COLOR_RED, COLOR_BLACK, COLOR_GREEN, COLOR_BLACK,
     COLOR_WHITE, COLOR_BLACK, COLOR_WHITE,
     COLOR_BLACK},
    {"GitHub Dark", COLOR_BLUE, COLOR_BLACK, COLOR_GREEN, COLOR_BLACK,
     COLOR_WHITE, COLOR_BLACK, COLOR_RED, COLOR_BLACK, COLOR_GREEN, COLOR_BLACK,
     COLOR_WHITE, COLOR_BLACK, COLOR_WHITE,
     COLOR_BLACK},
    {"Matrix", COLOR_GREEN, COLOR_BLACK, COLOR_GREEN, COLOR_BLACK, COLOR_GREEN,
     COLOR_BLACK, COLOR_RED, COLOR_BLACK, COLOR_GREEN, COLOR_BLACK, COLOR_GREEN,
     COLOR_BLACK, COLOR_GREEN, COLOR_BLACK},
    {"Cyberpunk", COLOR_CYAN, COLOR_MAGENTA, COLOR_YELLOW, COLOR_MAGENTA,
     COLOR_WHITE, COLOR_MAGENTA, COLOR_RED, COLOR_MAGENTA, COLOR_YELLOW,
     COLOR_MAGENTA, COLOR_WHITE, COLOR_MAGENTA,
     COLOR_WHITE, COLOR_MAGENTA},
    {"Catppuccin Latte", COLOR_BLUE, COLOR_WHITE, COLOR_GREEN, COLOR_WHITE,
     COLOR_BLACK, COLOR_WHITE, COLOR_RED, COLOR_WHITE, COLOR_GREEN, COLOR_WHITE,
     COLOR_BLACK, COLOR_WHITE, COLOR_BLACK,
     COLOR_WHITE},
    {"Catppuccin Frappe", COLOR_CYAN, COLOR_BLACK, COLOR_GREEN, COLOR_BLACK,
     COLOR_WHITE, COLOR_BLACK, COLOR_MAGENTA, COLOR_BLACK, COLOR_GREEN,
     COLOR_BLACK, COLOR_WHITE, COLOR_BLACK,
     COLOR_WHITE, COLOR_BLACK},
    {"Catppuccin Macchiato", COLOR_YELLOW, COLOR_BLACK, COLOR_GREEN,
     COLOR_BLACK, COLOR_WHITE, COLOR_BLACK, COLOR_MAGENTA, COLOR_BLACK,
     COLOR_GREEN, COLOR_BLACK, COLOR_WHITE,
     COLOR_BLACK, COLOR_WHITE, COLOR_BLACK},
    {"Catppuccin Mocha", COLOR_MAGENTA, COLOR_BLACK, COLOR_GREEN, COLOR_BLACK,
     COLOR_WHITE, COLOR_BLACK, COLOR_RED, COLOR_BLACK, COLOR_GREEN, COLOR_BLACK,
     COLOR_WHITE, COLOR_BLACK, COLOR_WHITE,
     COLOR_BLACK},
    {"Tokyo Night", COLOR_CYAN, COLOR_BLUE, COLOR_GREEN, COLOR_BLUE,
     COLOR_WHITE, COLOR_BLUE, COLOR_RED, COLOR_BLUE, COLOR_GREEN, COLOR_BLUE,
     COLOR_WHITE, COLOR_BLUE, COLOR_WHITE,
     COLOR_BLUE},
    {"Tokyo Night Storm", COLOR_WHITE, COLOR_BLUE, COLOR_GREEN, COLOR_BLUE,
     COLOR_CYAN, COLOR_BLUE, COLOR_RED, COLOR_BLUE, COLOR_GREEN, COLOR_BLUE,
     COLOR_CYAN, COLOR_BLUE, COLOR_CYAN, COLOR_BLUE},
    {"Tokyo Night Light", COLOR_BLUE, COLOR_WHITE, COLOR_GREEN, COLOR_WHITE,
     COLOR_BLACK, COLOR_WHITE, COLOR_RED, COLOR_WHITE, COLOR_GREEN, COLOR_WHITE,
     COLOR_BLACK, COLOR_WHITE, COLOR_BLACK,
     COLOR_WHITE},
    {"Everforest Dark", COLOR_GREEN, COLOR_BLACK, COLOR_YELLOW, COLOR_BLACK,
     COLOR_WHITE, COLOR_BLACK, COLOR_RED, COLOR_BLACK, COLOR_YELLOW,
     COLOR_BLACK, COLOR_WHITE, COLOR_BLACK,
     COLOR_WHITE, COLOR_BLACK},
    {"Everforest Light", COLOR_GREEN, COLOR_WHITE, COLOR_BLUE, COLOR_WHITE,
     COLOR_BLACK, COLOR_WHITE, COLOR_RED, COLOR_WHITE, COLOR_BLUE, COLOR_WHITE,
     COLOR_BLACK, COLOR_WHITE, COLOR_BLACK,
     COLOR_WHITE},
    {"Rosé Pine", COLOR_MAGENTA, COLOR_BLACK, COLOR_GREEN, COLOR_BLACK,
     COLOR_WHITE, COLOR_BLACK, COLOR_RED, COLOR_BLACK, COLOR_GREEN, COLOR_BLACK,
     COLOR_WHITE, COLOR_BLACK, COLOR_WHITE,
     COLOR_BLACK},
    {"Rosé Pine Moon", COLOR_MAGENTA, COLOR_BLUE, COLOR_GREEN, COLOR_BLUE,
     COLOR_WHITE, COLOR_BLUE, COLOR_RED, COLOR_BLUE, COLOR_GREEN, COLOR_BLUE,
     COLOR_WHITE, COLOR_BLUE, COLOR_WHITE,
     COLOR_BLUE},
    {"Rosé Pine Dawn", COLOR_BLUE, COLOR_WHITE, COLOR_GREEN, COLOR_WHITE,
     COLOR_BLACK, COLOR_WHITE, COLOR_RED, COLOR_WHITE, COLOR_GREEN, COLOR_WHITE,
     COLOR_BLACK, COLOR_WHITE, COLOR_BLACK,
     COLOR_WHITE},
    {"Kanagawa", COLOR_YELLOW, COLOR_BLACK, COLOR_GREEN, COLOR_BLACK,
     COLOR_WHITE, COLOR_BLACK, COLOR_RED, COLOR_BLACK, COLOR_GREEN, COLOR_BLACK,
     COLOR_WHITE, COLOR_BLACK, COLOR_WHITE,
     COLOR_BLACK},
    {"Aura", COLOR_MAGENTA, COLOR_BLACK, COLOR_GREEN, COLOR_BLACK, COLOR_WHITE,
     COLOR_BLACK, COLOR_RED, COLOR_BLACK, COLOR_GREEN, COLOR_BLACK, COLOR_WHITE,
     COLOR_BLACK, COLOR_WHITE, COLOR_BLACK},
    {"Winter is Coming", COLOR_CYAN, COLOR_BLUE, COLOR_WHITE, COLOR_BLUE,
     COLOR_WHITE, COLOR_BLUE, COLOR_RED, COLOR_BLUE, COLOR_WHITE, COLOR_BLUE,
     COLOR_WHITE, COLOR_BLUE, COLOR_WHITE,
     COLOR_BLUE},
    {"Sonokai", COLOR_YELLOW, COLOR_BLACK, COLOR_GREEN, COLOR_BLACK,
     COLOR_WHITE, COLOR_BLACK, COLOR_RED, COLOR_BLACK, COLOR_GREEN, COLOR_BLACK,
     COLOR_WHITE, COLOR_BLACK, COLOR_WHITE,
     COLOR_BLACK},
    {"Edge", COLOR_GREEN, COLOR_WHITE, COLOR_BLUE, COLOR_WHITE, COLOR_BLACK,
     COLOR_WHITE, COLOR_RED, COLOR_WHITE, COLOR_BLUE, COLOR_WHITE, COLOR_BLACK,
     COLOR_WHITE, COLOR_BLACK, COLOR_WHITE},
    {"FireCode", COLOR_YELLOW, COLOR_BLACK, COLOR_GREEN, COLOR_BLACK,
     COLOR_WHITE, COLOR_BLACK, COLOR_RED, COLOR_BLACK, COLOR_GREEN, COLOR_BLACK,
     COLOR_WHITE, COLOR_BLACK, COLOR_WHITE,
     COLOR_BLACK},
    {"Hyper", COLOR_CYAN, COLOR_BLACK, COLOR_MAGENTA, COLOR_BLACK, COLOR_WHITE,
     COLOR_BLACK, COLOR_RED, COLOR_BLACK, COLOR_MAGENTA, COLOR_BLACK,
     COLOR_WHITE, COLOR_BLACK, COLOR_WHITE,
     COLOR_BLACK},
    {"Zenburn", COLOR_YELLOW, COLOR_BLACK, COLOR_GREEN, COLOR_BLACK,
     COLOR_WHITE, COLOR_BLACK, COLOR_RED, COLOR_BLACK, COLOR_GREEN, COLOR_BLACK,
     COLOR_WHITE, COLOR_BLACK, COLOR_WHITE,
     COLOR_BLACK}};

const int num_ordo_themes = sizeof(ordo_themes) / sizeof(OrdoTheme);
OrdoTheme *external_themes = NULL;
int num_external_themes = 0;

const OrdoTheme *theme_get_by_name(const char *name) {
  if (!name)
    return NULL;
  for (int i = 0; i < num_ordo_themes; i++) {
    if (strcmp(ordo_themes[i].name, name) == 0) {
      return &ordo_themes[i];
    }
  }
  for (int i = 0; i < num_external_themes; i++) {
    if (strcmp(external_themes[i].name, name) == 0) {
      return &external_themes[i];
    }
  }
  return NULL;
}

static void parse_and_add_theme(const char *file_path) {
  FILE *file = fopen(file_path, "r");
  if (!file)
    return;

  char line[256];
  OrdoTheme new_theme = {0};
  char *theme_name = NULL;

  while (fgets(line, sizeof(line), file)) {
    if (line[0] == '#' || line[0] == '\n' || line[0] == '\r')
      continue;

    char *separator = strchr(line, '=');
    if (separator) {
        *separator = '\0';
        char *key = line;
        char *value = separator + 1;

        char *trimmed_key = trim(key);
        char *trimmed_value = trim(value);

        if (trimmed_key && trimmed_value) {
            if (strcmp(trimmed_key, "name") == 0) {
                theme_name = strdup(trimmed_value);
            } else if (strcmp(trimmed_key, "header_fg") == 0) {
                new_theme.header_fg = color_name_to_value(trimmed_value);
            } else if (strcmp(trimmed_key, "header_bg") == 0) {
                new_theme.header_bg = color_name_to_value(trimmed_value);
            } else if (strcmp(trimmed_key, "done_fg") == 0) {
                new_theme.done_fg = color_name_to_value(trimmed_value);
            } else if (strcmp(trimmed_key, "done_bg") == 0) {
                new_theme.done_bg = color_name_to_value(trimmed_value);
            } else if (strcmp(trimmed_key, "pending_fg") == 0) {
                new_theme.pending_fg = color_name_to_value(trimmed_value);
            } else if (strcmp(trimmed_key, "pending_bg") == 0) {
                new_theme.pending_bg = color_name_to_value(trimmed_value);
            } else if (strcmp(trimmed_key, "error_fg") == 0) {
                new_theme.error_fg = color_name_to_value(trimmed_value);
            } else if (strcmp(trimmed_key, "error_bg") == 0) {
                new_theme.error_bg = color_name_to_value(trimmed_value);
            } else if (strcmp(trimmed_key, "success_fg") == 0) {
                new_theme.success_fg = color_name_to_value(trimmed_value);
            } else if (strcmp(trimmed_key, "success_bg") == 0) {
                new_theme.success_bg = color_name_to_value(trimmed_value);
            } else if (strcmp(trimmed_key, "task_done_fg") == 0) {
                new_theme.task_done_fg = color_name_to_value(trimmed_value);
            } else if (strcmp(trimmed_key, "task_done_bg") == 0) {
                new_theme.task_done_bg = color_name_to_value(trimmed_value);
            } else if (strcmp(trimmed_key, "task_pending_fg") == 0) {
                new_theme.task_pending_fg = color_name_to_value(trimmed_value);
            } else if (strcmp(trimmed_key, "task_pending_bg") == 0) {
                new_theme.task_pending_bg = color_name_to_value(trimmed_value);
            }
        }
        free(trimmed_key);
        free(trimmed_value);
    }
  }
  fclose(file);

  if (theme_name) {
    // Validar se todas as cores foram carregadas corretamente
    if (new_theme.header_fg == -1 || new_theme.header_bg == -1 ||
        new_theme.done_fg == -1 || new_theme.done_bg == -1 ||
        new_theme.pending_fg == -1 || new_theme.pending_bg == -1 ||
        new_theme.error_fg == -1 || new_theme.error_bg == -1 ||
        new_theme.success_fg == -1 || new_theme.success_bg == -1 ||
        new_theme.task_done_fg == -1 || new_theme.task_done_bg == -1 ||
        new_theme.task_pending_fg == -1 || new_theme.task_pending_bg == -1) {

      fprintf(stderr,
              "Aviso: Tema '%s' no arquivo '%s' tem cores invalidas e nao sera "
              "carregado.\n",
              theme_name, file_path);
      free(theme_name); // Libera a memoria do nome, ja que o tema foi rejeitado
      return;
    }

    if (theme_get_by_name(theme_name) == NULL) {
      OrdoTheme *new_external_themes = realloc(
          external_themes, (num_external_themes + 1) * sizeof(OrdoTheme));
      if (!new_external_themes) {
        // Nao foi possivel alocar, libera o nome do tema e retorna
        free(theme_name);
        return;
      }
      external_themes = new_external_themes;
      new_theme.name = theme_name;
      external_themes[num_external_themes++] = new_theme;
    } else {
      fprintf(stderr,
              "Aviso: Tema '%s' do arquivo '%s' ja existe e sera ignorado.\n",
              theme_name, file_path);
      free(theme_name);
    }
  }
}

static void load_themes_from_path(const char *path) {
  FileList file_list = platform_list_files_in_dir(path, ".theme");
  for (int i = 0; i < file_list.count; i++) {
    char *full_path = path_join(path, file_list.files[i]);
    if (full_path) {
      parse_and_add_theme(full_path);
      free(full_path);
    }
  }
  platform_free_file_list(&file_list);
}

void theme_load_external() {
  // 1. Carrega do diretório de configuração do usuário
  char *config_dir = platform_get_config_dir();
  if (config_dir) {
    char *themes_path = path_join(config_dir, "themes");
    if (themes_path) {
      platform_create_dir_recursive(themes_path);
      load_themes_from_path(themes_path);
      free(themes_path);
    }
    free(config_dir);
  }

  // 2. Carrega do subdiretório 'themes' local
  load_themes_from_path("themes");

  // 3. Carrega arquivos .theme do diretório raiz do projeto
  load_themes_from_path(".");
}

void theme_free_external() {
  for (int i = 0; i < num_external_themes; i++) {
    free((void *)external_themes[i].name);
  }
  free(external_themes);
  external_themes = NULL;
  num_external_themes = 0;
}
