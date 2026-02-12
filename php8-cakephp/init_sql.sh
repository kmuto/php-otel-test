# composerのインストール
# docker compose exec app /bin/bash
# cd /var/www/html/php8-cakephp
# composer install
#pecl install opentelemetry
#composer require -n open-telemetry/sdk open-telemetry/opentelemetry-auto-cakephp open-telemetry/exporter-otlp open-telemetry/opentelemetry-auto-mysqli
# MySQLコンテナに入る
docker compose exec -T db mysql -u cake_user -ppassword cake_app <<EOF
CREATE TABLE fruits (
    id INT AUTO_INCREMENT PRIMARY KEY,
    name VARCHAR(255) NOT NULL,
    created DATETIME,
    modified DATETIME
);

INSERT INTO fruits (name, created, modified) VALUES 
('Apple', NOW(), NOW()),
('Orange', NOW(), NOW()),
('Grape', NOW(), NOW()),
('Banana', NOW(), NOW()),
('Strawberry', NOW(), NOW());
EOF
