@echo off
setlocal

REM build.bat - Script to build the Ordo project
REM
REM This script automatically detects the MSYS2 environment and builds the project
REM in a separate directory for each environment.

REM --- Help Message ---
:show_help
echo Usage: %0 [command]
echo.
echo Commands:
	echo   all          (Default) Configures and compiles the project.
	echo   build        Compiles the project.
	echo   configure    Configures the project using Meson.
	echo   reconfigure  Force a reconfiguration of the project.
	echo   test         Runs the unit tests.
	echo   clean        Removes the build directory for the current environment.
	echo   distclean    Removes all build directories.
	echo   format       Formats the code using clang-format.
	echo   tidy         Runs clang-tidy on the source code.
	echo   help         Shows this help message.

echo.
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

set "BUILD_DIR=build"
if defined MSYS2_ENV (
    set "BUILD_DIR=build-%MSYS2_ENV%"
)

REM --- Dependency Check ---
:check_dependencies
echo Checking MSYS2 dependencies...
set "PREFIX="
if /i "%MSYSTEM%" == "MINGW64" set "PREFIX=mingw-w64-x86_64-"
if /i "%MSYSTEM%" == "MINGW32" set "PREFIX=mingw-w64-i64-"
if /i "%MSYSTEM%" == "UCRT64" set "PREFIX=mingw-w64-ucrt-x86_64-"
if /i "%MSYSTEM%" == "CLANG64" set "PREFIX=mingw-w64-clang-x86_64-"

if not defined PREFIX (
    echo Warning: Unsupported MSYSTEM '%MSYSTEM%'. Skipping dependency check.
    goto:eof
)

set "DEPS=gcc meson ninja sqlite3 pdcurses cjson clang-tools-extra"
set "PKG_NAMES="
for %%d in (%DEPS%) do (
    set "PKG_NAMES=!PKG_NAMES! %PREFIX%%%d"
)

pacman -Q %PKG_NAMES% >nul 2>&1
if errorlevel 1 (
    echo Error: The following required packages are missing:
    pacman -Q %PKG_NAMES% 2>&1 | findstr /R /C:"error: package '.*' was not found"
    echo.
    echo Please install them by running:
    echo   pacman -S%PKG_NAMES%
    echo.
    exit /b 1
)

echo All dependencies are satisfied. ✨
goto:eof

REM --- Command Functions ---
:configure
if not exist "%BUILD_DIR%\build.ninja" (
    echo Configuring for %MSYSTEM% in %BUILD_DIR%...
    meson setup "%BUILD_DIR%"
) else (
    echo Project already configured. Use '%0 reconfigure' to force.
)
if %errorlevel% neq 0 exit /b %errorlevel%
goto:eof

:reconfigure
echo Reconfiguring for %MSYSTEM% in %BUILD_DIR%...
meson setup --reconfigure "%BUILD_DIR%"
if %errorlevel% neq 0 exit /b %errorlevel%
goto:eof

:build
call:configure
echo Building for %MSYSTEM% in %BUILD_DIR%...
ninja -C "%BUILD_DIR%"
if %errorlevel% neq 0 exit /b %errorlevel%
goto:eof

:run_tests
meson configure "%BUILD_DIR%" | findstr /R /C:"tests.*true" >nul
if errorlevel 1 (
    echo Tests not enabled, reconfiguring...
    meson setup --reconfigure -Dtests=true "%BUILD_DIR%"
    if %errorlevel% neq 0 exit /b %errorlevel%
)
call:build
echo Running tests for %MSYSTEM% in %BUILD_DIR%...
meson test -C "%BUILD_DIR%"
if %errorlevel% neq 0 exit /b %errorlevel%
goto:eof

:clean
echo Cleaning build directory for %MSYSTEM%...
if exist "%BUILD_DIR%" (
    rd /s /q "%BUILD_DIR%"
)
goto:eof

:distclean
echo Cleaning all build directories...
for /d %%d in (build-*) do (
    if exist "%%d" (
        echo Removing "%%d"
        rd /s /q "%%d"
    )
)
goto:eof

:format
echo Formatting code with clang-format...
for /r %%f in (*.c, *.h) do (
    clang-format -i "%%f"
)
goto:eof

:tidy
echo Running clang-tidy...
call run-clang-tidy.sh
goto:eof


REM --- Main Logic ---
set "COMMAND=%1"
if not defined COMMAND set "COMMAND=all"

setlocal enabledelayedexpansion

REM Check dependencies for relevant commands
set "NO_DEPS_CHECK_CMDS=clean distclean help format"
echo "%NO_DEPS_CHECK_CMDS%" | findstr /i /c:"%COMMAND%" >nul
if errorlevel 1 (
    call:check_dependencies
    if errorlevel 1 exit /b 1
)


if /i "%COMMAND%" == "help" (
    call:show_help
    exit /b 0
)
if /i "%COMMAND%" == "all" (
    call:build
    exit /b %errorlevel%
)
if /i "%COMMAND%" == "build" (
    call:build
    exit /b %errorlevel%
)
if /i "%COMMAND%" == "configure" (
    call:configure
    exit /b %errorlevel%
)
if /i "%COMMAND%" == "reconfigure" (
    call:reconfigure
    exit /b %errorlevel%
)
if /i "%COMMAND%" == "test" (
    call:run_tests
    exit /b %errorlevel%
)
if /i "%COMMAND%" == "clean" (
    call:clean
    exit /b %errorlevel%
)
if /i "%COMMAND%" == "distclean" (
    call:distclean
    exit /b %errorlevel%
)
if /i "%COMMAND%" == "format" (
    call:format
    exit /b %errorlevel%
)
if /i "%COMMAND%" == "tidy" (
    call:tidy
    exit /b %errorlevel%
)

echo Unknown command: "%COMMAND%"
call:show_help
exit /b 1