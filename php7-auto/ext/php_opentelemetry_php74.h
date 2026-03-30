#ifndef PHP_OPENTELEMETRY_PHP74_H
#define PHP_OPENTELEMETRY_PHP74_H

extern zend_module_entry opentelemetry_php74_module_entry;
#define phpext_opentelemetry_php74_ptr &opentelemetry_php74_module_entry

ZEND_BEGIN_MODULE_GLOBALS(opentelemetry_php74)
    HashTable *observer_class_lookup;
    HashTable *observer_function_lookup;
    char *conflicts;
    int disabled;
ZEND_END_MODULE_GLOBALS(opentelemetry_php74)

ZEND_EXTERN_MODULE_GLOBALS(opentelemetry_php74)

#define OTELPHP74_G(v) ZEND_MODULE_GLOBALS_ACCESSOR(opentelemetry_php74, v)

#define PHP_OPENTELEMETRY_PHP74_VERSION "0.1.0"

#if defined(ZTS) && defined(COMPILE_DL_OPENTELEMETRY_PHP74)
ZEND_TSRMLS_CACHE_EXTERN()
#endif

#endif /* PHP_OPENTELEMETRY_PHP74_H */
