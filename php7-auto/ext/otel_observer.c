#include "php.h"
#include "otel_observer.h"
#include "zend_execute.h"
#include "zend_exceptions.h"
#include "php_opentelemetry_php74.h"

typedef struct otel_observer {
    zend_llist pre_hooks;
    zend_llist post_hooks;
} otel_observer;

typedef struct otel_exception_state {
    zend_object *exception;
    zend_object *prev_exception;
    const zend_op *opline_before_exception;
} otel_exception_state;

static void (*original_zend_execute_ex)(zend_execute_data *execute_data);
static void (*original_zend_execute_internal)(zend_execute_data *execute_data,
                                              zval *return_value);

static void *hash_find_ptr_lc(HashTable *ht, zend_string *key) {
    zend_string *lc = zend_string_tolower(key);
    void *result = zend_hash_find_ptr(ht, lc);
    zend_string_release(lc);
    return result;
}

static inline void func_get_this_or_called_scope(zval *zv,
                                                  zend_execute_data *ex) {
    if (ex->func->common.scope) {
        if (ex->func->common.fn_flags & ZEND_ACC_STATIC) {
            zend_class_entry *called_scope = zend_get_called_scope(ex);
            if (called_scope) {
                ZVAL_STR_COPY(zv, called_scope->name);
            } else {
                ZVAL_NULL(zv);
            }
        } else {
            zend_object *this = zend_get_this_object(ex);
            if (this) {
                ZVAL_OBJ(zv, this);
                Z_ADDREF_P(zv);
            } else {
                ZVAL_NULL(zv);
            }
        }
    } else {
        ZVAL_NULL(zv);
    }
}

static inline void func_get_function_name(zval *zv, zend_execute_data *ex) {
    if (ex->func->common.function_name) {
        ZVAL_STR_COPY(zv, ex->func->common.function_name);
    } else {
        ZVAL_NULL(zv);
    }
}

static inline void func_get_declaring_scope(zval *zv,
                                             zend_execute_data *ex) {
    if (ex->func->common.scope) {
        ZVAL_STR_COPY(zv, ex->func->common.scope->name);
    } else {
        ZVAL_NULL(zv);
    }
}

static inline void func_get_filename(zval *zv, zend_execute_data *ex) {
    if (ex->func->type == ZEND_USER_FUNCTION) {
        ZVAL_STR_COPY(zv, ex->func->op_array.filename);
    } else {
        ZVAL_NULL(zv);
    }
}

static inline void func_get_lineno(zval *zv, zend_execute_data *ex) {
    if (ex->func->type == ZEND_USER_FUNCTION) {
        ZVAL_LONG(zv, ex->func->op_array.line_start);
    } else {
        ZVAL_NULL(zv);
    }
}

static void func_get_args(zval *zv, zend_execute_data *ex) {
    uint32_t arg_count = ZEND_CALL_NUM_ARGS(ex);
    uint32_t first_extra_arg;
    uint32_t i;
    zval *p, *q;

    if (arg_count) {
        array_init_size(zv, arg_count);
        if (ex->func->type == ZEND_INTERNAL_FUNCTION) {
            first_extra_arg = arg_count;
        } else {
            first_extra_arg = ex->func->op_array.num_args;
        }
        zend_hash_real_init_packed(Z_ARRVAL_P(zv));
        ZEND_HASH_FILL_PACKED(Z_ARRVAL_P(zv)) {
            i = 0;
            p = ZEND_CALL_ARG(ex, 1);
            if (arg_count > first_extra_arg) {
                while (i < first_extra_arg) {
                    q = p;
                    if (EXPECTED(Z_TYPE_INFO_P(q) != IS_UNDEF)) {
                        ZVAL_DEREF(q);
                        if (Z_OPT_REFCOUNTED_P(q)) {
                            Z_ADDREF_P(q);
                        }
                        ZEND_HASH_FILL_SET(q);
                    } else {
                        ZEND_HASH_FILL_SET_NULL();
                    }
                    ZEND_HASH_FILL_NEXT();
                    p++;
                    i++;
                }
                p = ZEND_CALL_VAR_NUM(ex, ex->func->op_array.last_var +
                                              ex->func->op_array.T);
            }
            while (i < arg_count) {
                q = p;
                if (EXPECTED(Z_TYPE_INFO_P(q) != IS_UNDEF)) {
                    ZVAL_DEREF(q);
                    if (Z_OPT_REFCOUNTED_P(q)) {
                        Z_ADDREF_P(q);
                    }
                    ZEND_HASH_FILL_SET(q);
                } else {
                    ZEND_HASH_FILL_SET_NULL();
                }
                ZEND_HASH_FILL_NEXT();
                p++;
                i++;
            }
        }
        ZEND_HASH_FILL_END();
        Z_ARRVAL_P(zv)->nNumOfElements = arg_count;
    } else {
        ZVAL_EMPTY_ARRAY(zv);
    }
}

