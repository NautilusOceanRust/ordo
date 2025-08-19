#define _POSIX_C_SOURCE 200809L
#include "ui.h"
#include "i18n.h"
#include "utils.h"
#include <ctype.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

// --- Variáveis Globais da UI ---
WINDOW *sidebar_win, *content_win, *status_win;
#define SIDEBAR_WIDTH 30

// --- Funções de Ciclo de Vida ---
void ui_init(const AppConfig *config) {
  initscr();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);
  start_color();
  config_init_color_pairs(config);
  curs_set(0);
  mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, NULL);
  define_key("\x1b[<", 27); // Habilita o modo de relatório do mouse xterm
}

void ui_destroy() {
  delwin(sidebar_win);
  delwin(content_win);
  delwin(status_win);
  endwin();
}

// --- Funções de Desenho e Layout ---
void ui_draw_layout() {
  int screen_h, screen_w;
  getmaxyx(stdscr, screen_h, screen_w);
  if (sidebar_win)
    delwin(sidebar_win);
  if (content_win)
    delwin(content_win);
  if (status_win)
    delwin(status_win);
  sidebar_win = newwin(screen_h - 1, SIDEBAR_WIDTH, 0, 0);
  content_win =
      newwin(screen_h - 1, screen_w - SIDEBAR_WIDTH, 0, SIDEBAR_WIDTH);
  status_win = newwin(1, screen_w, screen_h - 1, 0);
  box(sidebar_win, 0, 0);
  box(content_win, 0, 0);
  wnoutrefresh(stdscr);
  wnoutrefresh(sidebar_win);
  wnoutrefresh(content_win);
  wnoutrefresh(status_win);
}

void ui_display_menu(WINDOW *win, const AppConfig *config,
                     AppView current_view) {
  wclear(win);
  box(win, 0, 0);
  wchar_t wide_buffer[256];
  char str_buffer[256];
  char truncated_buffer[256];

  const char *title = (current_view == VIEW_MAIN) ? "APP_TITLE" : "TRASH_TITLE";
  mbstowcs(wide_buffer, get_translation(title), 256);

  // Aplica a cor do cabeçalho
  wattron(win, COLOR_PAIR(config->color_pair_header));
  mvwaddwstr(win, 1, (SIDEBAR_WIDTH - wcswidth(wide_buffer, -1)) / 2,
             wide_buffer);
  wattroff(win, COLOR_PAIR(config->color_pair_header));

  mvwaddwstr(win, 2, 1, L"────────────────────────────");

  if (current_view == VIEW_MAIN) {
    const char *menu_items[] = {
        "MENU_ADD",    "MENU_REMOVE", "MENU_EDIT",   "MENU_TOGGLE",
        "MENU_VIEW",   "MENU_SETTINGS", "MENU_UNDO",   "MENU_REDO",
        "MENU_TRASH",  "MENU_EXIT"};
    const char *keys[] = {"1", "2", "3", "4", "5", "6", "u", "y", "t", "7"};
    for (size_t i = 0; i < sizeof(menu_items) / sizeof(menu_items[0]); ++i) {
      int len = snprintf(str_buffer, sizeof(str_buffer), "[%s] %s", keys[i],
               get_translation(menu_items[i]));
      if ((size_t)len >= sizeof(str_buffer)) {
        // Handle error
      }
      truncar_por_largura(truncated_buffer, sizeof(truncated_buffer),
                          str_buffer, SIDEBAR_WIDTH - 4);
      mbstowcs(wide_buffer, truncated_buffer, 256);
      mvwaddwstr(win, 4 + i, 2, wide_buffer);
    }
  } else { // VIEW_TRASH
    const char *menu_items[] = {"MENU_RESTORE", "MENU_PERM_DELETE",
                                "MENU_BACK"};
    const char *keys[] = {"r", "d", "q"};
    for (size_t i = 0; i < sizeof(menu_items) / sizeof(menu_items[0]); ++i) {
      int len = snprintf(str_buffer, sizeof(str_buffer), "[%s] %s", keys[i],
               get_translation(menu_items[i]));
      if ((size_t)len >= sizeof(str_buffer)) {
        // Handle error
      }
      truncar_por_largura(truncated_buffer, sizeof(truncated_buffer),
                          str_buffer, SIDEBAR_WIDTH - 4);
      mbstowcs(wide_buffer, truncated_buffer, 256);
      mvwaddwstr(win, 4 + i, 2, wide_buffer);
    }
  }

  wnoutrefresh(win);
}

