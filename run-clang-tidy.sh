#!/bin/bash
# Bash Script to run clang-tidy on all C sources in the project.
# It uses the compilation database to get the list of files and compiler flags.

set -e # Exit immediately if a command exits with a non-zero status.

PROJECT_ROOT=$(pwd)
BUILD_DIR=${1:-build} # Use the first argument as BUILD_DIR, or default to "build"

if [ -z "$BUILD_DIR" ]; then
    echo "Usage: $0 <build-directory>"
    exit 1
fi

COMPILE_DB="$BUILD_DIR/compile_commands.json"

if [ ! -d "$BUILD_DIR" ]; then
    echo "Error: Build directory '$BUILD_DIR' not found."
    echo "Please run the build first."
    exit 1
fi

if [ ! -f "$COMPILE_DB" ]; then
    echo "Error: Compilation database not found at $COMPILE_DB"
    echo "Please run 'meson setup' in '$BUILD_DIR' first."
    exit 1
fi

# Change into the build directory so that the relative paths in the
# compilation database are resolved correctly by clang-tidy.
echo "Running clang-tidy in $BUILD_DIR..."
cd "$BUILD_DIR"

# Extract file paths from the compilation database.
# The paths are relative to the build dir (e.g., ../main.c), which is now correct.
# The -p . tells clang-tidy to look for compile_commands.json in the current dir.
grep -o '"file": "[^"]*"' "compile_commands.json" | \
    sed 's/"file": "//;s/"//' | \
    grep -v 'sanitycheckc.c' | \
    xargs clang-tidy -p .

# Return to the original directory
cd "$PROJECT_ROOT"
