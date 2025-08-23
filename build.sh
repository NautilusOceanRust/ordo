#!/bin/bash
set -e

# build.sh - Script to build the Ordo project
#
# This script automatically detects the MSYS2 environment and builds the project
# in a separate directory for each environment.

# --- Help Message ---
show_help() {
    echo "Usage: ./build.sh [command]"
    echo ""
    echo "Commands:"
    echo "  all          (Default) Configure and compile the project."
    echo "  build        Compile the project."
    echo "  configure    Configure the project using Meson."
    echo "  reconfigure  Force a reconfiguration of the project."
    echo "  test         Run the unit tests."
    echo "  clean        Remove the build directory for the current environment."
    echo "  distclean    Remove all build directories."
    echo "  format       Format the code using clang-format."
    echo "  tidy         Run clang-tidy on the source code."
    echo "  help         Show this help message."
}

# --- Environment Detection ---
if [ -z "$MSYSTEM" ]; then
    echo "Error: This script is intended to be run in an MSYS2 environment."
    exit 1
fi

case "$MSYSTEM" in
MINGW64) BUILD_DIR="build-mingw64" ;;
MINGW32) BUILD_DIR="build-mingw32" ;;
UCRT64) BUILD_DIR="build-ucrt64" ;;
CLANG64) BUILD_DIR="build-clang64" ;;
*)
    echo "Error: Unsupported MSYS2 environment: $MSYSTEM"
    exit 1
    ;;
esac

# --- Dependency Check ---
check_dependencies() {
    echo "Checking MSYS2 dependencies..."

    local prefix
    case "$MSYSTEM" in
    MINGW64) prefix="mingw-w64-x86_64-" ;;
    MINGW32) prefix="mingw-w64-i686-" ;;
    UCRT64) prefix="mingw-w64-ucrt-x86_64-" ;;
    CLANG64) prefix="mingw-w64-clang-x86_64-" ;;
    esac

    local deps=("gcc" "meson" "ninja" "sqlite3" "pdcurses" "cjson" "clang-tools-extra")
    local pkg_names=()
    for dep in "${deps[@]}"; do
        pkg_names+=("${prefix}${dep}")
    done

    local missing_deps
    missing_deps=$(pacman -Q "${pkg_names[@]}" 2>/dev/null | grep -c "error: package .* was not found") || true
    
    if [ "$missing_deps" -gt 0 ]; then
        echo "Error: The following required packages are missing:"
        pacman -Q "${pkg_names[@]}" 2>&1 | grep "error: package .* was not found" | sed -e "s/error: package '//" -e "s/' was not found/  -/"
        echo
        echo "Please install them by running:"
        echo "  pacman -S ${pkg_names[*]}"
        echo
        exit 1
    fi

    echo "All dependencies are satisfied. ✨"
}

# --- Functions ---

# Function to configure the project
configure() {
    if [ ! -f "$BUILD_DIR/build.ninja" ]; then
        echo "Configuring for $MSYSTEM in $BUILD_DIR..."
        meson setup "$BUILD_DIR"
    else
        echo "Project already configured. Use './build.sh reconfigure' to force."
    fi
}

# Function to reconfigure the project
reconfigure() {
    echo "Reconfiguring for $MSYSTEM in $BUILD_DIR..."
    meson setup --reconfigure "$BUILD_DIR"
}

# Function to build the project
build() {
    configure
    echo "Building for $MSYSTEM in $BUILD_DIR..."
    ninja -C "$BUILD_DIR"
}

# Function to run tests
test_project() {
    # Check if tests are enabled in meson configuration
    if ! meson configure "$BUILD_DIR" | grep -q "tests.*true"; then
        echo "Tests not enabled, reconfiguring..."
        meson setup --reconfigure -Dtests=true "$BUILD_DIR"
    fi
    build
    echo "Running tests for $MSYSTEM in $BUILD_DIR..."
    meson test -C "$BUILD_DIR"
}

# Function to clean the build directory for the current environment
clean() {
    echo "Cleaning build directory for $MSYSTEM..."
    rm -rf "$BUILD_DIR"
}

# Function to clean all build directories
distclean() {
    echo "Cleaning all build directories..."
    rm -rf build-*
}

# Function to format the code
format() {
    echo "Formatting code with clang-format..."
    find . -name "*.c" -o -name "*.h" | xargs clang-format -i
}

# Function to run clang-tidy
tidy() {
    echo "Running clang-tidy..."
    ./run-clang-tidy.sh
}

# --- Main Logic ---
COMMAND="$1"

# Default command is 'all'
if [ -z "$COMMAND" ]; then
    COMMAND="all"
fi

# Check dependencies for relevant commands
case "$COMMAND" in
clean | distclean | help | format) ;;
*)
    check_dependencies
    ;;
esac

case "$COMMAND" in
all)
    build
    ;;
build)
    build
    ;;
configure)
    configure
    ;;
reconfigure)
    reconfigure
    ;;
test)
    test_project
    ;;
clean)
    clean
    ;;
distclean)
    distclean
    ;;
format)
    format
    ;;
tidy)
    tidy
    ;;
help)
    show_help
    ;;
*)
    echo "Error: Unknown command '$COMMAND'"
    show_help
    exit 1
    ;;
esac

echo "Done. 😉"