void ui_display_tasks(WINDOW *win, const Tarefa *tasks, int num_tasks,
                      const AppConfig *config, int scroll_offset,
                      int current_selection, AppView current_view) {
  
  wclear(win);
  box(win, 0, 0);

  // Adiciona o título da janela de conteúdo
  const char *title_key =
      (current_view == VIEW_MAIN) ? "TASKS_TITLE" : "TRASH_TITLE";
  wchar_t wide_title[100];
  mbstowcs(wide_title, get_translation(title_key), 100);

  // Aplica a cor do cabeçalho ao título
  wattron(win, COLOR_PAIR(config->color_pair_header));
  mvwaddwstr(win, 0, 3, L" ");
  mvwaddwstr(win, 0, 4, wide_title);
  mvwaddwstr(win, 0, 4 + wcswidth(wide_title, -1), L" ");
  wattroff(win, COLOR_PAIR(config->color_pair_header));

  int win_h, win_w;
  getmaxyx(win, win_h, win_w);
  int content_h = win_h - 2;
  if (num_tasks == 0) {
    const char *empty_msg_key =
        (current_view == VIEW_MAIN) ? "TASK_LIST_EMPTY" : "TRASH_EMPTY";
    wchar_t wide_buffer[100];
    mbstowcs(wide_buffer, get_translation(empty_msg_key), 100);
    mvwaddwstr(win, content_h / 2, (win_w - wcswidth(wide_buffer, -1)) / 2,
               wide_buffer);
  } else {
    for (int i = 0; i < content_h && (scroll_offset + i) < num_tasks; ++i) {
      int task_index = scroll_offset + i;

      // Define o par de cores com base no status da tarefa
      short color_pair = tasks[task_index].concluida
                             ? config->color_pair_task_done
                             : config->color_pair_task_pending;
      wattron(win, COLOR_PAIR(color_pair));

      const char *status_icon = tasks[task_index].concluida ? "✔" : "○";
      char task_line[win_w];
      snprintf(task_line, sizeof(task_line), " %s %s", status_icon,
               tasks[task_index].descricao);
      char truncated_line[win_w];
      truncar_por_largura(truncated_line, sizeof(truncated_line), task_line,
                          win_w - 4);
      wchar_t wide_task[win_w];
      mbstowcs(wide_task, truncated_line, win_w);
      if (task_index == current_selection)
        wattron(win, A_REVERSE);
      mvwaddwstr(win, i + 1, 2, wide_task);
      if (task_index == current_selection)
        wattroff(win, A_REVERSE);

      wattroff(win, COLOR_PAIR(color_pair));
    }
    if (num_tasks > content_h) {
      int bar_h = (int)((float)content_h / num_tasks * content_h);
      if (bar_h < 1)
        bar_h = 1;
      int bar_y = (int)((float)scroll_offset / num_tasks * content_h) + 1;
      for (int i = 0; i < bar_h; ++i)
        mvwaddch(win, bar_y + i, win_w - 2, ACS_BLOCK);
    }
  }
  wnoutrefresh(win);
}

void ui_show_message(const char *translation_key, bool is_error,
                     const AppConfig *config) {
  werase(status_win);
  short color_pair =
      is_error ? config->color_pair_error : config->color_pair_success;
  wattron(status_win, COLOR_PAIR(color_pair));
  mvwprintw(status_win, 0, 1, "%s", get_translation(translation_key));
  wattroff(status_win, COLOR_PAIR(color_pair));
  wnoutrefresh(status_win);
}

static WINDOW *create_popup(int h, int w, const char *title_key) {
  int screen_h, screen_w;
  getmaxyx(stdscr, screen_h, screen_w);
  int y = (screen_h - h) / 2;
  int x = (screen_w - w) / 2;
  WINDOW *win = newwin(h, w, y, x);
  box(win, 0, 0);
  wchar_t wide_title[100];
  mbstowcs(wide_title, get_translation(title_key), 100);
  mvwaddwstr(win, 1, (w - wcswidth(wide_title, -1)) / 2, wide_title);
  wrefresh(win);
  return win;
}

