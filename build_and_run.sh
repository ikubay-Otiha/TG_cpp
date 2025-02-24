#!/bin/bash

# When error occurs, stop the script
set -e

# read .env file
source .env

# check .env file is set
echo "VCPKG_PATH: $VCPKG_PATH"
echo "INCLUDE_PATH: $INCLUDE_PATH"

# get the project root directory
PROJECT_ROOT="$(pwd)"

# clean old build
if [[ "$@" == *clean* ]]; then
  echo "Cleaning old build..."
  rm -rf build
fi

# if build directory does not exist, create it
mkdir -p build

# change to build directory
cd build

# run CMake to generate build files
echo "Running CMake..."
cmake "$PROJECT_ROOT" \
  -DCMAKE_TOOLCHAIN_FILE="${VCPKG_PATH}/scripts/buildsystems/vcpkg.cmake" \
  -DINCLUDE_PATH="${INCLUDE_PATH}" 

# build the project
echo "Building project..."
make -j$(nproc)

# set the path to the executable
EXECUTABLE="$PROJECT_ROOT/build/TG_cpp"

# run Valgrind or GDB, or the executable directly
if [ -f "$EXECUTABLE" ]; then
  cd ..
  if [[ "$@" == *valgrind* ]]; then
    echo "Running valgrind..."
    valgrind --leak-check=full --show-leak-kinds=all "$EXECUTABLE"
  elif [[ "$@" = *gdb* ]]; then
    echo "Running GDB..."
    gdb "$EXECUTABLE"
  else
    echo "Running the executable..."
    exec "$EXECUTABLE"
  fi
else
  echo "Executable not found! Expected at: $EXECUTABLE"
  exit 1
fi