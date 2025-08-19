#ifndef I18N_H
#define I18N_H

// Interface para o sistema de internacionalização (i18n).

/**
 * @brief Carrega as traduções do arquivo .lang apropriado.
 * Tenta detectar o idioma do sistema. Se não encontrar um arquivo de tradução,
 * usa 'en.lang' como padrão.
 */
void i18n_init(const char *initial_lang);
void i18n_load_language(const char *lang);

/**
 * @brief Libera a memória alocada para as traduções.
 * Deve ser chamada no final do programa.
 */
void i18n_destroy();

/**
 * @brief Obtém a string de tradução para uma determinada chave.
 *
 * @param key A chave da tradução (ex: "MENU_ADD").
 * @return const char* A string traduzida ou a própria chave se não for
 * encontrada.
 */
const char *get_translation(const char *key);

#endif // I18N_H
