#define _POSIX_C_SOURCE 200809L
#include "i18n.h"
#include "utils.h" // For the trim function
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#endif

#define MAX_TRANSLATIONS 100
#define MAX_KEY_LENGTH 50
#define MAX_VALUE_LENGTH 200

typedef struct {
  char *key;
  char *value;
} Translation;

static Translation *translations = NULL;
static int num_translations = 0;

void get_system_lang(char *lang_buf, size_t buf_size) {
#ifdef _WIN32
  LANGID langID = GetUserDefaultUILanguage();
  switch (langID & 0xff) {
  case LANG_PORTUGUESE:
    safe_snprintf(lang_buf, buf_size, "pt");
    break;
  case LANG_ENGLISH:
    safe_snprintf(lang_buf, buf_size, "en");
    break;
  case LANG_SPANISH:
    safe_snprintf(lang_buf, buf_size, "es");
    break;
  case LANG_GERMAN:
    safe_snprintf(lang_buf, buf_size, "de");
    break;
  case LANG_ITALIAN:
    safe_snprintf(lang_buf, buf_size, "it");
    break;
  case LANG_JAPANESE:
    safe_snprintf(lang_buf, buf_size, "ja");
    break;
  case LANG_RUSSIAN:
    safe_snprintf(lang_buf, buf_size, "ru");
    break;
  default:
    safe_snprintf(lang_buf, buf_size, "en");
    break;
  }
#else
  const char *lang_env = getenv("LANG");
  if (lang_env && strlen(lang_env) >= 2) {
    safe_snprintf(lang_buf, 3, "%.2s", lang_env);
  } else {
    safe_snprintf(lang_buf, buf_size, "en"); // Default
  }
#endif
}

void i18n_load_language(const char *lang) {
  char lang_filename[256];
  int len = safe_snprintf(lang_filename, sizeof(lang_filename), "%s.lang", lang);
  if ((size_t)len >= sizeof(lang_filename)) {
    fprintf(stderr, "Error: lang string is too long.\n");
    return;
  }

  char *langs_dir = path_join(PROJECT_SOURCE_ROOT, "langs");
  if (!langs_dir)
    return;

  char *filepath = path_join(langs_dir, lang_filename);
  if (!filepath) {
    free(langs_dir);
    return;
  }

  FILE *file = fopen(filepath, "r");
  if (!file) {
    fprintf(stderr,
            "Warning: Language file '%s' not found. Using 'en.lang'.\n",
            lang_filename);
    free(filepath); // Free the old path
    filepath = path_join(langs_dir, "en.lang");
    if (!filepath) {
      free(langs_dir);
      return;
    }
    file = fopen(filepath, "r");
    if (!file) {
      fprintf(stderr, "Warning: Could not load any language file.\n");
      free(langs_dir);
      free(filepath);
      return;
    }
  }
  free(langs_dir);
  free(filepath);

  int capacity = 100;
  Translation *temp_translations = malloc(capacity * sizeof(Translation));
  if (!temp_translations) {
    fclose(file);
    return;
  }

  int temp_num_translations = 0;
  char line[4096];
  while (fgets(line, sizeof(line), file)) {
    if (line[0] == '#' || line[0] == '\n' || line[0] == '\r')
      continue;

    char *separator = strchr(line, '=');
    if (separator) {
      *separator = '\0';
      char *key = line;
      char *value = separator + 1;

      // Remove newlines and extra spaces
      value[strcspn(value, "\n\r")] = 0;
      char *trimmed_key = trim(key);
      char *trimmed_value = trim(value);

      if (trimmed_key && trimmed_value && strlen(trimmed_key) > 0) {
        if (temp_num_translations >= capacity) {
          capacity *= 2;
          Translation *new_translations =
              realloc(temp_translations, capacity * sizeof(Translation));
          if (!new_translations) {
            // Free memory before exiting
            free(trimmed_key);
            free(trimmed_value);
            for (int i = 0; i < temp_num_translations; i++) {
              free(temp_translations[i].key);
              free(temp_translations[i].value);
            }
            free(temp_translations);
            fclose(file);
            return;
          }
          temp_translations = new_translations;
        }

        temp_translations[temp_num_translations].key = trimmed_key;
        temp_translations[temp_num_translations].value = trimmed_value;
        temp_num_translations++;
      } else {
        // Free memory if the key or value are invalid
        free(trimmed_key);
        free(trimmed_value);
      }
    }
  }
  fclose(file);

  i18n_destroy();
  translations = temp_translations;
  num_translations = temp_num_translations;
}

void i18n_init(const char *initial_lang) {
  if (initial_lang && strlen(initial_lang) > 0) {
    i18n_load_language(initial_lang);
  } else {
    char system_lang[3];
    get_system_lang(system_lang, sizeof(system_lang));
    i18n_load_language(system_lang);
  }
}

// The destroy function is not strictly necessary with static allocation,
// but it's good practice for the future.
void i18n_destroy() {
  if (translations) {
    for (int i = 0; i < num_translations; i++) {
      free(translations[i].key);
      free(translations[i].value);
    }
    free(translations);
    translations = NULL;
  }
  num_translations = 0;
}

const char *get_translation(const char *key) {
  if (!translations)
    return key;
  for (int i = 0; i < num_translations; i++) {
    if (strcmp(translations[i].key, key) == 0) {
      return translations[i].value;
    }
  }
  return key;
}
