#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 700
#endif

#include "utils.h"
#include "database.h"
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
 * @brief Removes whitespace from the beginning and end of a string (UTF-8 safe).
 * @param str The input string (constant).
 * @return A newly allocated string with the result, or NULL if allocation fails.
 * The caller must free the returned memory.
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
    return NULL; // Return in case of allocation failure
  }

  wcstombs(trimmed_str, w_trimmed, mb_len + 1);

  free(w_str);
  free(w_trimmed);

  return trimmed_str;
}

void truncate_by_width(char *dest, size_t dest_size, const char *src,
                         int max_width) {
  if (dest_size == 0)
    return;
  dest[0] = '\0';

  const wchar_t *ellipsis = L"...";
  size_t src_len = strlen(src);

  // Allocates space for the string + ellipsis + null terminator
  wchar_t *w_src = malloc((src_len + wcslen(ellipsis) + 1) * sizeof(wchar_t));
  if (!w_src) {
    return;
  } // Allocation failure
  mbstowcs(w_src, src, src_len + 1);

  int total_width = wcswidth(w_src, -1);

  // If the string already fits, just copy it
  if (total_width <= max_width) {
    safe_snprintf(dest, dest_size, "%s", src);
    free(w_src);
    return;
  }

  // Truncates and adds ellipsis
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

  // Builds the final truncated string with ellipsis
  w_src[end_index] = L'\0'; // Truncates the wide string
  wcscat(w_src, ellipsis);  // Adds the ellipsis

  // Converts back to multibyte (UTF-8) and copies to the destination
  wcstombs(dest, w_src, dest_size);
  dest[dest_size - 1] = '\0';

  free(w_src);
}

int obterCaminhoBancoDeDados(char *buffer, size_t buffer_size) {
  char *app_path = platform_get_config_dir();
  if (!app_path) {
    fprintf(stderr, "Error: Could not get the configuration directory.\n");
    return 1;
  }


  if (!platform_create_dir_recursive(app_path)) {
    fprintf(stderr, "Error creating directory '%s': %s\n", app_path,
            strerror(errno));
    free(app_path);
    return 1;
  }


  char *db_path = path_join(app_path, NOME_BANCO_DE_DADOS);
  free(app_path);

  if (!db_path) {
      fprintf(stderr, "Error building the path to the database.\n");
      return 1;
  }

  size_t db_path_len = strlen(db_path);
  if (db_path_len >= buffer_size) {
      fprintf(stderr, "Error: The path to the database is too long.\n");
      free(db_path);
      return 1;
  }

  safe_snprintf(buffer, buffer_size, "%s", db_path);
  
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

  // Allocates a result buffer that is certainly large enough
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
    // Finds the end of the next word or the end of the string
    wchar_t *word_end = p;
    int word_width = 0;
    while (*word_end && !iswspace(*word_end)) {
      word_width += wcwidth(*word_end);
      word_end++;
    }

    // If the word itself is larger than the line width, break it
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
      current_width = 0; // The next word starts on a new line
      line_start = p;
      // Adds space if there is any
      while (*p && iswspace(*p)) {
        p++;
      }
      continue;
    }

    // Checks if the word fits on the current line
    if (current_width + (p == line_start ? 0 : 1) + word_width > max_width) {
      wcscat(w_dest, L"\n");
      current_width = 0;
      line_start = p;
    }

    // Adds the word to the result
    if (current_width > 0) {
      wcscat(w_dest, L" ");
      current_width++;
    }

    wcsncat(w_dest, p, word_end - p);
    current_width += word_width;
    p = word_end;

    // Skips the whitespace
    while (*p && iswspace(*p)) {
      p++;
    }
  }

  free(w_src);

  // Converts the resulting wide string back to multibyte
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

    // Removes the final slash from the base, if any
    if (base_len > 0 && base[base_len - 1] == separator) {
        base_len--;
    }

    // Calculates the total size and allocates memory
    size_t total_len = base_len + 1 + leaf_len + 1; // +1 for the slash, +1 for the null
    char *result = malloc(total_len);
    if (!result) return NULL;

    // Builds the final path
    safe_memcpy(result, base, base_len);
    result[base_len] = separator;
    safe_memcpy(result + base_len + 1, leaf, leaf_len + 1); // +1 to copy the null

    return result;
}

int safe_snprintf(char *str, size_t size, const char *format, ...) {
    if (size == 0) return -1; // Error: zero size buffer

    va_list args;
    va_start(args, format);
    int ret = vsnprintf(str, size, format, args);
    va_end(args);

    if (ret < 0) {
        // Encoding error
        str[0] = '\0';
        return -1;
    }
    if ((size_t)ret >= size) {
        // Truncation occurred
        str[size - 1] = '\0';
        return (int)(size - 1);
    }
    return ret;
}

void *safe_memcpy(void *dest, const void *src, size_t n) {
    if (dest == NULL || src == NULL) {
        return NULL;
    }
    return memcpy(dest, src, n);
}

void *safe_memmove(void *dest, const void *src, size_t n) {
    if (dest == NULL || src == NULL) {
        return NULL;
    }
    return memmove(dest, src, n);
}