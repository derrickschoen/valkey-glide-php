/*
  +----------------------------------------------------------------------+
  | ValkeyGlide Script Implementation                                    |
  +----------------------------------------------------------------------+
  | Copyright (c) 2023-2025 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  +----------------------------------------------------------------------+
*/

#include "valkey_glide_script.h"

#include <zend_exceptions.h>

#include "include/glide_bindings.h"
#include "valkey_glide_commands_common.h"
#include "valkey_glide_script_arginfo.h"

/* Global variables */
zend_class_entry*    valkey_glide_script_ce;
zend_object_handlers valkey_glide_script_object_handlers;

/**
 * Release a stored script hash: call drop_script FFI, free the error
 * string (if any), efree the hash, and NULL out the pointer/length.
 */
static void release_script_hash(char** hash, size_t* hash_len) {
    if (*hash == NULL) {
        return;
    }
    char* error = drop_script((uint8_t*) *hash, *hash_len);
    if (error != NULL) {
        free_drop_script_error(error);
    }
    efree(*hash);
    *hash     = NULL;
    *hash_len = 0;
}

/* Object creation */
zend_object* create_valkey_glide_script_object(zend_class_entry* ce) {
    valkey_glide_script_object* script_obj =
        ecalloc(1, sizeof(valkey_glide_script_object) + zend_object_properties_size(ce));

    zend_object_std_init(&script_obj->std, ce);
    object_properties_init(&script_obj->std, ce);

    script_obj->hash     = NULL;
    script_obj->hash_len = 0;

    memcpy(&valkey_glide_script_object_handlers,
           zend_get_std_object_handlers(),
           sizeof(valkey_glide_script_object_handlers));
    valkey_glide_script_object_handlers.offset    = XtOffsetOf(valkey_glide_script_object, std);
    valkey_glide_script_object_handlers.free_obj  = free_valkey_glide_script_object;
    valkey_glide_script_object_handlers.clone_obj = NULL; /* Prevent clone double-free */
    script_obj->std.handlers                      = &valkey_glide_script_object_handlers;

    return &script_obj->std;
}

/* Object destruction */
void free_valkey_glide_script_object(zend_object* object) {
    valkey_glide_script_object* script_obj = VALKEY_GLIDE_SCRIPT_GET_OBJECT(object);

    /* Release script from core cache. Silently ignores errors --
     * drop_script failures are expected after SCRIPT FLUSH or
     * duplicate-hash lifecycle edge cases. */
    release_script_hash(&script_obj->hash, &script_obj->hash_len);

    zend_object_std_dtor(&script_obj->std);
}

/* Script::__construct(string $code) */
PHP_METHOD(Script, __construct) {
    char*  code     = NULL;
    size_t code_len = 0;

    ZEND_PARSE_PARAMETERS_START(1, 1)
    Z_PARAM_STRING(code, code_len)
    ZEND_PARSE_PARAMETERS_END();

    valkey_glide_script_object* script_obj = VALKEY_GLIDE_SCRIPT_ZVAL_GET_OBJECT(getThis());

    /* Re-entry protection: if constructor is called again on same object,
     * drop the old hash first to prevent a leak. */
    release_script_hash(&script_obj->hash, &script_obj->hash_len);

    char* hash = store_script_and_get_hash(code);
    if (!hash) {
        zend_throw_exception(get_valkey_glide_exception_ce(), "Failed to store script in cache", 0);
        return;
    }

    script_obj->hash     = hash;
    script_obj->hash_len = strlen(hash);
}

/* Script::__destruct() */
PHP_METHOD(Script, __destruct) {
    /* Cleanup is handled in free_valkey_glide_script_object */
}

/* Script::getHash(): string */
PHP_METHOD(Script, getHash) {
    if (zend_parse_parameters_none() == FAILURE) {
        RETURN_THROWS();
    }

    valkey_glide_script_object* script_obj = VALKEY_GLIDE_SCRIPT_ZVAL_GET_OBJECT(getThis());

    if (script_obj->hash == NULL) {
        zend_throw_exception(get_valkey_glide_exception_ce(),
                             "Script hash is not available (constructor may have failed)",
                             0);
        RETURN_THROWS();
    }

    RETURN_STRINGL(script_obj->hash, script_obj->hash_len);
}

/* Class registration function using generated arginfo */
void register_valkey_glide_script_class(void) {
    valkey_glide_script_ce                = register_class_Script();
    valkey_glide_script_ce->create_object = create_valkey_glide_script_object;

    /* Disable serialization to prevent uninitialized internal state */
    valkey_glide_script_ce->ce_flags |= ZEND_ACC_NOT_SERIALIZABLE;
}

/* Getter function for the class entry */
zend_class_entry* get_valkey_glide_script_ce(void) {
    return valkey_glide_script_ce;
}
