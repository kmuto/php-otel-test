--TEST--
Post hook can modify return value
--EXTENSIONS--
opentelemetry_php74
--FILE--
<?php
use function OpenTelemetryPHP74\Instrumentation\hook;

function get_value(): string {
    return 'original';
}

hook(
    null,
    'get_value',
    null,
    function ($obj, array $params, $returnValue, $exception): string {
        return 'modified';
    }
);

echo get_value() . "\n";
?>
--EXPECT--
modified
