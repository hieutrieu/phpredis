/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2007 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | @author    "Alfonso Jimenez" <yo@alfonsojimenez.com>                 |
  | @copyright 2009 Alfonso Jimenez                                      |
  | @license   http://www.gnu.org/licenses/gpl.html GPL                  |
  +----------------------------------------------------------------------+
*/

/* $Id: header,v 1.16.2.1.2.1 2007/01/01 19:32:09 iliaa Exp $ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_redis.h"

static int le_redis;

/**
 * php_stream
 */
static php_stream *s = NULL;

static zend_class_entry *redis_class_entry_ptr;
static void destroy_Redis_resource(zend_rsrc_list_entry *resource TSRMLS_DC);

zend_function_entry redis_functions[] = {
     PHP_ME(Redis, __construct, NULL, 0)
     PHP_ME(Redis, connect, NULL, 0)
     PHP_ME(Redis, close, NULL, 0)
     PHP_ME(Redis, ping, NULL, 0)
     PHP_ME(Redis, get, NULL, 0)
     PHP_ME(Redis, set, NULL, 0)
     PHP_ME(Redis, add, NULL, 0)
     PHP_ME(Redis, exists, NULL, 0)
     PHP_ME(Redis, delete, NULL, 0)
     PHP_ME(Redis, incr, NULL, 0)
     PHP_ME(Redis, decr, NULL, 0)
     PHP_ME(Redis, type, NULL, 0)
     PHP_ME(Redis, getKeys, NULL, 0)
     PHP_ME(Redis, listPush, NULL, 0)
     PHP_ME(Redis, listPop, NULL, 0)
     PHP_ME(Redis, listSize, NULL, 0)
     PHP_ME(Redis, listTrim, NULL, 0)
     PHP_ME(Redis, listGet, NULL, 0)
     PHP_ME(Redis, listGetRange, NULL, 0)
     PHP_ME(Redis, setAdd, NULL, 0)
     PHP_ME(Redis, setSize, NULL, 0)
     PHP_ME(Redis, setRemove, NULL, 0)
     PHP_ME(Redis, setContains, NULL, 0)
     PHP_ME(Redis, setGetMembers, NULL, 0)
     {NULL, NULL, NULL}
};

zend_module_entry redis_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
     STANDARD_MODULE_HEADER,
#endif
     "redis",
     redis_functions,
     PHP_MINIT(redis),
     PHP_MSHUTDOWN(redis),
     PHP_RINIT(redis),
     PHP_RSHUTDOWN(redis),
     PHP_MINFO(redis),
#if ZEND_MODULE_API_NO >= 20010901
     "0.1",
#endif
     STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_REDIS
ZEND_GET_MODULE(redis)
#endif

/**
 * PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(redis)
{
    zend_class_entry redis_class_entry;
    INIT_CLASS_ENTRY(redis_class_entry, "Redis", redis_functions);
    redis_class_entry_ptr =
                         zend_register_internal_class(&redis_class_entry TSRMLS_CC);
    le_redis = zend_register_list_destructors_ex(
    destroy_Redis_resource, NULL, "Redis", module_number );
    return SUCCESS;
}

/**
 * PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(redis)
{
    if (s) {
        php_stream_close(s);
    }

    return SUCCESS;
}

/**
 * PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(redis)
{
    return SUCCESS;
}

/**
 * PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(redis)
{
    if (s) {
        php_stream_close(s);
    }

    return SUCCESS;
}

/**
 * PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(redis)
{
     php_info_print_table_start();
     php_info_print_table_header(2, "redis support", "enabled");
     php_info_print_table_end();
}

/**
 * Public constructor
 */
PHP_METHOD(Redis, __construct)
{
   int ret = zend_list_insert(NULL, le_redis);
   object_init_ex(getThis(), redis_class_entry_ptr);
}

/**
 * Method: connect
 */