// --- Funções Auxiliares para Manipulação de Texto --- // Encontra o início da palavra anterior
static int find_prev_word_start(const wchar_t *str, int current_pos) {
  if (current_pos == 0) return 0;
  int pos = current_pos - 1;
  // Pula espaços em branco à esquerda
  while (pos > 0 && iswspace(str[pos])) {
    pos--;
  }
  // Pula a palavra
  while (pos > 0 && !iswspace(str[pos - 1])) {
    pos--;
  }
  return pos;
}

// Encontra o início da próxima palavra
static int find_next_word_start(const wchar_t *str, int current_pos) {
    int len = wcslen(str);
    if (current_pos >= len) return len;
    int pos = current_pos;
    // Pula a palavra atual
    while(pos < len && !iswspace(str[pos])) {
        pos++;
    }
    // Pula os espaços em branco
    while(pos < len && iswspace(str[pos])) {
        pos++;
    }
    return pos;
}

// --- Códigos de Tecla Customizados ---
#define KEY_CTRL_RIGHT 500
#define KEY_CTRL_LEFT 501
#define KEY_CTRL_BACKSPACE 502

// --- Refatoração da Entrada de Texto ---

// Estrutura para manter o estado do widget de entrada de texto
typedef struct {
  WINDOW *win;
  wchar_t *buffer;
  int buffer_size;
  int cursor_pos;
  int len;
  int win_h;
  int win_w;
  int max_chars_per_line;
} TextInputState;

// Desenha o conteúdo do buffer de texto, o contador e o cursor
static void draw_text_input(TextInputState *state) {
  wclear(state->win);
  box(state->win, 0, 0);

  // Desenha o texto com quebra de linha
  int current_y = 1, current_x = 1;
  for (int i = 0; i < state->len; ++i) {
    if (current_x >= state->max_chars_per_line) {
      current_y++;
      current_x = 1;
    }
    if (current_y >= state->win_h - 2) break;
    mvwaddch(state->win, current_y, current_x, state->buffer[i]);
    current_x++;
  }

  // Desenha o contador de caracteres
  char counter[32];
  snprintf(counter, sizeof(counter), "[%d/%d]", state->len, state->buffer_size - 1);
  mvwprintw(state->win, state->win_h - 2, state->win_w - strlen(counter) - 2, "%s", counter);

  // Posiciona o cursor
  int cursor_y = 1 + (state->cursor_pos / state->max_chars_per_line);
  int cursor_x = 1 + (state->cursor_pos % state->max_chars_per_line);
  if (cursor_y < state->win_h - 1) {
    wmove(state->win, cursor_y, cursor_x);
  }
  wrefresh(state->win);
}

// Processa uma única entrada de tecla e atualiza o estado
// Retorna `false` se a edição deve terminar (Enter ou Esc), `true` caso contrário.
static bool handle_text_input(TextInputState *state, wint_t ch) {
  switch (ch) {
  case '\n': // Enter
    return false;
  case 27: // Esc
    state->buffer[0] = L'\0';
    return false;

  // --- Movimentação do Cursor ---
  case KEY_LEFT:
    if (state->cursor_pos > 0) state->cursor_pos--;
    break;
  case KEY_RIGHT:
    if (state->cursor_pos < state->len) state->cursor_pos++;
    break;
  case KEY_UP:
    if (state->cursor_pos >= state->max_chars_per_line) state->cursor_pos -= state->max_chars_per_line;
    break;
  case KEY_DOWN:
    state->cursor_pos = (state->cursor_pos + state->max_chars_per_line < state->len) ? state->cursor_pos + state->max_chars_per_line : state->len;
    break;
  case KEY_HOME:
    state->cursor_pos = 0;
    break;
  case KEY_END:
    state->cursor_pos = state->len;
    break;
  case KEY_CTRL_LEFT:
    state->cursor_pos = find_prev_word_start(state->buffer, state->cursor_pos);
    break;
  case KEY_CTRL_RIGHT:
    state->cursor_pos = find_next_word_start(state->buffer, state->cursor_pos);
    break;

  // --- Edição de Texto ---
  case KEY_BACKSPACE:
  case 127:
    if (state->cursor_pos > 0) {
      memmove(&state->buffer[state->cursor_pos - 1], &state->buffer[state->cursor_pos], (state->len - state->cursor_pos + 1) * sizeof(wchar_t));
      state->cursor_pos--;
    }
    break;
  case KEY_DC: // Delete
    if (state->cursor_pos < state->len) {
      memmove(&state->buffer[state->cursor_pos], &state->buffer[state->cursor_pos + 1], (state->len - state->cursor_pos) * sizeof(wchar_t));
    }
    break;
  case KEY_CTRL_BACKSPACE:
  {
    int prev_word_start = find_prev_word_start(state->buffer, state->cursor_pos);
    if (prev_word_start < state->cursor_pos) {
      memmove(&state->buffer[prev_word_start], &state->buffer[state->cursor_pos], (state->len - state->cursor_pos + 1) * sizeof(wchar_t));
      state->cursor_pos = prev_word_start;
    }
    break;
  }
  default:
    // Inserir caractere
    if (iswprint(ch) && state->len < state->buffer_size - 1) {
      memmove(&state->buffer[state->cursor_pos + 1], &state->buffer[state->cursor_pos], (state->len - state->cursor_pos + 1) * sizeof(wchar_t));
      state->buffer[state->cursor_pos] = ch;
      state->cursor_pos++;
    }
    break;
  }
  return true;
}

