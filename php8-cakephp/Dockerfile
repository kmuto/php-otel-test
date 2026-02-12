# PHP 8.4 Apacheイメージを使用
FROM php:8.4-apache

# システムの更新と必要なライブラリのインストール
RUN apt-get update && apt-get install -y \
    libicu-dev \
    libmariadb-dev \
    unzip \
    zip \
    zlib1g-dev \
    libpng-dev \
    libjpeg-dev \
    libzip-dev \
    && rm -rf /var/lib/apt/lists/*

# PHP拡張機能のインストール (CakePHPに必須なもの)
RUN docker-php-ext-install intl pdo_mysql gd zip

# Apacheのmod_rewriteを有効化 (CakePHPのルーティングに必須)
RUN a2enmod rewrite

RUN sed -i 's/Listen 80/Listen 8081/' /etc/apache2/ports.conf
RUN sed -i 's/:80/:8081/' /etc/apache2/sites-available/000-default.conf

# Composerのインストール
COPY --from=composer:latest /usr/bin/composer /usr/bin/composer

# 作業ディレクトリの設定
WORKDIR /var/www/html

# コンテナ起動時にApacheをフォアグラウンドで実行
CMD ["apache2-foreground"]
