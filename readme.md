# TG_cpp

## 概要

このプロジェクトは、C++17 で書かれた文字起こしアプリのバックエンドサーバーです。

## 必要な環境

*   OS: macOS, Linux, Raspberry Pi OS
*   コンパイラ: GCC, Clang
*   CMake: 3.10 以上
*   vcpkg

## ビルド&ラン方法

1.  vcpkg をインストールします。
    ```bash
    git clone https://github.com/microsoft/vcpkg.git
    cd vcpkg
    ./bootstrap-vcpkg.sh
    ```
2.  必要なライブラリをインストールします。
    ```bash
    ./vcpkg install crow
    ./vcpkg install cpr
    ./vcpkg install nlohmann-json
    ```
3.  CMakeLists.txt を修正します。
4.  以下のコマンドでビルド&実行します。
    ```bash
    sh build_and_run.sh
    ```