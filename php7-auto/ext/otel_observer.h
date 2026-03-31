#ifndef OTEL_OBSERVER_H
#define OTEL_OBSERVER_H

#include "php.h"

void opentelemetry_observer_init(INIT_FUNC_ARGS);
void opentelemetry_observer_shutdown(SHUTDOWN_FUNC_ARGS);
void observer_globals_init(void);
void observer_globals_cleanup(void);

zend_bool add_observer(zend_string *cn, zend_string *fn, zval *pre_hook,
                       zval *post_hook);

#endif /* OTEL_OBSERVER_H */
