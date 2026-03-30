#ifndef OPENTELEMETRY_PHP74_ARGINFO_H
#define OPENTELEMETRY_PHP74_ARGINFO_H

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(
    arginfo_OpenTelemetryPHP74_Instrumentation_hook, 0, 2, _IS_BOOL, 0)
    ZEND_ARG_TYPE_INFO(0, class, IS_STRING, 1)
    ZEND_ARG_TYPE_INFO(0, function, IS_STRING, 0)
    ZEND_ARG_OBJ_INFO(0, pre, Closure, 1)
    ZEND_ARG_OBJ_INFO(0, post, Closure, 1)
ZEND_END_ARG_INFO()

ZEND_FUNCTION(OpenTelemetryPHP74_Instrumentation_hook);

static const zend_function_entry ext_functions[] = {
    ZEND_NS_FALIAS(
        "OpenTelemetryPHP74\\Instrumentation",
        hook,
        OpenTelemetryPHP74_Instrumentation_hook,
        arginfo_OpenTelemetryPHP74_Instrumentation_hook)
    ZEND_FE_END
};

#endif
