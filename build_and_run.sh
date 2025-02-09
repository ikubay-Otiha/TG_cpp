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
if [ "$1" == "clean" ]; then
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

# run the executable if it exists
if [ -f "$EXECUTABLE" ]; then
  echo "Running the executable..."
  cd ..
  exec "$EXECUTABLE"
else
  echo "Executable not found! Expected at: $EXECUTABLE"
  exit 1
fi