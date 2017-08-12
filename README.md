# btree_properties
btree_properties is a simple library for dealing with properties and property
files. It is based on the `<search.h>` library and can be used as an example
for the use of the functions:

```c
tsearch, tfind, tdelete, twalk, tdestroy
```

## Why btree?
The normal way to deal with properties is to use a hash map, which is very
efficient with lookups. The `<search.h>` library provides the hsearch function
for this purpose. The problem is, that the `int hcreate(size_t nel)` functon,
which is used to create a map is called with a size. If the size is too small, 
the hash map becomes less efficient.

The btree functions are less efficient than the hash map, but do not require a
size.

## Context
To use the library functions an instance of `BTP_ctx` is necessary, 
which has to be created before the start of the work and which has 
to destroyed at its end. The following code snipet shows the usage:

```c
BTP_ctx *ctx = btp_create_ctx();

// do work

btp_destroy_ctx(ctx);
```

## Properties
A properties file can be read with the function `btp_read_properties`:

```c
btp_read_properties(ctx, "foo.properties");
```

Properties can be added directly with the function `btp_add_property`. The non 
obvious parameter is the last, boolean parameter `replace`. If the key exists
already and the parameter is `true`, the value will be replaced. If the parameter
is `false` nothing happens. The function returns `true` if the key / value pair
was added and `false` if it was not added or if the value was replaced.
In the following example `result-1` will be `true`, `result-2` will be `false`,
and the value will be `"value-2"`.

```c
bool result;

result-1 = btp_add_property(ctx, "key", "value-1", false);

result-2 = btp_add_property(ctx, "key", "value-2", true);
```

The function `btp_get_property_value` returns the value for a given key. If
the key does not exist `NULL` is returned. `NULL` is not a valid value for a key or
a value.

```c
char *value = btp_get_property_value(ctx, "key");
```

A key value pair can be removed with the function `btp_delete_property`. It returns
true, if the value is removed.

```c
bool result = btp_delete_property(ctx, "key");
```

## Iterator

The library has a simple callback mechanism to iterate over the key / value
entries.

```c
void print_properties(const char *key, const char *value) {
	printf("Key: '%s' Value: '%s'\n", key, value);
}

btp_iterate_properties(ctx, print_properties);

```

## Memory management
All keys and values are allocated by the libray functions and are freed if the
context is destroyed. If you need a value, you have to copy it. If the value of
a key is replaced, the old value will be freed.