static inline void func_get_retval(zval *zv, zval *retval) {
    if (!retval || Z_ISUNDEF_P(retval)) {
        ZVAL_NULL(zv);
    } else {
        ZVAL_COPY(zv, retval);
    }
}

static inline void func_get_exception(zval *zv) {
    zend_object *exception = EG(exception);
    if (exception) {
        ZVAL_OBJ(zv, exception);
        Z_ADDREF_P(zv);
    } else {
        ZVAL_NULL(zv);
    }
}

static void exception_isolation_start(otel_exception_state *save_state) {
    save_state->exception = EG(exception);
    save_state->prev_exception = EG(prev_exception);
    save_state->opline_before_exception = EG(opline_before_exception);

    EG(exception) = NULL;
    EG(prev_exception) = NULL;
    EG(opline_before_exception) = NULL;
}

static zend_object *exception_isolation_end(otel_exception_state *save_state) {
    zend_object *suppressed = EG(exception);

    EG(exception) = NULL;
    zend_clear_exception();

    EG(exception) = save_state->exception;
    EG(prev_exception) = save_state->prev_exception;
    EG(opline_before_exception) = save_state->opline_before_exception;

    return suppressed;
}

static const char *zval_get_chars(zval *zv) {
    if (zv != NULL && Z_TYPE_P(zv) == IS_STRING) {
        return Z_STRVAL_P(zv);
    }
    return "null";
}

static void exception_isolation_handle_exception(zend_object *suppressed,
                                                 zval *class_name,
                                                 zval *function_name,
                                                 const char *type) {
    zval exception_zv;
    zend_class_entry *exception_base;
    zval return_value;
    zval *message;

    ZVAL_UNDEF(&return_value);

    if (suppressed == NULL) {
        return;
    }

    ZVAL_OBJ(&exception_zv, suppressed);
    exception_base = zend_get_exception_base(&exception_zv);
    message = zend_read_property_ex(exception_base, &exception_zv,
                                    ZSTR_KNOWN(ZEND_STR_MESSAGE), 1,
                                    &return_value);

    php_error_docref(NULL, E_CORE_WARNING,
                     "OpenTelemetry: %s threw exception,"
                     " class=%s function=%s message=%s",
                     type, zval_get_chars(class_name),
                     zval_get_chars(function_name), zval_get_chars(message));

    zval_ptr_dtor(&return_value);

    OBJ_RELEASE(suppressed);
}

static zend_bool find_observers(HashTable *ht, zend_string *n,
                                zend_llist *pre_hooks,
                                zend_llist *post_hooks) {
    otel_observer *observer = hash_find_ptr_lc(ht, n);
    if (observer) {
        zend_llist_element *element;
        for (element = observer->pre_hooks.head; element;
             element = element->next) {
            zval_add_ref((zval *)&element->data);
            zend_llist_add_element(pre_hooks, &element->data);
        }
        for (element = observer->post_hooks.head; element;
             element = element->next) {
            zval_add_ref((zval *)&element->data);
            zend_llist_add_element(post_hooks, &element->data);
        }
        return 1;
    }
    return 0;
}

static void find_class_observers(HashTable *ht,
                                 HashTable *type_visited_lookup,
                                 zend_class_entry *ce,
                                 zend_llist *pre_hooks,
                                 zend_llist *post_hooks) {
    for (; ce; ce = ce->parent) {
        if (zend_hash_exists(type_visited_lookup, ce->name)) {
            continue;
        }
        zend_hash_add_empty_element(type_visited_lookup, ce->name);
        find_observers(ht, ce->name, pre_hooks, post_hooks);
        {
            uint32_t i;
            for (i = 0; i < ce->num_interfaces; i++) {
                find_class_observers(ht, type_visited_lookup,
                                     ce->interfaces[i], pre_hooks,
                                     post_hooks);
            }
        }
    }
}

static void find_method_observers(HashTable *ht, zend_class_entry *ce,
                                  zend_string *fn, zend_llist *pre_hooks,
                                  zend_llist *post_hooks) {
    HashTable type_visited_lookup;
    HashTable *lookup;
    zend_string *lc;

    zend_hash_init(&type_visited_lookup, 8, NULL, NULL, 0);
    lc = zend_string_tolower(fn);
    lookup = zend_hash_find_ptr(OTELPHP74_G(observer_class_lookup), lc);
    zend_string_release(lc);
    if (lookup) {
        find_class_observers(lookup, &type_visited_lookup, ce, pre_hooks,
                             post_hooks);
    }
    zend_hash_destroy(&type_visited_lookup);
}

