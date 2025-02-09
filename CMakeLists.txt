cmake_minimum_required(VERSION 3.31)
project(TG_cpp)

# .envファイルを読み込む
if(EXISTS "${PROJECT_SOURCE_DIR}/.env")  # .envファイルが存在する場合のみ処理
    file(READ "${PROJECT_SOURCE_DIR}/.env" ENV_FILE_CONTENTS)

    # 各行を処理
    foreach(LINE ${ENV_FILE_CONTENTS})
        string(STRIP "${LINE}" LINE) # 行頭・行末の空白を削除
        if(LINE AND NOT LINE MATCHES "^#") # 空行、コメント行をスキップ
            if(LINE MATCHES "^([a-zA-Z0-9_]+)=(.*)$") # 正しいフォーマットかチェック
                set(ENV_VAR_NAME "${CMAKE_MATCH_1}")
                set(ENV_VAR_VALUE "${CMAKE_MATCH_2}")
                set(${ENV_VAR_NAME} "${ENV_VAR_VALUE}") # 環境変数をCMakeに設定
                message(STATUS "Loaded ${ENV_VAR_NAME}=${ENV_VAR_VALUE}")  # デバッグ出力
            endif()
        endif()
    endforeach()
endif()

# 環境変数が正しく読み込まれているか確認
message(STATUS "INCLUDE_PATH: ${INCLUDE_PATH}")
message(STATUS "VCPKG_PATH: ${VCPKG_PATH}")

# 使用するC++の標準を設定
set(CMAKE_CXX_STANDARD 17)

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
    src/routes/route.cpp
    src/utils/ip_checker.cpp
)

# インクルードパスを追加
if(DEFINED INCLUDE_PATH AND IS_DIRECTORY "${INCLUDE_PATH}")
    message(STATUS "Adding include directory: ${INCLUDE_PATH}")
    target_include_directories(TG_cpp PUBLIC "${INCLUDE_PATH}")
endif()

# nlomann/json.hpp のインクルードパスを明示的に追加
target_include_directories(TG_cpp PUBLIC ${INCLUDE_PATH})

# コンパイルオプションを追加
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Wno-type-limits")