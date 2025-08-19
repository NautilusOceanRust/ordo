#define _POSIX_C_SOURCE 200809L
#include "settings_ui.h"
#include "config.h"
#include "i18n.h"
#include "theme.h"
#include "utils.h"
#include <dirent.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <wchar.h>

#define MAX_LANGS 20
#define MAX_LANG_CODE_LEN 10

// Funcao para aplicar um tema temporariamente para preview
static void apply_theme_preview(const OrdoTheme *theme, const AppConfig *config) {
    init_pair(config->color_pair_header, theme->header_fg, theme->header_bg);
    init_pair(config->color_pair_status_done, theme->done_fg, theme->done_bg);
    init_pair(config->color_pair_status_pending, theme->pending_fg, theme->pending_bg);
    init_pair(config->color_pair_error, theme->error_fg, theme->error_bg);
    init_pair(config->color_pair_success, theme->success_fg, theme->success_bg);
    init_pair(config->color_pair_task_done, theme->task_done_fg, theme->task_done_bg);
    init_pair(config->color_pair_task_pending, theme->task_pending_fg, theme->task_pending_bg);
}

static WINDOW *draw_theme_preview_window(int starty, int startx,
                                         const OrdoTheme *theme, const AppConfig *config) {
  if (startx + 40 > COLS)
    startx = COLS - 40;
  if (starty + 10 > LINES)
    starty = LINES - 10;
  if (startx < 0)
    startx = 0;
  if (starty < 0)
    starty = 0;

  WINDOW *win = newwin(10, 40, starty, startx);
  wbkgd(win, COLOR_PAIR(config->color_pair_header));
  box(win, 0, 0);

  wchar_t wide_buffer[256];
  char str_buffer[256];
  char truncated_buffer[256];

  snprintf(str_buffer, sizeof(str_buffer), "%s: %s",
           get_translation("THEME_PREVIEW"), theme->name);
  truncar_por_largura(truncated_buffer, sizeof(truncated_buffer), str_buffer,
                      38);
  mbstowcs(wide_buffer, truncated_buffer, 256);
  mvwaddwstr(win, 1, 2, wide_buffer);

  wattron(win, COLOR_PAIR(config->color_pair_header));
  mbstowcs(wide_buffer, get_translation("HEADER_TEXT"), 256);
  mvwaddwstr(win, 3, 4, wide_buffer);
  wattroff(win, COLOR_PAIR(config->color_pair_header));

  wattron(win, COLOR_PAIR(config->color_pair_task_done));
  snprintf(str_buffer, sizeof(str_buffer), "[x] %s",
           get_translation("DONE_TASK"));
  mbstowcs(wide_buffer, str_buffer, 256);
  mvwaddwstr(win, 4, 4, wide_buffer);
  wattroff(win, COLOR_PAIR(config->color_pair_task_done));

  wattron(win, COLOR_PAIR(config->color_pair_task_pending));
  snprintf(str_buffer, sizeof(str_buffer), "[ ] %s",
           get_translation("PENDING_TASK"));
  mbstowcs(wide_buffer, str_buffer, 256);
  mvwaddwstr(win, 5, 4, wide_buffer);
  wattroff(win, COLOR_PAIR(config->color_pair_task_pending));

  wattron(win, COLOR_PAIR(config->color_pair_success));
  mbstowcs(wide_buffer, get_translation("SUCCESS_MESSAGE"), 256);
  mvwaddwstr(win, 6, 4, wide_buffer);
  wattroff(win, COLOR_PAIR(config->color_pair_success));

  wattron(win, COLOR_PAIR(config->color_pair_error));
  mbstowcs(wide_buffer, get_translation("ERROR_MESSAGE"), 256);
  mvwaddwstr(win, 7, 4, wide_buffer);
  wattroff(win, COLOR_PAIR(config->color_pair_error));

  wnoutrefresh(win);
  return win;
}

