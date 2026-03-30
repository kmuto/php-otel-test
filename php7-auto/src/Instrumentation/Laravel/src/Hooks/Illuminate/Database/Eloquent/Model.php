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

namespace OpenTelemetryPHP74\Instrumentation\Laravel\Hooks\Illuminate\Database\Eloquent;

use Illuminate\Database\Eloquent\Model as EloquentModel;
use OpenTelemetry\API\Trace\SpanKind;
use OpenTelemetry\Context\Context;
use OpenTelemetryPHP74\Instrumentation\Laravel\Hooks\LaravelHook;
use OpenTelemetryPHP74\Instrumentation\Laravel\Hooks\LaravelHookTrait;
use OpenTelemetryPHP74\Instrumentation\Laravel\Hooks\PostHookTrait;
use Throwable;

use function OpenTelemetryPHP74\Instrumentation\hook;

class Model implements LaravelHook
{
    use LaravelHookTrait;
    use PostHookTrait;

    public function instrument(): void
    {
        $this->hookFind();
        $this->hookPerformInsert();
        $this->hookPerformUpdate();
        $this->hookDelete();
        $this->hookGetModels();
        $this->hookDestroy();
        $this->hookRefresh();
    }

    private function hookFind(): void
    {
        /** @psalm-suppress UnusedFunctionCall */
        hook(
            \Illuminate\Database\Eloquent\Builder::class,
            'find',
            function ($builder, array $params, string $class, string $function, ?string $filename, ?int $lineno) {
                $model = $builder->getModel();
                $spanBuilder = $this->instrumentation
                    ->tracer()
                    ->spanBuilder(get_class($model) . '::find')
                    ->setSpanKind(SpanKind::KIND_INTERNAL)
                    ->setAttribute('code.function.name', sprintf('%s::%s', $class, $function))
                    ->setAttribute('code.file.path', $filename)
                    ->setAttribute('code.line.number', $lineno)
                    ->setAttribute('laravel.eloquent.model', get_class($model))
                    ->setAttribute('laravel.eloquent.table', $model->getTable())
                    ->setAttribute('laravel.eloquent.operation', 'find');

                $parent = Context::getCurrent();
                $span = $spanBuilder->startSpan();
                Context::storage()->attach($span->storeInContext($parent));

                return $params;
            },
            function ($builder, array $params, $result, ?Throwable $exception) {
                $this->endSpan($exception);
            }
        );
    }

    private function hookPerformUpdate(): void
    {
        /** @psalm-suppress UnusedFunctionCall */
        hook(
            EloquentModel::class,
            'performUpdate',
            function (EloquentModel $model, array $params, string $class, string $function, ?string $filename, ?int $lineno) {
                $builder = $this->instrumentation
                    ->tracer()
                    ->spanBuilder(get_class($model) . '::update')
                    ->setSpanKind(SpanKind::KIND_INTERNAL)
                    ->setAttribute('code.function.name', sprintf('%s::%s', $class, $function))
                    ->setAttribute('code.file.path', $filename)
                    ->setAttribute('code.line.number', $lineno)
                    ->setAttribute('laravel.eloquent.model', get_class($model))
                    ->setAttribute('laravel.eloquent.table', $model->getTable())
                    ->setAttribute('laravel.eloquent.operation', 'update');

                $parent = Context::getCurrent();
                $span = $builder->startSpan();
                Context::storage()->attach($span->storeInContext($parent));

                return $params;
            },
            function (EloquentModel $model, array $params, $result, ?Throwable $exception) {
                $this->endSpan($exception);
            }
        );
    }

    private function hookPerformInsert(): void
    {
        /** @psalm-suppress UnusedFunctionCall */
        hook(
            EloquentModel::class,
            'performInsert',
            function (EloquentModel $model, array $params, string $class, string $function, ?string $filename, ?int $lineno) {
                $builder = $this->instrumentation
                    ->tracer()
                    ->spanBuilder(get_class($model) . '::create')
                    ->setSpanKind(SpanKind::KIND_INTERNAL)
                    ->setAttribute('code.function.name', sprintf('%s::%s', $class, $function))
                    ->setAttribute('code.file.path', $filename)
                    ->setAttribute('code.line.number', $lineno)
                    ->setAttribute('laravel.eloquent.model', get_class($model))
                    ->setAttribute('laravel.eloquent.table', $model->getTable())
                    ->setAttribute('laravel.eloquent.operation', 'create');

                $parent = Context::getCurrent();
                $span = $builder->startSpan();
                Context::storage()->attach($span->storeInContext($parent));

                return $params;
            },
            function (EloquentModel $model, array $params, $result, ?Throwable $exception) {
                $this->endSpan($exception);
            }
        );
    }

