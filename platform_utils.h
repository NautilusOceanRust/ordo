#ifndef PLATFORM_UTILS_H
#define PLATFORM_UTILS_H

#include <stdbool.h>

// Retorna o caminho para o diretório de configuração específico da plataforma.
// O chamador é responsável por liberar a memória retornada com free().
char *platform_get_config_dir(void);
char *platform_get_data_dir(void);

// Cria um diretório e todos os seus pais, se não existirem.
// Retorna true em caso de sucesso, false em caso de falha.
bool platform_create_dir_recursive(const char *path);

// Estrutura para armazenar uma lista de nomes de arquivos.
typedef struct {
  char **files;
  int count;
} FileList;

// Lista arquivos em um diretório que correspondem a uma extensão.
// O chamador é responsável por liberar a memória da FileList com
// platform_free_file_list.
FileList platform_list_files_in_dir(const char *dir_path,
                                    const char *extension);

// Libera a memória alocada por platform_list_files_in_dir.
void platform_free_file_list(FileList *file_list);

#endif // PLATFORM_UTILS_H
