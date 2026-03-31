#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "ext/standard/info.h"
#include "php_opentelemetry_php74.h"
#include "opentelemetry_php74_arginfo.h"
#include "otel_observer.h"
#include "stdlib.h"
#include "string.h"
#include "zend_closures.h"

static int check_conflict(HashTable *registry, const char *extension_name) {
    if (!extension_name || !*extension_name) {
        return 0;
    }
    zend_module_entry *module_entry;
    ZEND_HASH_FOREACH_PTR(registry, module_entry) {
        if (strcmp(module_entry->name, extension_name) == 0) {
            php_error_docref(NULL, E_NOTICE,
                             "Conflicting extension found (%s), "
                             "opentelemetry_php74 extension will be disabled",
                             extension_name);
            return 1;
        }
    }
    ZEND_HASH_FOREACH_END();
    return 0;
}

static void check_conflicts(void) {
    int conflict_found = 0;
    char *input = OTELPHP74_G(conflicts);

    if (!input || !*input) {
        return;
    }

    HashTable *registry = &module_registry;
    const char *s = NULL, *e = input;
    while (*e) {
        switch (*e) {
        case ' ':
        case ',':
            if (s) {
                size_t len = e - s;
                char *result = (char *)malloc((len + 1) * sizeof(char));
                strncpy(result, s, len);
                result[len] = '\0';
                if (check_conflict(registry, result)) {
                    conflict_found = 1;
                }
                free(result);
                s = NULL;
            }
            break;
        default:
            if (!s) {
                s = e;
            }
            break;
        }
        e++;
    }
    if (check_conflict(registry, s)) {
        conflict_found = 1;
    }

    OTELPHP74_G(disabled) = conflict_found;
}

ZEND_DECLARE_MODULE_GLOBALS(opentelemetry_php74)

PHP_INI_BEGIN()
STD_PHP_INI_ENTRY("opentelemetry_php74.conflicts", "", PHP_INI_ALL,
                   OnUpdateString, conflicts,
                   zend_opentelemetry_php74_globals,
                   opentelemetry_php74_globals)
PHP_INI_END()

PHP_FUNCTION(OpenTelemetryPHP74_Instrumentation_hook) {
    zend_string *class_name = NULL;
    zend_string *function_name = NULL;
    zval *pre = NULL;
    zval *post = NULL;

    ZEND_PARSE_PARAMETERS_START(2, 4)
        Z_PARAM_STR_EX(class_name, 1, 0)
        Z_PARAM_STR(function_name)
        Z_PARAM_OPTIONAL
        Z_PARAM_OBJECT_OF_CLASS_EX(pre, zend_ce_closure, 1, 0)
        Z_PARAM_OBJECT_OF_CLASS_EX(post, zend_ce_closure, 1, 0)
    ZEND_PARSE_PARAMETERS_END();

    RETURN_BOOL(add_observer(class_name, function_name, pre, post));
}

PHP_RINIT_FUNCTION(opentelemetry_php74) {
#if defined(ZTS) && defined(COMPILE_DL_OPENTELEMETRY_PHP74)
    ZEND_TSRMLS_CACHE_UPDATE();
#endif

    observer_globals_init();

    return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(opentelemetry_php74) {
    observer_globals_cleanup();

    return SUCCESS;
}

PHP_MINIT_FUNCTION(opentelemetry_php74) {
#if defined(ZTS) && defined(COMPILE_DL_OPENTELEMETRY_PHP74)
    ZEND_TSRMLS_CACHE_UPDATE();
#endif

    REGISTER_INI_ENTRIES();

    check_conflicts();

    if (!OTELPHP74_G(disabled)) {
        opentelemetry_observer_init(INIT_FUNC_ARGS_PASSTHRU);
    }

    return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(opentelemetry_php74) {
    if (!OTELPHP74_G(disabled)) {
        opentelemetry_observer_shutdown(SHUTDOWN_FUNC_ARGS_PASSTHRU);
    }

    UNREGISTER_INI_ENTRIES();

    return SUCCESS;
}

PHP_MINFO_FUNCTION(opentelemetry_php74) {
    php_info_print_table_start();
    php_info_print_table_row(2, "opentelemetry_php74 hooks",
                             OTELPHP74_G(disabled) ? "disabled (conflict)"
                                              : "enabled");
    php_info_print_table_row(2, "extension version",
                             PHP_OPENTELEMETRY_PHP74_VERSION);
    php_info_print_table_end();
    DISPLAY_INI_ENTRIES();
}

PHP_GINIT_FUNCTION(opentelemetry_php74) {
    ZEND_SECURE_ZERO(opentelemetry_php74_globals,
                     sizeof(*opentelemetry_php74_globals));
}

zend_module_entry opentelemetry_php74_module_entry = {
    STANDARD_MODULE_HEADER,
    "opentelemetry_php74",
    ext_functions,
    PHP_MINIT(opentelemetry_php74),
    PHP_MSHUTDOWN(opentelemetry_php74),
    PHP_RINIT(opentelemetry_php74),
    PHP_RSHUTDOWN(opentelemetry_php74),
    PHP_MINFO(opentelemetry_php74),
    PHP_OPENTELEMETRY_PHP74_VERSION,
    PHP_MODULE_GLOBALS(opentelemetry_php74),
    PHP_GINIT(opentelemetry_php74),
    NULL,
    NULL,
    STANDARD_MODULE_PROPERTIES_EX,
};

#ifdef COMPILE_DL_OPENTELEMETRY_PHP74
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
#endif
ZEND_GET_MODULE(opentelemetry_php74)
#endif
