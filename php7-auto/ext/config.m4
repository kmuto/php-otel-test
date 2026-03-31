dnl config.m4 for extension opentelemetry_php74

PHP_ARG_ENABLE([opentelemetry_php74],
  [whether to enable opentelemetry_php74 support],
  [AS_HELP_STRING([--enable-opentelemetry_php74],
    [Enable opentelemetry_php74 support])],
  [no])

if test "$PHP_OPENTELEMETRY_PHP74" != "no"; then
  AC_DEFINE(HAVE_OPENTELEMETRY_PHP74, 1, [ Have opentelemetry_php74 support ])

  PHP_NEW_EXTENSION(opentelemetry_php74, opentelemetry_php74.c otel_observer.c, $ext_shared,, "-Wall -Wextra -Wno-unused-parameter")
fi
