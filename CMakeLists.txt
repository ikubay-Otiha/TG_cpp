cmake_minimum_required(VERSION 3.31)
project(TG_cpp)

# .envファイルを読み込む
if(EXISTS "${PROJECT_SOURCE_DIR}/.env")  # .envファイルが存在する場合のみ処理
    file(READ "${PROJECT_SOURCE_DIR}/.env" ENV_FILE_CONTENTS)
    string(REPLACE "\n" ";" ENV_FILE_CONTENTS "${ENV_FILE_CONTENTS}")

    # 各行を処理
    foreach(LINE ${ENV_FILE_CONTENTS})
        string(STRIP "${LINE}" LINE)
        # コメント行と空行を明示的にスキップ
        if(LINE AND NOT LINE MATCHES "^#")
            if(LINE MATCHES "^([^=]+)=(.*)$")
                set(ENV_VAR_NAME "${CMAKE_MATCH_1}")
                set(ENV_VAR_VALUE "${CMAKE_MATCH_2}")

                # トリム処理
                string(STRIP "${ENV_VAR_VALUE}" ENV_VAR_VALUE)
            
                if(ENV_VAR_NAME STREQUAL "VCPKG_PATH")
                    set(VCPKG_PATH "${ENV_VAR_VALUE}" CACHE PATH "Path to vcpkg" FORCE)
                else()
                    set(${ENV_VAR_NAME} "${ENV_VAR_VALUE}" FORCE)
                endif()
            
                message(STATUS "Loaded ${ENV_VAR_NAME}=${ENV_VAR_VALUE}")
            endif()
        endif()
    endforeach()
endif()

# VCPKGのパスを確認する追加のチェック
if(NOT DEFINED VCPKG_PATH)
    # 環境変数からVCPKG_PATHを取得
    if(DEFINED ENV{VCPKG_PATH})
        set(VCPKG_PATH "$ENV{VCPKG_PATH}" CACHE PATH "Path to vcpkg" FORCE)
    endif()
endif()

# VCPKG_PATHが設定されているか最終確認
if(DEFINED VCPKG_PATH AND IS_DIRECTORY "${VCPKG_PATH}")
    message(STATUS "VCPKG_PATH is set to: ${VCPKG_PATH}")
    set(CMAKE_TOOLCHAIN_FILE "${VCPKG_PATH}/scripts/buildsystems/vcpkg.cmake")
else()
    message(FATAL_ERROR "VCPKG_PATH is not set or is not a valid directory. 
    Please set VCPKG_PATH in .env file or as an environment variable.")
endif()

# 環境変数が正しく読み込まれているか確認
message(STATUS "INCLUDE_PATH: ${INCLUDE_PATH}")
message(STATUS "VCPKG_PATH: ${VCPKG_PATH}")

# 使用するC++の標準を設定
set(CMAKE_CXX_STANDARD 17)

# デバッグビルド設定
set(CMAKE_BUILD_TYPE Debug)

# 実行ファイルを build ディレクトリに出力
set(CMAKE_BINARY_DIR "${CMAKE_SOURCE_DIR}/build")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# vcpkgツールチェインファイル指定
if(DEFINED VCPKG_PATH)
    set(CMAKE_TOOLCHAIN_FILE "${VCPKG_PATH}/scripts/buildsystems/vcpkg.cmake")
endif()

# 実行ファイルのターゲットを設定
add_executable(
    TG_cpp
    src/main.cpp
    src/network/curl_request.cpp
    src/routes/route.cpp
    src/utils/utils.cpp
    src/whisper/whisper_handler.cpp
)

find_package(CURL REQUIRED)
target_link_libraries(TG_cpp PRIVATE CURL::libcurl)

# インクルードパスを追加
if(DEFINED INCLUDE_PATH AND IS_DIRECTORY "${INCLUDE_PATH}")
    message(STATUS "Adding include directory: ${INCLUDE_PATH}")
    target_include_directories(TG_cpp PUBLIC "${INCLUDE_PATH}")
endif()

# インクルードパスを明示的に追加
target_include_directories(TG_cpp PUBLIC ${INCLUDE_PATH})

# リンクされているライブラリを出力
get_target_property(LINK_LIBRARIES TG_cpp LINK_LIBRARIES)
message(STATUS "LINK_LIBRARIES: ${LINK_LIBRARIES}")

# コンパイルオプションを追加
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Wno-type-limits")

# コンパイラバージョンを表示
message(STATUS "C++ Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")

# Valgrindを実行するカスタムターゲット
add_custom_target(valgrind
    COMMAND valgrind --leak-check=full ./TG_cpp
    DEPENDS TG_cpp
    )