PHP_METHOD(Redis, connect)
{
    struct timeval tv;

    char *host;
    char *hostname = NULL, *hash_key = NULL, *errstr = NULL;
    int host_len, hostname_len, port, err = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sl", &host, &host_len,
                              &port) == FAILURE) {
        RETURN_NULL();
    }

    hostname     = emalloc(strlen(host) + MAX_LENGTH_OF_LONG + 2);
    hostname_len = sprintf(hostname, "%s:%d", host, port);

    tv.tv_sec  = 5;
    tv.tv_usec = 0;

    s = php_stream_xport_create(hostname,
                                hostname_len,
                                ENFORCE_SAFE_MODE | REPORT_ERRORS,
                                STREAM_XPORT_CLIENT | STREAM_XPORT_CONNECT,
                                hash_key,
                                &tv,
                                NULL,
                                &errstr,
                                &err);
    if (!s) {
        php_error_docref(NULL TSRMLS_CC, E_ERROR, "%s", errstr);
        RETURN_FALSE;
    } else {
        RETURN_TRUE;
    }
}

/**
 * Method: close
 */
PHP_METHOD(Redis, close)
{
    if (s) {
        php_stream_close(s);
    }
}

/**
 * Method: set
 */
PHP_METHOD(Redis, set)
{
    if (s) {
        char *key, *val;
        int key_len, val_len;

        if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &key, &key_len,
                                  &val, &val_len) == FAILURE) {
            RETURN_NULL();
        }

        char *cmd;
        int cmd_len;
        char buf[1024];

        cmd_len = spprintf(&cmd, 0, "SET %s %d\r\n%s\r\n", key, strlen(val), val);

        php_stream_write(s, cmd, strlen(cmd));

        php_stream_gets(s, buf, sizeof(buf));

        if (buf[0] == 0x2b) {
            RETURN_TRUE;
        } else {
            RETURN_FALSE;
        }

    } else {
        php_error_docref(NULL TSRMLS_CC, E_ERROR, "The object is not connected");
        RETURN_FALSE;
    }
}

/**
 * Method: get
 */
PHP_METHOD(Redis, get)
{
    if (s) {
        char *name;
        int name_len;

        if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &name_len)
                                  == FAILURE) {
            RETURN_NULL();
        }

        char *cmd;
        int cmd_len = 0;

        cmd_len = spprintf(&cmd, 0, "GET %s\r\n", name);

        php_stream_write(s, cmd, strlen(cmd));

        char buf[2048];

        php_stream_gets(s, buf, sizeof(buf));

        if (buf[0] == 0x2d || buf[0] == 0x6e) {
            RETURN_NULL();
        }

        int res_len = atoi(buf);
        int buf_len = 0;

        while (res_len > 0) {
            php_stream_gets(s, buf, sizeof(buf));
            buf_len = strlen(buf);
            res_len = res_len - buf_len;
        }

        zval *value;
        zval trim_zv;
        MAKE_STD_ZVAL(value);

        ZVAL_STRING(value, buf, 1);
        trim_zv = *value;
        php_trim(Z_STRVAL(trim_zv), Z_STRLEN(trim_zv), NULL, 0, &trim_zv,
                 3 TSRMLS_CC);

        RETURN_STRING(Z_STRVAL(trim_zv), 1);
    } else {
        php_error_docref(NULL TSRMLS_CC, E_ERROR, "The object is not connected");
        RETURN_FALSE;
    }
}

/**
 * Method: add
 */
PHP_METHOD(Redis, add)
{
    if (s) {
        char *key, *val;
        int key_len, val_len;

        if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &key, &key_len,
                                  &val, &val_len) == FAILURE) {
            RETURN_NULL();
        }

        char *cmd;
        int cmd_len;
        char buf[1024];

        cmd_len = spprintf(&cmd, 0, "SETNX %s %d\r\n%s\r\n", key, strlen(val), val);

        php_stream_write(s, cmd, strlen(cmd));

        php_stream_gets(s, buf, sizeof(buf));

        if (atoi(buf) == 1) {
            RETURN_TRUE;
        } else {
            RETURN_FALSE;
        }
    } else {
        php_error_docref(NULL TSRMLS_CC, E_ERROR, "The object is not connected");
        RETURN_FALSE;
    }
}

