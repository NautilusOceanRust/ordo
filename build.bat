@echo off
setlocal

REM --- Help Message ---
:show_help
echo Usage: %0 [command]
echo.
echo Commands:
echo   all          (Default) Configures and compiles the project.
echo   build        Compiles the project.
echo   configure    Configures the project using Meson.
echo   test         Runs the unit tests.
echo   clean        Removes all build directories.
echo   help         Shows this help message.
echo.
goto:eof

REM --- MSYS2 Dependency Check ---
:check_dependencies
echo Checking MSYS2 dependencies...
set "PREFIX="
if /i "%MSYSTEM%"=="MINGW64" set "PREFIX=mingw-w64-x86_64-"
if /i "%MSYSTEM%"=="MINGW32" set "PREFIX=mingw-w64-i686-"
if /i "%MSYSTEM%"=="UCRT64"  set "PREFIX=mingw-w64-ucrt-x86_64-"
if /i "%MSYSTEM%"=="CLANG64" set "PREFIX=mingw-w64-clang-x86_64-"

if not defined PREFIX (
    echo Warning: Unsupported MSYSTEM '%MSYSTEM%'. Skipping dependency check.
    goto:eof
)

set "DEPS=gcc meson ninja sqlite3 pdcurses cjson"
set "MISSING_DEPS="

for %%d in (%DEPS%) do (
    pacman -Q "%PREFIX%%%d" >nul 2>&1
    if errorlevel 1 (
        set "MISSING_DEPS=!MISSING_DEPS! %PREFIX%%%d"
    )
)

if defined MISSING_DEPS (
    echo Error: The following required packages are missing:
    for %%p in (%MISSING_DEPS%) do (
        echo   - %%p
    )
    echo.
    echo Please install them by running:
    echo   pacman -S%MISSING_DEPS%
    echo.
    exit /b 1
)

echo All dependencies are satisfied. ✨
goto:eof


REM --- Environment Detection ---
set "MSYS2_ENV="
if defined MSYSTEM (
    set "MSYSTEM_LOWER=%MSYSTEM%"
    set "MSYS2_ENV=%MSYSTEM_LOWER%"
) else (
    echo Error: This script is intended to be run in an MSYS2 environment.
    exit /b 1
)

REM --- Build Directory ---
set "BUILD_DIR=build"
if defined MSYS2_ENV (
    set "BUILD_DIR=build-%MSYS2_ENV%"
)

REM --- Command Functions ---
:configure
echo Configuring in %BUILD_DIR%...
meson setup "%BUILD_DIR%"
if %errorlevel% neq 0 exit /b %errorlevel%
goto:eof

:build
echo Building in %BUILD_DIR%...
if not exist "%BUILD_DIR%\build.ninja" (
    call:configure
)
ninja -C "%BUILD_DIR%"
if %errorlevel% neq 0 exit /b %errorlevel%
goto:eof

:run_tests
echo Running tests in %BUILD_DIR%...
REM Ensure tests are enabled
meson setup --reconfigure -Dtests=true "%BUILD_DIR%"
if %errorlevel% neq 0 exit /b %errorlevel%
ninja -C "%BUILD_DIR%"
if %errorlevel% neq 0 exit /b %errorlevel%
meson test -C "%BUILD_DIR%"
if %errorlevel% neq 0 exit /b %errorlevel%
goto:eof

:clean
echo Cleaning all build directories...
for /d %%d in (build*) do (
    if exist "%%d" (
        echo Removing "%%d"
        rd /s /q "%%d"
    )
)
goto:eof

REM --- Main Logic ---
set "COMMAND=%1"
if not defined COMMAND set "COMMAND=all"

setlocal enabledelayedexpansion

REM Check dependencies for relevant commands
if /i not "%COMMAND%"=="clean" if /i not "%COMMAND%"=="help" (
    call:check_dependencies
    if errorlevel 1 exit /b 1
)

if /i "%COMMAND%"=="help" (
    call:show_help
    exit /b 0
)
if /i "%COMMAND%"=="all" (
    call:build
    exit /b %errorlevel%
)
if /i "%COMMAND%"=="build" (
    call:build
    exit /b %errorlevel%
)
if /i "%COMMAND%"=="configure" (
    call:configure
    exit /b %errorlevel%
)
if /i "%COMMAND%"=="test" (
    call:run_tests
    exit /b %errorlevel%
)
if /i "%COMMAND%"=="clean" (
    call:clean
    exit /b %errorlevel%
)

echo Unknown command: "%COMMAND%"
call:show_help
exit /b 1