static void ui_settings_select_theme(AppConfig *config) {
  int current_selection = 0;
  WINDOW *preview_win = NULL;

  for (int i = 0; i < num_ordo_themes; ++i) {
    if (strcmp(config->theme_name, ordo_themes[i].name) == 0) {
      current_selection = i;
      break;
    }
  }

  const OrdoTheme *original_theme = theme_get_by_name(config->theme_name);
  int scroll_offset = 0;
  int viewport_height = LINES - 5;

  while (1) {
    if (preview_win) {
      delwin(preview_win);
      preview_win = NULL;
    }
    clear();

    wchar_t wide_buffer[256];
    char truncated_buffer[256];
    mbstowcs(wide_buffer, get_translation("SELECT_THEME_INSTRUCTIONS"), 256);
    mvaddwstr(1, (COLS - wcswidth(wide_buffer, -1)) / 2, wide_buffer);

    if (current_selection < scroll_offset)
      scroll_offset = current_selection;
    if (current_selection >= scroll_offset + viewport_height)
      scroll_offset = current_selection - viewport_height + 1;

    int list_start_y = 3;
    int max_name_len = 0;
    for (int i = 0; i < num_ordo_themes; i++) {
      int len = strlen(ordo_themes[i].name);
      if (len > max_name_len)
        max_name_len = len;
    }
    int list_start_x = (COLS - max_name_len - 45) / 2;
    if (list_start_x < 0)
      list_start_x = 0;

    for (int i = 0;
         i < viewport_height && (scroll_offset + i) < num_ordo_themes; i++) {
      int theme_index = scroll_offset + i;
      if (theme_index == current_selection)
        attron(A_REVERSE);
      truncar_por_largura(truncated_buffer, sizeof(truncated_buffer),
                          ordo_themes[theme_index].name, max_name_len);
      mvprintw(list_start_y + i, list_start_x, " %s ", truncated_buffer);
      if (theme_index == current_selection)
        attroff(A_REVERSE);
    }

    const OrdoTheme *preview_theme = &ordo_themes[current_selection];
    apply_theme_preview(preview_theme, config);
    wnoutrefresh(stdscr);
    preview_win = draw_theme_preview_window(
        list_start_y, list_start_x + max_name_len + 5, preview_theme, config);
    doupdate();

    int choice = getch();
    switch (choice) {
    case KEY_UP:
      current_selection =
          (current_selection - 1 + num_ordo_themes) % num_ordo_themes;
      break;
    case KEY_DOWN:
      current_selection = (current_selection + 1) % num_ordo_themes;
      break;
    case '\n':
    case KEY_ENTER:
      snprintf(config->theme_name, sizeof(config->theme_name), "%s", ordo_themes[current_selection].name);
      config_save(config);
      config_init_color_pairs(config);
      return;
    case 'q':
    case 'Q':
      if (preview_win) {
        delwin(preview_win);
        preview_win = NULL;
      }
      apply_theme_preview(original_theme, config);
      config_init_color_pairs(config);
      return;
    }
  }
}