    private function hookDelete(): void
    {
        /** @psalm-suppress UnusedFunctionCall */
        hook(
            EloquentModel::class,
            'delete',
            function (EloquentModel $model, array $params, string $class, string $function, ?string $filename, ?int $lineno) {
                $builder = $this->instrumentation
                    ->tracer()
                    ->spanBuilder(get_class($model) . '::delete')
                    ->setSpanKind(SpanKind::KIND_INTERNAL)
                    ->setAttribute('code.function.name', sprintf('%s::%s', $class, $function))
                    ->setAttribute('code.file.path', $filename)
                    ->setAttribute('code.line.number', $lineno)
                    ->setAttribute('laravel.eloquent.model', get_class($model))
                    ->setAttribute('laravel.eloquent.table', $model->getTable())
                    ->setAttribute('laravel.eloquent.operation', 'delete');

                $parent = Context::getCurrent();
                $span = $builder->startSpan();
                Context::storage()->attach($span->storeInContext($parent));

                return $params;
            },
            function (EloquentModel $model, array $params, $result, ?Throwable $exception) {
                $this->endSpan($exception);
            }
        );
    }

    private function hookGetModels(): void
    {
        /** @psalm-suppress UnusedFunctionCall */
        hook(
            \Illuminate\Database\Eloquent\Builder::class,
            'getModels',
            function ($builder, array $params, string $class, string $function, ?string $filename, ?int $lineno) {
                $model = $builder->getModel();
                $spanBuilder = $this->instrumentation
                    ->tracer()
                    ->spanBuilder(get_class($model) . '::get')
                    ->setSpanKind(SpanKind::KIND_INTERNAL)
                    ->setAttribute('code.function.name', sprintf('%s::%s', $class, $function))
                    ->setAttribute('code.file.path', $filename)
                    ->setAttribute('code.line.number', $lineno)
                    ->setAttribute('laravel.eloquent.model', get_class($model))
                    ->setAttribute('laravel.eloquent.table', $model->getTable())
                    ->setAttribute('laravel.eloquent.operation', 'get')
                    ->setAttribute('db.statement', $builder->getQuery()->toSql());

                $parent = Context::getCurrent();
                $span = $spanBuilder->startSpan();
                Context::storage()->attach($span->storeInContext($parent));

                return $params;
            },
            function ($builder, array $params, $result, ?Throwable $exception) {
                $this->endSpan($exception);
            }
        );
    }

    private function hookDestroy(): void
    {
        /** @psalm-suppress UnusedFunctionCall */
        hook(
            EloquentModel::class,
            'destroy',
            function (string $modelClassName, array $params, string $class, string $function, ?string $filename, ?int $lineno) {
                // The class-string is passed to the $model argument, because \Illuminate\Database\Eloquent\Model::destroy is static method.
                // Therefore, create a class instance from a class-string, and then get the table name from the getTable function.
                $model = new $modelClassName();

                $builder = $this->instrumentation
                    ->tracer()
                    ->spanBuilder(get_class($model) . '::destroy')
                    ->setSpanKind(SpanKind::KIND_INTERNAL)
                    ->setAttribute('code.function.name', sprintf('%s::%s', $class, $function))
                    ->setAttribute('code.file.path', $filename)
                    ->setAttribute('code.line.number', $lineno)
                    ->setAttribute('laravel.eloquent.model', get_class($model))
                    ->setAttribute('laravel.eloquent.table', $model->getTable())
                    ->setAttribute('laravel.eloquent.operation', 'destroy');

                $parent = Context::getCurrent();
                $span = $builder->startSpan();
                Context::storage()->attach($span->storeInContext($parent));

                return $params;
            },
            function ($model, array $params, $result, ?Throwable $exception) {
                $this->endSpan($exception);
            }
        );
    }

    private function hookRefresh(): void
    {
        /** @psalm-suppress UnusedFunctionCall */
        hook(
            EloquentModel::class,
            'refresh',
            function (EloquentModel $model, array $params, string $class, string $function, ?string $filename, ?int $lineno) {
                $builder = $this->instrumentation
                    ->tracer()
                    ->spanBuilder(get_class($model) . '::refresh')
                    ->setSpanKind(SpanKind::KIND_INTERNAL)
                    ->setAttribute('code.function.name', sprintf('%s::%s', $class, $function))
                    ->setAttribute('code.file.path', $filename)
                    ->setAttribute('code.line.number', $lineno)
                    ->setAttribute('laravel.eloquent.model', get_class($model))
                    ->setAttribute('laravel.eloquent.table', $model->getTable())
                    ->setAttribute('laravel.eloquent.operation', 'refresh');

                $parent = Context::getCurrent();
                $span = $builder->startSpan();
                Context::storage()->attach($span->storeInContext($parent));

                return $params;
            },
            function (EloquentModel $model, array $params, $result, ?Throwable $exception) {
                $this->endSpan($exception);
            }
        );
    }
}
