<?php

declare(strict_types=1);

/**
 * Based on code from opentelemetry/opentelemetry-php-contrib
 * Copyright 2021 opentelemetry-php-contrib contributors
 * Licensed under the Apache License, Version 2.0
 * 
 * Modifications:
 * - Added support for PHP 7.4
 * - Updated to use OpenTelemetry extension for PHP 7.4
 */

namespace OpenTelemetryPHP74\Instrumentation\Laravel\Hooks;

use OpenTelemetry\API\Instrumentation\CachedInstrumentation;

interface LaravelHook
{
    /** @psalm-suppress PossiblyUnusedReturnValue */
    public static function hook(CachedInstrumentation $instrumentation): LaravelHook;

    public function instrument(): void;
}
