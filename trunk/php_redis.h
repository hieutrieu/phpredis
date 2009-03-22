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

#ifndef PHP_REDIS_H
#define PHP_REDIS_H

extern zend_module_entry redis_module_entry;
#define phpext_redis_ptr &redis_module_entry

PHP_METHOD(Redis, __construct);
PHP_METHOD(Redis, connect);
PHP_METHOD(Redis, close);
PHP_METHOD(Redis, ping);
PHP_METHOD(Redis, get);
PHP_METHOD(Redis, set);
PHP_METHOD(Redis, add);
PHP_METHOD(Redis, exists);
PHP_METHOD(Redis, delete);
PHP_METHOD(Redis, incr);
PHP_METHOD(Redis, decr);
PHP_METHOD(Redis, type);
PHP_METHOD(Redis, getKeys);
PHP_METHOD(Redis, listPush);
PHP_METHOD(Redis, listPop);
PHP_METHOD(Redis, listSize);
PHP_METHOD(Redis, listTrim);
PHP_METHOD(Redis, listGet);
PHP_METHOD(Redis, listGetRange);
PHP_METHOD(Redis, setAdd);
PHP_METHOD(Redis, setSize);
PHP_METHOD(Redis, setRemove);
PHP_METHOD(Redis, setContains);
PHP_METHOD(Redis, setGetMembers);
#ifdef PHP_WIN32
#define PHP_REDIS_API __declspec(dllexport)
#else
#define PHP_REDIS_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

PHP_MINIT_FUNCTION(redis);
PHP_MSHUTDOWN_FUNCTION(redis);
PHP_RINIT_FUNCTION(redis);
PHP_RSHUTDOWN_FUNCTION(redis);
PHP_MINFO_FUNCTION(redis);

#ifdef ZTS
#define REDIS_G(v) TSRMG(redis_globals_id, zend_redis_globals *, v)
#else
#define REDIS_G(v) (redis_globals.v)
#endif

#define PHP_REDIS_VERSION "0.0.1"

#endif	/* PHP_SPHINX_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */