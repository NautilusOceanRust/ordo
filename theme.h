#ifndef THEME_H
#define THEME_H

#include <ncurses.h>

// Defines the structure of a color theme.
// The colors are ncurses colors (COLOR_RED, COLOR_BLUE, etc.).
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

// Declaration of the global themes array.
extern const OrdoTheme ordo_themes[];
// Total number of available themes.
extern const int num_ordo_themes;

// Finds a theme by name. Returns NULL if not found.
const OrdoTheme *theme_get_by_name(const char *name);

// Loads themes from an external directory.
void theme_load_external();

// Frees the memory used by external themes.
void theme_free_external();

#endif // THEME_H