/**
 * Method: ping
 */
PHP_METHOD(Redis, ping)
{
    if (s) {
       char cmd[] = "PING\r\n";
       char buf[8];

       php_stream_write(s, cmd, strlen(cmd));
            
       php_stream_gets(s, buf, sizeof(char) * 6);

       if (buf[0] == 0x2b) {
          RETURN_STRING(buf, 1);
       } else {
          RETURN_FALSE;
       }
    } else {
        php_error_docref(NULL TSRMLS_CC, E_ERROR, "The object is not connected");
        RETURN_FALSE;
    }
}

/**
 * Method: incr
 */
PHP_METHOD(Redis, incr)
{
    if (s) {
        char *key;
        int key_len, value = 0;

        if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|l", &key, &key_len,
            &value) == FAILURE) {
            RETURN_NULL();
        }

        char *cmd;
        char buf[256];
        int cmd_len;

        if (value <= 1) {
            cmd_len = spprintf(&cmd, 0, "INCR %s\r\n", key);
        } else {
            cmd_len = spprintf(&cmd, 0, "INCRBY %s %d\r\n", key, value);
        }

        php_stream_write(s, cmd, strlen(cmd));
            
        php_stream_gets(s, buf, sizeof(buf));

        if (buf[0] == '1') {
           RETURN_TRUE;
        } else {
           RETURN_FALSE;
        }
    } else {
        php_error_docref(NULL TSRMLS_CC, E_ERROR, "The object is not connected");
        RETURN_FALSE;
    }
}

/**
 * Method: decr
 */
PHP_METHOD(Redis, decr)
{
    if (s) {
        char *key;
        int key_len, value = 0;

        if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|l", &key, &key_len,
                                  &value) == FAILURE) {
            RETURN_NULL();
        }

        char *cmd;
        int cmd_len;
        char buf[256];

        if (value <= 1) {
            cmd_len = spprintf(&cmd, 0, "DECR %s\r\n", key);
        } else {
            cmd_len = spprintf(&cmd, 0, "DECRBY %s %d\r\n", key, value);
        }

        php_stream_write(s, cmd, strlen(cmd));
            
        php_stream_gets(s, buf, sizeof(buf));

        if (buf[0] == '1') {
           RETURN_TRUE;
        } else {
           RETURN_FALSE;
        }
    } else {
        php_error_docref(NULL TSRMLS_CC, E_ERROR, "The object is not connected");
        RETURN_FALSE;
    }
}

/**
 * Method: exists
 */
PHP_METHOD(Redis, exists)
{
    if (s) {
        char *key;
        int key_len;

        if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &key, &key_len)
                                                                       == FAILURE) {
            RETURN_NULL();
        }

        char *cmd;
        int cmd_len, res;
        char *buf;

        cmd_len = spprintf(&cmd, 0, "EXISTS %s\r\n", key);

        php_stream_write(s, cmd, strlen(cmd));

        buf = (char *) emalloc(8);
            
        php_stream_gets(s, buf, 8);

        if (buf[0] == '1') {
           RETURN_TRUE;
        } else {
           RETURN_FALSE;
        }
    } else {
        php_error_docref(NULL TSRMLS_CC, E_ERROR, "The object is not connected");
        RETURN_FALSE;
    }
}

/**
 * Method: delete
 */
