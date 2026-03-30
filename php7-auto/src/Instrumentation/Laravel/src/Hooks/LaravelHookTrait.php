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

trait LaravelHookTrait
{
    /** @var LaravelHook */
    private static $instance;

    /** @var CachedInstrumentation */
    protected $instrumentation;

    protected function __construct(CachedInstrumentation $instrumentation)
    {
        $this->instrumentation = $instrumentation;
    }

    abstract public function instrument(): void;

    /** @psalm-suppress PossiblyUnusedReturnValue */
    public static function hook(CachedInstrumentation $instrumentation): LaravelHook
    {
        /** @psalm-suppress RedundantPropertyInitializationCheck */
        if (!isset(self::$instance)) {
            /** @phan-suppress-next-line PhanTypeInstantiateTraitStaticOrSelf,PhanTypeMismatchPropertyReal */
            self::$instance = new self($instrumentation);
            self::$instance->instrument();
        }

        return self::$instance;
    }
}