static otel_observer *resolve_observer(zend_execute_data *execute_data) {
    zend_function *fbc = execute_data->func;

    if (!fbc->common.function_name) {
        return NULL;
    }

    /* Check if globals are initialized (RINIT may not have been called yet) */
    if (OTELPHP74_G(observer_class_lookup) == NULL) {
        return NULL;
    }

    otel_observer observer_instance;
    zend_llist_init(&observer_instance.pre_hooks, sizeof(zval),
                    (llist_dtor_func_t)zval_ptr_dtor, 0);
    zend_llist_init(&observer_instance.post_hooks, sizeof(zval),
                    (llist_dtor_func_t)zval_ptr_dtor, 0);

    if (fbc->common.scope) {
        find_method_observers(OTELPHP74_G(observer_class_lookup),
                              fbc->common.scope, fbc->common.function_name,
                              &observer_instance.pre_hooks,
                              &observer_instance.post_hooks);
    } else {
        find_observers(OTELPHP74_G(observer_function_lookup),
                       fbc->common.function_name,
                       &observer_instance.pre_hooks,
                       &observer_instance.post_hooks);
    }

    if (!zend_llist_count(&observer_instance.pre_hooks) &&
        !zend_llist_count(&observer_instance.post_hooks)) {
        zend_llist_destroy(&observer_instance.pre_hooks);
        zend_llist_destroy(&observer_instance.post_hooks);
        return NULL;
    }

    otel_observer *observer = emalloc(sizeof(otel_observer));
    observer->pre_hooks = observer_instance.pre_hooks;
    observer->post_hooks = observer_instance.post_hooks;

    return observer;
}

static void observer_begin(zend_execute_data *execute_data,
                           zend_llist *hooks) {
    zval params[6];
    uint32_t param_count = 6;
    zend_llist_element *element;

    if (!zend_llist_count(hooks)) {
        return;
    }

    func_get_this_or_called_scope(&params[0], execute_data);
    func_get_args(&params[1], execute_data);
    func_get_declaring_scope(&params[2], execute_data);
    func_get_function_name(&params[3], execute_data);
    func_get_filename(&params[4], execute_data);
    func_get_lineno(&params[5], execute_data);

    for (element = hooks->head; element; element = element->next) {
        zend_fcall_info fci = empty_fcall_info;
        zend_fcall_info_cache fcc = empty_fcall_info_cache;

        if (zend_fcall_info_init((zval *)element->data, 0, &fci, &fcc, NULL,
                                 NULL) != SUCCESS) {
            php_error_docref(NULL, E_WARNING,
                             "Failed to initialize pre hook callable");
            continue;
        }

        zval ret;
        ZVAL_UNDEF(&ret);
        fci.param_count = param_count;
        fci.params = params;
        fci.retval = &ret;
#if PHP_VERSION_ID >= 80000
        fci.named_params = NULL;
#else
        fci.no_separation = 1;
#endif

        otel_exception_state save_state;
        exception_isolation_start(&save_state);

        if (zend_call_function(&fci, &fcc) == SUCCESS) {
            /* If pre hook returns an array, use it to modify arguments */
            if (Z_TYPE(ret) == IS_ARRAY &&
                !zend_is_identical(&ret, &params[1])) {
                zend_ulong idx;
                zval *val;
                uint32_t provided = ZEND_CALL_NUM_ARGS(execute_data);
                uint32_t first_extra_arg;

                if (execute_data->func->type == ZEND_INTERNAL_FUNCTION) {
                    first_extra_arg = provided;
                } else {
                    first_extra_arg = execute_data->func->op_array.num_args;
                }

                ZEND_HASH_FOREACH_NUM_KEY_VAL(Z_ARR(ret), idx, val) {
                    if (idx < provided) {
                        zval *target;
                        if (idx < first_extra_arg) {
                            target = ZEND_CALL_ARG(execute_data, idx + 1);
                        } else {
                            target = ZEND_CALL_VAR_NUM(
                                execute_data,
                                execute_data->func->op_array.last_var +
                                    execute_data->func->op_array.T +
                                    (idx - first_extra_arg));
                        }
                        zval_ptr_dtor(target);
                        ZVAL_COPY(target, val);

                        /* Also update the params[1] array */
                        if (Z_TYPE(params[1]) == IS_ARRAY) {
                            Z_TRY_ADDREF_P(val);
                            zend_hash_index_update(Z_ARR(params[1]), idx, val);
                        }
                    }
                }
                ZEND_HASH_FOREACH_END();
            }
        }

        {
            zend_object *suppressed = exception_isolation_end(&save_state);
            exception_isolation_handle_exception(suppressed, &params[2],
                                                 &params[3], "pre hook");
        }

        zval_dtor(&ret);
    }

    {
        uint32_t i;
        for (i = 0; i < param_count; i++) {
            zval_dtor(&params[i]);
        }
    }
}

