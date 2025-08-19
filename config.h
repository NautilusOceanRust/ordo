#ifndef CONFIG_H
#define CONFIG_H

#include <ncurses.h>

// Estrutura para armazenar as configuracoes de cores e outras preferencias.
typedef struct {
  char lang[10];
  char theme_name[100];

  // Pares de cores
  short color_pair_header;
  short color_pair_success;
  short color_pair_error;
  short color_pair_status_done;
  short color_pair_status_pending;
  short color_pair_task_done;
  short color_pair_task_pending;
} AppConfig;

// Carrega a configuracao do arquivo ~/.config/ordo/config.ini
// Se o arquivo nao existir, carrega os valores padrao.
void config_load(AppConfig *config);

// Salva a configuracao no arquivo ~/.config/ordo/config.ini
void config_save(const AppConfig *config);

// Inicializa os pares de cores do ncurses com base na configuracao carregada.
void config_init_color_pairs(const AppConfig *config);

// Define as configuracoes padrao.
void set_default_config(AppConfig *config);

#endif // CONFIG_H
