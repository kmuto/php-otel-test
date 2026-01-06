FROM php:8.4
RUN apt-get update && apt-get install -y --no-install-recommends git-core unzip \
  && php -r "copy('https://getcomposer.org/installer', 'composer-setup.php');" \
  && php composer-setup.php \
  && mv composer.phar /usr/local/bin/composer
RUN docker-php-ext-install mysqli pdo pdo_mysql \
  && docker-php-ext-enable mysqli pdo pdo_mysql

COPY opentelemetry.ini /usr/local/etc/php/conf.d/
RUN pecl install opentelemetry \
  && composer require -n open-telemetry/sdk open-telemetry/opentelemetry-auto-laravel open-telemetry/exporter-otlp open-telemetry/opentelemetry-auto-mysqli
