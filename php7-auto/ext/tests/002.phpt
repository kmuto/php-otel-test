--TEST--
Hook a user function with pre and post hooks
--EXTENSIONS--
opentelemetry_php74
--FILE--
<?php
use function OpenTelemetryPHP74\Instrumentation\hook;

function greet(string $name): string {
    return "Hello, {$name}!";
}

$result = hook(
    null,
    'greet',
    function ($obj, array $params, ?string $class, string $function, ?string $filename, ?int $lineno) {
        echo "PRE: function={$function} params=" . implode(',', $params) . "\n";
    },
    function ($obj, array $params, $returnValue, $exception) {
        echo "POST: returnValue={$returnValue}\n";
    }
);

var_dump($result);
echo greet('World') . "\n";
?>
--EXPECT--
bool(true)
PRE: function=greet params=World
POST: returnValue=Hello, World!
Hello, World!