static void observer_end(zend_execute_data *execute_data, zval *retval,
                         zend_llist *hooks) {
    zval params[6];
    uint32_t param_count = 6;
    zend_llist_element *element;

    if (!zend_llist_count(hooks)) {
        return;
    }

    func_get_this_or_called_scope(&params[0], execute_data);
    func_get_args(&params[1], execute_data);
    func_get_retval(&params[2], retval);
    func_get_exception(&params[3]);
    func_get_declaring_scope(&params[4], execute_data);
    func_get_function_name(&params[5], execute_data);

    for (element = hooks->tail; element; element = element->prev) {
        zend_fcall_info fci = empty_fcall_info;
        zend_fcall_info_cache fcc = empty_fcall_info_cache;

        if (zend_fcall_info_init((zval *)element->data, 0, &fci, &fcc, NULL,
                                 NULL) != SUCCESS) {
            php_error_docref(NULL, E_WARNING,
                             "Failed to initialize post hook callable");
            continue;
        }

        zval ret;
        ZVAL_UNDEF(&ret);
        fci.param_count = param_count;
        fci.params = params;
        fci.retval = &ret;
#if PHP_VERSION_ID >= 80000
        fci.named_params = NULL;
#else
        fci.no_separation = 1;
#endif

        otel_exception_state save_state;
        exception_isolation_start(&save_state);

        if (zend_call_function(&fci, &fcc) == SUCCESS) {
            /*
             * If the post hook has a return type hint that is not void,
             * use the return value to replace the original return value.
             */
            if (!Z_ISUNDEF(ret) &&
                (fcc.function_handler->common.fn_flags &
                 ZEND_ACC_HAS_RETURN_TYPE) &&
                ZEND_TYPE_CODE(
                    fcc.function_handler->common.arg_info[-1].type) !=
                    IS_VOID) {
                if (retval && !Z_ISUNDEF_P(retval)) {
                    zval_ptr_dtor(retval);
                    ZVAL_COPY(retval, &ret);
                    /* Update params[2] too */
                    zval_ptr_dtor(&params[2]);
                    ZVAL_COPY(&params[2], &ret);
                }
            }
        }

        {
            zend_object *suppressed = exception_isolation_end(&save_state);
            exception_isolation_handle_exception(suppressed, &params[4],
                                                 &params[5], "post hook");
        }

        zval_dtor(&ret);
    }

    {
        uint32_t i;
        for (i = 0; i < param_count; i++) {
            zval_dtor(&params[i]);
        }
    }
}

static void otel_execute_ex(zend_execute_data *execute_data) {
    otel_observer *observer = resolve_observer(execute_data);

    if (!observer) {
        original_zend_execute_ex(execute_data);
        return;
    }

    observer_begin(execute_data, &observer->pre_hooks);

    original_zend_execute_ex(execute_data);

    observer_end(execute_data, execute_data->return_value,
                 &observer->post_hooks);

    zend_llist_destroy(&observer->pre_hooks);
    zend_llist_destroy(&observer->post_hooks);
    efree(observer);
}

static void otel_execute_internal(zend_execute_data *execute_data,
                                  zval *return_value) {
    otel_observer *observer = resolve_observer(execute_data);

    if (!observer) {
        if (original_zend_execute_internal) {
            original_zend_execute_internal(execute_data, return_value);
        } else {
            execute_internal(execute_data, return_value);
        }
        return;
    }

    observer_begin(execute_data, &observer->pre_hooks);

    if (original_zend_execute_internal) {
        original_zend_execute_internal(execute_data, return_value);
    } else {
        execute_internal(execute_data, return_value);
    }

    observer_end(execute_data, return_value, &observer->post_hooks);

    zend_llist_destroy(&observer->pre_hooks);
    zend_llist_destroy(&observer->post_hooks);
    efree(observer);
}

static void destroy_observer(otel_observer *observer) {
    zend_llist_destroy(&observer->pre_hooks);
    zend_llist_destroy(&observer->post_hooks);
    efree(observer);
}

static void destroy_observer_lookup(zval *zv) {
    destroy_observer(Z_PTR_P(zv));
}