static void ui_settings_select_language(AppConfig *config) {
  char **available_langs = NULL;
  int lang_count = 0;
  int capacity = 10;
  available_langs = malloc(capacity * sizeof(char *));
  if (!available_langs)
    return;

  char *langs_path = path_join(PROJECT_SOURCE_ROOT, "langs");
  if (!langs_path) {
    free(available_langs);
    return;
  }

  DIR *d = opendir(langs_path);
  if (d) {
    struct dirent *dir;
    while ((dir = readdir(d)) != NULL) {
      if (strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0) {
        continue;
      }

      char *full_path = path_join(langs_path, dir->d_name);
      if (!full_path) continue;

      struct stat path_stat;
      stat(full_path, &path_stat);
      free(full_path);

      if (!S_ISREG(path_stat.st_mode)) {
        continue;
      }

      if (strstr(dir->d_name, ".lang")) {
        if (lang_count >= capacity) {
          capacity *= 2;
          char **new_langs =
              realloc(available_langs, capacity * sizeof(char *));
          if (!new_langs) {
            for (int i = 0; i < lang_count; ++i)
              free(available_langs[i]);
            free(available_langs);
            closedir(d);
            return;
          }
          available_langs = new_langs;
        }
        char *dot = strrchr(dir->d_name, '.');
        if (dot) {
          *dot = '\0';
          available_langs[lang_count++] = strdup(dir->d_name);
        }
      }
    }
    closedir(d);
  } else {
    free(available_langs);
    return;
  }

  if (lang_count == 0) {
    free(available_langs);
    return;
  }

  int current_selection = 0;
  for (int i = 0; i < lang_count; ++i) {
    if (strcmp(config->lang, available_langs[i]) == 0) {
      current_selection = i;
      break;
    }
  }

  while (1) {
    clear();
    wchar_t wide_buffer[256];
    mbstowcs(wide_buffer, get_translation("SELECT_LANGUAGE_INSTRUCTIONS"), 256);
    mvaddwstr(1, (COLS - wcswidth(wide_buffer, -1)) / 2, wide_buffer);

    int list_start_y = (LINES - lang_count) / 2;
    if (list_start_y < 3)
      list_start_y = 3;

    for (int i = 0; i < lang_count; i++) {
      if (i == current_selection)
        attron(A_REVERSE);
      mvprintw(list_start_y + i, (COLS - strlen(available_langs[i])) / 2,
               " %s ", available_langs[i]);
      if (i == current_selection)
        attroff(A_REVERSE);
    }
    refresh();

    int choice = getch();
    switch (choice) {
    case KEY_UP:
      current_selection = (current_selection - 1 + lang_count) % lang_count;
      break;
    case KEY_DOWN:
      current_selection = (current_selection + 1) % lang_count;
      break;
    case '\n':
    case KEY_ENTER:
      snprintf(config->lang, sizeof(config->lang), "%s", available_langs[current_selection]);
      config_save(config);
      i18n_load_language(config->lang);
      for (int i = 0; i < lang_count; i++)
        free(available_langs[i]);
      free(available_langs);
      return;
    case 'q':
    case 'Q':
      for (int i = 0; i < lang_count; i++)
        free(available_langs[i]);
      free(available_langs);
      return;
    }
  }
}

void settings_ui_show(AppConfig *config) {
  int choice = 0;
  while (choice != '3') {
    clear();
    wchar_t wide_buffer[256];
    char str_buffer[256];
    char truncated_buffer[256];

    mbstowcs(wide_buffer, get_translation("SETTINGS_MENU"), 256);
    mvaddwstr((LINES - 5) / 2, (COLS - wcswidth(wide_buffer, -1)) / 2,
              wide_buffer);

    int y = (LINES - 5) / 2 + 2;
    int x = (COLS - 20) / 2;

    snprintf(str_buffer, sizeof(str_buffer), "1. %s",
             get_translation("CHANGE_LANGUAGE"));
    truncar_por_largura(truncated_buffer, sizeof(truncated_buffer), str_buffer,
                        COLS - x * 2);
    mbstowcs(wide_buffer, truncated_buffer, 256);
    mvaddwstr(y++, x, wide_buffer);

    snprintf(str_buffer, sizeof(str_buffer), "2. %s",
             get_translation("CHANGE_THEME"));
    truncar_por_largura(truncated_buffer, sizeof(truncated_buffer), str_buffer,
                        COLS - x * 2);
    mbstowcs(wide_buffer, truncated_buffer, 256);
    mvaddwstr(y++, x, wide_buffer);

    int len = snprintf(str_buffer, sizeof(str_buffer), "3. %s",
             get_translation("BACK_TO_MAIN_MENU"));
    if ((size_t)len >= sizeof(str_buffer)) {
      // Handle error
    }
    truncar_por_largura(truncated_buffer, sizeof(truncated_buffer), str_buffer,
                        COLS - x * 2);
    mbstowcs(wide_buffer, truncated_buffer, 256);
    mvaddwstr(y, x, wide_buffer);

    mbstowcs(wide_buffer, get_translation("PROMPT_CHOICE"), 256);
    mvaddwstr(LINES - 1, (COLS - wcswidth(wide_buffer, -1) - 2) / 2,
              wide_buffer);

    refresh();
    choice = getch();

    switch (choice) {
    case '1':
      ui_settings_select_language(config);
      break;
    case '2':
      ui_settings_select_theme(config);
      break;
    }
  }
}