PHP_METHOD(Redis, delete)
{
    if (s) {
        char *key;
        int key_len;

        if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &key, &key_len)
                                                                       == FAILURE) {
            RETURN_NULL();
        }

        char *cmd;
        int cmd_len;
        char buf[8];

        cmd_len = spprintf(&cmd, 0, "DEL %s\r\n", key);

        php_stream_write(s, cmd, strlen(cmd));
            
        php_stream_gets(s, buf, sizeof(buf));

        if (buf[0] == '1') {
           RETURN_TRUE;
        } else {
           RETURN_FALSE;
        }
    } else {
        php_error_docref(NULL TSRMLS_CC, E_ERROR, "The object is not connected");
        RETURN_FALSE;
    }
}

/**
 * Method: getKeys
 */
PHP_METHOD(Redis, getKeys)
{
    if (s) {
        char *pattern;
        int pattern_len;

        if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &pattern,
                                  &pattern_len) == FAILURE) {
            RETURN_NULL();
        }

        char *cmd;
        int cmd_len = 0;

        cmd_len = spprintf(&cmd, 0, "KEYS %s\r\n", pattern);

        php_stream_write(s, cmd, strlen(cmd));

        char buf[4096];

        php_stream_gets(s, buf, sizeof(buf));

        if (buf[0] == 0x2d || buf[0] == 0x6e) {
            RETURN_NULL();
        }

        int res_len = atoi(buf);
        int buf_len = 0;

        while (res_len > 0) {
            php_stream_gets(s, buf, sizeof(buf));
            buf_len = strlen(buf);
            res_len = res_len - buf_len;
        }

        array_init(return_value);

        zval *delimiter;
        MAKE_STD_ZVAL(delimiter);
        ZVAL_STRING(delimiter, " ", 1);

        zval *keys;
        MAKE_STD_ZVAL(keys);
        ZVAL_STRING(keys, buf, 1);
        php_explode(delimiter, keys, return_value, -1);
    } else {
        php_error_docref(NULL TSRMLS_CC, E_ERROR, "The object is not connected");
        RETURN_FALSE;
    }
}

/**
 * Method: type
 */
PHP_METHOD(Redis, type)
{

    if (s) {
        char *key;
        int key_len;

        if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &key, &key_len)
                                  == FAILURE) {
            RETURN_NULL();
        }

        char *cmd;
        int cmd_len, res;
        char *buf;

        cmd_len = spprintf(&cmd, 0, "TYPE %s\r\n", key);

        php_stream_write(s, cmd, strlen(cmd));

        buf = (char *) emalloc(sizeof(char) * 7);
            
        php_stream_gets(s, buf, sizeof(buf));

        if (strcmp(buf, "str") == 0) {
           RETURN_LONG(1);
        } else if (strcmp(buf, "set") == 0){
           RETURN_LONG(2);
        } else if (strcmp(buf, "lis") == 0){
           RETURN_LONG(3);
        } else {
           RETURN_LONG(0);
        }
    } else {
        php_error_docref(NULL TSRMLS_CC, E_ERROR, "The object is not connected");
        RETURN_FALSE;
    }
}

/**
 * Method: listPush
 */
PHP_METHOD(Redis, listPush)
{
    if (s) {
        char *key, *val;
        int key_len, val_len, type = 0;

        if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|l", &key, &key_len,
                                  &val, &val_len, &type) == FAILURE) {
            RETURN_NULL();
        }

        char *cmd;
        int cmd_len, res;
        char *buf;

        if (type == 0) {
            cmd_len = spprintf(&cmd, 0, "RPUSH %s %d\r\n%s\r\n",
                                         key,
                                         strlen(val),
                                         val);
        } else if (type == 1) {
            cmd_len = spprintf(&cmd, 0, "LPUSH %s %d\r\n%s\r\n",
                                         key,
                                         strlen(val),
                                         val);
        } else {
            RETURN_FALSE;
        }

        php_stream_write(s, cmd, strlen(cmd));

        buf = (char *) emalloc(sizeof(char) * 8);
            
        php_stream_gets(s, buf, sizeof(buf));

        if (buf[0] == 0x2b) {
           RETURN_TRUE;
        } else {
           RETURN_FALSE;
        }
    } else {
        php_error_docref(NULL TSRMLS_CC, E_ERROR, "The object is not connected");
        RETURN_FALSE;
    }
}