// Função principal de entrada de texto, agora refatorada.
static void get_editable_input(WINDOW *win, char *buffer, int max_len) {
  wchar_t w_buffer[max_len];
  mbstowcs(w_buffer, buffer, max_len);

  TextInputState state;
  state.win = win;
  state.buffer = w_buffer;
  state.buffer_size = max_len;
  getmaxyx(win, state.win_h, state.win_w);
  state.max_chars_per_line = state.win_w - 2;
  state.len = wcslen(w_buffer);
  state.cursor_pos = state.len;
  
  keypad(win, TRUE);
  curs_set(1);

  // Define códigos de tecla para Ctrl + Setas/Backspace
  define_key("\x1b[1;5C", KEY_CTRL_RIGHT);
  define_key("\x1b[1;5D", KEY_CTRL_LEFT);
  define_key("\x08", KEY_CTRL_BACKSPACE);

  wint_t ch;
  bool keep_editing = true;
  
  // O loop principal agora é muito mais simples
  while (keep_editing) {
    state.len = wcslen(state.buffer);
    if (state.cursor_pos > state.len) {
        state.cursor_pos = state.len;
    }

    draw_text_input(&state);
    wget_wch(win, &ch);
    keep_editing = handle_text_input(&state, ch);
  }

  curs_set(0);
  wcstombs(buffer, w_buffer, max_len);
}

OrdoResult ui_add_task(const AppConfig *config, char **new_description_out) {
  (void)config;
  *new_description_out = NULL;
  WINDOW *win = create_popup(16, 70, "MENU_ADD");
  mvwprintw(win, 2, 2, "%s", get_translation("ADD_PROMPT"));

  WINDOW *input_win = newwin(10, 66, getbegy(win) + 4, getbegx(win) + 2);
  box(input_win, 0, 0);
  wrefresh(input_win);

  char mb_desc[MAX_DESCRICAO * 4] = {0};
  get_editable_input(input_win, mb_desc, sizeof(mb_desc));

  delwin(input_win);
  delwin(win);

  auto_free_ptr char *trimmed_desc = trim(mb_desc);
  if (!trimmed_desc || strlen(trimmed_desc) == 0) {
    return ORDO_ERROR_EMPTY_DESCRIPTION;
  }

  *new_description_out = strdup(trimmed_desc);
  if (!*new_description_out) {
    return ORDO_ERROR_GENERIC;
  }

  return ORDO_OK;
}

