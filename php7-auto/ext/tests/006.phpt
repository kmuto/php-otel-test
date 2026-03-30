--TEST--
Hook an internal (built-in) function
--EXTENSIONS--
opentelemetry_php74
--FILE--
<?php
use function OpenTelemetryPHP74\Instrumentation\hook;

hook(
    null,
    'array_merge',
    function ($obj, array $params, ?string $class, string $function, ?string $filename, ?int $lineno) {
        echo "PRE: function={$function}\n";
    },
    function ($obj, array $params, $returnValue, $exception) {
        echo "POST: count=" . count($returnValue) . "\n";
    }
);

$result = array_merge([1, 2], [3, 4]);
var_dump($result);
?>
--EXPECT--
PRE: function=array_merge
POST: count=4
array(4) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
  [3]=>
  int(4)
}
