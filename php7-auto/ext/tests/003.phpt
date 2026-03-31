--TEST--
Hook a class method with pre and post hooks
--EXTENSIONS--
opentelemetry_php74
--FILE--
<?php
use function OpenTelemetryPHP74\Instrumentation\hook;

class Greeter {
    public function greet(string $name): string {
        return "Hello, {$name}!";
    }
}

hook(
    'Greeter',
    'greet',
    function ($obj, array $params, ?string $class, string $function, ?string $filename, ?int $lineno) {
        echo "PRE: class={$class} function={$function}\n";
    },
    function ($obj, array $params, $returnValue, $exception) {
        echo "POST: returnValue={$returnValue}\n";
    }
);

$greeter = new Greeter();
echo $greeter->greet('World') . "\n";
?>
--EXPECT--
PRE: class=Greeter function=greet
POST: returnValue=Hello, World!
Hello, World!
