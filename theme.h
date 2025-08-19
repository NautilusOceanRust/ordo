#ifndef THEME_H
#define THEME_H

#include <ncurses.h>

// Define a estrutura de um tema de cores.
// As cores sao as do ncurses (COLOR_RED, COLOR_BLUE, etc.).
typedef struct {
  const char *name;
  short header_fg;
  short header_bg;
  short done_fg;
  short done_bg;
  short pending_fg;
  short pending_bg;
  short error_fg;
  short error_bg;
  short success_fg;
  short success_bg;
  short task_done_fg;
  short task_done_bg;
  short task_pending_fg;
  short task_pending_bg;
} OrdoTheme;

// Declaração do array global de temas.
extern const OrdoTheme ordo_themes[];
// Número total de temas disponíveis.
extern const int num_ordo_themes;

// Encontra um tema pelo nome. Retorna NULL se não for encontrado.
const OrdoTheme *theme_get_by_name(const char *name);

// Carrega temas de um diretorio externo.
void theme_load_external();

// Libera a memoria usada pelos temas externos.
void theme_free_external();

#endif // THEME_H
