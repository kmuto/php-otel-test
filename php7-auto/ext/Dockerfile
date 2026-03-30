FROM php:7.4-cli

RUN apt-get update && apt-get install -y \
    autoconf \
    gcc \
    make \
    && rm -rf /var/lib/apt/lists/*

COPY / /usr/src/ext/

WORKDIR /usr/src/ext

RUN phpize \
    && ./configure --enable-opentelemetry_php74 \
    && make -j$(nproc) \
    && make install

RUN docker-php-ext-enable opentelemetry_php74

CMD ["make", "test", "TESTS=tests/", "NO_INTERACTION=1"]
