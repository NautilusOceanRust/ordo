#include "app.h"
#include "input.h"
#include "ui.h"
#include <ncurses.h>
#include <stdio.h>
#include <time.h>

// Function to update the screen scrolling logic
static void update_scroll(AppState *app) {
  extern WINDOW *content_win;
  int content_height = getmaxy(content_win) - 2; // Top and bottom border

  if (app->current_selection < app->scroll_offset) {
    app->scroll_offset = app->current_selection;
  }
  if (app->current_selection >= app->scroll_offset + content_height) {
    app->scroll_offset = app->current_selection - content_height + 1;
  }
}

// Function to redraw the entire interface
static void redraw_ui(AppState *app) {
  extern WINDOW *sidebar_win, *content_win;

  ui_draw_layout();
  ui_display_menu(sidebar_win, &app->config, app->current_view);
  ui_display_tasks(content_win, app->task_list.tasks, app->task_list.count,
                   &app->config, app->scroll_offset, app->current_selection,
                   app->current_view);

  // Display the clock
  time_t now = time(NULL);
  struct tm *local_time = localtime(&now);
  char time_str[6];
  strftime(time_str, sizeof(time_str), "%H:%M", local_time);
  ui_display_clock(sidebar_win, time_str);

  doupdate();
}

int main() {
  AppState app;
  if (!app_init(&app)) {
    fprintf(stderr, "Failed to initialize the application.\n");
    return 1;
  }

  while (!app.should_exit) {
    if (app.refresh_tasks) {
      task_list_clear(&app.task_list);
      if (app.current_view == VIEW_MAIN) {
        database_load_tasks(&app.db, &app.task_list);
      } else { // VIEW_TRASH
        database_load_deleted_tasks(&app.db, &app.task_list);
      }
      app.refresh_tasks = false;
      if (app.current_selection >= app.task_list.count) {
        app.current_selection =
            app.task_list.count > 0 ? app.task_list.count - 1 : 0;
      }
    }

    update_scroll(&app);
    redraw_ui(&app);

    int key = getch();
    input_handle(&app, key);
  }

  app_destroy(&app);
  return 0;
}
