/*
  +----------------------------------------------------------------------+
  | ValkeyGlide Script Class                                             |
  +----------------------------------------------------------------------+
  | Copyright (c) 2023-2025 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  +----------------------------------------------------------------------+
*/

#ifndef VALKEY_GLIDE_SCRIPT_H
#define VALKEY_GLIDE_SCRIPT_H

#include "common.h"
#include "php.h"

/* Script object structure */
typedef struct {
    char*       hash;     /* SHA1 hash of the stored script */
    size_t      hash_len; /* Length of the hash string */
    zend_object std;      /* Standard PHP object - MUST be last */
} valkey_glide_script_object;

/* Class entry and handlers */
extern zend_class_entry*    valkey_glide_script_ce;
extern zend_object_handlers valkey_glide_script_object_handlers;

/* Object creation and destruction */
zend_object* create_valkey_glide_script_object(zend_class_entry* ce);
void         free_valkey_glide_script_object(zend_object* object);

/* Class methods */
PHP_METHOD(Script, __construct);
PHP_METHOD(Script, __destruct);
PHP_METHOD(Script, getHash);

/* Helper macros */
#define VALKEY_GLIDE_SCRIPT_GET_OBJECT(obj) \
    VALKEY_GLIDE_PHP_GET_OBJECT(valkey_glide_script_object, obj)
#define VALKEY_GLIDE_SCRIPT_ZVAL_GET_OBJECT(zv) \
    VALKEY_GLIDE_PHP_ZVAL_GET_OBJECT(valkey_glide_script_object, zv)

/* Class registration function */
void register_valkey_glide_script_class(void);

/* Getter function for class entry */
zend_class_entry* get_valkey_glide_script_ce(void);

#endif /* VALKEY_GLIDE_SCRIPT_H */
