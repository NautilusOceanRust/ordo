project('ordo', 'c',
  version : '0.1.0',
  default_options : ['c_std=c11', 'warning_level=2', 'b_gdb=true'])

# --- Extra Compiler Arguments ---
add_project_arguments('-pedantic', language: 'c')
add_project_arguments('-DPROJECT_SOURCE_ROOT="@0@"'.format(meson.project_source_root()), language: 'c')

# --- MSYS2 Environment Detection ---
msys2_env = get_option('msys2_env')
if msys2_env == 'auto'
  res = run_command('sh', '-c', 'echo $MSYSTEM')
  if res.returncode() == 0
    msys2_env = res.stdout().strip().to_lower()
  else
    msys2_env = ''
  endif
endif

# --- Dependencies ---
sqlite3_dep = dependency('sqlite3', required: true)
cjson_dep = dependency('cjson', required: true, fallback: ['libcjson', 'cjson_dep'])

if host_machine.system() == 'windows'
  # On Windows, we use pdcurses instead of ncurses
  ncursesw_dep = dependency('pdcurses', required: true, fallback: ['pdcurses', 'pdcurses_dep'])
  if msys2_env != ''
    message('Building for MSYS2 environment: ' + msys2_env)
  endif
else
  ncursesw_dep = dependency('ncursesw', required: true)
endif

# --- Main Executable ---
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

executable('ordo', sources,
  dependencies : [sqlite3_dep, ncursesw_dep, cjson_dep],
  install : true)

# --- Importer Tool ---
executable('ordo-importer', 'ordo-importer.c',
  dependencies : [sqlite3_dep, cjson_dep],
  install : true)

# --- Unit Tests ---
if get_option('tests')
  cmocka_dep = dependency('cmocka', required: true)

  # Utils tests
  utils_test_exe = executable('utils_test_runner', [
    'tests/test_utils.c',
    'utils.c',
    'platform_utils.c'
  ], dependencies: [cmocka_dep])
  test('utils_tests', utils_test_exe)

  # Database tests
  db_test_exe = executable('db_test_runner', [
    'tests/test_database.c',
    'database.c',
    'task_list.c',
    'utils.c',
    'platform_utils.c'
  ], dependencies: [cmocka_dep, sqlite3_dep])
  test('database_tests', db_test_exe)

  # Config tests
  config_test_exe = executable('config_test_runner', [
    'tests/test_config.c',
    'config.c',
    'utils.c',
    'theme.c',
    'platform_utils.c'
  ], dependencies: [cmocka_dep, ncursesw_dep])
  test('config_tests', config_test_exe)

  # Theme tests
  theme_test_exe = executable('theme_test_runner', [
    'tests/test_theme.c',
    'theme.c',
    'utils.c',
    'platform_utils.c'
  ], dependencies: [cmocka_dep])
  test('theme_tests', theme_test_exe)
endif

# --- Data Installation ---
install_data(
    'dracula.theme', 'solarized.theme',
    install_dir: get_option('datadir') / 'ordo' / 'themes'
)

install_data(
    'langs/de.lang', 'langs/en.lang', 'langs/es.lang', 'langs/it.lang',
    'langs/ja.lang', 'langs/la.lang', 'langs/pt.lang', 'langs/ru.lang',
    install_dir: get_option('datadir') / 'ordo' / 'langs'
)

# --- Development Tools ---
run_target(
    'clang-tidy',
    command: ['sh', join_paths(meson.project_source_root(), 'run-clang-tidy.sh')]
)

doxygen_prog = find_program('doxygen', required: false)
if doxygen_prog.found()
  run_target(
    'doxygen',
    command: [doxygen_prog, join_paths(meson.project_source_root(), 'Doxyfile')]
  )
endif

