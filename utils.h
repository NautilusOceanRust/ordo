#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>
#include <stddef.h>

// --- Automatic Memory Management (Smart Pointer Style) ---
// GCC/Clang attribute to call a cleanup function when the variable goes out of scope.
// This helps prevent memory leaks by ensuring `free` is called.

// Cleanup function that frees the memory pointed to by a pointer.
static inline void auto_free(void *p) {
  free(*(void**)p);
}

// Macro to define a pointer variable that will be automatically freed.
#define auto_free_ptr __attribute__((cleanup(auto_free)))

// --- Utility Function Declarations ---

#define NOME_APLICACAO "ordo"
#define NOME_BANCO_DE_DADOS "tasks.db"
#define MAX_PATH 4096

// String and path manipulation functions
void truncar_por_largura(char *dest, size_t dest_size, const char *src,
                         int max_width);
int obterCaminhoBancoDeDados(char *buffer, size_t tamanho_buffer);
char *trim(const char *str);
char *word_wrap(const char *src, int max_width);
char *path_join(const char *base, const char *leaf);
void generate_line(char *buffer, size_t buffer_size, const char *ch,
                   int length);
int str_width(const char *str);

// --- Safe string and memory manipulation functions ---
#include <stdarg.h>
#include <string.h>

int safe_snprintf(char *str, size_t size, const char *format, ...);
void *safe_memcpy(void *dest, const void *src, size_t n);
void *safe_memmove(void *dest, const void *src, size_t n);

#endif // UTILS_H