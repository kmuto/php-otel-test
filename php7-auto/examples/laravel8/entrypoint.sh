#!/bin/bash
set -e

# Run setup if database doesn't exist yet
#if [ ! -f /app/database/database.sqlite ]; then
#    echo "Running initial setup..."
#    chmod +x /app/setup.sh
#    /app/setup.sh
#fi

# Fix permissions for php-fpm
chown -R www-data:www-data /app/storage /app/bootstrap/cache /app/database 2>/dev/null || true

exec "$@"
