<?php

declare(strict_types=1);

/**
 * Based on code from open-telemetry/opentelemetry-php-contrib
 * Copyright 2021 opentelemetry-php-contrib contributors
 * Licensed under the Apache License, Version 2.0
 */

use OpenTelemetryPHP74\Instrumentation\Laravel\LaravelInstrumentation;
use OpenTelemetry\SDK\Sdk;

if (class_exists(Sdk::class) && Sdk::isInstrumentationDisabled(LaravelInstrumentation::NAME) === true) {
    return;
}

if (extension_loaded('opentelemetry_php74') === false) {
    trigger_error('The opentelemetry_php74 extension must be loaded in order to autoload the OpenTelemetry Laravel auto-instrumentation', E_USER_WARNING);

    return;
}

LaravelInstrumentation::register();