/**
 * Method: listPop
 */
PHP_METHOD(Redis, listPop)
{
    if (s) {
        char *key;
        int key_len, type = 0;

        if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|l", &key, &key_len,
                                  &type) == FAILURE) {
            RETURN_NULL();
        }

        char *cmd;
        int cmd_len;
        char *buf;

        if (type == 0) {
            cmd_len = spprintf(&cmd, 0, "RPOP %s\r\n", key);
        } else if (type == 1) {
            cmd_len = spprintf(&cmd, 0, "LPOP %s\r\n", key);
        } else {
            RETURN_FALSE;
        }

        php_stream_write(s, cmd, strlen(cmd));

        buf = (char *) emalloc(sizeof(char) * 8);
            
        php_stream_gets(s, buf, sizeof(buf));

        if (buf[0] == 0x2d || buf[0] == 0x6e) {
            RETURN_NULL();
        }

        int res_len = atoi(buf) + 1;
        int buf_len = 0;

        while (res_len > 0) {
            php_stream_gets(s, buf, sizeof(char) * res_len);
            buf_len = strlen(buf);
            res_len = res_len - buf_len;
        }

        RETURN_STRING(buf, 1);
    } else {
        php_error_docref(NULL TSRMLS_CC, E_ERROR, "The object is not connected");
        RETURN_FALSE;
    }
}

/**
 * Method: listSize
 */
PHP_METHOD(Redis, listSize)
{
    if (s) {
        char *key;
        int key_len;

        if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &key, &key_len)
                                                                       == FAILURE) {
            RETURN_NULL();
        }

        char *cmd;
        int cmd_len;
        char *buf;

        cmd_len = spprintf(&cmd, 0, "LLEN %s\r\n", key);

        php_stream_write(s, cmd, strlen(cmd));

        buf = (char *) emalloc(sizeof(char) * 16);
            
        php_stream_gets(s, buf, sizeof(buf));

        if (buf[0] == 0x2d) {
            php_error_docref(NULL TSRMLS_CC, E_ERROR, "'%s' is not a list", key);
            RETURN_FALSE;
        }

        int res = atoi(buf);

        RETURN_LONG(res);
    } else {
        php_error_docref(NULL TSRMLS_CC, E_ERROR, "The object is not connected");
        RETURN_FALSE;
    }
}

/**
 * Method: listTrim
 */
PHP_METHOD(Redis, listTrim)
{
    if (s) {
        char *key;
        int key_len, start, end;

        if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sll", &key, &key_len,
                                  &start, &end) == FAILURE) {
            RETURN_NULL();
        }

        char *cmd;
        int cmd_len;
        char *buf;

        cmd_len = spprintf(&cmd, 0, "LTRIM %s %d %d\r\n", key, start, end);

        php_stream_write(s, cmd, strlen(cmd));

        buf = (char *) emalloc(sizeof(char) * 5);
            
        php_stream_gets(s, buf, sizeof(buf));

        if (buf[0] == 0x2db) {
            RETURN_TRUE;
        } else {
            RETURN_FALSE;
        }
    } else {
        php_error_docref(NULL TSRMLS_CC, E_ERROR, "The object is not connected");
        RETURN_FALSE;
    }
}

/**
 * Method: listGet
 */
