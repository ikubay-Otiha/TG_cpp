cmake_minimum_required(VERSION 3.31)
project(TG_cpp)

# 使用するC++の標準を設定
set(CMAKE_CXX_STANDARD 17)

# 実行ファイルを build ディレクトリに出力
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# vcpkgツールチェインファイル指定
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake)

# 必要ライブラリを検索
find_package(crow REQUIRED)
find_package(cpr REQUIRED)
find_package(nlohmann_json REQUIRED)

# crow.h のインクルードパスを明示的に追加
include_directories(${CMAKE_SOURCE_DIR}/vcpkg/installed/x64-osx/include)

# 実行ファイルのターゲットを設定
add_executable(
    TG_cpp
    src/main.cpp
    src/routes/route.cpp
    src/utils/ip_checker.cpp
)

# nlomann/json.hpp のインクルードパスを明示的に追加
target_include_directories(TG_cpp PUBLIC
    ${CMAKE_SOURCE_DIR}/vcpkg/installed/x64-osx/include
)

# コンパイルオプションを追加
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")