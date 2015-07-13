# phpredis #

This extension provides an API for communicating with [Redis database](http://code.google.com/p/redis/), a persistent
key-value database with built-in net interface written in ANSI-C for Posix systems. Have a look at the [available methods](http://code.google.com/p/phpredis/wiki/referencemethods).

## EXAMPLE ##

```
$redis = new Redis();
$redis->connect('127.0.0.1', 6379);

$redis->set('key', 'val');

echo $redis->get('key');
```

## DOWNLOAD ##

You can get the lastest version of phpredis directly from the svn repository:

```
svn checkout http://phpredis.googlecode.com/svn/trunk/ phpredis-read-only
```

## INSTALLING ##

```
phpize
./configure
make && make install
```

## ABOUT ##
Alfonso Jimenez http://www.alfonsojimenez.com