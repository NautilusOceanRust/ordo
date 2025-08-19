project('ordo', 'c',
  version : '0.1.0',
  default_options : ['c_std=c11', 'warning_level=2', 'b_gdb=true'])

# Argumentos extras de compilação
add_project_arguments('-pedantic', language: 'c')
add_project_arguments('-DPROJECT_SOURCE_ROOT="@0@"'.format(meson.project_source_root()), language: 'c')

# Dependências via pkg-config
sqlite3_dep = dependency('sqlite3', required: true)
cjson_dep = dependency('libcjson', required: true)

# Dependências específicas de plataforma
if host_machine.system() != 'windows'
  ncursesw_dep = dependency('ncursesw', required: true)
else
  # No Windows, o pdcurses é normalmente vinculado de outra forma
  # Esta dependência pode precisar de ajuste específico para Windows
  ncursesw_dep = dependency('pdcurses', required: false) # Exemplo
endif

# Arquivos fonte do executável principal
  sources = [
    'main.c',
    'config.c',
    'database.c',
    'ui.c',
    'utils.c',
    'i18n.c',
    'theme.c',
    'settings_ui.c',
    'platform_utils.c',
    'task_list.c',
    'app.c',
    'undo_manager.c',
    'input.c'
  ]

# Executável principal
executable('ordo', sources,
  dependencies : [sqlite3_dep, ncursesw_dep],
  install : true)

# Ferramenta de conversão standalone
executable('ordo-importer', 'ordo-importer.c',
  dependencies : [sqlite3_dep, cjson_dep],
  install : true)


# --- Testes Unitários ---
cmocka_dep = dependency('cmocka', required: get_option('tests'))

# Instalação de arquivos de dados
install_data(
    'dracula.theme', 'solarized.theme',
    install_dir: get_option('datadir') / 'ordo' / 'themes'
)

install_data(
    'langs/de.lang', 'langs/en.lang', 'langs/es.lang', 'langs/it.lang',
    'langs/ja.lang', 'langs/la.lang', 'langs/pt.lang', 'langs/ru.lang',
    install_dir: get_option('datadir') / 'ordo' / 'langs'
)

# --- clang-tidy ---
run_target(
    'clang-tidy',
    command: ['sh', join_paths(meson.project_source_root(), 'run-clang-tidy.sh')],
    depends: []
)

if cmocka_dep.found()
  # Testes para o módulo utils
  utils_test_sources = [
    'tests/test_utils.c',
    'utils.c',
    'platform_utils.c'
  ]

  utils_test_exe = executable('utils_test_runner',
    utils_test_sources,
    dependencies: [cmocka_dep]
  )

  test('utils_tests', utils_test_exe)

  # Testes para o módulo database
  db_test_sources = [
    'tests/test_database.c',
    'database.c',
    'task_list.c',
    'utils.c',
    'platform_utils.c'
  ]

  db_test_exe = executable('db_test_runner',
    db_test_sources,
    dependencies: [cmocka_dep, sqlite3_dep]
  )

  test('database_tests', db_test_exe)

  # Testes para o módulo config
  config_test_sources = [
    'tests/test_config.c',
    'config.c',
    'utils.c',
    'theme.c',
    'platform_utils.c'
  ]

  config_test_exe = executable('config_test_runner',
    config_test_sources,
    dependencies: [cmocka_dep, ncursesw_dep]
  )

  test('config_tests', config_test_exe)

  # Testes para o módulo theme
  theme_test_sources = [
    'tests/test_theme.c',
    'theme.c',
    'utils.c',
    'platform_utils.c'
  ]

  theme_test_exe = executable('theme_test_runner',
    theme_test_sources,
    dependencies: [cmocka_dep]
  )

  test('theme_tests', theme_test_exe)
endif


# --- Doxygen Documentation ---
doxygen_prog = find_program('doxygen', required: false)
if doxygen_prog.found()
  run_target(
    'doxygen',
    command: [doxygen_prog, join_paths(meson.project_source_root(), 'Doxyfile')],
    depends: []
  )
endif

