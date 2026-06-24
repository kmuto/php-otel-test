#!/bin/bash
composer install

cat > .env <<EOT
APP_ENV=local
APP_DEBUG=true
APP_KEY=

DB_CONNECTION=mysql
DB_HOST=db
DB_PORT=3306
DB_DATABASE=laravel
DB_USERNAME=root
DB_PASSWORD=
LOG_STACK=single,otlp
OTEL_SERVICE_NAME=php84-laravel-keepsuit
OTEL_RESOURCE_ATTRIBUTES=service.namespace=php-demo
OTEL_EXPORTER_OTLP_ENDPOINT=http://otel-collector:4318
EOT

php artisan key:generate
php artisan migrate
php artisan db:seed
