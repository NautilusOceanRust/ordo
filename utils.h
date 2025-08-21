#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>
#include <stddef.h>

// --- Gerenciamento de Memória Automático (Estilo Smart Pointer) ---
// Atributo do GCC/Clang para chamar uma função de limpeza quando a variável sai do escopo.
// Isso ajuda a evitar vazamentos de memória, garantindo que `free` seja chamado.

// Função de limpeza que libera a memória apontada por um ponteiro.
static inline void auto_free(void *p) {
  free(*(void**)p);
}

// Macro para definir uma variável de ponteiro que será liberada automaticamente.
#define auto_free_ptr __attribute__((cleanup(auto_free)))

// --- Declarações de Funções Utilitárias ---

#define NOME_APLICACAO "ordo"
#define NOME_BANCO_DE_DADOS "tasks.db"
#define MAX_PATH 4096

// Funções de manipulação de strings e caminhos
void truncar_por_largura(char *dest, size_t dest_size, const char *src,
                         int max_width);
int obterCaminhoBancoDeDados(char *buffer, size_t tamanho_buffer);
char *trim(const char *str);
char *word_wrap(const char *src, int max_width);
char *path_join(const char *base, const char *leaf);
void generate_line(char *buffer, size_t buffer_size, const char *ch,
                   int length);
int str_width(const char *str);

// --- Funções seguras para manipulação de strings e memória ---
#include <stdarg.h>
#include <string.h>

int safe_snprintf(char *str, size_t size, const char *format, ...);
void *safe_memcpy(void *dest, const void *src, size_t n);
void *safe_memmove(void *dest, const void *src, size_t n);

#endif // UTILS_H