PHP_METHOD(Redis, listGet)
{
    if (s) {
        char *key;
        int key_len, index;

        if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sl", &key, &key_len, &index)
                                                                       == FAILURE) {
            RETURN_NULL();
        }

        char *cmd;
        int cmd_len;
        char *buf;

        cmd_len = spprintf(&cmd, 0, "LINDEX %s %d\r\n", key, index);

        php_stream_write(s, cmd, strlen(cmd));

        buf = (char *) emalloc(sizeof(char) * 8);
            
        php_stream_gets(s, buf, sizeof(buf));

        if (buf[0] == 0x2d || buf[0] == 0x6e) {
            RETURN_NULL();
        }

        int res_len = atoi(buf) + 1;
        int buf_len = 0;

        while (res_len > 0) {
            php_stream_gets(s, buf, sizeof(char) * res_len);
            buf_len = strlen(buf);
            res_len = res_len - buf_len;
        }

        RETURN_STRING(buf, 1);
    } else {
        php_error_docref(NULL TSRMLS_CC, E_ERROR, "The object is not connected");
        RETURN_FALSE;
    }
}

/**
 * Method: listGetRange
 */
PHP_METHOD(Redis, listGetRange)
{
    if (s) {
        char *key;
        int key_len, start, end;

        if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sll", &key, &key_len,
                                  &start, &end) == FAILURE) {
            RETURN_NULL();
        }

        char *cmd, *buf;
        int cmd_len;

        cmd_len = spprintf(&cmd, 0, "LRANGE %s %d %d\r\n", key, start, end);

        php_stream_write(s, cmd, strlen(cmd));

        buf = (char *) emalloc(sizeof(char) * 8);
            
        php_stream_gets(s, buf, sizeof(buf));

        if (buf[0] == 0x2d || buf[0] == 0x6e) {
            RETURN_NULL();
        }

        array_init(return_value);

        int numItems = atoi(buf);

        while (numItems > 0) {
            php_stream_gets(s, buf, sizeof(char) * 8);
            php_stream_gets(s, buf, sizeof(char) * 256);
            add_next_index_string(return_value, buf, 1);
            numItems--;
        }
    } else {
        php_error_docref(NULL TSRMLS_CC, E_ERROR, "The object is not connected");
        RETURN_FALSE;
    }
}

/**
 * Method: setAdd
 */
PHP_METHOD(Redis, setAdd)
{
    if (s) {
        char *key, *val;
        int key_len, val_len, count = 1;

        if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &key, &key_len,
                                  &val, &val_len) == FAILURE) {
            RETURN_NULL();
        }

        char *cmd;
        int cmd_len;
        char *buf;

        cmd_len = spprintf(&cmd, 0, "SADD %s %d\r\n%s\r\n", key, strlen(val), val);

        php_stream_write(s, cmd, strlen(cmd));

        buf = (char *) emalloc(sizeof(char) * 2);
            
        php_stream_gets(s, buf, sizeof(buf));

        int num = atoi(buf);

        if (num == 1) {
            RETURN_TRUE;
        } else if (num == 0) {
            php_error_docref(NULL TSRMLS_CC, E_WARNING,
                             "The new element was already a member of the set");
            RETURN_FALSE;   
        } else {
            RETURN_FALSE;
        }
    } else {
        php_error_docref(NULL TSRMLS_CC, E_ERROR, "The object is not connected");
        RETURN_FALSE;
    }
}

/**
 * Method: setSize
 */
PHP_METHOD(Redis, setSize)
{
    if (s) {
        char *key;
        int key_len;

        if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &key, &key_len)
                                                                       == FAILURE) {
            RETURN_NULL();
        }

        char *cmd;
        int cmd_len;
        char *buf;

        cmd_len = spprintf(&cmd, 0, "SCARD %s\r\n", key);

        php_stream_write(s, cmd, strlen(cmd));

        buf = (char *) emalloc(sizeof(char) * 16);
            
        php_stream_gets(s, buf, sizeof(buf));

        if (buf[0] == 0x2d) {
            php_error_docref(NULL TSRMLS_CC, E_ERROR,
                             "'%s' does not hold a set value", key);
            RETURN_FALSE;
        }

        int res = atoi(buf);

        RETURN_LONG(res);
    } else {
        php_error_docref(NULL TSRMLS_CC, E_ERROR, "The object is not connected");
        RETURN_FALSE;
    }
}

