#!/bin/bash

# When error occurs, stop the script
set -e

# get the project root directory
PROJECT_ROOT="$(pwd)"

# clean old build
if [ "$1" == "clean" ]; then
  echo "Cleaning old build..."
  rm -rf build
fi

# if build directory does not exist, create it
if [ ! -d "build" ]; then
  mkdir build
fi

# change to build directory
cd build

# build to use CMakelists.txt
echo "Running CMake..."
if ! cmake "$PROJECT_ROOT" -DCMAKE_TOOLCHAIN_FILE="$PROJECT_ROOT/vcpkg/scripts/buildsystems/vcpkg.cmake"; then
  echo "CMake configuration failed!"
  exit 1
fi

echo "Building project..."
if ! make -j$(nproc); then
  echo "Build failed!"
  exit 1
fi

# set the path to the executable
EXECUTABLE="$PROJECT_ROOT/build/TG_cpp"

# run the executable
if [ -f "$EXECUTABLE" ]; then
  echo "Running the executable..."
  cd ..
  exec "$EXECUTABLE"
else
  echo "Executable not found! Expected at: $EXECUTABLE"
  exit 1
fi