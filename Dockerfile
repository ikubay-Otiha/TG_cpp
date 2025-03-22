FROM ubuntu:20.04
# 対話形式の入力を防ぐためにタイムゾーンを設定
ENV DEBIAN_FRONTEND=noninteractive
# 基本的なパッケージをインストール
RUN apt-get update && apt-get install -y \
    build-essential \
    git \
    curl \
    zip \
    unzip \
    tar \
    pkg-config \
    libssl-dev \
    libcurl4-openssl-dev \
    ninja-build \
    g++ \
    gcc \
    python3 \
    software-properties-common \
    apt-transport-https \
    ca-certificates \
    gnupg \
    lsb-release \
    # Python3のビルドに必要なパッケージを追加
    autoconf \
    automake \
    autoconf-archive \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*
# Kitwareの公式リポジトリを追加して最新のCMakeをインストール
RUN apt-get update && \
    apt-get install -y gpg wget && \
    wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc 2>/dev/null | gpg --dearmor - | tee /usr/share/keyrings/kitware-archive-keyring.gpg >/dev/null && \
    echo "deb [signed-by=/usr/share/keyrings/kitware-archive-keyring.gpg] https://apt.kitware.com/ubuntu/ $(lsb_release -cs) main" | tee /etc/apt/sources.list.d/kitware.list >/dev/null && \
    apt-get update && \
    apt-get install -y cmake && \
    cmake --version && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*
# vcpkgのインストール
RUN git clone https://github.com/Microsoft/vcpkg.git /vcpkg \
    && /vcpkg/bootstrap-vcpkg.sh
# ワークディレクトリを設定
WORKDIR /app
# 環境変数を設定
ENV VCPKG_PATH=/vcpkg
ENV INCLUDE_PATH=/vcpkg/installed/arm64-linux/include
# READMEに記載されている必要なライブラリとboostをインストール
RUN /vcpkg/vcpkg install crow nlohmann-json cpr boost-stacktrace boost --triplet=arm64-linux
# IPアドレス設定用のディレクトリを作成
RUN mkdir -p /app/config
# 設定ファイルを作成
RUN echo '{"allowed_ips": ["192.168.1.2", "192.168.1.7"], "port": 3030}' > /app/config/allowed_ip.json
# ソースコードをコンテナにコピー
COPY . .
# CMakeLists.txtの修正: 必要なCMakeバージョンを調整（現在のバージョンに合わせる）
RUN if [ -f CMakeLists.txt ]; then \
    sed -i 's/cmake_minimum_required(VERSION 3.31)/cmake_minimum_required(VERSION 3.16)/' CMakeLists.txt; \
    fi
# ビルドスクリプトに実行権限を付与
RUN chmod +x /app/build_and_run.sh
# .envファイルを作成
RUN echo "VCPKG_PATH=/vcpkg\nINCLUDE_PATH=/vcpkg/installed/x64-linux/include" > /app/.env
# ポート3030を公開
EXPOSE 3030
# アプリケーションを起動
CMD ["/app/build_and_run.sh"]