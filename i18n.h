#ifndef I18N_H
#define I18N_H

// Interface for the internationalization (i18n) system.

/**
 * @brief Loads translations from the appropriate .lang file.
 * Tries to detect the system language. If a translation file is not found,
 * it defaults to 'en.lang'.
 */
void i18n_init(const char *initial_lang);
void i18n_load_language(const char *lang);

/**
 * @brief Frees the memory allocated for translations.
 * Should be called at the end of the program.
 */
void i18n_destroy();

/**
 * @brief Gets the translation string for a given key.
 *
 * @param key The translation key (e.g., "MENU_ADD").
 * @return const char* The translated string or the key itself if not
 * found.
 */
const char *get_translation(const char *key);

#endif // I18N_H
