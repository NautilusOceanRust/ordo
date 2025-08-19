#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 700
#endif

#include "utils.h"
#include "platform_utils.h"
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <wctype.h>

#ifdef _WIN32
#include <shlobj.h>
#include <windows.h>
#else
#include <sys/stat.h>
#include <unistd.h>
#endif

/**
 * @brief Remove espaços em branco do início e do fim de uma string (UTF-8 safe).
 * @param str A string de entrada (constante).
 * @return Uma nova string alocada com o resultado, ou NULL se a alocação
 * falhar. O chamador deve liberar a memória retornada.
 */
char *trim(const char *str) {
  if (!str)
    return NULL;

  size_t len = strlen(str);
  wchar_t *w_str = malloc((len + 1) * sizeof(wchar_t));
  if (!w_str)
    return NULL;
  mbstowcs(w_str, str, len + 1);

  const wchar_t *start = w_str;
  while (*start && iswspace(*start)) {
    start++;
  }

  const wchar_t *end = w_str + wcslen(w_str) - 1;
  while (end > start && iswspace(*end)) {
    end--;
  }

  size_t new_len = (end - start) + 1;
  wchar_t *w_trimmed = malloc((new_len + 1) * sizeof(wchar_t));
  if (!w_trimmed) {
    free(w_str);
    return NULL;
  }

  wcsncpy(w_trimmed, start, new_len);
  w_trimmed[new_len] = L'\0';

  size_t mb_len = wcstombs(NULL, w_trimmed, 0);
  char *trimmed_str = malloc(mb_len + 1);
  
  if (!trimmed_str) {
    free(w_str);
    free(w_trimmed);
    return NULL; // Retorna em caso de falha de alocação
  }

  wcstombs(trimmed_str, w_trimmed, mb_len + 1);

  free(w_str);
  free(w_trimmed);

  return trimmed_str;
}

void truncar_por_largura(char *dest, size_t dest_size, const char *src,
                         int max_width) {
  if (dest_size == 0)
    return;
  dest[0] = '\0';

  const wchar_t *ellipsis = L"...";
  size_t src_len = strlen(src);

  // Aloca espaço para a string + elipse + terminador nulo
  wchar_t *w_src = malloc((src_len + wcslen(ellipsis) + 1) * sizeof(wchar_t));
  if (!w_src) {
    return;
  } // Falha de alocação
  mbstowcs(w_src, src, src_len + 1);

  int total_width = wcswidth(w_src, -1);

  // Se a string já cabe, apenas copie
  if (total_width <= max_width) {
    snprintf(dest, dest_size, "%s", src);
    free(w_src);
    return;
  }

  // Trunca e adiciona elipse
  int ellipsis_width = wcswidth(ellipsis, -1);
  int target_width = max_width - ellipsis_width;
  if (target_width < 0)
    target_width = 0;

  int current_width = 0;
  size_t end_index = 0;
  while (w_src[end_index] != L'\0') {
    int char_width = wcwidth(w_src[end_index]);
    if (char_width < 0)
      char_width = 1;

    if (current_width + char_width > target_width) {
      break;
    }
    current_width += char_width;
    end_index++;
  }

  // Constrói a string final truncada com elipse
  w_src[end_index] = L'\0'; // Trunca a string wide
  wcscat(w_src, ellipsis);  // Adiciona a elipse

  // Converte de volta para multibyte (UTF-8) e copia para o destino
  wcstombs(dest, w_src, dest_size);
  dest[dest_size - 1] = '\0';

  free(w_src);
}

