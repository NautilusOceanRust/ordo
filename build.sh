#!/bin/bash
set -e


# build.sh - Script to build the Ordo project
#
# This script automatically detects the MSYS2 environment and builds the project
# in a separate directory for each environment.

set -e

# --- Environment Detection ---
if [ -z "$MSYSTEM" ]; then
  echo "Error: This script is intended to be run in an MSYS2 environment."
  exit 1
fi

case "$MSYSTEM" in
  MINGW64)
    BUILD_DIR="build-mingw64"
    ;;
  MINGW32)
    BUILD_DIR="build-mingw32"
    ;;
  UCRT64)
    BUILD_DIR="build-ucrt64"
    ;;
  CLANG64)
    BUILD_DIR="build-clang64"
    ;;
  *)
    echo "Error: Unsupported MSYS2 environment: $MSYSTEM"
    exit 1
    ;;
esac

# --- Functions ---

# Function to display help
function show_help() {
  echo "Usage: ./build.sh [command]"
  echo ""
  echo "Commands:"
  echo "  all       (Default) Configure and compile the project."
  echo "  build     Compile the project."
  echo "  configure Configure the project using Meson."
  echo "  test      Run the unit tests."
  echo "  clean     Remove all build directories."
  echo "  help      Show this help message."
}

# Function to configure the project
function configure() {
  echo "Configuring for $MSYSTEM in $BUILD_DIR..."
  meson setup "$BUILD_DIR"
}

# Function to build the project
function build() {
  echo "Building for $MSYSTEM in $BUILD_DIR..."
  ninja -C "$BUILD_DIR"
}

# Function to run tests
function test_project() {
  echo "Running tests for $MSYSTEM in $BUILD_DIR..."
  meson test -C "$BUILD_DIR"
}

# Function to clean build directories
function clean() {
  echo "Cleaning all build directories..."
  rm -rf build-*
}

# --- Main Logic ---
COMMAND="$1"

if [ -z "$COMMAND" ]; then
  COMMAND="all"
fi

case "$COMMAND" in
  all)
    configure
    build
    ;;
  build)
    build
    ;;
  configure)
    configure
    ;;
  test)
    test_project
    ;;
  clean)
    clean
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

echo "Done."
