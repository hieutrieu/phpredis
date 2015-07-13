Reference Methods

## Connection handling ##
  * Redis construct()
  * boolean connect(string host, int port)
  * string ping()
  * void close()

## Commands operating on string values ##
  * boolean set(string key, string value)
  * string get(string key)
  * boolean add(string key, string value)
  * boolean incr(string key, int value = 1)
  * boolean decr(string key, int value = 1)
  * array getMultiple(array(key1, key2, ..., keyn)) **new!**

## Commands operating on the key space ##
  * boolean exists(string key)
  * int type(string key)
  * array getKeys(string pattern)
  * boolean delete(string key)

## Commands operating on list objects ##
  * boolean lPush(string key, string value, int type = 0)
  * string lPop(string key, int type = 0)
  * int lSize(string key)
  * boolean listTrim(string key, int start, int end)
  * string lGet(string key, int index)
  * array lGetRange(string key, int start, int end)
  * boolean lRemove(string key, string value, int count = 0) **new!**

## Commands operating on set objects ##
  * boolean sAdd(string key, string member)
  * int sSize(string key)
  * mixed sRemove(string key, string member)
  * boolean sContains(string key, string member)
  * array sGetMembers(string key)

## Sorting ##
  * `array getSort(string key [,order = 0, pattern = "*", start=0, end = 0])` **new!**