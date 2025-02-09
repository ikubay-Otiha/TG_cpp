# TG_cpp

## 概要

このプロジェクトは、C++17 で書かれた文字起こしアプリのバックエンドサーバーです。

## 必要な環境

*   OS: macOS, Linux, Raspberry Pi OS
*   コンパイラ: GCC, Clang
*   CMake: 3.31 以上
*   vcpkg

## ビルド&ラン方法

1.  vcpkg をインストール
    ```bash
    git clone https://github.com/microsoft/vcpkg.git
    cd vcpkg
    ./bootstrap-vcpkg.sh
    ```
2.  必要なライブラリをインストール
    ```bash
    ./vcpkg install crow
    ./vcpkg install cpr
    ./vcpkg install nlohmann-json
    ```
3.  環境設定を.envへ記載
    ```
    # vcpkg path
    VCPKG_PATH=</path/to/your/vcpkg>
    # include path
    INCLUDE_PATH=</path/to/your/vcpkg/installed/your-architectuew/include>
    ```
4.  allowed_ip.jsonを作成し許可するパスを記載(Temporary)
    ```json
    {
        "allowed_ips": [
            "192.168.1.2",
            "192.168.1.7"
            ],
        "port": 3030
    }
    ```
5.  以下のコマンドでビルド&実行
    ```bash
    ./build_and_run.sh
    ```