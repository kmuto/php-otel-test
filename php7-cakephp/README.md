# PHP7 + CakePHP + OBI

## 初期
```
docker compose run -it --rm app /bin/bash
cd php7-cakephp
composer install
exit
./init_sql.sh
```

`php7-cakephp/config/app_local.php`を編集

```
        'default' => [
            'host' => 'db',

            'username' => 'cake_user',
            'password' => 'password',

            'database' => 'cake_app',
```

## 実行
```
MACKEREL_APIKEY=APIキー docker compose up
```

http://localhost:8080/php8-cakephp にアクセス

## 計装メモ
- データベースアクセスが抽象化されているのでmysqliのほかPDO (PHP Data Objects) のライブラリ計装が必要
