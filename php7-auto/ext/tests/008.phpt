--TEST--
Multiple hooks can be registered for the same function
--EXTENSIONS--
opentelemetry_php74
--FILE--
<?php
use function OpenTelemetryPHP74\Instrumentation\hook;

function target(): string {
    return 'result';
}

hook(
    null,
    'target',
    function ($obj, array $params) {
        echo "PRE hook 1\n";
    },
    function ($obj, array $params, $returnValue, $exception) {
        echo "POST hook 1\n";
    }
);

hook(
    null,
    'target',
    function ($obj, array $params) {
        echo "PRE hook 2\n";
    },
    function ($obj, array $params, $returnValue, $exception) {
        echo "POST hook 2\n";
    }
);

echo target() . "\n";
?>
--EXPECT--
PRE hook 1
PRE hook 2
POST hook 2
POST hook 1
result