/**
 * Method: setRemove
 */
PHP_METHOD(Redis, setRemove)
{
    if (s) {
        char *key, *val;
        int key_len, val_len;

        if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &key, &key_len,
                                  &val, &val_len) == FAILURE) {
            RETURN_NULL();
        }

        char *cmd;
        int cmd_len;
        char *buf;

        cmd_len = spprintf(&cmd, 0, "SREM %s %d\r\n%s\r\n", key, strlen(val), val);

        php_stream_write(s, cmd, strlen(cmd));

        buf = (char *) emalloc(sizeof(char) * 2);
            
        php_stream_gets(s, buf, sizeof(buf));

        if (buf[0] == 0x2d) {
            php_error_docref(NULL TSRMLS_CC, E_ERROR,
                             "'%s' does not hold a set value", key);
            RETURN_FALSE;
        }

        int res = atoi(buf);

        RETURN_LONG(res);
    } else {
        php_error_docref(NULL TSRMLS_CC, E_ERROR, "The object is not connected");
        RETURN_FALSE;
    }
}

/**
 * Method: setContains
 */
PHP_METHOD(Redis, setContains)
{
    if (s) {
        char *key, *val;
        int key_len, val_len;

        if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &key, &key_len,
                                  &val, &val_len) == FAILURE) {
            RETURN_NULL();
        }

        char *cmd;
        int cmd_len;
        char *buf;

        cmd_len = spprintf(&cmd, 0, "SISMEMBER %s %d\r\n%s\r\n",
                           key, strlen(val), val);

        php_stream_write(s, cmd, strlen(cmd));

        buf = (char *) emalloc(sizeof(char) * 2);
            
        php_stream_gets(s, buf, sizeof(buf));

        if (buf[0] == 0x2d) {
            php_error_docref(NULL TSRMLS_CC, E_ERROR,
                             "'%s' does not hold a set value", key);
            RETURN_FALSE;
        }

        int res = atoi(buf);

        if (res == 1) {
            RETURN_TRUE;
        } else {
            RETURN_FALSE;
        }
    } else {
        php_error_docref(NULL TSRMLS_CC, E_ERROR, "The object is not connected");
        RETURN_FALSE;
    }
}

/**
 * Method: setGetMembers
 */
PHP_METHOD(Redis, setGetMembers)
{
    if (s) {
        char *key;
        int key_len;

        if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &key, &key_len)
                                                                       == FAILURE) {
            RETURN_NULL();
        }

        char *cmd;
        int cmd_len;

        cmd_len = spprintf(&cmd, 0, "SMEMBERS %s\r\n", key);

        php_stream_write(s, cmd, strlen(cmd));

        char buf[4096];

        php_stream_gets(s, buf, sizeof(buf));

        if (buf[0] == 0x2d || buf[0] == 0x6e) {
            RETURN_NULL();
        }

        array_init(return_value);

        int numItems = atoi(buf);
        zval *member;
        zval trim_zv;
        MAKE_STD_ZVAL(member);

        while (numItems > 0) {
            php_stream_gets(s, buf, 32);
            php_stream_gets(s, buf, 256);
            ZVAL_STRING(member, buf, 1);
            trim_zv = *member;
            php_trim(Z_STRVAL(trim_zv), Z_STRLEN(trim_zv), NULL, 0, &trim_zv, 
                     3 TSRMLS_CC);
            add_next_index_string(return_value, Z_STRVAL(trim_zv), 1);
            numItems--;
        }
    } else {
        php_error_docref(NULL TSRMLS_CC, E_ERROR, "The object is not connected");
        RETURN_FALSE;
    }
}

static void destroy_Redis_resource(zend_rsrc_list_entry *resource TSRMLS_DC)
{
    if (s) {
        php_stream_close(s);
    }
}