#ifndef UI_H
#define UI_H

#include "app.h" // Inclui app.h para AppView
#include "config.h"
#include "error.h"
#include "task.h"
#include <ncurses.h> // Inclui ncurses para o tipo WINDOW
#include <stdbool.h>

// --- Funções de Ciclo de Vida da UI ---
void ui_init(const AppConfig *config);
void ui_destroy();

// --- Funções de Desenho e Layout ---
void ui_draw_layout();
void ui_display_tasks(WINDOW *win, const Tarefa *tasks, int num_tasks,
                      const AppConfig *config, int scroll_offset,
                      int current_selection, AppView current_view);
void ui_display_menu(WINDOW *win, const AppConfig *config,
                     AppView current_view);
void ui_display_clock(WINDOW *win, const char *time_string);
void ui_show_message(const char *translation_key, bool is_error,
                     const AppConfig *config);

// --- Funções de Interação ---
void ui_display_full_task(const Tarefa *task, const AppConfig *config);
OrdoResult ui_add_task(const AppConfig *config, char **new_description_out);
OrdoResult ui_edit_task(const AppConfig *config,
                        char **new_description_out);
bool ui_confirm_action(const char *translation_key, const char *title_key,
                       const AppConfig *config);

// --- Funções de Tratamento de Entrada ---
int ui_handle_sidebar_click(MEVENT event, AppView current_view);

#endif // UI_H