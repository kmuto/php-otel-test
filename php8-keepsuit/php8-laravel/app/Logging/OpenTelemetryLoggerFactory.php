<?php

namespace App\Logging;

use Monolog\Logger;
use OpenTelemetry\API\Globals;
use OpenTelemetry\Contrib\Logs\Monolog\Handler as OTelHandler;
use OpenTelemetry\Contrib\Logs\Monolog\Processor as OTelProcessor;

class OpenTelemetryLoggerFactory
{
    /**
     * カスタムMonologインスタンスの作成
     */
    public function __invoke(array $config): Logger
    {
        // 1. OpenTelemetryのグローバルなLoggerProviderを取得
        // (SDKが正しく初期化されていれば、ここからプロバイダーが取れます)
        $loggerProvider = Globals::loggerProvider();

        // 2. Monolog用のOpenTelemetryハンドラーを作成
        $handler = new OTelHandler(
            $loggerProvider,
            $config['level'] ?? 'debug'
        );

        // 3. ログに「トレースID」や「スパンID」を自動注入するプロセッサーを作成
        // これにより、もしトレース（リクエスト追跡）が走っていれば、ログとタイムラインが紐付きます
        $processor = new OTelProcessor();

        // 4. Monologにハンドラーとプロセッサーを詰め込んで返す
        $logger = new Logger('otlp');
        $logger->pushHandler($handler);
        $logger->pushProcessor($processor);

        return $logger;
    }
}
