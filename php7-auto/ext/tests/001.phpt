--TEST--
Check if opentelemetry_php74 extension is loaded
--EXTENSIONS--
opentelemetry_php74
--FILE--
<?php
printf('The extension "opentelemetry_php74" is available, version %s', phpversion('opentelemetry_php74'));
?>
--EXPECTF--
The extension "opentelemetry_php74" is available, version %s
