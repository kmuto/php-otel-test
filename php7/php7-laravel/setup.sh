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
EOT

php artisan key:generate
php artisan migrate

php create-table.php
