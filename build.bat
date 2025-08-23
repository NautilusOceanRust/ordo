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

REM --- Environment Detection ---
set "MSYS2_ENV="
if defined MSYSTEM (
    set "MSYSTEM_LOWER=%MSYSTEM%"
    REM Batch file does not have a built-in lowercase function, but MSYSTEM is usually consistent.
    REM We will use it as is, e.g., MINGW64, UCRT64.
    set "MSYS2_ENV=%MSYSTEM_LOWER%"
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
