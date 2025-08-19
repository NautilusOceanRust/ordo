#ifndef SETTINGS_UI_H
#define SETTINGS_UI_H

#include "config.h"

/**
 * @brief Exibe o menu de configurações.
 */
void settings_ui_show(AppConfig *config);

const char *get_translated_string(const char *key);

#endif // SETTINGS_UI_H
