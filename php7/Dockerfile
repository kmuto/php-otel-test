FROM php:7.4
RUN apt-get update && apt-get install -y --no-install-recommends git-core unzip \
  && php -r "copy('https://getcomposer.org/installer', 'composer-setup.php');" \
  && php composer-setup.php \
  && mv composer.phar /usr/local/bin/composer
RUN docker-php-ext-install mysqli pdo pdo_mysql \
  && docker-php-ext-enable mysqli pdo pdo_mysql
