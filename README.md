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

`setup.sh`は初回のみでよい。

- http://localhost:8000/
- http://localhost:8000/fruits
- http://localhost:8000/call
- http://localhost:8000/query

Docker環境をOBIで見ている都合で、サービス名は常に「php」となる。ネイティブでPHPを実行しているのであれば`OTEL_SERVICE_NAME`で設定できる。

アプリケーションには計装を入れておらず、OBIで外形的にプロセスの通信を監視している。

- バイナリプロトコルの監視に何らか問題がありそう。`PDO::ATTR_EMULATE_PREPARES => true,`を`config/database.php`に指定している。macOS+Rancherでは動作するが、Linuxでは効果が出ていない

## PHP 8.4 ゼロコード計装
```
cd php8
docker compose build
docker compose up -d db
docker compose run --rm php84-laravel ./setup.sh
MACKEREL_APIKEY=<YOUR MACKEREL API KEY> docker compose up
```

`setup.sh`は初回のみでよい。

- http://localhost:8001/
- http://localhost:8001/fruits
- http://localhost:8001/call
- http://localhost:8001/query

サービス名は「php84-laravel」としている。

- Laravel・PDOのゼロコード計装をしているため、細かな計装がされている。

## PHP 7.4 ゼロコード計装
```
cd php7-auto/examples/laravel8
docker compose build
docker compose up -d db
docker compose run --rm app ./setup.sh
MACKEREL_APIKEY=<YOUR MACKEREL API KEY> docker compose up
```

`setup.sh`は初回のみでよい。

- http://localhost:8002/
- http://localhost:8002/fruits
- http://localhost:8002/call
- http://localhost:8002/query

サービス名は「laravel8-php74-demo」としている。

- Laravelのゼロコード計装をしているため、細かな計装がされている。PDOのゼロコード計装がないのでphp8のほうに比べると少しスパンが少ない
- とはいえphp8同程度に必要な情報は可視化される
