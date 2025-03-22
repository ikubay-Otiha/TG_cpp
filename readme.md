# TG_cpp

## 概要
このプロジェクトは、C++17 で書かれた文字起こしアプリのバックエンドサーバーです。

## ローカル環境での実行

### 必要な環境
*   OS: macOS, Linux, Raspberry Pi OS
*   コンパイラ: GCC, Clang
*   CMake: 3.31 以上
*   vcpkg

### ビルド&ラン方法
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
    INCLUDE_PATH=</path/to/your/vcpkg/installed/your-architecture/include>
    ```
4.  プロジェクト配下にconfigディレクトリを作成し、allowed_ip.jsonを作成し許可するIPを記載(Temporary)
    ```bash
    mkdir -p config && touch config/allowed_ip.json
    ```
    allowed_ip.json
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

## Docker環境での実行

### 必要な環境
* Docker
* Docker Compose

### Docker単体での実行
1. Dockerfileがあるディレクトリで以下のコマンドを実行
    ```bash
    docker build -t tg_cpp .
    docker run -p 3030:3030 -v $(pwd)/config:/app/config tg_cpp
    ```

### Docker Composeでの実行 (推奨)
1. プロジェクト配下にconfigディレクトリを作成し、allowed_ip.jsonを作成
    ```bash
    mkdir -p config && touch config/allowed_ip.json
    ```
    allowed_ip.json
    ```json
    {
        "allowed_ips": [
            "192.168.1.2",
            "192.168.1.7"
        ],
        "port": 3030
    }
    ```

2. docker-compose.ymlがあるディレクトリで以下のコマンドを実行
    ```bash
    docker-compose up -d transcription_backend
    ```

### 設定の変更
* IPアドレス制限を変更する場合は、`config/allowed_ip.json`を編集後、Dockerコンテナを再起動
    ```bash
    docker-compose restart transcription_backend
    ```

* ポート番号を変更する場合は、`config/allowed_ip.json`とdocker-compose.ymlの両方を編集