int obterCaminhoBancoDeDados(char *buffer, size_t tamanho_buffer) {
  char *caminho_app = platform_get_config_dir();
  if (!caminho_app) {
    fprintf(stderr, "Erro: Nao foi possivel obter o diretorio de configuracao.\n");
    return 1;
  }


  if (!platform_create_dir_recursive(caminho_app)) {
    fprintf(stderr, "Erro ao criar diretorio '%s': %s\n", caminho_app,
            strerror(errno));
    free(caminho_app);
    return 1;
  }


  char *db_path = path_join(caminho_app, NOME_BANCO_DE_DADOS);
  free(caminho_app);

  if (!db_path) {
      fprintf(stderr, "Erro ao construir o caminho para o banco de dados.\n");
      return 1;
  }

  size_t db_path_len = strlen(db_path);
  if (db_path_len >= tamanho_buffer) {
      fprintf(stderr, "Erro: O caminho para o banco de dados e muito longo.\n");
      free(db_path);
      return 1;
  }

  snprintf(buffer, tamanho_buffer, "%s", db_path);
  
  free(db_path);
  return 0;
}

char *word_wrap(const char *src, int max_width) {
  if (!src)
    return NULL;

  size_t src_len = strlen(src);
  wchar_t *w_src = malloc((src_len + 1) * sizeof(wchar_t));
  if (!w_src)
    return NULL;
  mbstowcs(w_src, src, src_len + 1);

  // Aloca um buffer de resultado que é certamente grande o suficiente
  wchar_t *w_dest = malloc((wcslen(w_src) * 2 + 1) * sizeof(wchar_t));
  if (!w_dest) {
    free(w_src);
    return NULL;
  }
  w_dest[0] = L'\0';

  wchar_t *p = w_src;
  wchar_t *line_start = p;
  int current_width = 0;

  while (*p) {
    // Encontra o fim da próxima palavra ou o fim da string
    wchar_t *word_end = p;
    int word_width = 0;
    while (*word_end && !iswspace(*word_end)) {
      word_width += wcwidth(*word_end);
      word_end++;
    }

    // Se a palavra em si for maior que a largura da linha, quebre-a
    if (word_width > max_width) {
      if (current_width > 0) {
        wcscat(w_dest, L"\n");
        current_width = 0;
      }
      wchar_t *char_p = p;
      while (char_p < word_end) {
        int char_width = wcwidth(*char_p);
        if (current_width + char_width > max_width) {
          wcscat(w_dest, L"\n");
          current_width = 0;
        }
        wcsncat(w_dest, char_p, 1);
        current_width += char_width;
        char_p++;
      }
      p = word_end;
      current_width = 0; // A proxima palavra comeca em uma nova linha
      line_start = p;
      // Adiciona espaco se houver
      while (*p && iswspace(*p)) {
        p++;
      }
      continue;
    }

    // Verifica se a palavra cabe na linha atual
    if (current_width + (p == line_start ? 0 : 1) + word_width > max_width) {
      wcscat(w_dest, L"\n");
      current_width = 0;
      line_start = p;
    }

    // Adiciona a palavra ao resultado
    if (current_width > 0) {
      wcscat(w_dest, L" ");
      current_width++;
    }

    wcsncat(w_dest, p, word_end - p);
    current_width += word_width;
    p = word_end;

    // Pula os espacos em branco
    while (*p && iswspace(*p)) {
      p++;
    }
  }

  free(w_src);

  // Converte a string wide resultante de volta para multibyte
  size_t dest_len = wcstombs(NULL, w_dest, 0);
  char *dest = malloc(dest_len + 1);
  if (dest) {
    wcstombs(dest, w_dest, dest_len + 1);
  }

  free(w_dest);
  return dest;
}

char *path_join(const char *base, const char *leaf) {
    if (!base || !leaf) return NULL;

    const char separator =
#ifdef _WIN32
        '\\';
#else
        '/';
#endif

    size_t base_len = strlen(base);
    size_t leaf_len = strlen(leaf);

    // Remove a barra final da base, se houver
    if (base_len > 0 && base[base_len - 1] == separator) {
        base_len--;
    }

    // Calcula o tamanho total e aloca memória
    size_t total_len = base_len + 1 + leaf_len + 1; // +1 para a barra, +1 para o nulo
    char *result = malloc(total_len);
    if (!result) return NULL;

    // Constrói o caminho final
    memcpy(result, base, base_len);
    result[base_len] = separator;
    memcpy(result + base_len + 1, leaf, leaf_len + 1); // +1 para copiar o nulo

    return result;
}