static void destroy_observer_class_lookup(zval *zv) {
    HashTable *table = Z_PTR_P(zv);
    zend_hash_destroy(table);
    FREE_HASHTABLE(table);
}

static otel_observer *create_observer(void) {
    otel_observer *observer = emalloc(sizeof(otel_observer));
    zend_llist_init(&observer->pre_hooks, sizeof(zval),
                    (llist_dtor_func_t)zval_ptr_dtor, 0);
    zend_llist_init(&observer->post_hooks, sizeof(zval),
                    (llist_dtor_func_t)zval_ptr_dtor, 0);
    return observer;
}

static void add_function_observer(HashTable *ht, zend_string *fn,
                                  zval *pre_hook, zval *post_hook) {
    zend_string *normalized_fn;
    zend_string *lc;
    otel_observer *observer;

    if (ZSTR_LEN(fn) > 0 && ZSTR_VAL(fn)[0] == '\\') {
        normalized_fn =
            zend_string_init(ZSTR_VAL(fn) + 1, ZSTR_LEN(fn) - 1, 0);
    } else {
        normalized_fn = zend_string_copy(fn);
    }
    lc = zend_string_tolower(normalized_fn);
    zend_string_release(normalized_fn);

    observer = zend_hash_find_ptr(ht, lc);
    if (!observer) {
        observer = create_observer();
        zend_hash_update_ptr(ht, lc, observer);
    }
    zend_string_release(lc);

    if (pre_hook) {
        zval_add_ref(pre_hook);
        zend_llist_add_element(&observer->pre_hooks, pre_hook);
    }
    if (post_hook) {
        zval_add_ref(post_hook);
        zend_llist_add_element(&observer->post_hooks, post_hook);
    }
}

static void add_method_observer(HashTable *ht, zend_string *cn,
                                zend_string *fn, zval *pre_hook,
                                zval *post_hook) {
    zend_string *lc = zend_string_tolower(fn);
    HashTable *function_table = zend_hash_find_ptr(ht, lc);

    if (!function_table) {
        ALLOC_HASHTABLE(function_table);
        zend_hash_init(function_table, 8, NULL, destroy_observer_lookup, 0);
        zend_hash_update_ptr(ht, lc, function_table);
    }
    zend_string_release(lc);

    add_function_observer(function_table, cn, pre_hook, post_hook);
}

zend_bool add_observer(zend_string *cn, zend_string *fn, zval *pre_hook,
                       zval *post_hook) {
    if (OTELPHP74_G(observer_class_lookup) == NULL) {
        return 0;
    }

    if (cn) {
        add_method_observer(OTELPHP74_G(observer_class_lookup), cn, fn, pre_hook,
                            post_hook);
    } else {
        add_function_observer(OTELPHP74_G(observer_function_lookup), fn, pre_hook,
                              post_hook);
    }

    return 1;
}

void observer_globals_init(void) {
    if (!OTELPHP74_G(observer_class_lookup)) {
        ALLOC_HASHTABLE(OTELPHP74_G(observer_class_lookup));
        zend_hash_init(OTELPHP74_G(observer_class_lookup), 8, NULL,
                       destroy_observer_class_lookup, 0);
    }
    if (!OTELPHP74_G(observer_function_lookup)) {
        ALLOC_HASHTABLE(OTELPHP74_G(observer_function_lookup));
        zend_hash_init(OTELPHP74_G(observer_function_lookup), 8, NULL,
                       destroy_observer_lookup, 0);
    }
}

void observer_globals_cleanup(void) {
    if (OTELPHP74_G(observer_class_lookup)) {
        zend_hash_destroy(OTELPHP74_G(observer_class_lookup));
        FREE_HASHTABLE(OTELPHP74_G(observer_class_lookup));
        OTELPHP74_G(observer_class_lookup) = NULL;
    }
    if (OTELPHP74_G(observer_function_lookup)) {
        zend_hash_destroy(OTELPHP74_G(observer_function_lookup));
        FREE_HASHTABLE(OTELPHP74_G(observer_function_lookup));
        OTELPHP74_G(observer_function_lookup) = NULL;
    }
}

void opentelemetry_observer_init(INIT_FUNC_ARGS) {
    original_zend_execute_ex = zend_execute_ex;
    zend_execute_ex = otel_execute_ex;

    original_zend_execute_internal = zend_execute_internal;
    zend_execute_internal = otel_execute_internal;
}

void opentelemetry_observer_shutdown(SHUTDOWN_FUNC_ARGS) {
    zend_execute_ex = original_zend_execute_ex;
    zend_execute_internal = original_zend_execute_internal;
}
