--TEST--
Post hook receives exception information
--EXTENSIONS--
opentelemetry_php74
--FILE--
<?php
use function OpenTelemetryPHP74\Instrumentation\hook;

function throws(): void {
    throw new \RuntimeException('test exception');
}

hook(
    null,
    'throws',
    null,
    function ($obj, array $params, $returnValue, $exception) {
        echo "POST: exception=" . get_class($exception) . " message=" . $exception->getMessage() . "\n";
    }
);

try {
    throws();
} catch (\RuntimeException $e) {
    echo "CAUGHT: " . $e->getMessage() . "\n";
}
?>
--EXPECT--
POST: exception=RuntimeException message=test exception
CAUGHT: test exception
