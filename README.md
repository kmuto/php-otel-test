# PHP LaravelのOBI / ゼロコード計装のサンプル

- DBクエリ、外部HTTP API通信の様子の違いを見るサンプル
- Laravelのバージョンが異なるためか、7.4ではセッションのクエリが発行されない

## PHP 7.4 OBI計装
```
cd php7
docker compose build
docker compose up -d db
docker compose run --rm php74-laravel ./setup.sh
MACKEREL_APIKEY=<YOUR MACKEREL API KEY> docker compose up
```

- http://localhost:8000/
- http://localhost:8000/hello
- http://localhost:8000/call
- http://localhost:8000/query

Docker環境をOBIで見ている都合で、サービス名は常に「php」となる。ネイティブでPHPを実行しているのであれば`OTEL_SERVICE_NAME`で設定できる。

## PHP 8.4 ゼロコード計装

```
cd php8
docker compose build
docker compose up -d db
docker compose run --rm php84-laravel ./setup.sh
MACKEREL_APIKEY=<YOUR MACKEREL API KEY> docker compose up
```

- http://localhost:8000/
- http://localhost:8000/hello
- http://localhost:8000/call
- http://localhost:8000/query

サービス名は「php84-laravel」としている。
