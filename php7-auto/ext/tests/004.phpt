--TEST--
Pre hook receives correct parameters
--EXTENSIONS--
opentelemetry_php74
--FILE--
<?php
use function OpenTelemetryPHP74\Instrumentation\hook;

function add(int $a, int $b): int {
    return $a + $b;
}

hook(
    null,
    'add',
    function ($obj, array $params, ?string $class, string $function, ?string $filename, ?int $lineno) {
        var_dump($obj);
        var_dump($params);
        var_dump($class);
        var_dump($function);
        var_dump(is_string($filename));
        var_dump(is_int($lineno));
    }
);

add(1, 2);
?>
--EXPECT--
NULL
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}
NULL
string(3) "add"
bool(true)
bool(true)