OrdoResult ui_edit_task(const AppConfig *config,
                        char **new_description_out) {
  (void)config;
  *new_description_out = NULL;
  WINDOW *win = create_popup(16, 70, "MENU_EDIT");
  mvwprintw(win, 2, 2, "%s", get_translation("EDIT_NEW_DESC_PROMPT"));

  WINDOW *input_win = newwin(10, 66, getbegy(win) + 4, getbegx(win) + 2);
  box(input_win, 0, 0);
  wrefresh(input_win);

  char mb_desc[MAX_DESCRICAO * 4] = {0};
  get_editable_input(input_win, mb_desc, sizeof(mb_desc));

  delwin(input_win);
  delwin(win);

  auto_free_ptr char *trimmed_desc = trim(mb_desc);
  if (!trimmed_desc || strlen(trimmed_desc) == 0) {
    return ORDO_ERROR_EMPTY_DESCRIPTION;
  }

  // Aloca memória e retorna a nova descrição
  *new_description_out = strdup(trimmed_desc);
  if (!*new_description_out) {
    return ORDO_ERROR_GENERIC; // Falha de alocação
  }

  return ORDO_OK;
}

bool ui_confirm_action(const char *translation_key, const char *title_key,
                       const AppConfig *config) {
  WINDOW *win = create_popup(5, 60, title_key);
  wattron(win, COLOR_PAIR(config->color_pair_error));
  mvwprintw(win, 2, 4, "%s", get_translation(translation_key));
  wattroff(win, COLOR_PAIR(config->color_pair_error));
  wrefresh(win);
  int choice = wgetch(win);
  delwin(win);
  return (choice == 'y' || choice == 'Y' || choice == 's' || choice == 'S');
}

void ui_display_full_task(const Tarefa *task, const AppConfig *config) {
  (void)config;
  int win_h = 15, win_w = 70;
  WINDOW *win = create_popup(win_h, win_w, "VIEW_TITLE");

  char buffer[100];
  snprintf(buffer, sizeof(buffer), get_translation("VIEW_TASK_ID"), task->id);
  mvwprintw(win, 3, 2, "%s", buffer);

  snprintf(buffer, sizeof(buffer), get_translation("VIEW_TASK_STATUS"),
            task->concluida ? get_translation("STATUS_DONE")
                            : get_translation("STATUS_PENDING"));
  mvwprintw(win, 4, 2, "%s", buffer);

  mvwprintw(win, 6, 2, "%s", get_translation("VIEW_FULL_DESC"));

  // Lógica de quebra de linha (word wrap)
  int max_width = win_w - 4; // 2 de margem + 2 de borda
  auto_free_ptr char *wrapped_desc = word_wrap(task->descricao, max_width);

  if (wrapped_desc) {
    mvwprintw(win, 8, 2, "%s", wrapped_desc);
  } else {
    // Fallback para o comportamento antigo se a alocação falhar
    char truncated_desc[MAX_DESCRICAO];
    truncar_por_largura(truncated_desc, sizeof(truncated_desc), task->descricao,
                        max_width);
    mvwprintw(win, 8, 2, "%s", truncated_desc);
  }

  wrefresh(win);
  wgetch(win);
  delwin(win);
}

void ui_display_clock(WINDOW *win, const char *time_string) {
  if (!win) return;
  int win_h, win_w;
  getmaxyx(win, win_h, win_w);
  if (win_w <= 0) return;

  // Limpa a área do relógio para evitar sobreposição
  char empty_line[win_w];
  int len = snprintf(empty_line, sizeof(empty_line), "%*s", win_w - 2, "");
  if ((size_t)len >= sizeof(empty_line)) {
    // Handle error
  }
  mvwprintw(win, win_h - 2, 1, "%s", empty_line);

  // Desenha o relógio centralizado
  mvwprintw(win, win_h - 2, (win_w - strlen(time_string)) / 2, "%s", time_string);
  wnoutrefresh(win);
}

int ui_handle_sidebar_click(MEVENT event, AppView current_view) {
    if (event.y >= 4) {
        int item_index = event.y - 4;
        if (current_view == VIEW_MAIN) {
            const char *keys[] = {"1", "2", "3", "4", "5", "6", "u", "y", "t", "7"};
            if ((size_t)item_index < sizeof(keys) / sizeof(keys[0])) {
                return keys[item_index][0];
            }
        } else { // VIEW_TRASH
            const char *keys[] = {"r", "d", "q"};
            if ((size_t)item_index < sizeof(keys) / sizeof(keys[0])) {
                return keys[item_index][0];
            }
        }
    }
    return 0; // Nenhum item de menu clicado
}
